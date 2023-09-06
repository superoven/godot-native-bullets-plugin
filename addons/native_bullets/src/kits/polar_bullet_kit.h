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
		Vector2 dir = final_transform.get_origin() - prev_transform.get_origin() + this->delta_velocity;
		final_transform.set_rotation(dir.angle());
		return final_transform;
	}

	Transform2D _get_transform(float_t _r, float_t _theta) {
		Vector2 origin = transform.get_origin();
		float_t base_rotation = transform.get_rotation();
		float_t final_rotation = base_rotation + Math::deg2rad(_theta);
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

	bool lifetime_curves_loop = true;
	// bool free_after_lifetime = false;
	Ref<Curve> r_over_lifetime;
	Ref<Curve> theta_over_lifetime;

	static void _register_methods() {
		register_property<PolarBulletKit, Ref<Texture>>("texture", &PolarBulletKit::texture, Ref<Texture>(), 
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Texture");
		register_property<PolarBulletKit, Ref<Curve>>("r_over_lifetime", &PolarBulletKit::r_over_lifetime, Ref<Curve>(),
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Curve");
		register_property<PolarBulletKit, Ref<Curve>>("theta_over_lifetime", &PolarBulletKit::theta_over_lifetime, Ref<Curve>(), 
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Curve");


		BULLET_KIT_REGISTRATION(PolarBulletKit, Bullet)
	}
};

// Bullets pool definition.
class PolarBulletsPool : public AbstractBulletsPool<PolarBulletKit, PolarBullet> {

	// void _init_bullet(Bullet* bullet); Use default implementation.

	void _enable_bullet(Bullet* bullet) {
		// Reset the bullet lifetime.
		bullet->lifetime = 0.0f;
		Rect2 texture_rect = Rect2(-kit->texture->get_size() / 2.0f, kit->texture->get_size());
		RID texture_rid = kit->texture->get_rid();
		
		// Configure the bullet to draw the kit texture each frame.
		VisualServer::get_singleton()->canvas_item_add_texture_rect(bullet->item_rid,
			texture_rect,
			texture_rid);
	}

	// void _disable_bullet(Bullet* bullet); Use default implementation.

	bool _process_bullet(PolarBullet* bullet, float delta) {
		float_t adjusted_lifetime = bullet->lifetime / bullet->lifetime_curves_span;
		if(kit->lifetime_curves_loop) {
			adjusted_lifetime = fmod(adjusted_lifetime, 1.0f);
		}

		if(kit->r_over_lifetime.is_valid()) {
			bullet->prev_r = bullet->r;
			bullet->r = kit->r_over_lifetime->interpolate(adjusted_lifetime);
		}

		if(kit->theta_over_lifetime.is_valid()) {
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
		// Rotate the bullet based on its velocity "rotate" is enabled.
		// TODO: Handle the rotation properly
		// if(bullet->velocity.length() > 0.0f) {
		// 	bullet->transform.set_rotation(bullet->velocity.angle());
		// }
		// Bullet is still alive, increase its lifetime.
		bullet->lifetime += delta;
		// Return false if the bullet should not be deleted yet.
		return false;
	}
};

BULLET_KIT_IMPLEMENTATION(PolarBulletKit, PolarBulletsPool)

#endif
