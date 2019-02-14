#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QTimer>
#include <QImage>
#include <QTime>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void DealFrame(cv::Mat& image);
    void LoopToDealFrame();
    void findRed(cv::Mat& img, cv::Mat& out, int thresholdBlue, int thresholdGreen , int thresholdRed );
    double findLargestContour(cv::Mat& img, cv::Mat& out);
    void updateDisplay();

    void on_EndProcess_clicked();

    void on_OpenCamera_clicked();

    void on_OpenFile_clicked();

    void on_Process_clicked();

    void on_StartREC_clicked();

    void on_EndREC_clicked();

    void on_PauseREC_clicked();

    void on_FilePath_clicked();

    void on_pushButton_clicked();

    void sliderRGBinit();

    void setLineEditValue_Blue(int value);
    void setLineEditValue_Red(int value);
    void setLineEditValue_Green(int value);
    void setContoursColor_Blue(int value);
    void setContoursColor_Red(int value);
    void setContoursColor_Green(int value);
    void initStyle();

    void on_showColor_clicked();

private:
    Ui::MainWindow *ui;
    QImage img;
    QTimer *time_clock;
    QString str = "";
    QString file_path=".";
    bool startProcess = false;
    bool REC_flag = false;

    QTimer *pTimer;
    QTime baseTime;
    //显示的时间
    QString timeStr;

};

#endif // MAINWINDOW_H
