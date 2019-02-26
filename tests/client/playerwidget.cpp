#include "playerwidget.h"

PlayerWidget::PlayerWidget(QWidget* parent):QWidget(parent),player(new PlayerPro(displayer,1)),
    displayer(new YuvDisplayer((void*)this->winId()))
{
    setStyleSheet("background-color:black;");
}




PlayerWidget::~PlayerWidget(){
    player->shutdown();
}

void PlayerWidget::run(string address){
    if(!running){
        play_add=address;
        player->play(play_add.c_str());
        displayer->prompt("Loading...");
        running=true;
    }
    else{
        if(play_add.compare(address)!=0){
            player->shutdown();
            play_add=address;
            player->play(play_add.c_str());
            displayer->prompt("Loading...");
        }
    }
}


void PlayerWidget::pause(){
    player->pause(bpause);
    bpause=!bpause;
}
