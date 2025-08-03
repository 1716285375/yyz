#ifndef APPICONMANAGER_H
#define APPICONMANAGER_H

#include <QObject>
#include <QIcon>
#include <QMap>

class AppIconManager : public QObject
{
    Q_OBJECT

public:
    static AppIconManager* instance();

    // 获取图标
    QIcon getIcon(const QString &iconName);

private:
    explicit AppIconManager(QObject *parent = nullptr);

    // 缓存图标
    QMap<QString, QIcon> m_iconCache;
    static AppIconManager* m_instance;
};

#endif // APPICONMANAGER_H
