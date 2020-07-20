#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QString>
#include <QFileDialog>
#include <QTimer>
#include <QImage>
#include <QScrollBar>
#include <opencv2/opencv.hpp>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();
    void infoOutput(QString Message);
    QImage MatImageToQt(const cv::Mat &src);

private:
    Ui::Form *ui;
    QString filePath;
    QTimer *replayTimer;
    cv::VideoCapture cap;
    cv::Mat frame;
    QImage imgReplay;


private slots:
    void showPlayList();
    void timerStart();
    void replay();
};

#endif // FORM_H
