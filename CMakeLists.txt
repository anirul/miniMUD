cmake_minimum_required(VERSION 3.14)

project(miniMUD)

if(WIN32)
	add_compile_options("/std:c++latest")
	add_compile_options("/D _WIN32_WINNT=0x0601")
endif(WIN32)
set(CMAKE_CXX_STANDARD 17)

find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

add_library(mud_lib
	${CMAKE_CURRENT_SOURCE_DIR}/mud_lib/mud_lib.pb.cc
	${CMAKE_CURRENT_SOURCE_DIR}/mud_lib/mud_lib.pb.h
	${CMAKE_CURRENT_SOURCE_DIR}/mud_lib/helper.cc
	${CMAKE_CURRENT_SOURCE_DIR}/mud_lib/helper.h
)

target_link_libraries(mud_lib
  PRIVATE
	protobuf::libprotobuf
	gRPC::gpr
	gRPC::grpc
	gRPC::grpc++
)

add_executable(server
	${CMAKE_CURRENT_SOURCE_DIR}/server/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/server/game.h
	${CMAKE_CURRENT_SOURCE_DIR}/server/game.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/server/character.h
	${CMAKE_CURRENT_SOURCE_DIR}/server/character.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/server/enemy.h
	${CMAKE_CURRENT_SOURCE_DIR}/server/enemy.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/server/keyboard.h
	${CMAKE_CURRENT_SOURCE_DIR}/server/keyboard.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/server/session.h
	${CMAKE_CURRENT_SOURCE_DIR}/server/session.cpp
)

target_link_libraries(server
  PUBLIC
	mud_lib
  PRIVATE 
	protobuf::libprotobuf
	gRPC::gpr
	gRPC::grpc
	gRPC::grpc++
)