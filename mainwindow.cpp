#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <stdio.h>
#include <string>
#include <sstream>
#include <QString>
#include <QFileDialog>
#include <QCameraInfo>
#include <QDateTime>
#include <QColor>
#include <QColorDialog>

using namespace std;
using namespace cv;

VideoCapture capture;
VideoWriter saveVideo;

//#define GUI 1 // Use this when debugging on PC

int _BAUD = 115200;
int _thresholdBlue = 50; //阈值的蓝色
int _thresholdGreen = 50;
int _thresholdRed = 80;

int _dilate = 0; //扩张
int _erode = 3;  //侵蚀

int _contoursRed = 0;
int _contoursGreen = 255;
int _contoursBlue = 25;

int _thickness = 3;
int _lineType = 8;
double t = 0;
double fps;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initStyle();
    ui_init();

    time_clock = new QTimer();
    time_clock->setInterval(30);    //超时时间
    time_clock->start();

    ui->getCameraName->addItem(QString("选择摄像头"));
    foreach (const QCameraInfo &cameraInfo , QCameraInfo::availableCameras()) {
        ui->getCameraName->addItem(cameraInfo.description());
    }
    ui->getCameraName->addItem("FLIR Lepton");

}

MainWindow::~MainWindow()
{
    delete ui;
    delete time_clock;
    delete pTimer;
    capture.release();
}
void MainWindow::on_Process_clicked()
{
    if(capture.isOpened()){
        startProcess = true;
        ui->output->append("开启检测！");
    }
}

void MainWindow::on_EndProcess_clicked()
{
    if(capture.isOpened()){
        startProcess = false;
        ui->output->append("停止检测！");
    }
}

void MainWindow::on_OpenCamera_clicked()
{
    int num = ui->getCameraName->currentIndex() - 1;
    //    printf("%d\n",num);
    capture.open(num);
    if(capture.isOpened()){
        ui->output->append("摄像头打开成功！");
    }else{
        ui->output->append("<font color=\"#FF0000\">摄像头打开失败，请检测是否正确连接。</font>");
    }
    connect(time_clock, SIGNAL(timeout()), this, SLOT(LoopToDealFrame()));    //当达到超时时间，则发射信号，执行指定的槽函数
}

void MainWindow::on_OpenFile_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("打开视频"), ".", tr("Video File(*.avi *.mp4 *.h264)"));
    capture.open(file_name.toStdString());
    cout << file_name.toStdString() <<endl;

    if(capture.isOpened()){
        ui->output->append("视频打开成功！");
        ui->output->append(file_name);
        ui->getCameraName->setCurrentIndex(0);
    }else{
        ui->output->append("<font color=\"#FF0000\">视频打开失败！</font>");
    }
    connect(time_clock, SIGNAL(timeout()), this, SLOT(LoopToDealFrame()));    //当达到超时时间，则发射信号，执行指定的槽函数

}

void MainWindow::LoopToDealFrame()
{
    Mat srcImage;
    Mat forQImage;
    if(capture.isOpened())
    {
        capture >> srcImage;
        fps = capture.get(CAP_PROP_FPS);
        if(fps != 0.0){
            string fpsString("FPS:");
            fpsString += to_string(fps);
            putText(srcImage, // 图像矩阵
                                fpsString,                  // string型文字内容
                                cv::Point(5, 20),           // 文字坐标，以左下角为原点
                                cv::FONT_HERSHEY_SIMPLEX,   // 字体类型
                                0.5, // 字体大小
                                cv::Scalar(255, 0, 0));       // 字体颜色
        }

        if (!srcImage.empty())
        {
            if(startProcess == true){
                DealFrame(srcImage);//里边是opencv处理 所以使用的是BGR 还未转化为RGB
            }
            cvtColor(srcImage, forQImage, CV_BGR2RGB); // opencv是BGR QImage是RGB

            //img = QImage((const unsigned char *)srcImage.data, srcImage.cols, srcImage.rows, QImage::Format_RGB888); //Format_RGB888

            img = QImage((const unsigned char *)forQImage.data, forQImage.cols, forQImage.rows, QImage::Format_RGB888); //Format_RGB888

            //QImage fitpixmap = img.scaled(ui->label->width(), ui->label->height(),
            //                              Qt::IgnoreAspectRatio, Qt::SmoothTransformation);  // 饱满填充

            QImage fitpixmap = img.scaled(ui->label->width(), ui->label->height(),
                                          Qt::KeepAspectRatio, Qt::SmoothTransformation);  // 按比例缩放

            ui->label->setPixmap(QPixmap::fromImage(fitpixmap));
            ui->label->show();

            if(REC_flag == 1){
                saveVideo << srcImage;
                // ui->outputREC->append("add one frame");
            }
        }else{
            ui->output->append("<font color=\"#FF0000\">视频读取结束</font>");
            return;
        }

    }
}

void MainWindow::findRed(cv::Mat &img, cv::Mat &out, int thresholdBlue = 100, int thresholdGreen = 100, int thresholdRed = 150)
{
    img.copyTo(out);
    inRange(img, Scalar(0, 0, thresholdRed), Scalar(thresholdBlue, thresholdGreen, 255), out);
    out = out < 128;
}

// 获取图像中最大的轮廓
double MainWindow::findLargestContour(cv::Mat &img, cv::Mat &out)
{
    cv::Mat analyze;
    img.copyTo(analyze);

    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;
    cv::findContours( analyze, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    double area;
    double maxArea = 0;;
    int maxIndex = 0;
    int i = 0;
    for( i = 1; i < contours.size(); i++ )	/// Start at 1 to skip entire image contour
    {
        area = contourArea(contours[i]);
        if (area > maxArea)
        {
            maxArea = area;
            maxIndex = i;
        }
    }

    if (maxIndex != 0)
    {
        drawContours( out, contours, maxIndex, Scalar(_contoursBlue,_contoursGreen,_contoursRed), _thickness, _lineType, hierarchy, 0, Point() );
    }

    return maxArea;
}

//受害者检测
void MainWindow::DealFrame(Mat &image)
{
    Mat modified;
    Mat result;
    clock_t start;

    unsigned int lastFind = 0;
    double time;

    assert((start = clock()) != -1);
    findRed(image, modified, _thresholdBlue, _thresholdGreen, _thresholdRed); /// three channels

    dilate(modified, modified, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * _dilate + 1, 2 * _dilate + 1), cv::Point(_dilate, _dilate)));
    erode(modified, result, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * _erode + 1, 2 * _erode + 1), cv::Point(_erode, _erode)));

    // Found object
    if (findLargestContour(result, image) > 200)
    {

        // Calculate execution time and send coordinates to base station
        time = (double)(clock() - start) / CLOCKS_PER_SEC;
        if ((std::time(0) - lastFind) >= 5)
        {
            lastFind = std::time(0);
            // printf("Sent coordinates\n");
        }
        //printf("Found object: %fs\n", time);
        str.sprintf("Found object: %fs", time);//(33.9456384530,108.8079071045)
        ui->output->append(str); // 每次向后追加
        // ui->output->setPlainText(str);
    }else{
        str.sprintf("No object");
        ui->output->append(str);
    }

    #ifdef GUI
    imshow("Image", image);
    cv::waitKey(25);	/// 67 = 15 FPS
    #endif
}


void MainWindow::on_StartREC_clicked()
{
    if(capture.isOpened()){
        REC_flag = 1;
        QDateTime time = QDateTime::currentDateTime();
        QString dateTime = time.toString("yyyy-MM-dd-hh-mm-ss");
        QString file_name= QString("video%1.avi").arg(dateTime);
        QString s = file_path;
        QString newname = s.append('/').append(file_name);
        s.clear();
        ui->outputREC->append("开始录制");
        ui->outputREC->append(newname);
        // 开始计时
        this->pTimer = new QTimer;
        connect(this->pTimer,SIGNAL(timeout()),this,SLOT(updateDisplay()));

        this->baseTime = this->baseTime.currentTime();
        this->pTimer->start(1000); // 每1000ms = 1s 调用一次计时器

        //    printf("%f\n",(capture.get(CAP_PROP_FPS)));
        //    printf("%f\n",(capture.get(CAP_PROP_FRAME_WIDTH)));
        //    printf("%f\n",(capture.get(CAP_PROP_FRAME_HEIGHT)));
        if(capture.get(CAP_PROP_FPS) == 0.0){
            saveVideo.open(newname.toStdString(),CV_FOURCC('M', 'P', '4', '2'), 25, Size(640, 480), true); //capture.get(CAP_PROP_FPS)获得帧率
        }else{
            int height = capture.get(CAP_PROP_FRAME_HEIGHT);
            int wed = capture.get(CAP_PROP_FRAME_WIDTH);
            saveVideo.open(newname.toStdString(),CV_FOURCC('M', 'P', '4', '2'), capture.get(CAP_PROP_FPS) , Size(wed, height), true); //capture.get(CAP_PROP_FPS)获得帧率
        }

    }
}

void MainWindow::on_EndREC_clicked()
{
    if(capture.isOpened()){
        if(REC_flag == 1 || REC_flag == 2){ //如果状态是 录像 或 暂停 那么可以结束
            REC_flag = 0;
            saveVideo.release();
            ui->outputREC->append("<font color=\"#FF0000\">结束录制</font>");
            // 结束计时
            delete this->pTimer;
        }
    }
}

void MainWindow::on_PauseREC_clicked()
{
    if(capture.isOpened()){
        if(REC_flag == 1){
            REC_flag = 2;
            ui->outputREC->append("暂停");
        }
    }
}

void MainWindow::on_FilePath_clicked()
{
    //if(capture.isOpened()){
        file_path = QFileDialog::getExistingDirectory(this, "请选择保存路径", "./");
        ui->outputREC->append(file_path);
    //}
}

void MainWindow::on_holdonREC_clicked()
{
    if(capture.isOpened()){
        if(REC_flag == 2){
            REC_flag = 1;
            ui->outputREC->append("继续");
        }
    }
}

//更新时间
void MainWindow::updateDisplay()
{
    /*
     * 1.点击开始后获取到当前的时间并且赋值给baseTime
     * 2.启动定时器,间隔1s
     * 3.槽函数中再次获取当前的时间currTime
     * 4.计算两个时间的差值t
     * 5.声明一个showTime对象给他加上t的差值
     * 6.格式化后设置显示
     */
    QTime currTime = QTime::currentTime();
    int t = this->baseTime.msecsTo(currTime);
    QTime showTime(0,0,0);
    showTime = showTime.addMSecs(t);
    this->timeStr = showTime.toString("hh:mm:ss");
    // ui->outputREC->append(timeStr);
    this->ui->Lcd_play->display(timeStr);
}

void MainWindow::ui_init(){

    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);    //禁止最大化
    setFixedSize(this->width(),this->height()); // 禁止拖动窗口大小

    ui->output->document()->setMaximumBlockCount(100);
    ui->outputREC->document()->setMaximumBlockCount(100);

    ui->output->append("欢迎使用红外热成像协助搜救系统！");
    ui->Lcd_play->display("00:00:00");

    ui->label->setFrameShape (QFrame::Box);
    ui->label->setStyleSheet("border-width: 1px;border-style: solid;border-color: rgb(128, 128, 128);");


    ui->BlueSlider->setMinimum(0);          //设置滑动条控件的最小值
    ui->BlueSlider->setMaximum(255);        //设置滑动条控件的最大值
    ui->BlueSlider->setValue(_thresholdBlue);   //设置滑动条控件的值
    ui->BlueSlider->setPageStep(1);             //设置每次滑动加 1

    ui->BlueText->setText(QString::number(_thresholdBlue));

    ui->RedSlider->setMinimum(0);
    ui->RedSlider->setMaximum(255);
    ui->RedSlider->setValue(_thresholdRed);
    ui->RedSlider->setPageStep(1);
    ui->RedText->setText(QString::number(_thresholdRed));

    ui->GreenSlider->setMinimum(0);
    ui->GreenSlider->setMaximum(255);
    ui->GreenSlider->setValue(_thresholdGreen);
    ui->GreenSlider->setPageStep(1);
    ui->GreenText->setText(QString::number(_thresholdGreen));

    connect(ui->BlueSlider, SIGNAL(valueChanged(int)), this, SLOT(setLineEditValue_Blue(int)));
    connect(ui->RedSlider, SIGNAL(valueChanged(int)), this, SLOT(setLineEditValue_Red(int)));
    connect(ui->GreenSlider, SIGNAL(valueChanged(int)), this, SLOT(setLineEditValue_Green(int)));

    // spinBox 边框
    ui->spinBoxBlue->setValue(_contoursBlue);
    ui->spinBoxGreen->setValue(_contoursGreen);
    ui->spinBoxRed->setValue(_contoursRed);
    QString strcol = QString("background-color: rgb(%1,%2,%3);").arg(_contoursRed).arg(_contoursGreen).arg(_contoursBlue);
    ui->showColor->setStyleSheet(strcol);
    strcol.clear();

    connect(ui->spinBoxBlue, SIGNAL(valueChanged(int)), this, SLOT(setContoursColor_Blue(int)));
    connect(ui->spinBoxRed, SIGNAL(valueChanged(int)), this, SLOT(setContoursColor_Red(int)));
    connect(ui->spinBoxGreen, SIGNAL(valueChanged(int)), this, SLOT(setContoursColor_Green(int)));

    //线条粗细
    ui->thicknessSlider->setMinimum(-1);          //设置滑动条控件的最小值
    ui->thicknessSlider->setMaximum(10);        //设置滑动条控件的最大值
    ui->thicknessSlider->setValue(_thickness);    //设置滑动条控件的值
    ui->thicknessText->setText(QString::number(_thickness));
    ui->thicknessSlider->setPageStep(1);
    connect(ui->thicknessSlider, SIGNAL(valueChanged(int)), this, SLOT(setLineEditValue_Thickness(int)));
}

void MainWindow::setContoursColor_Blue(int value){
    _contoursBlue = ui->spinBoxBlue->value();

    QString strcol = QString("background-color: rgb(%1,%2,%3);").arg(_contoursRed).arg(_contoursGreen).arg(_contoursBlue);
    ui->showColor->setStyleSheet(strcol);
    strcol.clear();
}

void MainWindow::setContoursColor_Green(int value){
    _contoursGreen = ui->spinBoxGreen->value();

    QString strcol = QString("background-color: rgb(%1,%2,%3);").arg(_contoursRed).arg(_contoursGreen).arg(_contoursBlue);
    ui->showColor->setStyleSheet(strcol);
    strcol.clear();
}

void MainWindow::setContoursColor_Red(int value){
    _contoursRed = ui->spinBoxRed->value();

    QString strcol = QString("background-color: rgb(%1,%2,%3);").arg(_contoursRed).arg(_contoursGreen).arg(_contoursBlue);
    ui->showColor->setStyleSheet(strcol);
    strcol.clear();
}

void MainWindow::setLineEditValue_Blue(int value){
    int pos = ui->BlueSlider->value();
    QString str = QString("%1").arg(pos);
    ui->BlueText->setText(str);
    _thresholdBlue = pos;

}

void MainWindow::setLineEditValue_Red(int value){
    int pos = ui->RedSlider->value();
    QString str = QString("%1").arg(pos);
    ui->RedText->setText(str);
    _thresholdRed = pos;

}

void MainWindow::setLineEditValue_Green(int value){
    int pos = ui->GreenSlider->value();
    QString str = QString("%1").arg(pos);
    ui->GreenText->setText(str);
    _thresholdGreen = pos;
}

void MainWindow::setLineEditValue_Thickness(int value){
    int pos = ui->thicknessSlider->value();
    QString str= "";
    if(pos >= 0){
        str = QString("%1").arg(pos);
    }else{
        str = QString("Full");
    }
    ui->thicknessText->setText(str);
    _thickness = pos;
}


void MainWindow::initStyle()
{
    //加载样式表
    //QFile file(":/qss/psblack.css");
    QFile file(":/qss/flatwhite.css");
    //QFile file(":/qss/lightblue.css");
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }
}

void MainWindow::on_showColor_clicked()
{
    QColor color = QColorDialog::getColor(Qt::blue);
    if(color.isValid()){
        // ui->output->setPalette(QPalette(c));
        _contoursBlue = color.blue();
        _contoursGreen = color.green();
        _contoursRed = color.red();

        ui->spinBoxBlue->setValue(_contoursBlue);
        ui->spinBoxGreen->setValue(_contoursGreen);
        ui->spinBoxRed->setValue(_contoursRed);
        // setStyleSheet中后边为字符串，需要拼接
        // QString strcol = "background-color: rgb(";
        // strcol.append(QString::number(_contoursRed)).append(',');
        // strcol.append(QString::number(_contoursGreen)).append(',');
        // strcol.append(QString::number(_contoursBlue)).append(");");

        QString strcol = QString("background-color: rgb(%1,%2,%3);").arg(_contoursRed).arg(_contoursGreen).arg(_contoursBlue);
        ui->showColor->setStyleSheet(strcol);
        strcol.clear();
    }
}
