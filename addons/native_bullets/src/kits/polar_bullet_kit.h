#ifndef POLAR_BULLET_KIT_H
#define POLAR_BULLET_KIT_H

#include <Math.hpp>
#include <Texture.hpp>
#include <PackedScene.hpp>

#include "../bullets.h"
#include "../bullet_kit.h"

using namespace godot;


// Bullet definition.
class PolarBullet : public Bullet {
	GODOT_CLASS(PolarBullet, Bullet)
public:
	float_t prev_r;
	float_t prev_theta; // in degrees
	float_t r;
	float_t theta; // in degrees
	Vector2 delta_velocity;

	Vector2 unit_dir_vector;
	float starting_speed;

	Transform2D get_transform() {
		Transform2D prev_transform = this->_get_transform(this->prev_r, this->prev_theta);
		Transform2D final_transform = this->_get_transform(this->r, this->theta);
		Vector2 dir = final_transform.get_origin() - prev_transform.get_origin() + delta_velocity;
		final_transform.set_rotation(dir.angle());
		return final_transform;
	}

	Transform2D _get_transform(float_t _r, float_t _theta) {
		float_t theta_offset = Dictionary(this->data)["theta_offset"];
		float_t theta_mult = Dictionary(this->data)["theta_mult"];
		float_t final_rotation = Math::deg2rad(_theta + theta_offset) * theta_mult;
		Transform2D ret = transform.translated(Vector2::RIGHT.rotated(final_rotation) * _r);
		return ret;
	}

	int32_t get_z_index() {
		int32_t base_z_index = this->z_index;
		float_t theta_offset = Dictionary(this->data)["theta_offset"];
		float_t normalized_rotation = fmod(this->theta + theta_offset, 360.0);
		int32_t ret = (base_z_index - 1) ? (normalized_rotation <= 90.0 && normalized_rotation >= 270.0) : 1000;//(base_z_index + 1);
		return ret;
	}

	void set_velocity(Vector2 velocity) {
		starting_speed = velocity.length();
		this->velocity = velocity;
		// Godot::print("Setting velocity! ", this->velocity);
	}

	void _init() {
		this->reset();
	}

	void reset() {
		prev_r = 0.0;
		prev_theta = 0.0;
		r = 0.0;
		theta = 0.0;
		unit_dir_vector = Vector2::RIGHT;
	}

	static void _register_methods() {
		register_property<DynamicBullet, Vector2>("velocity",
			&DynamicBullet::set_velocity,
			&DynamicBullet::get_velocity, Vector2());
		register_property<DynamicBullet, float>("starting_speed",
			&DynamicBullet::starting_speed, 0.0f);
	}
};

// Composite Bullet kit definition.
class PolarBulletKit : public BulletKit {
	GODOT_CLASS(PolarBulletKit, BulletKit)
public:
	BULLET_KIT(PolarBulletsPool)

	Ref<Texture> texture;

	// bool lifetime_curves_loop = true;
	// bool free_after_lifetime = false;
	bool r_loop = true;
	bool r_as_speed = false;
	float_t r_min = 0.0;
	float_t r_max = 1.0;
	Ref<Curve> r_over_lifetime;
	bool theta_loop = true;
	bool theta_as_speed = false;
	float_t theta_min = 0.0;
	float_t theta_max = 0.0;
	Ref<Curve> theta_over_lifetime;

	bool lifetime_curves_loop = true;
	Ref<Curve> speed_multiplier_over_lifetime;

	static void _register_methods() {
		register_property<PolarBulletKit, Ref<Texture>>("texture", &PolarBulletKit::texture, Ref<Texture>(), 
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Texture");

		// R Properties
		register_property<PolarBulletKit, bool>("r_loop", &PolarBulletKit::r_loop, false,
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT);
		register_property<PolarBulletKit, float_t>("r_min", &PolarBulletKit::r_min, 0.0,
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT);
		register_property<PolarBulletKit, float_t>("r_max", &PolarBulletKit::r_max, 1.0,
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT);
		register_property<PolarBulletKit, bool>("r_as_speed", &PolarBulletKit::r_as_speed, false,
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT);
		register_property<PolarBulletKit, Ref<Curve>>("r_over_lifetime", &PolarBulletKit::r_over_lifetime, Ref<Curve>(),
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Curve");

		// Theta Properties
		register_property<PolarBulletKit, bool>("theta_loop", &PolarBulletKit::theta_loop, false,
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT);
		register_property<PolarBulletKit, float_t>("theta_min", &PolarBulletKit::theta_min, 0.0,
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT);
		register_property<PolarBulletKit, float_t>("theta_max", &PolarBulletKit::theta_max, 1.0,
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT);
		register_property<PolarBulletKit, bool>("theta_as_speed", &PolarBulletKit::theta_as_speed, false,
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT);
		register_property<PolarBulletKit, Ref<Curve>>("theta_over_lifetime", &PolarBulletKit::theta_over_lifetime, Ref<Curve>(), 
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Curve");

		// Dynamic Speed Properties
		register_property<PolarBulletKit, bool>("lifetime_curves_loop", &PolarBulletKit::lifetime_curves_loop, true,
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT);
		register_property<PolarBulletKit, Ref<Curve>>("speed_multiplier_over_lifetime", &PolarBulletKit::speed_multiplier_over_lifetime, Ref<Curve>(), 
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Curve");

		BULLET_KIT_REGISTRATION(PolarBulletKit, PolarBullet)
	}
};

// Bullets pool definition.
class PolarBulletsPool : public AbstractBulletsPool<PolarBulletKit, PolarBullet> {

	// void _init_bullet(Bullet* bullet); Use default implementation.

	void _enable_bullet(PolarBullet* bullet) {
		// Reset the bullet lifetime.
		bullet->lifetime = 0.0f;
		bullet->reset();
		Rect2 texture_rect = Rect2(-kit->texture->get_size() / 2.0f, kit->texture->get_size());
		RID texture_rid = kit->texture->get_rid();

		// Configure the bullet to draw the kit texture each frame.
		VisualServer::get_singleton()->canvas_item_add_texture_rect(bullet->item_rid,
			texture_rect,
			texture_rid);
		bullet->z_index = kit->z_index;
		VisualServer::get_singleton()->canvas_item_set_z_index(bullet->item_rid, bullet->get_z_index());
		bullet->r = Dictionary(bullet->data)["r_init"];
		// Initialize to static unit vector (to support 0 speed)
		if (!Math::is_equal_approx(bullet->velocity.length(), 0.0)) {
			bullet->unit_dir_vector = bullet->velocity.normalized();
			bullet->starting_speed = bullet->velocity.length();
		}

		// Vector2 normal = Vector2::RIGHT; //.rotated(bullet->transform->get_angle());
		// VisualServer::get_singleton()->canvas_item_add_line(bullet->item_rid,
		// 	// bullet->transform.get_origin(),
		// 	Vector2(0.0, 0.0),
		// 	// bullet->transform.get_origin() + (normal * 20.0),
		// 	(normal * 30.0),
		// 	Color(0.0, 0.0, 0.0, 0.0),
		// 	3.0);
	}

	bool _process_bullet(PolarBullet* bullet, float delta) {
		if(kit->r_over_lifetime.is_valid()) {
			float_t adjusted_lifetime = bullet->lifetime / bullet->lifetime_curves_span;
			if(kit->r_loop) {
				adjusted_lifetime = fmod(adjusted_lifetime, 1.0f);
			}
			bullet->prev_r = bullet->r;
			float_t interp_val = kit->r_over_lifetime->interpolate(adjusted_lifetime);
			float_t final_val = Math::lerp(kit->r_min, kit->r_max, interp_val);
			if (kit->r_as_speed) {
				bullet->r += final_val * delta;
			} else {
				bullet->r = final_val;
			}
		} else {
			float_t speed = Dictionary(bullet->data)["r_speed"];
			bullet->r += speed * delta;
		}

		// TODO: This could be abstracted if we really wanted
		if(kit->theta_over_lifetime.is_valid()) {
			float_t adjusted_lifetime = bullet->lifetime / bullet->lifetime_curves_span;
			if(kit->theta_loop) {
				adjusted_lifetime = fmod(adjusted_lifetime, 1.0f);
			}
			bullet->prev_theta = bullet->theta;
			float_t interp_val = kit->theta_over_lifetime->interpolate(adjusted_lifetime);
			float_t final_val = Math::lerp(kit->theta_min, kit->theta_max, interp_val);
			if (kit->theta_as_speed) {
				bullet->theta += final_val * delta;
			} else {
				bullet->theta = final_val;
			}
		} else {
			float_t speed = Dictionary(bullet->data)["theta_speed"];
			bullet->theta += speed * delta;
		}
		
		// TODO: Make this sane
		// TODO: Handle the case of zero speed??
		// Dynamic Movement processing
		if(kit->speed_multiplier_over_lifetime.is_valid()) {
			float_t adjusted_lifetime = bullet->lifetime / bullet->lifetime_curves_span;
			if(kit->lifetime_curves_loop) {
				adjusted_lifetime = fmod(adjusted_lifetime, 1.0f);
			}
			float_t speed_multiplier = kit->speed_multiplier_over_lifetime->interpolate(adjusted_lifetime);
			// Vector2 dir_vec = Vector2::RIGHT.rotated(bullet->get_transform().get_rotation());
			bullet->velocity = (bullet->unit_dir_vector) * bullet->starting_speed * speed_multiplier;
		}

		_process_acceleration(bullet, delta);
		bullet->delta_velocity = bullet->velocity * delta;
		bullet->transform.set_origin(bullet->transform.get_origin() + bullet->delta_velocity);
		_process_animation(bullet, delta);

		if(!active_rect.has_point(bullet->get_transform().get_origin())) {
			// Return true if the bullet should be deleted.
			return true;
		}
		// Bullet is still alive, increase its lifetime.
		bullet->lifetime += delta;
		// Return false if the bullet should not be deleted yet.
		return false;
	}
};

BULLET_KIT_IMPLEMENTATION(PolarBulletKit, PolarBulletsPool)

#endif
