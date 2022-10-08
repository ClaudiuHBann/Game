#include "pch.h"
#include "Utility/Logger.h"

int main() {
#ifdef _DEBUG
	Logger logger;
#else
	Logger logger("C:\\Users\\Claudiu HBann\\Desktop\\Logger.log");
#endif // _DEBUG


	logger.Log("Logger", Logger::Type::NONE);
	logger.Log("Logger", Logger::Type::NONE, Logger::Color::FG_GREEN);
	logger.Log("Logger", Logger::Type::INFO);
	logger.Log("Logger", Logger::Type::WARNING);
	logger.Log("Logger", Logger::Type::ERROR);
	logger.Log("Logger", Logger::Type::ERROR, Logger::Color::BG_MAGENTA);

	return 0;
}