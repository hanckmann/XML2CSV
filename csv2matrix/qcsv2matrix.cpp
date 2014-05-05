#include "qcsv2matrix.h"

#include <iostream>
#include <QDateTime>

/**
 * @brief QCSV2MATRIX::QCSV2MATRIX
 */
QCSV2MATRIX::QCSV2MATRIX() :
    noHeader(false),
    lastLine(false),
    unique(false),
    csvSeperator(";"),
    mtxSeperator(";"),
    minColumns(-1),
    mtxFileName(""),
    csvFileName("")
{
}

bool QCSV2MATRIX::convert(const QFile& sourceCSVFile, const QFile &sourceMTXFile, const QString& csvSeperator, const QString& mtxSeperator, const QList<int>& rawColumns, const QList<int>& ignoreColumns, const QList<int>& dtColumns, const QString& dtFormat = "", const int& minColumns = -1, const bool& noHeader = false, const bool& lastLine = false, const bool& unique = false)

{
    // Initialise CSV reader
    QFile csvFile(sourceCSVFile.fileName());
    if (!csvFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QTextStream csvStream(&csvFile);

    // Initialise MTX writer
    QFile mtxFile(sourceMTXFile.fileName());
    if(!mtxFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream mtxStream(&mtxFile);

    // Init variables
    int lineCount = 0;
    int uniqueValue = 0;
    QString prevLine;
    QString header;
    QList<QMap<QString, int>* > columnLegends;
    QList<int> copyColumns(rawColumns);
    copyColumns.append(dtColumns);

    // Loop through file
    while (!csvStream.atEnd())
    {
        QString line = csvStream.readLine();
        // Skip empty lines
        if(line.isEmpty())
            continue;

        // Process header
        if((lineCount == 0) && !lastLine && !noHeader)
        {
            header = line;
            ++lineCount;
            continue;
        }

        // Process previous line
        if(!prevLine.isEmpty())
        {
            QStringList columnList = prevLine.split(csvSeperator);
            QString mtxLine = "";
            int processed = 0;
            for(int index = 0; index < columnList.size(); ++index)
            {
                // Check if this is an ignoreColumn (if yes ignore)
                if(ignoreColumns.contains(index))
                    continue;

                QString columnValue = columnList.at(index);
                // Check if we need to extend the columnLegends
                if(processed >= columnLegends.size())
                {
                    QMap<QString, int> *msi = new QMap<QString, int>;
                    columnLegends.append(msi);
                    // Check if this should be a raw column
                    if(stringIsNumber(columnValue))
                    {
                        // Add this number in the rawColumns list
                        copyColumns.append(processed);
                    }
                }
                // Check if this is a raw column
                if(!copyColumns.contains(processed))
                {
                    if(!columnLegends.at(processed)->contains(columnValue))
                    {
                        QMap<QString, int> * tmpLegend = columnLegends[processed];
                        int value;
                        if(unique)
                        {
                            value = uniqueValue;
                            ++uniqueValue;
                        }
                        else
                        {
                            value = tmpLegend->size();
                        }
                        tmpLegend->insert(columnValue, value);
                    }
                    columnValue.setNum(columnLegends.at(processed)->value(columnValue));
                }
                // Check if this is a date-time column that should be converted to unix time
                if(dtColumns.contains(processed))
                {
                    QDateTime dt = QDateTime::fromString(columnValue, dtFormat);
                    columnValue = "";
                    columnValue = QString::number(dt.toMSecsSinceEpoch());
                }
                // The columnValue is prepared for the mtx
                if(processed > 0)
                    mtxLine += mtxSeperator;
                mtxLine += columnValue;
                ++processed;
            }
            // Extend the number of columns to satisfy minColumns
            for(;processed < minColumns; ++processed)
            {
                mtxLine += mtxSeperator;
            }
            // Write the line to file
            mtxStream << mtxLine << "\n";
        }

        // Go to next line
        prevLine = line;
        ++lineCount;
    }

    if(lastLine)
    {
        header = prevLine;
    }
    else
    {
        //process last line
        std::cout << "WARN: the current implementation does not process the last line with these settings!" << std::endl;
    }
    std::cout << "WARN: the current implementation does not adjust the header to remove ignored columns!" << std::endl;

    // Close the files neatly
    csvFile.close();
    mtxFile.close();

    // Serialise the legends and headers
    // Initialise Header writer
    QFile headerFile(sourceMTXFile.fileName() + ".header");
    if(!headerFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream headerStream(&headerFile);

    // Header and legend
    headerStream << "\\ Header and legend information for the matrix file: " << sourceMTXFile.fileName() << "\n";
    QStringList headerList = header.split(csvSeperator);
    QString newHeader;
    int processed = 0;
    for(int index = 0; index < headerList.size(); ++index)
    {
        // Check if this is an ignoreColumn (if yes ignore)
        if(ignoreColumns.contains(index))
            continue;

        headerStream << "Column " << processed << ": " << headerList.at(index) << "\n";
        headerStream << "Legend: \t";

        QMap<QString, int> *tmpLegend;
        tmpLegend = columnLegends.at(processed);
        QMap<QString, int>::const_iterator i = tmpLegend->constBegin();
        while (i != tmpLegend->constEnd()) {
            headerStream  << "\n" << "\t" << i.value() << "\t" << i.key();
            ++i;
        }
        headerStream  << "\n" ;
        ++processed;
    }

    headerFile.close();

    return true;
}

bool QCSV2MATRIX::stringIsNumber(const QString &str)
{
    bool ok;
    str.toDouble(&ok);
    return ok;
}

