#include <QtTest/QtTest>
#include "banner.h"

class TestBanner : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testTitle();
    void testMinimizeVisible();
    void testSignals();
    void cleanupTestCase();

private:
    Banner *banner;
    bool closed = false;
    bool minimized = false;
};

void TestBanner::initTestCase()
{
    banner = new Banner("Test Banner", true);
    connect(banner, &Banner::signalClose, this, [&]() { closed = true; });
    connect(banner, &Banner::signalMinimize, this, [&]() { minimized = true; });
}

void TestBanner::testTitle()
{
    banner->setTitle("Hello");
    QCOMPARE(banner->findChild<QLabel*>()->text(), QString("Hello"));
}

void TestBanner::testMinimizeVisible()
{
    banner->setShowMinimize(false);
    QVERIFY(!banner->findChild<QPushButton*>("btnMinimize"));
}

void TestBanner::testSignals()
{
    QPushButton* btnClose = banner->findChild<QPushButton*>("btnClose");
    QVERIFY(btnClose != nullptr);
    QSignalSpy spyClose(banner, &Banner::signalClose);

    btnClose->click();
    QCOMPARE(spyClose.count(), 1);

    banner->setShowMinimize(true);

    QPushButton* btnMinimize = banner->findChild<QPushButton*>("btnMin");
    QVERIFY(btnMinimize != nullptr);
    QSignalSpy spyMin(banner, &Banner::signalMinimize);

    btnMinimize->click();
    QCOMPARE(spyMin.count(), 1);
}


void TestBanner::cleanupTestCase()
{
    delete banner;
}

QTEST_MAIN(TestBanner)
#include "test_banner.moc"
