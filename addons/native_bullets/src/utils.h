#ifndef UTILS_H
#define UTILS_H

#include <Texture.hpp>

using namespace godot;

float exp_decay(float a, float b, float decay, float delta);
Transform2D _transform_set_scale(Transform2D transform, Vector2 scale_vec);

#endif
