#ifndef BANNER_H
#define BANNER_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMouseEvent>

class Banner : public QWidget
{
    Q_OBJECT

public:
    explicit Banner(QWidget *parent = nullptr);
    Banner(const QString &title, bool showMinimize = true, QWidget *parent = nullptr);
    Banner(const QString& title, QSize size, QWidget *parent = nullptr);
    Banner(const QString& title, int width, int height, QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setShowMinimize(bool show);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void setupUI();
    void setupConnections();

private:
    int m_width;
    int m_height;
    QString m_title;
    QLabel *lblTitle;
    QPushButton *btnMinimize;
    QPushButton *btnClose;
    QHBoxLayout *hLayoutMain;

    QPoint dragPosition;
    bool isDragging = false;
    bool showMinBtn = true;

signals:
    void signalClose();
    void signalMinimize();
};

#endif // BANNER_H
