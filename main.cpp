#include <cstring>
#ifdef BENCH
#include <chrono>
#endif

#ifdef FMT
#include <fmt/core.h>
#endif

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "HTTP/HTTP.hpp"

#define HTTPPORT 80

int main()
{
	int HTTP_sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(HTTP_sock_fd < 0)
	{
		#ifdef FMT
		fmt::print(stderr, "Socket opening error.\n");
		#endif
		return -1;
	}

	struct sockaddr_in server_addr;
	bzero(reinterpret_cast<char*>(&server_addr), sizeof(server_addr));

	server_addr.sin_family 		= AF_INET;
	server_addr.sin_port 		= htons(80);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(HTTP_sock_fd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) < 0)
	{
		#ifdef FMT
		fmt::print(stderr, "Binding error.\n");
		#endif
		return -1;
	}

	listen(HTTP_sock_fd, 10);

	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);

	char buffer[1024];
	HTTP::Request req;

	while(1)
	{
		int incoming_socket = accept(HTTP_sock_fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len);

		if(incoming_socket < 0)
		{
			#ifdef FMT
			fmt::print(stderr, "Accepting error.\n");
			#endif
			return -1;
		}

		#ifdef BENCH
		auto t1 = std::chrono::high_resolution_clock::now();
		#endif

		bzero(buffer, sizeof(buffer));
		
		int bytes = read(incoming_socket, buffer, sizeof(buffer) - 1);
		if(bytes < 0)
		{
			#ifdef FMT
			fmt::print(stderr, "Reading error.\n");
			#endif
			return -1;
		}

		//fmt::print("{}\n", req.getRequestText());

		req.parse(buffer);
		int out_port = 3000;
		if(req.getRoute().find("search/") != std::string::npos &&
		  (req.getRequestType() == HTTP::REQTYPE::POST ||
		   req.getRequestType() == HTTP::REQTYPE::PUT))
		{
			out_port = 7700;
		}
		else if(((req.getRoute().find("api/") != std::string::npos) ||
				 (req.getRoute().find("graphql") != std::string::npos)||
				 (req.getRoute().find("uploads/")) != std::string::npos) &&
				 (req.getRequestType() == HTTP::REQTYPE::GET || req.getRequestType() == HTTP::REQTYPE::POST))
		{
			out_port = 1337;
		}
		else if(req.getRequestType() == HTTP::REQTYPE::GET)
		{
			out_port = 3000;
		}

		int outgoing_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(outgoing_socket < 0)
		{
			#ifdef FMT
			fmt::print(stderr, "Outgoing socket opening error.\n");
			#endif
			return -1;
		}
	
		struct sockaddr_in outgoing_addr;
		bzero(reinterpret_cast<char*>(&outgoing_addr), sizeof(outgoing_addr));
			
		outgoing_addr.sin_family = AF_INET;
		outgoing_addr.sin_port	 = htons(out_port);

		if(inet_pton(AF_INET, "127.0.0.1", &outgoing_addr.sin_addr) <= 0)
		{
			#ifdef FMT
			fmt::print(stderr, "IP resolve error.\n");
			#endif
			return -1;
		}

		if(connect(outgoing_socket, reinterpret_cast<struct sockaddr*>(&outgoing_addr), sizeof(outgoing_addr)) < 0)
		{
			#ifdef FMT
			fmt::print(stderr, "Connection error.\n");
			#endif
			return -1;
		}

		std::strcpy(buffer, req.getRequestText().c_str());
		bytes = write(outgoing_socket, buffer, sizeof(buffer) - 1);
		if(bytes < 0)
		{
			#ifdef FMT
			fmt::print(stderr, "Write to outgoing error.\n");
			#endif
		}

		bzero(buffer, sizeof(buffer));
		bytes = read(outgoing_socket, buffer, sizeof(buffer) - 1);
		if(bytes < 0)
		{	
			#ifdef FMT
			fmt::print(stderr, "Read from outgoing error.\n");
			#endif
		}
		close(outgoing_socket);

		bytes = write(incoming_socket, buffer, sizeof(buffer) - 1);
		if(bytes < 0)
		{
			#ifdef FMT
			fmt::print(stderr, "Write to incoming error.\n");
			#endif
		}

		close(incoming_socket);

		#ifdef BENCH
		auto t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> ms_double = t2 - t1;
		fmt::print("Request time: {}ms\n", ms_double.count());
		#endif
	}

	return 0;
}
