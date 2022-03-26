#include <QNekoPlayer>
#include <QApplication>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMenuBar>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    QMainWindow window;
    QNekoPlayer *player = QNekoPlayer::Create(nullptr);

    window.setCentralWidget(player);
    window.menuBar()->addAction("Open",[&](){
        auto f = QFileDialog::getOpenFileUrl();
        player->playURL(f);
    });

    window.show();


    return a.exec();
}
