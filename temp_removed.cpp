
Q_INVOKABLE void processClick();
Q_INVOKABLE void checkFile();
Q_INVOKABLE void processVector();
Q_INVOKABLE void checkAutoComplete();               // for text input autocompletion
Q_INVOKABLE void processQuit();                     // sole use is to log user "quit"
Q_INVOKABLE void setFocus(int focus_type);          // sends a signal to the UI on what to set the focus to
// for virtual keyboard
Q_INVOKABLE void processVKey(QString key_value);    // sends signal back to UI with key value
Q_INVOKABLE void setCaps(bool caps);                // sets capitalization output
Q_INVOKABLE void setVKenabled(int vk_enabled);      // sets variable in UI and c++ - 0 = false; 1 = true - unused

void ProcessDirectiveChoices(int directive_type, int directive_pos, int project_pos);
void ProcessStringMap();
void ProcessMap(std::string filename);
bool RemoveResponse(int field_id);
int GetDataCodeID(int field_id);

// these three will change or become deprecated with data code overhaul - useful but unecessary for this
int UIProcess::GetDataCodeID(int field_id)
{
	int data_code = 0;
	int dca_size = 0;

	dca_size = (sizeof(datacodeID_to_fieldID) / sizeof(*datacodeID_to_fieldID));
	for (int i = 0; i < dca_size; ++i) {
		if (datacodeID_to_fieldID[i] == field_id) {
			data_code = i;
			Log.LogRecord(Logs::High, Logs::UILog, "data_code: %i field_id %i", data_code, field_id);
			break;
		}
	}
	Log.LogRecord(Logs::High, Logs::UILog, "data_code %i size: %i", data_code, dca_size);
	return data_code;
}

// ---- AutoComplete function ---- //
void UIProcess::checkAutoComplete()
{
    std::vector<std::string> data_list_text_vec;
    std::vector<std::string> ac_words;
    std::string response;
    std::string trunc_res;
    QString q_response;
    size_t input_len = 0;
    size_t response_len = 0;
    int return_count = 0;
    int field_id = 0;

    // -------   autofill description   ---------
    // get fieldID to ID in the UI easier
    // get strlen of input data (s_data1)
    // link the map-vector to the i_data3
    // iterate through the strings and get string lengths
    // remove the shorter checked data strings by passing to next iteration
    // truncate all qualified data strings
    // do signal - > return the 1st match in the window/return first 10? in a menu
    // send how many words and fieldID - if 0 words, don't open menu list or close it
    // send the words prefixed by a header of the field and how many are incoming
    // send words

    if (i_data3 <= 0) {
        return;
    }

    input_len = s_data1.length();
    data_list_text_vec = file_data_map[DataFileString[i_data3]];
    field_id = GetFieldIDFromDataCode(i_data3);

    for (unsigned int i = 0; i < data_list_text_vec.size(); i++) {
        if (return_count >= 10) {
            break;
        }

        response = data_list_text_vec[i];
        response_len = response.length();

        if (response_len <= input_len) {
            continue;
        }

        q_response = QString::fromStdString(response);
        QString qtrunc_res = q_response;

        trunc_res = response.substr(0, input_len);
        qtrunc_res = QString::fromStdString(trunc_res);

        if (qtrunc_res == s_data1) {
            ++return_count;
            ac_words.push_back(response);
            q_response = QString::fromStdString(response);
            Log.LogRecord(Logs::High, Logs::UILog, "data_list_text_vec: count: %i response (%s) (%s)", return_count, response.c_str(), trunc_res.c_str());
        }
    }
    emit sendWordCount(field_id, return_count);

    if (return_count > 0) {
        for (unsigned int j = 0; j < ac_words.size(); ++j) {
            response = ac_words[j];
            q_response = QString::fromStdString(response);
            emit sendWords(true, field_id, q_response);
        }
    }

    data_list_text_vec.clear();
    ac_words.clear();
    return_count = 0;
    i_data3 = 0;
    s_data1.clear();
}

// for user name string entries, need to expand it's usage
void UIProcess::ProcessStringMap()
{
    ProcessData Pdata;
    std::string f_name;
    std::string l_name;
    std::string filename;

    l_name = string_map.at(2);
    f_name = string_map.at(1);
    string_map.clear();

    user_data_map[static_cast<float>(1.002)] = f_name;
    user_data_map[static_cast<float>(2.002)] = l_name;

    filename = l_name + ", " + f_name;
    Log.LogRecord(Logs::Normal, Logs::UILog, "[%s] edited name.", username.c_str());
    Pdata.SessionLog("[%s] has ended the session.", filename.c_str());

    // check and return a new filename if necessary
    filename = Pdata.CheckOutputFile(filename);
    std::string tar_directory = Pdata.StringFormat("%s/%s.txt", directory_vec.at(USER_DATA_FOLDER).c_str(), filename.c_str());

    Pdata.FileOpen(tar_directory, CREATE);

    // this gets replaced in the batch files
    std::string rCBATline = Pdata.StringFormat("7z a \"%s.docx\" \"C:\\MyProgram\\TestApp\\Form\\form_template\\*\"", filename.c_str());
    std::string rPBATline = Pdata.StringFormat("start /min WINWORD.EXE /q /n \"%s.docx\" /mFileCloseOrExit", filename.c_str());
    std::string rDBATline = Pdata.StringFormat("del /F /Q /A \"C:\\MyProgram\\Documents\\%s.docx\"", filename.c_str());

    // not needed for ParseFile
    Pdata.ChangeTargetName(directory_vec.at(COMPRESS_BAT), fCBATstring, rCBATline);
    Pdata.ChangeTargetName(directory_vec.at(PRINT_BAT), fPBATstring, rPBATline);
    Pdata.ChangeTargetName(directory_vec.at(DEL_FILES_BAT), fDBATstring, rDBATline);

    ProcessMap(filename);
}

// do something else - unused
void UIProcess::processQuit()
{
Log.LogRecord(Logs::Normal, Logs::UILog, "Session ended by \"Quit\".");

Log.LogRecord(Logs::Normal, Logs::UILog, "[%s] ended the session by \"Quit\".", username.length() >= 4 ? username.c_str() : "BLANK");
}

// this function will evolve as my understanding of how it works does
// there will likely be an enumerated list of what to set the focus to upon signal recv in the UI
void UIProcess::setFocus(int focus_type)
{
	if (focus_type == 1) {
		emit sendFocusType(1);
	}
}

// the result of this wil be used to populate the menu
// directive_pos = user choice from menu 1, project_pos = user choice from menu 2
void UIProcess::ProcessDirectiveChoices(int directive_type, int directive_pos, int project_pos)
{
	int code = 0;

	bool valid = true;

	if (project_pos >= maximum_projects) {
		Log.LogRecord(Logs::Normal, Logs::Error, "Invalid project:", project_pos);
		valid = false;
	}
	if (directive_pos >= maximum_directives) {
		Log.LogRecord(Logs::Normal, Logs::Error, "Invalid directive:", directive_pos);
		valid = false;
	}

	y_dim & maximum_projects(dir_type_navigation[directive_type]);

	if (valid == true) {
		code = maximum_projects[project_pos][directive_pos];
	}
	if (code <= 0) {
		Log.LogRecord(Logs::Normal, Logs::Error, "Invalid data code: Project:(%i) Directive:(%i) Data Code:(%i)", project_pos, directive_pos, code);
	}
	Log.LogRecord(Logs::Normal, Logs::UILog, "Project:(%i) Directive:(%i) Data Code:(%i)", project_pos, directive_pos, code);
}

// ---- virtual keyboard functions - <i_data2> is equivalent to the field on foucs
bool caps_lock = false;
//bool VKenabled = false; // - this shouldn't be needed

// signals:
// for virtual keyboard
void setVKenable(int n_data);
void sendKeyToField(int n_data, QString str_data);        // send VKeyboard touch emission to the data field in the UI

void UIProcess::processVKey(QString key_value)
{
    // should not be able to use the Vkeyboard where there are no text fields although this should be handled UI side
    if (i_data2 < 0) {
        emit invalidData();
        return;
    }
    std::string skey_value;
    if (key_value == "SHIFT") {
        caps_lock == false ? setCaps(true) : setCaps(false);
        //caps_lock == false ? caps_lock = true : caps_lock = false;
    }
    if (caps_lock == true && key_value.length() == 1) {
        key_value = key_value.toUpper();
    }

    skey_value = key_value.toUtf8().constData();
    Log.LogRecord(Logs::High, Logs::UILog, "processVKey: caps_lock %i key = %s", caps_lock, skey_value.c_str());
    Log.LogRecord(Logs::High, Logs::UILog, "processVKey: i_data2 %i key_value %s ", i_data2, skey_value.c_str());
    emit sendKeyToField(i_data2, key_value);
}

void UIProcess::setCaps(bool caps)
{
    if (caps == true) {
        caps_lock = true;
    }
    else {
        caps_lock = false;
    }
    Log.LogRecord(Logs::High, Logs::UILog, "UIProcess caps = %i",  caps_lock);
}

void UIProcess::setVKenabled(int vk_enabled)
{
    VKenabled = pi_data1;
    emit setVKenable(vk_enabled);
    // emit response to set variable in ui
}

// this isn't necessary with current implementation
void UIProcess::checkFile()
{
    ProcessData Pdata;
    std::string username;
    QString q_username;

    username = username_map.at(2) + ", ";
    username.append(username_map.at(1));

    q_username = QString::fromStdString(username);
    const char * c_username = username.c_str();

    Log.LogRecord(Logs::High, Logs::UILog, "UIProcess::checkFile()");

    if (std::ifstream(Pdata.StringFormat("C:/Users/Dude/Documents/Test_programs/QtTest/documents_test/%s.txt", c_username))) {
        Log.LogRecord(Logs::High, Logs::UILog, "ERROR: file already exists with name: %s", username.c_str());
        emit badName();
    }
    else {
        emit validFile();
    }
}

// currently no use but creates a comma-delimited string
void UIProcess::processVector()
{
    QString q_response;

    for (unsigned int i = 0; i < response_vec.size(); i++) {
        q_response.append(QString::fromStdString(response_vec.at(i)));
        q_response.append(",");
    }

    Log.LogRecord(Logs::High, Logs::UILog, "response = %s", q_response.c_str());
    emit processedData();
}

// processClick adds string data into map
// handles button click data storage
void UIProcess::processClick()
{
    if (i_data1 < 1) { // responseID (ie. menu choice)
        i_data1 = 0;
    }
    if (i_data2 < 1) { // fieldID
        i_data2 = 0;
    }

    std::map<float, std::string>::iterator it;
    std::string s_response;

    float pr_stat;
    int confirm_status = 0;

    bool replace = false;
    bool reset = false;

    confirm_status = GetConfirmID(i_data2);
    replace = ReplacePrevData(confirm_status);

    Log.LogRecord(Logs::Med, Logs::UILog, "processClick replace %i confirm_status %i fieldID %i  ", replace, confirm_status, i_data2);
    if (replace == true) { // for most button pages except 4button (TestApp)
        reset = RemoveResponse(i_data2);
    }
    else { // for multiple answers to one field
        Log.LogRecord(Logs::High, Logs::UILog, "processClick insert %i", i_data1);
        pr_stat = i_data2 + static_cast<float>(i_data1) / 1000;
        it = response_map.find(pr_stat);

        if (it != response_map.end()) {
            response_map.erase(it);
            Log.LogRecord(Logs::High, Logs::UILog, "processClick insert-erase same %i", i_data1);
        }
        else {
            s_response = GetStringResponse(i_data2, i_data1);
            response_map[pr_stat] = s_response;
            Log.LogRecord(Logs::High, Logs::UILog, "processClick insert-add new %i", i_data1);
        }
    }
    if (reset == true) {
        s_response = GetStringResponse(i_data2, i_data1);
        pr_stat = i_data2 + static_cast<float>(i_data1) / 1000;
        response_map[pr_stat] = s_response;

        Log.LogRecord(Logs::Normal, Logs::UILog, "processClick: reset -> responce %s response %i pr_stat %1.3f", s_response.c_str(), i_data1, pr_stat);
    }

    ProcessDynamicMenu(true, i_data2);

    ProcessDynamicText(true, i_data2);

    i_data1 = 0;
    i_data2 = 0;
}

void UIProcess::ProcessMap(std::string filename)
{
    //ProcessData Pdata;
    //std::map<float, std::string>::iterator m_it;

    std::string response;
    float page;

    for (m_it = response_map.begin(); m_it != response_map.end(); ++m_it) {
        page = m_it->first;
        response = m_it->second;
        user_data_map[page] = response;
    }
    //Pdata.ProcessUserData(filename);
}

// tied to processClick() map with strings
bool UIProcess::RemoveResponse(int field_id)
{
    std::map<float, std::string>::iterator it;
    int data_code = 0;
    int response;
    bool reset = true;


    Log.LogRecord(Logs::High, Logs::UILog, "RemoveResponse1");
    for (it = response_map.begin(); it != response_map.end(); ++it) {
        Log.LogRecord(Logs::High, Logs::UILog, "RemoveResponse2");
        if (field_id == static_cast<int>(it->first)) {
            response = (it->first - field_id) * 1001;
            data_code = GetDataCodeFromFieldID(field_id);
            response_map.erase(it);

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


// ParseFunctions

void GetNewInputFilenames(std::string file_ext); // unused - this searches the file in memory from the previous step for any new/missing filenames and puts new files into memory for PDirective 1 code addition

// unused - this needs to check "parse_indir_vec" (the infile directory) for files - or separate the files further down the line
void ParseFunctions::GetNewInputFilenames(std::string file_ext)
{
	std::string source_line;
	std::string out_file_line;

	std::size_t line_pos = 0;

	bool exists = false;

	// cycle through all files for possible parse
	for (unsigned int i = 0; i < input_filename_vec.size(); i++) {
		source_line = input_filename_vec.at(i);
		exists = false;

		// check these files for target parse file extension - safeguard, not necessary with current system
		line_pos = source_line.find(file_ext);
		if (line_pos != std::string::npos) {

			// parse output file for target files(string)
			for (unsigned int j = 0; j < output_file_data_vec.size(); j++) {
				out_file_line = output_file_data_vec.at(j);

				// find the filename with the accpetable ext
				line_pos = out_file_line.find(source_line);
				if (line_pos != std::string::npos) {
					exists = true;
					Log.LogRecord(Logs::High, Logs::General, " Exists: %s", source_line.c_str());
					break;
				}
			}
			// filename does not exist in .h file
			if (exists == false) {
				Log.LogRecord(Logs::High, Logs::General, " New file found: %s", source_line.c_str());
				transfer_string_vec.push_back(source_line);
			}
		}
	}
}
