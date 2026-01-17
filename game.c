#pragma once

#include "./objects.c"
#include <raylib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
  GAME_MENU = 0,
  GAME_ON,
  GAME_WON,
  GAME_OVER,
} GameState;

typedef struct {
  uint32_t lives;
  uint16_t num_lives;
  uint16_t score;
  uint16_t num_rows;
  uint16_t num_cols;
  uint16_t **tiles_array;
  Rectangle **tiles_boxes;
  GameState game_state;
  Font game_font;
  char score_buffer[100];

  uint16_t paddle_velocity;
  uint16_t ball_velocity;

  bool is_ball_attached;
  Obj ball;
  Obj paddle;
  int window_width, window_height;
  const char *window_title;
} Game;

void resetGame(Game *g);
bool isTilesEmpty(Game *g);
void drawMenu(Game *g, const char *result_text, Color text_color);
void updatePaddle(Game *g, float deltaTime);
void updateBall(Game *g, float deltaTime);

Game initGame(void) {
  Game g;
  g.window_width = 800;
  g.window_height = 700;
  g.paddle_velocity = 380;
  g.ball_velocity = 450;

  g.ball = initObj(15, 15, (Vector2){0},
                   (Vector2){g.ball_velocity, -g.ball_velocity}, WHITE);
  g.paddle = initObj(
      100, 15, (Vector2){.x = g.window_width * 0.5, .y = g.window_height - 20},
      (Vector2){g.paddle_velocity, g.paddle_velocity}, WHITE);
  g.is_ball_attached = true;
  g.num_lives = 1;
  g.window_title = "BreakOut in C";
  g.num_rows = 5;
  g.num_cols = 10;
  g.game_state = GAME_MENU;
  g.game_font = LoadFont("./Planet Jumbo.otf");
  // printf("Is font loaded: %p\n", (void *)&g.game_font);

  g.tiles_array = calloc(g.num_rows, sizeof(*g.tiles_array));
  for (size_t i = 0; i < g.num_rows; ++i) {
    g.tiles_array[i] = calloc(g.num_cols, sizeof(*g.tiles_array[i]));
  }

  g.tiles_boxes = calloc(g.num_rows, sizeof(*g.tiles_boxes));
  for (size_t i = 0; i < g.num_rows; ++i) {
    g.tiles_boxes[i] = calloc(g.num_cols, sizeof(*g.tiles_boxes[i]));
  }

  for (size_t i = 0; i < g.num_rows; i++) {
    for (size_t j = 0; j < g.num_cols; j++) {
      g.tiles_boxes[i][j].x = j * g.window_width / 10.0f;
      g.tiles_boxes[i][j].y = i * 30 + 100;
      g.tiles_boxes[i][j].width = g.window_width / 10.0f;
      g.tiles_boxes[i][j].height = 30;
    }
  }

  return g;
}

void gameOnUpdate(Game *g, float deltaTime) {

  switch (g->game_state) {
  case GAME_MENU:
  case GAME_WON:
  case GAME_OVER:
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
      resetGame(g);
      g->game_state = GAME_ON;
    }
    break;
  case GAME_ON: {

    updateBall(g, deltaTime);
    updatePaddle(g, deltaTime);

    if (CheckCollisionRecs(g->ball.dimensions, g->paddle.dimensions)) {
      g->ball.dimensions.y -= 5;
      g->ball.velocity.y *= -1.0f; // Flip horizontal direction
    }

    if (!isTilesEmpty(g)) {
      g->game_state = GAME_WON;
    }

    if (g->lives == 0) {
      g->game_state = GAME_OVER;
    }

  } break;
  }
}

void gameOnRender(Game *g) {
  switch (g->game_state) {
  case GAME_MENU: {
    drawMenu(g, "Breakout", (Color){255, 0, 0, 255});
  } break;
  case GAME_WON: {
    drawMenu(g, "You Won", (Color){255, 251, 9, 255});
  } break;
  case GAME_OVER:
    drawMenu(g, "Game Over", (Color){255, 0, 0, 255});
    break;
  case GAME_ON: {
    sprintf(g->score_buffer, "Score: %d", g->score);
    DrawTextEx(g->game_font, g->score_buffer,
               (Vector2){g->window_width / 2.0f - 80, 20}, 20, 10,
               (Color){255, 255, 255, 255});
    uint8_t color = 0;
    for (size_t j = 0; j < 5; ++j) {
      for (size_t i = 0; i < 10; ++i) {

        if (g->tiles_array[j][i] != UINT16_MAX) {
          DrawRectangleRec(
              g->tiles_boxes[j][i],
              (Color){color * 255 / 5, color + 90, color + 11, 255});

          if (CheckCollisionRecs(g->ball.dimensions, g->tiles_boxes[j][i])) {
            g->tiles_array[j][i] = UINT16_MAX;
            g->score += 10;
            g->ball.velocity.y *= -1.0f; // Flip vertical direction
          }
        }
      }
      color += 50;
    }
    DrawRectangleRec(g->paddle.dimensions, g->paddle.color);
    DrawRectangleRec(g->ball.dimensions, g->paddle.color);
  } break;
  }
}

void gameShutdown(Game *g) {
  UnloadFont(g->game_font);
  for (size_t i = 0; i < g->num_rows; i++) {
    free(g->tiles_array[i]);
    free(g->tiles_boxes[i]);
  }
  free(g->tiles_array);
  free(g->tiles_boxes);
}

void resetGame(Game *g) {
  g->score = 0;
  g->lives = g->num_lives;
  for (size_t i = 0; i < g->num_rows; i++) {
    for (size_t j = 0; j < g->num_cols; j++) {
      g->tiles_array[i][j] = 0;
    }
  }
}

bool isTilesEmpty(Game *g) {
  bool is_empty = false;

  for (size_t i = 0; i < g->num_rows; i++) {
    for (size_t j = 0; j < g->num_cols; j++) {
      if (g->tiles_array[i][j] != UINT16_MAX) {
        is_empty = true;
      }
    }
  }
  return is_empty;
}

void drawMenu(Game *g, const char *result_text, Color text_color) {
  DrawTextEx(g->game_font, result_text,
             (Vector2){g->window_width / 2.0f, g->window_height / 2.0f}, 25, 10,
             text_color);
  DrawTextEx(g->game_font, "Click to start",
             (Vector2){g->window_width / 2.0f, g->window_height - 50}, 15, 5,
             (Color){255, 255, 255, 255});
}

void updateBall(Game *g, float deltaTime) {
  if (IsKeyDown(KEY_UP)) {
    g->is_ball_attached = false;
  }

  if (g->is_ball_attached) {
    g->ball.dimensions.x =
        (g->paddle.dimensions.x + g->paddle.dimensions.width / 2) -
        g->ball.dimensions.width / 2;
    g->ball.dimensions.y = g->paddle.dimensions.y - 20;
  } else {
    g->ball.dimensions.x += g->ball.velocity.x * deltaTime;
    g->ball.dimensions.y += g->ball.velocity.y * deltaTime;

    if (g->ball.dimensions.x - g->ball.dimensions.width < 0 ||
        g->ball.dimensions.x + g->ball.dimensions.width > g->window_width) {
      g->ball.velocity.x *= -1.0f; // Flip horizontal direction
    }
    if (g->ball.dimensions.y - g->ball.dimensions.width < 0) {
      g->ball.velocity.y *= -1.0f; // Flip vertical direction
    }
    if (g->ball.dimensions.y + g->ball.dimensions.width > g->window_height) {
      g->ball.velocity.y *= -1.0f; // Flip vertical direction
      g->is_ball_attached = true;
      g->lives--;
    }
  }
}

void updatePaddle(Game *g, float deltaTime) {
  if (g->paddle.dimensions.x >= 0 && IsKeyDown(KEY_LEFT)) {
    g->paddle.dimensions.x -= g->paddle.velocity.x * deltaTime;
  }
  if (g->paddle.dimensions.x + g->paddle.dimensions.width + 10 <=
          g->window_width &&
      IsKeyDown(KEY_RIGHT)) {
    g->paddle.dimensions.x += g->paddle.velocity.x * deltaTime;
  }
  if (g->paddle.dimensions.x < 0 &&
      g->paddle.dimensions.x + g->paddle.dimensions.width + 10 <=
          g->window_width) {
  }
}
