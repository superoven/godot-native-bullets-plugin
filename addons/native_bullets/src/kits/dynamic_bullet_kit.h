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
	// float lifetime_curves_span = 1.0f;
	bool lifetime_curves_loop = true;
	bool free_after_lifetime = false;
	Ref<Curve> speed_multiplier_over_lifetime;
	Ref<Curve> rotation_offset_over_lifetime;
	Ref<Curve> alpha_over_lifetime;
	Ref<Curve> red_over_lifetime;

	static void _register_methods() {
		register_property<DynamicBulletKit, Ref<Texture>>("texture", &DynamicBulletKit::texture, Ref<Texture>(), 
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Texture");
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
	}

	// void _disable_bullet(Bullet* bullet); Use default implementation.

	bool _process_bullet(DynamicBullet* bullet, float delta) {
		// Normalize the lifetime value
		float_t adjusted_lifetime = bullet->lifetime / kit->lifetime_curves_span;
		if(kit->lifetime_curves_loop) {
			adjusted_lifetime = fmod(adjusted_lifetime, 1.0f);
		}
		
		// Movement processing
		if(kit->speed_multiplier_over_lifetime.is_valid()) {
			float_t speed_multiplier = kit->speed_multiplier_over_lifetime->interpolate(adjusted_lifetime);
			Vector2 dir_vec = Vector2::RIGHT.rotated(bullet->transform.get_rotation());
			bullet->velocity = (dir_vec) * bullet->starting_speed * speed_multiplier;
		}
		if(kit->rotation_offset_over_lifetime.is_valid()) {
			float_t abs_rotation_offset = kit->rotation_offset_over_lifetime->interpolate(adjusted_lifetime);
			float_t abs_rotation_radians = (M_PI / 180.0) * abs_rotation_offset;
			float_t result = abs_rotation_radians + bullet->starting_transform.get_rotation();
			float_t offset = result - bullet->transform.get_rotation();
			bullet->velocity = bullet->velocity.rotated(offset);
			bullet->transform.set_rotation(result);
		}
		_process_acceleration(bullet, delta);
		bullet->transform.set_origin(bullet->transform.get_origin() + bullet->velocity * delta);
		if(!active_rect.has_point(bullet->transform.get_origin())) {
			// Return true if the bullet should be deleted.
			return true;
		}

		// Animation and Visuals Processing
		if(kit->alpha_over_lifetime.is_valid()) {
			float_t alpha = kit->alpha_over_lifetime->interpolate(adjusted_lifetime);
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
		_process_animation(bullet, delta);

		// Various checks for lifetime and potential cleanup
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