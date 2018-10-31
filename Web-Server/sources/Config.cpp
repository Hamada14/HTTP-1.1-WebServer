#include "../headers/Config.h"

#include "../headers/Util.h"

#include <fstream>
#include <iostream>
#include <cassert>
#include <stdlib.h>

Config *Config::instance = NULL;

const std::string Config::DEFAULT_SETTINGS_FILE = "config";

const std::regex Config::SETTING_REGEX("[a-zA-Z][a-zA-Z0-9\\-_]+\\s*\\=.+");

const char Config::SETTING_SEPARATOR = '=';

const std::string Config::PUBLIC_DIRECTORY_KEY = "public_directory";
const std::string Config::PORT_KEY = "port";

const std::string Config::DEFAULT_PUBLIC_DIRECTORY = std::string(getenv("HOME")) + "/web-server";
const std::string Config::DEFAULT_PORT = "8080";

Config::Config() {
    this->key_to_value = {
            {PUBLIC_DIRECTORY_KEY,              DEFAULT_PUBLIC_DIRECTORY           },
            {PORT_KEY,                          DEFAULT_PORT                       }
    };
}

Config::~Config() {}

Config * Config::getInstance() {
    if (instance != NULL) {
        return instance;
    }
    return instance = new Config();
}

void Config::init(int argc, const char **argv) {
    readFileProperties();
    readCommandLineProperties(argc, argv);
}

void Config::readFileProperties() {
    std::ifstream settings_file;
    settings_file.open(DEFAULT_SETTINGS_FILE);
    std::string current_line;
    int i = 0;
    while (getline(settings_file, current_line)) {
        i++;
        if (!regex_match(current_line, SETTING_REGEX)) {
            std::cerr << "Invalid settings file line at {" << i <<
                      "}th line... \nRemaining parameters are ignored." << std::endl;
            exit(-1);
        }
        int sep_pos = current_line.find(SETTING_SEPARATOR);
        std::string key = Util::trim(current_line.substr(0, sep_pos));
        std::string val = Util::trim(current_line.substr(sep_pos + 1));
        if (!validKey(key)) {
            std::cerr << "Invalid Parameter key {" << current_line << "}" << std::endl;
            exit(-1);
        }
        key_to_value[key] = val;
    }
    settings_file.close();
}

void Config::readCommandLineProperties(int argc, const char **argv) {
    if (argc % 2 != 1) {
        std::cerr << "Invalid command line parameters...Parameters'll be ignored" << std::endl;
        exit(-1);
    }
    for (int i = 1; i < argc; i += 2) {
        std::string str(argv[i]);
        if ((str.length() > 1) && (str[0] == '-') && validKey(str.substr(0))) {
            key_to_value[str.substr(0)] = std::string(argv[i + 1]);
        } else {
            std::cerr << "Invalid Parameter key {" << str << "}" << std::endl;
            exit(-1);
        }
    }
}

bool Config::validKey(std::string key) {
    return key_to_value.find(key) != key_to_value.end();
}

std::string Config::get(std::string key) {
    assert(key_to_value.count(key));
    return key_to_value[key];
}