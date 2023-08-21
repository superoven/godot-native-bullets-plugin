#include <VisualServer.hpp>
#include <Physics2DServer.hpp>
#include <World2D.hpp>
#include <Viewport.hpp>
#include <OS.hpp>
#include <Engine.hpp>
#include <Font.hpp>
#include <RegExMatch.hpp>
#include <Mutex.hpp>

#include "bullets.h"

using namespace godot;


void Bullets::_register_methods() {
	register_method("_physics_process", &Bullets::_physics_process);
	register_method("_ready", &Bullets::_ready);

	register_method("mount", &Bullets::mount);
	register_method("unmount", &Bullets::unmount);
	register_method("get_bullets_environment", &Bullets::get_bullets_environment);

	register_method("spawn_bullet", &Bullets::spawn_bullet);
	register_method("release_bullet", &Bullets::release_bullet);

	register_method("is_bullet_valid", &Bullets::is_bullet_valid);
	register_method("is_kit_valid", &Bullets::is_kit_valid);

	register_method("get_available_bullets", &Bullets::get_available_bullets);
	register_method("get_active_bullets", &Bullets::get_active_bullets);
	register_method("get_pool_size", &Bullets::get_pool_size);
	register_method("get_z_index", &Bullets::get_z_index);

	register_method("get_total_available_bullets", &Bullets::get_total_available_bullets);
	register_method("get_total_active_bullets", &Bullets::get_total_active_bullets);

	register_method("is_bullet_existing", &Bullets::is_bullet_existing);
	register_method("get_bullet_from_shape", &Bullets::get_bullet_from_shape);
	register_method("get_kit_from_bullet", &Bullets::get_kit_from_bullet);

	register_method("set_bullet_property", &Bullets::set_bullet_property);
	register_method("get_bullet_property", &Bullets::get_bullet_property);
	register_method("apply_bullet_properties", &Bullets::apply_bullet_properties);

	register_method("apply_bullets_animation", &Bullets::apply_bullets_animation);

	register_method("apply_bullet_properties_to_kit", &Bullets::apply_bullet_properties_to_kit);
	register_method("apply_bullets_animation_to_kit", &Bullets::apply_bullets_animation_to_kit);
	register_method("enable_collisions_to_kit", &Bullets::enable_collisions_to_kit);
}

Bullets::Bullets() { }

Bullets::~Bullets() {
	_clear_rids();
}

void Bullets::_init() {
	available_bullets = 0;
	active_bullets = 0;
	total_bullets = 0;
	invalid_id = PoolIntArray();
	invalid_id.resize(3);
	invalid_id.set(0, -1);
	invalid_id.set(1, -1);
	invalid_id.set(2, -1);
}

void Bullets::_ready() {
	this->set_physics_process(false);
	// Godot::print("Setting physics process off lmao");
	_lock = Mutex();
}

void Bullets::_physics_process(float delta) {
	if(Engine::get_singleton()->is_editor_hint()) {
		return;
	}
	// Godot::print("processing Bullets...");
	// _lock.lock();
	int32_t bullets_variation = 0;

	for(int32_t i = 0; i < pool_sets.size(); i++) {
		for(int32_t j = 0; j < pool_sets[i].pools.size(); j++) {
			bullets_variation = pool_sets[i].pools[j].pool->_process(delta);
			available_bullets -= bullets_variation;
			active_bullets += bullets_variation;
		}
	}
	// _lock.unlock();
}

void Bullets::_clear_rids() {
	for(int32_t i = 0; i < shared_areas.size(); i++) {
		Physics2DServer::get_singleton()->area_clear_shapes(shared_areas[i]);
		Physics2DServer::get_singleton()->free_rid(shared_areas[i]);
	}
}

int32_t Bullets::_get_pool_index(int32_t set_index, int32_t bullet_index) {
	if(bullet_index >= 0 && set_index >= 0 && set_index < pool_sets.size() && bullet_index < pool_sets[set_index].bullets_amount) {
		int32_t pool_threshold = pool_sets[set_index].pools[0].size;
		int32_t pool_index = 0;

		while(bullet_index >= pool_threshold) {
			pool_index++;
			pool_threshold += pool_sets[set_index].pools[pool_index].size;
		}
		if(pool_index < pool_sets[set_index].pools.size()) {
			return pool_index;
		}
	}
	return -1;
}

void Bullets::mount(Node* bullets_environment) {
	// _lock.lock();
	if(bullets_environment == nullptr || this->bullets_environment == bullets_environment) {
		// _lock.unlock();
		return;
	}
	if(this->bullets_environment != nullptr) {
		this->bullets_environment->set("current", false);
	}
	
	this->bullets_environment = bullets_environment;
	this->bullets_environment->set("current", true);

	Array bullet_kits = bullets_environment->get("bullet_kits");
	Array pools_sizes = bullets_environment->get("pools_sizes");
	Array parents_paths = bullets_environment->get("parents_hints");
	Array z_indices = bullets_environment->get("z_indices");

	pool_sets.clear();
	areas_to_pool_set_indices.clear();
	kits_to_set_pool_indices.clear();
	_clear_rids();
	shared_areas.clear();

	available_bullets = 0;
	active_bullets = 0;

	Dictionary collision_layers_masks_to_kits;

	Viewport* default_viewport = bullets_environment->get_viewport();
	RID default_parent_canvas = default_viewport->find_world_2d()->get_canvas();
	
	for(int32_t i = 0; i < bullet_kits.size(); i++) {
		Ref<BulletKit> kit = bullet_kits[i];
		if(!kit->is_valid()) {
			ERR_PRINT("BulletKit is not valid!");
			continue;
		}
		// By default add the the BulletKit to a no-collisions list. (layer and mask = 0)
		int64_t layer_mask = 0;
		if(kit->collisions_enabled && kit->collision_shape.is_valid()) {
			// If collisions are enabled, add the BulletKit to another list.
			layer_mask = (int64_t)kit->collision_layer + ((int64_t)kit->collision_mask << 32);
		}
		if(collision_layers_masks_to_kits.has(layer_mask)) {
			collision_layers_masks_to_kits[layer_mask].operator Array().append(kit);
		}
		else {
			Array array = Array();
			array.append(kit);
			collision_layers_masks_to_kits[layer_mask] = array;
		}
	}
	// Create the PoolKitSets array. If they exist, a set will be allocated for no-collisions pools.
	pool_sets.resize(collision_layers_masks_to_kits.size());
	
	Array layer_mask_keys = collision_layers_masks_to_kits.keys();
	for(int32_t i = 0; i < layer_mask_keys.size(); i++) {
		Array kits = collision_layers_masks_to_kits[layer_mask_keys[i]];
		Ref<BulletKit> first_kit = kits[0];

		pool_sets[i].pools.resize(kits.size());

		RID shared_area = RID();
		if(layer_mask_keys[i].operator int64_t() != 0) {
			// This is a collisions-enabled set, create the shared area.
			shared_area = Physics2DServer::get_singleton()->area_create();
			Physics2DServer::get_singleton()->area_set_collision_layer(shared_area, first_kit->collision_layer);
			Physics2DServer::get_singleton()->area_set_collision_mask(shared_area, first_kit->collision_mask);
			Physics2DServer::get_singleton()->area_set_monitorable(shared_area, true);
			Physics2DServer::get_singleton()->area_set_space(shared_area, get_world_2d()->get_space());

			shared_areas.append(shared_area);
			areas_to_pool_set_indices[shared_area] = i;
		}
		int32_t pool_set_available_bullets = 0;

		for(int32_t j = 0; j < kits.size(); j++) {
			Ref<BulletKit> kit = kits[j];

			PoolIntArray set_pool_indices = PoolIntArray();
			set_pool_indices.resize(2);
			set_pool_indices.set(0, i);
			set_pool_indices.set(1, j);
			kits_to_set_pool_indices[kit] = set_pool_indices;
			
			int32_t kit_index_in_node = bullet_kits.find(kit);
			int32_t pool_size = pools_sizes[kit_index_in_node];

			Node* parent_node_hint = nullptr;

			NodePath parent_path = parents_paths[0];
			if (!parent_path.is_empty()) {
				parent_node_hint = bullets_environment->get_node(parent_path);
			}
			if (parent_node_hint == nullptr) {
				parent_node_hint = bullets_environment;
			}

			pool_sets[i].pools[j].pool = kit->_create_pool();
			pool_sets[i].pools[j].bullet_kit = kit;
			pool_sets[i].pools[j].size = pool_size;
			pool_sets[i].pools[j].z_index = z_indices[kit_index_in_node];

			pool_sets[i].pools[j].pool->_init(parent_node_hint, shared_area, pool_set_available_bullets,
				i, kit, pool_size, z_indices[kit_index_in_node]);

			pool_set_available_bullets += pool_size;
		}
		pool_sets[i].bullets_amount = pool_set_available_bullets;
		available_bullets += pool_set_available_bullets;
	}
	total_bullets = available_bullets;
	// _lock.unlock();
}

void Bullets::unmount(Node* bullets_environment) {
	// _lock.lock();
	if(this->bullets_environment == bullets_environment) {
		pool_sets.clear();
		areas_to_pool_set_indices.clear();
		kits_to_set_pool_indices.clear();
		_clear_rids();
		shared_areas.clear();

		available_bullets = 0;
		active_bullets = 0;
		total_bullets = 0;

		this->bullets_environment = nullptr;
	} else {
		String message = "Unmount: Something went wrong? Bullet Environment was: {0}";
		ERR_PRINT(message.format(Array::make(bullets_environment)));
	}
	// _lock.unlock();
}

Node* Bullets::get_bullets_environment() {
	// CRASH_BAD_INDEX(1, 3);
	CRASH_NOW();
	ERR_PRINT("about to try out the lock");
	Godot::print_error("about to try out the lock", "function", "file", 1);
	// OS::get_singleton()->flush_stdout();
	if (_lock.try_lock() == Error::OK) {
		Godot::print_error("getting the lock lmao", "function", "file", 1);
		Node* be = bullets_environment;
		_lock.unlock();
		return be;
	} else {
		Godot::print_error("couldn't get the lock?", "function", "file", 1);
	}
	return nullptr;
}

Variant Bullets::spawn_bullet(Ref<BulletKit> kit, Dictionary properties) {
	// _lock.lock();
	if(available_bullets > 0 && kits_to_set_pool_indices.has(kit)) {
		PoolIntArray set_pool_indices = kits_to_set_pool_indices[kit].operator PoolIntArray();
		BulletsPool* pool = pool_sets[set_pool_indices[0]].pools[set_pool_indices[1]].pool.get();

		if(pool->get_available_bullets() > 0) {
			available_bullets -= 1;
			active_bullets += 1;

			BulletID bullet_id = pool->spawn_bullet(properties);
			PoolIntArray to_return = invalid_id;
			to_return.set(0, bullet_id.index);
			to_return.set(1, bullet_id.cycle);
			to_return.set(2, bullet_id.set);
			// _lock.unlock();
			return to_return;
		}
	}
	// _lock.unlock();
	return invalid_id;
}

bool Bullets::release_bullet(Variant id) {
	// _lock.lock();
	PoolIntArray bullet_id = id.operator PoolIntArray();
	bool result = false;

	int32_t pool_index = _get_pool_index(bullet_id[2], bullet_id[0]);
	if(pool_index >= 0) {
		result = pool_sets[bullet_id[2]].pools[pool_index].pool->release_bullet(BulletID(bullet_id[0], bullet_id[1], bullet_id[2]));
		if(result) {
			available_bullets += 1;
			active_bullets -= 1;
		}
	}
	// _lock.unlock();
	return result;
}

bool Bullets::is_bullet_valid(Variant id) {
	// _lock.lock();
	PoolIntArray bullet_id = id.operator PoolIntArray();

	int32_t pool_index = _get_pool_index(bullet_id[2], bullet_id[0]);
	if(pool_index >= 0) {
		bool ret = pool_sets[bullet_id[2]].pools[pool_index].pool->is_bullet_valid(BulletID(bullet_id[0], bullet_id[1], bullet_id[2]));
		// _lock.unlock();
		return ret;
	}
	// _lock.unlock();
	return false;
}

bool Bullets::is_kit_valid(Ref<BulletKit> kit) {
	// _lock.lock();
	bool ret = kits_to_set_pool_indices.has(kit);
	// _lock.unlock();
	return ret;
}

int32_t Bullets::get_available_bullets(Ref<BulletKit> kit) {
	// _lock.lock();
	if(kits_to_set_pool_indices.has(kit)) {
		PoolIntArray set_pool_indices = kits_to_set_pool_indices[kit];
		bool ret = pool_sets[set_pool_indices[0]].pools[set_pool_indices[1]].pool->get_available_bullets();
		// _lock.unlock();
		return ret;
	}
	// _lock.unlock();
	return 0;
}

int32_t Bullets::get_active_bullets(Ref<BulletKit> kit) {
	// _lock.lock();
	if(kits_to_set_pool_indices.has(kit)) {
		PoolIntArray set_pool_indices = kits_to_set_pool_indices[kit];
		bool ret = pool_sets[set_pool_indices[0]].pools[set_pool_indices[1]].pool->get_active_bullets();
		// _lock.unlock();
		return ret;
	}
	// _lock.unlock();
	return 0;
}

int32_t Bullets::get_pool_size(Ref<BulletKit> kit) {
	// _lock.lock();
	if(kits_to_set_pool_indices.has(kit)) {
		PoolIntArray set_pool_indices = kits_to_set_pool_indices[kit];
		bool ret = pool_sets[set_pool_indices[0]].pools[set_pool_indices[1]].size;
		// _lock.unlock();
		return ret;
	}
	// _lock.unlock();
	return 0;
}

int32_t Bullets::get_z_index(Ref<BulletKit> kit) {
	// _lock.lock();
	if(kits_to_set_pool_indices.has(kit)) {
		PoolIntArray set_pool_indices = kits_to_set_pool_indices[kit];
		bool ret = pool_sets[set_pool_indices[0]].pools[set_pool_indices[1]].z_index;
		// _lock.unlock();
		return ret;
	}
	// _lock.unlock();
	return 0;
}

int32_t Bullets::get_total_available_bullets() {
	// _lock.lock();
	int32_t ret = available_bullets;
	// _lock.unlock();
	return ret;
}

int32_t Bullets::get_total_active_bullets() {
	// _lock.lock();
	int32_t ret = active_bullets;
	// _lock.unlock();
	return ret;
}

bool Bullets::is_bullet_existing(RID area_rid, int32_t shape_index) {
	// _lock.lock();
	if(!areas_to_pool_set_indices.has(area_rid)) {
		// _lock.unlock();
		return false;
	}
	int32_t set_index = areas_to_pool_set_indices[area_rid];
	int32_t pool_index = _get_pool_index(set_index, shape_index);
	if(pool_index >= 0) {
		bool ret = pool_sets[set_index].pools[pool_index].pool->is_bullet_existing(shape_index);
		// _lock.unlock();
		return ret;
	}
	// _lock.unlock();
	return false;
}

Variant Bullets::get_bullet_from_shape(RID area_rid, int32_t shape_index) {
	// _lock.lock();
	if(!areas_to_pool_set_indices.has(area_rid)) {
		// _lock.unlock();
		return invalid_id;
	}
	int32_t set_index = areas_to_pool_set_indices[area_rid];
	int32_t pool_index = _get_pool_index(set_index, shape_index);
	if(pool_index >= 0) {
		BulletID result = pool_sets[set_index].pools[pool_index].pool->get_bullet_from_shape(shape_index);

		PoolIntArray to_return = invalid_id;
		to_return.set(0, result.index);
		to_return.set(1, result.cycle);
		to_return.set(2, result.set);
		// _lock.unlock();
		return to_return;
	}
	// _lock.unlock();
	return invalid_id;
}

Ref<BulletKit> Bullets::get_kit_from_bullet(Variant id) {
	// _lock.lock();
	PoolIntArray bullet_id = id.operator PoolIntArray();

	int32_t pool_index = _get_pool_index(bullet_id[2], bullet_id[0]);
	if(pool_index >= 0 && pool_sets[bullet_id[2]].pools[pool_index].pool->is_bullet_valid(BulletID(bullet_id[0], bullet_id[1], bullet_id[2]))) {
		Ref<BulletKit> ret = pool_sets[bullet_id[2]].pools[pool_index].bullet_kit;
		// _lock.unlock();
		return ret;
	}
	// _lock.unlock();
	return Ref<BulletKit>();
}

void Bullets::set_bullet_property(Variant id, String property, Variant value) {
	// _lock.lock();
	PoolIntArray bullet_id = id.operator PoolIntArray();

	int32_t pool_index = _get_pool_index(bullet_id[2], bullet_id[0]);
	if(pool_index >= 0) {
		pool_sets[bullet_id[2]].pools[pool_index].pool->set_bullet_property(BulletID(bullet_id[0], bullet_id[1], bullet_id[2]), property, value);
	}
	// _lock.unlock();
}

void Bullets::apply_bullets_animation(Variant id, String animation_name) {
	// _lock.lock();
	PoolIntArray bullet_id = id.operator PoolIntArray();

	int32_t pool_index = _get_pool_index(bullet_id[2], bullet_id[0]);
	if(pool_index >= 0) {
		pool_sets[bullet_id[2]].pools[pool_index].pool->apply_bullets_animation(BulletID(bullet_id[0], bullet_id[1], bullet_id[2]), animation_name);
	}
	// _lock.unlock();
}

void Bullets::apply_bullet_properties(Variant id, Dictionary properties) {
	// _lock.lock();
	PoolIntArray bullet_id = id.operator PoolIntArray();
	int32_t pool_index = _get_pool_index(bullet_id[2], bullet_id[0]);
	if(pool_index >= 0) {
		pool_sets[bullet_id[2]].pools[pool_index].pool->apply_bullet_properties(BulletID(bullet_id[0], bullet_id[1], bullet_id[2]), properties);
	}
	// _lock.unlock();
}

void Bullets::apply_bullet_properties_to_kit(Ref<BulletKit> kit, Dictionary properties) {
	// _lock.lock();
	if(kits_to_set_pool_indices.has(kit)) {
		PoolIntArray set_pool_indices = kits_to_set_pool_indices[kit];
		pool_sets[set_pool_indices[0]].pools[set_pool_indices[1]].pool->apply_all(properties);
	}
	// _lock.unlock();
}

void Bullets::apply_bullets_animation_to_kit(Ref<BulletKit> kit, String animation_name) {
	// _lock.lock();
	if(kits_to_set_pool_indices.has(kit)) {
		PoolIntArray set_pool_indices = kits_to_set_pool_indices[kit];
		pool_sets[set_pool_indices[0]].pools[set_pool_indices[1]].pool->apply_bullets_animation_to_all(animation_name);
	}
	// _lock.unlock();
}

void Bullets::enable_collisions_to_kit(Ref<BulletKit> kit, bool enabled) {
	// _lock.lock();
	if(kits_to_set_pool_indices.has(kit)) {
		PoolIntArray set_pool_indices = kits_to_set_pool_indices[kit];
		pool_sets[set_pool_indices[0]].pools[set_pool_indices[1]].pool->enable_collisions(enabled);
	}
	// _lock.unlock();
}

Variant Bullets::get_bullet_property(Variant id, String property) {
	// _lock.lock();
	PoolIntArray bullet_id = id.operator PoolIntArray();

	int32_t pool_index = _get_pool_index(bullet_id[2], bullet_id[0]);
	if(pool_index >= 0) {
		Variant ret = pool_sets[bullet_id[2]].pools[pool_index].pool->get_bullet_property(BulletID(bullet_id[0], bullet_id[1], bullet_id[2]), property);
		// _lock.unlock();
		return ret;
	}
	// _lock.unlock();
	return Variant();
}
