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
    std::cout << " -r <integer>    Number of rows to be parsed (for testing the output)." << std::endl;
    std::cout << " -e <key1,value1;key2,value2>" << std::endl;
    std::cout << "                 XML attribut to be expanded as element in the csv." << std::endl;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationName("xml2csv");

    // Check argument count
    if (a.arguments().size() <= 1)
    {
        std::cerr << "Error: no operation specified (use -h for help)." << std::endl;
        return 1;
    }

    // Process arguments
    QString xmlFileName;
    QString csvFileName;
    QString csvSeperator = ";";
    QStringList attributeExpand;
    int maxRows = 0;
    int splitLevel = 1;

    for(int i = 1; i < a.arguments().size(); i+=2)
    {
        // Check for help request
        if(a.arguments().at(i).startsWith("-h") ||
                a.arguments().at(i+1).startsWith("-h"))
        {
            printHelp(a.applicationName());
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
                        printHelp(a.applicationName());
                        return 2;
                    }
                }
                else
                {
                    // Check for attribute expansion
                    if(a.arguments().at(i).startsWith("-e"))
                    {
                        bool* ok = new bool(false);
                        attributeExpand = a.arguments().at(i+1).split(";");
                        if(!ok)
                        {
                            std::cerr << "Error! Expansion argument invalid. "
                                      << a.arguments().at(i+1).toStdString() << std::endl;
                            printHelp(a.applicationName());
                            return 3;
                        }
                    }
                    else
                    {
                        // Check for max rows parameter
                        if(a.arguments().at(i).startsWith("-r"))
                        {
                            bool* ok = new bool(false);
                            maxRows = a.arguments().at(i+1).toInt(ok);
                            if(!ok)
                            {
                                std::cerr << "Error! Rows argument invalid. "
                                          << a.arguments().at(i+1).toStdString() << std::endl;
                                printHelp(a.applicationName());
                                return 4;
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
        }
    }

    // Check if the files exist
    QFile xmlFile(xmlFileName);
    QFile csvFile(csvFileName);
    if(!xmlFile.exists())
    {
        std::cerr << "Error! File does not exist."
                  << xmlFileName.toStdString() << std::endl;
        printHelp(a.applicationName());
        return 9;
    }


    std::cout << "INFO: Settings:" << std::endl;
    std::cout << "INFO:   XML\t" << xmlFileName.toStdString() << std::endl;
    std::cout << "INFO:   CSV\t" << csvFileName.toStdString() << std::endl;
    std::cout << "INFO:   sep\t" << csvSeperator.toStdString() << std::endl;
    std::cout << "INFO:   rows\t" << maxRows << std::endl;
    std::cout << "INFO:   level\t" << splitLevel << std::endl;

    // All should be well, lets try to process this thing
    QXML2CSV xc;
    xc.setWriteToScreen(false);
    xc.setCsvSeparator(csvSeperator);
    xc.setAttributeExpansion(attributeExpand);
    xc.parse(xmlFile,csvFile,splitLevel,maxRows);

    return 0;
}
