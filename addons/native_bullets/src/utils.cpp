#include <cmath>

#include <Texture.hpp>

#include "utils.h"

using namespace godot;

float exp_decay(float a, float b, float decay, float delta) {
    return b + (a - b) * exp(-decay * delta);
}

Transform2D _transform_set_scale(Transform2D transform, Vector2 scale_vec) {
    Transform2D ret_trans = Transform2D();
	ret_trans.set_origin(Vector2(0.0, 0.0));
    ret_trans.set_rotation(transform.get_rotation());
    ret_trans = ret_trans.scaled(scale_vec);
    ret_trans.set_origin(transform.get_origin());
	return ret_trans;
}
	