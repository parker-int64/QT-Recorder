#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFile>
#include <QScrollBar>
#include <QString>
#include <QMouseEvent>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <QString>
#include <QTimer>
#include <time.h>
#include "form.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void outputMessage(QString Message);
    void mousePressEvent(QMouseEvent *event);       //鼠标按下事件
    void mouseReleaseEvent(QMouseEvent *event);     //鼠标按下事件
    void mouseMoveEvent(QMouseEvent *event);        //鼠标移动事件

    QImage MatImageToQt(const cv::Mat &src);    // Mat转化成Qimage
    std::string getLocalTime();


private:
    Form *form;
    Ui::Widget *ui;
    QTimer *timer;          // 定时器
    QTimer *recordTimer;    // 录制定时器
    QImage *image;          // QImage
    QPoint move_point;      // 移动的距离
    bool mouse_press;       // 鼠标按下
    cv::Mat frame;
    cv::VideoCapture cap;
    cv::VideoWriter writer;
    cv::Mat screenshot;
    int screenshotNum;
    int videoNum;
    QString videoFilePath;

private slots:
    void uiShowMinimized();
    void startCapture(int captureWidth = 320, int captureHeight = 240,QString deviceName = "0");
    void endCapture();
    void readFrame();
    void startRecord(QString saveLocation = "../videos");
    void record();
    void endRecord();
    void takePhotos(QString saveLocation = "../photos");
    void showReplay();


};
#endif // WIDGET_H
