#include <raylib.h>

int main() { 

	const int screen_width = 600;
	const int screen_height = 500;

	// init app
	InitWindow(600, 500, "amazing window");

	Font font = GetFontDefault();

	const char text[] = "Sample Text";

	Vector2 mouse_pos = {0};
	Vector2 player_pos = {
		.x = screen_width/2.0f,
		.y = screen_height/2.0f,
	};

	Camera2D game_camera = {
		.offset = (Vector2) {600, 500},
		.rotation = 0.0f,
		.target = mouse_pos,
		.zoom = 1.0f
	};



	if(!IsWindowFullscreen()) {
		ToggleFullscreen();
	}
	// run app
	while (!WindowShouldClose()) {
		float delta_time = GetFrameTime();
		// update
		if(IsKeyDown(KEY_W)) player_pos.y -= (200.0f * delta_time);
		if(IsKeyDown(KEY_S)) player_pos.y += (200.0f * delta_time);
		if(IsKeyDown(KEY_A)) player_pos.x -= (200.0f * delta_time);
		if(IsKeyDown(KEY_D)) player_pos.x += (200.0f * delta_time);

		game_camera.target = player_pos;
		

		mouse_pos = GetMousePosition();
		//UpdateCamera((Camera *)&game_camera, 0);

		// Draw
		BeginDrawing();

		DrawText(text , mouse_pos.x, mouse_pos.y, 4, BLACK);
		DrawFPS(30, 30);

		ClearBackground(SKYBLUE);

		BeginMode2D(game_camera);
			for (int x = -1000; x < 1000; x += 50)
				DrawLine(x, -1000, x, 1000, LIGHTGRAY);
			for (int y = -1000; y < 1000; y += 50)
				DrawLine(-1000, y, 1000, y, LIGHTGRAY);
		DrawCircleV(player_pos, 20, DARKGREEN);
		EndMode2D();
		EndDrawing();
	}
	// close app
	CloseWindow();
	return(0); 
}

