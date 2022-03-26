#include <QNekoPlayer>
#include <QtAVWidgets>
#include <QtAV/AVPlayer.h>
#include <QtAV>

#include "ui_nekoplayer.h"

namespace{

class QNekoPlayerImpl:public QNekoPlayer{
    public:
        QNekoPlayerImpl(QWidget *parent);
        ~QNekoPlayerImpl();
    private:
        //Process keyboard events
        void keyReleaseEvent(QKeyEvent *event) override;
    private:
        Ui_NekoPlayer *ui;
        QtAV::WidgetRenderer *renderer;
        QtAV::AVPlayer *player;

        //Status
        bool fullscreen = false;
        bool paused = false;//Widget paused status
        void on_fullscreen_button();
        
        void on_progressSlider_sliderMoved(int position);
        void on_progressSlider_sliderPressed();
        void on_progressSlider_sliderReleased();

        void on_player_error(const QtAV::AVError &error);
        void on_player_loaded();
        void on_player_unloaded();
        void on_player_positionChanged(qint64 position);
        void on_player_finished();

        void on_playButton_clicked();

        void do_pause();
        void do_continue();
    private:
        void playURL(const QUrl &url) override;
        void pause() override;
        void continue_() override;
        void *av_player() override;
    private:
        QString dur_to_string(qint64 dur);
};

QNekoPlayerImpl::QNekoPlayerImpl(QWidget *parent) :
    QNekoPlayer(parent),
    ui(new Ui_NekoPlayer){
    
    QtAV::Widgets::registerRenderers();

    ui->setupUi(this);
    renderer = new QtAV::WidgetRenderer();
    player = new QtAV::AVPlayer;

    ui->playerLayout->addWidget(renderer);

    player->setRenderer(renderer);
    //Connect control menu
    connect(ui->fullScreenButton,&QToolButton::clicked,[&](){
        on_fullscreen_button();
    });
    connect(ui->playButton,&QToolButton::clicked,[&](){
        on_playButton_clicked();
    });
    //Connect control slider
    connect(ui->progressSlider,&QSlider::sliderMoved,[&](int position){
        on_progressSlider_sliderMoved(position);
    });
    connect(ui->progressSlider,&QSlider::sliderPressed,[&](){
        on_progressSlider_sliderPressed();
    });
    connect(ui->progressSlider,&QSlider::sliderReleased,[&](){
        on_progressSlider_sliderReleased();
    });
    //Connect Player L/S
    connect(player,&QtAV::AVPlayer::loaded,[&](){
        on_player_loaded();
    });
    connect(player,&QtAV::AVPlayer::error,[&](const QtAV::AVError &error){
        on_player_error(error);
    });
    connect(player,&QtAV::AVPlayer::positionChanged,[&](qint64 position){
        on_player_positionChanged(position);
    });
    connect(player,&QtAV::AVPlayer::stoppedAt,[&](qint64 pos){
        on_player_finished();
    });
}

QNekoPlayerImpl::~QNekoPlayerImpl(){
    delete renderer;
    delete player;
    delete ui;
}
void QNekoPlayerImpl::on_fullscreen_button(){
    fullscreen = !fullscreen;
    //Find top level widget
    QWidget *w = this->window();
    while(w->parentWidget()){
        w = w->parentWidget();
    }
    if(fullscreen){
        w->showFullScreen();
    }
    else{
        w->showNormal();
    }
}
void QNekoPlayerImpl::on_progressSlider_sliderMoved(int position){
    qDebug() << "Slider moved to " << position;
    player->setPosition(position);
}
void QNekoPlayerImpl::on_progressSlider_sliderPressed(){
    qDebug() << "sliderPressed";
    if(not paused){
        do_pause();
    }
}
void QNekoPlayerImpl::on_progressSlider_sliderReleased(){
    qDebug() << "sliderReleased";
    if(not paused){
        do_continue();
    }
}
void QNekoPlayerImpl::on_player_positionChanged(qint64 position){
    ui->progressSlider->setValue(position);
    ui->currentTimelabel->setText(dur_to_string(position));
}
void QNekoPlayerImpl::on_player_loaded(){
    //Configure slider
    ui->progressSlider->setEnabled(true);
    ui->progressSlider->setMaximum(player->mediaStopPosition());
    ui->progressSlider->setMinimum(player->mediaStartPosition());
    ui->progressSlider->setValue(player->position());
    //Configure duration label
    ui->currentTimelabel->setText(
        QString::fromUtf8("00:00:00")
    );
    ui->maxTimeLabel->setText(
        dur_to_string(player->mediaStopPosition())
    );
    ui->playButton->setEnabled(true);
    ui->playButton->setText(QString::fromUtf8("Pause"));

    paused = false;

    emit videoLoaded();
}
void QNekoPlayerImpl::on_player_finished(){
    qDebug() << "Player finished";
    //Reset slider
    ui->progressSlider->setValue(player->mediaStartPosition());
    ui->progressSlider->setDisabled(true);
    //Reset duration label
    ui->currentTimelabel->setText(
        QString::fromUtf8("00:00:00")
    );

    //Disable play button
    ui->playButton->setEnabled(false);
    ui->playButton->setText(QString::fromUtf8("Play"));

    emit videoFinished();
}
void QNekoPlayerImpl::on_player_error(const QtAV::AVError &error){
    //TODO: Handle error
    qDebug() << "Player error: " << error.string();
}

void QNekoPlayerImpl::on_playButton_clicked(){
    if(!paused){
        do_pause();
    }
    else{
        do_continue();
    }
    paused = !paused;
}

void QNekoPlayerImpl::do_continue(){
    //TODO: only this it will work
    ui->playButton->setText(QString::fromUtf8("Pause"));
    player->pause(false);
    player->pause();
    player->pause(false);

    emit videoContinued();
}
void QNekoPlayerImpl::do_pause(){
    ui->playButton->setText(QString::fromUtf8("Play"));
    player->pause();

    emit videoPaused();
}
//Parent methods
void QNekoPlayerImpl::playURL(const QUrl &url){
    player->play(url.toString());
}
void *QNekoPlayerImpl::av_player(){
    return player;
}

QString QNekoPlayerImpl::dur_to_string(qint64 dur){
    return  QString::number(dur/1000/60/60) + ":" +
            QString::number(dur/1000/60) + ":" +
            QString::number(dur/1000%60);
}

//Key
void QNekoPlayerImpl::keyReleaseEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Right){
        qDebug() << "Right";
        event->accept();

        //Seek
        player->setPosition(player->position() + 1000);
    }
    else if(event->key() == Qt::Key_Left){
        qDebug() << "Left";
        event->accept();

        player->setPosition(player->position() - 1000);
    }
    else if(event->key() == Qt::Key_Space and ui->playButton->isEnabled()){
        //As same as play/pause button
        event->accept();
        on_playButton_clicked();
    }
    else{
        event->ignore();
    }
}
void QNekoPlayerImpl::pause(){
    if(ui->playButton->isEnabled()){
        do_pause();
    }
}
void QNekoPlayerImpl::continue_(){
    if(ui->playButton->isEnabled()){
        do_continue();
    }
}

}


QNekoPlayer *QNekoPlayer::Create(QWidget *parent){
    return new QNekoPlayerImpl(parent);
}
