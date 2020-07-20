#include "widget.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    // 初始化变量
    mouse_press = false;
    ui->setupUi(this);
    this->setWindowTitle("QTRecorder");
    timer = new QTimer(this);   // 初始化定时器
    image = new QImage();       // 初始化QImage
    recordTimer = new QTimer(this);
    this->setFixedSize(this->width(),this->height()); // 固定大小
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint); // 隐藏顶栏
    this->setMouseTracking(true); // 打开鼠标追踪



    connect(ui->menuExitButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->menuSmallButton,SIGNAL(clicked()),this,SLOT(uiShowMinimized()));


    // 摄像头操作
    connect(timer,SIGNAL(timeout()),this,SLOT(readFrame()));                                // 定时器操作，时间到开始buffer并显示每一帧
    connect(ui->startCaptureButton,SIGNAL(clicked()),this,SLOT(startCapture()));            // 开始捕捉
    connect(ui->exitButton,SIGNAL(clicked()),this,SLOT(endCapture()));                      // 停止捕捉

    // 录制和抓拍
    connect(ui->takePhotosButton,SIGNAL(clicked(bool)),this,SLOT(takePhotos()));            // 按钮操作，按下开始抓拍一帧并写入文件
    connect(recordTimer,SIGNAL(timeout()),this,SLOT(record()));                             // 定时器操作，时间到则写入一帧至文件
    connect(ui->recordButton,SIGNAL(clicked(bool)),this,SLOT(startRecord()));               // 按钮操作，按下录制时器运作
    connect(ui->endRecordBtn,SIGNAL(clicked(bool)),this,SLOT(endRecord()));                 // 按钮操作，按下停止录制，不过不确定会不会有丢帧的现象

    connect(ui->replayButton,SIGNAL(clicked(bool)),this,SLOT(showReplay()));                // 打开回放界面


    // load qss
    QFile qss("../recorder-opencv/uiComponents/scrollbar.qss");
    qss.open(QFile::ReadOnly);
    QString style = qss.readAll();
    ui->textBrowser->verticalScrollBar()->setStyleSheet(style);
    qss.close();

    ui->textBrowser->insertPlainText("This is a program written by parker-int64\n"
                                     "Email:huxiaochuan2011@hotmail.com\n");

    // 抓拍和视频文件的个数
    screenshotNum = 0;
    videoNum = 0;

}

Widget::~Widget()
{
    delete ui;
}



/*
 * 输出信息
*/
void Widget::outputMessage(QString Message){
    ui->textBrowser->insertPlainText(Message);
    ui->textBrowser->moveCursor(QTextCursor::End);
}


/*
 * 最小化
*/
void Widget::uiShowMinimized(){
    this->showMinimized();
}


/*
 * 按下鼠标事件
*/
void Widget::mousePressEvent(QMouseEvent *event)
{
 if(event->button() == Qt::LeftButton)
 {
      mouse_press = true;

      //鼠标相对于窗体的位置
      move_point = event->pos();

 }

}


/*
 * 移动鼠标事件
*/
void Widget::mouseMoveEvent(QMouseEvent *event)
{
    //若鼠标左键被按下
    if(mouse_press)
    {
        //鼠标相对于屏幕的位置
        QPoint move_pos = event->globalPos();

       //移动主窗体位置
       this->move(move_pos - move_point);
    }

}


/*
 * 松开鼠标左键事件
*/
void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    //设置鼠标为未被按下
    mouse_press = false;

}




/*
 * 开始捕捉
*/
void Widget::startCapture(int captureWidth, int captureHeight,QString deviceName){
    cap.set(cv::CAP_PROP_FRAME_WIDTH,captureWidth);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT,captureHeight);
    cap.open(0);
    if(!cap.isOpened()){
       outputMessage("发生了错误，无法打开摄像头！\n");
       return ;
    }
    timer->start(33);

}

/*
 * 退出/结束捕捉
 * 并清空label_2
*/
void Widget::endCapture(){
    timer->stop();
    cap.release();
    ui->label_2->clear();
    outputMessage("结束捕捉，清理完成...\n");
}


/*
 * 读取一帧并返回
 * 此帧显示在label_2上
*/
void Widget::readFrame(){
    cap.read(frame);
    cv::putText(frame,getLocalTime(),cv::Point(0,20),cv::FONT_HERSHEY_COMPLEX,0.8,cv::Scalar(0,255,0),1,8);   // 绘制时间
    QImage img = MatImageToQt(frame);
    ui->label_2->setPixmap(QPixmap::fromImage(img));
}



/*
 * 开始录制的定时器
*/
void Widget::startRecord(QString saveLocation){
    if(!cap.isOpened()){
        outputMessage("请先打开摄像头，再开始录制！\n");
        return ;
    }
    videoFilePath = saveLocation + QString("/videos%1.avi").arg(videoNum);
    writer = cv::VideoWriter(videoFilePath.toStdString(), writer.fourcc('M','J','P','G'), 25.0,cv::Size(640,240));
    recordTimer->start(33);
}


/*
 * 录制的帧写入文件
*/
void Widget::record(){
    writer.write(frame);
    if(!writer.isOpened()){
        outputMessage("无法写入回放文件!\n");
        recordTimer->stop();
        return ;
    }

}

/*
 * 停止录制
*/
void Widget::endRecord(){
    writer.release();
    recordTimer->stop();
    outputMessage("录制完成，视频文件已存放至"+videoFilePath+"\n");
    videoNum++;
}

/*
 * 开始抓拍
*/
void Widget::takePhotos(QString saveLocation){
    if(!cap.isOpened()){
        outputMessage("请先打开摄像头，再开始抓拍！\n");
        return ;
    }
    cap.read(screenshot);
    cv::putText(screenshot,getLocalTime(),cv::Point(0,20),cv::FONT_HERSHEY_COMPLEX,0.8,cv::Scalar(0,255,0),1,8);   // 绘制时间
    QString filePath = saveLocation + QString("/screenshot%1.jpg").arg(screenshotNum);
    cv::imwrite(filePath.toStdString(),screenshot);
    outputMessage("抓拍文件已被存储至:"+filePath+"\n");
    screenshotNum++;
}

/*
 * 打开回放窗口
*/
void Widget::showReplay(){
    form = new Form();
    form->setWindowTitle("回放");
    form->show();
    this->showMinimized();
    outputMessage("已打开回放窗口！\n");
}

/*
 * Mat转成QImage的函数
*/
QImage Widget::MatImageToQt(const cv::Mat &src){
    //CV_8UC1 8位无符号的单通道---灰度图片
    if(src.type() == CV_8UC1)
    {
        //使用给定的大小和格式构造图像
        //QImage(int width, int height, Format format)
        QImage qImage(src.cols,src.rows,QImage::Format_Indexed8);
        //扩展颜色表的颜色数目
        qImage.setColorCount(256);

        //在给定的索引设置颜色
        for(int i = 0; i < 256; i ++)
        {
            //得到一个黑白图
            qImage.setColor(i,qRgb(i,i,i));
        }
        //复制输入图像,data数据段的首地址
        uchar *pSrc = src.data;
        //
        for(int row = 0; row < src.rows; row ++)
        {
            //遍历像素指针
            uchar *pDest = qImage.scanLine(row);
            //从源src所指的内存地址的起始位置开始拷贝n个
            //字节到目标dest所指的内存地址的起始位置中
            memcmp(pDest,pSrc,src.cols);
            //图像层像素地址
            pSrc += src.step;
        }
        return qImage;
    }
    //为3通道的彩色图片
    else if(src.type() == CV_8UC3)
    {
        //得到图像的的首地址
        const uchar *pSrc = (const uchar*)src.data;
        //以src构造图片
        QImage qImage(pSrc,src.cols,src.rows,src.step,QImage::Format_RGB888);
        //在不改变实际图像数据的条件下，交换红蓝通道
        return qImage.rgbSwapped();
    }
    //四通道图片，带Alpha通道的RGB彩色图像
    else if(src.type() == CV_8UC4)
    {
        const uchar *pSrc = (const uchar*)src.data;
        QImage qImage(pSrc, src.cols, src.rows, src.step, QImage::Format_ARGB32);
        //返回图像的子区域作为一个新图像
        return qImage.copy();
    }
    else
    {
        return QImage();
    }
}

/*
 * 获取系统时间
*/
std::string Widget::getLocalTime(){
    time_t timep;
    time (&timep); //获取time_t类型的当前时间
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&timep) );//对日期和时间进行格式化
    return tmp;
}






