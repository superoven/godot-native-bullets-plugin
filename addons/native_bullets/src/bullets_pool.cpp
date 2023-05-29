#include "bullets_pool.h"
#include "bullets.h"

using namespace godot;


BulletsPool::BulletsPool() {}

BulletsPool::~BulletsPool() {}

int32_t BulletsPool::get_available_bullets() {
	return available_bullets;
}

int32_t BulletsPool::get_active_bullets() {
	return active_bullets;
}

Node* BulletsPool::get_bullets_animation(String animation_name) {
	if (bullets_animations.has(animation_name)) {
		return bullets_animations[animation_name];
	}
	return nullptr;
}
