#include <fstream>
#include <string>

#include <QDebug>

#include "Logging/logging.h"

#include "qmldatacodes.h"

#include "datacodes.h"

#include "parsefiles.h"


QMLDataCodes::QMLDataCodes()
{

}

// do the same as "DATA_PROMPTS_DIR" for "DATA_RESPONSES_DIR" when the responses evolve and put it in datacodes.cpp to be processed prior to this function
void QMLDataCodes::LoadQMLFieldData()
{
    std::string m_directory;
    std::string entry;

    m_directory = directory_vec.at(DATA_PROMPTS_DIR);
    m_directory.append("dynamic_text.txt");
    data_code_stream.open(m_directory, std::ios_base::out | std::fstream::in);
    while (std::getline(data_code_stream, entry)) {
        dynamic_text_vec.push_back(entry);
    }
    data_code_stream.close();
    Log.LogRecord(Logs::High, Logs::DataCode, "LoadQMLFieldData:\n %s", m_directory.c_str());

    m_directory = directory_vec.at(DATA_RESPONSES_DIR);
    m_directory.append("responses.txt");
    data_code_stream.open(m_directory, std::ios_base::out | std::fstream::in);
    while (std::getline(data_code_stream, entry)) {
        text_response_vec.push_back(entry);
    }
    data_code_stream.close();
    Log.LogRecord(Logs::High, Logs::DataCode, "LoadQMLFieldData:\n %s", m_directory.c_str());
}

void QMLDataCodes::LoadQMLFileData(std::string tar_directory)
{
    std::string m_directory;
    std::string filename;
    std::string entry;

    int data_code = 0;

    qmlprojects_txt.clear();
    qmldirectives_txt.clear();

    Log.LogRecord(Logs::High, Logs::DataCode, " LoadQMLFileData: directory %s", tar_directory.c_str());

    for (unsigned int i = 2; i < QMLMaxDataCode; ++i) {
        filename = QmlDataFileString[i];
        data_code = i;

        m_directory = tar_directory + filename;
        data_code_stream.open(m_directory, std::ios_base::out | std::fstream::in);

        switch (data_code) {
        case QMLPROJECTS_TXT:
            while (std::getline(data_code_stream, entry)) {
                qmlprojects_txt.push_back(entry);
                Log.LogRecord(Logs::High, Logs::DataCode, " LoadQMLFileData: QMLPROJECTS_TXT %s", entry.c_str());
            }
            break;
        case QMLDIRECTIVES_TXT:
            while (std::getline(data_code_stream, entry)) {
                qmldirectives_txt.push_back(entry);                
                Log.LogRecord(Logs::High, Logs::DataCode, " LoadQMLFileData: QMLDIRECTIVES_TXT %s", entry.c_str());
            }
            break;
        }
        data_code_stream.close();
    }
}

void QMLDataCodes::PrepQMLUILabels()
{
    DataCodes DC;
    ParseFiles PF;

    std::string in_path;

    in_path = directory_vec.at(1) + list_labels_file;
    PF.GetInputData(in_path);
    DC.GetLabels();    
}

void QMLDataCodes::PrepQMLFileData()
{
    LoadQMLFileData(directory_vec.at(DATA_LISTS_DIR));

    file_data_map[QmlDataFileString[QMLPROJECTS_TXT]] = qmlprojects_txt;
    file_data_map[QmlDataFileString[QMLDIRECTIVES_TXT]] = qmldirectives_txt;
    Log.LogRecord(Logs::High, Logs::DataCode, "\tqmlprojects_txt:(%i)", qmlprojects_txt.size());
    Log.LogRecord(Logs::High, Logs::DataCode, "\tqmldirectives_txt:(%i)", qmldirectives_txt.size());
}
