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
#define MAX_CLIENTS 2
#define MAX_EVENTS 10

typedef struct Package {
	int client_id;
	char message[1024];
} Package;

void serialize_package(uint8_t *buf, struct Package *pkg) {
	int offset = 0;

	memcpy(buf + offset, &pkg->client_id, sizeof(pkg->client_id));
	offset += sizeof(pkg->client_id);

	memcpy(buf + offset, &pkg->message, sizeof(pkg->message));
}

void deserialize_package(struct Package *pkg, uint8_t *buf) {
	int offset = 0;

	memcpy(&pkg->client_id, buf + offset, sizeof(pkg->client_id));
	offset += sizeof(pkg->client_id);

	memcpy(&pkg->message, buf + offset, sizeof(pkg->message));
}

void add_item(int *arr, int item, int length) {
	for(int i = 0; i < length; i++) {
		if (arr[i] == 0) {
			arr[i] = item;
			break;
		} 
	}
}

void remove_item(int *arr, int item, int length) {
	for(int i = 0; i < length; i++) {
		if(arr[i] == item) {
			arr[i] = 0;
			break;
		}
	}
}

void print_array(int *arr, int length) {
	printf("{");
	for (int i = 0; i < length; i++) {
		printf("%d", *(arr + i));
		if (i < length - 1) printf(", ");
	}
	printf("}\n");
}


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
	printf("Maximum clients allowed: %d\n", MAX_CLIENTS);

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

	int registered_clients[MAX_CLIENTS] = {0};

	// event loop
	while (1) {
		// returns how many events are on the ready list
		// -1 blocks indefinitely until at least one event is ready
		printf("waiting for next event...\n\n");
		int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		printf("event triggered\n");
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
					
				// kick client if max clients is reached
				int is_full = 1;
				for(int i = 0; i < MAX_CLIENTS; i++) {
					if (registered_clients[i] == 0) {
						is_full = 0;
						break;
					}
				}


				// need to keep track of clients manually
				add_item(registered_clients, client_fd, MAX_CLIENTS);
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);
				printf("Accepted new client: %d\n", client_fd);
				printf("Clients Connected: ");
				print_array(registered_clients, MAX_CLIENTS);

				// maybe tell the client why
				if(is_full) {
					close(client_fd);
					printf("Client \"%d\" kicked from server: server full\n", client_fd);
				}

				// send client fd number as their ID
				send(client_fd, &client_fd, sizeof(client_fd), 0);
			} else {
				// if it is a client event
				Package pkg;
				uint8_t ser_pkg[sizeof(struct Package)];
				int count = recv(events[i].data.fd, ser_pkg, sizeof(ser_pkg), 0);
				if (count <= 0) {
					// client disconnected or error
					close(events[i].data.fd);
					printf("Closed client: %d\n", events[i].data.fd);
					remove_item(registered_clients, events[i].data.fd, MAX_CLIENTS);
					printf("Clients Connected: ");
					print_array(registered_clients, MAX_CLIENTS);
				} else {
					// Echo back
					deserialize_package(&pkg, ser_pkg);
					printf("package recieved from client: %d\n", pkg.client_id);
					printf("with message: %s\n", pkg.message);
					for(int i = 0; i < MAX_CLIENTS; i++) {
						if(registered_clients[i] != 0) {
							write(registered_clients[i], ser_pkg, count);
						}
					}
				}
			}
		}
	}

	return(0);
}
