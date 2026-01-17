#include "./game.c"
#include <raylib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

int main(void) {
  Game g = initGame();
  InitWindow(g.window_width, g.window_height, g.window_title);
  SetTargetFPS(60);
  // uint16_t score = 0;
  //  Counter removed_tiles;

  while (!WindowShouldClose()) {
    float deltaTime = GetFrameTime();

    gameOnUpdate(&g, deltaTime);

    BeginDrawing();
    ClearBackground(BLACK);
    // draw commands
    gameOnRender(&g);

    EndDrawing();
  }

  gameShutdown(&g);
  CloseWindow();
  return EXIT_SUCCESS;
}
