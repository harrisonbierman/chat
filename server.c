#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 65432
#define BUFFER_SIZE 1024

int main() {
	// server_fd = server file descriptor, refers to socket created
	int server_fd, new_socket;		
	// a struct from <netinet/in.h> from <arpa/inet.h>
	struct sockaddr_in address; 
	int opt = 1;
	int addrlen = sizeof(address);
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

	// set socket option to reuse address and port
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		perror("setsockopt is false");
		exit(EXIT_FAILURE);
	}

	// Bind the socket to localhost and PORT
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT); // host to network byte order short
	
	// bind the address to the socket
	// "I want this socket reachable at this address"
	// Cast the less generic sockaddr_in struct to the more 
	// generic sockaddr struct for the bind function
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("Bind failed");
		exit(EXIT_FAILURE);
	}
	
	// Listen for a connection (backlog of 3)
	if (listen(server_fd, 3) < 0) {
		perror("Listen");
		exit(EXIT_FAILURE);
	}
	printf("Server is listening on port %d\n", PORT);

	// Accept on incoming connection
	// the socket description, the address of the socket, and the length of the socket
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
		perror("Accept");
		exit(EXIT_FAILURE);
	}

	// Read data from the client
	int valread = read(new_socket, buffer, BUFFER_SIZE);
	printf("Reveived: %s\n", buffer);

	// Echo the data back to the client
	send(new_socket, buffer, strlen(buffer), 0);
	printf("Echo sent\n");

	//close the connection and socket
	close(new_socket);
	close(server_fd);
	return 0;
}
