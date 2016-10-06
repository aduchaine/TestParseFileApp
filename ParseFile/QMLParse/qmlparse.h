#ifndef QMLPARSE_H
#define QMLPARSE_H


#define QMLID_START 200			// corresponds to the low range of search strIDs
#define QMLID_END 299			// corresponds to the high range of search strIDs
#define STANDARD_BITS 2			// used for HandleFunction() for constant bit_pos


enum QTDirectiveStatus {

    MaxQTDirStat = 26

};

static const char* QTDirStatusString[QTDirectiveStatus::MaxQTDirStat] = {

        // 0: do nothing, everything is okay
    "No flags set. Continuing process.",
        // 1: possibility to write or end parse
    "Unable to retrieve .cpp array main component codes."
    "\nAttempt to update the array with the option below or check UIProcess.cpp file for issues.",
        // 2: possibility to write or end parse
    "Unable to retrieve .cpp array page component codes."
    "\nAttempt to update the array with the option below or check UIProcess.cpp file for issues.",
        // 3: mandatory end parse
    "No input files in .cpp file.",
        // 4: possibility to write or end parse
    "Unable to retrieve .qml page files.",
        // 5: possibility to write or end parse - bad directories possibly, if previous flags were not tripped, nothing else explains this - F_18 prev
    "Unable to retrieve .qml main file data.",
        // 6: possibility to write or end parse - probably bad formats or blank, this is dissimilar to case 7
    "Unable to retrieve .qml main component codes."
    "\n\"main.qml\" is required and no console program exists to create. It must be created manually.",
        // 7: possibility to write or end parse - probably bad formats or blank, if previous flags were not tripped, F_5 prev
    "Unable to retrieve .qml page component codes.",
        // 8: possibility to write or end parse - nothing set up to read/analyze then write/insert into pages
    "More pages exist in the array than the .qml files."
    "\nCreate new .qml pages with the option below or alter the array manually.",
        // 9: possibility to write or end parse - probably bad formats if previous flags were not tripped, F_5 prev - this is the same result as "case 7"
    "Unable to retrieve .qml page component IDs.",
        // 10: possibility to write or end parse - probably bad formats or blank, if previous flags were not tripped, F_5 prev
    "Unable to locate Page ID in .qml page file(s)."
    "\nManually check file(s) for pageID.",
        // 11: mandatory end parse - F_15
    "No output directory in .cpp file."
    "\nCheck program logic.",
        // 12: cannot continue parse, program flawed
    ".cpp dependency paths missing from program data set."
    "\nCheck program logic or existence of data list .txt file.",
        // 13: not an error, flag to rewrite main.qml
    "Array main component codes changed. Must rewrite main.qml."
    "\nClick to update and continue.",
        // 14: not an error, flag to rewrite pages
    "Array page component codes changed. Must rewrite page .qmls."
    "\nClick to update and continue.",
        // 15: no navigation comp codes cannot continue this directive
    "Unable to retrieve navigation component code string IDs from .cpp file.",
        // 16: cannot rewrite, cannot continue this directive
    ".cpp array page component codes missing from data set."
    "\nCheck program logic.",
        // 17: possibility to write or end parse - bad directories possibly, if previous flags were not tripped, nothing else explains this - F_5 prev
    ".qml page files missing from data set."
    "\nCheck program logic.",
        // 18: cannot rewrite, cannot continue this directive
    "Unable to retrieve .cpp array logic IDs."
    "\nManually check the file.",
        // 19: rewrite component code .cpp array or rewrite page .qml files
    "Differences exist between the array and the .qml page files."
    "\nChoose which one to change.",
        // 20: unable to parse differences accurately
    "There are too many differences between the .qml file and .cpp array to accurately parse and keep existing data intact."
    "\nMake changes to the file manually and then re-sync to the array.",
        // 21: processing main.qml notify
    "Processing differences between array and main.qml.",
        // 22: processing .qml pages notify
    "Processing differences between array and page .qmls.",
        // 23: possibility to write or end parse - nothing set up to read/analyze then write/insert into pages
    "Fewer pages exist in the array than the .qml files."
    "\nNo console program exists and nothing is currently setup for this circumstance.",
        // 24: rewrite component code .cpp array or rewrite main.qml
    "Differences exist between the array and the main.qml file."
    "\nChoose which one to change.",
        // 25: do not need to end parse, program will not function, get missing required filenames
    ".qml dependency files missing from repository."
    "\nFind the files and assure they are within the proper repo folder.",

};

enum QTRootDir {

    Nadas = 0,
    PARSEFILE_ROOT,
    TESTPARSEDAPP_ROOT,
    MaxQTRootDir

};

// another option is to use QtAPP root directory of project/program and find the targets from there
// the # of projects = the # of root directories therefore, the # of entries in the file with the projects should = the # root directories
static const char * QTRootDirectory[QTRootDir::MaxQTRootDir] = {

    "",
    "C:/Users/Dude/Documents/Development/Qt_development/TestParseFileApp/",
    "C:/Users/Dude/Documents/Development/Qt_development/TestParsedApp/",
};

enum QTOutDir {

    Nada = 0,
    APP_OUTPAGES,
    APP_OUTSUPPORT,
    MaxOutputDir

};

// this directory must change if the location of the program files change
// another option is to use QtAPP root directory of project/program and find the targets from there
static const char* QTOutputDirectory[QTOutDir::MaxOutputDir] = {

    "",
    "Pages/",
    "SupportComponents/",

};

enum QTOutFile {

	Zilch = 0,
	DATACODES_H,
	DATACODES_CPP,
	UIPROCESS_CPP,
	OUT_MAIN_QML,
	OUT_INTERPAGEPROCESSES,
	OUT_KEYBOARDPROP1,
	OUT_WRITELINETEXT,
	MaxOutputFile

};

static const char* QTOutputFile[QTOutFile::MaxOutputFile] = {

	"",
	"datacodes.h",
	"datacodes.cpp",
	"uiprocess.cpp",
	"main.qml",
	"InterpageProcesses.qml",
	"KeyboardProp1.qml",
	"WriteLineText.qml",

};

enum QTInDir {

	NothNess = 0,
	APP_INPAGES,
	APP_DATAFILES,
	APP_DATALISTS,
	QML_FILES,
	MaxQMLInDir

};

// this directory must change if the location of the program files change
// another option is to use root directory of project/program and find the targets from there
static const char* QTInputDirectory[QTInDir::MaxQMLInDir] = {

    "",
    "Pages/",
	"DataFiles/",
    "DataFiles/Lists/",	
    "C:/Users/Dude/Documents/Development/Qt_development/Qml_files/",             // QtApp developement files

};

enum QTInFile {

	Dull = 0,
	APP_UIPROCESS,
	QML_FILENAMES,
	QML_DEPENDANTS,
	QML_CONTINGENTS,
	IN_MAIN_QML,
	DATA_LISTS,
	MaxQMLInFile

};

static const char* QTInputFile[QTInFile::MaxQMLInFile] = {

	"",
	"uiprocess.cpp",
	"qml_filenames.txt",
	"qml_dependants.txt",
	"qml_contingents.txt",
	"main.qml",
	"data_lists.txt"

};

enum QMLStrIDSubsets {

	NOTHING = 0,
	MAINQMLNEW,					// main.qml template
	SPEC_COMPCODE_MAIN,			// main.qml special case parse strings - these are in a specific order
	PAGEQMLNEW,					// new page .qml template
	COMPNOID,					// comp codes with no comp id - better option may be to check those WITH an ID
	CONTCODE_WLT,				// contingent component codes which use WriteLineText qml functions
	CONTCODE_KP,				// contingent component codes which use KeyboardProp1 qml functions
	CONTCODE_PGSLT,				// contingent component codes for SLText qml page functions
	CONTCODE_PGMENU,			// contingent component codes for DDMenu/ACTMenu qml page functions
	CONTCODE_PGFIELD,			// contingent component codes for all components which qualify as a field (only 4)
	CONTCODE_NAV,				// contingent component codes for main.qml components which // make sthis more efficient with what exists and below
	CONTCODE_PGNAV,				// contingent component codes for page .qmls components
	NOCOMPCODE_FILE,			// items which do not have a component code but require parsing
	COMP_PAGEIDONLY,			// component codes whose components have IDs with only the pageID included in it
	MaxSubsets

};

static const char * QMLSubsets[QMLStrIDSubsets::MaxSubsets] = {

	"",
	"{ 160, 161, 162, 163, 164, 165, 166, 167, 168 }",
	"{ 262, 274, 268 }",
	"{ 100, 101, 102, 103 }",
	"{ 100, 101, 102, 103, 115, 119, 120, 121, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 150, 153, 154, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 180, 181, 182, 183, 184 }",
	"{ 112, 114 }",
	"{ 115 }",
	"{ 112 }",
	"{ 113, 114 }",
	"{ 112, 113, 114, 116 }",
	"{ 270, 269 }",
	"{ 230, 231, 232, 233 }",
    "{ 263, 268 }",
    "{ 113, 114, 115, 122 }",

};

// should create another array or add to subsets above for specific folders based on project/root

// remove F 17 from these arrays, F 25 atm also - anything which doesn't require a file or strID_vec find strings does not need to be included here and can be classified differently
static const int QMLfunction_dirFiles[5][30] = { // equivalent LOCAL_PARSE = dirFile_from_parseFunc
	//									parse_function:
	//0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29

    { 0, 1, 1, 2, 2, 0, 0, 2, 0, 2, 0, 2, 1, 2, 2, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 2, 1, 0 },  // out dir - root directory counts as 1
    { 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3, 0, 4, 0, 4, 0, 0, 0, 0, 4, 0 },  // out file
    { 0, 4, 4, 0, 0, 2, 1, 0, 1, 0, 2, 0, 0, 0, 0, 0, 5, 3, 1, 0, 1, 0, 5, 1, 2, 0, 1, 0, 0, 1 },  // in dir - root directory counts as 1
	{ 0, 2, 2, 3, 0, 3, 4, 0, 4, 3, 3, 0, 0, 0, 0, 0, 2, 2, 4, 0, 3, 0, 2, 3, 0, 0, 4, 0, 0, 4 },	// extensions - No_type = 0,ALL_TYPES,TEXT,QML,CPP,HEADER,MaxFileType
    { 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1 },  // in file switch to populate using known files

};

static const int QMLfunction_inputFiles[3][30] = { // equivalent LOCAL_PARSE = inputFile_from_parseFunc
	//									parse_function:
	//0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29

	{ 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 6, 1, 0, 5, 0, 4, 5, 0, 0, 5, 0, 0, 5 },
	{ 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 3, 6, 1, 0, 5, 0, 4, 5, 0, 0, 5, 0, 0, 5 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

static const int QMLfunction_stringIDs[3][30] = { // equivalent LOCAL_PARSE = stringIDs_from_parseFunc
	//											parse_function:
	//0  1   2   3   4   5   6   7   8  9  10 11 12  13  14  15  16 17  18 19  20 21 22  23  24 25   26   27 28   29

	{ 0, 1,  7,  0, 12, 12, 13, 16, 24, 0, 12, 0, 0, 26, 12, 28, 28, 0, 25, 0,  0, 0, 0, 33, 37, 0,   0,   0, 0,   0 },
	{ 0, 6, 11,  0, 12, 12, 15, 23, 24, 0, 12, 0, 0, 27, 12, 32, 31, 0, 25, 0,  0, 0, 0, 36, 37, 0,   0,   0, 0,   0 },
	{ 0, 0,  0, -3,  0,  0,  0,  0,  0, 0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0, -2, 0, 0,  0,  0, 0, -10, -11, 0, -12 },
};

static const int QMLdirective_functions[5][13] = { // equivalent LOCAL_PARSE = parseFunc_from_parseDir
	//				parse_directive:
    //0   1   2   3   4   5  6   7   8  9  10  11  12

    { 0, -1, -2, 18, -3,  5, 5,  5,  5, 8, -4,  5, 17 },
    { 0, 28,  9, 29, -4, 10, 6, 14, 14, 9, 11, 14,  1 },
    { 0, 19, 12, 16, -5, 13, 7,  8, 23, 0, 12,  3,  2 },
    { 0, 21, 11,  8,  0,  0, 0, 24, 26, 0,  0,  0,  0 },
    { 0,  0,  0,  0,  0,  0, 0, 25, 27, 0,  0,  0,  0 },


};

static const int QMLsubDir_functions[11][8] = { // equivalent LOCAL_PARSE = parseFunc_from_parseSubDir
	//	parse_subdirective:
	//0  -1	 -2  -3  -4  -5  -6  -7

	{ 0, 15,  5, 18, 18,  5,  5, 15 },
	{ 0, 18,  8, 22, 20,  8, 10, 28 },
	{ 0, 20, 10,  8, 21, 10, 13,  8 },
	{ 0,  0,  0, 12,  0, 11,  8,  5 },
	{ 0,  0,  0, 19,  0,  0,  0, 14 },
	{ 0,  0,  0,  0,  0,  0,  0, 16 },
	{ 0,  0,  0,  0,  0,  0,  0, 10 },
	{ 0,  0,  0,  0,  0,  0,  0, 17 },
	{ 0,  0,  0,  0,  0,  0,  0, 13 },
	{ 0,  0,  0,  0,  0,  0,  0,  6 },
	{ 0,  0,  0,  0,  0,  0,  0,  7 },

};


class QMLParse
{
public:
	QMLParse();

    void QMLMainParse(int parse_directive, bool reparse);
    int QMLSubdirective(int parse_subdirective, int parse_function_count, int subdirective_count);
    int HandleFunctions(int current_directive, int function_id, int str_transfer_status, int function_count, int subfunction_count, int bit_value);

};

extern std::string approot_directory;

#endif // QMLPARSE_H
