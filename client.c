#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 65432
#define BUFFER_SIZE 1024

int main() {

	// define socket address
	struct sockaddr_in server_address = {
		.sin_family = AF_INET,
		.sin_port = htons(PORT),
	};

	// Create a socket (IPv4, TCP
	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd == -1) {
		perror("socket creating failed\n");
		exit(EXIT_FAILURE);
	}

	// Convert IPv4 address from text to binary form
	// store it into the server_address.sin_addr
	if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
		perror("Invalid address/ Address not supported\n");
		exit(EXIT_FAILURE);
	}

	// Connect to server
	if (connect(client_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
		perror("Connection Failed\n");
		exit(EXIT_FAILURE);
	}

	
	
	while (1) {
		char buf[1024];
		printf("write something to the server: ");
		fgets(buf, sizeof(buf), stdin);
		printf("\033[A");
		printf("\r\033[2K");

		// Send the Message
		send(client_fd, buf, strlen(buf), 0);
		printf("client: %s", buf);

		int valread = read(client_fd, buf, BUFFER_SIZE);
		printf("server: %s", buf);
		
	}
	
	//Close the socket
	close(client_fd);
	return 0;
}
