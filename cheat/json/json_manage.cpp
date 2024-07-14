#include <stdexcept>
#include <iostream>
#include <fstream>
#include "../includes.h"

std::string GetExecutablePath() {
    char buffer[MAX_PATH];
    DWORD size = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    if (size == 0) {
        throw std::runtime_error("Failed to get executable path");
    }
    return std::string(buffer, size);
}

std::string GetExecutableDir(const std::string& executablePath) {
    size_t pos = executablePath.find_last_of("\\/");
    if (pos == std::string::npos) {
        return ""; // No directory part found
    }
    return executablePath.substr(0, pos);
}

std::string GetCurrentJsonPath() {
    std::string executablePath = GetExecutablePath();
    std::string executableDir = GetExecutableDir(executablePath);
    std::string jsonFilePath = executableDir + "\\data.json";
    return jsonFilePath;
}

std::string GetValueFromJSON(const std::string& key, const std::string& defaultValue) {
    std::string jsonFilePath = GetCurrentJsonPath();
    std::ifstream jsonFile(jsonFilePath);
    if (!jsonFile.is_open()) {
        std::cerr << "Could not open the JSON file located at: " << jsonFilePath << std::endl;
        return defaultValue;
    }

    json jsonData;
    jsonFile >> jsonData;
    jsonFile.close();

    if (jsonData.contains(key)) {
        return jsonData[key].get<std::string>();
    }
    else {
        return defaultValue;
    }
}

bool GetBooleanFromJSON(const std::string& key, bool defaultValue) {
    std::string jsonFilePath = GetCurrentJsonPath();
    std::ifstream jsonFile(jsonFilePath);
    if (!jsonFile.is_open()) {
        std::cerr << "Could not open the JSON file: " << jsonFilePath << std::endl;
        return defaultValue;
    }

    json jsonData;
    jsonFile >> jsonData;
    jsonFile.close();

    if (jsonData.contains(key)) {
        return jsonData[key].get<bool>();
    }
    else {
        return defaultValue;
    }
}
