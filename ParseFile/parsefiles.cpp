#include <fstream>
#include <iostream>

#include "Logging/logging.h"

#include "parsefiles.h"
#include "parsefunctions.h"
#include "parsedirectory.h"

#include "QMLParse/qmlparse.h"

#include "uiprocess.h"


std::ifstream read_stream;
std::ofstream write_stream;

std::list<DirectiveStatus_struct> dirstat_list;
std::map<int, std::string> dir_status_string_map;
std::map<int, int> reparse_code_map;

std::vector<int> parse_directive_vec;
std::vector<std::string> parse_outdir_vec;
std::vector<std::string> parse_outpaths_vec;
std::vector<std::string> parse_indir_vec;

typedef std::vector<int>x_dim;
typedef std::vector<x_dim>y_dim;
typedef std::vector<y_dim>z_dim;

// these may not be needed
// max values between all directive_type arrays - (+ 1) is present for how the array is organized (x_dim[0] values are always 0)
const size_t max_functions(MAX_DIRTYPE_FUNCTIONS);		// x_dim - function_dirFile,function_inputFiles,function_inputFiles - max # functions + 1
const size_t max_directives(MAX_DIRECTIVES);			// x_dim - directive_functions										- max # Parse Directives + 1 
const size_t max_subdirectives(MAX_SUBDIRECTIVES);		// x_dim - subdirective_functions									- max # Parse Subdirectives + 1
const size_t max_func_calls(MAX_FUNCTION_CALLS);		// y_dim - function_dirFile,directive_functions						- max function calls
const size_t max_func_details(STRID_SUBSETS_Y_DIM);		// y_dim - function_inputFiles,function_stringIDs					- max y_dim of function infiles and strIDs - this could change but currently covers both arrays
const size_t max_subfunc_calls(MAX_SUBFUNCTION_CALLS);	// y_dim - subdirective_functions									- max subdirective function calls
const size_t max_directive_types(MaxDirectiveType);		// z_dim - all get this dimension									- # directive_types + 1
const int DefaultValue(0);

z_dim function_dirFile(max_directive_types, y_dim(max_func_calls, x_dim(max_functions, DefaultValue)));						// uses "max_func_calls" but could change to it's own type
z_dim function_inputFiles(max_directive_types, y_dim(max_func_details, x_dim(max_functions, DefaultValue)));
z_dim function_stringIDs(max_directive_types, y_dim(max_func_details, x_dim(max_functions, DefaultValue)));
z_dim directive_functions(max_directive_types, y_dim(max_func_calls, x_dim(max_directives, DefaultValue)));
z_dim subdirective_functions(max_directive_types, y_dim(max_subfunc_calls, x_dim(max_subdirectives, DefaultValue)));


static const int dirFile_from_parseFunc[5][2] = { // 5th row is a flag to get data from "inputFile_from_parseFunc" - "Filetype" may not be needed for this case
	{ 0, 0 },	// 1st "OutputDir"								- existence implies the function will try to write to a directory
	{ 0, 0 },	// 2nd "OutputFileNames"						- existence implies the function will try to write to a file
    { 0, 2 },	// 3rd "InputDir"								- existence implies the function will try to read from a directory
	{ 0, 5 },	// 4th "Filetype"								- the type of file to read from or write to when filename is not known
	{ 0, 1 },	// 5th "LoadInputFilenames" switch				- this is only a notation, it doesn't do anything uses "Filetype" to load existing input filenames of that type from a local source file to read from
				// potential column								- general data - ie. numbers, text, line/file length, other files containing lists, etc
};

const int inputFile_from_parseFunc[3][2] = { // known files from InputFileNames
	{ 0, 1 },	// InputFileNames begin							- they must be in order and they can only use the strIDs available to them
	{ 0, 1 },	// InputFileNames end							- can likely add individual items in rows below the first two or add to the vector they are placed in
	{ 0, 0 },	// subset of InputFileNames						- these will be acccessed by the subset marker as necessary and are in no particular order
};

const int stringIDs_from_parseFunc[3][2] = {
	{ 0, 0 },	// stringID begin								- they must be in order and they can only use the strIDs available to them
	{ 0, 0 },	// stringID end									- can likely add individual items in rows below the first two or add to the vector they are placed in
	{ 0, 0 },	// subset of stringIDs							- these will be acccessed by the subset marker as necessary and are in no particular order
};

static const int parseFunc_from_parseDir[5][2] = {
	{ 0, 1 },	// 1st parsefunc #
	{ 0, 0 },	// 2nd parsefunc #
	{ 0, 0 },	// 3rd parsefunc #
	{ 0, 0 },	// 4th parsefunc #
	{ 0, 0 },	// 5th parsefunc #
};

static const int parseFunc_from_parseSubDir[1][2] = { // subdirectives run common function groups
	{ 0, 0 },	// 1st parsefunc #
};


ParseFiles::ParseFiles()
{

}


/*
 startup parse will require a separate program or a dll? as recommended on stack - no idea how this would work

- post startup choices
I. Welcome
II. Instructions
III. Menu of directive types (1)
    - choice will populate next menu, add timer thing for processing (please wait)
    - this will work similar to how autocomplete poppulates
IV. Choose a Project (1)
    - choice will populate next menu, add timer thing for processing (please wait)
    - this will work similar to how autocomplete poppulates
V. Choose a directive (??)
    -

- startup script Functions

I) 15                   - handle directories
    A) F_15 - Check/create directories

II) 20,18,17,19,28,21   - handle main
     A) F_20,F_18 - Retrive main comp codes
        1) F_20 - main qml comp codes
           a) not exists, flag 1
        2) F_18 - main array comp codes
           b) not exists, flag 2
        3) flag options, flags are resolved by (exit) or (user input + action)
             - no flags, continue
             - if both not exists done
             - if one missing ask to write
           b) flag 1 - F_28 - user input 1, new main qml
             - write, done, output recompile or go to page existence, skip to III
           a) flag 2 - F_19 - user input 2, change main array
             - write, done, output recompile or go to page existence, skip to III
     B) F_17 - Analyze main comp codes
       - flag 1, main qml
       - flag 2, main array
        1) flag options, flags are resolved by (exit) or (user input + action)
             - no flags, continue
             - if differences ask which flag to resolve
           a) flag 1 - F_21 - user input 1, change main qml
             - write, done, output recompile or go to page existence, goto III
           b) flag 2 - F_19 - user input 2, change main array
             - write, done, output recompile or go to page existence, goto III

III) 5,8,10,9,12        - handle page existence
    A) F_5,F_8 - Check page array and files
        1) F_5 - page qml files
           a) not exists, flag 1
        2) F_8 - page array comp codes
           b) not exists, flag 2
        3) flag options, flags are resolved by (exit) or (user input + action)
             - no flags, continue
             - if both not exists done
             - if one missing ask to write
           b) flag 1 - F_9 - user input 1, new page qmls
             - write, done, output recompile or go to page comp codes, skip to IV
           a) flag 2 - F_10,F_12 - user input 2, change page array
                one) F_10 - get qml page comp codes
                  - if no array data, notify
                two) F_12 - write
                    - write, done, output recompile or go to page comp codes , skip to IV

IV) 5,8,10,11,12        - handle page comp codes
    A) F_5 - page qml files
    B) F_10,F_8 - Analyze page comp codes
        1) F_10 - get qml page comp codes
        2) F_8 - page array comp codes
        3) F_17 - compare page comp codes
             - no flags, continue
             - if differences ask which flag to resolve
           a) flag 1 - F_11 - user input 1, change pages qml
             - write, done, output recompile
           b) flag 2 - F_12 - user input 2, change page array
             - write, done, output recompile
           c) flag 3 - add new pages and change existing
           d) flag 4 - delete pages and change existing

V) 18,8,16            - handle file dependencies
    A) F_18 - main array comp codes
    B) F_16 - file dependencies
      - if files missing, notify
    C) F_8 - page array comp codes
    D) F_16 - file dependencies
      - if files missing, notify

VI) 18,8,22,12,19       - handle contingents
  - checks contingents and adds/removes as necessary
  - this is currently accomplished in a very specific order through "case 9:" in QMLMainParse()
  - this is due to various cross checking of array comp codes between pages and main
  - this order is dependant on memory containers being populated at various stages
  - current order - 18,22,8,22,12,18,22,19
    A) F_18 - main array comp codes
    B) F_22 - contingent operations
    C) F_8 - page array comp codes
    D) F_22 - contingent operations
    E) F_12 - write cpp page array
    F) F_18 - main array comp codes
    G) F_22 - contingent operations
    H) F_19 - write cpp main array

- user input sync comp IDs y/n
VII) 5,10,13             - handle component ID synchronizing
    A) F_5 - page qml files
    B) F_10 - get qml page comp codes
    C) F_13 - process component IDs

- user input page navigation y/n
VIII) 5,14,23,26,27      - handle page navigation
    A) F_5 - page qml files
    B) F_14 - reorder page qml files
    C) F_23 - process and write to main navigation switch
        - step VI should handle the pre-reqs
    D) F_26 - retrieve navigation component IDs(next/previous/etc buttons)
    E) F_27 - write to pages navigation compIDs
        - step VI should handle the pre-reqs

IX) 5,14,8,24,25      - handle support components
  - checks contingents and adds/removes as necessary
  - this is currently accomplished in a very specific order through "case 11:" in QMLMainParse()
  - this is due to access of various directories and files
  - this order is dependant on memory containers being populated at various stages
  - current order - 5,14,8,24,25,24,25,24,25,24,25,24,25,24,25
    A) F_5 - page qml files
    B) F_14 - reorder page qml files
    C) F_8 - page array comp codes
    D) F_24 -
    E) F_25 -

- user input sync logic IDs y/n
X) 5,6,7               - handle logic ID synchronizing
    A) F_5 - page qml files
    B) F_6 - retrieve logic IDs from arrays and organize
      - if no array data, notify
    C) F_7 - write logic IDs to .qml pages

*** need to implement a stop and resume function
*** possibly saving the progress until resumed by user input passing a bitflag or using an op code similar to Novice
*** possibly, to locate position in directive

*/

void ParseFiles::ReparseSetup(int directive_type, int parse_directive, int signal_code, int update_choice)
{
    ParseFiles PF;

    // don't need to iterate through the list here, all variables are passed and should be accurate
    // we should save the signal code and choice to memory for use within the directive type

    // may need to iterate through the list here for multiple processing of signal_codes

    switch (directive_type) {
    case QT_QML:
        Log.LogRecord(Logs::High, Logs::Test, "DIR_TYPE_%i:DIRECTIVE_%i:CODE_%i:CHOICE_%i", directive_type, parse_directive, signal_code, update_choice);

        reparse_code_map[signal_code] = update_choice;

        // function to handle signal codes and choices will be done within the flagged area since the flag above is true

        PF.MainParseFunction(directive_type, parse_directive, true);
        break;
    }
}


void ParseFiles::MainParseFunction(int directive_type, int parse_directive, bool reparse)
{
	QMLParse QP;	

    Log.LogRecord(Logs::Med, Logs::General, "MainParseFunction: (%i)", parse_directive);

	GetParsePaths(directive_type, 0); // 0 for the intial function position

	switch (directive_type) {
	case QT_QML:
        QP.QMLMainParse(parse_directive, reparse);
		break;
	default:
		ShowDirectiveTypeData(directive_type);
		GetOutDirs();
		GetOutPath();
		GetInDirs();
		GetInFiles();		

		Log.LogRecord(Logs::Normal, Logs::Error, "Invalid Parse Directive: (%i)", parse_directive);
		
		break;
	}

    // need to skip cleanup if "prompt_set" - probably not for "end_parse"
    if (prompt_set == true || end_parse == true) {
        return;
    }
    else {
        Log.LogRecord(Logs::High, Logs::Test, "DoDirectiveCleanup: (%i)", parse_directive);
        DoDirectiveCleanup(true, false, true);
    }
}

// return_status = 0 <blank> will be handled by the "directive_type"
void ParseFiles::PrepReturnCodeStrings(int directive_type)
{
    std::string ret_code_string;

    dir_status_string_map.clear(); // assure this is needed

    switch (directive_type) {
        case QT_QML:
        for (unsigned int i = 0; i < MaxQTDirStat; ++i) {
            ret_code_string = QTDirStatusString[i];
            dir_status_string_map[i] = ret_code_string;
            Log.LogRecord(Logs::High, Logs::Test, "return_code_string: %s", ret_code_string.c_str());
        }

        Log.LogRecord(Logs::High, Logs::Test, "dir_status_string_map # elements (%i)", dir_status_string_map.size());
        break;
    }
}

void ParseFiles::AddToDirStatList(int directive_type, int main_directive, int current_directive, int function_id, int directive_status, bool clear)
{
    if (directive_type < 0) {
        Log.LogRecord(Logs::Normal, Logs::Error, "  Invalid directive_type: (%i)  Unable to continue", directive_type);
        end_parse = true;
        return;
    }

    if (clear == true) {
        dirstat_list.clear();
    }

    std::map<int, std::string>::iterator m_it;

    std::string dir_status_string;

    m_it = dir_status_string_map.find(directive_status);
    if (m_it != dir_status_string_map.end()) {
        dir_status_string = m_it->second;
    }
    else {
        Log.LogRecord(Logs::Normal, Logs::Error, "AddToDirStatList: unable to find string within map dir_type(%i) main dir(%i) cur dir(%i) function(%i) dir_status(%i)",
                      directive_type, main_directive, current_directive, function_id, directive_status);
    }

    DirectiveStatus_struct dir_status;
    new DirectiveStatus_struct; // uncertain about this, if it's needed

    dir_status.dir_type = directive_type;
    dir_status.m_dir = main_directive;
    dir_status.p_dir = current_directive;
    dir_status.func_id = function_id;
    dir_status.dir_status = directive_status;
    dir_status.dir_stat_string = dir_status_string;

    dirstat_list.push_back(dir_status);

    Log.LogRecord(Logs::High, Logs::Test, "AddToDirStatList: directive_type(%i) main_directive(%i) current_directive(%i) function_id(%i) directive_status(%i) dir_status_string(%s)",
                  dir_status.dir_type, dir_status.m_dir, dir_status.p_dir, dir_status.func_id, dir_status.dir_status, dir_status.dir_stat_string.c_str());
}

// add "directive_types" as they become available
void ParseFiles::PrepareDirectiveType(int directive_type)
{
	ParseDirectory PDir;
	ParseFunctions PFunc;

	std::string in_dir;
	std::string out_dir;
	std::string file_ext;

	int func_id = 0;

	// do all LOCAL_PARSE stuff to prepare directive_type
    PDir.LoadData(0, 0);
	PrepParseDirective(0, 1);
	GetParsePaths(0, 0);
    PrepareFunction(0, 0, 0, 0);

	GetSubsetStringIDs(0, directive_type); // for directive_type setup we'll get subsets and pass directive_type as the subset	
	func_id = GetFuncIDFromList(0);
    out_dir = GetElementFromPrepFuncList(func_id, 1);
	in_dir = GetElementFromPrepFuncList(func_id, 2) + PFunc.PStr(strID_vec.at(0)); // get the full directive_type directory	
    file_ext = GetElementFromPrepFuncList(func_id, 3);

	parse_indir_vec.clear();
	parse_indir_vec.push_back(in_dir);
    AddToPrepFuncList(func_id, out_dir, in_dir, file_ext, true);
    PrepInputFiles(0, func_id);


	for (unsigned int i = 0; i < input_filename_vec.size(); ++i) {
		GetInputData(in_dir + input_filename_vec.at(i));
	}
	if (end_parse == true) {
		return;
    }

    PrepDirTypeArrayData(directive_type);

	// clean up everything
    DoDirectiveCleanup(true, true, true);
}

void ParseFiles::PrepDirTypeArrayData(int directive_type)
{
	ParseFunctions PFunc;

	int actual_xdim = 0;
	int actual_ydim = 0;

	for (unsigned int h = 0; h < function_dirFile.size(); ++h) { // function directories and files array
		if (h == directive_type) {
			actual_xdim = PFunc.ArrayDimFromFile(true, false, true, PFunc.PStr(strID_vec.at(1)), "");
			actual_ydim = PFunc.ArrayDimFromFile(true, false, false, PFunc.PStr(strID_vec.at(1)), "");
            Log.LogRecord(Logs::High, Logs::General, "(%i) x/y (%i)  function to directory and file codes", actual_xdim, actual_ydim);

			y_dim & max_func_calls(function_dirFile[h]);

			for (int i = 0; i < actual_ydim; ++i) { // max_func_calls.size()
            //	x_dim & max_functions(max_func_calls[i]);

				for (int j = 0; j < actual_xdim; ++j) { // max_functions.size()
					switch (directive_type) {
					case QT_QML:
						max_func_calls[i][j] = QMLfunction_dirFiles[i][j];
						break;
					default:
						Log.LogRecord(Logs::Normal, Logs::Error, "ERROR: directive_type invalid: (%i)", directive_type);
						break;
					}
				}
			}
		}
	}
	for (unsigned int h = 0; h < function_inputFiles.size(); ++h) { // function input files array
		if (h == directive_type) {
			actual_xdim = PFunc.ArrayDimFromFile(true, false, true, PFunc.PStr(strID_vec.at(2)), "");
			actual_ydim = PFunc.ArrayDimFromFile(true, false, false, PFunc.PStr(strID_vec.at(2)), "");
            Log.LogRecord(Logs::High, Logs::General, "(%i) x/y (%i)  function to input file code", actual_xdim, actual_ydim);

			y_dim & max_func_details(function_inputFiles[h]);

			for (int i = 0; i < actual_ydim; ++i) { // max_func_details.size()
            //	x_dim & max_functions(max_func_details[i]);

				for (int j = 0; j < actual_xdim; ++j) { // max_functions.size()
					switch (directive_type) {
					case QT_QML:
						max_func_details[i][j] = QMLfunction_inputFiles[i][j];
						break;
					default:
						Log.LogRecord(Logs::Normal, Logs::Error, "ERROR: directive_type invalid: (%i)", directive_type);
						break;
					}
				}
			}
		}
	}
	for (unsigned int h = 0; h < function_stringIDs.size(); ++h) { // function strIDs array
		if (h == directive_type) {
			actual_xdim = PFunc.ArrayDimFromFile(true, false, true, PFunc.PStr(strID_vec.at(3)), "");
			actual_ydim = PFunc.ArrayDimFromFile(true, false, false, PFunc.PStr(strID_vec.at(3)), "");
            Log.LogRecord(Logs::High, Logs::General, "(%i) x/y (%i)  function to string IDs", actual_xdim, actual_ydim);

			y_dim & max_func_details(function_stringIDs[h]);

			for (int i = 0; i < actual_ydim; ++i) { // max_func_details.size()
            //	x_dim & max_functions(max_func_details[i]);

				for (int j = 0; j < actual_xdim; ++j) { // max_functions.size()
					switch (directive_type) {
					case QT_QML:
						max_func_details[i][j] = QMLfunction_stringIDs[i][j];
						break;
					default:
						Log.LogRecord(Logs::Normal, Logs::Error, "ERROR: directive_type invalid: (%i)", directive_type);
						break;
					}
				}
			}
		}
	}
	for (unsigned int h = 0; h < directive_functions.size(); ++h) { // directive functions array
		if (h == directive_type) {
			actual_xdim = PFunc.ArrayDimFromFile(true, false, true, PFunc.PStr(strID_vec.at(4)), "");
			actual_ydim = PFunc.ArrayDimFromFile(true, false, false, PFunc.PStr(strID_vec.at(4)), "");
            Log.LogRecord(Logs::High, Logs::General, "(%i) x/y (%i)  directive to function codes", actual_xdim, actual_ydim);

			y_dim & max_func_calls(directive_functions[h]);

			for (int i = 0; i < actual_ydim; ++i) { // max_func_calls.size()
            //	x_dim & max_directives(max_func_calls[i]);

				for (int j = 0; j < actual_xdim; ++j) { // max_directives.size()
					switch (directive_type) {
					case QT_QML:
						max_func_calls[i][j] = QMLdirective_functions[i][j];
						break;
					default:
						Log.LogRecord(Logs::Normal, Logs::Error, "ERROR: directive_type invalid: (%i)", directive_type);
						break;
					}
				}
			}
		}
	}
	for (unsigned int h = 0; h < subdirective_functions.size(); ++h) { // subdirective functions array
		if (h == directive_type) {
			actual_xdim = PFunc.ArrayDimFromFile(true, false, true, PFunc.PStr(strID_vec.at(5)), "");
			actual_ydim = PFunc.ArrayDimFromFile(true, false, false, PFunc.PStr(strID_vec.at(5)), "");
            Log.LogRecord(Logs::High, Logs::General, "(%i) x/y (%i)  subdirective to function codes", actual_xdim, actual_ydim);

			y_dim & max_subfunc_calls(subdirective_functions[h]);

			for (int i = 0; i < actual_ydim; ++i) { // max_subfunc_calls.size()
            //	x_dim & max_subdirectives(max_subfunc_calls[i]);

				for (int j = 0; j < actual_xdim; ++j) { // max_subdirectives.size()
					switch (directive_type) {
					case QT_QML:
						max_subfunc_calls[i][j] = QMLsubDir_functions[i][j];
						break;
					default:
						Log.LogRecord(Logs::Normal, Logs::Error, "ERROR: directive_type invalid: (%i)", directive_type);
						break;
					}
				}
			}
		}
	}
}

void ParseFiles::PrepParseDirective(int directive_type, int parse_directive)
{
	if (end_parse == true) {
		return;
	}

	int parse_func_id = 0;
	int parse_subdirective = 0;
	int parse_sub_func_id = 0;
	
	Log.LogRecord(Logs::Normal, Logs::General, "Parse Directive Type: %i  Parse Directive: %i", directive_type, parse_directive);

	if (directive_type != 0) {
		y_dim & max_func_calls(directive_functions[directive_type]);		

		for (unsigned int i = 0; i < max_func_calls.size(); ++i) {
			if (max_func_calls[i][parse_directive] != 0) {
				parse_func_id = max_func_calls[i][parse_directive];

				if (parse_func_id < 0) {
					parse_subdirective = -parse_func_id;
                    Log.LogRecord(Logs::Med, Logs::General, " Branching into Parse Subdirective: %i", parse_func_id);

					y_dim & max_subfunc_calls(subdirective_functions[directive_type]);

					for (unsigned int j = 0; j < max_subfunc_calls.size(); ++j) {
						if (max_subfunc_calls[j][parse_subdirective] != 0) {
							parse_sub_func_id = max_subfunc_calls[j][parse_subdirective];
							parse_directive_vec.push_back(parse_sub_func_id);
							Log.LogRecord(Logs::High, Logs::General, "Parse Subdirective/FunctionID: %i:%i", parse_directive, parse_sub_func_id);
						}						
					}					
				}
				parse_directive_vec.push_back(parse_func_id);
				Log.LogRecord(Logs::High, Logs::General, "Parse Directive/FunctionID: %i:%i", parse_directive, parse_func_id);
			}
			if (parse_directive_vec.size() == 0) {
				Log.LogRecord(Logs::Normal, Logs::Error, "Parse Directive out of range: (%i)", parse_directive);
				break;
			}
		}
	}
	else if (directive_type == 0) {
		for (int i = 0; i < MAX_FUNCTION_CALLS; ++i) {
			if (parseFunc_from_parseDir[i][parse_directive] != 0) {
				parse_func_id = parseFunc_from_parseDir[i][parse_directive];
				if (parse_func_id < 0) {
					parse_subdirective = -parse_func_id;
                    Log.LogRecord(Logs::Med, Logs::General, " Branching into Parse Subdirective: %i", parse_func_id);

					for (int j = 0; j < MAX_SUBFUNCTION_CALLS; ++j) {
						if (parseFunc_from_parseSubDir[j][parse_subdirective] != 0) {
							parse_sub_func_id = parseFunc_from_parseSubDir[j][parse_subdirective];
							parse_directive_vec.push_back(parse_sub_func_id);
							Log.LogRecord(Logs::High, Logs::General, "Parse Subdirective/FunctionID: %i:%i", parse_directive, parse_sub_func_id);
						}
					}
				}
				parse_directive_vec.push_back(parse_func_id);
				Log.LogRecord(Logs::High, Logs::General, "Parse Directive/FunctionID: %i:%i", parse_directive, parse_func_id);
			}

			if (parse_directive_vec.size() == 0) {
				Log.LogRecord(Logs::Normal, Logs::Error, "Parse Directive out of range: (%i)", parse_directive);
				break;
			}
		}
	}
}

int ParseFiles::GetFunctionID(int parse_function_count)
{
	int parse_func_id = 0;

	for (unsigned int j = parse_function_count; j < parse_directive_vec.size(); ++j) {
		parse_func_id = parse_directive_vec.at(j);
	}
	parse_func_id = parse_directive_vec.at(parse_function_count);

	return parse_func_id;
}

// not much handling of invalid paths, the function just won't work
// gets all the necessary directories/files into separate memory containers based on "Parse Directive" given by the user 
void ParseFiles::GetParsePaths(int directive_type, int parse_function_count)
{
    int parse_func_id = 0;
	
	for (unsigned int j = parse_function_count; j < parse_directive_vec.size(); ++j) {
		parse_func_id = parse_directive_vec.at(j);

		// this signals the end of a subdirective, break and come back to this later - we may need separate cases for the subdirectives
		if (parse_func_id < 0) {			
			break;
		}

		for (int i = 0; i < FUNCTION_Y_DIM; ++i) {
			if (i == 0) {
				PrepParseOutpath(directive_type, parse_func_id, true);
			}
			else if (i == 1) {
				PrepParseOutpath(directive_type, parse_func_id, false);
			}
			else if (i == 2) {
				PrepParseIndir(directive_type, parse_func_id);
			}
		}
	}
}

// if files are included(true), it is implied they exist although non-existent files of a chosen PD should notify the user
void ParseFiles::PrepParseOutpath(int directive_type, int parse_function, bool directory)
{
	ParseDirectory PDir;

	std::string output_path;

	int out_dir_path = 0;
	int out_file_path = 0;	
	
	if (directive_type == LOCAL_PARSE) {
		out_dir_path = dirFile_from_parseFunc[0][parse_function];
		out_file_path = dirFile_from_parseFunc[1][parse_function];
	}
	else {
		y_dim & max_func_calls(function_dirFile[directive_type]);

		out_dir_path = max_func_calls[0][parse_function];
		out_file_path = max_func_calls[1][parse_function];
	}

	if (directory == true) {
		output_path = PDir.GetOutputDir(out_dir_path, parse_function);
		parse_outdir_vec.push_back(output_path);
	}
	else if (directory == false) {
		output_path = PDir.GetOutputPath(out_dir_path, out_file_path, parse_function);
		parse_outpaths_vec.push_back(output_path);
	}	

	output_path.clear();
}

void ParseFiles::PrepParseIndir(int directive_type, int parse_function)
{
	ParseDirectory PDir;

	std::string in_directory;

	int in_dir_path = 0;

	if (directive_type == LOCAL_PARSE) {
		in_dir_path = dirFile_from_parseFunc[2][parse_function];
	}
	else {
		y_dim & max_func_calls(function_dirFile[directive_type]);

		in_dir_path = max_func_calls[2][parse_function];
	}
		
	in_directory = PDir.GetInputDir(in_dir_path, parse_function);
	parse_indir_vec.push_back(in_directory);

    Log.LogRecord(Logs::Med, Logs::General, "PrepParseIndir: in_directory: %s", in_directory.c_str());
	in_directory.clear();
}

// with multiple subdirectives, this will need to be reset multiple times and 
void ParseFiles::PrepareFunction(int directive_type, int transfer_status, int parse_function_count, int subdirective_count)
{
	if (end_parse == true) {
		return;
	}

	std::string out_dir;
	std::string in_dir;
	std::string file_ext;

	int p_func_id = 0;
	
	p_func_id = parse_directive_vec.at(parse_function_count);
	out_dir = parse_outdir_vec.at(subdirective_count);
	in_dir = parse_indir_vec.at(subdirective_count);
	file_ext = GetFileExt(directive_type, p_func_id);

	GetStringIDs(directive_type, transfer_status, p_func_id);
	PrepInputFiles(directive_type, p_func_id);
	AddToPrepFuncList(p_func_id, out_dir, in_dir, file_ext, true); // could be an issue with subdir and negative numbers
}

std::string ParseFiles::GetFileExt(int directive_type, int parse_function)
{
	std::string file_extension;

	int file_type = 0;

	if (directive_type == LOCAL_PARSE) {
		file_type = dirFile_from_parseFunc[3][parse_function];
	}
	else {
		y_dim & max_func_calls(function_dirFile[directive_type]);

		file_type = max_func_calls[3][parse_function];
	}
	file_extension = FileType[file_type];

	return file_extension;
}

// somewhat directive/function-specific - should try to standardize a bit more
// transfer_status 2,3 is previously processed strIDs
void ParseFiles::GetStringIDs(int directive_type, int transfer_status, int parse_function)
{
	if (end_parse == true) {
		return;
	}	

	ParseFunctions PFunc;

	int stringID_begin;
	int stringID_end;
	int stringID_subset;

	if (transfer_status == 0) {
		if (directive_type == LOCAL_PARSE) {
			stringID_begin = stringIDs_from_parseFunc[0][parse_function];
			stringID_end = stringIDs_from_parseFunc[1][parse_function];
		}
		else {
			y_dim & max_func_details(function_stringIDs[directive_type]);

			stringID_begin = max_func_details[0][parse_function];
			stringID_end = max_func_details[1][parse_function];
		}

		for (int i = stringID_begin; i <= stringID_end; ++i) {
			if (i == 0) {
				if (directive_type != 0) {

                    Log.LogRecord(Logs::Med, Logs::General, "No String IDs for function: %i", parse_function);
				}
				break;
			}
			strID_vec.push_back(i);			

			Log.LogRecord(Logs::High, Logs::General, "String IDs to memory for use: %i", i);		
		}
	}
	else if (transfer_status == 1) { // includes stringID subsets
		if (directive_type == LOCAL_PARSE) {
			stringID_begin = stringIDs_from_parseFunc[0][parse_function];
			stringID_end = stringIDs_from_parseFunc[1][parse_function];
		}
		else {
			y_dim & max_func_details(function_stringIDs[directive_type]);

			stringID_begin = max_func_details[0][parse_function];
			stringID_end = max_func_details[1][parse_function];
		}		

		for (int i = stringID_begin; i <= stringID_end; ++i) {
			if (i == 0) { // if this is 0 then break, no strings required
                Log.LogRecord(Logs::Med, Logs::General, "   No contiguous String IDs inclusion for function: %i", parse_function);
				continue;
			}
			strID_vec.push_back(i);
			Log.LogRecord(Logs::High, Logs::General, "  String IDs to memory for use: %i", i);
		}

		for (int i = 2; i < STRID_SUBSETS_Y_DIM; ++i) {
			if (directive_type == LOCAL_PARSE) {
				stringID_subset = stringIDs_from_parseFunc[i][parse_function];
			}
			else {
				y_dim & max_func_details(function_stringIDs[directive_type]);

				stringID_subset = max_func_details[i][parse_function];
			}			

			if (stringID_subset == 0) {
                Log.LogRecord(Logs::Med, Logs::General, "   Dir_type: %i PFunc: %i - No String ID subset inclusion at array position: %i", directive_type, parse_function, i);
				break;
			}

			strID_vec.push_back(stringID_subset); // the subset_id is the marker			
			GetSubsetStringIDs(directive_type, stringID_subset);
		}
	}
	else if (transfer_status == 2) { // .cpp array data		
		for (unsigned int i = 0; i < array_strID_vec.size(); ++i) {
			strID_vec.push_back(array_strID_vec.at(i));
			Log.LogRecord(Logs::High, Logs::General, "   String IDs to memory for use: %i", i);
		}
	}
	else if (transfer_status == 3) { // file data		
		for (unsigned int i = 0; i < content_strID_vec.size(); ++i) {
			strID_vec.push_back(content_strID_vec.at(i));
			Log.LogRecord(Logs::High, Logs::General, "   String IDs to memory for use: %i", i);
		}
	}
}

// GetArrayNumbers from sepcific PD type
// subsetIDs are currently labelled starting at -1
void ParseFiles::GetSubsetStringIDs(int directive_type, int subset_id)
{
	// directive_type
	// 1 = QT_QML

	ParseFunctions PFunc;

	switch (directive_type) {
	case LOCAL_PARSE:
		PFunc.GetArrayNumbers(PStrSubsets[subset_id], "", 0);
		break;
	case QT_QML:
		PFunc.GetArrayNumbers(QMLSubsets[-subset_id], "", 0);
		break;
	default:
		Log.LogRecord(Logs::Normal, Logs::Error, "ERROR: directive_type invalid: (%i)", directive_type);
		break;
	}

	for (unsigned int i = 0; i < transfer_int_vec.size(); ++i) {
		strID_vec.push_back(transfer_int_vec.at(i));
		Log.LogRecord(Logs::High, Logs::General, "   GetSubsetStringIDs #: %i", transfer_int_vec.at(i));
	}
	transfer_int_vec.clear();
}

// this is used for known input files in a specific directory, GetInputFiles() checks for all unknown files of a specific extension in a specific directory
// for directive_type setup (directive_type = 0), we need to get the specific folder where the file is found - this is in strID_vec as the first element
// need to differentiate the PStr based on directive_type for the above step
// in this case (directive_type = 1) == (PStr 1-6) but how to get this when directive_type = 0 for many of these function variables at setup
// unless all thse functions are created over again and made unique for directive_type setup
void ParseFiles::PrepInputFiles(int directive_type, int parse_function)
{
	ParseDirectory PDir;

	int inputFile_begin;
	int inputFile_end;
	int inputFile_subset = 0;
	int file_transfer_status = 0;

	std::string in_file;
	std::string subset_marker = "mark";

	file_transfer_status = GetIFTransferStatus(directive_type, parse_function);

	if (file_transfer_status == 0) {
		return;
	}

	if (file_transfer_status == 1) {
		if (directive_type == LOCAL_PARSE) {
			inputFile_begin = inputFile_from_parseFunc[0][parse_function];
			inputFile_end = inputFile_from_parseFunc[1][parse_function];
		}
		else {		
			y_dim & max_func_details(function_inputFiles[directive_type]);

			inputFile_begin = max_func_details[0][parse_function];
			inputFile_end = max_func_details[1][parse_function];
		}		

		for (int i = inputFile_begin; i <= inputFile_end; ++i) {
			if (i == 0) {
				Log.LogRecord(Logs::Normal, Logs::Error, "ERROR: No input file code where one should exist for function: (%i)", parse_function);
				continue;
			}

			switch (directive_type) {
			case LOCAL_PARSE:
				in_file = InputFile[i];				
				break;
			case QT_QML:                
                in_file = QTInputFile[i];
				break;
			default:
				Log.LogRecord(Logs::Normal, Logs::Error, "ERROR: directive_type invalid: (%i)", directive_type);
				break;
			}

			if (CheckIfFileExists(false, true, in_file) == false) {
				if (directive_type != 0) {
					Log.LogRecord(Logs::Normal, Logs::Error, "Input File not found: (%s)", in_file.c_str());
				}
			}
			else {
				input_filename_vec.push_back(in_file);
				Log.LogRecord(Logs::High, Logs::General, "   Input File to memory for use: %s", in_file.c_str());
            }
		}
	}
    else if (file_transfer_status == 2) { // includes inputFile subsets
		if (directive_type == LOCAL_PARSE) {
			inputFile_begin = inputFile_from_parseFunc[0][parse_function];
			inputFile_end = inputFile_from_parseFunc[1][parse_function];
		}
		else {
			y_dim & max_func_details(function_inputFiles[directive_type]);

			inputFile_begin = max_func_details[0][parse_function];
			inputFile_end = max_func_details[1][parse_function];
		}		

		for (int i = inputFile_begin; i <= inputFile_end; ++i) {
			if (i == 0) {
				Log.LogRecord(Logs::Normal, Logs::General, "   Skipping Input File inclusion: %i", i);
				continue;
			}			

			switch (directive_type) {
			case LOCAL_PARSE:
				in_file = InputFile[i];
				break;
			case QT_QML:
				in_file = QTInputFile[i];
				break;
			default:
				Log.LogRecord(Logs::Normal, Logs::Error, "ERROR: directive_type invalid: (%i)", directive_type);
				break;
			}

			if (CheckIfFileExists(false, true, in_file) == false) {
				Log.LogRecord(Logs::Normal, Logs::Error, " Input File not found: %s", in_file.c_str());							
			}
			else {
				input_filename_vec.push_back(in_file);
				Log.LogRecord(Logs::High, Logs::General, "   Input File to memory for use: %s", in_file.c_str());
			}
		}
		for (int i = 2; i < INFILE_SUBSETS_Y_DIM; ++i) {
			if (i == 0) {
				Log.LogRecord(Logs::High, Logs::General, "   No Input File subset inclusion at position: %i", i); // this won't happen
				break;
			}

			input_filename_vec.push_back(subset_marker); // "mark" is the subset marker

			if (directive_type == LOCAL_PARSE) {
				inputFile_subset = inputFile_from_parseFunc[i][parse_function];
			}
			else {
				y_dim & max_func_details(function_inputFiles[directive_type]);

				inputFile_subset = max_func_details[i][parse_function];
			}			
			//PDir.GetSubsetInputFiles(inputFile_subset); // need to implement similar to stringID_subset
		}
	}
}

// output files can be prepped similar, likely
int ParseFiles::GetIFTransferStatus(int directive_type, int parse_function)
{
	int file_transfer_status = 0;
	int inputFile_subset = 0;

	if (directive_type == LOCAL_PARSE) {
		file_transfer_status = dirFile_from_parseFunc[4][parse_function];
		inputFile_subset = inputFile_from_parseFunc[2][parse_function];
	}
	else {
		y_dim & max_func_calls(function_dirFile[directive_type]);
		y_dim & max_func_details(function_inputFiles[directive_type]);

		file_transfer_status = max_func_calls[4][parse_function];
		inputFile_subset = max_func_details[2][parse_function];
	}	

	if (inputFile_subset > 0) {
		file_transfer_status = 2;
	}
	return file_transfer_status;
}

// function to manipultate linked list here
// this function should be placed immediately after a successful login
void ParseFiles::AddToPrepFuncList(int function_id, std::string output_directory, std::string input_directory, std::string file_extension, bool clear)
{
	if (function_id < 0) {
		Log.LogRecord(Logs::Normal, Logs::Error, "  Invalid function_id: (%i)\n    Aborting parse", function_id);
		end_parse = true;
		return;
	}

    if (clear == true) {
		prepfunc_list.clear();
	}

	PrepFuncDir_struct prep_func;
	new PrepFuncDir_struct; // uncertain about this, if it's needed

	prep_func.p_func_id = function_id;
	prep_func.out_dir = output_directory;
	prep_func.in_dir = input_directory;
	prep_func.file_ext = file_extension;

	prepfunc_list.push_back(prep_func);
    Log.LogRecord(Logs::High, Logs::General, "AddToPrepFuncList:\n funcID: %i\n out dir: %s\n in dir: %s\n file ext: %s",
                  prep_func.p_func_id, prep_func.out_dir.c_str(), prep_func.in_dir.c_str(), prep_func.file_ext.c_str());

    Log.LogRecord(Logs::High, Logs::Test, "AddToPrepFuncList: size %i", prepfunc_list.size());
}

std::string ParseFiles::GetElementFromPrepFuncList(int function_id, int element_type)
{
	// element types
	// 1 = out_dir
	// 2 = in_dir
	// 3 = file_ext

	std::list<PrepFuncDir_struct>::iterator l_it;
	std::string element; // = "";

	element.clear();

	for (l_it = prepfunc_list.begin(); l_it != prepfunc_list.end(); ++l_it) {
		if (l_it->p_func_id == function_id) {
			if (element_type == 1) {
				element = l_it->out_dir;
			}
			else if (element_type == 2) {
				element = l_it->in_dir;
			}
			else if (element_type == 3) {
				element = l_it->file_ext;
			}
			break;
		}
	}
	return element;
}

// find_id > 0 implies there may be more than one entry or verification of entry
// find_id = 0 implies only one entry in list
int ParseFiles::GetFuncIDFromList(int find_id)
{
	std::list<PrepFuncDir_struct>::iterator l_it;

	int function_id = 0;

	for (l_it = prepfunc_list.begin(); l_it != prepfunc_list.end(); ++l_it) {
		if (find_id > 0) {
			if (l_it->p_func_id == find_id) {
				function_id = l_it->p_func_id;
				break;
			}
		}
		else {
			function_id = l_it->p_func_id;
			break;
		}
	}
	return function_id;
}

void ParseFiles::GetOutputData(std::string tar_directory)
{
	if (end_parse == true) {
		return;
	}

	std::string line;
	int line_ct = 0; // involved in testing but could possibly be useful for various parsing

	FileOpen(tar_directory, READ);

	while (std::getline(read_stream, line)) {
		output_file_data_vec.push_back(line);
		++line_ct;
		Log.LogRecord(Logs::High, Logs::General, "output data: %i :%s", line_ct, line.c_str());
	}
	read_stream.close();
	line_ct = 0;
	Log.LogRecord(Logs::High, Logs::General, "output_file_data_vec.size(): %i", output_file_data_vec.size());
    Log.LogRecord(Logs::High, Logs::General, "\t\t...file data successfully moved to internal memory.");
}

void ParseFiles::GetInputData(std::string tar_directory)
{
	if (end_parse == true) {
		return;
	}

	std::string line;
	int line_ct = 0; // involved in testing but could possibly be useful for various parsing

	if (FileOpen(tar_directory, READ) == false) {
		end_parse = true;
		Log.LogRecord(Logs::Normal, Logs::Error, "ERROR GetInputData: Unable to open file.\n    %s", tar_directory.c_str());
	}	

	while (std::getline(read_stream, line)) {
		input_file_data_vec.push_back(line);
		++line_ct;
		Log.LogRecord(Logs::High, Logs::General, "input data: %i :%s", line_ct, line.c_str());	
	}
	Log.LogRecord(Logs::High, Logs::General, "Line Count = Vector size in memory: %i", line_ct);
    Log.LogRecord(Logs::High, Logs::General, "\t\t...file data successfully moved to internal memory.");
	read_stream.close();
	line_ct = 0;	
}

void ParseFiles::FileReWrite(std::string tar_directory, bool clear_mem)
{
	if (end_parse == true) {
		return;
	}

    ParseFunctions PFunc;
    std::string line;

	FileOpen(tar_directory, TRUNCATE);

	for (unsigned int i = 0; i < output_file_data_vec.size(); i++) {
		line = output_file_data_vec.at(i);
		write_stream << line << std::endl;
		Log.LogRecord(Logs::High, Logs::General, "%s", line.c_str());
	}

	if (clear_mem == true) {
		output_file_data_vec.clear();
	}
	write_stream.close();
    line = PFunc.ExtractFilenameFromString(tar_directory);

    Log.LogRecord(Logs::Normal, Logs::General, "\t\t...file (%s) re-written successfully", line.c_str());
    Log.LogRecord(Logs::High, Logs::Test, "\t\t...file (%s) re-written successfully", line.c_str());
}

// only two usages
int ParseFiles::GetSubdirectiveFunctionCt(int directive_type, int parse_subdirective)
{
	int parse_sub_func_ct = 0;
    int parse_sub_func_id = 0;

    if (parse_subdirective < 0) {
        parse_sub_func_id = -parse_subdirective;
    }

	for (int i = 0; i < MAX_SUBFUNCTION_CALLS; ++i) {
		if (directive_type == LOCAL_PARSE) {
			if (parseFunc_from_parseSubDir[i][parse_sub_func_id] != 0) {
				++parse_sub_func_ct;
			}
		}
		else {
			y_dim max_subfunc_calls(subdirective_functions[directive_type]);

			if (max_subfunc_calls[i][parse_sub_func_id] != 0) {
				++parse_sub_func_ct;
			}
		}
	}
	Log.LogRecord(Logs::High, Logs::General, "Parse Subdirective/Function Count: %i:%i", -parse_sub_func_id, parse_sub_func_ct);
	return parse_sub_func_ct;
}

// the second option has gone through a check using boost::filesystem but conditions may have changed
bool ParseFiles::CheckIfFileExists(bool check_out_file, bool file_unknown, std::string filename)
{
	ParseDirectory PDir;

	bool exists = false;

    Log.LogRecord(Logs::High, Logs::Test, "CheckIfFileExists: %i %i %s", check_out_file, file_unknown, filename.c_str());

	if (check_out_file == true) {
		if (file_unknown == false) {
			for (unsigned int i = 0; i < output_filename_vec.size(); ++i) {
				if (filename == output_filename_vec.at(i)) {
					exists = true;
					break;
				}
			}
		}
		else {
			exists = PDir.BoostCheckPath(1, "", filename);
		}
	}
	else {
		if (file_unknown == false) {
			for (unsigned int i = 0; i < input_filename_vec.size(); ++i) {
				if (filename == input_filename_vec.at(i)) {
					exists = true;
					break;
				}
			}
		}
		else {
			exists = PDir.BoostCheckPath(2, "", filename);
		}
	}
	return exists;
}

std::string ParseFiles::MakeNewFilename(std::string file_ext, std::string filename)
{
	std::string filestring;
	std::string filesuffix;

    int max_suffix_len = 3;
    int suffix_len;

	for (int i = 0; i < 1000; ++i) { // create a generic filename with ext
		filesuffix = std::to_string(i);
        suffix_len = static_cast<int>(filesuffix.length());
        for (int k = suffix_len; k < max_suffix_len; ++k) {
			filesuffix.insert(0, "0");
		}
		filesuffix.insert(0, "_");
		filestring = filename + filesuffix + file_ext;
		Log.LogRecord(Logs::High, Logs::General, " MakeNewFilename out filename: %s", filename);

		if (CheckIfFileExists(true, true, filestring) == false) {
			break;
		}
	}
	return filestring;
}

// no current uses
void ParseFiles::RemoveStringIDs(int strid_pos, int subset_pos)
{
	std::vector<int>::iterator v_it;

	int subset_counter = 0;
	int id_pos1 = 0;
	int id_pos2 = 0;

	bool found_first_pos = false;

	v_it = strID_vec.begin();

	if (strid_pos == -1) {
		for (unsigned int i = 0; i < strID_vec.size(); ++i) {
			if (strID_vec.at(i) < 0) {
				++subset_counter;
			}

			if (subset_counter > subset_pos) {
				id_pos2 = i + 1;
				break;
			}
			else if (subset_counter == subset_pos && found_first_pos == false) {
				id_pos1 = i;
				found_first_pos = true;
			}
		}
		strID_vec.erase(v_it + id_pos1, v_it + id_pos2);
		Log.LogRecord(Logs::High, Logs::General, "%i pos1:pos2 %i", id_pos1, id_pos2);
	}
	else {
		strID_vec.erase(v_it + strid_pos);
	}
}

// number of files listed in source file for a specific function - only one usage
unsigned int ParseFiles::GetInputFileCt(int directive_type, int parse_function_count)
{
	int p_func_id = 0;
	int inputFile_begin = 0;
	int inputFile_end = 0;
	int inputFile_subset = 0;
	int file_transfer_status = 0;
	int file_count = 0;

	p_func_id = parse_directive_vec.at(parse_function_count);

	file_transfer_status = GetIFTransferStatus(directive_type, p_func_id);

	if (file_transfer_status == 1) {

		if (directive_type == LOCAL_PARSE) {
			Log.LogRecord(Logs::Normal, Logs::General, "LOCAL_PARSE: nothing here: %i", directive_type);
		}
		else {
			y_dim & max_func_details(function_inputFiles[directive_type]);
			 
			inputFile_begin = max_func_details[0][p_func_id];
			inputFile_end = max_func_details[1][p_func_id];
		}		

		for (int i = inputFile_begin; i <= inputFile_end; ++i) {
			++file_count;
		}
	}
	else if (file_transfer_status == 2) { // includes inputFile subsets

		if (directive_type == LOCAL_PARSE) {
			Log.LogRecord(Logs::Normal, Logs::General, "LOCAL_PARSE: nothing here: %i", directive_type);
		}
		else {
			y_dim & max_func_details(function_inputFiles[directive_type]);

			inputFile_begin = max_func_details[0][p_func_id];
			inputFile_end = max_func_details[1][p_func_id];
		}	

		for (int i = inputFile_begin; i <= inputFile_end; ++i) {
			++file_count;
		}

		for (int i = 2; i < INFILE_SUBSETS_Y_DIM; ++i) {

			if (directive_type == LOCAL_PARSE) {
				Log.LogRecord(Logs::Normal, Logs::General, "LOCAL_PARSE: nothing here: %i", directive_type);
				
			}
			else {
				y_dim & max_func_details(function_inputFiles[directive_type]);

				inputFile_subset = max_func_details[i][p_func_id];
			}
			//PDir.GetSubsetInputFiles(inputFile_subset); // need to implement similar to stringID_subset
		}
	}
	return file_count;
}

bool ParseFiles::FileOpen(std::string tar_directory, int open_status)
{
	if (read_stream.is_open()) {
		read_stream.close();
	}
	if (write_stream.is_open()) {
		write_stream.close();
	}
	bool valid = false;

	if (open_status == CREATE) {
		if (std::ifstream(tar_directory)) {
			Log.LogRecord(Logs::High, Logs::General, "File already exists!\n  %s", tar_directory.c_str());			
			return valid;
		}
	}
	else if (!(std::ifstream(tar_directory))) {
		Log.LogRecord(Logs::High, Logs::General, "File does not exist!\n  %s", tar_directory.c_str());
		return valid;
	}

	switch (open_status) {
	case READ:
		read_stream.open(tar_directory, std::ios_base::out | std::fstream::in);
		valid = true;
		break;
	case CREATE:
		write_stream.open(tar_directory, std::ios_base::out | std::ios_base::app);
		valid = true;
		break;
	case APPEND:
		write_stream.open(tar_directory, std::ios_base::app | std::ios_base::out);
		valid = true;
		break;
	case TRUNCATE:
		write_stream.open(tar_directory, std::ios::trunc);
		valid = true;
		break;
	default:
		write_stream.open(tar_directory, std::ios_base::in | std::ios_base::out);
		valid = true;
		break;
	}
	return valid;
}

// many of these are used for each parse directive, this will become more complex as subdirectives are added
void ParseFiles::DoDirectiveCleanup(bool clear_directive, bool clear_paths, bool clear_temp_data)
{
	ParseDirectory PDir;

	if (clear_directive == true) {
		parse_directive_vec.clear();
	}	
	if (clear_paths == true) {
		PDir.ClearPathData();
    }
    if (clear_temp_data == true) {
        ClearTempData();
    }

	parse_outdir_vec.clear();
	parse_indir_vec.clear();	
	parse_outpaths_vec.clear();
    input_filename_vec.clear();

	output_file_data_vec.clear();
	input_file_data_vec.clear();

    strID_vec.clear();

	Log.LogRecord(Logs::High, Logs::General, "Parse Directive cleaned up.");
}

void ParseFiles::ClearTempData()
{
    transfer_i_string_map.clear();
    transfer_s_string_map.clear();

    transfer_string_vec.clear();
    transfer_int_vec.clear();
    temp_int_vec.clear();

    array_strID_vec.clear();
    content_strID_vec.clear();
}

// expand this for more bits
bool ParseFiles::CheckBitFlags(int num_bits, int bit_value, int bit_pos)
{
	double total_bit_value;
	int i;
	int iter;

	bool bit_on = false;

	total_bit_value = pow(2, num_bits);
	Log.LogRecord(Logs::High, Logs::General, "total_bit_value: %d", total_bit_value);

	for (i = 1, iter = 1; i < total_bit_value; i <<= 1, iter++) { // the numeric value "255" represents the max bitmask range(8-bit)
		if (bit_value & i) {
			switch (bit_pos) {
			case 1:
				if (iter == bit_pos) {
					bit_on = true;
				}
				break;
			case 2:
				if (iter == bit_pos) {
					bit_on = true;
				}
				break;
			case 3:
				if (iter == bit_pos) {
					bit_on = true;
				}
				break;
			case 4:
				if (iter == bit_pos) {
					bit_on = true;
				}
				break;
			case 5:
				if (iter == bit_pos) {
					bit_on = true;
				}
				break;
			case 6:
				if (iter == bit_pos) {
					bit_on = true;
				}
				break;
			case 7:
				if (iter == bit_pos) {
					bit_on = true;
				}
				break;
			case 8:
				if (iter == bit_pos) {
					bit_on = true;
				}
				break;
			case 9:
				if (iter == bit_pos) {
					bit_on = true;
				}
				break;
			case 10:
				if (iter == bit_pos) {
					bit_on = true;
				}
				break;
			}
		}
	}
	return bit_on;
}

unsigned int ParseFiles::GetBitValue(int bit_pos, bool total_value)
{
	double bit_value = 0;

	if (total_value == false) {
		bit_value = pow(2, bit_pos);
	}
	else {
		for (int i = 0; i < bit_pos + 1; ++i) {
			bit_value = bit_value + pow(2, i);
		}
	}
	return static_cast<unsigned int>(bit_value);
}

// expand this for more bits
int ParseFiles::GetBitPos(int num_bits, int bit_value)
{
	double total_bit_value;
	int i;
	int iter;
	int return_value = -1;

	total_bit_value = pow(2, num_bits);
	Log.LogRecord(Logs::High, Logs::General, "total_bit_value: %d", total_bit_value);

	for (i = 1, iter = 1; i < total_bit_value; i <<= 1, iter++) {
		if (bit_value & i) {
			switch (iter) {
			case 1:
				return_value = iter;
				break;
			case 2:
				return_value = iter;
				break;
			case 3:
				return_value = iter;
				break;
			case 4:
				return_value = iter;
				break;
			case 5:
				return_value = iter;
				break;
			case 6:
				return_value = iter;
				break;
			case 7:
				return_value = iter;
				break;
			case 8:
				return_value = iter;
				break;
			case 9:
				return_value = iter;
				break;
			case 10:
				return_value = iter;
				break;
			}
		}
	}
	return return_value;
}

// the functions below are for testing
void ParseFiles::ShowDirectiveTypeData(int directive_type)
{
	
	// show all - checking the various 3D containers will work similar to this but with more conditions/variables passed
	for (unsigned int h = 0; h < function_dirFile.size(); ++h) {
		if (h == directive_type) {
			std::cout << "function to directory and file codes" << std::endl;
			y_dim & max_func_calls(function_dirFile[h]);

			for (unsigned int i = 0; i < max_func_calls.size(); ++i) {
				x_dim & max_functions(max_func_calls[i]);

				for (unsigned int j = 0; j < max_functions.size(); ++j) {
					std::cout << max_functions.at(j) << ",";
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
	}
	for (unsigned int h = 0; h < function_inputFiles.size(); ++h) {
		if (h == directive_type) {
			std::cout << "function to input file code" << std::endl;
			y_dim & max_func_details(function_inputFiles[h]);

			for (unsigned int i = 0; i < max_func_details.size(); ++i) {
				x_dim & max_functions(max_func_details[i]);

				for (unsigned int j = 0; j < max_functions.size(); ++j) {
					std::cout << max_functions.at(j) << ",";
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
	}
	for (unsigned int h = 0; h < function_stringIDs.size(); ++h) {
		if (h == directive_type) {
			std::cout << "function to string IDs" << std::endl;
			y_dim & max_func_details(function_stringIDs[h]);

			for (unsigned int i = 0; i < max_func_details.size(); ++i) {
				x_dim & max_functions(max_func_details[i]);

				for (unsigned int j = 0; j < max_functions.size(); ++j) {
					std::cout << max_functions.at(j) << ",";
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
	}
	for (unsigned int h = 0; h < directive_functions.size(); ++h) {
		if (h == directive_type) {
			std::cout << "directive to function codes" << std::endl;
			y_dim & max_func_calls(directive_functions[h]);

			for (unsigned int i = 0; i < max_func_calls.size(); ++i) {
				x_dim & max_directives(max_func_calls[i]);

				for (unsigned int j = 0; j < max_directives.size(); ++j) {
					std::cout << max_directives.at(j) << ",";
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
	}
	for (unsigned int h = 0; h < subdirective_functions.size(); ++h) {
		if (h == directive_type) {
			std::cout << "subdirective to function codes" << std::endl;
			y_dim & max_subfunc_calls(subdirective_functions[h]);

			for (unsigned int i = 0; i < max_subfunc_calls.size(); ++i) {
				x_dim & max_subdirectives(max_subfunc_calls[i]);

				for (unsigned int j = 0; j < max_subdirectives.size(); ++j) {
					std::cout << max_subdirectives.at(j) << ",";
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
	}
}

void ParseFiles::GetOutDirs()
{
	for (unsigned int i = 0; i < parse_outdir_vec.size(); ++i) {
		std::cout << " out dir to parse to: " << parse_outdir_vec.at(i) << std::endl;
	}
}

void ParseFiles::GetOutPath()
{
	for (unsigned int i = 0; i < parse_outpaths_vec.size(); ++i) {
		std::cout << " outpath to parse to: " << parse_outpaths_vec.at(i) << std::endl;
	}
}

void ParseFiles::GetInDirs()
{
	for (unsigned int i = 0; i < parse_indir_vec.size(); ++i) {
		std::cout << " in dir to parse from: " << parse_indir_vec.at(i) << std::endl;
	}
}

void ParseFiles::GetInFiles()
{
	for (unsigned int i = 0; i < input_filename_vec.size(); ++i) {
		std::cout << " in files to check: " << input_filename_vec.at(i) << std::endl;
	}
}

void ParseFiles::GetStrIDs()
{
    for (unsigned int i = 0; i < strID_vec.size(); ++i) {
        std::cout << " string IDs: " << strID_vec.at(i) << std::endl;
    }
}
