#include <iostream>
#include <vector>

#include <QDebug>

#include "Logging/logging.h"

#include "qmluiprocess.h"

#include "qmlparse.h"

#include "parsefiles.h"

#include "datacodes.h"
#include "uiprocess.h"



// specific directive requirements should have data for extra dynamic fields located within directive type files(ie. qmlparse.cpp)
// this implies the need to check the directive type -> directive for further dynamic fields (probably as a kind of switch array)
// these are the dynamic fields which vary in use between directives
// should eventually use a field-type code instead (ie. 1 = button, 2 = sltext edit) or the fieldID

typedef std::vector<int>x_dim;
typedef std::vector<x_dim>y_dim;
typedef std::vector<y_dim>z_dim;

const size_t maximum_QMLprojects(MaxQTRootDir);        // y_dim - number of projects = "MAX_PROJECTS"

const size_t max_fields(7);              // x_dim - num fields + 1 or QMLdynamicField xdim
const size_t max_contingents(1);         // y_dim - QMLdynamicField ydim - only one dimension

static const int QMLdynamicField[]  = { 0, 2, 3, -1, -1, -1, -1 };  // FieldID: 0 - 6
static const int QMLtextIDLink[]    = { 0, 3, 3, 3, 3, 3, 3 };  // FieldID: 0 - 6

// these are check just prior to execution
static const int QML_dynamic_fields[3][13] = {
//                  Directive:
//    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12

    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0 },               // 1st dynamic fieldID
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0 },               // 2nd dynamic fieldID
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0 },               // 3rd dynamic fieldID

};
/*
 * the next button can serve as execute
--- directive                       --- extra options before start          --- options after start
Setup and Check main.qml    -                                                   boolean choices
Check QML Pages             -                                                   boolean choices
Check File Dependencies     -
Contingent Components       -
Check Component IDs         -   write = true, get = false
Process Global Logic IDs    -   execute
Support Components          -   execute
Navigation Contingents      -   execute
Create QML Page             -   filename (string)
Alter Page Components       -   do all (true or false) -> pageID string, user_choice - array = 1 or file = 2 (could be bool)
Create Template Page        -   pageID (string), filename (string), user_switch - insert pageID (true or false)
Datacodes and Lists         -   execute
*/

static const int QMLdynamicTextField[]      = { 0, 0, 0, 1 };   // true or false TextID: 0 - 3

static const int QMLdynamicTextLink[2][7] = {
//          FieldID:
//    0, 1, 2, 3, 4, 5, 6
    { 0, 0, 1, 2, 0, 0, 0 },    // 1st dynamicTextID  (fieldID 3 uses dynamicTextIDs 2, 3)
    { 0, 0, 0, 3, 0, 0, 0 },    // 2nd dynamicTextID
};

static const int QMLdirective_from_project[3][13] = {
//                  Directive_pos:
//    0, 1, 2, 3, 4, 5,  6, 7, 8,  9, 10, 11, 12

    { 1, 2, 3, 4, 5, 6,  7, 8, 9, 10, 11, 12,  0 },     // 1st project pos 0
    { 1, 2, 3, 4, 5, 6,  7, 8, 9, 10, 11, 12,  0 },     // 2nd project pos 1
    { 2, 5, 6, 7, 8, 9, 12, 0, 0,  0,  0,  0,  0 },     // 3rd project pos 2

};

static const int QMLfieldID_to_datacodeID[]       = { 0, 1, 2, 3, 0, 0, 0 };
static const int QMLdatacodeID_to_fieldID[]       = { 0, 1, 2, 3 };

y_dim QMLdynamicFields(max_contingents, x_dim(max_fields, DefaultValue));

//z_dim dir_type_navigation(directive_types, y_dim(maximum_projects, x_dim(maximum_directives, DefaultValue)));

// need an array(s) to handle the dynamic fields for various directives


QMLUIProcess::QMLUIProcess()
{

}

void QMLUIProcess::PrepQMLDirType()
{
    bool valid = false;

    Log.LogRecord(Logs::Med, Logs::UILog, "PrepQMLDirType");

    z_dim dir_type_navigation(directive_types, y_dim(maximum_QMLprojects, x_dim(maximum_directives, DefaultValue)));

    for (unsigned int h = 0; h < dir_type_navigation.size(); ++h) { // function directories and files array
        if (h == QT_QML) {
            y_dim & maximum_projects(dir_type_navigation[QT_QML]);

            projects_vec.clear();


            for (unsigned int i = 1; i < maximum_projects.size(); ++i) {
                projects_vec.push_back(i);
            }

            Log.LogRecord(Logs::High, Logs::UILog, "projects_vec.size(%i)", projects_vec.size());
            Log.LogRecord(Logs::High, Logs::UILog, "dir_type_navigation.size(%i)", dir_type_navigation.size());
            Log.LogRecord(Logs::High, Logs::UILog, "maximum_projects.size(%i)", maximum_projects.size());

            for (unsigned int i = 0; i < maximum_projects.size(); ++i) {
                for (unsigned int k = 0; k < projects_vec.size(); ++k) {
                    Log.LogRecord(Logs::High, Logs::UILog, "  position i:element k-1  (%i:%i)" , i, projects_vec.at(k) - 1);

                    if (i == projects_vec.at(k) - 1) {
                        valid = true;
                        break;
                    }
                }
                x_dim & maximum_directives(maximum_projects[i]);

                for (unsigned int j = 0; j < maximum_directives.size(); ++j) {
                    if (valid == true) {
                        maximum_projects[i][j] = QMLdirective_from_project[i][j];
                    }
                    else { // populate "0" if no project in pos to access data properly
                        maximum_projects[i][j] = DefaultValue;
                    }
                    //std::cout << maximum_directives.at(j) << ",";
                }
                //std::cout << " <- directives  project: " << i + 1 << std::endl;
                valid = false;
            }
        }
    }
}

void QMLUIProcess::PrepQMLDynamicFields(bool show_values)
{
    for (unsigned int i = 0; i < QMLdynamicFields.size(); ++i) {
        x_dim & max_fields(QMLdynamicFields[i]);

        for (unsigned int j = 0; j < max_fields.size(); ++j) {
            QMLdynamicFields[i][j] = QMLdynamicField[j];
        }
    }

    if (show_values == true) {
        Log.LogRecord(Logs::Normal, Logs::UILog, "PrepQMLDynamicFields");
        Log.LogRecord(Logs::Normal, Logs::UILog, "\tdynamicFields size: %i", QMLdynamicFields.size());
        for (unsigned int i = 0; i < QMLdynamicFields.size(); ++i) {
            x_dim & max_fields(QMLdynamicFields[i]);

            Log.LogRecord(Logs::Normal, Logs::UILog, "\tmax_fields size: %i", max_fields.size());

            for (unsigned int j = 0; j < max_fields.size(); ++j) {
                Log.LogRecord(Logs::Normal, Logs::UILog, "\t\trequired %i contingent %i", j , max_fields.at(j));
            }

        }
    }
}

int QMLUIProcess::GetQMLDynamicFields(bool get_required, int field_id)
{
    std::map<int, int>::iterator m_it;

    int directive = 0;
    int linked_field = 0;

    Log.LogRecord(Logs::High, Logs::UILog, "GetQMLDynamicFields:");

    // iterate through similar to above when the contingent # changes
    x_dim & max_fields(QMLdynamicFields[0]);

    Log.LogRecord(Logs::High, Logs::UILog, "max_fields size: %i", max_fields.size());

    if (get_required == true) {
        for (unsigned int i = 0; i < max_fields.size(); ++i) {
            if (max_fields.at(i) == field_id) {
                linked_field = i;
                Log.LogRecord(Logs::High, Logs::UILog, "Get DynamicField: required?(%s) field_id:(%i) required linked_field:(%i)", Log.DbgSwtch(get_required).c_str(), field_id, linked_field);
                break;
            }
        }
    }
    else {
        // need to handle fields which are linked to field 3 (4,5,6) based on directive
        if (field_id == 3) {
            Log.LogRecord(Logs::High, Logs::Test, "field_id: %i", field_id);
            //get response
            m_it = response_map.find(field_id);

            if (m_it != response_map.end()) {
                directive = response_map[field_id];
                // need to return several linked fields
            }
            else {
                linked_field = -1;
            }
        }
        else {
            linked_field = max_fields[field_id];
        }
        Log.LogRecord(Logs::High, Logs::Test, "Get DynamicField: required?(%s) field_id:(%i) contingent linked_field: %i", Log.DbgSwtch(get_required).c_str(), field_id, linked_field);
    }
    return linked_field;
}

int QMLUIProcess::QMLDynamicText(int field_id, int dynamic_text_pos)
{
    int dynamic_text_id = 0;

    dynamic_text_id = QMLdynamicTextLink[dynamic_text_pos][field_id];

    return dynamic_text_id;
}

int QMLUIProcess::GetQMLTextIDLink(int field_id)
{
    int text_id = 0;

    text_id = QMLtextIDLink[field_id];

    return text_id;
}

int QMLUIProcess::GetQMLDirective(int response_id, int iter)
{
    int element = 0;

    element = QMLdirective_from_project[response_id][iter];

    return element;
}

int QMLUIProcess::QMLDataCode(int field_id)
{
    int data_code = 0;

    data_code = QMLfieldID_to_datacodeID[field_id];

    return data_code;
}

int QMLUIProcess::QMLFieldIDFromDataCode(int data_code)
{
    int field_id = 0;

    field_id = QMLdatacodeID_to_fieldID[data_code];

    return field_id;
}
