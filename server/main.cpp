#include <iostream>
#include <fstream>
#include "../mud_lib/keyboard.h"
#include "game.h"
#include "session.h"
#include <grpcpp/server_builder.h>
#include <grpcpp/security/credentials.h>

int main(int ac, char** av)
{
	std::cout << "starting server..." << std::endl;
	auto pg = std::make_shared<server::game>();
	// pg.run();
	std::string server_address("0.0.0.0:4242");
	server::session session{pg};
	grpc::ServerBuilder builder{};
	builder.AddListeningPort(
		server_address, 
		grpc::InsecureServerCredentials());
	builder.RegisterService(&session);
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
	server->Wait();
	return 0;
}