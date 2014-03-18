#include <QCoreApplication>
#include <QStringList>
#include <QList>
#include <QFile>

#include <iostream>

void printHelp(QString appName)
{
    std::cout << "Usage: " << appName.toStdString()
              << " <options> -i <csv inputfile>" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << " -h               diplay this help." << std::endl;
    std::cout << " -n               should be provided if the csv file does not provide header information." << std::endl;
    std::cout << " -l               should be provided if the last line contains the column description." << std::endl;
    std::cout << " -u               should be provided if all ids representing text in the csv should be unique." << std::endl;
    std::cout << " -c <char>        define csv separation token (default=';')." << std::endl;
    std::cout << " -m <char>        define matrix separation token (default=';')." << std::endl;
    std::cout << " -d <integer>     define minum number of columns in the matrix, if there are less columns, empty " << std::endl;
    std::cout << "                  values will be added, but if there are more columns those will NOT be omitted." << std::endl;
    std::cout << " -r <int_1 int_n> define the columns that will be copied without processing (i.e. columns with " << std::endl;
    std::cout << "                  date information). This should be a space separated list of column numbers " << std::endl;
    std::cout << "                  where the first column is 0 (zero)." << std::endl;
    std::cout << " -o <outputfile>  define the output matrix filename." << std::endl;
    std::cout << "Required:" << std::endl;
    std::cout << " -i <inputfile>   define the input csv filename." << std::endl;
}

QString b2s(const bool& b)
{
    if(b)
        return QString("true");
    else
        return QString("false");
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationName("csv2matrix");

    // Check argument count
    if (a.arguments().size() <= 1)
    {
        std::cerr << "Error: no operation specified (use -h for help)." << std::endl;
        return 1;
    }

    // Process arguments
    bool noHeader = false;
    bool lastLine = false;
    bool unique = false;
    QString csvSeperator = ";";
    QString mtxSeperator = ";";
    int minColumns = -1;
    QList<int> rawColumns;
    QString mtxFileName;
    QString csvFileName;

    for(int i = 1; i < a.arguments().size(); i++)
    {
        // Check if this is a argument identifier
        if(!a.arguments().at(i).startsWith("-") )
            continue;

        bool isLastArgument = false;
        if(i >= a.arguments().size())
            isLastArgument = true;

        // Check for help request
        if(a.arguments().at(i).startsWith("-h"))
        {
            printHelp(a.applicationName());
            return 2;
        }
        // Check for no_header
        else if(a.arguments().at(i).startsWith("-n"))
        {
            noHeader = true;
        }
        // Check for last_line
        else if(a.arguments().at(i).startsWith("-l"))
        {
            lastLine = true;
        }
        // Check for unique
        else if(a.arguments().at(i).startsWith("-u"))
        {
            unique = true;
        }
        // Check for csvSeparator
        else if(a.arguments().at(i).startsWith("-c"))
        {
            if(!isLastArgument)
            {
                csvSeperator = a.arguments().at(i+1);
            }
            else
            {
                printHelp(a.applicationName());
                return 3;
            }
        }
        // Check for mtxSeperator
        else if(a.arguments().at(i).startsWith("-m"))
        {
            if(!isLastArgument)
            {
                mtxSeperator = a.arguments().at(i+1);
            }
            else
            {
                printHelp(a.applicationName());
                return 4;
            }
        }
        // Check for minColumns
        else if(a.arguments().at(i).startsWith("-d"))
        {
            if(!isLastArgument)
            {
                bool ok;
                minColumns = a.arguments().at(i+1).toInt(&ok);
                if(!ok)
                {
                    std::cerr << "Error! -d <int> argument invalid. "
                              << a.arguments().at(i+1).toStdString() << std::endl;
                    printHelp(a.applicationName());
                    return 5;
                }
            }
            else
            {
                printHelp(a.applicationName());
                return 6;
            }
        }
        // Check for rawColumns
        else if(a.arguments().at(i).startsWith("-r"))
        {
            if(!isLastArgument)
            {
                for(int j = i+1; j < a.arguments().size(); j++)
                {
                    bool ok;
                    int c = a.arguments().at(j).toInt(&ok);
                    if(!ok)
                    {
                        break;
                    }
                    else
                    {
                        rawColumns.append(c);
                    }
                }
            }
            else
            {
                printHelp(a.applicationName());
                return 8;
            }
        }
        // Check for mtxFileName
        else if(a.arguments().at(i).startsWith("-o"))
        {
            if(!isLastArgument)
            {
                mtxFileName = a.arguments().at(i+1);
            }
            else
            {
                printHelp(a.applicationName());
                return 9;
            }
        }
        // Check for csvFileName
        else if(a.arguments().at(i).startsWith("-i"))
        {
            if(!isLastArgument)
            {
                csvFileName = a.arguments().at(i+1);
            }
            else
            {
                printHelp(a.applicationName());
                return 10;
            }
        }
        else
        {
            std::cout << a.arguments().at(i).toStdString() << std::endl;
            printHelp(a.applicationName());
            return 11;
        }
    }

    // Generate mtxFileName if not provided as argument
    if(mtxFileName.isEmpty())
    {
        mtxFileName = csvFileName.left(csvFileName.size()-4) + ".matrix";
    }

    // Check if the files exist
    QFile csvFile(csvFileName);
    QFile mtxFile(mtxFileName);
    if(!csvFile.exists())
    {
        std::cerr << "Error! File does not exist."
                  << csvFileName.toStdString() << std::endl;
        printHelp(a.applicationName());
        return 12;
    }

    std::cout << "INFO: Settings:" << std::endl;
    std::cout << "INFO:   noHeader:\t" << b2s(noHeader).toStdString() << std::endl;
    std::cout << "INFO:   lastLine:\t" << b2s(lastLine).toStdString() << std::endl;
    std::cout << "INFO:   unique:\t\t" << b2s(unique).toStdString() << std::endl;
    std::cout << "INFO:   csvSeperator:\t" << csvSeperator.toStdString() << std::endl;
    std::cout << "INFO:   mtxSeperator:\t" << mtxSeperator.toStdString() << std::endl;
    std::cout << "INFO:   minColumns:\t" << minColumns << std::endl;
    std::cout << "INFO:   rawColumns:\t";
        for(int i = 0; i < rawColumns.size(); ++i)
        {
            if(i > 0)
            {
                std::cout << ", ";
            }
            std::cout << rawColumns.at(i);
        }
        std::cout << std::endl;
    std::cout << "INFO:   mtxFileName\t" << mtxFileName.toStdString() << std::endl;
    std::cout << "INFO:   csvFileName\t" << csvFileName.toStdString() << std::endl;

    // All should be well, lets try to process this thing
    return 0;
}
