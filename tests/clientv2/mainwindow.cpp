#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit->setText("rtsp://admin:gdutcs719@10.21.71.146:554/Streaming/Channels/101");
    threads.push_back(std::thread(&MainWindow::renew,this));
}

void MainWindow::renew()
{
    SDLDisplayerHelper::Instance().runLoop();
}
MainWindow::~MainWindow()
{
    SDLDisplayerHelper::Instance().shutdown();
    delete ui;
    usleep(1500);
    exit(0);
}

void MainWindow::on_pushButton_clicked()
{
    string play_add=ui->lineEdit->text().toStdString();
    if(!play_add.empty()){
        ui->openGLWidget->run(play_add);
        ui->openGLWidget_2->run("rtsp://127.0.0.1/record/live/0/2019-01-23/11-00-53.mp4");
        ui->widget->run(play_add);
        ui->widget_2->run(play_add);

        int duration=(int)ui->openGLWidget->getDuration()/1000000;
        if(play_add.find("mp4")!=play_add.npos){
            ui->playButton->setUpdatesEnabled(true);
        }
        else{
            ui->playButton->setUpdatesEnabled(false);
        }
        if(duration>0){
            char buffer[30];
            DebugL<<duration;
            sprintf(buffer,"%02d:%02d/%02d:%02d",0,0,(int)duration/60,(int)duration%60);
            QString str = buffer;
            ui->horizontalSlider->setUpdatesEnabled(true);
            ui->horizontalSlider->setMaximum(duration);
            ui->label_4->setText(str);
        }
        else{
            ui->horizontalSlider->setUpdatesEnabled(false);
            ui->label_4->setText("--:--/--:--");
        }
    }
}

void MainWindow::on_playButton_clicked()
{
    if(ui->playButton->updatesEnabled()==true){
        //ui->openGLWidget->pause();
        if(ui->playButton->text().compare("PLAY")!=0){
            ui->playButton->setText("PLAY");
        }
        else{
            ui->playButton->setText("PAUSE");
        }
    }
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
//    ui->openGLWidget->seekTo(position);
}
