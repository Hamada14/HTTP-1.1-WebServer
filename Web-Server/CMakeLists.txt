cmake_minimum_required(VERSION 3.1)
project(Web_Server)

set(CMAKE_CXX_STANDARD 14)

find_package( Threads REQUIRED )
add_executable(Web_Server main.cpp sources/ThreadPool.cpp headers/ThreadPool.h sources/Server.cpp headers/Server.h sources/Socket.cpp headers/Socket.h sources/ServerWorker.cpp headers/ServerWorker.h sources/Request.cpp headers/Request.h sources/GetRequest.cpp headers/GetRequest.h sources/PostRequest.cpp headers/PostRequest.h sources/MIMETypesLoader.cpp headers/MIMETypesLoader.h sources/Config.cpp headers/Config.h sources/Util.cpp headers/Util.h)

target_link_libraries (Web_Server Threads::Threads)
