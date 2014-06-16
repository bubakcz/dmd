#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <string>
#include "Include/AMD/GPUPerfAPI.h"
namespace logger{

#ifndef LOGGER_HEADER_PROTECTOR
#define LOGGER_HEADER_PROTECTOR

#define DEBUG 1
#define WARNING 2
#define ERR 3

	void set_logger_target(void * demomanager_instance);

	void GPA_logging_function( GPA_Logging_Type messageType, const char* message );

	void debug(const char * message);

	void debug(std::string message);

	void debug(QString message);

	void warning(const char * message);

	void warning(std::string message);

	void warning(QString message);

	void error(const char * message);

	void error(std::string message);

	void error(QString message);

	extern int level;

}

#endif
#endif

