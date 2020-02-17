#include <iostream>
#include <fstream>
#include "game.h"
#include "session.h"
#include <grpcpp/server_builder.h>
#include <grpcpp/security/credentials.h>

int main(int ac, char** av)
{
	std::cout << "starting server..." << std::endl;
	auto pg = std::make_shared<server::game>();
	auto m = std::make_shared<std::mutex>();
	std::string server_address{ "0.0.0.0:4242" };
	server::session session{ m, pg };
	grpc::ServerBuilder builder{};
	builder.AddListeningPort(
		server_address, 
		grpc::InsecureServerCredentials());
	builder.RegisterService(&session);
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
	auto total_time_ = std::chrono::milliseconds(100);
	auto t = std::thread([&server] { server->Wait(); });
	while (true)
	{
		auto start_time = std::chrono::high_resolution_clock::now();
		m->lock();
		pg->run_once();
		m->unlock();
		auto end_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = end_time - start_time;
		if (total_time_ > duration)
		{
			std::this_thread::sleep_for(total_time_ - duration);
		}
		else
		{
			std::cerr << "too long..." << std::endl;
		}
	}
	return 0;
}