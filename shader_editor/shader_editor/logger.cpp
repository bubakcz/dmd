#include "logger.h"
#include "shader_editor.h"



namespace logger{

	shader_editor * manager = NULL;

	int level = 0;
	int failures = 0;

	std::vector<std::string> preinit_message_list;
	std::vector<int> preinit_severity_list;

	void set_logger_target(void * demomanager_instance)
	{
		manager = (shader_editor *)demomanager_instance;
		debug("logging initiated");
		debug((QString("total messages before init: ")+QString::number(failures)).toStdString().c_str());

		for (int i = 0; i < preinit_severity_list.size(); i++)
		{
			switch (preinit_severity_list.at(i))
			{
			case DEBUG: debug(preinit_message_list.at(i).c_str()); break;
			case WARNING: debug(preinit_message_list.at(i).c_str()); break;
			case ERR: debug(preinit_message_list.at(i).c_str()); break;
			default: debug((std::string("undefined severity! ") + preinit_message_list.at(i)).c_str()); break;
			}
		}

	};

	void preinit_record(const char * message, int severity)
	{
		preinit_message_list.push_back(message);
		preinit_severity_list.push_back(severity);
	}

	void debug(const char * message)
	{
		if(manager && level <= DEBUG)
			manager->writeToConsoleQt(QtDebugMsg, message);
		else
			preinit_record(message, DEBUG);
	};

	void debug(std::string message) {debug(message.c_str());};

	void debug(QString message) {debug(message.toStdString().c_str());};

	void warning(const char * message)
	{
		if(manager && level <= WARNING)
			manager->writeToConsoleQt(QtWarningMsg, message);
		else
			preinit_record(message, WARNING);
	};

	void warning(std::string message) {warning(message.c_str());};

	void warning(QString message) {warning(message.toStdString().c_str());};

	void error(const char * message)
	{
		if(manager && level <= ERR)
			manager->writeToConsoleQt(QtCriticalMsg, message);
		else
			preinit_record(message, ERR);
	};

	void error(std::string message) {error(message.c_str());};

	void error(QString message) {error(message.toStdString().c_str());};

	void GPA_logging_function( GPA_Logging_Type messageType, const char* message )
	{
		debug(message);
	}

}
