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

using namespace std;

const int BUFFER_SIZE = 512;
const int DEFAULT_PORT = 80;
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

string readFile(string file){
    ifstream fin(file);
    string ret, line;
    while(getline(fin, line)) ret += line;
    return ret;
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
    // struct hostent *server;

    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) error("ERROR opening socket.");

    // server = gethostbyname(hostName.c_str());
    // if(server == NULL) error("ERROR, no such host.");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_port = htons(atoi(portNumber.c_str()));
    serv_addr.sin_family = AF_INET;

    // bcopy((char *)server->h_addr,
       //   (char *)&serv_addr.sin_addr.s_addr,
         // server->h_length);
    if(connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting.");

    bzero(buffer, BUFFER_SIZE);
    return sockfd;
}

void executeCommand(string method, string fileName, string hostName, string portNumber){
    int sockfd = makeSocket(hostName, portNumber);

    if(method == "POST"){
        string fileString = readFile(fileName);
        string header = "POST " + fileName + " HTTP/1.1\r\n" +
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

        sendReturnValue = send(sockfd, fileString.c_str(), fileString.size(), 0);
        if(sendReturnValue < 0){
            puts("POST: file send failed.");
            return;
        }
        printf("POST: file sent successfully, return code = %d\n", sendReturnValue);
    } else {
        string header = "GET " + fileName + " HTTP/1.1\r\n" +
                        "Host: 127.0.0.1\r\n\r\n";
        // cout << header << endl;
        int sendReturnValue = send(sockfd, header.c_str(), header.size(), 0);
        if(sendReturnValue < 0){
            puts("GET: header send failed.");
            return;
        }
        printf("GET: header sent successfully, return code = %d\n", sendReturnValue);

        int dataLength;
        char buffer[BUFFER_SIZE];
        bool continue_work = true;
        string terminator = "\r\n\r\n";
        int terminatorLen = terminator.size();
        while(continue_work) {
            dataLength = read(sockfd, buffer, BUFFER_SIZE);
            buffer[dataLength] = '\0';
            printf("%s\n", buffer);

            for(int i = 0; i < dataLength - terminatorLen; i++) {
                bool done = false;
                for(int j = 0; j < terminatorLen; j++) {
                    if(terminator[j] != buffer[i + j]) break;
                    if(j == terminatorLen - 1) {
                        done = true;
                    }
                }
                continue_work = !done;
            }
        }
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