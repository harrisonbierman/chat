#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <arpa/inet.h>

#define PORT 65432
#define BUFFER_SIZE 1024
#define MAX_EVENTS 10

void make_socket_non_blocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	// the reason for this is to add O_NONBLOCK to the 
	// existing flags and not to override them
	// since the options use bit position using the 
	// or operator combinds the bits together 
	// ex: 0010 | 1000 = 1010
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
	// server_fd = server file descriptor, refers to socket created
	int server_fd, client_socket;		

	// a struct from <netinet/in.h> from <arpa/inet.h>
	struct sockaddr_in address = {
		// Bind the socket to localhost and PORT
		.sin_family = AF_INET,
		.sin_addr.s_addr = INADDR_ANY,
		.sin_port = htons(PORT), // host to network byte order short
	};

	int opt = 1;
	char buffer[BUFFER_SIZE] = {0};

	// Create a Socket (IPv4, TCP)
	// AF_INET is IPv4 or '2', SOCK_STREAM is TCP, or '1'
	// third agrument '0' is an unspecified default protocol
	// returns a non-negative integer if successful
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}
	// makes functions like accept() nonblocking obvi
	make_socket_non_blocking(server_fd);

	// set socket option to reuse address and port
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		perror("setsockopt is false");
		exit(EXIT_FAILURE);
	}

	// bind the address to the socket
	// "I want this socket reachable at this address"
	// Cast the less generic sockaddr_in struct to the more 
	// generic sockaddr struct for the bind function
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("Server bind failed");
		exit(EXIT_FAILURE);
	}

	// Listen for a connection (backlog of 3)
	if (listen(server_fd, 3) < 0) {
		perror("Listen");
		exit(EXIT_FAILURE);
	}

	printf("Server is listening on port %d\n", PORT);

	int epoll_fd = epoll_create1(0);

	// set up event that watches for incoming traffic
	// with EPOLLIN = check if there is read data
	struct epoll_event server_event = {
		.events = EPOLLIN,
		.data.fd = server_fd,
	};

	// adding the server_fd to the epoll_fd instance
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &server_event);

	struct epoll_event events[MAX_EVENTS];

	// event loop
	while (1) {
		// returns how many events are on the ready list
		// -1 blocks indefinitely until at least one event is ready
		printf("waiting for next event...\n");
		int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		printf("event deployed\n");
		for (int i = 0; i < n; i++) {
			// if server is on the ready list
			if (events[i].data.fd == server_fd) {
				// accept new connection
				// null for now because we dont care about the
				// client address information
				int client_fd = accept(server_fd, NULL, NULL);
				make_socket_non_blocking(client_fd);
				
				// EPOLLIN = check if there is read data
				struct epoll_event client_event = {
					.events = EPOLLIN,
					.data.fd = client_fd,
				};

				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);
				printf("Accepted new client: %d\n", client_fd);
			} else {
				// if it is a client event
				char buf[1024];
				int valread = read(events[i].data.fd, buf, sizeof(buf));
				if (valread <= 0) {
					// client disconnected or error
					close(events[i].data.fd);
					printf("Closed client: %d\n", events[i].data.fd);
				} else {
					// Echo back
					write(events[i].data.fd, buf, valread);
				}
			}
		}
	}

	return(0);
}
