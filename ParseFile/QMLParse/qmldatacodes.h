#ifndef QMLDATACODES_H
#define QMLDATACODES_H


#include <vector>


enum QMLDataCodeID { // these should eventually be programmed into the UI by as they change

    QMLPROJECTS_TXT = 2,
    QMLDIRECTIVES_TXT,
    QMLMaxDataCode

};

static const char * QmlDataFileString[QMLDataCodeID::QMLMaxDataCode] = {

    "",
    "directive_types.txt",
    "qmlprojects.txt",
    "qmldirectives.txt",

};


class QMLDataCodes
{
public:
    QMLDataCodes();

    void LoadQMLFileData(std::string tar_directory);
    void PrepQMLUILabels();
    void PrepQMLFileData();
    void LoadQMLFieldData();

    std::vector<std::string> qmlprojects_txt;
    std::vector<std::string> qmldirectives_txt;

};

#endif // QMLDATACODES_H
