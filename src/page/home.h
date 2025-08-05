#ifndef HOME_H
#define HOME_H

#include <QMainWindow>
#include <functional>


class QWidget;
class QLabel;
class QToolBar;
class RenderOpenGL;
class Dock;
class MenuBar;
class Voice;
class Translate;

class Home : public QMainWindow
{
    Q_OBJECT

public:
    explicit Home(QWidget *parent = nullptr);
    ~Home();

private:
    void setupUi();
    void setupTopNav();
    void setupCenterLayout();
    void setupMenuDispatcher();
    void handleMenuAction(const QString& key);

private:
    MenuBar *menuBar_ = nullptr;
    QWidget *wgtCenter = nullptr;
    QWidget *wgtLeft = nullptr;
    QWidget *wgtContent = nullptr;
    // QWidget *wgtRight = nullptr;
    QWidget *wgtTopNav = nullptr;
    Voice* voice = nullptr;
    Translate* translate = nullptr;
    RenderOpenGL* render = nullptr;
    Dock* dwgtVoice = nullptr;
    Dock* dwgtDanmu = nullptr;

    QMap<QString, std::function<void()>> menuDispatcher_;
};

#endif // HOME_H
