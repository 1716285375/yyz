// misc.cpp

#include "misc.h"
#include "type.h"
#include "log.h"
#include <QObject>
#include <QDebug>
#include <QAction>
#include <QActionGroup>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMenu>
#include <QMenuBar>
#include <QMap>
#include <QList>
#include <QShortcut>


namespace zg {

    QAction* createAction(const MenuNode& node, QObject* owner, QActionGroup* group)
    {
        QAction* action = new QAction(QObject::tr(node.title.toUtf8()), owner);
        action->setCheckable(node.checkable);
        action->setChecked(node.checked);
        action->setEnabled(node.enabled);
        action->setShortcut(QKeySequence(node.shortcut));
        action->setIcon(node.icon);

        if (!node.tooltip.isEmpty())
            action->setToolTip(node.tooltip);

        if (node.checkable && !node.checkedIcon.isNull()) {
            QObject::connect(action, &QAction::toggled, owner, [action, node](bool checked) {
                action->setIcon(checked ? node.checkedIcon : node.icon);
            });
            action->setIcon(node.checked ? node.checkedIcon : node.icon);
        }

        if (group && node.checkable)
            group->addAction(action);

        return action;
    }

    void buildMenu(QMenu* parentMenu, const QList<MenuNode>& nodes, QObject* owner,
                   QMap<QString, QAction*>& actionMap, QActionGroup* parentGroup)
    {
        for (const auto& node : nodes) {
            if (node.isLeaf()) {
                if (node.title == "---") {
                    parentMenu->addSeparator();
                    continue;
                }
                QAction* action = createAction(node, owner, nullptr);
                parentMenu->addAction(action);
                actionMap[node.key] = action;
            } else {
                QMenu* subMenu = new QMenu(QObject::tr(node.title.toUtf8()), parentMenu);
                subMenu->setEnabled(node.enabled);
                if (!node.tooltip.isEmpty())
                    subMenu->setToolTip(node.tooltip);

                QActionGroup* group = nullptr;
                if (node.exclusive) {
                    group = new QActionGroup(subMenu);
                    group->setExclusive(true);
                }

                buildMenu(subMenu, node.children, owner, actionMap, group);

                parentMenu->addMenu(subMenu);
            }
        }
    }

    void buildMenuBar(QMenuBar* menuBar, const QList<MenuNode>& nodes, QObject* owner,
                      QMap<QString, QAction*>& actionMap)
    {
        for (const auto& node : nodes) {
            QMenu* menu = new QMenu(QObject::tr(node.title.toUtf8()));
            menu->menuAction()->setIcon(node.icon);
            buildMenu(menu, node.children, owner, actionMap, nullptr); // 传递 nullptr
            menuBar->addMenu(menu);
        }
    }

    void buildTrayMenu(QMenu *parentMenu, const QList<MenuNode> &nodes, QObject *owner,
                       QMap<QString, QAction *> &actionMap)
    {
        // 直接调用 buildMenu，托盘菜单不需要外层单独创建子菜单
        buildMenu(parentMenu, nodes, owner, actionMap);
    }


    QList<MenuNode> parseMenuNodeList(const QJsonArray& array)
    {
        QList<MenuNode> list;
        for (const auto& val : array) {
            if (val.isObject()) {
                list.append(MenuNode::fromJson(val.toObject()));
            }
        }
        return list;
    }


    void saveCheckableStates(QSettings& settings, const QMap<QString, QAction*>& actionMap)
    {
        for (auto it = actionMap.begin(); it != actionMap.end(); ++it) {
            if (it.value()->isCheckable()) {
                settings.setValue(it.key(), it.value()->isChecked());
            }
        }
    }

    void loadCheckableStates(QSettings& settings, QMap<QString, QAction*>& actionMap) {
        for (auto it = actionMap.begin(); it != actionMap.end(); ++it) {
            if (it.value()->isCheckable()) {
                it.value()->setChecked(settings.value(it.key(), false).toBool());
            }
        }
    }

    void printMenuNode(const MenuNode &node)
    {
        LOG_QS_INFO(
            QString("key: %1 |"
                    "title: %2 |"
                    "icon: %3 |"
                    "enabled: %4 |"
                    "chckedIcon: %5 |"
                    "tooltip: %6 |"
                    "shortcut: %7 |"
                    "exclusive: %8"
                ).arg(node.key)
                .arg(node.title)
                .arg(node.icon.name())
                .arg(node.enabled)
                .arg(node.checkedIcon.name())
                .arg(node.tooltip)
                .arg(node.shortcut)
                .arg(node.exclusive)

            );
    }

    bool copyResourceToFile(const QString &resourcePath, const QString &targetPath)
    {
        // 打开资源文件
        QFile resourceFile(resourcePath);
        if (!resourceFile.open(QIODevice::ReadOnly)) {
            LOG_CORE_ERROR("Failed to open resource file: {}", resourcePath.toStdString());
            return false;
        }

        // 读取资源文件内容
        QByteArray data = resourceFile.readAll();
        resourceFile.close();

        // 确保目标目录存在
        QDir().mkpath(QFileInfo(targetPath).absolutePath());

        // 打开目标文件
        QFile targetFile(targetPath);
        if (!targetFile.open(QIODevice::WriteOnly)) {
            LOG_CORE_ERROR("Failed to open target file: {}", targetPath.toStdString());
            return false;
        }

        // 写入目标文件
        targetFile.write(data);
        targetFile.close();

        return true;
    }

    bool loadJsonConfig(const QString& inPath, const QString& defaultPath, QJsonArray& outArray)
    {
        if (!QFile::exists(inPath)) {
            if (copyResourceToFile(defaultPath, inPath)) {
                qInfo() << QStringLiteral("User config not found. Copied default: %1").arg(inPath);
            } else {
                qCritical() << QStringLiteral("Failed to copy default config: %1 -> %2").arg(defaultPath, inPath);
                return false;
            }
        }

        // Try to read config file
        QFile file(inPath);
        if (!file.open(QIODevice::ReadOnly)) {
            LOG_CORE_ERROR("Failed to open  config file: {}", inPath.toStdString());
            return false;
        }

        QByteArray jsonData = file.readAll();
        file.close();

        // Parse JSON with error detection
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            LOG_CORE_ERROR("Failed to parse config JSON: {}", parseError.errorString().toStdString());
            return false;
        }

        if (!doc.isArray()) {
            LOG_CORE_WARN("Invalid config format: expected JSON array but got {}",
                            doc.isObject() ? "object" : doc.isNull() ? "null" : "unknown type");
            return false;
        }

        outArray = doc.array();
        return true;
    }

}
