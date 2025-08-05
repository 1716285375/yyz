#ifndef MISC_H
#define MISC_H

#include "type.h"

class QAction;
class QActionGroup;
class QMenu;
class QMenuBar;
class QJsonDocument;
class QSettings;
class QJsonArray;
class QJsonObject;



namespace zg {

    QAction* createAction(const MenuNode& node, QObject* owner, QActionGroup* group = nullptr);

    void buildMenu(QMenu* parentMenu, const QList<MenuNode>& nodes, QObject* owner,
                   QMap<QString, QAction*>& actionMap, QActionGroup* parentGroup = nullptr);

    void buildMenuBar(QMenuBar* menuBar, const QList<MenuNode>& nodes, QObject* owner,
                      QMap<QString, QAction*>& actionMap);

    void buildTrayMenu(QMenu* parentMenu, const QList<MenuNode>& nodes, QObject* owner,
                       QMap<QString, QAction*>& actionMap);

    QList<MenuNode> parseMenuNodeList(const QJsonArray& array);


    // 保存所有可勾选 QAction 的状态到 QSettings
    void saveCheckableStates(QSettings& settings, const QMap<QString, QAction*>& actionMap);

    // 从 QSettings 读取并恢复所有 QAction 的勾选状态
    void loadCheckableStates(QSettings& settings, QMap<QString, QAction*>& actionMap);

    // 文件操作
    bool copyResourceToFile(const QString& resourcePath, const QString& targetPath);
    // 加载JSON配置文件
    bool loadJsonConfig(const QString& userPath, const QString& defaultPath, QJsonArray& outArray);

    void printMenuNode(const MenuNode& node);
}

#endif // MISC_H
