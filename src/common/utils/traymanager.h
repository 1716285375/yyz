#ifndef TRAYMANAGER_H
#define TRAYMANAGER_H


#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QJsonDocument>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QTranslator>
#include <QMap>
#include <QSettings>

class FileWatcher;

class TrayManager : public QObject {
    Q_OBJECT

public:
    static TrayManager* instance();

    void setupTray();
    void showMessage(const QString& title, const QString& message, QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information, int timeout = 1000);

signals:
    void trayActionTriggered(const QString& key);

private slots:


private:
    explicit TrayManager(QObject* parent = nullptr);

    void setMenu();                     // 加载菜单结构
    void reloadMenu();                   // 重新加载菜单（热更新）
    void watchMenuFile();                // 启动监听器
    void connectActions();

    static TrayManager* instance_;

    QSystemTrayIcon* trayIcon_ = nullptr;

    QMap<QString, QAction*> actionMap_;
    QSettings settings_;

    FileWatcher* fileWatcher_ = nullptr;
    QMenu* trayMenu_ = nullptr;         // 托盘菜单
};


#endif // TRAYMANAGER_H
