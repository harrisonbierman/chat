#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 65432
#define LOCAL_HOST "127.0.0.1"
#define LAN_IP "192.168.0.131"
#define LINVEO_IP "141.140.12.190"
#define BUFFER_SIZE 1024

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
	if (inet_pton(AF_INET, LAN_IP, &server_address.sin_addr) <= 0) {
		perror("Invalid address/ Address not supported\n");
		exit(EXIT_FAILURE);
	}

	// Connect to server
	if (connect(client_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
		perror("Connection Failed\n");
		exit(EXIT_FAILURE);
	}


	//make_socket_non_blocking(client_fd);

	// server gives client unique id
	int client_id;
	read(client_fd, &client_id, sizeof(client_id));

	printf("Welcome to the server!\nYour ID is: %d\n", client_id);

	
	while (1) {
		if(fork() == 0) {
			// child process for sending
			char msg[1024];
			// blocking 
			fgets(msg, sizeof(msg), stdin);
			printf("\033[A");
			printf("\r\033[2K");
			
			printf("Me: %s", msg);

			// create package 
			Package pkg = {
				.client_id = client_id,
			};
			
			// need to memset because even though the Package struct
			// is on the stack and memory allocated. The memory in
			// the spot where the message is suppose to go is data
			// that has been left over. So we got to zero it
			memset(pkg.message, 0, sizeof(pkg.message));
			memcpy(pkg.message, msg, strnlen(msg, sizeof(pkg.message) - 1)); 
			pkg.message[sizeof(pkg.message) - 1] = '\0';

			uint8_t ser_pkg[sizeof(struct Package)];

			serialize_package(ser_pkg, &pkg);

			// Send the Message
			send(client_fd, &ser_pkg, sizeof(ser_pkg), 0);
			//printf("client %d: %s\n", pkg.client_id, pkg.message);

			exit(0);
		}

		// main process for receiving
		Package received_pkg;
		uint8_t ser_rec_pkg[sizeof(struct Package)];

		int count = recv(client_fd, ser_rec_pkg, sizeof(struct Package), 0);
		if(count <= 0) {
			printf("You disconnected\n");
			close(client_fd);
			return 0;
		}

		deserialize_package(&received_pkg, ser_rec_pkg);

		if (received_pkg.client_id != client_id) {
			printf("Client %d: %s", received_pkg.client_id, received_pkg.message);
		}
	}
	
	//Close the socket
	close(client_fd);
	return 0;
}
