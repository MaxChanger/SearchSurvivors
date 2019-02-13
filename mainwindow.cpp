#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <ctime>
#include <stdio.h>
#include <string>
#include <sstream>
#include <QString>
#include <QFileDialog>
#include <QCameraInfo>
#include <QDateTime>

using namespace std;
using namespace cv;

VideoCapture capture;
VideoWriter saveVideo;

#define GUI 1 /// Use this when debugging on PC

int _BAUD = 115200;
int _thresholdBlue = 255; //阈值的蓝色 //50 50 80
int _thresholdGreen = 255;
int _thresholdRed = 80;
int _dilate = 0; //扩张
int _erode = 3;  //侵蚀


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);    //禁止最大化
    setFixedSize(this->width(),this->height()); // 禁止拖动窗口大小

    ui->output->document()->setMaximumBlockCount(100);
    ui->outputREC->document()->setMaximumBlockCount(100);

    ui->output->append("欢迎使用红外热成像无人机协助搜救系统！");
    ui->Lcd_play->display("00:00:00");

    time_clock = new QTimer();
    time_clock->setInterval(30);    //超时时间
    time_clock->start();

    ui->getCameraName->addItem(QString("选择摄像头"));
    foreach (const QCameraInfo &cameraInfo , QCameraInfo::availableCameras()) {
        ui->getCameraName->addItem(cameraInfo.description());
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_Process_clicked()
{
    startProcess = true;
    ui->output->append("开启检测！");
}
void MainWindow::on_EndProcess_clicked()
{
    startProcess = false;
    ui->output->append("停止检测！");

}

void MainWindow::on_OpenCamera_clicked()
{
    int num = ui->getCameraName->currentIndex() - 1;
//    printf("%d\n",num);
    capture.open(num);
    if(capture.isOpened()){
        ui->output->append("摄像头打开成功！");
    }else{
        ui->output->append("摄像头打开失败，请检测是否正确链接。");
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
        ui->output->append("视频打开失败！");
    }
    connect(time_clock, SIGNAL(timeout()), this, SLOT(LoopToDealFrame()));    //当达到超时时间，则发射信号，执行指定的槽函数

}

void MainWindow::LoopToDealFrame()
{
    Mat srcImage;

    if (capture.isOpened())
    {
        // cout << "open" << endl;
        capture >> srcImage;
        if (!srcImage.empty())
        {
            cvtColor(srcImage, srcImage, CV_BGR2RGB); // 为什么要转化
            if(startProcess == true){
                DealFrame(srcImage);
            }
            img = QImage((const unsigned char *)srcImage.data, srcImage.cols, srcImage.rows, QImage::Format_RGB888); //Format_RGB888
            ui->label->setPixmap(QPixmap::fromImage(img));
            ui->label->show();
            if(REC_flag == true){
                // saveVideo.write(srcImage);
                saveVideo << srcImage;
                // ui->outputREC->append("add one frame");
            }
        }
    }
    else
    {
        cout << "not open" << endl;
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
        drawContours( out, contours, maxIndex, Scalar(0,0,0), 6, 8, hierarchy, 0, Point() );
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
        printf("Found object: %fs\n", time);
        str.sprintf("Found object: %fs", time);
        ui->output->append(str); // 每次向后追加
        // ui->output->setPlainText(str);


    }else{
        str.sprintf("No object");
        ui->output->append(str);
    }
}


void MainWindow::on_StartREC_clicked()
{
    if(capture.isOpened()){
        REC_flag = true;
        QDateTime time = QDateTime::currentDateTime();
        QString dateTime = time.toString("yyyy-MM-dd-hh-mm-ss");
        QString file_name= QString("Video%1.avi").arg(dateTime);
        QString s = file_path;
        QString newname = s.append('/').append(file_name);
        s.clear();
        ui->outputREC->append("开始录制");
        ui->outputREC->append(newname);
        //    printf("%f\n",(capture.get(CAP_PROP_FPS)));
        //    printf("%f\n",(capture.get(CAP_PROP_FRAME_WIDTH)));
        //    printf("%f\n",(capture.get(CAP_PROP_FRAME_HEIGHT)));
        if(capture.get(CAP_PROP_FPS) == 0.0){
            saveVideo.open(newname.toStdString(),CV_FOURCC('M', 'P', '4', '2'), 8, Size(640, 480), true); //capture.get(CAP_PROP_FPS)获得帧率
        }else{
            int height = capture.get(CAP_PROP_FRAME_HEIGHT);
            int wed = capture.get(CAP_PROP_FRAME_WIDTH);
            saveVideo.open(newname.toStdString(),CV_FOURCC('M', 'P', '4', '2'), capture.get(CAP_PROP_FPS) , Size(wed, height), true); //capture.get(CAP_PROP_FPS)获得帧率
        }
        // 开始计时
        this->pTimer = new QTimer;
        connect(this->pTimer,SIGNAL(timeout()),this,SLOT(updateDisplay()));

        this->baseTime = this->baseTime.currentTime();
        this->pTimer->start(1000); // 每1000ms = 1s 调用一次计时器

    }
}

void MainWindow::on_EndREC_clicked()
{
    if(capture.isOpened()){
        REC_flag = false;
        saveVideo.release();
        ui->outputREC->append("结束录制");
        // 结束计时
        delete this->pTimer;
    }
}

void MainWindow::on_PauseREC_clicked()
{
    if(capture.isOpened()){
        REC_flag = false;
        ui->outputREC->append("暂停");
    }
}

void MainWindow::on_FilePath_clicked()
{
    if(capture.isOpened()){
        file_path = QFileDialog::getExistingDirectory(this, "请选择保存路径...", "./");
        ui->outputREC->append(file_path);
    }
}

void MainWindow::on_pushButton_clicked()
{
    if(capture.isOpened()){
        REC_flag = true;
        ui->outputREC->append("继续录制");
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
