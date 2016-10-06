#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>

#include "Logging/logging.h"

#include "parsedirectory.h"

#include "uiprocess.h"
#include "datacodes.h"


Logging Log;

/*

DirectiveType -> directive_types.txt,
root_dir -> projects.txt,
parse directives ->  directive.txt

Step 1: Directive Types
    - processing -> ParseFile: enumeration of all types are located in parsefile.h "DirectiveType"
        - in parsedirectory, where the paths are constructed, the "DirectiveType" links to the source folder where the root directories can be found
            ie. "QTRootDirectory" in qmlprase.h has the project
    - UI -> data lists - directive_types.txt
        - located in "datalists/" - these can be referenced by the filedatasize() because the enumeration does not coincide with the position of the click
            - we should create a standard throughout the system before proceeding

    - data_code 1- dir_type response determines data code for the second field

*/

/*
- need a character handling function probably similar to the chat char input function in Novice for loading file data and special characters

- implement datacode for file checks within the directive type for directive and project
    - these files should be exclusive to the directive type and located within that folder

- left off at case 5 and case -6 for checking "directive_code" - think it's done


- issue with menu navigation(2) - arrows go out of bounds and enter/return selects incorrect item - likely an easy fix - fixed

- add splashpage changetext() function changes to other text field usages, esp RoT

- QML_PG_CREATE if there aren't any pages we need to process differently

- lots of component updates to go into QMLFiles folder - all support components should be updated already
- fix focus on ProcessDynamicMenu() - clicking the menu twice becomes necessary after the dynamic process - fixed
- data lists - each directive_type.txt element should have as a secondary element the filename of the datalist where the menus will lead to...
    - the other option is to create a 2D array with directive_type and links to the next file...
    - this should occur for as many datalists as their are in each directive_type tree
- need to account for insertion of data lists - PD 12
- go through "debug_log_2016-09-6" to reduce what is parsed to make more efficient
- add option for background.png resources to parse in - partially done, within main.qml
- other parse items to add located in splashpage.qml - partially done
- consider leaving file streams open during a session, it seems constant opening and closing of the streams takes a fair amount of time
    - check time differences between using fopen and boost and change accordingly
    - don't use file open to check if exists maybe
*/

/* QT directive notes
// POSSIBLE_USER_INPUT:  depends on directive -> a menu would be available for out_dir, in_dir where necessary	-  the <transfer_status> in getstringIDs will eventually be passed via user input
int directive_type = 1;			// 1 = QT parses
int parse_directive = 11;		// 2nd choice - // 12, 5, 13, 4, 10, 11 - complete create

// these choices will be prompted as necessary by the choices above
int ui_page_num = -1;			// -1 = all pages (default)
int user_choice = 0;			// change .cpp array or .qml page file (could be a switch)
std::string user_file;			// file naming
bool user_switch = false;		// 2 possibilities: change/sync or get componentIDs - reordering pageIDs
bool do_all = false;			// process all switch available based on PD - if all ui_page_num = -1 or default to -1 and don't set


// to do/test
PD 6 erased compcodes incorrectly then put them back on a second run for the cpp array
AppendQMLComponents() should probably pass another argument(variable)
F_11 QML_PG_CHANGE and likely QML_MAIN_CHANGE does not put accurate compIDs in some contingetn areas, need to parse these (ie. in pages- kbpropDDmenu.onFocusChanged: interpageprocspg)
    - interpageprocspg and possibly keyprop1 complete
fix datacodes lists to populate in a specific order possibly by making the first line the position and then omitting the first line when populating the string...
    ...or adding a header file to read first with the order in the file in a delimited format, adding the list label or name here may be good


PARSE DIRECTIVES
case 1:     Subdirective -1 - check/create directories, retrieve main .qml and array comp codes
            change "main" components in .qml file from .cpp array  - testApp/ (simple insert/erase/replace page components)
            change "main" components in .cpp array from .qml file - testApp/ (array changes-> rewrite array)
case 2:		process page .qml data
            Subdirective -2 - match page IDs to filenames, retrieve page .qml and array comp codes

case 3:     populate dependency files
case 4:		change .qml files with contingent changes
            Subdirective -3 - check/change all contingent components
                change "main" contingent components .cpp array - testApp/ (array changes-> rewrite array)
                    - assures no duplicates of certain comp codes, cross checks pages where necessary, cross checks main, inserts/changes codes/array dim as needed
                change "page" contingent components .cpp array - testApp/ (array changes-> rewrite array)
                    - cross checks pages, cross checks main where necessary, inserts/changes codes/array dim as needed
case 5:		reorder .qml component IDs within "page" file - testApp/pages/ (single page or all pages)
            retrieve .qml component IDs within "page" file - testApp/pages/ (single page or all pages)
case 6:		sync global IDs in all .qml "page" files to .cpp arrays - testApp/ & testApp/pages/ (menuID, textID, fieldID, responseID)
                - this will not produce accurate results if components are changed - the .cpp arrays in testApp/ must be altered
case 7:     alters all SupportComponents .qml files valid compIDs through all page files, (enumerates, deletes, inserts)
                - does not do "onNavigateSideways()" because the UI logic is not complete
case 8:     alters "main" page filename navigation switch, alter page navigation contingents (enumerates, deletes, inserts in main; inserts in pages) - uses main.qml compIDs

case 9:		create .qml "page" file from .cpp array - testApp/pages/ (creates as many pages as the array defines; component IDs set upon insertion)
case 10:	change "page" components in .qml file from .cpp array  - testApp/pages/ (single page file or all pages; simple insert/erase/replace page components)
            change "page" components in .cpp array from .qml file - testApp/pages/ (single page file or all pages; array changes-> rewrite array)
case 11:    create basic .qml "page" file - testApp/pages/ - (new page file with minimal components; will shift pageIDs if inserted) fix F 14 to not write if no change
case 12:	add .txt filenames to datacodes.h/.cpp for data lists - testApp/

PARSE SUBDIRECTIVES
case -1:	check/create directories, retrieve main .qml and array comp codes
case -2:	match page IDs to filenames, retrieve page .qml and array comp codes
case -3:
case -4:	sync main .qml from array
case -5:	sync page .qmls from array

case -6: 	snyc filename to pageID, prepare content pg compIds, reorder compIDs within file and prepare compID map(no uses), prepare array pg compIds - testApp/
                - this will likely evolve into a setup sub which would be executed prior to changing anything related to pg compIDs or compIDs within file
                - under these circumstances, some of the functions included/soon-to-be-included in this sub would be executed at the end of the
case -7:	entire check of TestApp UI arrays/files (main + pages) - not much or any file creation atm - disabled


// flags -- sbit_pos for case 1,2
subdir flag = 1
flag a = 2
flag b = 3
flag c = 4
flag d = 5
flag e = 6
*/

/*
 * dynamic field notes
- use CheckFieldProcess() to omit flagged menu data from being populated
- can call an array contingent fields where the x_dim will be the fieldID and y_dim the contingent or required process
- or check page processes and use that to set access although this does not set specific fields explicitly
- GetContingentFields() linkage to HandleDynamicText() needs some work
- field_process == true -> this process needs work
- show contingent data, no change state
- need to link the field to the number of dynamic text possibilitites
- there should not be more than one contingent item per field, if multiple items need to occur, additional implementation needs to happen
- most of the dynamic text system in HandleDynamicText - separate functions for use outside of this function are commented out at the bottom
    - this only handles two

*/

/*
- dynamic field handling needs an overhaul to account for different dir_types
    - automatic fields need to be displayed with all others invisible - this is not handled currently
    - pages with dynamic fields probably need to be labelled or flagged as such
    - once a dir_type is selected, normal dynamic field handling can probably be performed
    - when the processes for doNewPageProcess() from the UI is called, check for processes first and if none, do auto fields
    -- all processes below can be combined into a single function and performed after a processed flag is set, maybe...
            uiProcess.sendPromptData(pageNum);
            uiProcess.sendMenuData(pageNum);
            uiProcess.sendFieldData(pageNum);
            uiProcess.getNextData(pageNum);

*/


int main(int argc, char *argv[])
{
    // directive type (for Qt parses) brings up a menu of projects which corresponds to the root directory

    UIProcess UI;

    UI.ProcessStart();

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    qmlRegisterType<UIProcess>("UIProcess", 1, 0, "UIProcess");

    QQmlApplicationEngine engine;

    if (end_parse == true) {
        engine.load(QUrl(QStringLiteral("qrc:/main_error.qml")));
    }
    else {
        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    }

    return app.exec();
}
