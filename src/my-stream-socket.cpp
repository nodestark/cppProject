#include <sys/socket.h>
#include <sys/un.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/in.h>
#include <arpa/inet.h>

int main() {

	int fd_socket = socket(AF_INET, SOCK_STREAM, 0);

	std::cout << fd_socket << std::endl;

	struct sockaddr_in address;

	address.sin_family = AF_INET;

	inet_aton("127.0.0.1", &address.sin_addr);
	address.sin_port = htons( 8088 );

	int status_connect= connect( fd_socket, (struct sockaddr *) &address, sizeof(address) );

	std::cout << status_connect << std::endl;

	char recv_buf[128];

	int data_recv = recv( fd_socket, recv_buf, sizeof(recv_buf), 0);
	std::cout.write(recv_buf, data_recv);

	//std::cout << recv_buf << std::endl;

	std::string str= "leonardo";
	send( fd_socket, str.c_str(), strlen( str.c_str() ), 0);

	std::cout << "End!" << std::endl;

	return 0;
}
