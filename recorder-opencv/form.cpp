#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    replayTimer = new QTimer(this);
    this->setFixedSize(this->width(),this->height());
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(timerStart()));      // OK按钮，开始定时
    connect(ui->fileSelect,SIGNAL(clicked(bool)),this,SLOT(showPlayList()));    // 选择文件按钮
    connect(replayTimer,SIGNAL(timeout()),this,SLOT(replay()));                 // 时间一到执行回放函数

    // 滚动条QSS
    QFile qss("../recorder-opencv/uiComponents/scrollbar.qss");
    qss.open(QFile::ReadOnly);
    QString style = qss.readAll();
    ui->textEdit->verticalScrollBar()->setStyleSheet(style);
    qss.close();
}

Form::~Form()
{
    replayTimer->stop();
    cap.release();
    delete ui;
}


/*
 * 输出文件信息
*/
void Form::infoOutput(QString Message){
    ui->textEdit->insertPlainText(Message);
}

/*
 * 打开选择文件夹
*/
void Form::showPlayList(){
    filePath = QFileDialog::getOpenFileName(this,tr("选择回放文件"),"../videos","Video Files (*.avi *.mp4)");
    infoOutput("已选择文件:"+filePath+"\n");
//    if(!filePath.isEmpty()) replayTimer->start(33);  // 文件路径没问题，则计时器启动
    cap.open(filePath.toStdString());
    if(!cap.isOpened()){
        infoOutput("打开文件错误，请检查路径！\n");
        replayTimer->stop();
        return ;
    }
}


/*
 * replay timer启动
*/
void Form::timerStart(){
    replayTimer->start(33);
}


/*
 * 回放
*/
void Form::replay(){
    cap.read(frame);
    if(frame.empty()){
        ui->showReply->clear();
        replayTimer->stop();
        infoOutput("帧为空或播放已结束。\n");
        return ;
    }
    imgReplay = MatImageToQt(frame);
    ui->showReply->setPixmap(QPixmap::fromImage(imgReplay));     // 画在label中
}

/*
 * Mat转换QImage
*/
QImage Form::MatImageToQt(const cv::Mat &src){
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

