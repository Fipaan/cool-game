#include "game.h"

int main() {
	Game game = init_game(SCENE_MAIN, 1600, 900, "Cool Game!");
	while(!WindowShouldClose()) {
		update_game(game);
		update_scene(&game);
		draw_scene(game);
	}
	return 0;
}
