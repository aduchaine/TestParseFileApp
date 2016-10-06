#ifndef UIPROCESS_H
#define UIPROCESS_H

#include <QObject>


#define NAME_CHAR "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890`~-\'. "

// redo this eventually
// may not be needed with "DataCodes" - the only usage is to get mundane "ResponseString" above
// not true, it won't know to toggle other buttons otherwise - although it could use the array instead - redo this eventually
// question and response are mapped and used in the files for id
// --- // -- this is for organizational purposes mostly - so people can understand what a flag means
namespace Question {

    // sets flag for number and type of acceptable responses per fieldID
    enum ConfirmType {

        No_Confirmation = 0,
        PROCESS_DATA,
        STRING_VALIDATE,
        SINGLE_REQ,
        SINGLE_ACC,
        MULTIPLE_REQ,
        MaxConfirmType

    };

}

// most of this will be used for ParseFile
class UIProcess : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString ps_data1 READ ps_data1 WRITE setS_data1)        // used to send strings to the c++ side
    Q_PROPERTY(int pi_data1 READ pi_data1 WRITE setI_data1)            // used to send intergers to the c++ side
    Q_PROPERTY(int pi_data2 READ pi_data2 WRITE setI_data2)            // used to send intergers to the c++ side
    Q_PROPERTY(int pi_data3 READ pi_data3 WRITE setI_data3)            // used to send intergers to the c++ side
    Q_PROPERTY(bool pb_data1 READ pb_data1 WRITE setB_data1)           // used to send bool values to the c++ side

public:
    UIProcess(QObject *parent = 0);

    // UI-used variables
    QString ps_data1() const;
    void setS_data1(const QString &ps_data1);

    int pi_data1() const;
    void setI_data1(const int &pi_data1);

    int pi_data2() const;
    void setI_data2(const int &pi_data2);

    int pi_data3() const;
    void setI_data3(const int &pi_data3);

    bool pb_data1() const;
    void setB_data1(const bool &pb_data1);

    Q_INVOKABLE void parseSelf(bool initial, bool reparse);                                             // setup routine
    Q_INVOKABLE void processDirectiveResult(int directive_type, int from_field, bool process_update);   // setup routine

    Q_INVOKABLE void pageProcess(int page_id);          // process all page functions on a stackview change
    Q_INVOKABLE void getPreviousPage(int current_page); // very simple signal function to keep page navigation at stackView

    Q_INVOKABLE void processClick();                    // records data on each click instance
    Q_INVOKABLE void confirmData(int question);         // intial data processing
    Q_INVOKABLE void checkStringInput();                // used for confirming text input

    void ProcessStart();                                // backend kickoff

    // new
    void GetProjectFromDirectiveType(int directive_type);
    void PrepDirTypeArrayData(int directive_type);
    void HandleSignals(int signal_type, int directive, int signal_code, int i_signal_data, std::string s_signal_data);
    std::string GetSignalInputText(int directive, int signal_type);

    void SendTextFieldData(int page_id, bool no_process);       // sends prompt text
    void SendFieldData(int page_id, bool no_process);        // for button text/responses - will need to account for switches, etc
    void SendMenuData(int page_id, bool no_process);         // for menu data when a page is loaded
    void GetNextData(int page_id, bool no_process);          // used to change field properties/states when navigating

    // new dynamic text/menu system
    void PrepDynamicFieldData(bool show_values);            // show_values is for debugging
    void ProcessDynamicText(bool on_click, int field_id);

    void HandleDynamicText(bool next_pos, int field_id);
    void ProcessDynamicMenu(bool on_click, int field_id);
    void HandleMenuLists(bool on_click, int field_id);

    int GetDynamicField(bool get_required, int field_id);

    void ProcessMap();

    // utility functions
    void HandleFieldLabels(int load_type, int field_id, int linked_field);
    int GetResponseIDFromMap(int field_id);
    bool RemoveResponse(int field_id);
    bool CheckFieldProcess(int field_id);
    int GetPageProcesses(int page_id);
    int IsValidInput(std::string p_input, bool check_acceptable, bool check_restricted, bool check_length, const char * valid_chars, const char * invalid_chars, int min_length, int max_length);
    std::string GetStringResponse(int field_id, int num_response);
    bool ValidateString(int field_id);

    // tiny utility functions to make acquiring the data easier
    // all of these tie in to page process, confirmation, page data population, etc
    void GetFieldID(int page_id);
    void GetTextID(int page_id);
    int GetPageID(int field_id);
    int GetConfirmID(int field_id);
    int GetFieldIDFromDataCode(int data_code);
    int GetDataCodeFromFieldID(int field_id);
    int GetNumInputFields(int page_id);
    int GetNumTextFields(int page_id);
    int GetMinPageProcesses(int page_id);
    bool ReplacePrevData(int confirm_status);
    int GetNumResponses(int field_id);
    void GetResponseIDs(int field_id);    

    // for UI data tranfer - variables passed must have the same name in the UI to use the data - combine and rename these where necessary
signals:

    void sendFocusType(int s_data);                             // sends focus type command to set focus

        // to populate menus/buttons and autocomplete
    void sendWords(bool f_switch, int d_data, QString str_data);// sends data_code/words for auto-complete
    void sendWordCount(int n_data, int s_data);                 // sends word_count/field_id as header - (field_id, w_count)
    void sendToField(int n_data, int s_data, QString str_data); // sends button text labels/responses (field_id, responseID, response_string)
    void sendToPrompt(int n_data, QString str_data);            // sends field text (textID, text)
    void dynamicText(int d_data, int n_data, QString str_data); // test - sends dynamic text - d_data(how to change), n_data(field/textID) , str_data(text string)

    // affects navigation
    void previousPage(int n_data);                              // very simple signal to keep page navigation at stackView
    void validData(int n_data);                                 // used on each page to signal data acceptance and navigation choice, returns next page#
    void processedData();                                       // used to signal program closure
    void noData();                                              // no data recorded, error popup
    void invalidData();                                         // no data recorded, error popup with specific text

    // UI state change signals
    void beginApp(int n_data);                                  // used to signal completion of backend app preparation processing
    void stringData(QString str_data, int str_pos);             // used to send string data when navigating back/forward to fill textedit fields - behaves funny
    void changeState(int s_data, bool f_switch);                // used to change button properties when only a single state change is required
    void dataCode(int n_data, int s_data);                      // used to send click data_code when next clicked to change button properties/states if navigating back/forward
    void fieldState(int n_data, int s_data);                    // field state changes - n_data = fieldID, s_data = transition type

    // these are used specifically for data sent from the UI - transfer data to a different variable quickly
private:
    QString s_data1;
    int i_data1;
    int i_data2;
    int i_data3;
    bool b_data1;

};

extern const size_t maximum_directives;    // x_dim - maximum directives + 1 in any project = "MAX_DIRECTIVES"
extern const size_t directive_types;		// z_dim - number of directive_types + 1 = MaxDirectiveType
extern const int DefaultValue;

extern std::vector<int> projects_vec;

extern std::map<int, int> response_map;
extern std::map<float, std::string> user_data_map;
extern std::vector<std::string> text_label_vec;         // initially for menu labels
extern std::vector<int> fieldID_vec;
extern bool startup;
extern bool prompt_set;

#endif // UIPROCESS_H
