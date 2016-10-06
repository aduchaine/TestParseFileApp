#ifndef LOGGING_H
#define LOGGING_H


#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS


static const char * root_foldername = "TestParseFileApp";


namespace Logs {

	enum LogLevel {
		Normal = 1,
		Med,
		High
	};

	enum Category {
		None = 0,
		Error,
        UIError,
		General,
        DataCode,
        UILog,
        UIStartup,
        ParseDir,
        ParseFunc,
		Test,
		MaxCategoryID
	};

	static const char* CategoryName[Category::MaxCategoryID] = {
		"",
		"ERROR",
        "UIERROR",
		"General",
        "DataCode",
        "UILog",
        "UIStartup",
        "ParseDir",
        "ParseFunc",
        "TEST",
	};

	enum LogDir {
		NoLDir = 0,
		LOG_DIRS,
		ERROR_DIR,
		DEBUG_DIR,
        UI_LOG_DIR,
		MaxLogDirs
	};

	static const char* LogDirectories[LogDir::MaxLogDirs] = {
		"",
		"\\Logs\\",
		"\\Logs\\Error\\",
		"\\Logs\\Debug\\",
        "\\Logs\\UILog\\",
	};

	enum LogFiles {
		NoLFile = 0,
		ERROR_LOGS,
		DEBUG_LOGS,
        UI_LOGS,
		MaxLogFilenames
	};

	static const char* LogFileName[LogFiles::MaxLogFilenames] = {
		"",
		"error_log",
		"debug_log",
        "ui_log",
	};
}


class Logging
{
public:	
	
	struct LogSettings {
		int log_to_file;
		int log_to_console;
		int is_category_enabled;
	};

	LogSettings log_settings[Logs::Category::MaxCategoryID];

	Logging();

	void DefaultLogSettings();
	void CheckLogFiles();	
	void CheckFileTime(std::string dest_dir, std::string dest_file, bool write);
	void LogRecord(Logs::LogLevel log_level, int log_category, std::string message, ...);

    std::string DbgSwtch(int debug_switch);
	std::string FormatLogPath(std::string dir_stop);
	void ConsoleMessage(int log_level, int log_category, const std::string & message);
	void LogWrite(int log_level, int log_category, const std::string & message);
	std::string MessageFormat(int log_category, const std::string & in_message);
	const std::string StringFormat(const char * format, ...);
	const std::string vStringFormat(const char * format, va_list args);
	void TimeStamp(char * time_stamp, bool filename);

	void LogDirectory(const std::string & directory_name);
	bool LogFile(std::string tar_directory, int open_status);

};

extern Logging Log;

extern std::string root_directory;

#endif // LOGGING_H
