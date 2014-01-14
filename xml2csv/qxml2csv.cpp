#include "qxml2csv.h"

#include <iostream>
#include <QMap>

#include <qindexmap.h>

/**
 * @brief QXML2CSV::QXML2CSV
 */
QXML2CSV::QXML2CSV()
{
}

bool QXML2CSV::parse(const QFile &sourceXML, const QFile &destinationCSV, const int &splitLevel, const QString &csvSeparator)
{
    // Initialise XML parser
    QFile xmlFile(sourceXML.fileName());
    QXmlInputSource inputSource(&xmlFile);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.setErrorHandler(this);

    // Initialise variables
    this->splitLevel = splitLevel;
    this->csvSeparator = csvSeparator;
    this->destinationCSV.setFileName(destinationCSV.fileName());

    nameStack.clear();
    columnNames.clear();
    columnData.clear();
    currentLevel = 0;

    // Parse
    return reader.parse(inputSource);
}

bool QXML2CSV::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &attributes)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(qName);
    ++currentLevel;

    // Prepare column information for 1 row
    if(currentLevel >= splitLevel)
    {
        QString elementName = localName;
        if(!nameStack.isEmpty())
            elementName = nameStack.top() + "." + elementName;
        nameStack.push(elementName);
        //std::cout << currentLevel << " push:" << elementName.toStdString() << std::endl;

        // Parse attributes if present
        for(int i = 0; i < attributes.length(); ++i)
        {
            QString attributeName  = elementName + "." + attributes.localName(i);
            QString attributeValue = attributes.value(i);
            columnNames.add(attributeName);
            columnData.insert(attributeName, attributeValue);

            std::cout << "attribute:\t" << attributeName.toStdString()
                      << "\t" << attributeValue.toStdString() << std::endl;
        }
    }

    return true;
}

bool QXML2CSV::endElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(qName);

    // Store the elements data
    if(currentLevel >= splitLevel)
    {
        if(!nameStack.isEmpty())
        {
            QString pop = nameStack.pop();
            //std::cout << currentLevel << " pop :" << pop.toStdString() << std::endl;
        }
        else
        {
            std::cerr << "Stack empty ?!?" << std::endl;
        }

        //std::cout << "ee " << localName.toStdString() << "\t" << currentLevel << std::endl;
    }

    // Create the row
    if(currentLevel == splitLevel)
    {
        printRowToScreen();
    }

    --currentLevel;

    return true;
}

bool QXML2CSV::characters(const QString &str)
{
    if(currentLevel >= splitLevel)
    {
        // Parse the elements data
        QString elementName = nameStack.top();
        QString elementValue = str;

        if(!elementValue.simplified().replace(" ","").isEmpty())
        {
            columnNames.add(elementName);
            columnData.insert(elementName, elementValue);

            std::cout << "element  :\t" << elementName.toStdString()
                      << "\t" << elementValue.toStdString() << std::endl;
        }
    }

    return true;
}

bool QXML2CSV::fatalError(const QXmlParseException &exception)
{
    std::cout << "error " << exception.message().toStdString() << std::endl;

    return true;
}

void QXML2CSV::printRowToScreen()
{

}
