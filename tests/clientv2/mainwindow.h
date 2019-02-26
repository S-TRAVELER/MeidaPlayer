#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <thread>
#include "Player/MediaPlayer.h"
#include  "../YuvDisplayer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void renew();

private slots:
    void on_pushButton_clicked();

    void on_playButton_clicked();

    void on_horizontalSlider_sliderMoved(int position);

private:
    Ui::MainWindow *ui;
    std::vector<std::thread> threads;
};

#endif // MAINWINDOW_H
