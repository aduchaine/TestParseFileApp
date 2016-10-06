#include <iostream>
#include <vector>
#include <map>
#include <fstream>

#include <QDebug>

#include "Logging/logging.h"

#include "uiprocess.h"
#include "datacodes.h"

#include "parsefiles.h"
#include "parsedirectory.h"

#include "QMLParse/qmldatacodes.h"
#include "QMLParse/qmluiprocess.h"


static const int componentID_from_main[25] = { 160, 161, 162, 163, 164, 174, 175, 176, 172, 153, 169, 170, 150, 154, 171, 150, 165, 180, 183, 181, 182, 184, 166, 167, 168 };

static const int componentID_from_pageID[32][2] = {
//                    Page:
//     0,   1,   2,   3,   4,   5,   6,   7

    { 100, 100 },
    { 101, 101 },
    { 102, 102 },
    { 110, 110 },
    { 111, 111 },
    { 111, 110 },
    { 110, 151 },
    { 119, 113 },
    { 132, 113 },
    { 131, 113 },
    { 130, 150 },
    { 133, 110 },
    { 121, 151 },
    { 115, 112 },
    { 103, 116 },
    { 999, 116 },
    {  -1, 150 },
    {  -1, 110 },
    {  -1, 119 },
    {  -1, 134 },
    {  -1, 132 },
    {  -1, 131 },
    {  -1, 130 },
    {  -1, 133 },
    {  -1, 137 },
    {  -1, 136 },
    {  -1, 135 },
    {  -1, 121 },
    {  -1, 115 },
    {  -1, 122 },
    {  -1, 103 },
	{  -1, 999 },

};

// many of these could be created as the components within the page are created above if the component has a standard use
// responseID and fieldID would be more difficult but possible if the app data is present
// fieldID 1-6 - 1st pos accounted in program logic
static const int fieldID_to_pageID[7]          = { 0, 1, 1, 1, 1, 1, 1 };
static const int fieldID_to_num_responses[7]   = { 0, 1, 1, 1, 1, 1, 1 };

// boolean value - if false(0) it's a dynamic field
static const int defaultActiveField[]          = { 0, 1, 0, 0, 0, 0, 0 };

// related to above
static const int fieldID_to_datacodeID[]       = { 0, 1, 0, 0, 0, 0, 0 };
static const int datacodeID_to_fieldID[]       = { 0, 1 };


static const int fieldID_to_confirmID[]        = { 0, 5, 5, 5, 2, 3, 3 };

static const int confirmID_to_switch[]         = { 0, 0, 0, 1, 0, 1 };  // confirmID 1-5 - flag(1) allows multiple responses per page

static const int pageID_to_num_inputfields[]   = { 0, 6 };              // pageID 0-1
static const int pageID_to_num_textfields[]    = { 2, 1 };              // pageID 0-1

static const int fieldID_from_pageID[6][2] = {
//    Page:
//    0, 1

    { 0, 1 },    // 1st FieldID of page
    { 0, 2 },    // 2nd FieldID of page
    { 0, 3 },    // 3rd FieldID of page
    { 0, 4 },    // 4th FieldID of page
    { 0, 5 },    // 5th FieldID of page
    { 0, 6 },    // 6th FieldID of page
};

static const int textID_from_pageID[2][2] = {
//    Page:
//    0, 1

    { 1, 3 },       // 1st TextID of page
    { 2, 0 },       // 2nd TextID of page

};

// boolean value
static const int dynamicTextField[]      = { 0, 0, 0, 1 };

static const int responseID_from_fieldID[1][7] = {
//      FieldID:
//    0, 1, 2, 3, 4, 5, 6

    { 0, 3, 3, 3, 2, 4, 5 },   // 1st responseID of field

};

// much of this will need to go into the directive_type
typedef std::vector<int>x_dim;
typedef std::vector<x_dim>y_dim;
typedef std::vector<y_dim>z_dim;

const size_t directive_types(MaxDirectiveType);
const size_t maximum_directives(MAX_DIRECTIVES);

std::vector<int> projects_vec;

// most of this can remain here
std::map<int, int> field_emit_map;
std::vector<int> fieldID_vec;
std::vector<int> textfieldID_vec;
std::vector<int> responseID_vec;
std::vector<int> dynamicTextID_vec;

std::map<int, int> response_map;
//std::map<float, std::string> response_map;
std::map<int, std::string> string_map;
std::map<float, std::string> user_data_map;

std::string username;
std::string f_name;
std::string l_name;

int cur_dir_type = 0;
int cur_directive = 0;

bool startup = false;
bool prompt_set = false;
bool rebuild = false;


UIProcess::UIProcess(QObject *parent) : QObject(parent)
{

}

QString UIProcess::ps_data1() const
{
    return s_data1;
}

void UIProcess::setS_data1(const QString &ps_data1)
{
    s_data1 = ps_data1;
}

int UIProcess::pi_data1() const
{
    return i_data1;
}

void UIProcess::setI_data1(const int &pi_data1)
{
    i_data1 = pi_data1;
}

int UIProcess::pi_data2() const
{
    return i_data2;
}

void UIProcess::setI_data2(const int &pi_data2)
{
    i_data2 = pi_data2;
}

int UIProcess::pi_data3() const
{
    return i_data3;
}

void UIProcess::setI_data3(const int &pi_data3)
{
    i_data3 = pi_data3;
}

bool UIProcess::pb_data1() const
{
    return b_data1;
}

void UIProcess::setB_data1(const bool &pb_data1)
{
    b_data1 = pb_data1;
}

void UIProcess::ProcessStart()
{
    DataCodes DC;

    Log.DefaultLogSettings();
    Log.CheckLogFiles();
    DC.StartupUIData();

    startup = true;
    parseSelf(true, false);
}

void UIProcess::getPreviousPage(int current_page)
{
    int previous_page = 0;

    Log.LogRecord(Logs::High, Logs::UILog, "getPreviousPage: current_page %i", current_page);
    if (current_page == 0) {
        return;
    }
    if (current_page > 0) {
        previous_page = current_page - 1;
    }
    Log.LogRecord(Logs::High, Logs::UILog, "getPreviousPage: previous_page %i", previous_page);

    emit previousPage(previous_page);
}

void UIProcess::processClick()
{
    if (i_data1 < 1) { // responseID (ie. menu choice)
        i_data1 = 0;
    }
    if (i_data2 < 1) { // fieldID
        i_data2 = 0;
    }

    DataCodes DC;

    std::map<int, int>::iterator m_it;

    int confirm_status = 0;

    bool replace = false;
    bool reset = false;

    confirm_status = GetConfirmID(i_data2);
    replace = ReplacePrevData(confirm_status);

    Log.LogRecord(Logs::Med, Logs::UILog, "processClick: replace(%s) confirm_status %i fieldID %i  ", Log.DbgSwtch(replace).c_str(), confirm_status, i_data2);
    if (replace == true) {
        reset = RemoveResponse(i_data2);
    }
    else { // for multiple answers to one field
        Log.LogRecord(Logs::High, Logs::UILog, "processClick insert %i", i_data1);
        m_it = response_map.find(i_data2);

        if (m_it != response_map.end()) {
            response_map.erase(m_it);
            Log.LogRecord(Logs::High, Logs::UILog, "processClick insert-erase same %i", i_data1);
        }
        else {
            response_map[i_data2] = i_data1;
            Log.LogRecord(Logs::High, Logs::UILog, "processClick insert-add new %i", i_data1);
        }
    }
    if (reset == true) {
        response_map[i_data2] = i_data1;
        Log.LogRecord(Logs::Med, Logs::UILog, "processClick: reset -> response %i field %i ", i_data1, i_data2);
    }

    if (i_data2 == 1) {
        if (cur_dir_type != i_data1) {
            cur_dir_type = i_data1;

            DC.PrepUIData(cur_dir_type);
            DC.PrepUILabels(cur_dir_type);
            PrepDirTypeArrayData(cur_dir_type);
            PrepDynamicFieldData(false);

            Log.LogRecord(Logs::High, Logs::Test, "processClick: set cur_dir_type %i");
        }
    }
    ProcessDynamicMenu(true, i_data2);
    ProcessDynamicText(true, i_data2);
    i_data1 = 0;
    i_data2 = 0;
}

/*
- confirm_status == 1:
    - iterate and analyze map data to make sure each field has an entry - probably not necessary
        - goto page with bad data - assure "next" will work and possibly make another button to skip linear navigation
        - probably not necessary but is great for learning and review options
    - an array or some other type of structure(list) combined with a string-response map loaded on startup is likely all that's needed to check against and process
*/
// this could be considered a main processing loop
void UIProcess::confirmData(int page_id)
{
    /*
     confirm_status = 1 -> final data processing
     confirm_status = 2 -> string data, single page(currently)
     confirm_status = 3 -> mouse/button clicks, single page, one answer required
     confirm_status = 4 -> mouse/button clicks, single page, one answer required, more than one answer acceptable
     confirm_status = 11+ -> mouse/button clicks, single page, multiple answers required, multiple fields
    */

//    ProcessData Pdata;
    int min_page_processes = 0;
    int confirm_status = 0;
    int page_processes = 0;
    int field = 0;
    int q_counter = 0;

    if (page_id < 0) { // this may cause problems with other processing - main_update.qml implementation
        return;
    }

    Log.LogRecord(Logs::High, Logs::UILog, "confirmData: BEGIN DO page_processes & GetFieldID ");

    min_page_processes = GetMinPageProcesses(page_id);
    page_processes = GetPageProcesses(page_id);
    GetFieldID(page_id);

    for (unsigned int i = 0; i < fieldID_vec.size(); i++) {
        field = fieldID_vec.at(i);
        confirm_status = GetConfirmID(field);

        if (confirm_status == Question::STRING_VALIDATE) { // currently for name strings
            ++q_counter;
            Log.LogRecord(Logs::High, Logs::UILog, "confirmData: ValidateString->field_id %i count %i", field, q_counter);
            if (ValidateString(field) == false) {
                emit invalidData();
                break;
            }
            // for username only
            if (q_counter == 2) { // was "else if"
                emit validData(page_id + 1);
                if (username != l_name + ", " + f_name) {
                    if (username.length() >= 4) {
                        username = l_name + ", " + f_name;
                        Log.LogRecord(Logs::Normal, Logs::UILog, "[%s] edited name.", username.c_str());
                    }
                    else {
                        username = l_name + ", " + f_name;
                        Log.LogRecord(Logs::Normal, Logs::UILog, "[%s] has begun the session.", username.c_str());

                    }
                }
            }
        }
        // a new function similar to processClick() will likely need to be made to handle the extra variable in the chosen answer
        // the variables are page number, required responses per page, field id, response id and now data codes (fow larger possible definitive responses)
        // adding this section changes this part of program and I'll need to organize how it works differently based on this - mostly done
        // each page will require a different number of required responses (page_processes) - this funcrion need to be done yet, some of it's done
        // pages with multiple fields will need to check all fields before emitting a signal
        // same as -> (confirm_status == Question::SINGLE_REQ || confirm_status == Question::SINGLE_ACC || confirm_status == Question::MULTIPLE_REQ)
        else if (confirm_status > 2) {
            //++q_counter;

            Log.LogRecord(Logs::Med, Logs::UILog, "confirmData: page_id: %i  page_processes (act: %i req : %i)", page_id, page_processes, min_page_processes);
            if (page_processes < min_page_processes) {
                Log.LogRecord(Logs::High, Logs::UILog, "confirmData: noData page_id: %i", page_id);
                emit noData();
                break;
            }
            else {
                Log.LogRecord(Logs::High, Logs::UILog, "confirmData: validData page_id: %i", page_id);
                emit validData(page_id + 1);
            }
        }
    }

    if (confirm_status > 2) {
        ProcessMap();
    }

    if (confirm_status == Question::PROCESS_DATA) { // final process
        Log.LogRecord(Logs::High, Logs::UILog, "confirmData: PROCESS_DATA %i %i", page_processes, min_page_processes);
        if (page_processes == min_page_processes) {
            //ProcessStringMap();
            emit processedData();
        }
        else {
            emit noData();
        }
    }
    // "BeginPage" confirmation
    else if (page_id == 0) {
        Log.LogRecord(Logs::High, Logs::UILog, "confirmData: curpage BeginPage");
        emit validData(page_id + 1);
    }
    fieldID_vec.clear();
    Log.LogRecord(Logs::High, Logs::UILog, "confirmData: END");
}

void UIProcess::checkStringInput()
{
    std::string string_data;
    string_data = s_data1.toUtf8().constData(); // this also works -> <namestring = m_data.toLocal8Bit().constData();>

    if (i_data2 < 0) {
        i_data2 = 0;
        Log.LogRecord(Logs::High, Logs::UILog, "checkString i_data2 %i m_data %s ", i_data2, string_data.c_str());
        emit invalidData();
        return;
    }
    // only legitimate strings should pass through - strings in data sets should be converted to to thier int position before getting here
    // this may still leave previously valid answers in the response_map
    if (i_data1 > ResponseCode::VARIABLE) {
        RemoveResponse(i_data2);
        Log.LogRecord(Logs::High, Logs::UILog, "checkString  RemoveResponse i_data2 %i s_data1 %s", i_data2, string_data.c_str());
        emit noData();
        return;
    }

    Log.LogRecord(Logs::High, Logs::UILog, "checkString i_data2 %i m_data %s ", i_data2, string_data.c_str());
    string_map[i_data2] = string_data;

    i_data1 = 0;
    i_data2 = 0;
    s_data1.clear();
}

// if there are any processes, the dir_type has been set
void UIProcess::pageProcess(int page_id)
{
    bool no_process = false;

    // all static stuff should be done if no processes - this includes setting dynamic fields to invis
    if (GetPageProcesses(page_id) == 0) {
        no_process = true;
    }

    Log.LogRecord(Logs::High, Logs::Test, "pageProcess 1 page_id(%i) no_process(%s)", page_id, Log.DbgSwtch(no_process).c_str());
    SendTextFieldData(page_id, no_process);
    SendFieldData(page_id, no_process);
    SendMenuData(page_id, no_process);
    GetNextData(page_id, no_process);
}

// default works, come back to this for dynamic text
void UIProcess::SendTextFieldData(int page_id, bool no_process)
{
    std::string text_data;
    QString q_text_data;

    int text_id = 0;
    int field_id = 0;

    size_t line_pos = 0;

    GetTextID(page_id);

    // need to handle this better if there are more than on text fields on the page
    // need to know if it's a dynamic text field
    for (unsigned int i = 0; i < textfieldID_vec.size(); i++) {
        text_id = textfieldID_vec.at(i);

        if (dynamicTextField[text_id] == 1) {
            if (no_process == false) {
                GetFieldID(page_id);

                Log.LogRecord(Logs::High, Logs::Test, "SendTextFieldData: dynamic text-process text_id: %i text_data: %s ", text_id, text_data.c_str());

                for (unsigned int i = 0; i < fieldID_vec.size(); i++) {
                    field_id = fieldID_vec.at(i);

                    ProcessDynamicText(false, field_id);
                }
                fieldID_vec.clear();
            }
            else {
                text_data = text_prompt_vec.at(text_id - 1);

                line_pos = text_data.find("\\n");
                if (line_pos != std::string::npos) {
                    text_data.replace(line_pos, 2, "\n");
                }
                q_text_data = QString::fromStdString(text_data);
                Log.LogRecord(Logs::High, Logs::Test, "SendTextFieldData: dynamic text-no process text_id: %i text_data: %s ", text_id, text_data.c_str());

                emit sendToPrompt(text_id, q_text_data);
            }
        }
        else {
            Log.LogRecord(Logs::Med, Logs::Test, "SendTextFieldData");

            text_data = text_prompt_vec.at(text_id - 1);

            line_pos = text_data.find("\\n");
            if (line_pos != std::string::npos) {
                text_data.replace(line_pos, 2, "\n");
            }
            q_text_data = QString::fromStdString(text_data);
            Log.LogRecord(Logs::High, Logs::Test, "SendTextFieldData: normal text - text_id: %i text_data: %s ", text_id, text_data.c_str());

            emit sendToPrompt(text_id, q_text_data);
        }
    }
    textfieldID_vec.clear();
}

void UIProcess::SendFieldData(int page_id, bool no_process)
{
    std::string field_data;
    QString q_field_data;

    int field_id = 0;
    int response_id = 0;

    GetFieldID(page_id);

    if (fieldID_vec.size() == 0) {
        return;
    }

    for (unsigned int i = 0; i < fieldID_vec.size(); ++i) {
        field_id = fieldID_vec.at(i);

        // much of this could be handled differently
        // handle labels - come back to this for SLtext labels
        if (defaultActiveField[field_id] == 1) {
            HandleFieldLabels(0, field_id, 0);

            if (no_process == true) {
                continue;
            }
        }

        if (no_process == true) {
            emit changeState(field_id, false);
        }

        // much of this could be handled differently
        GetResponseIDs(field_id);

        for (unsigned int i = 0; i < responseID_vec.size(); ++i) {
            if (responseID_vec.at(i) <= DISTINCT) {
                Log.LogRecord(Logs::High, Logs::UILog, "sendFieldData: text/menus response_id: %i field_data: %s ", response_id, field_data.c_str());
                continue;
            }
            else {
                response_id = responseID_vec[i];
                field_data = text_response_vec.at(response_id - 1); // text_id - 1 - this is for text document pop -  revert the -1 otherwise
                q_field_data = QString::fromStdString(field_data);

                Log.LogRecord(Logs::High, Logs::Test, "sendFieldData: buttons response_id: %i field_data: %s ", response_id, field_data.c_str());

                emit sendToField(field_id, response_id, q_field_data);
            }
        }
        responseID_vec.clear();
    }
    fieldID_vec.clear();
}

void UIProcess::SendMenuData(int page_id, bool no_process)
{
    int field_id = 0;

    GetFieldID(page_id);

    if (fieldID_vec.size() == 0) {
        return;
    }
    if (no_process) {}

    for (unsigned int i = 0; i < fieldID_vec.size(); i++) {
        field_id = fieldID_vec.at(i);
        ProcessDynamicMenu(false, field_id);
    }
    fieldID_vec.clear();
}

void UIProcess::GetNextData(int page_id, bool no_process)
{
    if (no_process == true) {
        return;
    }

    std::map<int, int>::iterator m_it;
    std::map<int, std::string>::iterator it2;

    QString qp_string;
    std::string p_string;

    int confirm_status = 0;
    int data_code = 0;
    int field = 0;
    int response = 0;

    GetFieldID(page_id);

    for (unsigned int i = 0; i < fieldID_vec.size(); i++) {
        field = fieldID_vec.at(i);
        confirm_status = GetConfirmID(field);

        if (confirm_status > Question::STRING_VALIDATE) {
        //    if (page_processes > 0) { //  && page < L_PAGE_NUM - was there so it didn't process items beyond max page but it's not needed
                for (m_it = response_map.begin(); m_it != response_map.end(); ++m_it) {
                    if (field == m_it->first) {
                        field = m_it->first;
                        response = m_it->second;
                        data_code = GetDataCodeFromFieldID(field);
                        Log.LogRecord(Logs::Med, Logs::UILog, "response_map: field %i response %i", field, response);
                        // this does the "file_data_map" processing if the field checks to use this map
                        if (data_code != 0) {
                            for (unsigned int j = 0; j < file_data_map[DataFileString[MaxDataCode - 1]].size(); j++) { // (j = 1) M_DATA_CODE are # of vector data_sets
                                if (p_string == (file_data_map[DataFileString[data_code]]).at(j)) {
                                    Log.LogRecord(Logs::High, Logs::UILog, "getNextData: found from data_code  ", p_string.c_str());
                                    Log.LogRecord(Logs::High, Logs::UILog, "getNextData: data_code r: %i dc: %i", response, data_code);
                                    emit dataCode(response, data_code);
                                    break;
                                }
                            }
                        }
                        emit changeState(response, true);
                    }
                }
        //    }
        }
        else if (confirm_status == Question::STRING_VALIDATE) {
            for (it2 = string_map.begin(); it2 != string_map.end(); ++it2) {
                if (field == it2->first) {
                    p_string = it2->second;
                    qp_string = QString::fromStdString(p_string);
                    Log.LogRecord(Logs::High, Logs::UILog, "getNextData: STRING_VALIDATE str: %s qID %i", p_string.c_str(), field);
                    Log.LogRecord(Logs::High, Logs::UILog, "getNextData: string_map.size() %i ", string_map.size());
                    emit stringData(qp_string, field);
                    break;
                }
            }
        }
    }
    fieldID_vec.clear();
}

int UIProcess::GetDataCodeFromFieldID(int field_id)
{
    QMLUIProcess QtUI;

    int data_code = 0;

    // need to account for fields/data_codes prior to a choice and prior to dynamic field activation
    // check for static fields with data codes

    data_code = fieldID_to_datacodeID[field_id];

    if (data_code == 0) {
        switch (cur_dir_type) {
        case QT_QML:
            data_code = QtUI.QMLDataCode(field_id);
            break;
        }
    }
    return data_code;
}

int UIProcess::GetFieldIDFromDataCode(int data_code)
{
    QMLUIProcess QtUI;

    int field_id = 0;

    switch (cur_dir_type) {
    case QT_QML:
        field_id = QtUI.QMLFieldIDFromDataCode(data_code);
        break;
    }
    return field_id;
}

void UIProcess::PrepDirTypeArrayData(int directive_type)
{
    QMLUIProcess QtUI;

    Log.LogRecord(Logs::High, Logs::UILog, "PrepDirTypeArrayData: directive_type(%i)", directive_type);

    switch (directive_type) {
    case QT_QML:
        QtUI.PrepQMLDirType();
        break;
    }
}

void UIProcess::PrepDynamicFieldData(bool show_values)
{
    QMLUIProcess QtUI;

    switch (cur_dir_type) {
    case QT_QML:
        QtUI.PrepQMLDynamicFields(show_values);
        break;
    }
}

// this has the ability to have an expanded ydim for multiple required or dynamic fields but, the function will need to be altered to handle properly
int UIProcess::GetDynamicField(bool get_required, int field_id)
{
    QMLUIProcess QtUI;

    int linked_field = 0;

    switch (cur_dir_type) {
    case QT_QML:
        linked_field = QtUI.GetQMLDynamicFields(get_required, field_id);
        break;
    }
    Log.LogRecord(Logs::High, Logs::Test, "Get DynamicField: field_id:(%i) dir_type:(%i) linked_field:(%i)", field_id, cur_dir_type, linked_field);

    return linked_field;
}

// this seems to work well, but only implemented for a single text field process
void UIProcess::HandleDynamicText(bool next_pos, int field_id)
{
    QMLUIProcess QtUI;

    std::string dynamic_text;
    QString q_dynamic_text;

    int text_id = 0;
    int dynamic_text_id = 0;
    int dynamic_text_pos = 0;

    if (next_pos == true) {
        dynamic_text_pos = 1;
    }

    switch (cur_dir_type) {
    case QT_QML:
        text_id = QtUI.GetQMLTextIDLink(field_id);
        dynamic_text_id = QtUI.QMLDynamicText(field_id, dynamic_text_pos);
        break;
    }

    if (dynamic_text_id > 0) {
        dynamic_text = dynamic_text_vec.at(dynamic_text_id - 1);
        q_dynamic_text = QString::fromStdString(dynamic_text);

        Log.LogRecord(Logs::High, Logs::UILog, "HandleDynamicText: field_id:(%i) text_id:%i dynamic_text_id:(%i) dynamic_text:(%s)",
                      field_id, text_id, dynamic_text_id, dynamic_text.c_str());

        if (dynamic_text.length() > 0) {
            Log.LogRecord(Logs::High, Logs::UILog, "  dynamic_text.length:");
            emit dynamicText(0, text_id, q_dynamic_text);
        }
    }
}

// Dynamic Menu notes - response_id = i_data1
// the linked_field is the contingent field - only the clicked fieldID is processed, we must check this field for linked_fields
// find contingent fields above the processed/passed field, clear any data and flag as not processed (RemoveResponse does this), repopulate with proper data
// this is used on click - linked field is the next in line field - field_ID is clicked, linked_field_id needs to be activated
void UIProcess::ProcessDynamicMenu(bool on_click, int field_id)
{
    if (field_id == 0) {
        return;
    }

    if (GetDataCodeFromFieldID(field_id) == 0) {
        return;
    }

    std::map<int, int>::iterator m_it;

    int page_id = 0;
    int required_field = 0;
    int contingent_field = 0;
    int linked_field_id = 0;

    size_t num_fields = 0;

    bool field_process = false;

    Log.LogRecord(Logs::High, Logs::Test, "ProcessDynamicMenu: ");

    /*
     need to handle data_code == 1 and data_code > 1 separately
        - if data_code == 1 is processed/clicked, populate the appropriate lists, dynamic text, etc
        but only do it when the field corresponding to data_code == 1 changes
    */

    // checking the menus on a new page cycles through all field_ids - check earlier
    if (on_click == false) {
        field_process = CheckFieldProcess(field_id);
        linked_field_id = GetDynamicField(true, field_id);

        Log.LogRecord(Logs::Med, Logs::UILog, "DynamicMenu:(%s) fieldID:(%i) linked_field:(%i)",
                      Log.DbgSwtch(field_process).c_str(), field_id, linked_field_id);

        if (field_process == false) {
            if (linked_field_id > 0) {
                if (CheckFieldProcess(linked_field_id) == false) {
                    Log.LogRecord(Logs::Med, Logs::UILog, "CheckFieldProcess:(false) fieldID:(%i) linked_field:(%i)", field_id, linked_field_id);
                    emit changeState(field_id, false); // makes linked field menus beyond the first branch invisible
                }
            }
        }
        HandleMenuLists(on_click, field_id);
    }
    else {
        page_id = GetPageID(field_id);
        GetFieldID(page_id);      

        // here we need to check the directive, if chosen(field = 3), and set the other contingent fields accordingly
        contingent_field = GetDynamicField(false, field_id);
        num_fields = fieldID_vec.size();

        Log.LogRecord(Logs::High, Logs::Test, "DynamicMenu: num_fields(%i) field_id(%i) contingent_field(%i)", num_fields, field_id, contingent_field);

        // this will cycle through the max # of fields on the page checking the contingent then the next contingent...
        // ...up the tree until either the last field of the page or the contingent doesn't have a contingent(0)
        // here we need to check the directive, if chosen, and set the other contingetn fields accordingly
        for (unsigned int i = 0; i < num_fields; i++) {
            if (i != 0) {
                contingent_field = GetDynamicField(false, contingent_field);
            }
            Log.LogRecord(Logs::High, Logs::Test, "DynamicMenu(%i) contingent_field(%i)", i, contingent_field);

            if (contingent_field > 0 ) {
                // this isn't necessary but it could be a PH for additional condition checking, such as multiple dynamics
                if (CheckFieldProcess(contingent_field) == true) {
                    field_emit_map[contingent_field] = 2;
                    RemoveResponse(contingent_field);
                    Log.LogRecord(Logs::Med, Logs::UILog, "DynamicMenu: process(%s) contingent_field:(%i)", Log.DbgSwtch(true).c_str(), contingent_field);
                }
                else {
                    field_emit_map[contingent_field] = 2;
                    Log.LogRecord(Logs::High, Logs::Test, "DynamicMenu: process(%s) contingent_field:(%i)", Log.DbgSwtch(false).c_str(), contingent_field);
                }
            }
            else {
                break;
            }
        }
        Log.LogRecord(Logs::Med, Logs::Test, "\tProcessDynamicMenu:");
        linked_field_id = GetDynamicField(false, field_id);

        Log.LogRecord(Logs::High, Logs::Test, "field_id %i linked_field_id %i", field_id, linked_field_id);

        if (linked_field_id != 0) {
            // check each field for Get DynamicField contingent != 0
            for (unsigned int i = 0; i < fieldID_vec.size(); i++) {
                required_field = fieldID_vec.at(i);
                contingent_field = GetDynamicField(false, required_field);
                Log.LogRecord(Logs::Med, Logs::UILog, "DynamicMenu: req:(%i) cont:(%i)", required_field, contingent_field);

                // if the required field's contingent = 0, the contingent field = linked_field and this will be set below
                if (contingent_field != 0) {
                    Log.LogRecord(Logs::Med, Logs::UILog, "DynamicMenu: req:(%i) cont:(%i)", required_field, contingent_field);

                    if (CheckFieldProcess(contingent_field) == true) {
                        field_emit_map[required_field] = 1;
                        Log.LogRecord(Logs::Med, Logs::UILog, "DynamicMenu: process(%s) req:(%i) cont:(%i)", Log.DbgSwtch(true).c_str(), required_field, contingent_field);
                    }
                }
            }
            field_emit_map[linked_field_id] = 1;
            field_emit_map[field_id] = 1;
        }
        Log.LogRecord(Logs::Med, Logs::Test, "Dynamic");

        HandleMenuLists(on_click, field_id);

        for (m_it = field_emit_map.begin(); m_it != field_emit_map.end(); ++m_it) {
            if (m_it->second == 1) {
                Log.LogRecord(Logs::High, Logs::UILog, "DynamicMenu: emit 1 fieldID:(%i)", m_it->first);
                emit fieldState(m_it->first, 1);
            }
            else if (m_it->second == 2) {
                Log.LogRecord(Logs::High, Logs::UILog, "DynamicMenu: emit 2 fieldID:(%i)", m_it->first);
                HandleFieldLabels(1, m_it->first, field_id); // temp to divert from error-> field_id
                emit fieldState(m_it->first, 2);
            }

        }
        HandleFieldLabels(1, linked_field_id, field_id); // temp to divert from error-> field_id

        fieldID_vec.clear();
        field_emit_map.clear();
    }
}

// change this to handle all menu lists - sendMenuData
void UIProcess::HandleMenuLists(bool on_click, int field_id)
{
    QMLUIProcess QtUI;

    std::vector<std::string> data_list_text_vec;
    std::vector<std::string> menu_list_vec;

    std::string response;
    QString q_response;

    int data_code = 0;
    int response_id = -1;
    int return_count = 0;
    int element = 0;
    int dynamic_field = 0;

    if (on_click == false) {
        data_code = GetDataCodeFromFieldID(field_id);

        Log.LogRecord(Logs::High, Logs::Test, "HandleMenuLists: data_code %i", data_code);

        // need to handle data_code == 1 and data_code > 1 separately
        if (data_code > 0) {
            data_list_text_vec = file_data_map[DataFileString[data_code]];

            for (unsigned int i = 0; i < data_list_text_vec.size(); i++) {
                menu_list_vec.push_back(data_list_text_vec.at(i));
            }
            Log.LogRecord(Logs::High, Logs::Test, "HandleMenuLists: menu_list_vec %i", menu_list_vec.size());
        }
    }
    else if (on_click == true) {
        response_id = GetResponseIDFromMap(field_id);
        dynamic_field = GetDynamicField(false, field_id);

        Log.LogRecord(Logs::Med, Logs::Test, "HandleMenuLists: field_id(%i) dynamic_field(%i)", field_id, dynamic_field);

        if (dynamic_field > 0) {
            data_code = GetDataCodeFromFieldID(dynamic_field);
        }

        Log.LogRecord(Logs::High, Logs::Test, "DynamicMenu: clicked:(%s) field_id:(%i) dynamic_field:(%i) data_code:(%i) response_id:(%i)",
                     Log.DbgSwtch(on_click).c_str(), field_id, dynamic_field, data_code, response_id);

        // need to handle data_code == 1 and data_code > 1 separately
        if (data_code > 0) {

            switch (cur_dir_type) {
            case QT_QML:
                Log.LogRecord(Logs::High, Logs::Test, "HandleMenuLists: data_code %i", data_code);
                data_list_text_vec = file_data_map[QmlDataFileString[data_code]];
                break;
            }
            Log.LogRecord(Logs::High, Logs::Test, "HandleMenuLists: data_list_text_vec.size %i", data_list_text_vec.size());

            for (unsigned int i = 0; i < data_list_text_vec.size(); i++) {
                if (dynamic_field == 2) {
                    Log.LogRecord(Logs::High, Logs::Test, "HandleMenuLists: data_list_text_vec:%s", data_list_text_vec.at(i).c_str());
                    for (unsigned int j = 0; j < projects_vec.size(); j++) {
                        if (projects_vec.at(j) - 1 == i) {
                            response = data_list_text_vec.at(i);
                            menu_list_vec.push_back(response);
                            ++return_count;

                            Log.LogRecord(Logs::High, Logs::Test, "DynamicMenu: dynamic_field:(%i) data_code:(%i) project data position:(%i) projects:(%s)",
                                          dynamic_field, data_code, i, response.c_str());
                        }
                    }
                }
                else if (dynamic_field == 3) {
                    for (unsigned int j = return_count; j < maximum_directives; j++) {

                        switch (cur_dir_type) {
                            case QT_QML:
                            element = QtUI.GetQMLDirective(response_id, j);
                            break;
                        }

                        if (element - 1 == i) {
                            response = data_list_text_vec.at(i);
                            menu_list_vec.push_back(response);
                            ++return_count;

                            Log.LogRecord(Logs::High, Logs::UILog, "DynamicMenu: dynamic_field:(%i) data_code:(%i) project data position:(%i) projects:(%s)",
                                          dynamic_field, data_code, i, response.c_str());
                            break;
                        }
                    }
                }
            }
        }
        field_id = dynamic_field;
    }
    return_count = static_cast<int>(menu_list_vec.size());

    emit sendWordCount(field_id, return_count);

    for (unsigned int i = 0; i < menu_list_vec.size(); ++i) {
        response = menu_list_vec[i];
        q_response = QString::fromStdString(response);
        Log.LogRecord(Logs::High, Logs::Test, "sendMenuData: menu pos(%i) menu_data(%s)", i, response.c_str());
        emit sendWords(on_click, field_id, q_response);
    }
    data_list_text_vec.clear();
    menu_list_vec.clear();
}

// may need to change GetStringResponse() and use similar implementation to autocomplete to populate the menus
void UIProcess::ProcessDynamicText(bool on_click, int field_id)
{
    int linked_field_id = 0;

    bool field_process = false;

    Log.LogRecord(Logs::High, Logs::Test, "ProcessDynamicText: ");

    // the linked_field is the required field - sendMenuData() calls and cycles through all fieldIDs on the page
    if (on_click == false) {
        field_process = CheckFieldProcess(field_id);

        if (field_process == false) {
            linked_field_id = GetDynamicField(true, field_id);

            Log.LogRecord(Logs::Med, Logs::UILog, "ProcessDynamicText: on_click?(%s) processed?(%s) field_id:%i linked_field:%i)",
                          Log.DbgSwtch(on_click).c_str(), Log.DbgSwtch(field_process).c_str(), field_id, linked_field_id);

            HandleFieldLabels(0, field_id, linked_field_id); // omit eventually

            Log.LogRecord(Logs::High, Logs::Test, "ProcessDynamicText: ");

            if (linked_field_id > 0) {
                if (CheckFieldProcess(linked_field_id) == true) {
                    Log.LogRecord(Logs::High, Logs::UILog, "ProcessDynamicText: fieldID:(%i) linked_field:(%i) and processed true", field_id, linked_field_id);
                    HandleDynamicText(false, field_id);
                }
            }
        }
        else {
            HandleDynamicText(true, field_id);
        }
    }
    // the linked_field is the contingent field - only the clicked fieldID is processed, we must check this field for linked_fields
    // no transition will occur when nothing changes on a click, unsure about clicking the same item
    else if (on_click == true) {
        linked_field_id = GetDynamicField(false, field_id);

        Log.LogRecord(Logs::Med, Logs::UILog, "HandleContingentFields: on_click:(%s) field_id:%i linked_field:%i",
                      Log.DbgSwtch(on_click).c_str(), field_id, linked_field_id);

        if (linked_field_id == 0) {
            HandleDynamicText(true, field_id);
        }
        else {
            HandleDynamicText(false, linked_field_id);
        }
    }
}

// this processes the selections form the menus and performs the parse - this does not include additional options
void UIProcess::ProcessMap()
{
    ParseFiles PF;
    ParseDirectory PDir;

    std::map<int, int>::iterator m_it;
    int field_id = 0;

    int dir_type = 0;
    int project = 0;
    int directive = 0;

    for (m_it = response_map.begin(); m_it != response_map.end(); ++m_it) {
        field_id = m_it->first;

        switch (field_id) {
        case 1:
            dir_type = m_it->second;
            break;
        case 2:
            project = m_it->second + 1;
            break;
        case 3:
            directive = m_it->second + 1;
            break;
        }
    }
    Log.LogRecord(Logs::High, Logs::Test, "ProcessMap: dir_type(%i) project(%i) directive(%i)", dir_type, project, directive);

    PF.DoDirectiveCleanup(true, true, true);

    PF.PrepareDirectiveType(dir_type);
    PF.PrepReturnCodeStrings(dir_type);
    PDir.LoadData(dir_type, project);

    PF.PrepParseDirective(dir_type, directive);
    PF.MainParseFunction(dir_type, directive, false);

}

int UIProcess::GetResponseIDFromMap(int field_id)
{
    //std::map<float, std::string>::iterator m_it;
    std::map<int, int>::iterator m_it;

    int response_id = 0;

    for (m_it = response_map.begin(); m_it != response_map.end(); ++m_it) {
        if (field_id == m_it->first) {
            response_id = m_it->second;
        }
    }
    Log.LogRecord(Logs::Med, Logs::UILog, "GetResponseIDFromMap: response_id %i", response_id);

    return response_id;
}

bool UIProcess::RemoveResponse(int field_id)
{
    std::map<int, int>::iterator m_it;
    int data_code = 0;
    int response = 0;
    bool reset = true;

    Log.LogRecord(Logs::High, Logs::Test, "RemoveResponse1");
    for (m_it = response_map.begin(); m_it != response_map.end(); ++m_it) {
        Log.LogRecord(Logs::High, Logs::UILog, "RemoveResponse2");
        if (field_id == m_it->first) {
            field_id = m_it->first;
            response = m_it->second;
            data_code = GetDataCodeFromFieldID(field_id);
            response_map.erase(m_it);

            if (i_data1 == response) {
                if (data_code == 0) {
                    reset = false;
                }
            }
            if (data_code == 0) {
                emit changeState(response, false);
            }
            Log.LogRecord(Logs::Med, Logs::UILog, "RemoveResponse:(replace-erase) data_code: %i resp_compare: %i -> %i", data_code, response, i_data1);
            break;
        }
    }
    return reset;
}

// change implementation to handle all labels
// if dynamic menu and linked field is not processed, skip this
void UIProcess::HandleFieldLabels(int load_type, int field_id, int linked_field_id)
{
    std::string menu_label;
    QString q_menu_label;

    int data_code = 0;

    // determine the field type and populate the label from the appropriate field type vector
    data_code = GetDataCodeFromFieldID(field_id);

    Log.LogRecord(Logs::Med, Logs::UILog, "LoadMenuLabel: type %i field %i linked_field_id %i data_code %i", load_type, field_id, linked_field_id, data_code);

    if (data_code > 0) {
        if (linked_field_id > 0 && CheckFieldProcess(linked_field_id) == true) {
            menu_label = text_label_vec.at(data_code - 1);
        }
        else if (linked_field_id == 0) {
            menu_label = text_label_vec.at(data_code - 1);
        }
        q_menu_label = QString::fromStdString(menu_label);

        emit sendToField(field_id, load_type, q_menu_label);
    }
}

// similar but simpler than GetPageProcesses
bool UIProcess::CheckFieldProcess(int field_id)
{
    //std::map<float, std::string>::iterator m_it;
    std::map<int, int>::iterator m_it;

    bool process = false;

    for (m_it = response_map.begin(); m_it != response_map.end(); ++m_it) {
        if (field_id == m_it->first) {
            Log.LogRecord(Logs::High, Logs::UILog, "CheckFieldProcess: Process = true");
            process = true;
            break;
        }
    }
    Log.LogRecord(Logs::High, Logs::Test, "CheckFieldProcess: field_id:(%i) process(%s)", field_id, Log.DbgSwtch(process).c_str());
    return process;
}

// this erases "fieldID_vec" and must be placed before other uses of the vec wihtin a function
// use "CheckFieldProcess()" for simple chekcs
// the result of this will determine if the user goes to the next page
// change the map name and possibly implementation for braoder use
int UIProcess::GetPageProcesses(int page_id)
{
    //std::map<float, std::string>::iterator it;
    std::map<int, int>::iterator m_it;

    int page_processes = 0;
    int confirm_status = 0;
    int field_id = 0;

    GetFieldID(page_id);

    for (unsigned int i = 0; i < fieldID_vec.size(); i++) {
        field_id = fieldID_vec.at(i);
        confirm_status = GetConfirmID(field_id);

        if (confirm_status == Question::SINGLE_REQ || confirm_status == Question::SINGLE_ACC){
            for (m_it = response_map.begin(); m_it != response_map.end(); ++m_it) {
                if (field_id == m_it->first) {
                    ++page_processes;
                    Log.LogRecord(Logs::High, Logs::UILog, "GetPageProcesses: SINGLE_REQ %i", page_processes);
                }
            }
        }
        else if (confirm_status == Question::MULTIPLE_REQ) {
            for (m_it = response_map.begin(); m_it != response_map.end(); ++m_it) {
                if (field_id == m_it->first) {
                    ++page_processes;
                    Log.LogRecord(Logs::High, Logs::UILog, "GetPageProcesses: MULTIPLE_REQ %i", page_processes);
                }
            }
        }
        else if (confirm_status == Question::PROCESS_DATA) {
            Log.LogRecord(Logs::High, Logs::UILog, "GetPageProcesses: PROCESS_DATA = 1");
            page_processes = 1;
        }
    }
    Log.LogRecord(Logs::High, Logs::UILog, "GetPageProcesses: END");
    fieldID_vec.clear();
    return page_processes;
}

// for menus, may need to change with dynamic menus
std::string UIProcess::GetStringResponse(int field_id, int num_response)
{
    std::string response;
    int confirm_id = 0;

    if (i_data3 < 0 || i_data3 >= MaxDataCode) {
        i_data3 = 0;
    }

    Log.LogRecord(Logs::High, Logs::UILog, "field_id: %i i_data3: %i num_response: %i", field_id, i_data3, num_response);
    if (field_id == GetFieldIDFromDataCode(i_data3)) {
        for (unsigned int j = 0; j < file_data_map[DataFileString[MaxDataCode - 1]].size(); j++) { // "M_DATA_CODE / MaxDataCode - 1 not as clean looking
            response = file_data_map[DataFileString[i_data3]].at(num_response);
            Log.LogRecord(Logs::High, Logs::UILog, "GetStringResponse: found %s", response.c_str());
            break;
        }
        i_data3 = 0;
        return response;
    }
    confirm_id = GetConfirmID(field_id);
    if (confirm_id != Question::MULTIPLE_REQ) {
        response = text_response_vec[num_response - 1]; // ResponseString[num_response]; // response_code_vec need to redo this for getting data from the vector
    }
    Log.LogRecord(Logs::High, Logs::UILog, "num_response: %i confirm_id: %i", num_response, confirm_id);
    return response;
}

// 0 = passed, 1 = badchar, 2 = badlength
int UIProcess::IsValidInput(std::string p_input, bool check_acceptable, bool check_restricted, bool check_length, const char * valid_chars, const char * invalid_chars, int min_length, int max_length)
{
    int passed_check = 0;

    std::string check_char(p_input);

    const unsigned int maxcharlength = max_length;
    const unsigned int mincharlength = min_length;

    if (check_acceptable == true) {
        if (check_char.find("  ") != std::string::npos) {
                passed_check = 1;
            }
        if (check_char.find_first_not_of(valid_chars) != std::string::npos) {
            passed_check = 1;
        }
    }
    if (check_restricted == true) {
        if (check_char.find_first_of(invalid_chars) != std::string::npos) {
            passed_check = 1;
        }
    }
    if (check_length == true) {
        if (p_input.size() > maxcharlength || p_input.size() < mincharlength) {
            passed_check = 2;
        }
    }
    Log.LogRecord(Logs::High, Logs::UILog, "IsValidInput: passed_check = %i", passed_check);
    return passed_check;
}

// when better nextButton with text field ID is implemented the above function and this one should be combined/changed
bool UIProcess::ValidateString(int field_id)
{
    std::map<int, std::string>::iterator it;
    std::string p_string;
    int passed_check = -1;
    bool passed = false;

    p_string = string_map[field_id];
    passed_check = IsValidInput(p_string.c_str(), true, false, true, NAME_CHAR, NULL, 2, 25);

    if (passed_check == 0) {
        passed = true;
    }

    if (field_id == 1 || field_id == 2) {
        if (field_id == 1) {
            f_name = string_map[field_id];
        }
        else if (field_id == 2) {
            l_name = string_map[field_id];
        }
        if (string_map.size() < 2) {
            passed = false;
        }
    }
    Log.LogRecord(Logs::High, Logs::UILog, "ValidateString: END %i", passed);
    return passed;
}

// all array data manipulation functions below - arrays located at the top o this file
void UIProcess::GetFieldID(int page_id)
{
    int num_fields = GetNumInputFields(page_id);
    int field_id = 0;

    for (int i = 0; i < num_fields; ++i) {
        if (fieldID_from_pageID[i][page_id] != 0) {
            field_id = fieldID_from_pageID[i][page_id];
            fieldID_vec.push_back(field_id);
            Log.LogRecord(Logs::High, Logs::UILog, "fieldIDs %i", field_id);
        }
        else {
            break;
        }
    }
    Log.LogRecord(Logs::High, Logs::UILog, "num_fields %i", num_fields);
}

void UIProcess::GetTextID(int page_id)
{
    int num_fields = GetNumTextFields(page_id);
    int textfield_id = 0;

    for (int i = 0; i < num_fields; ++i) {
        if (textID_from_pageID[i][page_id] != 0) {
            textfield_id = textID_from_pageID[i][page_id];
            textfieldID_vec.push_back(textfield_id);
            Log.LogRecord(Logs::High, Logs::UILog, "textfield_id %i", textfield_id);
        }
        else {
            break;
        }
    }
    Log.LogRecord(Logs::High, Logs::UILog, "num_fields %i", num_fields);
}

int UIProcess::GetPageID(int field_id)
{
    return fieldID_to_pageID[field_id];
}

int UIProcess::GetConfirmID(int field_id)
{
    return fieldID_to_confirmID[field_id];
}

int UIProcess::GetNumInputFields(int page_id)
{
    return pageID_to_num_inputfields[page_id];
}

int UIProcess::GetNumTextFields(int page_id)
{
    return pageID_to_num_textfields[page_id];
}

// this needs quite a bit of work but, will work for now
int UIProcess::GetMinPageProcesses(int page_id)
{
    return pageID_to_num_inputfields[page_id];
}

bool UIProcess::ReplacePrevData(int confirm_status)
{
    if (confirmID_to_switch[confirm_status] == 1) {
        return true;

    }
    else {
        return false;
    }
}

int UIProcess::GetNumResponses(int field_id)
{
    return fieldID_to_num_responses[field_id];
}

void UIProcess::GetResponseIDs(int field_id)
{
    int num_responses = GetNumResponses(field_id);
    int response_id = 0;

    for (int i = 0; i < num_responses; ++i) {
        if (responseID_from_fieldID[i][field_id] != 0) {
            response_id = responseID_from_fieldID[i][field_id];
            responseID_vec.push_back(response_id);
            Log.LogRecord(Logs::High, Logs::UILog, "response_id: %i num_responses: %i", response_id, num_responses);
        }
        else {
            break; // stop processing, nothing in this or next element
        }
    }
    Log.LogRecord(Logs::High, Logs::UILog, "num_responses %i", num_responses);
}
/*
log to file the results, create reports
    - if (startup == true && l_it->p_dir < 3) { continue; } // do the report after directive 3 is done

turn this into the main process and pass a bool to flag the process type
*/

// this is currently used to handle the startup routine but could be used throughout with alterations
void UIProcess::HandleSignals(int signal_type, int directive, int signal_code, int i_signal_data, std::string s_signal_data)
{
    std::string input_text;
    QString q_input_text;
    QString qs_signal_data;

    qs_signal_data = QString::fromStdString(s_signal_data);

    input_text = GetSignalInputText(directive, signal_code);
    q_input_text = QString::fromStdString(input_text);

    switch (signal_type) {
    case 0: // quit/end program
        emit processedData();
        break;
    case 1: // this is for errors/end_parse
        Log.LogRecord(Logs::High, Logs::UIStartup, "HandleSignals: type(%i) i_data(%i) s_data(%s)", signal_type, i_signal_data, s_signal_data.c_str());
        emit dynamicText(1, 0, qs_signal_data);
        break;
    case 2: // for user-accepted updates - set n_data(textID) appropriately - this will need work
        Log.LogRecord(Logs::High, Logs::UIStartup, "HandleSignals: type(%i) i_data(%i) s_data(%s)", signal_type, i_signal_data, s_signal_data.c_str());
        emit dynamicText(0, 0, qs_signal_data);
        if (signal_code == 19 || signal_code == 24) {
            emit dynamicText(4, 0, q_input_text);

            // need a better workaround for multiple choices
            input_text = GetSignalInputText(0, signal_code);
            q_input_text = QString::fromStdString(input_text);
            emit dynamicText(5, 0, q_input_text);
        }
        else {
            emit dynamicText(4, 0, q_input_text);
        }
        break;
    case 3: // send signal to fade out of buttons and in of text
        Log.LogRecord(Logs::High, Logs::UIStartup, "HandleSignals: ");
        emit changeState(0, false);
        emit dynamicText(0, 0, qs_signal_data);
        break;
    case 4: // signal that "reparse"/user choice processing is complete and to redo startup processes
        Log.LogRecord(Logs::High, Logs::UIStartup, "HandleSignals: emit beginApp(4) ");
        emit beginApp(4); // user choice processing complete
        emit dynamicText(0, 0, qs_signal_data);
        break;
    case 5: // signal for "reparse" startup processes complete for UI
        //q_input_text = "The build procedure must be completed for the application.\nFailure could produce undesired results.";
        Log.LogRecord(Logs::High, Logs::UIStartup, "HandleSignals: emit beginApp(5) ");
        emit beginApp(5); // "reparse" startup processes complete, signal rebuild
        break;
    default:
        break;
    }
}

void UIProcess::processDirectiveResult(int directive_type, int from_field, bool process_update)
{
    /*
    l_it->dir_type          = directive_type
    l_it->p_dir             = parse_directive
    l_it->func_id           = function_id
    l_it->dir_status        = return_status
    l_it->dir_stat_string   = ret_code_string
    */

    std::list<DirectiveStatus_struct>::iterator l_it;

    std::string s_signal_data;

    int signal_type = 0;
    int signal_code = 0;
    int i_signal_data = 0;

    Log.LogRecord(Logs::High, Logs::UIStartup, "directive_type:(%i) dirstat_list size:(%i)", directive_type, dirstat_list.size());

    for (l_it = dirstat_list.begin(); l_it != dirstat_list.end(); ++l_it) {
        signal_code = l_it->dir_status;

        switch (directive_type) {
        case QT_QML:
            if (process_update == true) {
                if (signal_code > 0) {
                    Log.LogRecord(Logs::High, Logs::UIStartup, "processDirectiveResult: directive_type(%i) from_field(%i) process_update(%s)", directive_type, from_field, Log.DbgSwtch(process_update).c_str());

                    s_signal_data = GetSignalInputText(l_it->p_dir, signal_code);
                    if (s_signal_data == "Close") {
                        Log.LogRecord(Logs::High, Logs::UIStartup, "s_signal_data == Close  processDirectiveResult: quit");
                        HandleSignals(0, 0, 0, 0, "");
                    }
                    else { // if (s_signal_data == "Update") // anything else but close will be a processing command for this "directive_type" atm
                        if (signal_code == 13) {
                            Log.LogRecord(Logs::High, Logs::UIStartup, "processDirectiveResult  rewrite main.qml");
                        }
                        if (signal_code == 14) {
                            Log.LogRecord(Logs::High, Logs::UIStartup, "processDirectiveResult  rewrite .qml pages");
                        }
                        Log.LogRecord(Logs::High, Logs::UIStartup, "processDirectiveResult  update");
                        HandleSignals(3, 0, 0, 0, "\nProcessing updates...");
                    }
                }
            }
            else {

                Log.LogRecord(Logs::High, Logs::Error, "(DIR_TYPE_%i:MAIN_DIR_%i:CUR_DIR_%i:FUNCTION_%i:CODE_%i) %s",
                              l_it->dir_type, l_it->m_dir, l_it->p_dir, l_it->func_id, signal_code, l_it->dir_stat_string.c_str());

                s_signal_data = Log.StringFormat("(DIR_TYPE_%i:MAIN_DIR_%i:CUR_DIR_%i:FUNCTION_%i:CODE_%i)\n%s",
                                                 l_it->dir_type, l_it->m_dir, l_it->p_dir, l_it->func_id, signal_code, l_it->dir_stat_string.c_str());

                if (startup == true) {
                    if (end_parse == true) {
                        signal_type = 1;
                    }
                    else if (prompt_set == true) {
                        signal_type = 2;
                    }

                    // only one "signal_code" should ever get this far - only one should be on the list > 0
                    if (signal_code > 0) {
                        HandleSignals(signal_type, l_it->p_dir, signal_code, i_signal_data, s_signal_data);
                    }
                }
            }
            break;
        }
    }
    // assure this is taken care of elswhere on a positive check or a reparse
    if (process_update == true) {
        //dirstat_list.clear();
    }
    if (rebuild == true) {
        if (directive_type == 1 && process_update == true) {
            Log.LogRecord(Logs::Med, Logs::UIStartup, "rebuild true  processDirectiveResult: quit");
            rebuild = false;
            HandleSignals(0, 0, 0, 0, "");
        }
    }
}

std::string UIProcess::GetSignalInputText(int directive, int signal_code)
{
    std::string signal_text;

    switch (signal_code) {
    case 3:
    case 5:
    case 9:
    case 10:
    case 11:
    case 12:
    case 15:
    case 16:
    case 17:
    case 18:
    case 20:
    case 21: // 21/22 needs to automatically go to the first page - these are feedback text
    case 22:
        signal_text = "Close";
        break;
    case 1:
    case 2:
    case 4:
    case 6:
    case 7:
    case 8:
        signal_text = "Update";
        break;
    case 13:
    case 14:
        if (directive == -3) {
            signal_text = "Update";
        }
        else {
            signal_text = "Close";
        }
        break;
    case 19:
    case 24:
        if (directive == 0) {
            signal_text = "Update array";
        }
        else {
            signal_text = "Update .qml file";
        }
        break;

    }
    return signal_text;
}

// directive 12 function 17 needs much work
void UIProcess::parseSelf(bool initial, bool reparse)
{
    ParseDirectory PDir;
    ParseFiles PF;

    const int startup_directives[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 12 }; // size 9 last dir 12

    std::list<DirectiveStatus_struct>::iterator l_it;
    std::vector<int> startup_dir_vec;

    int intial_dir = 3;
    int signal_code = 0;
    int user_choice = 0;
    int cur_dir = 0;

    startup_dir_vec.clear();

    for (int i = 0; i < 9; ++i) { // i < 9
        startup_dir_vec.push_back(startup_directives[i]);
    }

    if (reparse == true) {
        // clear reparse map - need to clear this between parses when a selection is made, as well
        // when no more changes are found the user will be prompted to rebuild because this should only be cleared before iterating between
        reparse_code_map.clear();
        for (l_it = dirstat_list.begin(); l_it != dirstat_list.end(); ++l_it) {
            signal_code = l_it->dir_status;
            if (signal_code > 0) {
                cur_dir = l_it->p_dir;
                Log.LogRecord(Logs::High, Logs::UIStartup, "(DIR_TYPE_%i:MAIN_DIR_%i:CUR_DIR_%i:FUNCTION_%i:CODE_%i:CHOICE_%i)",
                              l_it->dir_type, l_it->m_dir, l_it->p_dir, l_it->func_id, signal_code, i_data1);

                user_choice = i_data1;

                // need to handle this within ReparseSetup() and cur_directive somewhere also (this will be user input)
                if (cur_dir < 0) {
                    cur_dir = cur_directive;
                }
                Log.LogRecord(Logs::High, Logs::UIStartup, "Reparse");

                PF.ReparseSetup(1, cur_dir, signal_code, user_choice);

                if (end_parse == true) { // if endparse, do no other processing, only notify
                    Log.LogRecord(Logs::High, Logs::UIStartup, "Reparse end_parse == true ");
                    emit beginApp(6);
                    return;
                }
                // uncertain if the opposite of this condition would not require a signal if we are already here
                if (prompt_set == false) {
                    Log.LogRecord(Logs::Normal, Logs::UIStartup, "\t***Restarting Initial Parse***\n");
                    HandleSignals(4, 0, 0, 0, "Restarting...");
                    rebuild = true;
                    break;
                }
            }
        }
        dirstat_list.clear(); //test
    }

    else if (initial == true) {
        PF.DoDirectiveCleanup(true, true, true); // this may need to be set via a passed variable, not here

        for (int i = 0; i < intial_dir; ++i) {
            cur_directive = startup_dir_vec.at(i);
            Log.LogRecord(Logs::High, Logs::UIStartup, " intial_dir %i", cur_directive);

            if (cur_directive == 1) { // we may need to skip this part if we are re-parsing, not here
                PF.PrepareDirectiveType(1);
                PF.PrepReturnCodeStrings(1);
                PDir.LoadData(1, 1);
            }
            PF.PrepParseDirective(1, cur_directive);
            PF.MainParseFunction(1, cur_directive, false);

            if (end_parse == true) { // shows splash error
                return;
            }
            if (prompt_set == true) { // ends intital parse to load the UI screen signalled below
                //if (cur_directive == 1) { end_parse = true; }
                return;
            }
        }
        Log.LogRecord(Logs::Normal, Logs::UIStartup, "\t***Initial Startup Process Complete***\n");
    }
    else {
        // shows background and UI buttons to update
        // if "prompt_set" at any point in previous portion we get here to make changes
        // if we get here rebuild = false because we're doing updates - assure this will work in all startup circumstances
        if (prompt_set == true) {
            rebuild = false;
            Log.LogRecord(Logs::High, Logs::UIStartup, "parseSelf initial prompt_set == true ");
            emit beginApp(2); // prompt processing and user choices
            return;
        }

        for (unsigned int i = intial_dir; i < startup_dir_vec.size(); ++i) {
            cur_directive = startup_dir_vec.at(i);
            Log.LogRecord(Logs::High, Logs::UIStartup, "parseSelf directives %i", cur_directive);

            PF.PrepParseDirective(1, cur_directive);
            PF.MainParseFunction(1, cur_directive, false);

            if (end_parse == true) { // if endparse, do no other processing, only notify
                Log.LogRecord(Logs::High, Logs::UIStartup, "end_parse == true ");
                emit beginApp(1);
                return;
            }
            // if we get here rebuild = false because we're doing updates - assure this will work in all startup circumstances
            if (prompt_set == true) {
                rebuild = false;
                Log.LogRecord(Logs::High, Logs::UIStartup, "parseSelf prompt_set == true ");
                emit beginApp(2); // prompt processing and user choices
                return;
            }
        }

        if (rebuild == true) {
            Log.LogRecord(Logs::High, Logs::UIStartup, "rebuild == true");
            HandleSignals(5, 0, 0, 0, "Close");
        }
        else {
            Log.LogRecord(Logs::High, Logs::UIStartup, "rebuild == false");
            emit beginApp(0);
        }
        startup = false; // not positive this will go here
        cur_directive = 0;
        dirstat_list.clear();

        // do PREPARE_LISTDATA

        Log.LogRecord(Logs::Normal, Logs::UIStartup, "\t***Startup Processes Complete***\n");
    }
    startup_dir_vec.clear();    
}
