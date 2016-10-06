#include <fstream>
#include <string>

#include <QDebug>

#include "Logging/logging.h"

#include "datacodes.h"

#include "parsefiles.h"
#include "parsefunctions.h"
#include "parsedirectory.h"

#include "QMLParse/qmldatacodes.h"          // include in parse functions


std::ifstream data_code_stream;

std::vector<std::string> directory_vec;
std::map<std::string, std::vector<std::string>> file_data_map;
std::vector<std::string> text_label_vec;
std::vector<std::string> text_response_vec;
std::vector<std::string> text_prompt_vec;
std::vector<std::string> dynamic_text_vec;


static const int qml_component_noID[44] = { 100, 101, 102, 103, 115, 119, 120, 121, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 150, 153, 154,
                                            160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 180, 181, 182, 183, 184 };

static const int qml_component_hasID[10] = { 110, 111, 112, 113, 114, 116, 117, 118, 151, 152 };

static const int qml_component_hasfile[18] = { 110, 111, 112, 113, 114, 115, 116, 117, 118, 122, 167, 169, 170, 171, 172, 173, 175, 176 };


DataCodes::DataCodes()
{

}

/*

  This needs to change to handle how the datafiles will be used
  most data files will independent with the directive type
  many of these processes will remain but, until choices are made, the data/lists will not be loaded
  this may mean the entire datafiles tree will need to be within each directive type which almost implies a "uiprocess" for each directive type
    - possibly a datacodes class for each directive_type
    - PrepareFileData() will likely not need to occur the way it currently does
    - PrepareFileData() may need a passed variable upon dir_type selection which will point to the proper directory
    - PrepDataDirectories() will only check for the directories and populate the dir_types
    - LoadFieldData() will also need to change

  likely need to iterate through all directive type directories in "PrepDataDirectories()" to check/create nec datafiles folders
    - this is going to be a problem with access unless all directories + filenames are enumerated somewhere, currently
    - option is to prep this as it is and use a same/similar function and pass dir_type to access dir_type directory
    - need a data_files_dir_vec to store the directories of the directive_type files
        - root + \\ParseFile\\QMLParse\\
        - root + dir_type_source_directory
        DirTypeSourceDirectory[%i] + DataDirectoryString[%i]


   will also need a cleanup function if a different dir_type is selected
*/

void DataCodes::StartupUIData()
{
    StartupDataDirectories();
    StartupFileData();
    StartupFieldData();

    Log.LogRecord(Logs::Normal, Logs::DataCode, "\t\t...file data successfully moved to internal memory.");
}

// this also prepares the directive_type directory list
void DataCodes::StartupDataDirectories()
{
    ParseDirectory PDir;

    std::string directory;

    for (int i = 0; i < MaxDirectiveType; ++i) {
        directory = root_directory + DirTypeSourceDirectory[i] + DataDirectoryString[DATA_FILES_DIR];
        if (PDir.BoostCheckPath(7, directory, "") == false) {
            PDir.MakeDirectory(directory);
        }

        directory = root_directory + DirTypeSourceDirectory[i] + DataDirectoryString[DATA_LISTS_DIR];
        if (PDir.BoostCheckPath(7, directory, "") == false) {
            PDir.MakeDirectory(directory);
        }

        directory = root_directory + DirTypeSourceDirectory[i] + DataDirectoryString[DATA_PROMPTS_DIR];
        if (PDir.BoostCheckPath(7, directory, "") == false) {
            PDir.MakeDirectory(directory);
        }

        directory = root_directory + DirTypeSourceDirectory[i] + DataDirectoryString[DATA_RESPONSES_DIR];
        if (PDir.BoostCheckPath(7, directory, "") == false) {
            PDir.MakeDirectory(directory);
        }
    }

    for (unsigned int i = 0; i < DataFileDirectories::MaxDataDirectory; ++i) {
        directory = root_directory + DataDirectoryString[i];
        directory_vec.push_back(directory);

        Log.LogRecord(Logs::High, Logs::DataCode, "StartupDataDirectories:\n %s", directory.c_str());
    }
}

void DataCodes::StartupFileData()
{
    std::string directory;
    std::string m_directory;
    std::string filename;
    std::string entry;

    int data_code = 1;

    directory = directory_vec.at(DATA_LISTS_DIR);
    filename = DataFileString[data_code];
    m_directory = directory + filename;
    data_code_stream.open(m_directory, std::ios_base::out | std::fstream::in);

    Log.LogRecord(Logs::High, Logs::DataCode, "StartupFileData:\n %s", m_directory.c_str());

    switch (data_code) {
    case DataCodeID::DIRECTIVE_TYPES_TXT:
        while (std::getline(data_code_stream, entry)) {
            directive_types_txt.push_back(entry);
        }
        break;
    }
    data_code_stream.close();
    Log.LogRecord(Logs::High, Logs::DataCode, "StartupFileData: size %i", directive_types_txt.size());

    file_data_map[DataFileString[DIRECTIVE_TYPES_TXT]] = directive_types_txt;
}

// each section will have a standard portion and a dir_type-specific portion - atm the standard portion is included in each dir_type file
// this might need a switch statement with a dir_type function call similar to what will be needed for PrepareFileData()
// see "LoadQMLFieldData()"
void DataCodes::StartupFieldData()
{
    std::string m_directory;
    std::string entry;

    m_directory = directory_vec.at(DATA_PROMPTS_DIR);
    m_directory.append("prompts.txt");
    data_code_stream.open(m_directory, std::ios_base::out | std::fstream::in);
    while (std::getline(data_code_stream, entry)) {
        text_prompt_vec.push_back(entry);
    }
    data_code_stream.close();
    Log.LogRecord(Logs::High, Logs::DataCode, "StartupFieldData:\n %s", m_directory.c_str());

    m_directory = directory_vec.at(DATA_RESPONSES_DIR);
    m_directory.append("responses.txt");
    data_code_stream.open(m_directory, std::ios_base::out | std::fstream::in);
    while (std::getline(data_code_stream, entry)) {
        text_response_vec.push_back(entry);
    }
    data_code_stream.close();
    Log.LogRecord(Logs::High, Logs::DataCode, "StartupFieldData:\n %s", m_directory.c_str());
}

void DataCodes::PrepUIData(int directive_type)
{
    PrepDataDirectories(directive_type);
    PrepareFileData(directive_type);
    LoadFieldData(directive_type);

    Log.LogRecord(Logs::Normal, Logs::DataCode, "\t\t...directive type file data successfully moved to internal memory.");
}

void DataCodes::PrepDataDirectories(int directive_type)
{
    std::string directory;

    directory_vec.clear();

    directory_vec.push_back(root_directory);

    for (unsigned int i = 1; i < DataFileDirectories::MaxDataDirectory; ++i) {
        directory = root_directory + DirTypeSourceDirectory[directive_type] + DataDirectoryString[i];
        directory_vec.push_back(directory);
        Log.LogRecord(Logs::High, Logs::DataCode, "PrepDataDirectories:\n %s", directory.c_str());
    }
}

void DataCodes::GetLabels() // F_17
{
    ParseFunctions PFunc;

    std::map<std::string, std::string> file_label_map;
    std::map<int, std::string> label_pos_map;

    std::map<std::string, std::string>::iterator m_it;
    std::map<int, std::string>::iterator mi_it;

    for (unsigned int i = 0; i < input_file_data_vec.size(); ++i) {
        PFunc.GetDelimitedData(input_file_data_vec.at(i), PFunc.PStr(506), 2);

        Log.LogRecord(Logs::High, Logs::DataCode, " file(%s) menu string(%s)", transfer_string_vec.at(0).c_str(), transfer_string_vec.at(1).c_str());
        label_pos_map[i] = transfer_string_vec.at(0);
        file_label_map[transfer_string_vec.at(0)] = transfer_string_vec.at(1);
        transfer_string_vec.clear();
    }
    for (mi_it = label_pos_map.begin(); mi_it != label_pos_map.end(); ++mi_it) {
        for (m_it = file_label_map.begin(); m_it != file_label_map.end(); ++m_it) {
            if (m_it->first == mi_it->second) {
                text_label_vec.push_back(m_it->second);
                Log.LogRecord(Logs::High, Logs::DataCode, "\tfile:(%s) menu string:(%s)", m_it->first.c_str(), m_it->second.c_str());
            }
        }
    }
    file_label_map.clear();
    label_pos_map.clear();
}

// datacodes/qmlparse, qmlparse, parsefile, qmlparsefunction, parsefunctions, uiprocess
void DataCodes::PrepUILabels(int directive_type)
{
    ParseFiles PF;

    QMLDataCodes QtDC;

    std::string in_path;

    text_label_vec.clear();

    in_path = root_directory + DataDirectoryString[DATA_FILES_DIR] + list_labels_file;
    PF.GetInputData(in_path);
    GetLabels();
    input_file_data_vec.clear();

    switch (directive_type) {
    case QT_QML:
        QtDC.PrepQMLUILabels();
        break;
    }
    input_file_data_vec.clear();
}


void DataCodes::PrepareFileData(int directive_type)
{
    QMLDataCodes QtDC;

    switch (directive_type) {
    case QT_QML:
        QtDC.PrepQMLFileData();
        break;
    }
}

void DataCodes::LoadFieldData(int directive_type)
{
    QMLDataCodes QtDC;

    std::string m_directory;
    std::string entry;

    Log.LogRecord(Logs::Med, Logs::DataCode, "LoadQMLFieldData:");

    text_prompt_vec.clear();
    dynamic_text_vec.clear();
    text_response_vec.clear();

    m_directory = directory_vec.at(0);
    m_directory.append(DataDirectoryString[DATA_PROMPTS_DIR]);
    m_directory.append("prompts.txt");
    data_code_stream.open(m_directory, std::ios_base::out | std::fstream::in);
    while (std::getline(data_code_stream, entry)) {
        text_prompt_vec.push_back(entry);
    }
    data_code_stream.close();
    Log.LogRecord(Logs::High, Logs::DataCode, "LoadQMLFieldData:\n %s", m_directory.c_str());

    m_directory = directory_vec.at(0);
    m_directory.append(DataDirectoryString[DATA_RESPONSES_DIR]);
    m_directory.append("responses.txt");
    data_code_stream.open(m_directory, std::ios_base::out | std::fstream::in);
    while (std::getline(data_code_stream, entry)) {
        text_response_vec.push_back(entry);
    }
    data_code_stream.close();
    Log.LogRecord(Logs::High, Logs::DataCode, "LoadQMLFieldData:\n %s", m_directory.c_str());

    switch (directive_type) {
    case QT_QML:
        QtDC.LoadQMLFieldData();
        break;
    }
}
