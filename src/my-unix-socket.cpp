#include <sys/socket.h>
#include <sys/un.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

// fcntl(fromAccept, F_SETFL, fcntl(fromAccept, F_GETFL) | O_NONBLOCK);

int main() {

	remove("mysocket.socket"); // drop fd...

	int fd_socket = socket(AF_UNIX, SOCK_STREAM, 0);

	struct sockaddr_un address;

	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, "mysocket.socket");

	bind(fd_socket, (struct sockaddr*) &address, sizeof(address));

	if (listen(fd_socket, 5 /*N connection queue requests*/) != -1)
		while (true) {

			std::cout << "Waiting connection..." << std::endl;

			unsigned int sock_len = 0;
			int fd_accept = accept(fd_socket, (struct sockaddr*) &address, &sock_len);

			fcntl(fd_accept, F_SETFL, fcntl(fd_accept, F_GETFL) | O_NONBLOCK);

			std::cout << "...connected" << std::endl;

			char recv_buf[128];
			int data_recv = 0;

			do {

				data_recv = recv(fd_accept, recv_buf, sizeof(recv_buf), 0); // Wait until receive, returns 0 on close;

				if (strstr(recv_buf, "quit"))
					break;

				if (data_recv > 0)
					std::cout.write(recv_buf, data_recv);

			} while (data_recv != 0);

			close(fd_accept);
		}

	std::cout << "End!" << std::endl;

	return 0;
}
