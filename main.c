#include <netinet/in.h>
#include <raylib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 65432

int main() { 

	struct sockaddr_in sock_addr = {
		.sin_family = AF_INET, 
		.sin_port = htons(PORT)
	};

	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	
	
	// init app
	InitWindow(600, 500, "amazing window");
	// run app
	while (!WindowShouldClose()) {
		BeginDrawing();

		ClearBackground(SKYBLUE);

		EndDrawing();
	}
	// close app
	CloseWindow();
	return(0); 
}

