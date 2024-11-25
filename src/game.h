#include <stdio.h>
#include "game_impl.h"

#define CAMERA_RHEIGHT 0.8f

#define V3(vec3) (vec3).x, (vec3).y, (vec3).z

void draw_cube(Cube cube, bool body, bool wires) {
	if(body) DrawCubeV(cube.pos, cube.size, cube.body_color);
	if(wires) DrawCubeWiresV(cube.pos, cube.size, cube.wires_color);
}

Vector3 get_rotation(Vector3 start, Vector3 end) {
	return Vector3Normalize(Vector3Subtract(end, start));
}

void draw_text(Text text) {
	DrawTextPro(
			GetFontDefault(), 
			text.text, text.pos, 
			text.origin, text.rotation, 
			text.font, text.spacing, text.color);
}

Vector3 camera_to_player(Game game, Camera camera) {
	Vector3 camera_relative = Vector3Subtract(camera.target, camera.position);
	camera_relative.y = 0.0f;
	camera_relative = Vector3Scale(Vector3Normalize(camera_relative), game.player.radius);
	camera_relative.y = game.player.height * CAMERA_RHEIGHT;

	return Vector3Subtract(camera.position, camera_relative);
}

Vector3 player_to_camera(Game game, Camera camera) {
	Vector3 camera_relative = Vector3Subtract(camera.target, camera.position);
	camera_relative.y = 0.0f;
	camera_relative = Vector3Scale(Vector3Normalize(camera_relative), game.player.radius);
	camera_relative.y = game.player.height * CAMERA_RHEIGHT;

	return Vector3Add(game.player.position, camera_relative);
}

Game init_game(SceneName init_scene, int width, int height, const char* title) {
	Game game = {
		.size = {
			.x = width,
			.y = height,
		},
		.curr_scene = init_scene,
		.player = {
			.radius = 10.0f,
			.height = 20.0f,
		},
	};
	InitWindow(width, height, title);
	game.scene[SCENE_MAIN].object = __objects_main;
	game.scene[SCENE_MAIN].object[OBJECT_MAIN_CUBE].as.cube = &__object_main_cube;
	game.scene[SCENE_MAIN].object[OBJECT_MAIN_CAMERA].as.camera = &__objects_main_camera;
	{
		Vector3 cam_pos = game.scene[SCENE_MAIN].object[OBJECT_MAIN_CAMERA].as.camera->position;
		game.player.position = camera_to_player(game, __objects_main_camera);
	}
	__objects_text_debug = (Text) {
		.pos = Vector2Multiply(game.size, (Vector2) {0.05f, 0.9f}),
		.font = height * 0.05f,
		.color = RAYWHITE,
		.text = buf,
		.spacing = height * 0.005f,
	};
	game.scene[SCENE_MAIN].object[OBJECT_MAIN_TEXT_DEBUG].as.text = &__objects_text_debug;
	return game;
}

void draw_scene_main(Game game, Scene scene) {
	BeginDrawing();
	BeginMode3D(*scene.object[OBJECT_MAIN_CAMERA].as.camera);
	ClearBackground(GetColor(0x181818FF));
	DrawGrid(50, 1.0f);
	draw_cube(*scene.object[OBJECT_MAIN_CUBE].as.cube, true, true);
	//DrawCubeV(game.player_position, game.player_size, RAYWHITE);
	EndMode3D();
	//draw_text(*scene.object[OBJECT_MAIN_TEXT_DEBUG].as.text);
	EndDrawing();
}

bool isMoving() {
	return IsKeyDown(KEY_W) || IsKeyDown(KEY_S) || IsKeyDown(KEY_A) || IsKeyDown(KEY_D);
}

void update_scene_main(Game* game, Scene scene) {
	Camera* camera = scene.object[OBJECT_MAIN_CAMERA].as.camera;
	float dt = GetFrameTime();
	Vector2 mouse_delta = GetMouseDelta();
	Vector3 rotation = {
		.x = mouse_delta.x,
		.y = mouse_delta.y,
		.z = 0.0f,
	};
	if(Vector3DistanceSqr(camera->position, camera->target) < 100.0f) {
		camera->target = Vector3Add(camera->target, Vector3Scale(Vector3Subtract(camera->target, camera->position), 10.0f));
	}
	UpdateCameraPro(camera, (Vector3){0}, rotation, (IsKeyDown(KEY_KP_ADD) - IsKeyDown(KEY_KP_SUBTRACT)) * 100.0f * dt);
	if(isMoving()) {
		Vector3 dv = Vector3Subtract(camera->target, camera->position);
		Vector2 forward = {dv.x, dv.z};
		Vector2 left = {forward.y, -forward.x};
		Vector2 moving = Vector2Add(Vector2Scale(forward, IsKeyDown(KEY_W) - IsKeyDown(KEY_S)), Vector2Scale(left, IsKeyDown(KEY_A) - IsKeyDown(KEY_D)));
		moving = Vector2Scale(Vector2Normalize(moving), 25.0f * dt);
		Vector3 moving3 = {moving.x, 0.0f, moving.y};
		camera->position = Vector3Add(camera->position, moving3);
		camera->target = Vector3Add(camera->target, moving3);
	}
	game->player.position = camera_to_player(*game, *camera);
	if(game->player.position.y > 0.0f) game->player.speed.y -= 0.02f * dt;	
	game->player.speed.y += 35.0f * IsKeyPressed(KEY_SPACE) * (game->player.speed.y <= 0.0f) * (game->player.position.y * game->player.position.y <= 0.01f) * dt;
	Vector3 new_pos = Vector3Add(game->player.position, game->player.speed);
	if(new_pos.y < 0.0f) {
		new_pos.y = 0.0f;
		game->player.speed = (Vector3) {0};
	}
	if(Vector3LengthSqr(new_pos) > 0.0f) {
		game->player.position = new_pos;
		Vector3 delta = Vector3Subtract(camera->target, camera->position);
		camera->position = player_to_camera(*game, *camera);
		camera->target = Vector3Add(camera->position, delta);
	}
	//snprintf(buf, sizeof(buf), "Player: (%.2f, %.2f, %.2f), Camera: (%.2f, %.2f, %.2f)", V3(game->player_position), V3(camera->position));
}

void update_game(Game game) {
	float mouse_delta = Vector2LengthSqr(Vector2Subtract(GetMousePosition(), Vector2Scale(game.size, 0.5f)));
	if(mouse_delta > 100.0f) SetMousePosition(game.size.x * 0.5f, game.size.y * 0.5f);
}

void update_scene(Game* game) {
	switch(game->curr_scene) {
		case SCENE_MAIN: {
			update_scene_main(game, game->scene[SCENE_MAIN]);
		} break;
		case SCENE_COUNT: {
			printf("UNREACHABLE");
		} break;
	}
}

void draw_scene(Game game) {
	switch(game.curr_scene) {
		case SCENE_MAIN: {
			draw_scene_main(game, game.scene[SCENE_MAIN]);
		} break;
		case SCENE_COUNT: {
			printf("UNREACHABLE");
		} break;
	}
}
