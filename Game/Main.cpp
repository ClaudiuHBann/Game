#include "pch.h"
#include "Utility/Logger.h"

int main() {
#ifdef _DEBUG
	Logger logger;
#else
	Logger logger("C:\\Users\\Claudiu HBann\\Desktop\\Logger.log");
#endif // _DEBUG

	logger.Log("Start", Logger::Type::INFO);

	return 0;
}