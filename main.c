#include <raylib.h>

int main() { 

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

