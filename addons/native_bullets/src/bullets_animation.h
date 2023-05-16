#ifndef BULLET_ANIMATION_H
#define BULLET_ANIMATION_H

#include <Godot.hpp>
#include <Transform2D.hpp>
#include <Node.hpp>
#include <Curve.hpp>

#include <limits>

// #include "bullets.h"

using namespace godot;

// struct BulletID {
// 	int32_t index;
// 	int32_t cycle;
// 	int32_t set;

// 	BulletID(int32_t index, int32_t cycle, int32_t set): 
// 		index(index), cycle(cycle), set(set) {}
// };

class BulletsAnimation : public Node {
	GODOT_CLASS(BulletsAnimation, Node)

public:
    float_t duration;
    Ref<Curve> glow_curve;
    Ref<Curve> scale_curve;
    Ref<Curve> rotation_curve;
    Ref<Curve> alpha_curve;


	// RID item_rid;
	// int32_t cycle = 0;
	// int32_t shape_index = -1;
	// Transform2D transform;
	// Vector2 velocity;
	// // Allows the bullet velocity to accelerate in this direction
	// Vector2 acceleration_basis_vector;
	// // Accelation Speed along the acceleration_basis_vector
	// float_t acceleration_speed = 0.0;
	// // Maximum Speed we can accelerate to
	// float_t max_speed = std::numeric_limits<float>::max();
	// Color modulate;
	// float glow_degree;
	// String animation_name;
	// float animation_start_time;
	// float lifetime;
	// Variant data;

    BulletsAnimation() {
        // TODO: Figure out what's up with this
        // glow_curve = Curve::_new();
        // scale_curve = Curve::_new();
        // rotation_curve = Curve::_new();
        // alpha_curve = Curve::_new();
    }

	void _init() {}

    void _ready() {}

    static void _register_methods() {
        register_property<BulletsAnimation, Ref<Curve>>("glow_curve", &BulletsAnimation::glow_curve,
            Ref<Curve>(), GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT,
            GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Curve");
    }

	// RID get_item_rid() { return item_rid; }
	// void set_item_rid(RID value) { ERR_PRINT("Can't edit the item rid of bullets!"); }

	// int32_t get_cycle() { return cycle; }
	// void set_cycle(int32_t value) { ERR_PRINT("Can't edit the cycle of bullets!"); }

	// int32_t get_shape_index() { return shape_index; }
	// void set_shape_index(int32_t value) { ERR_PRINT("Can't edit the shape index of bullets!"); }


		// register_property<Bullet, RID>("item_rid", &Bullet::set_item_rid, &Bullet::get_item_rid, RID());
		// register_property<Bullet, int32_t>("cycle", &Bullet::set_cycle, &Bullet::get_cycle, 0);
		// register_property<Bullet, int32_t>("shape_index", &Bullet::set_shape_index, &Bullet::get_shape_index, 0);

		// register_property<Bullet, Transform2D>("transform", &Bullet::transform, Transform2D());
		// register_property<Bullet, Vector2>("velocity", &Bullet::velocity, Vector2());

		// register_property<Bullet, Vector2>("acceleration_basis_vector", &Bullet::acceleration_basis_vector, Vector2());
		// register_property<Bullet, float_t>("acceleration_speed", &Bullet::acceleration_speed, 0.0f);
		// register_property<Bullet, float_t>("max_speed", &Bullet::max_speed, 0.0f);

		// register_property<Bullet, Color>("modulate", &Bullet::modulate,
		// 	Color(1.0, 1.0, 1.0, 1.0), GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT,
		// 	GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Color");
		
		// register_property<Bullet, float>("glow_degree", &Bullet::glow_degree, 1.0f);

		// register_property<Bullet, float>("lifetime", &Bullet::lifetime, 0.0f);
		// register_property<Bullet, Variant>("data", &Bullet::data, Variant());
};

#endif