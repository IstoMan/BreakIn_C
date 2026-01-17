#pragma once

#include <raylib.h>

typedef struct {
  Rectangle dimensions;
  Vector2 velocity;
  Color color;
} Obj;

Obj initObj(int width, int height, Vector2 initial_position,
            Vector2 initial_velocity, Color color) {
  Obj obj;

  obj.dimensions.x = initial_position.x - (width / 2.0);
  obj.dimensions.y = initial_position.y - (height / 2.0);

  obj.dimensions.width = width;
  obj.dimensions.height = height;
  obj.velocity.x = initial_velocity.x;
  obj.velocity.y = initial_velocity.y;

  obj.color = color;

  return obj;
}
