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

const int BUFFER_SIZE = 512;

// The arguments passed in the command line, NOT USED!
int serverPort;
char *serverIP;

using namespace std;

struct Command {
    string method, fileName, hostName, portNumber;
};

Command* parseCommand(string command){
    Command* ret = NULL;
    const string commandPattern = "(GET|POST)\\s(\\S+)\\s(\\S+)(\\s\\((\\d+)\\))?";
    regex commandRegex = regex(commandPattern);
    smatch baseMatch;
    if(regex_match(command, baseMatch, commandRegex)){
        ret = (Command *) malloc(sizeof(struct Command));
        ret->method = baseMatch[1];
        ret->fileName = baseMatch[2];
        ret->hostName = baseMatch[3];
        ret->portNumber = baseMatch[5] == "" ? "80" : baseMatch[5].str();
    }
    return ret;
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
	exit(0);
}

int makeSocket(string hostName, string portNumber){
	int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) error("ERROR opening socket.");

    server = gethostbyname(hostName.c_str());
    if(server == NULL) error("ERROR, no such host.");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(atoi(portNumber.c_str()));
    if(connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting.");
  
    bzero(buffer, BUFFER_SIZE);
    return sockfd;
}

void executeCommand(string method, string fileName, string hostName, string portNumber){
	int sockfd = makeSocket(hostName, portNumber);
        
    if(method == "POST"){
    	string fileString = readFile(fileName);
    	string header = "POST " + fileName + " HTTP/1.0\r\n" +
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
    }
    else{
    	string header = "GET " + fileName + " HTTP/1.0\r\n" +
    		"Host: 127.0.0.1\r\n" +
    		"User-Agent: Mozilla/5.0 (en-us)\r\n\r\n";
    	int sendReturnValue = send(sockfd, header.c_str(), header.size(), 0);
	    if(sendReturnValue < 0){
	    	puts("GET: header send failed.");
	    	return;
	    }
	    printf("GET: header sent successfully, return code = %d\n", sendReturnValue);

    	int nDataLength;
	   	char buffer[BUFFER_SIZE];
	    while ((nDataLength = recv(sockfd, buffer, BUFFER_SIZE, 0)) > 0)
	    	buffer[nDataLength] = '\0', printf("%s\n", buffer);
    }
    close(sockfd);
}

int main(int argc, char *argv[]){
	if(argc < 2 or argc > 3){
    	cerr <<"Invalid command, correct syntax: " + string(argv[0]) + " server_ip port_number" <<endl;
    	cerr <<"Notice that the port_number is optional with 80 as default.";
		return 0;
	}

	serverIP = argv[1];
	serverPort = argc == 3 ? atoi(argv[2]) : 80;

    auto commands = readFileLines("commands.txt");
    for(string c : commands){
    	Command *command = parseCommand(c);
    	if(command) executeCommand(command->method, command->fileName, command->hostName, command->portNumber);
    	else cerr <<"Invalid command: " + c <<endl;
    }
}