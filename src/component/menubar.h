#ifndef MENUBAR_H
#define MENUBAR_H


#include <QMenuBar>
#include <QSettings>
#include <QActionGroup>

class MenuBar : public QMenuBar {
    Q_OBJECT
public:
    explicit MenuBar(QWidget* parent = nullptr);

    void loadState();
    void saveState();


private:
    void setupMenu();
    // void applyActionState(QAction* action, const MenuNode& node);

    QMap<QString, QAction*> actionMap_;
    QSettings settings_;
};


#endif // MENUBAR_H
