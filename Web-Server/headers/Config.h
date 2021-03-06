#ifndef Config_H
#define Config_H

#include <map>
#include <string>
#include <set>
#include <regex>

class Config {
public:
    const static std::string PUBLIC_DIRECTORY_KEY;
    const static std::string PORT_KEY;
    const static std::string CONNECTION_TIME_OUT;
    const static std::string HIGH_TRAFFIC_CONNECTION_TIME_OUT;


    ~Config();
    static Config* getInstance();
    void init(int argv, const char **argc);

    std::string get(std::string);

private:
    std::map<std::string, std::string>key_to_value;

    static Config *instance;

    const static std::string DEFAULT_SETTINGS_FILE;

    const static std::regex SETTING_REGEX;

    const static char SETTING_SEPARATOR;

    const static std::string DEFAULT_PUBLIC_DIRECTORY;
    const static std::string DEFAULT_PORT;
    const static std::string DEFAULT_CONNECTION_TIME_OUT;
    const static std::string DEFAULT_HIGH_TRAFFIC_CONNECTION_TIME_OUT;

    Config();
    void readFileProperties();
    void readCommandLineProperties(int, const char **);
    bool validKey(std::string);
};
#endif // ifndef Config_H