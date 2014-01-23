#include <QCoreApplication>
#include <QStringList>

#include <iostream>

#include <QFile>
#include <QTextStream>

void printHelp(QString appName)
{
    std::cout << "Usage: " << appName.toStdString()
              << " <xml filter tag> <xml inputfile> <xml outputfile>" << std::endl;
    std::cout << " <xml filter tag> provide the tag enclosing the information that must be filtered." << std::endl;
    std::cout << " <xml inputfile>  input XML file." << std::endl;
    std::cout << " <xml outputfile> output XML file (with the filtered tags)." << std::endl;
    std::cout << "Note that the output xml tags will be enclosed by a xmlcleanup tag." << std::endl;
}

void someOutput(const int &readCount, const int &writeCount)
{
    if(readCount%1000 == 0)
    {
        std::cout << "\r Reads: " << readCount << "      Writes: " << writeCount;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Check argument count
    if(a.arguments().size() < 4)
    {
        printHelp(a.applicationName());
        return 1;
    }

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

    // Add super enclosing open-tag
    sink << "<" << a.applicationName() << ">" << endl;

    // Start copying and filtering
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
    std::cout << "\r                                                            \r";

    // Add super enclosing close-tag
    sink << "</" << a.applicationName() << ">" << endl;

    // Close files
    sourceFile.close();
    sinkFile.close();

    // Some stats and Exit
    std::cout << "Read lines:    " << readLines << std::endl;
    std::cout << "Written lines: " << writtenLines << std::endl;
    double stat = (100.0 - ((100.0 / (double) readLines) * (double) writtenLines));
    std::cout << "Filtered out:  " << stat << " %" << std::endl;

    return 0;
}
