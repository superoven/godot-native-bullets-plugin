#ifndef BULLET_KIT_H
#define BULLET_KIT_H

#include <Godot.hpp>
#include <Resource.hpp>
#include <Shape2D.hpp>
#include <Material.hpp>
#include <Texture.hpp>
#include <PackedScene.hpp>
#include <Script.hpp>

#include "bullet.h"

#define BULLET_KIT(BulletPoolType)				\
BulletsPool* _create_pool() override;

#define BULLET_KIT_REGISTRATION(BulletKitType, BulletType)											\
register_property<BulletKitType, String>("bullet_class_name",										\
	&BulletKitType::_property_setter, &BulletKitType::_property_getter, #BulletType,			\
	GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_NOEDITOR);

#define BULLET_KIT_IMPLEMENTATION(BulletKitType, BulletPoolType)					\
BulletsPool* BulletKitType::_create_pool() {												\
	return new BulletPoolType();																\
}																										

using namespace godot;

class BulletsPool;

class BulletKit : public Resource {
	GODOT_CLASS(BulletKit, Resource)

public:
	// The material used to render each bullet.
	Ref<Material> material;
	// Controls whether collisions with other objects are enabled. Turning it off increases performances.
	bool collisions_enabled = true;
	Ref<Shape2D> collision_shape;
	// Controls whether the active rect is automatically set as the viewport visible rect.
	bool use_viewport_as_active_rect = true;
	// Controls where the bullets can live, if a bullet exits this rect, it will be removed.
	Rect2 active_rect;
	// If enabled, bullets will auto-rotate based on their direction of travel.
	bool rotate = false;
	// Allows the ability to have a unique-ish value in each instance of the bullet material.
	// Can be used to offset the bullets animation by a unique amount to avoid having them animate in sync.
	int32_t unique_modulate_component = 0;
	// Additional data the user can set via the editor.
	Variant data;

	void _init() {}

	void _property_setter(String value) {}
	String _property_getter() { return ""; }

	static void _register_methods() {
		register_property<BulletKit, Ref<Material>>("material", &BulletKit::material,
			Ref<Material>(), GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT,
			GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Material");
		register_property<BulletKit, bool>("collisions_enabled", &BulletKit::collisions_enabled, true,
			GODOT_METHOD_RPC_MODE_DISABLED, (godot_property_usage_flags)(GODOT_PROPERTY_USAGE_DEFAULT | GODOT_PROPERTY_USAGE_UPDATE_ALL_IF_MODIFIED),
			GODOT_PROPERTY_HINT_NONE);
		register_property<BulletKit, Ref<Shape2D>>("collision_shape", &BulletKit::collision_shape,
			Ref<Shape2D>(), GODOT_METHOD_RPC_MODE_DISABLED,GODOT_PROPERTY_USAGE_DEFAULT,
			GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Shape2D");
		register_property<BulletKit, bool>("use_viewport_as_active_rect", &BulletKit::use_viewport_as_active_rect, true,
			GODOT_METHOD_RPC_MODE_DISABLED, (godot_property_usage_flags)(GODOT_PROPERTY_USAGE_DEFAULT | GODOT_PROPERTY_USAGE_UPDATE_ALL_IF_MODIFIED),
			GODOT_PROPERTY_HINT_NONE);
		register_property<BulletKit, Rect2>("active_rect", &BulletKit::active_rect, Rect2(),
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_NONE);
		register_property<BulletKit, bool>("rotate", &BulletKit::rotate, false);
		register_property<BulletKit, int32_t>("unique_modulate_component", &BulletKit::unique_modulate_component, 0,
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_ENUM, "None,Red,Green,Blue,Alpha");
		register_property<BulletKit, Variant>("data", &BulletKit::data, Dictionary(),
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_NONE);
		
		register_property<BulletKit, String>("bullet_class_name",
			&BulletKit::_property_setter, &BulletKit::_property_getter, "",
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_NOEDITOR);
		register_property<BulletKit, String>("bullet_properties",
			&BulletKit::_property_setter, &BulletKit::_property_getter, "",
			GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_EDITOR);
	}

	virtual BulletsPool* _create_pool() { return nullptr; }
};

#include "bullets_pool.h"

#endif