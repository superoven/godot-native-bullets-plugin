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

	Transform2D get_transform() {
		Transform2D prev_transform = this->_get_transform(this->prev_r, this->prev_theta);
		Transform2D final_transform = this->_get_transform(this->r, this->theta);
		Vector2 dir = final_transform.get_origin() - prev_transform.get_origin();// + this->delta_velocity;
		// dir = dir.rotated(final_transform.get_rotation());
		// float_t theta_offset = Dictionary(this->data)["theta_offset"];
		// Godot::print("dir angle: {0} theta_offset: {1} final_transform angle: {2}", dir.angle(), theta_offset, final_transform.get_rotation());
		// float_t basis_rotation = transform.get_rotation();
		// final_transform.set_rotation(dir.angle() - basis_rotation);
		final_transform.set_rotation(dir.angle());
		return final_transform;
	}

	Transform2D _get_transform(float_t _r, float_t _theta) {
		// Vector2 origin = transform.get_origin();
		// float_t base_rotation = transform.get_rotation();
		// Godot::print("base_rotation: {0}. theta: {1}", base_rotation, _theta);
		// float_t final_rotation = base_rotation + Math::deg2rad(_theta);
		// Dictionary anim = Dictionary(this->data)->get("theta");
		float_t theta_offset = Dictionary(this->data)["theta_offset"];

		// Godot::print("theta_offset: {0}", theta_offset);
		float_t final_rotation = Math::deg2rad(_theta + theta_offset);
		Transform2D ret = transform.translated(Vector2::RIGHT.rotated(final_rotation) * _r);
		return ret;
	}

	void _init() {
		prev_r = 0.0;
		prev_theta = 0.0;
		r = 0.0;
		theta = 0.0;
	}

	static void _register_methods() {
		// register_property<PolarBullet, Transform2D>("transform",
		// 	&PolarBullet::set_transform,
		// 	&PolarBullet::get_transform, Transform2D());
		// register_property<PolarBullet, Transform2D>("starting_transform",
		// 	&PolarBullet::starting_transform, Transform2D());
		// register_property<PolarBullet, Vector2>("velocity",
		// 	&PolarBullet::set_velocity,
		// 	&PolarBullet::get_velocity, Vector2());
		// register_property<PolarBullet, float>("starting_speed",
		// 	&PolarBullet::starting_speed, 0.0f);
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
	Ref<Curve> r_over_lifetime;
	bool theta_loop = true;
	Ref<Curve> theta_over_lifetime;

	static void _register_methods() {
		register_property<PolarBulletKit, Ref<Texture>>("texture", &PolarBulletKit::texture, Ref<Texture>(), 
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Texture");
		register_property<PolarBulletKit, Ref<Curve>>("r_over_lifetime", &PolarBulletKit::r_over_lifetime, Ref<Curve>(),
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Curve");
		register_property<PolarBulletKit, Ref<Curve>>("theta_over_lifetime", &PolarBulletKit::theta_over_lifetime, Ref<Curve>(), 
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Curve");
		register_property<PolarBulletKit, bool>("r_loop", &PolarBulletKit::r_loop, false,
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT);
		register_property<PolarBulletKit, bool>("theta_loop", &PolarBulletKit::theta_loop, false,
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT);

		BULLET_KIT_REGISTRATION(PolarBulletKit, PolarBullet)
	}
};

// Bullets pool definition.
class PolarBulletsPool : public AbstractBulletsPool<PolarBulletKit, PolarBullet> {

	// void _init_bullet(Bullet* bullet); Use default implementation.

	void _enable_bullet(PolarBullet* bullet) {
		// Reset the bullet lifetime.
		bullet->lifetime = 0.0f;
		Rect2 texture_rect = Rect2(-kit->texture->get_size() / 2.0f, kit->texture->get_size());
		RID texture_rid = kit->texture->get_rid();
		
		// Configure the bullet to draw the kit texture each frame.
		VisualServer::get_singleton()->canvas_item_add_texture_rect(bullet->item_rid,
			texture_rect,
			texture_rid);
		// Vector2 normal = Vector2::RIGHT; //.rotated(bullet->transform->get_angle());
		// VisualServer::get_singleton()->canvas_item_add_line(bullet->item_rid,
		// 	// bullet->transform.get_origin(),
		// 	Vector2(0.0, 0.0),
		// 	// bullet->transform.get_origin() + (normal * 20.0),
		// 	(normal * 30.0),
		// 	Color(0.0, 0.0, 0.0, 0.0),
		// 	3.0);
	}

	// void _disable_bullet(Bullet* bullet); Use default implementation.

	// bool _adjust_r(PolarBullet* bullet, bool loop, float delta) {
	// 	float_t adjusted_lifetime = bullet->lifetime / bullet->lifetime_curves_span;
	// 	if(loop) {
	// 		adjusted_lifetime = fmod(adjusted_lifetime, 1.0f);
	// 	}
	// }

	bool _process_bullet(PolarBullet* bullet, float delta) {
		if(kit->r_over_lifetime.is_valid()) {
			float_t adjusted_lifetime = bullet->lifetime / bullet->lifetime_curves_span;
			if(kit->r_loop) {
				adjusted_lifetime = fmod(adjusted_lifetime, 1.0f);
			}
			bullet->prev_r = bullet->r;
			bullet->r = kit->r_over_lifetime->interpolate(adjusted_lifetime);
		}

		// TODO: This could be abstracted if we really wanted
		if(kit->theta_over_lifetime.is_valid()) {
			float_t adjusted_lifetime = bullet->lifetime / bullet->lifetime_curves_span;
			if(kit->theta_loop) {
				adjusted_lifetime = fmod(adjusted_lifetime, 1.0f);
			}
			bullet->prev_theta = bullet->theta;
			bullet->theta = kit->theta_over_lifetime->interpolate(adjusted_lifetime);
		}

		_process_acceleration(bullet, delta);
		_process_animation(bullet, delta);
		bullet->delta_velocity = bullet->velocity * delta;
		bullet->transform.set_origin(bullet->transform.get_origin() + bullet->delta_velocity);

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
