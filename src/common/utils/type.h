#ifndef TYPE_H
#define TYPE_H

#include <QString>
#include <QList>
#include <QIcon>
#include <QKeySequence>

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
};

#endif // TYPE_H
