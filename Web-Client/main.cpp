#include <iostream>
#include <regex>
#include <string>
#include <fstream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <sys/socket.h>
#include <algorithm>

using namespace std;

const int BUFFER_SIZE = 512;
const int DEFAULT_PORT = 80;
const int RECEIVE_TIMEOUT = 2; // in seconds
const string COMMANDS_FILE = "commands.txt";

// The arguments passed in the command line, NOT USED!
int serverPort;
char *serverIP;

bool parseCommand(string command, string &method, string &fileName, string &hostName, string &portNumber){
    const string commandPattern = "(GET|POST)\\s(\\S+)\\s(\\S+)(\\s\\((\\d+)\\))?";
    regex commandRegex = regex(commandPattern);
    smatch baseMatch;
    if(regex_match(command, baseMatch, commandRegex)){
        method = baseMatch[1];
        fileName = baseMatch[2];
        hostName = baseMatch[3];
        portNumber = baseMatch[5] == "" ? "80" : baseMatch[5].str();
        return true;
    }
    return false;
}

string readFile(string file_name){
    ifstream fin(file_name);
    string ret, line;
    size_t buffer_size = 1024;
    char buffer[buffer_size];
    size_t len = 0;
    std::ostringstream oss;
    while((len = fin.readsome(buffer, buffer_size)) > 0)
    {
        oss.write(buffer, len);
    }
    while(getline(fin, line)) ret += line;
    return oss.str();
}

vector<string> readFileLines(string file){
    string line;
    vector<string> ret;
    ifstream fin(file);
    while(getline(fin, line)) ret.push_back(line);
    return ret;
}

void error(string msg){
    cerr <<msg <<endl;
    exit(-1);
}

int makeSocket(string hostName, string portNumber){
    int sockfd;
    struct sockaddr_in serv_addr;

    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) error("ERROR opening socket.");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_port = htons(atoi(portNumber.c_str()));
    serv_addr.sin_family = AF_INET;

    if(connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting.");

    bzero(buffer, BUFFER_SIZE);
    return sockfd;
}


std::string read_buffer;
std::regex REQUEST_TERMINATOR_REGEX("\\r\\n\\r\\n");

std::string readBytes(int socket_descriptor, int length) {
    clock_t last_received_time = clock();
    while(read_buffer.length() < length) {
        char buffer[BUFFER_SIZE] = {0};
        size_t data_length = read(socket_descriptor, buffer, BUFFER_SIZE);
        std::string new_input(buffer, buffer + data_length);
        read_buffer += new_input;
        clock_t current_time = clock();
        if(data_length != 0) {
            last_received_time = current_time;
        } else if(double(current_time - last_received_time) / CLOCKS_PER_SEC > RECEIVE_TIMEOUT) {
            printf("Timed out waiting for response");
            return "";
        }
    }
    std::string result = read_buffer.substr(0, length);
    read_buffer = read_buffer.substr(length, read_buffer.length() - length);
    return result;
}


std::string readNextResponse(int socket_descriptor) {
    std::smatch matches;
    std::regex_search(read_buffer, matches, REQUEST_TERMINATOR_REGEX);
    clock_t last_received_time = clock();
    while(matches.size() == 0) {
        char buffer[BUFFER_SIZE] = {0};
        size_t data_length = read(socket_descriptor, buffer, BUFFER_SIZE);
        std::string new_input(buffer, buffer + data_length);
        read_buffer += new_input;
        std::regex_search(read_buffer, matches, REQUEST_TERMINATOR_REGEX);
        clock_t current_time = clock();
        if(data_length != 0) {
            last_received_time = current_time;
        } else if(double(current_time - last_received_time) / CLOCKS_PER_SEC > RECEIVE_TIMEOUT) {
            printf("Timed out waiting for response");
            return "";
        }
    }
    int break_point = matches[0].length() + matches.position(0);
    std::string current_request = read_buffer.substr(0, break_point);
    read_buffer = read_buffer.substr(break_point, read_buffer.length() - break_point + 1);
    return current_request;
}

int extract_content_length(std::string response) {
    std::smatch matches;
    std::regex content_length_regex("\\r\\nContent-Length:\\s*([0-9]+)\\r\\n");
    std::regex_search(response, matches, content_length_regex);
    return matches.length() >= 2 ? std::stoi(matches[1]) : 0;
}

void write_data_to_file(std::string content, std::string file_name) {
    std::ofstream f_stream(file_name.c_str());
    f_stream.write(content.c_str(), content.length());
}

void executeCommand(string method, string fileName, string hostName, string portNumber){
    int sockfd = makeSocket(hostName, portNumber);

    if(method == "POST"){
        fileName = fileName.substr(1); // Removing the first forward slash
        string fileString = readFile(fileName);
        string header = "POST /" + fileName + " HTTP/1.1\r\n" +
                        "Host: 127.0.0.1\r\n" +
                        "Content-Type: application/octet-stream\r\n" +
                        "Content-Length: " + to_string(fileString.size()) + "\r\n" +
                        "Connection: close\r\n\r\n";
        int sendReturnValue = send(sockfd, header.c_str(), header.size(), 0);
        if(sendReturnValue < 0){
            puts("POST: header send failed.");
            return;
        }
        printf("POST: header sent successfully, return code = %d\n", sendReturnValue);
        std::string response = readNextResponse(sockfd);
        std::cout << response << std::endl;
        if(response.substr(9, 3) == "200") {
            sendReturnValue = send(sockfd, fileString.c_str(), fileString.size(), 0);
        } else {
            printf("Not allowed to Post file to the server\n");
        }
        if(sendReturnValue < 0) {
            puts("POST: file send failed.");
            return;
        }
        printf("POST: file sent successfully, return code = %d\n", sendReturnValue);
        std::cout <<  "-----------------------------------------------------------------" << std::endl;
    } else {
        string header = "GET " + fileName + " HTTP/1.1\r\n" +
                        "Host: 127.0.0.1\r\n\r\n";

        int sendReturnValue = send(sockfd, header.c_str(), header.size(), 0);
        if(sendReturnValue < 0){
            puts("GET: header send failed.");
            return;
        }
        printf("GET: header sent successfully, return code = %d\n", sendReturnValue);

        std::string response = readNextResponse(sockfd);
        if(response.length() == 0) {
            printf("Error receiving response due to timeout\n");
            return;
        }
        int content_length = extract_content_length(response);
        std::string file_content = readBytes(sockfd, content_length);
        std::cout << response << std::endl;
        std::cout << file_content << std::endl;
        std::cout << "-----------------------------------------------------------------" << std::endl;
        write_data_to_file(file_content, fileName.substr(1));
    }
    close(sockfd);
}

int main(int argc, char *argv[]){
    if(argc < 2 or argc > 3){
        cerr <<"Invalid command, correct syntax: " + string(argv[0]) + " server_ip port_number" <<endl;
        cerr <<"Notice that the port_number is optional with " <<DEFAULT_PORT <<" as default.";
        return 0;
    }

    serverIP = argv[1];
    serverPort = argc == 3 ? atoi(argv[2]) : DEFAULT_PORT;

    auto commands = readFileLines(COMMANDS_FILE);
    for(string c : commands){
        string method, fileName, hostName, portNumber;
        if(parseCommand(c, method, fileName, hostName, portNumber)){
        	cout <<"Executing command: " <<c <<endl;
        	executeCommand(method, fileName, hostName, portNumber);
        }
        else cerr <<"Invalid command: " + c <<endl;
    }
}