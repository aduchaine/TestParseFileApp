#include <iostream>

#include "Logging/logging.h"

#include "qmlparse.h"
#include "qmlparsefunction.h"

#include "parsefiles.h"
#include "parsedirectory.h"
#include "parsefunctions.h"

#include "uiprocess.h"


const int pd_type = 1;
int p_dir = 0;
int subdir_status = 0;


// the code for F_1 and F_2 (data_codes) needs quite a bit of work with recent changes, it is almost useless
QMLParse::QMLParse()
{

}

void QMLParse::QMLMainParse(int parse_directive, bool reparse)
{
    ParseFiles PF;
	ParseFunctions PFunc;

	QMLParseFunction QPFunc;

    UIProcess UI;

	std::map<int, std::string>::iterator m_it;
    std::map<int, int>::iterator mi_it;

    std::string out_dir;
	std::string out_file;
	std::string out_path;
    std::string in_file;
    std::string file_ext;

    std::string utility_string;

    int p_func_count = 0;
    int p_subdir_count = 0;
    int p_func_id = 0;
    int directive_code = 0;

    unsigned int fbitmask_value = 0;
    int fbit_pos = 0;
    int supp_op = 0;

    int num_bits = 0;

    bool utility_switch = false;

    // proposed user input or test variables
    int ui_page_num = -1; // user input page number for all pages this must be set to -1, currently
    int user_choice = 0; // user input for test change .cpp array or .qml page file  1 = change_file 2 = change_array
    std::string user_file; // user input for file naming
    bool user_switch = false; // user input for(2): change/sync or get componentIDs - reordering pageIDs or inserting a page within the order
    bool do_all = false; // poss user input or a switch based on input

/*
- do a function to handle singal codes and choices
    - we may need to skip the variables above or reset them to the appropriate values
    - all the values which were somehow not saved will need to be set
    - some directives will only need a subdir_status set (ones with subdirectives and a bitmasking filter)
    - many of the variables' values set above will be set properly within the directive as it reparses
    - ones which need to be set here to filter properly are: fbit_pos, directive_code, user choices(to set everything else)
        - "directive_code" and user choices need to be accessed here and set within Reparse Setup() or parse Self() - done
            - the "directive_code" will determine what the data-type of the user choice needs to be processed
            - most user choices will be defined and can be enumerated and use a single map container, some will require a string container
            - this should probably use a vector or a global variable
        - the "directive_code"
*/

    p_dir = parse_directive;

    // likely need to add the "reparse" flag at each choice which would currently just continue through the parse
    // in some cases, the reparse flag can overwrite the startup flag
    if (reparse == true) {
        //subdir_status -= PF.GetBitValue(0, false); // removes subdir flag to get data from the subdirective where necessary      

        // need to get the main directive if the value passed refers to a subdirective either here or in reparse()


        // this could/should probably be "dirstat_list.size()" if elements > 0
        if (reparse_code_map.size() == 1) {
            for (mi_it = reparse_code_map.begin(); mi_it != reparse_code_map.end(); ++mi_it) {
                directive_code = mi_it->first;
                user_choice = mi_it->second;
                Log.LogRecord(Logs::High, Logs::Test, "reparse  directive_code(%i) user_choice(%i)", directive_code, user_choice);
            }
        }
        // error, should only have one entry - need end_parse, do write report when mapsize > 1
        else {
            for (mi_it = reparse_code_map.begin(); mi_it != reparse_code_map.end(); ++mi_it) {
                directive_code = mi_it->first;
                utility_string.append(std::to_string(directive_code) + PFunc.PStr(505));
            }
            Log.LogRecord(Logs::Normal, Logs::Error, "Reparse error: only one directive code can be processed - code count(%i) codes< %s>", reparse_code_map.size(), utility_string.c_str());
            end_parse = true;
            utility_string.clear();
            return;
        }


        // PD 3 no choices PD 1,2,4 choices
        if (parse_directive < 5) {
            fbit_pos = 3;
        }
        else {
            fbit_pos = 0;
        }

        if (parse_directive == 1) {
            if (directive_code == 24) { // 24 uses the same fbit_pos and subdir_status

                // possible calc_subdir_status function here if (fbit_pos > 0)
                if (user_choice == 1) {
                    subdir_status += PF.GetBitValue((fbit_pos + 1) - 1, false); // flag c page choice
                }
                else if (user_choice == 2) {
                    subdir_status += PF.GetBitValue((fbit_pos + 2) - 1, false); // flag d array choice
                }
                Log.LogRecord(Logs::High, Logs::Test, "Reparse: Dir(%i) Code(%i) Input(%i) subdir_status(%i)", parse_directive, directive_code, user_choice, subdir_status);
            }
        }

        if (parse_directive == 2) {
            if (directive_code == 19) {
                if (user_choice == 1) {
                    subdir_status += PF.GetBitValue((fbit_pos + 1) - 1, false); // flag c page choice
                }
                else if (user_choice == 2) {
                    subdir_status += PF.GetBitValue((fbit_pos + 2) - 1, false); // flag d array choice
                }                
                Log.LogRecord(Logs::High, Logs::Test, "Reparse: Dir(%i) Code(%i) Input(%i) subdir_status(%i)", parse_directive, directive_code, user_choice, subdir_status);
            }
        }

        if (parse_directive == 4) {

            Log.LogRecord(Logs::High, Logs::Test, "\tparse_directive %i:SUB_STATUS_%i:CODE_%i", parse_directive, subdir_status, directive_code);
            if (user_choice == 3) {
                if (directive_code == 13) {
                    // set to rewrite main
                    subdir_status += PF.GetBitValue((fbit_pos + 1) - 1, false); // flag c main.qml choice
                    Log.LogRecord(Logs::High, Logs::Test, "P_DIR_%i:SUB_STATUS_%i:CODE_%i", parse_directive, subdir_status, directive_code);
                }

                if (directive_code == 14) {
                    // set to rewrite pages
                    subdir_status += PF.GetBitValue((fbit_pos + 2) - 1, false); // flag d .qml pages choice
                    Log.LogRecord(Logs::High, Logs::Test, "P_DIR_%i:SUB_STATUS_%i:CODE_%i", parse_directive, subdir_status, directive_code);

                }
            }

            Log.LogRecord(Logs::High, Logs::Test, "Reparse: Dir(%i) Code(%i) Input(%i) subdir_status(%i)", parse_directive, directive_code, user_choice, subdir_status);
        }
        Log.LogRecord(Logs::High, Logs::Test, "reparse - P_DIR_%i:SUB_STATUS_%i:CODE_%i", parse_directive, subdir_status, directive_code);

        // for testing
        //end_parse = true;
        //return;
    }

    // for case 1,2 we need to determine if DoDirectiveCleanup() will break it - doesn't appear to under controlled conditions
    switch (parse_directive) {
    // Setup and Check main.qml
    case 1: // process main .qml data - Subdirective -1 - check/create directories, retrieve main.qml and array comp codes

        num_bits = 5;
        fbit_pos = 3;

        if (PF.CheckBitFlags(num_bits, subdir_status, 1) == false) { // subdir flag unset
            QMLSubdirective(-1, p_func_count, p_func_count);
        }
        p_func_count = p_func_count + PF.GetSubdirectiveFunctionCt(pd_type, -1);

        if (PF.CheckBitFlags(num_bits, subdir_status, 1) == false) { // subdir flag unset - can't do anything beyond this w/o 1 of 2 files
            subdir_status = 0;
            end_parse = true;
            break;
        }

        PF.DoDirectiveCleanup(false, false, false); // increase "p_func_count" to bypass the subdirective trigger in "parse_directive_vec/parseFunc_from_parseDir"
        ++p_func_count;
        Log.LogRecord(Logs::Normal, Logs::General, " Resuming Main Directive");
        PF.GetParsePaths(pd_type, p_func_count);


        // F_28 QML_CREATE_MAIN - does not return a "directive_code"
        if (PF.CheckBitFlags(num_bits, subdir_status, 2) == true) { // flag a set
            if (startup == true) {
                end_parse = true;
                break;
            }
            else if (reparse == false) {
                break;
            }
            p_func_id = PF.GetFunctionID(p_func_count);
            fbitmask_value = PF.GetBitValue(fbit_pos - 1, false) + PF.GetBitValue((fbit_pos + 1) - 1, false); // p_subdir_count + outpath option
            directive_code = HandleFunctions(parse_directive, p_func_id, 2, p_func_count, p_subdir_count, fbitmask_value);
            subdir_status = 0;
            prompt_set = false;
            break;
        }
        ++p_func_count;
        ++p_subdir_count;


        // F_19 - CPP_ARRAY_CHGMAINCOMPCODES
        if (PF.CheckBitFlags(num_bits, subdir_status, 3) == true) { // flag b set
            if (reparse == false) {
                break;
            }
            //if (startup == true) { break; }

            p_func_id = PF.GetFunctionID(p_func_count);
            directive_code = HandleFunctions(parse_directive, p_func_id, 3, p_func_count, p_subdir_count, PF.GetBitValue(fbit_pos - 1, false));

            // assure this is the right thing to do
            if (directive_code == 0) {
                subdir_status = 0;
                prompt_set = false;
                break;
            }
            else { // no stringsIDs tranferred, flag b should not be set or could not find keywords - need to test
                //return_status for failure
                Log.LogRecord(Logs::Normal, Logs::Error, "Program Issue: cannot continue - FUNC_ID_%i:SUB_STATUS_%i:CODE_%i", p_func_id, subdir_status, directive_code);
                subdir_status = 0;
                end_parse = true;
                break;
            }
        }
        ++p_func_count;
        ++p_subdir_count;


        // F_17 - this will be a problem w/o any user input, should probably do a notify when startup is complete but don't write
        if (subdir_status == PF.GetBitValue(1 - 1, false)) { // subdir flag set, flag a, b, c, d unset
            QPFunc.CompareCompCodes(true);

            if (process_required == true) {
                directive_code = 24; // rewrite array or .qml file
                PF.AddToDirStatList(pd_type, p_dir, parse_directive, p_func_id, directive_code, false);
                prompt_set = true;
                break;
            }
            else if (end_parse == true) {
                directive_code = 20; // parse incomplete, cannot continue
                PF.AddToDirStatList(pd_type, p_dir, parse_directive, p_func_id, directive_code, false);
                break;
            }
        }

        // differences exist if we are at the next two
        // F_21 - QML_MAIN_CHANGE
        if (PF.CheckBitFlags(num_bits, subdir_status, 4) == true) { // flag c set from user_choice
            QPFunc.CompareCompCodes(false);
            if (process_required == true) {
                p_func_id = PF.GetFunctionID(p_func_count);
                directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_subdir_count, PF.GetBitValue(fbit_pos - 1, false));
                if (directive_code == 5) { // no output_file_data and no explanation
                    subdir_status = 0;
                    end_parse = true;                    
                    break;
                }
                else { // if we got here we are done
                    subdir_status = 0;
                    prompt_set = false;
                    break;
                }
            }
        }
        // F_19 - CPP_ARRAY_CHGMAINCOMPCODES
        if (PF.CheckBitFlags(num_bits, subdir_status, 5) == true) { // if (flag d set from user_choice)            
            p_func_count -= 2;
            p_subdir_count -= 2;

            QPFunc.CompareCompCodes(true);
            if (process_required == true) {
                p_func_id = PF.GetFunctionID(p_func_count);
                directive_code = HandleFunctions(parse_directive, p_func_id, 3, p_func_count, p_subdir_count, PF.GetBitValue(fbit_pos - 1, false));
                if (directive_code == 13) { // we're done - rethink setting this dir_code
                    subdir_status = 0;
                    prompt_set = false;
                    break;
                }
                else { // no stringsIDs tranferred, flag d should not be set                                        
                    Log.LogRecord(Logs::Normal, Logs::Error, "Program Issue: cannot continue - FUNC_ID_%i:SUB_STATUS_%i:CODE_%i", p_func_id, subdir_status, directive_code);
                    subdir_status = 0;
                    end_parse = true;
                    break;
                }
            }
        }
        ++p_func_count;
        ++p_subdir_count;


        if (subdir_status == 1) { // no changes needed
            Log.LogRecord(Logs::High, Logs::General, " End Main Directive - subdir_status %i", subdir_status);
            subdir_status = 0;
        }
        else {
            Log.LogRecord(Logs::Normal, Logs::Error, "Invalid status (%i): check program logic", subdir_status);
            subdir_status = 0;
            end_parse = true;
        }
        prompt_set = false;
        break;

    // Check QML Pages
    case 2: // process page .qml data - Subdirective -2 - match page IDs to filenames, retrieve page .qml and array comp codes

        num_bits = 6;
        fbit_pos = 3;

        if (PF.CheckBitFlags(num_bits, subdir_status, 1) == false) { // subdir flag unset
            QMLSubdirective(-2, p_func_count, p_func_count);
        }
        p_func_count = p_func_count + PF.GetSubdirectiveFunctionCt(pd_type, -2);

        if (PF.CheckBitFlags(num_bits, subdir_status, 1) == false) { // subdir flag unset - can't do anything beyond this w/o 1 of 2 files
            subdir_status = 0;
            end_parse = true;            
            break;
        }


        PF.DoDirectiveCleanup(false, false, false);
        ++p_func_count;
        Log.LogRecord(Logs::Normal, Logs::General, " Resuming Main Directive");
        PF.GetParsePaths(pd_type, p_func_count);


        // F_9 QML_PG_CREATE -- need to go back to the beginning of this directive if pages created or do F_5 again after this runs
        if (PF.CheckBitFlags(num_bits, subdir_status, 2) == true || PF.CheckBitFlags(num_bits, subdir_status, 6) == true) { // flag a or e set
            if (reparse == false) {
                break;
            }
            //if (startup == true) { break; }

            p_func_id = PF.GetFunctionID(p_func_count);

            if (PF.CheckBitFlags(num_bits, subdir_status, 2) == true) {
                directive_code = HandleFunctions(parse_directive, p_func_id, 2, p_func_count, p_subdir_count, 0);
            }
            else {
                directive_code = HandleFunctions(parse_directive, p_func_id, 2, p_func_count, p_subdir_count, PF.GetBitValue(fbit_pos - 1, false));
            }

            if (directive_code == 16) { // no strID comp codes
                subdir_status = 0;
                end_parse = true;
                break;
            }
            else { // if we got here we're done but this function will get changed
                subdir_status = 0;
                prompt_set = false;
                break;
            }
        }
        ++p_func_count;
        ++p_subdir_count;


        // F_12 CPP_ARRAY_CHGCOMPCODES
        if (PF.CheckBitFlags(num_bits, subdir_status, 3) == true) { // flag b set
            if (reparse == false) {
                break;
            }
            //if (startup == true) { break; }

            p_func_id = PF.GetFunctionID(p_func_count);
            directive_code = HandleFunctions(parse_directive, p_func_id, 3, p_func_count, p_subdir_count, PF.GetBitValue(fbit_pos - 1, false)); // use p_subdir_count
            if (directive_code == 14) { // we're done - rethink setting this dir_code - this code applies to PD 4
                subdir_status = 0;
                prompt_set = false;
                break;
            }
            else { // no stringsIDs tranferred, flag b should not be set
                Log.LogRecord(Logs::Normal, Logs::Error, "Program Issue: cannot continue - FUNC_ID_%i:SUB_STATUS_%i:CODE_%i", p_func_id, subdir_status, directive_code);
                subdir_status = 0;
                break;
            }
        }
        ++p_func_count;
        ++p_subdir_count;


        // F_17 - this will be a problem w/o any user input, should probably do a notify when startup is complete
        if (subdir_status == PF.GetBitValue(1 - 1, false)) { // subdir flag set, flag a, b, c, d unset
            QPFunc.CompareCompCodes(true);

            if (process_required == true) {
                directive_code = 19;
                PF.AddToDirStatList(pd_type, p_dir, parse_directive, p_func_id, directive_code, false);
                prompt_set = true;
                Log.LogRecord(Logs::High, Logs::Test, " F_17 process_required ");
                break;
            }
            else if (end_parse == true) {
                directive_code = 20;
                PF.AddToDirStatList(pd_type, p_dir, parse_directive, p_func_id, directive_code, false);
                subdir_status = 0;
                break;
            }
        }

        // differences exist if we are at the next two
        // F_11 QML_PG_CHANGE
        if (PF.CheckBitFlags(num_bits, subdir_status, 4) == true) { // flag c set from user_choice
            Log.LogRecord(Logs::Normal, Logs::General, "  Synchronizing all .qml page's components to .cpp array.");

            p_func_id = PF.GetFunctionID(p_func_count);
            PF.PrepareFunction(pd_type, 2, p_func_count, p_subdir_count);
            out_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 1);

            // this needs testing -- appears to work
            // allows proper data arrangement - data arrangement is done below
            for (unsigned int i = 0; i < content_strID_vec.size(); ++i) {
                if (i == 0) {
                    PFunc.TransferVec(1, 0, true);
                }
                PFunc.TransferVec(1, content_strID_vec.at(i), false);
            }
            if (pageID_to_path_map.size() > 0) {
                for (m_it = pageID_to_path_map.begin(); m_it != pageID_to_path_map.end(); ++m_it) {
                    out_path = m_it->second;
                    PF.GetOutputData(out_path);

                    QPFunc.ArrangeCompIDsintoVec(m_it->first); // data arrangement - appears to work
                    QPFunc.CompareCompCodes(false);

                    if (process_required == true) {
                        Log.LogRecord(Logs::Normal, Logs::General, "\t\t...processing changes for page %i", m_it->first);
                        QPFunc.QML_PG_CHANGE(m_it->first); // F_11 - this would work in many cases but we need a way to alter the file in a smaller way
                        PF.FileReWrite(out_path, true);
                    }
                    output_file_data_vec.clear();
                }
                subdir_status = 0;
                prompt_set = false;
                break;
            }
            else {
                directive_code = 17;
                PF.AddToDirStatList(pd_type, p_dir, parse_directive, p_func_id, directive_code, false);
                Log.LogRecord(Logs::Normal, Logs::Error, "Program Issue: cannot continue - FUNC_ID_%i:SUB_STATUS_%i:CODE_%i", QML_PG_CHANGE, subdir_status, directive_code);
                subdir_status = 0;
                end_parse = true;
                break;
            }
        }

        // F_12 CPP_ARRAY_CHGCOMPCODES
        if (PF.CheckBitFlags(num_bits, subdir_status, 5) == true) { // flag d set from user_choice
            --p_func_count;
            --p_subdir_count;

            QPFunc.CompareCompCodes(true);
            if (process_required == true) {
                p_func_id = PF.GetFunctionID(p_func_count);
                directive_code = HandleFunctions(parse_directive, p_func_id, 3, p_func_count, p_subdir_count, PF.GetBitValue(fbit_pos - 1, false)); // use p_subdir_count
                if (directive_code == 14) { // we're done - rethink setting this dir_code - this code applies to PD 4
                    subdir_status = 0;
                    prompt_set = false;
                    break;
                }
                else { // no stringsIDs tranferred, flag d should not be set
                    Log.LogRecord(Logs::Normal, Logs::Error, "Program Issue: cannot continue - FUNC_ID_%i:SUB_STATUS_%i:CODE_%i", p_func_id, subdir_status, directive_code);
                    end_parse = true;
                    break;
                }
            }
            ++p_func_count;
            ++p_subdir_count;
        }

        if (subdir_status == 1) {
            Log.LogRecord(Logs::High, Logs::General, " End Main Directive - subdir_status %i", subdir_status);
            subdir_status = 0;
        }
        else {
            Log.LogRecord(Logs::Normal, Logs::Error, "Invalid status (%i): check program logic", subdir_status);
            subdir_status = 0;
            end_parse = true;
        }
        prompt_set = false;
        break;

    // Check File Dependencies
    case 3: // populate dependency files - we don't need to do much with the flag settings here, this will get taken care of at the end of this function
        // set a flag to give feedback on file tranfers possibly with a global counter

        // F_18 - CPP_ARRAY_GETMAINCOMPCODES
        fbit_pos = 3;
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, PF.GetBitValue(fbit_pos - 1, false));
        // directive_code 1,3 - 1 no comp codes, flag to choose write but can continue this directive
        if (directive_code == 3) { // no file - can't do this w/o .cpp file
            end_parse = true;
            break;
        }
        ++p_func_count;


        // F_29 - QML_MAIN_GETITEMNOCOMPCODE
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 1, p_func_count, p_func_count, 0);
        if (directive_code == 3) { // no file but can continue, major warning prompt           
            Log.LogRecord(Logs::Normal, Logs::Error, "Program Issue: - FUNC_ID_%i:CODE_%i", p_func_id, directive_code);
        }
        ++p_func_count;


        // F_16 - FILE_CHECK_DEPEND
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code != 0) { // directive_code = 12,25 - 12 program logic or .txt files missing, 25 manually check for .qml files
            end_parse = true;
            break;
        }
        // directive_code = 12 - need to manually check for files but can continue
        ++p_func_count;


        // F_8 - CPP_ARRAY_GETCOMPCODES
        fbit_pos = 3;
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, PF.GetBitValue(fbit_pos - 1, false));
        // directive_code = 2,3,8,23 - 2 no comp codes but can continue, flag to sync array, 8 more array pages than file pages, flag to write pages, 23 fewer array pages than file pages, nothing setup
        if (directive_code == 3) { // no file
            end_parse = true;
            break;
        }        
        prompt_set = false;
        p_func_count = 2;


        // F_16 - FILE_CHECK_DEPEND
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code != 0) { // directive_code = 12,25 - 12 program logic or .txt files missing, 25 manually check for .qml files
            end_parse = true;
            break;
        }
        ++p_func_count;
        break;

    // Contingent Components
    case 4: // component checking/insertion/deletion for contingent components,

        num_bits = 3;

        // case 4 after write, reparse subdir for additional changes

        if (PF.CheckBitFlags(num_bits, subdir_status, 1) == false) { // subdir flag unset
            QMLSubdirective(-3, p_func_count, p_func_count);
        }

        if (PF.CheckBitFlags(num_bits, subdir_status, 1) == false) { // subdir flag unset
            subdir_status = 0;
            end_parse = true;
            break;
        }


        // assure temp data should be cleared
        PF.DoDirectiveCleanup(false, false, true);
        p_func_count = p_func_count + PF.GetSubdirectiveFunctionCt(pd_type, -3);
        ++p_func_count;
        Log.LogRecord(Logs::Normal, Logs::General, " Resuming Main Directive");
        PF.GetParsePaths(pd_type, p_func_count);


        if (PF.CheckBitFlags(num_bits, subdir_status, 2) == true) { // flag a set
            if (reparse == false) {
                break;
            }
            Log.LogRecord(Logs::Normal, Logs::Test, "-4 Resuming Main Directive");
            QMLSubdirective(-4, p_func_count, p_subdir_count);
            Log.LogRecord(Logs::Normal, Logs::General, " Resuming Main Directive");
            if (subdir_status > 0) {

            }
        }


        // assure temp data should be cleared
        PF.DoDirectiveCleanup(false, false, true);
        p_func_count = p_func_count + PF.GetSubdirectiveFunctionCt(pd_type, -4);
        ++p_func_count;
        PF.GetParsePaths(pd_type, p_func_count);


        if (PF.CheckBitFlags(num_bits, subdir_status, 3) == true) { // flag b set
            if (reparse == false) {
                break;
            }
            QMLSubdirective(-5, p_func_count, p_subdir_count);
            Log.LogRecord(Logs::Normal, Logs::General, " Resuming Main Directive");
            if (subdir_status > 0) {
                // no signal handling
                Log.LogRecord(Logs::Normal, Logs::Test, "case -5  error");
                end_parse = true;
                break;
            }
        }
        ++p_func_count;

        Log.LogRecord(Logs::High, Logs::General, " End Main Directive - subdir_status %i", subdir_status);
        prompt_set = false;
        break;

    // Check Component IDs
    case 5: // sync or get content compIDs - this cannot and should not be used when changing pages' components, this would be used before/after

        user_switch = true; // write = true, get compIDs = false

        // F_5 FILE_MATCH_PG
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code == 4) { // cannot continue - flag write page
            end_parse = true;
            break;
        }
        ++p_func_count;


        // F_10 QML_PG_GETCOMPCODES
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code != 0) { // directive_code = 7, 17 - cannot continue - 7 flag write page
            if (directive_code == 17) {
                end_parse = true;
                break;
            }
            if (reparse == false) {
                break;
            }
        }
        ++p_func_count;

        Log.LogRecord(Logs::High, Logs::Test, " pageID_to_path_map %i", pageID_to_path_map.size());


        // F_13 QML_PG_PROCESSCOMPIDS
        if (user_switch == true) {
            fbit_pos = 3;
            fbitmask_value = PF.GetBitValue(fbit_pos - 1, false);
        }
        else {
            fbitmask_value = 0;
        }
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, fbitmask_value);
        if (directive_code != 0) { // directive_code = 9, 17 - cannot continue
            Log.LogRecord(Logs::Normal, Logs::Error, "Program Issue: cannot continue - FUNC_ID_%i:SUB_STATUS_%i:CODE_%i", QML_PG_CHANGE, subdir_status, directive_code);
            end_parse = true;
            break;
        }
        ++p_func_count;
        prompt_set = false;
        break;

        // Process Global Logic IDs
    case 6: // sync all .qml file global logic IDs TESTAPP - menuID, textID, fieldID, responseID

        // F_5 FILE_MATCH_PG
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code == 4) { // cannot continue - flag write page            
            end_parse = true;
            break;
        }
        ++p_func_count;


        // F_6 CPP_ARRAY_GETLOGICIDS
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code != 0) {
            if (directive_code == 18) { // array not setup or no entries - cannot continue                
                end_parse = true;
                break;
            }
            else { // directive_code = 3                
                end_parse = true;
                break;
            }
            break;
        }
        ++p_func_count;


        // F_7 QML_PG_UPDATELOGICIDS
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code != 0) { // directive_code = 17
            Log.LogRecord(Logs::Normal, Logs::Error, "Program Issue: cannot continue - FUNC_ID_%i:CODE_%i", p_func_id, directive_code);
            end_parse = true;
            break;
        }
        ++p_func_count;
        prompt_set = false;
        break;

        // Support Components
    case 7: // parse into all SupportComponents files

        // these 3 can probably be a subdir

        // F_5 FILE_MATCH_PG
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code == 4) { // cannot continue - flag write page
            break;
        }
        ++p_func_count;


        // F_14 QML_PG_ORDER
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code != 0) {
            if (directive_code == 10) { // manually check file                
                break;
            }
            else { // directive_code = 17
                Log.LogRecord(Logs::Normal, Logs::Error, "Program Issue: cannot continue - FUNC_ID_%i:CODE_%i", p_func_id, directive_code);
                end_parse = true;
                break;
            }
            break;
        }
        ++p_func_count;


        // F_8 CPP_ARRAY_GETCOMPCODES
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code != 0) { // directive_code = 2,3,8,23 - cannot accurately continue
            if (directive_code == 3) { // no file
                end_parse = true;
            }
            break;
        }
        ++p_func_count;

        Log.LogRecord(Logs::Normal, Logs::General, "  Processing page .qml contingent components.");

        // ---------  this can likely be done a little different due to several areas of the same file that can be altered at the same time
        // ---------  combine onNavigateSideways into this - they belong in InterpageProcesses
        // need to work on UI logic for onNavigateSideways() before adding it here
        // do InterpageProcesses operations - the point of this function is to get compIDs strings from each page and put into memory

        // F_24 QML_SUPPORT_GETCONTINGENTDATA - Part I.1
        supp_op = 1;
        fbit_pos = STANDARD_BITS + supp_op;
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, PF.GetBitValue(fbit_pos - 1, false));
        if (directive_code != 0) { // no error codes exist for this complicated function
            break;
        }
        Log.LogRecord(Logs::High, Logs::Test, "directive_code %i supp_op: %i support_bit: %i support_bit_val %i", directive_code, supp_op, fbit_pos, PF.GetBitValue(fbit_pos - 1, false));
        ++p_func_count;

        //Log.LogRecord(Logs::Normal, Logs::General, "  Processing page .qml contingent components.");

        // F_25 variables - getoutput_data, write, out_dir(multi-specific), out_file(multi-specific or mult-nondescript), supp_op - not worth it
        // F_25 QML_SUPPORT_UPDATECONTINGENTS - Part I.1 // no error handling

        out_dir = approot_directory + QTOutputDirectory[APP_OUTSUPPORT];
        out_file = QPFunc.GetSupportFile(supp_op);
        out_path = out_dir + out_file;
        PF.GetOutputData(out_path);

        if (process_required == true) {
            process_required = false;            

            PF.PrepareFunction(pd_type, 0, p_func_count, p_func_count);
            QPFunc.QML_SUPPORT_UPDATECONTINGENTS(supp_op);

            if (process_required == true) {
                utility_switch = true;
            }
        }
        strID_vec.clear();
        p_func_count = 3;


        // F_24 QML_SUPPORT_GETCONTINGENTDATA - Part I.2
        supp_op = 8;
        fbit_pos = STANDARD_BITS + supp_op;
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, PF.GetBitValue(fbit_pos - 1, false));
        if (directive_code != 0) { // no error codes exist for this complicated function
            break;
        }
        ++p_func_count;


        // F_25 QML_SUPPORT_UPDATECONTINGENTS - Part I.2
        if (process_required == true) {
            process_required = false;

            PF.PrepareFunction(pd_type, 0, p_func_count, p_func_count);
            QPFunc.QML_SUPPORT_UPDATECONTINGENTS(supp_op);
            if (process_required == true) {
                utility_switch = true;
            }
        }
        if (utility_switch == true) {
            PF.FileReWrite(out_path, true);
            utility_switch = false;
        }
        output_file_data_vec.clear();
        strID_vec.clear();
        p_func_count = 3;


        // F_24 QML_SUPPORT_GETCONTINGENTDATA - Part II - do WriteLineText operations
        supp_op = 2;
        fbit_pos = STANDARD_BITS + supp_op;
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, PF.GetBitValue(fbit_pos - 1, false));
        if (directive_code != 0) { // no error codes exist for this complicated function
            break;
        }
        ++p_func_count;


        // F_25 QML_SUPPORT_UPDATECONTINGENTS - Part II
        if (process_required == true) {
            process_required = false;

            PF.PrepareFunction(pd_type, 0, p_func_count, p_func_count);
            out_dir = approot_directory + QTOutputDirectory[APP_OUTSUPPORT];
            out_file = QPFunc.GetSupportFile(supp_op);
            out_path = out_dir + out_file;
            PF.GetOutputData(out_path);
            QPFunc.QML_SUPPORT_UPDATECONTINGENTS(supp_op);
            if (process_required == true) {
                PF.FileReWrite(out_path, true);
            }
            output_file_data_vec.clear();
        }
        strID_vec.clear();
        p_func_count = 3;


        // F_24 QML_SUPPORT_GETCONTINGENTDATA - Part III - do KeyboardProp1 operations
        supp_op = 3;
        fbit_pos = STANDARD_BITS + supp_op;
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, PF.GetBitValue(fbit_pos - 1, false));
        if (directive_code != 0) { // no error codes exist for this complicated function
            break;
        }
        ++p_func_count;


        // F_25 QML_SUPPORT_UPDATECONTINGENTS - Part III
        if (process_required == true) {
            process_required = false;

            PF.PrepareFunction(pd_type, 0, p_func_count, p_func_count);
            out_dir = approot_directory + QTOutputDirectory[APP_OUTSUPPORT];
            out_file = QPFunc.GetSupportFile(supp_op);
            out_path = out_dir + out_file;
            PF.GetOutputData(out_path);
            QPFunc.QML_SUPPORT_UPDATECONTINGENTS(supp_op);
            if (process_required == true) {
                PF.FileReWrite(out_path, true);
            }
            output_file_data_vec.clear();
        }
        strID_vec.clear();
        p_func_count = 3;


        // F_24 QML_SUPPORT_GETCONTINGENTDATA - Part IV - do SLText page operations - don't clear input_file_vec -> flag bit_pos 2
        supp_op = 4;
        fbit_pos = STANDARD_BITS + supp_op;
        fbitmask_value = PF.GetBitValue(fbit_pos - 1, false) + PF.GetBitValue(2 - 1, false);
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, fbitmask_value);
        if (directive_code != 0) { // no error codes exist for this complicated function
            break;
        }
        ++p_func_count;


        // F_25 QML_SUPPORT_UPDATECONTINGENTS - Part IV
        if (process_required == true) {
            process_required = false;

            PF.PrepareFunction(pd_type, 0, p_func_count, p_func_count);
            out_dir = approot_directory + QTOutputDirectory[APP_OUTPAGES];
            for (unsigned int i = 0; i < input_filename_vec.size(); ++i) {
                in_file = input_filename_vec.at(i);
                out_path = out_dir + in_file;
                PF.GetOutputData(out_path);
                QPFunc.QML_SUPPORT_UPDATECONTINGENTS(supp_op);
                if (process_required == true) {
                    PF.FileReWrite(out_path, true);
                }
                output_file_data_vec.clear();
            }
        }
        strID_vec.clear();
        input_filename_vec.clear();
        p_func_count = 3;


        // ---------  this can likely be done a little different due to several areas of the same file that can be altered at the same time
        // F_24 QML_SUPPORT_GETCONTINGENTDATA - Part V - do DDMenu/ACTMenu operations - don't clear input_file_vec -> flag bit_pos 2
        supp_op = 5;
        fbit_pos = STANDARD_BITS + supp_op;
        fbitmask_value = PF.GetBitValue(fbit_pos - 1, false) + PF.GetBitValue(2 - 1, false);
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, fbitmask_value);
        if (directive_code != 0) { // no error codes exist for this complicated function
            break;
        }
        ++p_func_count;


        // F_25 QML_SUPPORT_UPDATECONTINGENTS - Part V
        if (process_required == true) {
            process_required = false;

            PF.PrepareFunction(pd_type, 0, p_func_count, p_func_count);
            out_dir = approot_directory + QTOutputDirectory[APP_OUTPAGES];
            for (unsigned int i = 0; i < input_filename_vec.size(); ++i) {
                in_file = input_filename_vec.at(i);
                out_path = out_dir + in_file;
                PF.GetOutputData(out_path);

                QPFunc.QML_SUPPORT_UPDATECONTINGENTS(supp_op);
                strID_vec.clear();
                ++supp_op;

                QPFunc.PrepareSupportOperation(6, true, false, true);
                QPFunc.QML_SUPPORT_UPDATECONTINGENTS(supp_op);
                strID_vec.clear();
                ++supp_op;

                QPFunc.PrepareSupportOperation(7, true, false, true);
                QPFunc.QML_SUPPORT_UPDATECONTINGENTS(supp_op);
                if (process_required == true) {
                    PF.FileReWrite(out_path, true);
                }
            }
        }
        output_file_data_vec.clear();
        strID_vec.clear();
        transfer_string_vec.clear();
        transfer_int_vec.clear();
        p_func_count = 3;

        // prompt_set = false; // include ??

        break;

        // Navigation Contingents
    case 8: // parse into main.qml page file names for navigation and page file navigation contingents - all components should be populated/sync'd by this point

        // F_5 FILE_MATCH_PG
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code == 4) { // cannot continue - flag write page            
            end_parse = true;
            break;
        }
        ++p_func_count;


        // F_14 QML_PG_ORDER
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code != 0) {   // directive_code = 17, 10
            Log.LogRecord(Logs::Normal, Logs::Error, "Program Issue: cannot continue - FUNC_ID_%i:CODE_%i", p_func_id, directive_code);
            end_parse = true;
            break;
        }
        ++p_func_count;


        // F_23 QML_MAIN_CONTINGENTS
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code != 0) { // directive_code = 3            
            end_parse = true;
            break;
        }
        ++p_func_count;


        // F_26 GetCompIDFromMain - get compIDs for contingents within pages from main - 230,231,232,233
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 1, p_func_count, p_func_count, 0);
        if (directive_code != 0) {
            if (directive_code == 15) {
                end_parse = true;
                break; // can't continue w/o navigation component IDs - this error handling is not 100% accurate, only finding one ID of many possible will return 0
            }
            else { // directive_code = 3                
                end_parse = true;
                break;
            }
            break;
        }
        ++p_func_count;


        // F_27 ParseIntoPages
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 1, p_func_count, p_func_count, 0);
        if (directive_code != 0) { // directive_code = 17
            Log.LogRecord(Logs::Normal, Logs::Error, "Program Issue: cannot continue - FUNC_ID_%i:CODE_%i", p_func_id, directive_code);
            end_parse = true;
            break;
        }
        ++p_func_count;
        prompt_set = false;
        break;

        // Create QML Page
    case 9: // create one or more new .qml pages in ../pages from .cpp array data

        user_file = "Test"; // user input

        // F_8 CPP_ARRAY_GETCOMPCODES
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code != 0) {
            if (directive_code == 3) { // no file
                end_parse = true;
            }
            break;
        }
        ++p_func_count;
        

        // F_9 QML_PG_CREATE
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 2, p_func_count, p_func_count, 0);
        if (directive_code == 16) { // no strID comp codes
            break;
        }
        ++p_func_count;
        break;

        // Alter Page Components
    case 10: // change current .qml page(s) or change array from .qml page(s) in testApp/pages
        // POSSIBLE_USER_INPUT:  pageID, componentIDs (this would change how this Directive works completely) do all - pageID, array or file

        do_all = true;
        ui_page_num = -1; // for testing, if do_all = true, change this to -1
        user_choice = 1; // 1 = change_file 2 = change_array


        QMLSubdirective(-6, p_func_count, p_func_count); //  --- // subdirective -1
        p_func_count = p_func_count + PF.GetSubdirectiveFunctionCt(pd_type, -6);


        // must have array_strID_vec data
        // subdirective complete - cleanup directory residuals in memory and increase "p_func_count" to bypass the subdirective trigger in "parse_directive_vec" memory
        PF.DoDirectiveCleanup(false, false, false);
        ++p_func_count; // to bypass subdirective notation in "parseFunc_from_parseDir"
        Log.LogRecord(Logs::Normal, Logs::General, " Resuming main Directive");
        PF.GetParsePaths(pd_type, p_func_count);

        if (user_choice == 1) {
            if (do_all == false) {
                Log.LogRecord(Logs::Normal, Logs::General, "  Synchronizing .qml page (%i) components to .cpp array.", ui_page_num);
            }
            else {
                Log.LogRecord(Logs::Normal, Logs::General, "  Synchronizing all .qml page's components to .cpp array.");
            }

            PF.PrepareFunction(pd_type, 2, p_func_count, p_subdir_count);
            out_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 1);

            // this needs testing -- appears to work
            // allows proper data arrangement - data arrangement is done below
            if (do_all == true) {
                for (unsigned int i = 0; i < content_strID_vec.size(); ++i) {
                    if (i == 0) {
                        PFunc.TransferVec(1, 0, true);
                    }
                    PFunc.TransferVec(1, content_strID_vec.at(i), false);
                }
            }

            for (m_it = pageID_to_path_map.begin(); m_it != pageID_to_path_map.end(); ++m_it) {
                if (do_all == false) {
                    if (m_it->first == ui_page_num) {
                        out_path = m_it->second;
                        PF.GetOutputData(out_path);

                        QPFunc.CompareCompCodes(false); // F_17 data compare

                        Log.LogRecord(Logs::Normal, Logs::General, "\t\t...processing changes for page %i", m_it->first);
                        if (process_required == true) {
                            QPFunc.QML_PG_CHANGE(m_it->first);
                            PF.FileReWrite(out_path, true);
                        }
                        output_file_data_vec.clear();
                        break;
                    }
                }
                else {
                    out_path = m_it->second;
                    PF.GetOutputData(out_path);

                    QPFunc.ArrangeCompIDsintoVec(m_it->first); // data arrangement - appears to work

                    QPFunc.CompareCompCodes(false); // F_17 data compare

                    if (process_required == true) {
                        Log.LogRecord(Logs::Normal, Logs::General, "\t\t...processing changes for page %i", m_it->first);
                        QPFunc.QML_PG_CHANGE(m_it->first); // this would work in many cases but we need a way to alter the file in a smaller way
                        PF.FileReWrite(out_path, true);
                    }
                    output_file_data_vec.clear();
                }
            }
        }
        // F_12 CPP_ARRAY_CHGCOMPCODES
        else if (user_choice == 2) {
            ++p_func_count;
            ++p_subdir_count;

            QPFunc.CompareCompCodes(true); // F_17
            if (end_parse == true) {
                // can't proceed
            }

            fbit_pos = 3; // switch to use p_subdir_count instead of p_func_count
            p_func_id = PF.GetFunctionID(p_func_count);
            directive_code = HandleFunctions(parse_directive, p_func_id, 3, p_func_count, p_subdir_count, PF.GetBitValue(fbit_pos - 1, false));
            if (directive_code != 0) {
                break;
            }
            ++p_func_count;
        }
        break;

        // Create Template Page
    case 11: // create new .qml page in testApp/pages
        // POSSIBLE_USER_INPUT:  pageID, filename, user_switch(insert pageID t/f)

        user_file = "Test";
        ui_page_num = 5; // optional user input - if pageID exists, all >= will be increased by 1 (page insertion)
        user_switch = true;


        // F_5 FILE_MATCH_PG
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code == 4) { // cannot continue - flag write page
            break;
        }
        ++p_func_count;


        // F_14 - QML_PG_ORDER
        fbit_pos = 3;
        p_func_id = PF.GetFunctionID(p_func_count);
        if (user_switch == true) {
            fbitmask_value = PF.GetBitValue((fbit_pos - 1) - 1, false); // don't clear data = 2
            directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, fbitmask_value);
            if (directive_code != 0) {
                break;
            }
        }
        fbitmask_value = PF.GetBitValue((fbit_pos - 1) - 2, false) + PF.GetBitValue(fbit_pos - 1, false); // skip prep and make space for new page = val 1 + val 3
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, fbitmask_value);
        if (directive_code != 0) {
            break;
        }
        ++p_func_count;


        // F_3
        PF.PrepareFunction(pd_type, 1, p_func_count, p_func_count); // do not to overwrite an existing file - get all filenames with <in_filetype>
        out_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 1);
        file_ext = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 3);
        out_file = PF.MakeNewFilename(file_ext, user_file); // new name function and check if exists

        PF.FileOpen(out_dir + out_file, CREATE);
        QPFunc.QML_CREATE_PAGE(out_file, ui_page_num); // parse strings will be retrieved within the function
        PF.FileReWrite(out_dir + out_file, true);
        Log.LogRecord(Logs::Normal, Logs::General, "\t%s created", out_file.c_str());
        break;

        // Datacodes and Lists
    case 12: // add lists to header/source files for testApp

        // F_17 PREPARE_LISTDATA
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code != 0) {
            break;
        }
        ++p_func_count;


        // F_1 CPP_CODE_DCH
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code != 0) {
            break;
        }
        ++p_func_count;


        // F_2 CPP_CODE_DCSRC
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_directive, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code != 0) {
            break;
        }
        ++p_func_count;
        break;

    default:

        Log.LogRecord(Logs::Normal, Logs::Error, "Invalid Parse Directive: (%i)", parse_directive);

        break;

        PF.ShowDirectiveTypeData(pd_type);
        PF.GetOutDirs();
        PF.GetOutPath();
        PF.GetInDirs();
        PF.GetInFiles();
        PF.GetStrIDs();

        Log.LogRecord(Logs::Normal, Logs::Test, "out_file: %s", out_file.c_str());
        Log.LogRecord(Logs::Normal, Logs::Test, "output_file_data_vec size: %i", output_file_data_vec.size());
        Log.LogRecord(Logs::Normal, Logs::Test, "input_file_data_vec size: %i", input_file_data_vec.size());

        for (unsigned int i = 0; i < strID_vec.size(); ++i) { std::cout << i << " strID_vec " << strID_vec.at(i) << std::endl; }

        // get subdirective code or should it be assumed?
        QMLSubdirective(-7, p_func_count, p_func_count);
        p_func_count = p_func_count + PF.GetSubdirectiveFunctionCt(pd_type, -7);

    }

    /*
    - F_25, F_11, F_3, F_22 <- these need individual handling of "AddToDirStatList()"
        - F_22 requires special handling, is unique, and could probably be split
        - F_25 uses directories and files and this process could likely be included in the function itself
            - it does not use a specific dir/file
        - F_3 is not currently implemented properly and could be combined with F_11 - QML_PG_CHANGE
        - F_11 requires individual handling, currently
    */

    Log.LogRecord(Logs::High, Logs::Test, "prompt_set(%s) end_parse(%s) reparse(%s)",
                  Log.DbgSwtch(prompt_set).c_str(), Log.DbgSwtch(end_parse).c_str(), Log.DbgSwtch(reparse).c_str());

    // trigger/call/signal processDirectiveResult from UI to process directive_status and do not erase any data saved to temp memory
        // other option is to restart process with the user choice set into it, probably easier
    if (startup == true) {
        if (prompt_set == true || end_parse == true) {
            Log.LogRecord(Logs::High, Logs::Test, "startup prompt_set == true || end_parse == true");
            return;
        }
    }
    else if (prompt_set == true || end_parse == true) {
        Log.LogRecord(Logs::High, Logs::Test, "prompt_set == true || end_parse == true");
        return;
    }

    if (reparse == true) {
        if (prompt_set == false) { // the flag to reparse the entire portion if necessary
            // this portion is completed in the same function where reparse was called
        }
    }

    // cleaup should be done in the Directive type, as well

    subdir_status = 0; // make sure this doesn't screw anything else up

	transfer_i_string_map.clear();
	transfer_s_string_map.clear();
    transfer_string_vec.clear();
    transfer_int_vec.clear();
	temp_int_vec.clear();	
    array_strID_vec.clear();
    content_strID_vec.clear();
}

int QMLParse::QMLSubdirective(int parse_subdirective, int parse_function_count, int subdirective_count)
{
    ParseFiles PF;
    ParseFunctions PFunc;

    QMLParseFunction QPFunc;

    std::map<int, std::string>::iterator m_it;

    std::string out_path;

    std::string in_file;
    std::string in_path;
    std::string in_dir;

    int p_func_id = 0;
    int p_func_count = 0;
    int p_subdir_count = 0;
    int directive_code = 0;

    int num_bits = 0;
    int sbit_pos = 0;

    unsigned int bitmask_value = 0;
    int bit_pos = 0;

    unsigned int fbitmask_value = 0;
    int fbit_pos = 0;

    bool switch1 = false;
    bool switch2 = false;

    Log.LogRecord(Logs::Normal, Logs::General, " Branching into Parse Subdirective: %i", parse_subdirective);

    // proposed user input or test variables
    bool user_switch = false; // user input for(2): change/sync or get componentIDs - reordering pageIDs //	bool do_all = true;

    // define where we're at in the Directive as a whole first
    p_func_count = parse_function_count;
    if (subdirective_count < p_func_count) {
        p_subdir_count = subdirective_count;
    }
    else {
        p_subdir_count = p_func_count;
    }

    switch (parse_subdirective) {

    case -1: // check/create directories, retrieve main .qml and array comp codes

        // F_15 APP_SETUP
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_subdirective, p_func_id, 1, p_func_count, p_func_count, 0);
        if (directive_code == 11) { // no directory
            break;
        }
        ++p_func_count;


        // F_18 CPP_ARRAY_GETMAINCOMPCODES
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code != 0) {
            if (directive_code == 1) {
                sbit_pos = 3;
                subdir_status += PF.GetBitValue(sbit_pos - 1, false); // set flag b
            }
            else { // directive_code = 3; // no file
                break;
            }
        }
        ++p_func_count;


        // F_20 QML_MAIN_GETCOMPCODES
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_subdirective, p_func_id, 1, p_func_count, p_func_count, 0);
        if (directive_code != 0) {
            if (directive_code == 6) { // error, no comp codes
                sbit_pos = 2;
                subdir_status += PF.GetBitValue(sbit_pos - 1, false); // set flag a
                if (startup == true) {
                    break;
                }
            }
            else { // directive_code = 3; // no file
                break;
            }
        }
        ++p_func_count;


        sbit_pos = 1;
        subdir_status += PF.GetBitValue(sbit_pos - 1, false); // set subdir flag
        Log.LogRecord(Logs::Med, Logs::General, " End subdirective - subdir_status %i", subdir_status);
        break;

    case -2: // match page IDs to filenames, retrieve page .qml and array comp codes

        // F_5 FILE_MATCH_PG
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code == 4) {
            sbit_pos = 2;
            subdir_status += PF.GetBitValue(sbit_pos - 1, false); // set flag a
        }
        ++p_func_count;


        // F_8 CPP_ARRAY_GETCOMPCODES
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code != 0) {
            if (directive_code == 2) {
                sbit_pos = 3;
                subdir_status += PF.GetBitValue(sbit_pos - 1, false); // set flag b
            }
            else if (directive_code == 8) {
                sbit_pos = 6;
                subdir_status += PF.GetBitValue(sbit_pos - 1, false); // set flag e
            }
            else { // directive_code = 3,23; // no file, more file pages
                break;
            }
        }
        ++p_func_count;


        // F_10 QML_PG_GETCOMPCODES
        if (subdir_status == 0) { // no flags set
            p_func_id = PF.GetFunctionID(p_func_count);
            directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_func_count, 0);
            if (directive_code != 0) {
                if (directive_code == 7) {
                    sbit_pos = 2;
                    subdir_status += PF.GetBitValue(sbit_pos - 1, false); // set flag a;

                    Log.LogRecord(Logs::Normal, Logs::Test, "QML_PG_GETCOMPCODES subdir_status %i", subdir_status);
                }
                else { // directive_code = 17; this should not happen
                    break;
                }
            }
        }
        ++p_func_count;

        sbit_pos = 1;
        subdir_status += PF.GetBitValue(sbit_pos - 1, false); // set subdir flag
        Log.LogRecord(Logs::Med, Logs::General, " End subdirective - subdir_status %i", subdir_status);
        break;

    case -3: // check/change all contingent components in array

        num_bits = 3;

        // F_18 CPP_ARRAY_GETMAINCOMPCODES
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code != 0) { // directive_code = 1 or 3 cannot continue this directive
            break;
        }
        ++p_func_count;


        // F_22
        PF.PrepareFunction(pd_type, 0, p_func_count, p_func_count);
        in_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 2);

        if (input_filename_vec.size() > 0) {
            for (unsigned int i = 0; i < input_filename_vec.size(); ++i) {
                in_file = input_filename_vec.at(i);
            }
            in_path = in_dir + in_file;
            Log.LogRecord(Logs::Normal, Logs::General, "  Parsing contingent components");

            PF.GetInputData(in_path);

            QPFunc.CPP_ARRAY_CONTINGENTOPERATIONS(true, true, false);

            QPFunc.CPP_ARRAY_CONTINGENTOPERATIONS(true, false, false);
        }
        input_filename_vec.clear();
        ++p_func_count;


        // F_8 CPP_ARRAY_GETCOMPCODES
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code != 0) { // directive_code = 2,3,8,23 cannot continue this directive
            break;
        }
        --p_func_count;


        // F_22
        PF.PrepareFunction(pd_type, 0, p_func_count, p_func_count);
        in_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 2);

        if (input_filename_vec.size() > 0) {
            for (unsigned int i = 0; i < input_filename_vec.size(); ++i) {
                in_file = input_filename_vec.at(i);
            }
            in_path = in_dir + in_file;
            Log.LogRecord(Logs::Normal, Logs::General, "  Parsing contingent components");

            PF.GetInputData(in_path);

            QPFunc.CPP_ARRAY_CONTINGENTOPERATIONS(false, true, false);

            QPFunc.CPP_ARRAY_CONTINGENTOPERATIONS(false, false, false);

            QPFunc.CPP_ARRAY_CONTINGENTOPERATIONS(false, false, true);
        }
        input_filename_vec.clear();
        ++p_func_count;
        ++p_func_count;


        // F_12 - CPP_ARRAY_CHGCOMPCODES -
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_subdirective, p_func_id, 2, p_func_count, p_func_count, 0); // (2) doesn't follow the normal logic - double check
        if (directive_code != 0) {
            if (directive_code == 14) { // flag b to change page .qmls
                sbit_pos = 3;
                subdir_status += PF.GetBitValue(sbit_pos - 1, false); // set flag b - write page .qmls QML_PG_CHANGE - F_8, F_11, F_10, F_17
            }
            else { // no stringsIDs tranferred
                Log.LogRecord(Logs::Normal, Logs::Error, "Program Issue: cannot continue - FUNC_ID_%i:SUB_CODE_%i", p_func_id, subdir_status, directive_code);
                break;
            }
        }
        p_func_count = 0;


        // F_18 CPP_ARRAY_GETMAINCOMPCODES
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code != 0) { // directive_code = 1 or 3 cannot continue this directive
            break;
        }
        ++p_func_count;


        // F_22
        QPFunc.CPP_ARRAY_CONTINGENTOPERATIONS(true, false, true);
        p_func_count = 4;


        // F_19 CPP_ARRAY_CHGMAINCOMPCODES
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_subdirective, p_func_id, 2, p_func_count, p_func_count, 0); // (2) doesn't follow the normal logic
        if (directive_code != 0) {
            if (directive_code == 13) { // flag a to change main.qml
                sbit_pos = 2;
                subdir_status += PF.GetBitValue(sbit_pos - 1, false); // set flag a - write main.qml QML_MAIN_CHANGE - F_18, F_21, F_20, F_17
            }
            else { // no stringsIDs tranferred
                Log.LogRecord(Logs::Normal, Logs::Error, "Program Issue: cannot continue - FUNC_ID_%i:CODE_%i", p_func_id, subdir_status, directive_code);
                break;
            }
        }
        ++p_func_count;

        sbit_pos = 1;
        subdir_status += PF.GetBitValue(sbit_pos - 1, false); // set subdir flag
        Log.LogRecord(Logs::Med, Logs::General, " End subdirective - subdir_status %i", subdir_status);

        break;

    case -4: // sync main.qml from array - from contingents (subdir -3) but check to see if it will rewrite if no compcodes from .qml file

        num_bits = 3;

        // F_18 CPP_ARRAY_GETMAINCOMPCODES
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_subdir_count, 0);
        if (directive_code != 0) { // directive_code = 1 or 3 cannot continue this directive
            break;
        }
        ++p_func_count;
        ++p_subdir_count;


        // F_20 QML_MAIN_GETCOMPCODES
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_subdirective, p_func_id, 1, p_func_count, p_subdir_count, 0);
        if (directive_code != 0) { // directive_code = 3 or 6
            if (directive_code == 6) { // no comp codes, cannot continue
                //   sbit_pos = 2;
                //   subdir_status += PF.GetBitValue(sbit_pos - 1, false); // set flag a
                break;
            }
            else { // directive_code = 3; // no file
                break;
            }
        }
        ++p_func_count;
        ++p_subdir_count;


        // F_21 - QML_MAIN_CHANGE
        QPFunc.CompareCompCodes(false);
        if (process_required == true) {
            directive_code = 21;
            PF.AddToDirStatList(pd_type, p_dir, parse_subdirective, p_func_id, directive_code, false);
        }
        else if (end_parse == true) {
            directive_code = 20;
            PF.AddToDirStatList(pd_type, p_dir, parse_subdirective, p_func_id, directive_code, false);
            break;
        }

        Log.LogRecord(Logs::Normal, Logs::General, "\t\t...processing changes for main.qml");

        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_subdir_count, PF.GetBitValue(fbit_pos - 1, false));
        if (directive_code == 5) { // no output_file_data and no explanation
            break;
        }
        ++p_func_count;
        ++p_subdir_count;
        Log.LogRecord(Logs::Med, Logs::General, " End subdirective - subdir_status %i", subdir_status);
        break;

    case -5: // sync page.qmls from array

        num_bits = 3;

        // F_5 FILE_MATCH_PG
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_subdir_count, 0);
        if (directive_code == 4) { // unless flags and ability to write are changed, cannot continue - cannot continue - flag write page
            break;
        }
        ++p_func_count;
        ++p_subdir_count;


        // F_8 CPP_ARRAY_GETCOMPCODES
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_subdir_count, 0);
        if (directive_code != 0) { // 2,3,8,23 - unless ability to write to array within directive is changed, cannot continue
            break;
        }
        ++p_func_count;
        ++p_subdir_count;


        // F_10 QML_PG_GETCOMPCODES
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_subdir_count, 0);
        if (directive_code != 0) { // directive_code = 7, 17
            Log.LogRecord(Logs::Normal, Logs::Error, "Program Issue: cannot continue - FUNC_ID_%i:SUB_STATUS_%i:CODE_%i", p_func_id, subdir_status, directive_code);
            break;
        }
        ++p_func_count;
        ++p_subdir_count;


        // F_11 QML_PG_CHANGE
        PF.PrepareFunction(pd_type, 2, p_func_count, p_subdir_count);
        p_func_id = PF.GetFunctionID(p_func_count);

        // this needs more testing
        // allows proper data arrangement - data arrangement is done below
        for (unsigned int i = 0; i < content_strID_vec.size(); ++i) {
            if (i == 0) {
                PFunc.TransferVec(1, 0, true);
            }
            PFunc.TransferVec(1, content_strID_vec.at(i), false);
        }
        if (pageID_to_path_map.size() > 0) {
            for (m_it = pageID_to_path_map.begin(); m_it != pageID_to_path_map.end(); ++m_it) {
                out_path = m_it->second;
                PF.GetOutputData(out_path);

                QPFunc.ArrangeCompIDsintoVec(m_it->first); // data arrangement - appears to work
                QPFunc.CompareCompCodes(false); // this is acting funny, it's not recognizing

                if (end_parse == true) {
                    directive_code = 20;
                    PF.AddToDirStatList(pd_type, p_dir, parse_subdirective, p_func_id, directive_code, false);
                    break;
                }
                else if (process_required == true) {
                    //directive_code = 22;
                    PF.AddToDirStatList(pd_type, p_dir, parse_subdirective, p_func_id, directive_code, false);
                    Log.LogRecord(Logs::Normal, Logs::General, "\t\t...processing changes for page %i", m_it->first);
                    QPFunc.QML_PG_CHANGE(m_it->first); // F_11 - this would work in many cases but we need a way to alter the file in a smaller way
                    PF.FileReWrite(out_path, true);
                    subdir_status = 0;
                }
                output_file_data_vec.clear();
            }
            break;
        }
        else {
            directive_code = 17;
            PF.AddToDirStatList(pd_type, p_dir, parse_subdirective, p_func_id, directive_code, false);
            break;
        }


        Log.LogRecord(Logs::Med, Logs::Test, " End subdirective - directive_code %i  subdir_status %i", directive_code, subdir_status);
        if (directive_code > 0) {

        }
        else {
            //subdir_status = 0;
        }

        ++p_func_count;
        ++p_subdir_count;
        Log.LogRecord(Logs::Med, Logs::General, " End subdirective - subdir_status %i", subdir_status);

        break;


    case -6:

        // F_5 FILE_MATCH_PG
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code == 4) { // cannot continue - flag write page
            break;
        }
        ++p_func_count;


        // F_10 QML_PG_GETCOMPCODES
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code != 0) { // directive_code = 7, 17
            break;
        }
        ++p_func_count;


        // F_13 - QML_PG_PROCESSCOMPIDS reordering isn't necessary
        if (user_switch == true) { // do_all user variable
            fbit_pos = 3;
            fbitmask_value = PF.GetBitValue(fbit_pos - 1, false);
        }
        else {
            fbitmask_value = 0;
        }
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_func_count, fbitmask_value);
        if (directive_code != 0) {
            break;
        }
        ++p_func_count;


        // F_8 CPP_ARRAY_GETCOMPCODES
        p_func_id = PF.GetFunctionID(p_func_count);
        directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_func_count, 0);
        if (directive_code != 0) {
            break;
        }
        ++p_func_count;
        Log.LogRecord(Logs::Med, Logs::General, " End subdirective - subdir_status %i", subdir_status);

        break;

    case -7: // 4 possiblities currently in this subdir - singlepage/all,  checkfile/check accuracy,

        Log.LogRecord(Logs::Normal, Logs::General, "  Performing setup routine.");
        num_bits = 10;

        while (bitmask_value < 1023) {
            p_func_count = 0;

            // F_15 - APP_SETUP create directories
            bit_pos = 1;
            if (PF.CheckBitFlags(num_bits, bitmask_value, bit_pos) == false) { // flag 1 - bitmask_value = 0
                Log.LogRecord(Logs::Normal, Logs::General, "  Checking for application directories");

                p_func_id = PF.GetFunctionID(p_func_count);
                directive_code = HandleFunctions(parse_subdirective, p_func_id, 1, p_func_count, p_func_count, 0);
                if (directive_code != 0) {
                    break;
                }

                bitmask_value = bitmask_value + PF.GetBitValue(bit_pos - 1, false); // + 1
            }
            p_func_count = 1;

            Log.LogRecord(Logs::High, Logs::General, "\tp_func_count:bit_pos:bitmask_value  %i:%i:%i", p_func_count, bit_pos, bitmask_value);

            // F_28 - QML_CREATE_MAIN create main .qml if not exists
            bit_pos = 2;
            if (PF.CheckBitFlags(num_bits, bitmask_value, bit_pos) == false) { // flag 2 - bitmask_value = 1

                fbit_pos = 4;
                p_func_id = PF.GetFunctionID(p_func_count);
                directive_code = HandleFunctions(parse_subdirective, p_func_id, 2, p_func_count, p_func_count, PF.GetBitValue(fbit_pos - 1, false));
                if (directive_code != 0) {
                    break;
                }

                bitmask_value = bitmask_value + PF.GetBitValue(bit_pos - 1, false); // + 2
            }
            p_func_count = 2;

            Log.LogRecord(Logs::High, Logs::General, "\tp_func_count:bit_pos:bitmask_value  %i:%i:%i", p_func_count, bit_pos, bitmask_value);


            // F_8 - CPP_ARRAY_GETCOMPCODES - compIDs for file checking
            bit_pos = 3;
            if (PF.CheckBitFlags(num_bits, bitmask_value, bit_pos) == false) { // flag 3 - bitmask_value = 3
                fbit_pos = 3;
                fbitmask_value = PF.GetBitValue(fbit_pos - 1, false) + PF.GetBitValue((fbit_pos - 1) - 1, false); // don't clear data 2 and get unique component codes 3
                p_func_id = PF.GetFunctionID(p_func_count);
                directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_func_count, fbitmask_value);
                if (directive_code != 0) {
                    switch1 = false;
                }
                else {
                    bitmask_value = bitmask_value + PF.GetBitValue(bit_pos - 1, false); // + 4
                    switch1 = true;
                }
            }

            Log.LogRecord(Logs::High, Logs::General, "\tp_func_count:bit_pos:bitmask_value  %i:%i:%i", p_func_count, bit_pos, bitmask_value);

            // ----------------  something may not work here - solution below may work

            // we can't overwrite the "array_strID_vec" before we check for files, we need to be able to check the files and then come back to this
            // F_8 - CPP_ARRAY_GETCOMPCODES - compIDs for page writing and component checking
            if (bitmask_value > PF.GetBitValue(bit_pos, true)) {
                if (switch1 == true) { // file exists1 - flag 6 - bitmask_value = 31, 23 these will change
                    fbit_pos = 1;
                    fbitmask_value = PF.GetBitValue(fbit_pos - 1, false);
                    p_func_id = PF.GetFunctionID(p_func_count);
                    directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_func_count, fbitmask_value);

                    // fix this - was automatic false
                    if (directive_code != 0) {
                        switch1 = false; //switch1 = false
                    }
                    else {
                        switch1 = true; //switch1 = true
                    }
                }
            }
            p_func_count = 3;

            Log.LogRecord(Logs::High, Logs::General, "\tp_func_count:bit_pos:bitmask_value  %i:%i:%i", p_func_count, bit_pos, bitmask_value);

            // F_5 FILE_MATCH_PG
            bit_pos = 4;
            if (PF.CheckBitFlags(num_bits, bitmask_value, bit_pos) == false) { // flag 4 - bitmask_value = 3, 7
                p_func_id = PF.GetFunctionID(p_func_count);

                directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_func_count, 0);
                if (directive_code == 4) { // flag write page
                    Log.LogRecord(Logs::Normal, Logs::General, "    No input files located.");
                    switch2 = true;
                }
                else {
                    bitmask_value = bitmask_value + PF.GetBitValue(bit_pos - 1, false); // + 8
                    switch2 = false;
                }
            }
            p_func_count = 4;

            Log.LogRecord(Logs::High, Logs::General, "\tp_func_count:bit_pos:bitmask_value  %i:%i:%i", p_func_count, bit_pos, bitmask_value);

            // F_14 QML_PG_ORDER
            bit_pos = 5;
            if (PF.CheckBitFlags(num_bits, bitmask_value, bit_pos) == false && switch2 == false) { //  file exists2 - flag 5 - bitmask_value = 15, 11
                p_func_id = PF.GetFunctionID(p_func_count);

                directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_func_count, 0);
                if (directive_code != 0) {
                    break;
                }

                bitmask_value = bitmask_value + PF.GetBitValue(bit_pos - 1, false); // + 16
            }
            p_func_count = 5;

            Log.LogRecord(Logs::High, Logs::General, "\tp_func_count:bit_pos:bitmask_value  %i:%i:%i", p_func_count, bit_pos, bitmask_value);

            if (bitmask_value == 19) {
                Log.LogRecord(Logs::Normal, Logs::Error, "Setup cannot continue without array data and page files.\n  Aborting directive.");
                end_parse = true;
                break;
            }

            // F_16 - FILE_CHECK_DEPEND check component file existence from array compIDs
            bit_pos = 6;
            if (PF.CheckBitFlags(num_bits, bitmask_value, bit_pos) == false && switch1 == true) { // file exists1 - flag 6 - bitmask_value = 31, 23

                p_func_id = PF.GetFunctionID(p_func_count);
                directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_func_count, 0);
                if (directive_code == 12) {
                    //end_parse = true;
                    // need to manually check for missing files but can continue
                }

                bitmask_value = bitmask_value + PF.GetBitValue(bit_pos - 1, false); // + 32
            }
            p_func_count = 6;

            Log.LogRecord(Logs::High, Logs::General, "\tp_func_count:bit_pos:bitmask_value  %i:%i:%i", p_func_count, bit_pos, bitmask_value);

            if (PF.CheckBitFlags(num_bits, bitmask_value, 4) == false) { // bitmask_value = 23 -> 4 = the bit_pos corresponding to matching pages to files
                // create page files
                // continue -> go back to beginning of subdir loop to match page files for the next steps
            }

            // F_10 QML_PG_GETCOMPCODES
            bit_pos = 7;
            if (PF.CheckBitFlags(num_bits, bitmask_value, bit_pos) == false && switch2 == false) { // file exists2 - flag 7 - bitmask_value = 63, 27
                p_func_id = PF.GetFunctionID(p_func_count);
                directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_func_count, 0);
                if (directive_code != 0) { // directive_code = 7, 17
                    break; // will likely break this process
                }

                bitmask_value = bitmask_value + PF.GetBitValue(bit_pos - 1, false); // + 64
            }
            p_func_count = 7;

            Log.LogRecord(Logs::High, Logs::General, "\tp_func_count:bit_pos:bitmask_value  %i:%i:%i", p_func_count, bit_pos, bitmask_value);

            if (PF.CheckBitFlags(num_bits, bitmask_value, 3) == false) { // bitmask_value = 27 -> 3 = the bit_pos corresponding to retrieving array pg compIDs
                // rewrite array
                // continue -> go back to beginning of subdir loop to retrieve array pg compIDs for the next steps
            }

            // go back to beginning of subdir loop to retrieve all array pg compIDs for the next steps
            if (switch1 == true) {
                continue;
            }

            // F_17
            // F 11,12 alter the page, array respectively - this should be a choice or only notification
            bit_pos = 8;
            if (PF.CheckBitFlags(num_bits, bitmask_value, bit_pos) == false) {
                Log.LogRecord(Logs::Normal, Logs::General, "  Comparing component codes between array and content data");

                // need to loop over files from pagematch map

                PF.PrepareFunction(pd_type, 0, p_func_count, p_func_count);

                QPFunc.CompareCompCodes(false); // F_17

                bitmask_value = bitmask_value + PF.GetBitValue(bit_pos - 1, false); // + 128
            }
            p_func_count = 8;

            Log.LogRecord(Logs::High, Logs::General, "\tp_func_count:bit_pos:bitmask_value  %i:%i:%i", p_func_count, bit_pos, bitmask_value);

            // the result of function above will ask to rewrite array or page files
            // after this clear array/content_strID_vecs - or not, I think we need the data below although it may not be ordered properly

            // not needed for setup unless ordering the ids will be part of it
            // F_13 - QML_PG_PROCESSCOMPIDS function also incrementally orders the ids
            bit_pos = 9;
            if (PF.CheckBitFlags(num_bits, bitmask_value, bit_pos) == false) {
                if (user_switch == true) {
                    fbit_pos = 3;
                    fbitmask_value = PF.GetBitValue(fbit_pos - 1, false);
                }
                else {
                    fbitmask_value = 0;
                }
                p_func_id = PF.GetFunctionID(p_func_count);
                directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_func_count, fbitmask_value);
                if (directive_code != 0) {
                    break;
                }
            }
            p_func_count = 9;

            Log.LogRecord(Logs::High, Logs::General, "\tp_func_count:bit_pos:bitmask_value  %i:%i:%i", p_func_count, bit_pos, bitmask_value);

            // F_6 CPP_ARRAY_GETLOGICIDS
            bit_pos = 9;
            if (PF.CheckBitFlags(num_bits, bitmask_value, bit_pos) == false) {
                p_func_id = PF.GetFunctionID(p_func_count);
                directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_func_count, 0);
                if (directive_code != 0) { // cannot continue
                    break;
                }
                bitmask_value = bitmask_value + PF.GetBitValue(bit_pos - 1, false); // + 256
            }
            p_func_count = 10;

            Log.LogRecord(Logs::High, Logs::General, "\tp_func_count:bit_pos:bitmask_value  %i:%i:%i", p_func_count, bit_pos, bitmask_value);

            // F_7 QML_PG_UPDATELOGICIDS
            bit_pos = 10;
            if (PF.CheckBitFlags(num_bits, bitmask_value, bit_pos) == false && PF.GetBitValue(bit_pos - 2, true) <= bitmask_value) {
                p_func_id = PF.GetFunctionID(p_func_count);
                directive_code = HandleFunctions(parse_subdirective, p_func_id, 0, p_func_count, p_func_count, 0);
                if (directive_code != 0) {
                    break;
                }
                bitmask_value = bitmask_value + PF.GetBitValue(bit_pos - 1, false); // + 512
            }

            Log.LogRecord(Logs::High, Logs::General, "\tp_func_count:bit_pos:bitmask_value  %i:%i:%i", p_func_count, bit_pos, bitmask_value);

            if (end_parse == true) {
                Log.LogRecord(Logs::Normal, Logs::Error, "  Parse incomplete, invalid data. Unable to continue.\n    bitmask code:  %i", bitmask_value);
                break;
            }
        }

        break;

    default:

        PF.ShowDirectiveTypeData(pd_type);
        PF.GetOutDirs();
        PF.GetOutPath();
        PF.GetInDirs();
        PF.GetInFiles();
        PF.GetStrIDs();

        Log.LogRecord(Logs::Normal, Logs::General, "out_file: ");

        Log.LogRecord(Logs::Normal, Logs::Error, "Invalid Parse Subdirective: (%i)", parse_subdirective);
        break;
    }

    end_parse = false;
    return subdir_status;
}

int QMLParse::HandleFunctions(int current_directive, int function_id, int str_transfer_status, int function_count, int subfunction_count, int bit_value)
{
    /*
        bit_pos static flags:
        0 = do nothing
        1 = skip PrepareFunction()
        2 = skip clearing data containers
        3+ = variable, dependant upon the function
        return_status:
        0 = do nothing, everything is fine
        3 = input_file_vec size = 0 - input_filename_vec.size() == 0 - F_20, F_18, F_5, F_8, F_23, F_26,
        4 = no dependency files
    */

    ParseFiles PF;
    ParseDirectory PDir;
    ParseFunctions PFunc;

    QMLParseFunction QPFunc;

    std::map<int, std::string>::iterator m_it;

    std::string out_dir;
    std::string in_dir;
    std::string file_ext;

    std::string out_file;
    std::string out_path;
    std::string in_file;
    std::string in_path;

    int return_status = 0;
    int supp_op = 0;

    int num_pages = 0;
    int num_bits = 0;
    int adj_bit_value = 0;
    int fbit_pos = 0;

    // proposed user input or test variables - create a linked list of user input to access here
    int ui_page_num = -1; // user input page number for all pages this must be set to -1, currently
    bool do_all = false;
    bool user_switch = false; // user input for(2): change/sync or get componentIDs - reordering pageIDs
    //	int user_choice = 0; // user input for test change .cpp array or .qml page file 1 = change_file 2 = change_array
    std::string user_file; // user input for file naming

    // use bitmasks to alter specific function variables and pass the value to this function
    num_bits = 3;
    if (PF.CheckBitFlags(num_bits, bit_value, 1) == false) {
        PF.PrepareFunction(pd_type, str_transfer_status, function_count, subfunction_count);
        Log.LogRecord(Logs::High, Logs::General, "PrepareFunction FID: %i",function_id);
    }
    else {
        Log.LogRecord(Logs::Normal, Logs::General, "  Skipping Prepare Function operations");
    }

    switch (function_id) {

    case CPP_CODE_DCH: // F_1
        in_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 2);
        file_ext = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 3);

        out_path = parse_outpaths_vec.at(function_count);
        PDir.GetInputFiles(in_dir, file_ext);
        PF.GetOutputData(out_path);

        Log.LogRecord(Logs::Normal, Logs::General, "  Updating datacodes.h with new data file lists.");

        QPFunc.CPP_CODE_DCH();
        if (end_parse == true) { // possibly leave the log in qmlparsefunc
            Log.LogRecord(Logs::Normal, Logs::General, " ...no new files, Parse Directive complete");
        }
        else {
            PF.FileReWrite(out_path, true);
        }
        strID_vec.clear();
        input_filename_vec.clear();
        output_file_data_vec.clear();
        break;

    case CPP_CODE_DCSRC: // F_2 - data used here is reliant upon F_1
        out_path = parse_outpaths_vec.at(function_count);

        if (transfer_string_vec.size() > 0) {
            PF.GetOutputData(out_path);
            Log.LogRecord(Logs::Normal, Logs::General, "  Updating datacodes.cpp with new data file lists.");

            QPFunc.CPP_CODE_DCSRC();
            PF.FileReWrite(out_path, true);
        }
        strID_vec.clear();
        transfer_string_vec.clear();
        output_file_data_vec.clear();
        break;

    case QML_CREATE_PAGE: // F_3
        break;

    case QML_ALTER_NEWPAGE: // F_4
        ui_page_num = 8; // user input

        if (pageID_to_path_map.size() > 0) {
            for (m_it = pageID_to_path_map.begin(); m_it != pageID_to_path_map.end(); ++m_it) {
                if (m_it->first == ui_page_num) {
                    out_path = m_it->second;
                    break;
                }
            }
        }
        else {
            return_status = 17;
        }

        PF.GetOutputData(out_path);
        QPFunc.QML_ALTER_NEWPAGE();
        PF.FileReWrite(out_path, true);
        break;

    case FILE_MATCH_PG: // F_5
        in_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 2);
        file_ext = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 3);

        PDir.GetInputFiles(in_dir, file_ext);
        Log.LogRecord(Logs::Normal, Logs::General, "  Matching page IDs to filenames");

        pageID_to_path_map.clear();

        if (input_filename_vec.size() > 0) {
            for (unsigned int i = 0; i < input_filename_vec.size(); ++i) {
                in_file = input_filename_vec.at(i);
                in_path = in_dir + in_file;

                PF.GetInputData(in_path);
                QPFunc.FILE_MATCH_PG(in_path);
            }
        }
        else {
            return_status = 4;
        }

        input_filename_vec.clear();
        strID_vec.clear();
        break;

    case CPP_ARRAY_GETLOGICIDS: // F_6
        in_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 2);

        Log.LogRecord(Logs::Normal, Logs::General, "  Retrieving .qml logic IDs from .cpp arrays.");

        if (input_filename_vec.size() > 0) {
            for (unsigned int i = 0; i < input_filename_vec.size(); ++i) {
                in_file = input_filename_vec.at(i);
            }
        }
        else {
            return_status = 3;
        }
        in_path = in_dir + in_file;
        PF.GetInputData(in_path);
        QPFunc.CPP_ARRAY_GETLOGICIDS();

        if (process_required == false) {
            return_status = 18;
        }
        input_filename_vec.clear();
        strID_vec.clear();

        break;

    case QML_PG_UPDATELOGICIDS: // F_7
        out_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 1);

        Log.LogRecord(Logs::Normal, Logs::General, "  Analyzing logic IDs in all .qml page files");

        if (pageID_to_path_map.size() > 0) {
            for (m_it = pageID_to_path_map.begin(); m_it != pageID_to_path_map.end(); ++m_it) {
                out_path = m_it->second;

                if (out_path.length() > 0) {
                    process_required = false;
                    PF.GetOutputData(out_path);
                    QPFunc.QML_PG_UPDATELOGICIDS(m_it->first);
                    if (process_required == true) {
                        PF.FileReWrite(out_path, true);
                    }
                    output_file_data_vec.clear();
                }
            }
        }
        else {
            return_status = 17;
        }
        strID_vec.clear();
        break;

    case CPP_ARRAY_GETCOMPCODES: // F_8 // need to implement better for accurate page comparisons        
        ui_page_num = -1;

        fbit_pos = 3; // sets get unique ids for file-folder population

        in_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 2);

        if (input_filename_vec.size() > 0) {
            for (unsigned int i = 0; i < input_filename_vec.size(); ++i) {
                in_file = input_filename_vec.at(i);
            }
            in_path = in_dir + in_file;
            PF.GetInputData(in_path);
        }
        else {
            return_status = 3;
        }

        if (PF.CheckBitFlags(num_bits, bit_value, fbit_pos) == false) {
            Log.LogRecord(Logs::Normal, Logs::General, "  Retrieving page component IDs from array data");
            QPFunc.CPP_ARRAY_GETCOMPCODES(false, ui_page_num);

            if (PFunc.ArrayDimFromFile(true, false, true, QPFunc.QPStr(strID_vec[0]), "") > pageID_to_path_map.size()) {
                Log.LogRecord(Logs::Normal, Logs::ParseFunc, "array pages: %i page map: %i", PFunc.ArrayDimFromFile(true, false, true, QPFunc.QPStr(strID_vec[0]), ""), pageID_to_path_map.size());
                return_status = 8;
            }
            else if (PFunc.ArrayDimFromFile(true, false, true, QPFunc.QPStr(strID_vec[0]), "") < pageID_to_path_map.size()) {
                Log.LogRecord(Logs::Normal, Logs::ParseFunc, "array pages: %i page map: %i", PFunc.ArrayDimFromFile(true, false, true, QPFunc.QPStr(strID_vec[0]), ""), pageID_to_path_map.size());
                return_status = 23;
            }
        }
        else {
            Log.LogRecord(Logs::Normal, Logs::General, "  Retrieving unique page component codes from array data");
            QPFunc.CPP_ARRAY_GETCOMPCODES(true, ui_page_num);
        }
        if (process_required == false) {
            return_status = 2;
        }
        Log.LogRecord(Logs::High, Logs::Test, "array pages: %i page map: %i", PFunc.ArrayDimFromFile(true, false, true, QPFunc.QPStr(strID_vec[0]), ""), pageID_to_path_map.size());

        if (PF.CheckBitFlags(num_bits, bit_value, 2) == false) {
            strID_vec.clear();
            input_filename_vec.clear();
        }
        input_file_data_vec.clear();
        break;

    case QML_PG_CREATE: // F_9 - this needs quite a bit of work to insert pages based on array data and not creating unecessarily
        // will likely need to use logic from QMLMainFunction -> case 2 and bitflags to the various processes
        // also change/add logic and bitmasking to naming to skip the numeric portion
        user_file = "Test"; // user input

        fbit_pos = 3;

        out_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 1);
        file_ext = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 3);

        if (strID_vec.size() > 0) {
            for (unsigned int i = 0; i < strID_vec.size(); ++i) {
                if (strID_vec.at(i) == 100) { // get the number of files to be created - can use x_dim
                    ++num_pages;
                }
                else {
                    continue;
                }
            }
        }
        else {
            return_status = 16;
        }

        if (PF.CheckBitFlags(num_bits, bit_value, fbit_pos) == false) {
            Log.LogRecord(Logs::Normal, Logs::General, "Creating (%i) QML pages in:\n  %s", num_pages, out_dir.c_str());
            for (int i = 0; i < num_pages; ++i) {
                out_file = PF.MakeNewFilename(file_ext, user_file);
                PF.FileOpen(out_dir + out_file, CREATE);

                QPFunc.QML_PG_CREATE(out_file, i);
                PF.FileReWrite(out_dir + out_file, true);
            }
        }
        else { // if there aren't any pages we need to process differently
            for (int i = 0; i < num_pages; ++i) {
                for (m_it = pageID_to_path_map.begin(); m_it != pageID_to_path_map.end(); ++m_it) {
                    if (i == m_it->first) {
                        continue;
                    }
                    out_file = PF.MakeNewFilename(file_ext, user_file);
                    PF.FileOpen(out_dir + out_file, CREATE);

                    QPFunc.QML_PG_CREATE(out_file, i);
                    PF.FileReWrite(out_dir + out_file, true);

                    Log.LogRecord(Logs::Normal, Logs::General, "Created QML page (%i) in:\n  %s", i, out_dir.c_str());
                }
            }


        }
        strID_vec.clear();
        break;

    case QML_PG_GETCOMPCODES: // F_10
        do_all = true;

        Log.LogRecord(Logs::Normal, Logs::General, "  Retrieving page component IDs from file data in memory");

        if (pageID_to_path_map.size() > 0) {
            for (m_it = pageID_to_path_map.begin(); m_it != pageID_to_path_map.end(); ++m_it) {
                if (do_all == false) {
                    if (m_it->first == ui_page_num) {
                        in_path = m_it->second;

                        if (in_path.length() > 0) {
                            PF.GetInputData(in_path);
                            QPFunc.QML_PG_GETCOMPCODES();
                        }
                        break;
                    }
                }
                else {
                    in_path = m_it->second;

                    if (in_path.length() > 0) {
                        PF.GetInputData(in_path);
                        QPFunc.QML_PG_GETCOMPCODES();
                    }
                }
            }
        }
        else {
            return_status = 17;
        }
        if (content_strID_vec.size() == 0) {
            return_status = 7;
        }
        input_filename_vec.clear(); // is this needed?
        strID_vec.clear();
        break;

    case QML_PG_CHANGE: // F_11
        break;

    case CPP_ARRAY_CHGCOMPCODES: // F_12
        do_all = true;
        fbit_pos = 3; // switch to use p_subdir_count instead of p_func_count

        if (do_all == true) {
            ui_page_num = -1;
        }

        if (PF.CheckBitFlags(num_bits, bit_value, fbit_pos) == true) {
            out_path = parse_outpaths_vec.at(subfunction_count);
        }
        else {
            out_path = parse_outpaths_vec.at(function_count);
        }
        PF.GetOutputData(out_path);

        QPFunc.CPP_ARRAY_CHGCOMPCODES(ui_page_num);

        if (process_required == true) {
            if (do_all == false) {
                Log.LogRecord(Logs::Normal, Logs::General, "  Rewriting .cpp array to match .qml page (%i) components", ui_page_num);
            }
            else {
                Log.LogRecord(Logs::Normal, Logs::General, "  Rewriting .cpp array to match all .qml page's components.");
            }
            PF.FileReWrite(out_path, true);
            return_status = 14;
        }
        else {
            output_file_data_vec.clear();
        }
        strID_vec.clear();
        break;

    case QML_PG_PROCESSCOMPIDS: // F_13
        do_all = true;
        fbit_pos = 3; // user_switch or change_ids = true

        if (PF.CheckBitFlags(num_bits, bit_value, fbit_pos) == true) {
            Log.LogRecord(Logs::Normal, Logs::General, "  Reordering component IDs in .qml files");
            user_switch = true;
        }
        else {
            Log.LogRecord(Logs::Normal, Logs::General, "  Retrieving component IDs in .qml files");
            user_switch = false;
        }

        if (pageID_to_path_map.size() > 0) {
            for (m_it = pageID_to_path_map.begin(); m_it != pageID_to_path_map.end(); ++m_it) {
                if (do_all == false) {
                    if (m_it->first == ui_page_num) {
                        out_path = m_it->second;
                        PF.GetOutputData(out_path);

                        if (user_switch == true) {
                            QPFunc.QML_PG_PROCESSCOMPIDS(ui_page_num, true, false);
                            if (process_required == true) {
                                PF.FileReWrite(out_path, false);
                            }
                        }
                        QPFunc.QML_PG_PROCESSCOMPIDS(ui_page_num, false, false);
                    }
                }
                else {
                    out_path = m_it->second;
                    PF.GetOutputData(out_path);

                    if (user_switch == true) {
                        QPFunc.QML_PG_PROCESSCOMPIDS(m_it->first, true, true);
                        if (process_required == true) {
                            Log.LogRecord(Logs::Normal, Logs::General, "  Reordering component IDs in .qml files");
                            PF.FileReWrite(out_path, false);
                        }
                    }
                    QPFunc.QML_PG_PROCESSCOMPIDS(m_it->first, false, true);
                }
            }
        }
        else {
            return_status = 17;
        }
        if (end_parse == true) {
            return_status = 9;
        }
        transfer_int_vec.clear();
        strID_vec.clear();
        break;

    case QML_PG_ORDER: // F_14
        ui_page_num = 8; // only used when reorder = false
        fbit_pos = 3; // insert new page value into current values, incrementing values >= ++

        Log.LogRecord(Logs::Normal, Logs::General, "  Checking order of page IDs in .qml files");

        if (pageID_to_path_map.size() > 0) {
            for (m_it = pageID_to_path_map.begin(); m_it != pageID_to_path_map.end(); ++m_it) {
                out_path = m_it->second;

                if (out_path.length() > 0) {
                    PF.GetOutputData(out_path);

                    if (PF.CheckBitFlags(num_bits, bit_value, fbit_pos) == false) {
                        QPFunc.QML_PG_ORDER(true, ui_page_num, num_pages, m_it->first);
                        ++num_pages;
                    }
                    else {
                        QPFunc.QML_PG_ORDER(false, ui_page_num, num_pages, m_it->first);
                    }

                    if (process_required == false) {
                        output_file_data_vec.clear();
                        continue;
                    }

                    if (end_parse == true) {
                        return_status = 10;
                        break;
                    }
                    PF.FileReWrite(out_path, true);
                }
            }
        }
        else {
            return_status = 17;
        }

        if (PF.CheckBitFlags(num_bits, bit_value, 2) == false) {
            strID_vec.clear();
        }
        num_pages = 0;
        break;

    case APP_SETUP: // F_15
        out_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 1);
        if (out_dir.length() > 0) {
            QPFunc.APP_SETUP(out_dir);
        }
        else {
            return_status = 11;
        }
        strID_vec.clear();
        break;

    case FILE_CHECK_DEPEND: // F_16
        process_required = true;

        out_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 1);
        in_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 2);

        if (PF.GetInputFileCt(pd_type, function_count) > input_filename_vec.size()) {
            end_parse = true;
            return_status = 12; // issue with program dir/files
            strID_vec.clear();
            break;
        }
        // get filenames
        for (unsigned int i = 0; i < input_filename_vec.size(); ++i) {
            if (input_filename_vec.at(i) == QTInputFile[QML_FILENAMES]) {
                in_path = in_dir + input_filename_vec.at(i);

                PF.GetInputData(in_path);
                QPFunc.FILE_CHECK_DEPEND(false, true, out_dir);
            }
            input_file_data_vec.clear();
        }
        // populate folders
        for (unsigned int i = 0; i < input_filename_vec.size(); ++i) {
            if (input_filename_vec.at(i) == QTInputFile[QML_DEPENDANTS]) {
                in_path = in_dir + input_filename_vec.at(i);

                PF.GetInputData(in_path);
                QPFunc.FILE_CHECK_DEPEND(false, false, out_dir);
                QPFunc.FILE_CHECK_DEPEND(true, false, out_dir);
            }
            if (process_required == false) {
                return_status = 25; // missing files
            }
            input_file_data_vec.clear();
        }

        Log.LogRecord(Logs::High, Logs::Test, " FILE_CHECK_DEPEND: return_status(%i)", return_status);

        strID_vec.clear();
        break;

    case PREPARE_LISTDATA: // F_17

        in_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 2);

        if (input_filename_vec.size() > 0) {
            for (unsigned int i = 0; i < input_filename_vec.size(); ++i) {
                in_file = input_filename_vec.at(i);
                Log.LogRecord(Logs::High, Logs::Test, " in_file:%s ", in_file.c_str());
            }
            in_path = in_dir + in_file;

            PF.GetInputData(in_path);
            Log.LogRecord(Logs::High, Logs::Test, " in_path:\n%s ", in_path.c_str());
            Log.LogRecord(Logs::High, Logs::General, " input_file_data_vec:%i ", input_file_data_vec.size());
        }
        else {
            return_status = 3;
        }
        QPFunc.PREPARE_LISTDATA();

        for (unsigned int i = 0; i < transfer_string_vec.size(); ++i) {
            text_label_vec.push_back(transfer_string_vec.at(i));
        }

        transfer_string_vec.clear();
        input_filename_vec.clear();
        break;

    case CPP_ARRAY_GETMAINCOMPCODES: // F_18
        fbit_pos = 3; // get unique comp codes for the purpose of file checking

        in_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 2);

        if (input_filename_vec.size() > 0) {
            for (unsigned int i = 0; i < input_filename_vec.size(); ++i) {
                in_file = input_filename_vec.at(i);
            }
            in_path = in_dir + in_file;
            Log.LogRecord(Logs::Normal, Logs::General, "  Retrieving main.qml component codes from array data");

            PF.GetInputData(in_path);

            if (PF.CheckBitFlags(num_bits, bit_value, fbit_pos) == false) {
                QPFunc.CPP_ARRAY_GETMAINCOMPCODES(false);
            }
            else {
                QPFunc.CPP_ARRAY_GETMAINCOMPCODES(true);
            }
        }
        else {
            return_status = 3;
        }
        if (process_required == false) {
            return_status = 1;
        }
        input_filename_vec.clear();
        strID_vec.clear();
        break;

    case CPP_ARRAY_CHGMAINCOMPCODES: // F_19
        fbit_pos = 3; // switch to use p_subdir_count instead of p_func_count

        if (PF.CheckBitFlags(num_bits, bit_value, fbit_pos) == true) {
            out_path = parse_outpaths_vec.at(subfunction_count);
        }
        else {
            out_path = parse_outpaths_vec.at(function_count);
        }

        PF.GetOutputData(out_path);

        QPFunc.CPP_ARRAY_CHGMAINCOMPCODES();

        if (process_required == true) {
            Log.LogRecord(Logs::Normal, Logs::General, "  Rewriting main .cpp array to match all main.qml's components.");
            PF.FileReWrite(out_path, true);
            if (current_directive == 4) {
                return_status = 13;
            }
        }
        else {
            //set a return_status for failure
            output_file_data_vec.clear();
        }
        strID_vec.clear();
        break;

    case QML_MAIN_GETCOMPCODES: // F_20
        in_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 2);

        if (input_filename_vec.size() > 0) {
            for (unsigned int i = 0; i < input_filename_vec.size(); ++i) {
                in_file = input_filename_vec.at(i);
            }
            in_path = in_dir + in_file;

            Log.LogRecord(Logs::Normal, Logs::General, "  Retrieving main.qml component codes from file data");

            PF.GetInputData(in_path);
            QPFunc.QML_MAIN_GETCOMPCODES();
        }
        else {
            return_status = 3;
        }
        if (process_required == false) {
            return_status = 6;
        }
        input_filename_vec.clear();
        strID_vec.clear();
        break;

    case QML_MAIN_CHANGE: // F_21
        fbit_pos = 3; // switch to use p_subdir_count instead of p_func_count

        if (PF.CheckBitFlags(num_bits, bit_value, fbit_pos) == true) {
            out_path = parse_outpaths_vec.at(subfunction_count);
        }
        else {
            out_path = parse_outpaths_vec.at(function_count);
        }
        Log.LogRecord(Logs::Normal, Logs::General, "  Synchronizing main.qml components to .cpp array.");

        PF.GetOutputData(out_path);
        if (output_file_data_vec.size() > 0) {
            QPFunc.QML_MAIN_CHANGE();
            PF.FileReWrite(out_path, true);
        }
        else {
            return_status = 5;
        }
        break;

    case CPP_ARRAY_CONTINGENTOPERATIONS: // F_22 - does not need to be here
        break;

    case QML_MAIN_CONTINGENTS: // F_23
        in_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 2);

        Log.LogRecord(Logs::Normal, Logs::General, "  Processing main.qml contingent components.");

        if (input_filename_vec.size() > 0) {
            for (unsigned int i = 0; i < input_filename_vec.size(); ++i) {
                in_file = input_filename_vec.at(i);
            }
            out_path = in_dir + in_file;

            PF.GetOutputData(out_path);

            QPFunc.QML_MAIN_CONTINGENTS();
            if (process_required == true) {
                PF.FileReWrite(out_path, true);
            }
        }
        else {
            return_status = 3;
        }
        output_file_data_vec.clear();
        strID_vec.clear();
        break;

    case QML_SUPPORT_GETCONTINGENTDATA: // F_24
        num_bits = 10; // resulting supp_op checks vs "QMLsupport_operation" array, many possibilities

        if (PF.CheckBitFlags(num_bits, bit_value, 2) == true) {
            adj_bit_value = PF.GetBitValue((2 - 1), false);
            fbit_pos = PF.GetBitPos(10, bit_value - adj_bit_value);
        }
        else {
            fbit_pos = PF.GetBitPos(10, bit_value);
        }
        supp_op = fbit_pos - STANDARD_BITS;
        Log.LogRecord(Logs::High, Logs::Test, "fbit_pos: %i support_op: %i ", fbit_pos, supp_op);

        QPFunc.PrepareSupportOperation(supp_op, true, true, false);
        in_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 2);

        if (input_filename_vec.size() > 0) {
            for (unsigned int i = 0; i < input_filename_vec.size(); ++i) {
                in_file = input_filename_vec.at(i);
                in_path = in_dir + in_file;

                PF.GetInputData(in_path);
                QPFunc.QML_SUPPORT_GETCONTINGENTDATA(supp_op);
                input_file_data_vec.clear();
                process_required = true;
            }
        }
        else {
            process_required = false;
        }

        if (PF.CheckBitFlags(num_bits, bit_value, 2) == false) {
            input_filename_vec.clear();
        }
        strID_vec.clear();

        break;

    case QML_SUPPORT_UPDATECONTINGENTS: // F_25
        break;

    case GetCompIDFromMain: // F_26
        in_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 2);

        if (input_filename_vec.size() > 0) {
            for (unsigned int i = 0; i < input_filename_vec.size(); ++i) {
                in_file = input_filename_vec.at(i);
            }
            in_path = in_dir + in_file;

            PF.GetInputData(in_path);
            QPFunc.GetCompIDFromMain();

            if (process_required == false) {
                return_status = 15;
            }
        }
        else {
            return_status = 3;
        }
        input_filename_vec.clear();
        strID_vec.clear();
        break;

    case ParseIntoPages: // F_27
        if (pageID_to_path_map.size() > 0) {
            for (m_it = pageID_to_path_map.begin(); m_it != pageID_to_path_map.end(); ++m_it) {
                out_path = m_it->second;
                if (out_path.length() > 0) {
                    PF.GetOutputData(out_path);

                    QPFunc.ParseIntoPages();
                    if (process_required == false) {
                        output_file_data_vec.clear();
                    }
                    else {
                        PF.FileReWrite(out_path, true);
                    }
                }
            }
        }
        else {
            return_status = 17;
        }
        strID_vec.clear();
        break;

    case QML_CREATE_MAIN: // F_28
        num_bits = 4;
        fbit_pos = 4; // switch to use p_subdir_count instead of p_func_count

        out_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 1);
        if (PF.CheckBitFlags(num_bits, bit_value, fbit_pos) == true) {
            out_path = parse_outpaths_vec.at(subfunction_count);
        }
        else {
            out_path = parse_outpaths_vec.at(function_count);
        }

        fbit_pos = 3; // check file size bit - possibly need another for no array but in F 18
        if (PF.CheckBitFlags(num_bits, bit_value, fbit_pos) == true) {
            PF.GetOutputData(out_path);

            if (output_file_data_vec.size() < 50) {
                output_file_data_vec.clear();

                Log.LogRecord(Logs::Normal, Logs::General, "  Creating main.qml component template.");

                QPFunc.QML_CREATE_MAIN(out_dir);
                PF.FileOpen(out_path, CREATE);
                PF.FileReWrite(out_path, true);
            }
        }
        else {
            Log.LogRecord(Logs::Normal, Logs::General, "  Creating main.qml.");

            QPFunc.QML_CREATE_MAIN(out_dir);
            PF.FileOpen(out_path, CREATE);
            PF.FileReWrite(out_path, true);
        }

        output_file_data_vec.clear();
        strID_vec.clear();
        break;

    case QML_MAIN_GETITEMNOCOMPCODE: // F_29
        in_dir = PF.GetElementFromPrepFuncList(PF.GetFuncIDFromList(0), 2);

        Log.LogRecord(Logs::Normal, Logs::General, "  Parsing main.qml for components with no component code.");

        if (input_filename_vec.size() > 0) {
            for (unsigned int i = 0; i < input_filename_vec.size(); ++i) {
                in_file = input_filename_vec.at(i);
            }
            in_path = in_dir + in_file;

            PF.GetInputData(in_path);
            QPFunc.QML_MAIN_GETITEMNOCOMPCODE();
        }
        else {
            return_status = 3;
        }
        input_file_data_vec.clear();
        input_filename_vec.clear();
        strID_vec.clear();
        break;

    default:
        break;
    }       

    if (return_status > 0) {
        prompt_set = true;
    }
    PF.AddToDirStatList(pd_type, p_dir, current_directive, function_id, return_status, false);

    end_parse = false;
    return return_status;
}
