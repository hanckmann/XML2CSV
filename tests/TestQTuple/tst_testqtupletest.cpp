#include <QString>
#include <QtTest>

#include <../../xml2csv/QTuple.h>

class TestQTupleTest : public QObject
{
    Q_OBJECT

public:
    TestQTupleTest();

private Q_SLOTS:
    void testCase1();
};

TestQTupleTest::TestQTupleTest()
{
}

void TestQTupleTest::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(TestQTupleTest)

#include "tst_testqtupletest.moc"
