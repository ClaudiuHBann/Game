#pragma once

class ManagerFile {
public:
	static inline bool Exists(const string& file) {
		return exists(file);
	}
};