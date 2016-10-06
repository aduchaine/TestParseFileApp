#ifndef DATACODES_H
#define DATACODES_H


#include <map>
#include <vector>


#define M_FIELD_ID 11


enum DataCodeID { // these should eventually be programmed into the UI by as they change

    No_Data_Code = 0,
    DIRECTIVE_TYPES_TXT,
    MaxDataCode

};

static const char* DataFileString[DataCodeID::MaxDataCode] = { // this should be rewritten as relative path to main folder an other files can be added easier

    "no_vector",
    "directive_types.txt",

};

enum DataFileDirectories {

    No_Listing = 0,
    DATA_FILES_DIR,
    DATA_LISTS_DIR,
    DATA_PROMPTS_DIR,
    DATA_RESPONSES_DIR,
    MaxDataDirectory

};

static const char* DataDirectoryString[DataFileDirectories::MaxDataDirectory] = {

    "",
    "\\DataFiles\\",
    "\\DataFiles\\Lists\\",
    "\\DataFiles\\Prompts\\",
    "\\DataFiles\\Responses\\",

};

enum FieldCode {

    Empty_Field = 0,
    MaxFieldCode = M_FIELD_ID

};

enum ResponseCode {

    No_Response = 0,
    END_PROCESS,
    VARIABLE,
    DISTINCT,
    MaxResponseCode

};

static const char* ResponseString[ResponseCode::MaxResponseCode] = { // this isn't used

    "",
    "process_data",
    "variable response",
    "distinct response",

};

static const char * list_labels_file = "data_lists.txt";

class DataCodes
{
public:
    DataCodes();

    void StartupUIData();
    void StartupDataDirectories();
    void StartupFileData();
    void StartupFieldData();

    void PrepUIData(int directive_type);
    void PrepDataDirectories(int directive_type);

    void GetLabels();
    void PrepUILabels(int directive_type);

    void PrepareFileData(int directive_type);
    void LoadFieldData(int directive_type);

    std::vector<std::string> directive_types_txt;

};

extern std::ifstream data_code_stream;
extern std::vector<std::string> directory_vec;

extern std::map<std::string, std::vector<std::string>> file_data_map;
extern std::vector<std::string> text_response_vec; // this is all kinds of problems, should add a 0 in the first element or start everything at 0
extern std::vector<std::string> text_prompt_vec;
extern std::vector<std::string> dynamic_text_vec;

#endif // DATACODES_H
