#include <raylib.h>
#include <raymath.h>

typedef struct Cube {
	Vector3 pos;
	Vector3 size;
	Color body_color, wires_color;
} Cube;

typedef struct Text {
	char *text;
	Vector2 pos;
	Vector2 origin;
	float rotation;
	float font;
	float spacing;
	Color color;
} Text;

typedef struct Object {
	union {
		Cube *cube;
		Camera *camera;
		Text *text;
	} as;
} Object;

typedef struct Scene {
	Object *object;
} Scene;

typedef enum SceneName {
	SCENE_MAIN,
	SCENE_COUNT,
} SceneName;

typedef enum ObjectMainName {
	OBJECT_MAIN_CUBE,
	OBJECT_MAIN_CAMERA,
	OBJECT_MAIN_TEXT_DEBUG,
	OBJECT_MAIN_COUNT,
} ObjectMainName;

typedef struct Game {
	Scene scene[SCENE_COUNT];
	SceneName curr_scene;
	Vector2 size;
	struct Player {
		Vector3 position;
		Vector3 speed;
		float height;
		float radius;
	} player;
} Game;

Cube __object_main_cube = {
	.pos = {0.0f, 5.0f, 0.0f},
	.size = {10.0f, 10.0f, 10.0f},
	.body_color = {0xFF, 0x00, 0x00, 0xFF},
	.wires_color = {0x00, 0x00, 0x00, 0xFF},
};


Camera __objects_main_camera = {
	.up = {0.0f, 1.0f, 0.0f},
	.fovy = 90.0f,
	.projection = CAMERA_PERSPECTIVE,
	.position = {30.0f, 100.0f, 30.0f},
	.target = {0.0f, 0.0f, 0.0f},
};

Text __objects_text_debug = {0};

char buf[128] = {0};

Object __objects_main[OBJECT_MAIN_COUNT] = {0};
