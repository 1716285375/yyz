#include "appiconmanager.h"
#include "log.h"

#include <QIcon>
#include <QDir>
#include <QResource>
#include <QDebug>


// AppIconManager.cpp
AppIconManager* AppIconManager::m_instance = nullptr;

AppIconManager* AppIconManager::instance()
{
    if (m_instance == nullptr) {
        m_instance = new AppIconManager();
    }
    return m_instance;
}

AppIconManager::AppIconManager(QObject *parent)
    : QObject(parent)
{
    // 正确遍历Qt资源系统中的图标文件
    QDir iconDir(":/icons/");  // 注意末尾斜线，确保路径正确
    if (!iconDir.exists()) {
        LOG_ERROR("Icons resource directory not found!");
        return;
    }

    if (QFile::exists(":/icons/resources/icons/close.png")) {
        LOG_DEBUG("Test icon exists");
    } else {
        LOG_ERROR("Test icon not found!");
    }

    QStringList allIconFiles = iconDir.entryList(QDir::Files);
    LOG_DEBUG("All files in resource dir: {}", allIconFiles.join(", ").toStdString());

    // 方法1：兼容大小写的过滤器
    QStringList iconFiles = iconDir.entryList(
        QStringList() << "*.png" << "*.PNG" << "*.svg" << "*.SVG" << "*.jpg" << "*.JPG",
        QDir::Files
        );

    LOG_DEBUG("Found {} icon files", iconFiles.size());

    foreach (const QString& fileName, iconFiles) {
        QString iconName = QFileInfo(fileName).baseName();
        QString resourcePath = ":/icons/" + fileName;

        if (!QFile::exists(resourcePath)) {
            LOG_WARN("Icon resource not found: {}", resourcePath.toStdString());
            continue;
        }

        QIcon icon(resourcePath);
        if (icon.isNull()) {
            LOG_WARN("Failed to load icon: {}", resourcePath.toStdString());
            continue;
        }

        m_iconCache.insert(iconName, icon);
        LOG_DEBUG("Cached icon: {} from {}", iconName.toStdString(), fileName.toStdString());
    }
}

QIcon AppIconManager::getIcon(const QString &iconName)
{
    if (m_iconCache.contains(iconName)) {
        return m_iconCache[iconName];
    }
    LOG_QS_ERROR(QString("Icon not found: %1").arg(iconName));
    return QIcon();  // 返回空图标
}
