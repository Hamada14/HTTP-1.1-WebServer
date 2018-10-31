cmake_minimum_required(VERSION 3.12)
project(Web_Server)

set(CMAKE_CXX_STANDARD 14)

add_executable(Web_Server main.cpp sources/Server.cpp headers/Server.h sources/Socket.cpp headers/Socket.h sources/ServerWorker.cpp headers/ServerWorker.h sources/Request.cpp headers/Request.h sources/GetRequest.cpp headers/GetRequest.h sources/PostRequest.cpp headers/PostRequest.h sources/MIMETypesLoader.cpp headers/MIMETypesLoader.h sources/Config.cpp headers/Config.h sources/Util.cpp headers/Util.h)