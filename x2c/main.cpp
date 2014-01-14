#include <QCoreApplication>
#include <QStringList>
#include <QFile>

#include <iostream>

#include "qxml2csv.h"

void printHelp(QString appName)
{
    std::cout << "Usage: " << appName.toStdString()
              << " <options> <xml inputfile> <csv outputfile>" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << " -h              Diplay this help." << std::endl;
    std::cout << " -s <character>  CSV column separator." << std::endl;
    std::cout << " -l <integer>    XML element nesting level to divide into rows." << std::endl;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Check argument count
    if (a.arguments().size() <= 1)
    {
        std::cerr << "Error: no operation specified (use -h for help)." << std::endl;
        return 1;
    }

    // Process arguments
    QString xmlFileName;
    QString csvFileName;
    QString csvSeperator = ",";
    int splitLevel = 1;

    for(int i = 1; i < a.arguments().size(); i+=2)
    {
        // Check for help request
        if(a.arguments().at(i).startsWith("-h") ||
           a.arguments().at(i+1).startsWith("-h"))
        {
            printHelp(a.arguments().at(0));
            return 2;
        }
        else
        {
            // Check for csvSeparator
            if(a.arguments().at(i).startsWith("-s"))
            {
                csvSeperator = a.arguments().at(i+1);
            }
            else
            {
                // Check for row separation level
                if(a.arguments().at(i).startsWith("-l"))
                {
                    bool* ok = new bool(false);
                    splitLevel = a.arguments().at(i+1).toInt(ok);
                    if(!ok)
                    {
                        std::cerr << "Error! The row separation number must be an integer. "
                                  << a.arguments().at(i+1).toStdString() << std::endl;
                        printHelp(a.arguments().at(0));
                        return 2;
                    }
                }
                else
                {
                    xmlFileName = a.arguments().at(i);
                    csvFileName = a.arguments().at(i+1);
                }
            }
        }
    }

    // Check if the files exist
    QFile xmlFile(xmlFileName);
    QFile csvFile(csvFileName);
    if(!xmlFile.exists())
    {
        std::cerr << "Error! File does not exist."
                  << xmlFileName.toStdString() << std::endl;
        printHelp(a.arguments().at(0));
        return 3;
    }
    /*
    if(csvFile.exists())
    {
        std::cerr << "Error! File exists, refusing to overwrite."
                  << csvFileName.toStdString() << std::endl;
        printHelp(a.arguments().at(0));
        return 4;
    }
    */


    std::cout << "INFO: Settings:" << std::endl;
    std::cout << "INFO:   XML\t" << xmlFileName.toStdString() << std::endl;
    std::cout << "INFO:   CSV\t" << csvFileName.toStdString() << std::endl;
    std::cout << "INFO:   sep\t" << csvSeperator.toStdString() << std::endl;
    std::cout << "INFO:   level\t" << splitLevel << std::endl;

    // All should be well, lets try to process this thing
    QXML2CSV xc;
    xc.parse(xmlFile,csvFile,splitLevel,csvSeperator);

    return 0;
}