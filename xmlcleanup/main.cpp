#include <QCoreApplication>
#include <QStringList>

#include <iostream>

#include <QFile>
#include <QTextStream>

void someOutput(const int &readCount, const int &writeCount)
{
    if(readCount%1000 == 0)
    {
        std::cout << "\r Reads: " << readCount << "\t Writes: " << writeCount;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString filterTag = a.arguments().at(1);
    QString sourceFileName = a.arguments().at(2);
    QString sinkFileName = a.arguments().at(3);

    // Info
    std::cout << "Input file:  " << sourceFileName.toStdString() << std::endl;
    std::cout << "Output file: " << sinkFileName.toStdString() << std::endl;
    std::cout << "Filter tag:  " << filterTag.toStdString() << std::endl;

    // Init
    bool inFilterTag = false;
    QString startTag = "<" + filterTag;
    QString endTag = "</" + filterTag;

    int readLines = 0;
    int writtenLines = 0;

    // Open source
    QFile sourceFile(sourceFileName);
    if (!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text))
       return 1;

    // Open sink
    QFile sinkFile(sinkFileName);
    if (!sinkFile.open(QIODevice::WriteOnly | QIODevice::Text))
       return 2;

    // Files are open
    QString line;
    QString lineSimplified;
    QTextStream source(&sourceFile);
    QTextStream sink(&sinkFile);
    while (!source.atEnd())
    {
        ++readLines;
        line = source.readLine();
        lineSimplified = line.simplified().replace(" ","");
        if(!inFilterTag)
        {
            if(lineSimplified.startsWith(startTag))
            {
                inFilterTag = true;
                ++writtenLines;
                sink << line << endl;
            }
        }
        else
        {
            ++writtenLines;
            sink << line << endl;
            if(lineSimplified.startsWith(endTag))
            {
                inFilterTag = false;
            }
        }
        someOutput(readLines, writtenLines);
    }
    std::cout << std::endl;

    // Close files
    sourceFile.close();
    sinkFile.close();

    // Some stats and Exit
    std::cout << "Read lines:    " << readLines << std::endl;
    std::cout << "Written lines: " << writtenLines << std::endl;

    return 0;
}
