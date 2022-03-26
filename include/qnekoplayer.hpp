#if !defined(_QNEKOPLAYER_HPP_)
#define _QNEKOPLAYER_HPP_
#include <QWidget>
/**
 * @brief Neko Player widget
 * 
 */
class QNekoPlayer:public QWidget{
    Q_OBJECT

    public Q_SLOTS:
        virtual void playURL(const QUrl &url) = 0;
        virtual void pause() = 0;
        virtual void continue_() = 0;
    Q_SIGNALS:
        void videoFinished();
        void videoLoaded();
        void videoPaused();
        void videoContinued();
    public:
        /**
         * @brief Get internal avplayer instance
         * 
         * @return void* 
         */
        virtual void *av_player() = 0;

        static QNekoPlayer *Create(QWidget *parent = nullptr);
    private:
        using QWidget::QWidget;
};

#endif // _QNEKOPLAYER_HPP_
