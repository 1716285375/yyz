#include "banner.h"

Banner::Banner(QWidget *parent)
    : m_width(600),
    m_height(50),
    m_title("Title"),
    QWidget(parent)
{
    setupUI();
    setupConnections();
}

Banner::Banner(const QString &title, bool showMinimize, QWidget *parent)
    : Banner(parent)
{
    lblTitle->setText(title);
    setShowMinimize(showMinimize);
}

Banner::Banner(const QString &title, QSize size, QWidget *parent)
{
    lblTitle->setText(title);
    m_width = size.width();
    m_height = size.height();
    resize(m_width, m_height);
}

Banner::Banner(const QString &title, int width, int height, QWidget *parent)
    : Banner(title, QSize(width, height), parent)
{

}

void Banner::setupUI()
{
    setFixedHeight(40);

    lblTitle = new QLabel("Title", this);
    lblTitle->setStyleSheet("color: white; font-weight: bold;");

    btnMinimize = new QPushButton("-", this);
    btnMinimize->setFixedSize(30, 30);
    btnMinimize->setVisible(showMinBtn);
    btnMinimize->setObjectName("btnMin");

    btnClose = new QPushButton("X", this);
    btnClose->setFixedSize(30, 30);
    btnClose->setObjectName("btnClose");

    hLayoutMain = new QHBoxLayout(this);
    hLayoutMain->addWidget(lblTitle);
    hLayoutMain->addStretch();
    if (showMinBtn) hLayoutMain->addWidget(btnMinimize);
    hLayoutMain->addWidget(btnClose);
    hLayoutMain->setContentsMargins(10, 0, 10, 0);
    hLayoutMain->setSpacing(5);

    setLayout(hLayoutMain);
    setStyleSheet("background-color: #333;");
}

void Banner::setupConnections()
{
    connect(btnClose, &QPushButton::clicked, this, &Banner::signalClose);
    connect(btnMinimize, &QPushButton::clicked, this, &Banner::signalMinimize);
}

void Banner::setTitle(const QString &title)
{
    lblTitle->setText(title);
}

void Banner::setShowMinimize(bool show)
{
    showMinBtn = show;
    btnMinimize->setVisible(show);
}

void Banner::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = true;
        dragPosition = event->globalPosition().toPoint() - parentWidget()->frameGeometry().topLeft();
        event->accept();
    }
}

void Banner::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging && (event->buttons() & Qt::LeftButton)) {
        parentWidget()->move(event->globalPosition().toPoint() - dragPosition);
        event->accept();
    }
}
