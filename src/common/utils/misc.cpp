// misc.cpp

#include "misc.h"
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

                QActionGroup* group = node.exclusive ? new QActionGroup(parentMenu) : nullptr;
                if (group)
                    group->setExclusive(true);

                for (const auto& child : node.children) {
                    if (child.title == "---") {
                        subMenu->addSeparator();
                        continue;
                    }

                    if (child.isLeaf()) {
                        QAction* action = createAction(child, owner, group);
                        subMenu->addAction(action);
                        actionMap[child.key] = action;
                    } else {
                        buildMenu(subMenu, {child}, owner, actionMap);
                    }
                }

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

    MenuNode parseMenuNode(const QJsonObject& obj)
    {
        MenuNode node;
        node.key = obj.value("key").toString();
        node.title = obj.value("title").toString();
        node.shortcut = obj.value("shortcut").toString();

        if (obj.contains("icon"))
            node.icon = QIcon(obj.value("icon").toString());

        if (obj.contains("checkedIcon"))
            node.checkedIcon = QIcon(obj.value("checkedIcon").toString());

        node.checkable = obj.value("checkable").toBool(false);
        node.checked = obj.value("checked").toBool(false);
        node.exclusive = obj.value("exclusive").toBool(false);

        // ✅ 新增字段解析
        node.enabled = obj.value("enabled").toBool(true);
        node.tooltip = obj.value("tooltip").toString();

        if (obj.contains("children") && obj["children"].isArray()) {
            node.children = parseMenuNodeList(obj["children"].toArray());
        }

        return node;
    }

    QList<MenuNode> parseMenuNodeList(const QJsonArray& array)
    {
        QList<MenuNode> list;
        for (const auto& val : array) {
            if (val.isObject()) {
                list.append(parseMenuNode(val.toObject()));
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

}
