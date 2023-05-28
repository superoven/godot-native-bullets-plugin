#ifndef DYNAMIC_BULLET_KIT_H
#define DYNAMIC_BULLET_KIT_H

#include <Texture.hpp>
#include <PackedScene.hpp>
#include <Curve.hpp>

#include "../bullets.h"
#include "../bullet_kit.h"

using namespace godot;


// Bullet definition.
class DynamicBullet : public Bullet {
	GODOT_CLASS(DynamicBullet, Bullet)
public:
	Transform2D starting_transform;
	float starting_speed;
	// Vector2 starting_

	void set_transform(Transform2D transform) {
		starting_transform = transform;
		this->transform = transform;
	}

	Transform2D get_transform() {
		return transform;
	}

	void set_velocity(Vector2 velocity) {
		starting_speed = velocity.length();
		this->velocity = velocity;
		// Godot::print("INSIDE C++: {0} Setting bullet velocity to: {1}", this, this->velocity);
	}

	Vector2 get_velocity() {
		return velocity;
	}

	void _init() {}

	static void _register_methods() {
		register_property<DynamicBullet, Transform2D>("transform",
			&DynamicBullet::set_transform,
			&DynamicBullet::get_transform, Transform2D());
		register_property<DynamicBullet, Transform2D>("starting_transform",
			&DynamicBullet::starting_transform, Transform2D());
		register_property<DynamicBullet, Vector2>("velocity",
			&DynamicBullet::set_velocity,
			&DynamicBullet::get_velocity, Vector2());
		register_property<DynamicBullet, float>("starting_speed",
			&DynamicBullet::starting_speed, 0.0f);
	}
};

// Bullet kit definition.
class DynamicBulletKit : public BulletKit {
	GODOT_CLASS(DynamicBulletKit, BulletKit)
public:
	BULLET_KIT(DynamicBulletsPool)

	Ref<Texture> texture;
	float lifetime_curves_span = 1.0f;
	bool lifetime_curves_loop = true;
	bool free_after_lifetime = false;
	Ref<Curve> speed_multiplier_over_lifetime;
	Ref<Curve> rotation_offset_over_lifetime;
	Ref<Curve> alpha_over_lifetime;
	Ref<Curve> red_over_lifetime;

	static void _register_methods() {
		register_property<DynamicBulletKit, Ref<Texture>>("texture", &DynamicBulletKit::texture, Ref<Texture>(), 
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Texture");
		register_property<DynamicBulletKit, float>("lifetime_curves_span", &DynamicBulletKit::lifetime_curves_span, 1.0f,
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_RANGE, "0.001,256.0");
		register_property<DynamicBulletKit, bool>("lifetime_curves_loop", &DynamicBulletKit::lifetime_curves_loop, true,
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT);
		register_property<DynamicBulletKit, bool>("free_after_lifetime", &DynamicBulletKit::free_after_lifetime, false,
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT);
		register_property<DynamicBulletKit, Ref<Curve>>("speed_multiplier_over_lifetime", &DynamicBulletKit::speed_multiplier_over_lifetime, Ref<Curve>(), 
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Curve");
		register_property<DynamicBulletKit, Ref<Curve>>("rotation_offset_over_lifetime", &DynamicBulletKit::rotation_offset_over_lifetime, Ref<Curve>(), 
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Curve");
		register_property<DynamicBulletKit, Ref<Curve>>("alpha_over_lifetime", &DynamicBulletKit::alpha_over_lifetime, Ref<Curve>(), 
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Curve");
		register_property<DynamicBulletKit, Ref<Curve>>("red_over_lifetime", &DynamicBulletKit::red_over_lifetime, Ref<Curve>(),
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Curve");
		
		BULLET_KIT_REGISTRATION(DynamicBulletKit, DynamicBullet)
	}
};

// Bullets pool definition.
class DynamicBulletsPool : public AbstractBulletsPool<DynamicBulletKit, DynamicBullet> {

	// void _init_bullet(Bullet* bullet); Use default implementation.

	void _enable_bullet(DynamicBullet* bullet) {
		// Reset the bullet lifetime.
		bullet->lifetime = 0.0f;
		Rect2 texture_rect = Rect2(-kit->texture->get_size() / 2.0f, kit->texture->get_size());
		RID texture_rid = kit->texture->get_rid();
		
		// Configure the bullet to draw the kit texture each frame.
		VisualServer::get_singleton()->canvas_item_add_texture_rect(bullet->item_rid,
			texture_rect,
			texture_rid);
		// Godot::print("INSIDE C++: {0} enabled! velocity: {1}", this, bullet->velocity);

	}

	// void _disable_bullet(Bullet* bullet); Use default implementation.

	bool _process_bullet(DynamicBullet* bullet, float delta) {
		float adjusted_lifetime = bullet->lifetime / kit->lifetime_curves_span;
		if(kit->lifetime_curves_loop) {
			adjusted_lifetime = fmod(adjusted_lifetime, 1.0f);
		}

		if(kit->speed_multiplier_over_lifetime.is_valid()) {
			float speed_multiplier = kit->speed_multiplier_over_lifetime->interpolate(adjusted_lifetime);
			bullet->velocity = bullet->velocity.normalized() * bullet->starting_speed * speed_multiplier;
		}
		if(kit->rotation_offset_over_lifetime.is_valid()) {
			float rotation_offset = kit->rotation_offset_over_lifetime->interpolate(adjusted_lifetime);
			float absolute_rotation = bullet->starting_transform.get_rotation() + rotation_offset;

			bullet->velocity = bullet->velocity.rotated(absolute_rotation - bullet->starting_transform.get_rotation());
		}
		if(kit->alpha_over_lifetime.is_valid()) {
			float alpha = kit->alpha_over_lifetime->interpolate(adjusted_lifetime);
			Color color = bullet->modulate;
			color.a = alpha;
			bullet->modulate = color;
			VisualServer::get_singleton()->canvas_item_set_modulate(bullet->item_rid, color);
		}
		if(kit->red_over_lifetime.is_valid()) {
			float red = kit->red_over_lifetime->interpolate(adjusted_lifetime);
			Color color = bullet->modulate;
			color.r = red;
			bullet->modulate = color;
			VisualServer::get_singleton()->canvas_item_set_modulate(bullet->item_rid, color);
		}
		_process_acceleration(bullet, delta);
		_process_animation(bullet, delta);

		bullet->transform.set_origin(bullet->transform.get_origin() + bullet->velocity * delta);

		if(!active_rect.has_point(bullet->transform.get_origin())) {
			// Return true if the bullet should be deleted.
			return true;
		}
		// Rotate the bullet based on its velocity "rotate" is enabled.
		if(kit->rotate) {
			bullet->transform.set_rotation(bullet->velocity.angle());
		}
		// Bullet is still alive, increase its lifetime.
		bullet->lifetime += delta;
		// If bullet should free itself after it's lifetime, do it
		if(kit->free_after_lifetime && bullet->lifetime > kit->lifetime_curves_span) {
			return true;
		}
		// Return false if the bullet should not be deleted yet.
		return false;
	}
};

BULLET_KIT_IMPLEMENTATION(DynamicBulletKit, DynamicBulletsPool)

#endif