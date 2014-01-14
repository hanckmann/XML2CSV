#include <iostream>
#include <QString>
#include <QVector>
#include <../../xml2csv/QIndexMap.h>

int main()
{
    int result = 0;

    // Test for int
    QIndexMap<uint> imi;
    uint testCount = 5;
    for(uint i = 0; i < testCount; ++i)
    {
        imi.add(i);
    }
    for(uint i = 0; i < testCount; ++i)
    {
        if(! (imi.key(i) == i))
            ++result;
        if(! (imi.key(i) == imi.index(i)))
            ++result;
    }

    // Test for QString
    QIndexMap<QString> ims;
    QString one = "one";
    QString two = "two";
    QString three = "three";
    ims.add(one);
    ims.add(two);
    ims.add(three);
    if(! (ims.key(0) == one))
        ++result;
    if(! (ims.key(1) == two))
        ++result;
    if(! (ims.key(2) == three))
        ++result;
    if(! (ims.index(one) == (uint)0))
        ++result;
    if(! (ims.index(two) == (uint)1))
        ++result;
    if(! (ims.index(three) == (uint)2))
        ++result;

    // Test if it works in a vector
    QIndexMap<uint> imi2;
    testCount = 3;
    for(uint i = 0; i < testCount; ++i)
        imi2.add(i);
    QVector< QIndexMap<uint> > vimi;
    vimi.append(imi);
    vimi.append(imi2);
    QIndexMap<uint> t = vimi.at(0);
    if(! (t.index(1) == imi2.index(1)))
        ++result;

    std::cout << "QIndexMap errors: " << result << std::endl;
    return result;
}
