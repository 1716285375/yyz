// MenuBar.cpp
#include "MenuBar.h"
#include "misc.h"

#include <QMenu>
#include <QAction>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>

MenuBar::MenuBar(QWidget* parent) : QMenuBar(parent), settings_("AnhuiWeilaiTechlonogy", "yyz")
{
    setupMenu();
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
