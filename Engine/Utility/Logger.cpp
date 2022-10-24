#include "pch.h"
#include "Logger.h"

Logger::Logger(const string& file, const bool logToBothStreams)
    : mStreamLogBoth(logToBothStreams) {
    mStream.open(file);
}

void Logger::Write(const stringstream& stream) {
    if (mStream.is_open()) {
        if (mStream) {
            mStream << stream.str();
        }

        if (mStreamLogBoth) {
            clog << stream.str();
        }
    } else {
        clog << stream.str();
    }
}
