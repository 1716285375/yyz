// MenuBar.cpp
#include "MenuBar.h"
#include "misc.h"

#include <QMenu>
#include <QAction>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QTimer>

MenuBar::MenuBar(QWidget* parent) : QMenuBar(parent), settings_("AnhuiWeilaiTechlonogy", "yyz")
{
    setupMenu();
    connectActions();
}


void MenuBar::saveState() {
    zg::saveCheckableStates(settings_, actionMap_);
}

void MenuBar::loadState() {
    zg::loadCheckableStates(settings_, actionMap_);
}


void MenuBar::setupMenu()
{
    QString jsonPath = ":json/menubar.json";
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isArray()) return;

    const QList<MenuNode> nodes = zg::parseMenuNodeList(doc.array());
    zg::buildMenuBar(this, nodes, this, actionMap_);

}

void MenuBar::connectActions()
{
    // 统一连接所有 action 的 triggered → menuTriggered(key)
    for (auto it = actionMap_.cbegin(); it != actionMap_.cend(); ++it) {
        const QString& key = it.key();
        QAction* action = it.value();

        // 加载配置中的 checked 状态（覆盖 JSON 中的默认）
        bool shouldChecked = settings_.value(key + "/checked", action->isChecked()).toBool();
        action->setChecked(shouldChecked);

        // 连接 triggered 信号
        connect(action, &QAction::triggered, this, [this, key, action]() {
            // 存储当前状态（仅当 checkable）
            if (action->isCheckable()) {
                settings_.setValue(key + "/checked", action->isChecked());
            }

            emit menuTriggered(key);
        });

        // 可选：初始为 checked 的 action，触发一次
        if (shouldChecked) {
            QTimer::singleShot(0, this, [this, key]() {
                emit menuTriggered(key);
            });
        }
    }
}
