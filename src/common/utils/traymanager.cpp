#include "traymanager.h"
#include "type.h"
#include "filewatcher.h"
#include "misc.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QAction>
#include <QCoreApplication>
#include <QDebug>


TrayManager* TrayManager::instance_ = nullptr;


TrayManager::TrayManager(QObject* parent)
    : QObject(parent),
    settings_("YourOrg", "YourApp"),
    fileWatcher_(new FileWatcher(this))
{
    // setupTray();
    connect(fileWatcher_, &FileWatcher::fileChanged, this, &TrayManager::reloadMenu);
}

void TrayManager::setupTray()
{
    setMenu();
    // 初始化托盘图标
    trayIcon_ = new QSystemTrayIcon(this);
    trayIcon_->setContextMenu(trayMenu_);
    trayIcon_->setIcon(QIcon(":/icons/tray.png"));
    trayIcon_->show();

    watchMenuFile();
}


void TrayManager::setMenu()
{
    QString trayPath = zg::path::trayMenu();
    QString defaultPath = ":/json/traymenu.json";

    QJsonArray arr = QJsonArray();
    if (!zg::loadJsonConfig(trayPath, defaultPath, arr)) {
        LOG_QS_WARN("load tray config error");
        return;
    }


    LOG_QS_INFO(QStringLiteral("Tray config loaded successfully with %1 items.").arg(arr.size()));

    const QList<MenuNode> nodes = zg::parseMenuNodeList(arr);

    trayMenu_ = new QMenu();
    zg::buildTrayMenu(trayMenu_, nodes, trayMenu_, actionMap_);

}

void TrayManager::reloadMenu()
{
    trayMenu_->clear();
    actionMap_.clear();

    setMenu();
}

void TrayManager::watchMenuFile()
{
    QString trayPath = zg::path::trayMenu();
    fileWatcher_->addWatch(trayPath);
}

void TrayManager::connectActions()
{
    // 统一连接所有 action 的 triggered → menuTriggered(key)

    for (auto it = actionMap_.cbegin(); it != actionMap_.cend(); ++it) {
        const QString& key = it.key();
        QAction* action = it.value();
        connect(it.value(), &QAction::triggered, this, [this, key](){
            emit trayActionTriggered(key);
        });
    }

}



TrayManager *TrayManager::instance()
{
    if (!instance_)
        instance_ = new TrayManager();
    return instance_;
}


void TrayManager::showMessage(const QString& title, const QString& message, QSystemTrayIcon::MessageIcon icon, int timeout)
{
    if (trayIcon_)
        trayIcon_->showMessage(title, message, icon, timeout);
}

