#pragma once

class Logger {
public:
    enum class Type : uint8_t {
        NONE,
        INFO,
        WARNING,
        ERROR
    };

    enum class Color : uint8_t {
        NONE,
        FG_BLACK = 30Ui8,
        FG_RED,
        FG_GREEN,
        FG_YELLOW,
        FG_BLUE,
        FG_MAGENTA,
        FG_CYAN,
        FG_WHITE,
        BG_BLACK = 40Ui8,
        BG_RED,
        BG_GREEN,
        BG_YELLOW,
        BG_BLUE,
        BG_MAGENTA,
        BG_CYAN,
        BG_WHITE,
        NONE2
    };

    Logger() = default;

    Logger(const string& file, const bool logToBothStreams = false);

    template<typename T>
    void Log(const T& data, const Type type = Type::NONE, const Color color = Color::NONE) {
        stringstream stream {};
        stream << mReset;

        auto typeAsInt = (uint8_t)type;
        if (typeAsInt < (uint8_t)Type::NONE && typeAsInt >(uint8_t)Type::ERROR) {
            typeAsInt = (uint8_t)Type::NONE;
        }
        const auto& typePair = mTypes[typeAsInt];

        const auto colorAsInt = (int16_t)color;
        if (colorAsInt > (uint8_t)Color::NONE && colorAsInt < (uint8_t)Color::NONE2) {
            stream << mBegin << colorAsInt << mEnd;
        } else if (typeAsInt != (uint8_t)Type::NONE) {
            stream << mBegin << (int16_t)typePair.second << mEnd;
        }

        if (typeAsInt != (uint8_t)Type::NONE) {
            stream << typePair.first << ' ';
        }

        stream << data << mReset << '\n';

        Write(stream);
    }

    template<typename T>
    void operator<<(const T& data) {
        Log(data);
    }

private:
    void Write(const stringstream& stream);

    ofstream mStream {};
    bool mStreamLogBoth = false;

    static inline vector<pair<string, Color>> mTypes {
      { "[NONE]", Color::NONE },
      { "[INFO]", Color::FG_CYAN },
      { "[WARNING]", Color::FG_YELLOW },
      { "[ERROR]", Color::FG_RED }
    };

    static inline string mReset = "\033[0m";
    static inline string mBegin = "\033[";
    static inline string mAnd = ";";
    static inline string mEnd = "m";
};