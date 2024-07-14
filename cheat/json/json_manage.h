#ifndef MANAGE_JSON_H
#define MANAGE_JSON_H

#include "../includes.h"
#include <string>

using json = nlohmann::json;

std::string GetExecutablePath();
std::string GetExecutableDir(const std::string& executablePath);
std::string GetCurrentJsonPath();
std::string GetValueFromJSON(const std::string& key, const std::string& defaultValue = "");
bool GetBooleanFromJSON(const std::string& key, bool defaultValue = false);


#endif // MANAGE_JSON_H