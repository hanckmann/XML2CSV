#include <iostream>
#include <QString>
#include <QVector>
#include <../../xml2csv/QTuple.h>

int main()
{
    int result = 0;

    // Test for int + equal operator
    int fi = 1;
    int si = 2;
    QTuple<int> ti(fi,si);
    if(ti.first() != fi)
        ++result;
    if(ti.second() != si)
        ++result;
    if(ti != ti)
        ++result;
    if(ti != ti)
        ++result;

    // Test for QString + equal operator
    QString fqs = "one";
    QString sqs = "two";
    QTuple<QString> tqs(fqs,sqs);
    if(! (tqs.first() == fqs))
        ++result;
    if(! (tqs.second() == sqs))
        ++result;
    if(tqs != tqs)
        ++result;

    // test unequal operator
    QTuple<int> ti2(2,3);
    if(! (ti != ti2))
        ++result;

    // Test if it works in a vector
    QVector< QTuple<int> > vti;
    vti.append(ti);
    vti.append(ti2);
    QTuple<int> t = vti.at(0);
    if(! (t == ti))
        ++result;

    std::cout << "QTuple errors: " << result << std::endl;
    return result;
}
