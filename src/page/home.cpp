#include "home.h"
#include "renderopengl.h"
#include "dock.h"
#include "menubar.h"
#include "log.h"
#include <voice.h>
#include <translate.h>

#include <voice.h>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QToolBar>
#include <QPushButton>
#include <QSpacerItem>
#include <QSplitter>
#include <QCoreApplication>
#include <QMap>

Home::Home(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
}

Home::~Home()
{
    menuBar_->saveState();
}

void Home::setupUi()
{
    setupTopNav();

    setupCenterLayout();
}

void Home::setupTopNav()
{
    setupMenuDispatcher();
    menuBar_ = new MenuBar(this);
    connect(menuBar_, &MenuBar::menuTriggered, this, &Home::handleMenuAction);
    menuBar_->loadState();
    setMenuBar(menuBar_);


}

void Home::setupCenterLayout()
{
    // 创建中心控件
    wgtCenter = new QWidget(this);
    auto layout = new QHBoxLayout(wgtCenter);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // 左侧面板
    wgtLeft = new QWidget(wgtCenter);
    wgtLeft->setObjectName("wgtLeft");
    wgtLeft->setFixedWidth(200);

    voice = new Voice(wgtLeft);
    translate = new Translate(wgtLeft);

    auto vSplitter = new QSplitter(Qt::Vertical, wgtLeft);
    auto vLayoutLeft = new QVBoxLayout(wgtLeft);
    vLayoutLeft->addWidget(vSplitter);
    // 代码设置QSplitter必要属性
    vSplitter->setHandleWidth(10);
    vSplitter->setChildrenCollapsible(false);
    vSplitter->setStretchFactor(0, 1);
    vSplitter->setStretchFactor(1, 1);

    vSplitter->addWidget(translate);
    vSplitter->addWidget(voice);

    // 中间内容区域
    wgtContent = new QWidget(wgtCenter);
    wgtContent->setObjectName("wgtContent");
    wgtContent->setStyleSheet("background-color: white;");
    auto vLayoutContent = new QVBoxLayout(wgtContent);
    render = new RenderOpenGL(wgtContent);
    vLayoutContent->addWidget(render);

    // 右侧 dock 面板
    // wgtRightDock = new QWidget(wgtCenter);
    // wgtRightDock->setObjectName("wgtRightDock");
    // wgtRightDock->setFixedWidth(200);
    // wgtRightDock->setStyleSheet("background-color: #f0f0f0;");
    // auto vLayoutRight = new QVBoxLayout(wgtRightDock);
    // dwgtVoice = new Dock("Voice", wgtRightDock);
    // dwgtDanmu = new Dock("Damnu", wgtRightDock);
    // vLayoutRight->addWidget(dwgtVoice);
    // vLayoutRight->addWidget(dwgtDanmu);


    layout->addWidget(wgtLeft);
    layout->addWidget(wgtContent, 1);  // 伸缩项
    // layout->addWidget(wgtRight);

    setCentralWidget(wgtCenter);

    // QMainWindow 构造函数内
    dwgtVoice = new Dock("语音", this);
    dwgtDanmu = new Dock("弹幕", this);

    // 添加到右侧
    addDockWidget(Qt::RightDockWidgetArea, dwgtVoice);
    addDockWidget(Qt::RightDockWidgetArea, dwgtDanmu);
}

void Home::setupMenuDispatcher()
{
    menuDispatcher_["file.new"] = [this]() {
        // 实际逻辑，如创建新文档
        LOG_QS_DEBUG(QString("New file triggered."));
    };

    menuDispatcher_["file.open"] = [this]() {
        // 打开文件逻辑
        LOG_QS_DEBUG(QString("Open file triggered."));
    };

    menuDispatcher_["file.quit"] = [this]() {
        QCoreApplication::instance()->quit();
    };

    menuDispatcher_["view.theme.light"] = [this]() {
        LOG_QS_DEBUG(QString("Switch to light theme"));
    };

    menuDispatcher_["view.theme.dark"] = [this]() {
        LOG_QS_DEBUG(QString("Switch to dark theme"));
    };
}

void Home::handleMenuAction(const QString& key)
{
    auto it = menuDispatcher_.find(key);
    if (it != menuDispatcher_.end()) {
        it.value()();  // 调用绑定的 lambda 或函数
    } else {
        LOG_QS_WARN(QString("No handler for menu key: %1").arg(key));
    }
}
