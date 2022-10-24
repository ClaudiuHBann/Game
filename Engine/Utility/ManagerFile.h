#pragma once

class ManagerFile {
public:
    static bool Exists(const string& file) {
        return exists(file);
    }
};