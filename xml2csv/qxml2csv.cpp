/**
 *Example in and output
 * This looks as follows:
 * <ELEMENT-LEVEL-0>
 *  <LEVEL-1>
 *		<LVL-2 attr1="attr-content-1" attr2="attr-content-2">level-2-data1</LVL-2>
 *		<SOMETHING>information1</SOMETHING>
 *	</LEVEL-1>
 *	<LEVEL-1>
 *		<LVL-2 attr1="attr-content-1" attr2="attr-content-2">level-2-data2</LVL-2>
 *		<SOMETHING>information2</SOMETHING>
 *		<MORE key1="name1-A" key2="data1-A">more-information</MORE>
 *		<MORE key1="name2-A" key2="data2-A">more-information</MORE>
 *		<MORE key1="name3-A" key2="data3-A">more-information</MORE>
 *	</LEVEL-1>
 *	<LEVEL-1>
 *		<LVL-2 attr1="attr-content-1" attr2="attr-content-2">level-2-data2</LVL-2>
 *		<MORE key1="name1-A" key2="data1-B">more-information</MORE>
 *		<MORE key1="name2-A" key2="data2-B">more-information</MORE>
 *		<MORE key1="name3-A" key2="data3-B">more-information</MORE>
 *	</LEVEL-1>
 *</ELEMENT-LEVEL-0>
 * Which results in:
 * attr-content-1,attr-content-2,level-2-data1,information1
 * attr-content-1,attr-content-2,level-2-data2,information2,name3-A,data3-A,more-information
 * attr-content-1,attr-content-2,level-2-data2,,name3-A,data3-B,more-information
 * LEVEL-1.LVL-2.attr1,LEVEL-1.LVL-2.attr2,LEVEL-1.LVL-2,LEVEL-1.SOMETHING,LEVEL-1.MORE.key1,LEVEL-1.MORE.key2,LEVEL-1.MOR
 */

#include "qxml2csv.h"

#include <iostream>
#include <QMap>

/**
 * @brief QXML2CSV::QXML2CSV
 */
QXML2CSV::QXML2CSV() :
    writeToScreen(false),
    rewrite(false),
    maxRows(0),
    csvSeparator(";")
{
}

/**
 * @brief QXML2CSV::parse
 * @param sourceXML
 * @param destinationCSV
 * @param splitLevel
 * @param maxRows
 * @return
 *
 * Starts parsing of XML to create the csv. The splitLevel provides the XML tag
 * level at which the row separation starts. The maxRows allows testing output
 * and stops the conversion process when the specified number of rows are
 * converted. If maxRows <= 0, the maxRows feature is disabled.
 */
bool QXML2CSV::parse(const QFile &sourceXML, const QFile &destinationCSV, const int &splitLevel, const int &maxRows = 0)
{
    // Initialise XML parser
    QFile xmlFile(sourceXML.fileName());
    QXmlInputSource inputSource(&xmlFile);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.setErrorHandler(this);

    // Initialise CSV writer
    csvFileName = destinationCSV.fileName();
    QString csvFileNameTemp = csvFileName;
    if(rewrite)
        csvFileNameTemp += ".tmp";
    QFile csvFile(csvFileNameTemp);
    if(!csvFile.open(QIODevice::ReadWrite))
        return false;
    csvStream.setDevice(&csvFile);

    // Initialise variables
    this->splitLevel = splitLevel;
    if(maxRows > 0)
        this->maxRows = maxRows;

    nameStack.clear();
    columnNames.clear();
    columnData.clear();
    currentLevel = 0;
    currentRow = 0;
    currentColumnCount = 0;

    // Parse
    bool result = reader.parse(inputSource);

    finalizeTable();

    return result;
}

/**
 * @brief QXML2CSV::setAttributeExpansion
 * @param attributeExpansion
 *
 * Enables attribute expansion for the provided attributes.
 * Attribute expansion converts two attribute values to a key-value pair (like
 * an element name and value).
 */
void QXML2CSV::setAttributeExpansion(const QStringList &attributeExpansion)
{
    for(int i = 0; i < attributeExpansion.size(); ++i)
    {
        QStringList aeList = attributeExpansion.at(i).split(",");
        if(aeList.size() == 2)
        {
            this->attributeExpansion.insert(aeList.at(0), aeList.at(1));
            this->attributeExpansionInverted.insert(aeList.at(1), aeList.at(0));
        }
        else
        {
            std::cerr << "Error: attribute expansion key-value pair invalid: "
                      << attributeExpansion.at(i).toStdString();
        }
    }
}

/**
 * @brief QXML2CSV::setCsvSeparator
 * @param csvSeparator
 *
 * Set the csv column separator character. Default: ;
 */
void QXML2CSV::setCsvSeparator(const QString &csvSeparator)
{
    this->csvSeparator = csvSeparator;
}

/**
 * @brief QXML2CSV::setWriteToScreen
 * @param writeToScreen
 *
 * Set to true if the output should also be written to the screen
 */
void QXML2CSV::setWriteToScreen(bool writeToScreen)
{
    this->writeToScreen = writeToScreen;
}

/**
 * @brief QXML2CSV::enableRewrite
 * @param rewrite
 *
 * If set to true, it ensures a fully qualified csv, meaning:
 * - all rows have commas to represent all columns
 * - the column names are written to the top of the csv file
 * Note that rewriting the csv file can be a slow and costly excersise.
 *
 * If set to false:
 * - not all column might be represented at every row
 * - the column names are written to the bottem of the csv file
 *
 * NOTE: This feature is not yet implemented!
 */
void QXML2CSV::enableRewrite(bool rewrite)
{
    this->rewrite = rewrite;
}

/**
 * @brief QXML2CSV::startElement
 * @param namespaceURI
 * @param localName
 * @param qName
 * @param attributes
 * @return
 *
 * Processes the element information and returns true.
 * If the XML tag level is higher than or equal to the splitLevel, than the
 * element attributes will be written added to the csv table and if the element
 * does not contain sub-elements the element information is also written to the
 * csv table.
 *
 * Importantly, this function keeps track of the XML tag level and the
 * information collected per row.
 */
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
            elementName = nameStack.top() + "." + elementName.replace(csvSeparator, "-");
        nameStack.push(elementName);
        //std::cout << currentLevel << " push:" << elementName.toStdString() << std::endl;

        QMap<QString, QString> tempAttrExp;
        QMap<QString, QString> tempAttrExpInvert;

        // Parse attributes if present
        for(int i = 0; i < attributes.length(); ++i)
        {
            QString attributeName  = elementName + "." + attributes.localName(i);
            QString attributeValue = attributes.value(i);

            // Check for attribute expansion - normal order
            if(attributeExpansion.contains(attributeName))
            {
                // Check if the VALUE has not already been read
                if(tempAttrExpInvert.contains(attributeName))
                {

                    // Find the KEY with the VALUE
                    QString tmpName = attributeName;
                    attributeName = elementName + "." + attributeValue;
                    attributeValue = tempAttrExpInvert.value(tmpName);
                    columnNames.add(attributeName);
                    columnData.insert(attributeName, attributeValue);
                }
                else
                {
                    // Store expanded name as KEY
                    tempAttrExp.insert(attributeExpansion.value(attributeName), elementName + "." + attributes.value(i));
                }
                continue;
            }
            if(tempAttrExp.contains(attributeName))
            {
                // Find the VALUE with the KEY
                attributeName = tempAttrExp.value(attributeName);
                columnNames.add(attributeName);
                columnData.insert(attributeName, attributeValue);
                continue;
            }

            // Check for attribute expansion - inverted order
            if(attributeExpansionInverted.contains(attributeName))
            {
                // Store the VALUE
                tempAttrExpInvert.insert(attributeExpansionInverted.value(attributeName), attributes.value(i));
                continue;
            }

            // Normal situation
            columnNames.add(attributeName);
            columnData.insert(attributeName, attributeValue);
        }
    }

    return true;
}

/**
 * @brief QXML2CSV::endElement
 * @param namespaceURI
 * @param localName
 * @param qName
 * @return
 *
 * Writes the collected (sub)element information to a row in the csv and returns
 * true.
 * Keeps track of the XML end tags such that the information of a csv row will
 * be written at the right moment.
 * See: write()
 */
bool QXML2CSV::endElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(localName);
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
        write();
        columnData.clear();
        if((maxRows) && (currentRow >= maxRows))
        {
            std::cerr << "Warning: Reached max number of rows." << std::endl;
            return false;
        }
    }

    --currentLevel;

    return true;
}

/**
 * @brief QXML2CSV::characters
 * @param str
 * @return
 *
 * Stores the information present between a start and end element.
 */
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
            columnData.insert(elementName, elementValue.replace(csvSeparator, "-"));

            //            std::cout << "element  :\t" << elementName.toStdString()
            //                      << "\t" << elementValue.toStdString() << std::endl;
        }
    }

    return true;
}

/**
 * @brief QXML2CSV::fatalError
 * @param exception
 * @return
 *
 * Presents parsing errors when they occur.
 */
bool QXML2CSV::fatalError(const QXmlParseException &exception)
{
    std::cerr << "error " << exception.message().toStdString() << std::endl;

    return true;
}

/**
 * @brief QXML2CSV::write
 *
 * Constructs the csv row from the data collected by the startElement() function
 * and writes the data to file and/or screen.
 */
void QXML2CSV::write()
{
    ++currentRow;

    // Build row string
    QString row;
    for(uint i = 0; i < columnNames.size(); ++i)
    {
        // Add separator (except at start of table)
        if(i>0)
            row.append(csvSeparator);

        // Add data if present
        QString key = columnNames.key(i);
        if(columnData.contains(key))
            row.append(columnData.value(key));
    }

    // Save column count when changed change
    if(currentColumnCount < columnNames.size())
    {
        //QTuple<int> ccc(currentRow, currentColumnCount);
        //columnCountChange.append(ccc);

        currentColumnCount = columnNames.size();
    }

    // Write string
    if(writeToScreen)
        std::cout << row.toStdString() << std::endl;
    csvStream << row << endl;
}

/**
 * @brief QXML2CSV::finalizeTable
 *
 * Finalises the csv file by adding the column headers.
 * If the rewrite functionality is required it will add the header information
 * at the start and copies all the followin rows under it (adding column
 * separation characters to make the csv table square).
 */
void QXML2CSV::finalizeTable()
{
    // Build table header
    QString tableHeader;
    for(uint i = 0; i < columnNames.size(); ++i)
    {
        // Add separator (except at start of table)
        if(i>0)
            tableHeader.append(csvSeparator);

        // Add data if present
        tableHeader.append(columnNames.key(i));
    }

    if(writeToScreen)
        std::cout << tableHeader.toStdString() << std::endl;


    if(!rewrite)
    {
        csvStream << tableHeader << endl;
    }
    else
    {
        QFile csvFile(csvFileName);
        if(!csvFile.open(QIODevice::WriteOnly))
            return;
        csvStream.setDevice(&csvFile);
    }
}
