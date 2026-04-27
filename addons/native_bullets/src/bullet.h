#ifndef BULLET_H
#define BULLET_H

#include <Godot.hpp>
#include <Transform2D.hpp>

#include <limits>
#include <cassert>

#include "bullets.h"
#include "utils.h"

using namespace godot;

static const float BASE_GLOW_AMOUNT = 1.75;
static const float SCALE_SPEED_HALF_LIFE = 24.0; //32.0; //16.0;
static const float GLOW_HALF_LIFE = 8.0; //32.0;
static const float BASE_MODULATE_HALF_LIFE = 16.0;
static const float MAX_OUT_SCALE = 4.5; //6.0; //3.0; //10.0; //3.0;

static const std::vector<float> STATE_GLOW_BUMPS = {
	0.4, //1.4,
	0.3, //1.3,
	0.2, //1.2,
	0.0
};

static const std::vector<float> STATE_SCALE_BUMPS = {
	1.0, //0.5,
	1.2, //0.6,
	1.4, //0.7,
	0.0
};

static const std::vector<Color> STATE_COLORS = {
	Color(0.0, 1.0, 0.0, 1.0),
	// Color(0, 0.717647, 0),
	// Color(0, 0.37941199999999997, 0),
	Color(0, 0.3, 0),
	// Color(0, 0.266667, 0),
	Color(0, 0.1333335, 0),
	Color(0, 0, 0)
};

#ifndef MAX
#define MAX(a, b) (a > b ? a : b)
#endif


struct BulletID {
	int32_t index;
	int32_t cycle;
	int32_t set;

	BulletID(int32_t index, int32_t cycle, int32_t set): 
		index(index), cycle(cycle), set(set) {}
};

class Bullet : public Object {
	GODOT_CLASS(Bullet, Object)

public:
	RID item_rid;
	int32_t cycle = 0;
	int32_t shape_index = -1;
	bool active = false;
	int prev_graze_type_state = 0;
	int graze_type_state = 0;
	Transform2D transform;
	Transform2D visual_transform;
	Vector2 velocity;
	// Allows the bullet velocity to accelerate in this direction
	Vector2 acceleration_basis_vector;
	// Accelation Speed along the acceleration_basis_vector
	float_t acceleration_speed = 0.0;
	// Maximum Speed we can accelerate to
	float_t max_speed = std::numeric_limits<float>::max();
	int32_t z_index = 0;

	// Dynamic Visual properties
	float_t scale_speed = 0.0;
	float_t scale_val = 1.0;
	float_t desired_scale = 1.0;
	float_t glow_speed = 0.0;
	float_t glow_val = 0.0;
	bool in_game = true;

	Color modulate;
	Color visual_modulate;
	float glow_degree;
	String animation_name;
	float animation_start_time;
	float lifetime;
	float lifetime_curves_span;
	Variant data;
	bool is_player_bullet = false;

	void _init() {
		// Godot::print("Resetting orig bullet! ", this);
	}

	RID get_item_rid() { return item_rid; }
	void set_item_rid(RID value) { ERR_PRINT("Can't edit the item rid of bullets!"); }

	int32_t get_cycle() { return cycle; }
	void set_cycle(int32_t value) { ERR_PRINT("Can't edit the cycle of bullets!"); }

	int32_t get_shape_index() { return shape_index; }
	void set_shape_index(int32_t value) { ERR_PRINT("Can't edit the shape index of bullets!"); }

	Transform2D get_transform() { return transform; }
	Transform2D get_visual_transform() { return visual_transform; }

	int32_t get_z_index() { return z_index; }

	static void _register_methods() {
		register_property<Bullet, RID>("item_rid", &Bullet::set_item_rid, &Bullet::get_item_rid, RID());
		register_property<Bullet, int32_t>("cycle", &Bullet::set_cycle, &Bullet::get_cycle, 0);
		register_property<Bullet, int32_t>("shape_index", &Bullet::set_shape_index, &Bullet::get_shape_index, 0);

		register_property<Bullet, Transform2D>("transform", &Bullet::transform, Transform2D());
		register_property<Bullet, Vector2>("velocity", &Bullet::velocity, Vector2());

		register_property<Bullet, int32_t>("graze_type_state", &Bullet::graze_type_state, 0);

		register_property<Bullet, Vector2>("acceleration_basis_vector", &Bullet::acceleration_basis_vector, Vector2());
		register_property<Bullet, float_t>("acceleration_speed", &Bullet::acceleration_speed, 0.0f);
		register_property<Bullet, float_t>("max_speed", &Bullet::max_speed, std::numeric_limits<float>::max());

		register_property<Bullet, String>("animation_name", &Bullet::animation_name, "");
		register_property<Bullet, float_t>("animation_start_time", &Bullet::animation_start_time, 0.0f);

		register_property<Bullet, Color>("modulate", &Bullet::modulate,
			Color(1.0, 1.0, 1.0, 1.0), GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT,
			GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Color");
		
		register_property<Bullet, float>("glow_degree", &Bullet::glow_degree, 1.0f);

		register_property<Bullet, float>("lifetime", &Bullet::lifetime, 0.0f);
		register_property<Bullet, float>("lifetime_curves_span", &Bullet::lifetime_curves_span, 1.0f,
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_RANGE, "0.001,256.0");
		register_property<Bullet, Variant>("data", &Bullet::data, Variant());

		register_property<Bullet, bool>("is_player_bullet", &Bullet::is_player_bullet, false);
		register_property<Bullet, bool>("in_game", &Bullet::in_game, true);
	}

	float _get_desired_scale() {
		if (in_game) {
			return 1.0;
		} else {
			return MAX_OUT_SCALE;
		}
	}

	Color _get_desired_modulate() {
		assert(graze_type_state < 4);
		Color base_desired_modulate = is_player_bullet ? Color(1.0, 1.0, 1.0, 0.6) : STATE_COLORS[graze_type_state];
		if (in_game) {
			return base_desired_modulate;
		} else {
			return Color(base_desired_modulate.r, base_desired_modulate.g, base_desired_modulate.b, 0.0);
		}
	}

	void _handle_state_change() {
		assert(graze_type_state < 4);
		assert(prev_graze_type_state < 4);
		// assert(prev_graze_type_state <= graze_type_state);
		while (prev_graze_type_state < graze_type_state) {
			glow_speed += STATE_GLOW_BUMPS[prev_graze_type_state];
			// Godot::print("Adjusting glow speed. now: {0}", glow_speed);
			scale_speed += STATE_SCALE_BUMPS[prev_graze_type_state];
			prev_graze_type_state += 1;
		}
		prev_graze_type_state = graze_type_state;
	}
	
	void _handle_scale(float delta) {
		scale_val += scale_speed;
		float prev_scale_val = scale_val;
		scale_val = exp_decay(
			scale_val,
			_get_desired_scale(),
			SCALE_SPEED_HALF_LIFE,
			delta
		);
		float diff = prev_scale_val - scale_val;
		if (diff >= 0.0) {
			scale_speed = MAX(scale_speed - diff, 0.0);
		}
	}

	void _handle_glow(float delta) {
		glow_val += glow_speed;
		float prev_glow_val = glow_val;
		glow_val = exp_decay(
			glow_val,
			0.0, // Always target equilibrium
			GLOW_HALF_LIFE,
			delta
		);
		float diff = prev_glow_val - glow_val;
		assert(diff >= 0.0);
		glow_speed = MAX(glow_speed - diff, 0.0);
	}

	void _handle_base_modulate(float delta) {
		float curr_glow = (!is_player_bullet) ? BASE_GLOW_AMOUNT + glow_val : 1.0;
		Color desired_modulate = _get_desired_modulate();
		visual_modulate.r = exp_decay(
			visual_modulate.r,
			desired_modulate.r * curr_glow,
			BASE_MODULATE_HALF_LIFE,
			delta
		);
		visual_modulate.g = exp_decay(
			visual_modulate.g,
			desired_modulate.g * curr_glow,
			BASE_MODULATE_HALF_LIFE,
			delta
		);
		visual_modulate.b = exp_decay(
			visual_modulate.b,
			desired_modulate.b * curr_glow,
			BASE_MODULATE_HALF_LIFE,
			delta
		);
		visual_modulate.a = exp_decay(
			visual_modulate.a,
			desired_modulate.a,
			BASE_MODULATE_HALF_LIFE,
			delta
		);
	}
};

#endif