#ifndef TYPE_H
#define TYPE_H


#include "log.h"


#include <QString>
#include <QList>
#include <QIcon>
#include <QKeySequence>
#include <QDir>
#include <QStandardPaths>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>


struct MenuNode {
    QString key;
    QString title;
    QString shortcut;
    QIcon icon;
    QIcon checkedIcon;
    bool checkable = false;
    bool checked = false;
    bool exclusive = false;
    bool enabled = true;                 // ✅ 新增：是否启用
    QString tooltip;                    // ✅ 新增：tooltip 提示
    QList<MenuNode> children;

    bool isLeaf() const { return children.isEmpty(); }

    static MenuNode fromJson(const QJsonObject& obj)
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
            QJsonArray arr = obj.value("children").toArray();
            for (const QJsonValue& childVal : std::as_const(arr)) {
                if (childVal.isObject())
                    node.children.append(MenuNode::fromJson(childVal.toObject()));
            }
        }

        return node;
    }

};



namespace zg::path{
    // 确保路径存在（可选工具函数）
    inline void ensureDir(const QString& path) {
        QDir dir(path);
        if (!dir.exists()) dir.mkpath(".");
    }
    // App 数据目录（通用根路径，跨平台）
    inline QString appDataRoot() {
        return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }

    // app JSON 路径（热更新使用）
    inline QString userMenu() {
        return appDataRoot() + "/app.json";
    }

    // 用户菜单 JSON 路径（热更新使用）
    inline QString menuBar() {
        return appDataRoot() + "/menubar.json";
    }

    // 托盘 JSON 配置
    inline QString trayMenu() {
        return appDataRoot() + "/traymenu.json";
    }

    // 用户语言配置文件
    inline QString userLang() {
        return appDataRoot() + "/lang.json";
    }

    // 日志文件路径
    inline QString logFile() {
        return appDataRoot() + "/logs/app.log";
    }

    // 用户自定义配置文件目录
    inline QString userConfigDir() {
        QString configDir = appDataRoot() + "config";
        ensureDir(configDir);
        return configDir;
    }

    // 用户自定义配置文件（例如 UI 配置）
    inline QString userConfig() {
        return appDataRoot() + "/config.json";
    }

    // 用户缓存路径（下载缓存、图标缓存等）
    inline QString cacheDir() {
        return QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    }

    // Temp
    inline QString tempDir() {
        QString temp = appDataRoot() + "/temp";
        ensureDir(temp);
        return temp;

    }

    // 截图或临时图片目录
    inline QString tempImageDir() {

        return appDataRoot() + "/temp/images";
    }




}

#endif // TYPE_H
