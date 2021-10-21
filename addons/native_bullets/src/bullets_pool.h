#ifndef BULLETS_POOL_H
#define BULLETS_POOL_H

#include <Godot.hpp>
#include <CanvasItem.hpp>
#include <AtlasTexture.hpp>
#include <Material.hpp>
#include <Color.hpp>

#include "bullet.h"
#include "bullet_kit.h"

using namespace godot;

class BulletsPool {
	
protected:
	int32_t* shapes_to_indices = nullptr;
	int32_t available_bullets = 0;
	int32_t active_bullets = 0;
	bool collisions_enabled;

	CanvasItem* canvas_parent;
	RID canvas_item;
	RID shared_area;
	int32_t starting_shape_index;

	Rect2 active_rect;

	template<typename T>
	void _swap(T &a, T &b) {
		T t = a;
		a = b;
		b = t;
	}

public:
	Ref<BulletKit> bullet_kit;
	int32_t pool_size = 0;

	BulletsPool();
	~BulletsPool();

	virtual void _init(Ref<BulletKit> kit, CanvasItem* canvas_parent, int32_t z_index,
		RID shared_area, int32_t starting_shape_index, int32_t pool_size) = 0;
	
	int32_t get_available_bullets();
	int32_t get_active_bullets();

	virtual int32_t _process(float delta) = 0;
	virtual void _draw(Ref<Font> debug_font) = 0;

	virtual void spawn_bullet(Dictionary properties) = 0;
	virtual BulletID obtain_bullet() = 0;
	virtual bool release_bullet(BulletID id) = 0;
	virtual bool is_bullet_valid(BulletID id) = 0;

	virtual BulletID get_bullet_from_shape(int32_t shape_index) = 0;

	virtual void set_bullet_property(BulletID id, String property, Variant value) = 0;
	virtual Variant get_bullet_property(BulletID id, String property) = 0;
};

template <class Kit, class BulletType>
class AbstractBulletsPool : public BulletsPool {

protected:
	Ref<Kit> kit;
	BulletType** bullets = nullptr;

	virtual inline void _init_bullet(BulletType* bullet);
	virtual inline void _enable_bullet(BulletType* bullet);
	virtual inline void _disable_bullet(BulletType* bullet);
	virtual inline bool _process_bullet(BulletType* bullet, float delta);

	inline void _release_bullet(int32_t index);

public:
	AbstractBulletsPool() {}
	~AbstractBulletsPool();

	virtual void _init(Ref<BulletKit> kit, CanvasItem* canvas_parent, int32_t z_index,
		RID shared_area, int32_t starting_shape_index, int32_t pool_size);

	virtual int32_t _process(float delta) override;
	virtual void _draw(Ref<Font> debug_font) override;

	virtual void spawn_bullet(Dictionary properties) override;
	virtual BulletID obtain_bullet() override;
	virtual bool release_bullet(BulletID id) override;
	virtual bool is_bullet_valid(BulletID id) override;

	virtual BulletID get_bullet_from_shape(int32_t shape_index) override;

	virtual void set_bullet_property(BulletID id, String property, Variant value) override;
	virtual Variant get_bullet_property(BulletID id, String property) override;
};

#include "bullets_pool.inl"

#endif