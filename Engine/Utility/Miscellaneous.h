#pragma once

#include "Logger.h"

#define GET_LOCATION string(string("file \"") + __FILE__ + "\" in function \"" + __func__ + "\" at line \"" + to_string(__LINE__) + '"')
#define GET_PARAMETER(param) string(string(typeid(param).name()) + " \"" + #param + "\" parameter from " + GET_LOCATION)

#ifdef _DEBUG
static Logger logger {};
#else
static Logger logger("Test.log");
#endif // _DEBUG

#define LOG(message, type) logger.Log(message, Logger::Type(type))
#define LOG_TYPE_WARNING Logger::Type::WARNING
#define LOG_TYPE_ERROR Logger::Type::ERROR

#define LOG_AND_RETURN_IF(pred, message, type, x) \
if((pred)) { \
	LOG(message, type); \
	return x; \
}
#define LOG_AND_RETURN_IF_PARAM_IS_NULL(param, x) LOG_AND_RETURN_IF(!param, GET_PARAMETER(param) + " is null!", LOG_TYPE_WARNING, x)
#define LOG_AND_RETURN_IF_NOT_INIT(pred, subsystems, x) LOG_AND_RETURN_IF(!pred, "The subsystem(s) \"" + string(subsystems) + "\" is(are) not initialized!", LOG_TYPE_ERROR, x)
