#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit->setText("rtsp://admin:gdutcs719@10.21.71.146:554/Streaming/Channels/101");
    Logger::Instance().add(std::make_shared<ConsoleChannel>("stdout", LTrace));
    threads.push_back(std::thread(&SDLDisplayerHelper::runLoop,&SDLDisplayerHelper::Instance()));
}

MainWindow::~MainWindow()
{
    SDLDisplayerHelper::Instance().shutdown();
    delete ui;
    usleep(1500);
    SDLDisplayerHelper::Instance().Destory();
    exit(0);
}

void MainWindow::on_pushButton_clicked()
{
    string play_add=ui->lineEdit->text().toStdString();
    if(!play_add.empty()){
        ui->openGLWidget->run(play_add);
        ui->openGLWidget_2->run("rtsp://127.0.0.1/record/live/0/2019-01-23/11-00-53.mp4");
        ui->widget->run(play_add);
        ui->widget_2->run("rtsp://127.0.0.1/record/live/0/2019-01-23/11-00-53.mp4");
    }
}

void MainWindow::on_playButton_clicked()
{
}

