#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 65432
#define BUFFER_SIZE 1024

int main() {
	int sock = 0;
	struct sockaddr_in serv_addr;
	char *message = "Hello, Server!";
	char buffer[BUFFER_SIZE] = {0};

	// Create a socket (IPv4, TCP
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		perror("socket creating failed\n");
		exit(EXIT_FAILURE);
	}
	
	// define the socket address 
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 address from text to binary form
	// store it into the serv_addr.sin_addr
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		perror("Invalid address/ Address not supported\n");
		exit(EXIT_FAILURE);
	}

	// Connect to server
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("Connection Failed\n");
		exit(EXIT_FAILURE);
	}

	
	
	while (1) {
		char user_input[120];
		printf("write something to the server: ");
		fgets(user_input, sizeof(user_input), stdin);
		printf("\033[A");
		printf("\r\033[2K");

		// Send the Message
		send(sock, user_input, strlen(user_input), 0);
		printf("client: %s", user_input);

		int valread = read(sock, buffer, BUFFER_SIZE);
		printf("server: %s", buffer);
		
	}
	
	//Close the socket
	close(sock);
	return 0;
}
