cmake_minimum_required(VERSION 3.14)

project(miniMUD)

if(WIN32)
	add_compile_options("/std:c++latest")
	add_compile_options("/D _WIN32_WINNT=0x0601")
endif(WIN32)
set(CMAKE_CXX_STANDARD 17)

find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(cryptopp CONFIG REQUIRED)

enable_testing()
find_package(Gtest CONFIG REQUIRED)

add_library(proto
	${CMAKE_CURRENT_SOURCE_DIR}/proto/mud_lib.pb.cc
	${CMAKE_CURRENT_SOURCE_DIR}/proto/mud_lib.pb.h
	${CMAKE_CURRENT_SOURCE_DIR}/proto/mud_lib.grpc.pb.cc
	${CMAKE_CURRENT_SOURCE_DIR}/proto/mud_lib.grpc.pb.h
)

target_link_libraries(proto
  PRIVATE
	protobuf::libprotobuf
	gRPC::gpr
	gRPC::grpc
	gRPC::grpc++
	cryptopp-static
)

add_library(mud_lib
	${CMAKE_CURRENT_SOURCE_DIR}/mud_lib/keyboard.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/mud_lib/keyboard.h
	${CMAKE_CURRENT_SOURCE_DIR}/mud_lib/helper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/mud_lib/helper.h
	${CMAKE_CURRENT_SOURCE_DIR}/mud_lib/hash.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/mud_lib/hash.h
)

target_link_libraries(mud_lib
  PUBLIC
	proto
  PRIVATE
	protobuf::libprotobuf
	gRPC::gpr
	gRPC::grpc
	gRPC::grpc++
	cryptopp-static
)

add_executable(server
	${CMAKE_CURRENT_SOURCE_DIR}/server/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/server/game.h
	${CMAKE_CURRENT_SOURCE_DIR}/server/game.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/server/character.h
	${CMAKE_CURRENT_SOURCE_DIR}/server/character.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/server/enemy.h
	${CMAKE_CURRENT_SOURCE_DIR}/server/enemy.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/server/session.h
	${CMAKE_CURRENT_SOURCE_DIR}/server/session.cpp
)

target_link_libraries(server
  PUBLIC
	proto
	mud_lib
  PRIVATE 
	protobuf::libprotobuf
	gRPC::gpr
	gRPC::grpc
	gRPC::grpc++
	cryptopp-static
)

add_executable(client
	${CMAKE_CURRENT_SOURCE_DIR}/client/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/client/client.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/client/client.h
)

target_link_libraries(client
  PUBLIC
	proto
	mud_lib
  PRIVATE 
	protobuf::libprotobuf
	gRPC::gpr
	gRPC::grpc
	gRPC::grpc++
	cryptopp-static
)