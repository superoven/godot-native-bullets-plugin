#ifndef BULLET_ANIMATION_H
#define BULLET_ANIMATION_H

#include <Godot.hpp>
#include <Transform2D.hpp>
#include <Node.hpp>
#include <Curve.hpp>

#include <limits>

using namespace godot;

const float_t DEFAULT_DURATION = 1.0f;

class BulletsAnimation : public Node {
	GODOT_CLASS(BulletsAnimation, Node)

public:
    float_t duration;
    Ref<Curve> scale_curve;
    Ref<Curve> rotation_degree_curve;
    Ref<Curve> alpha_curve;

    BulletsAnimation() {
        duration = DEFAULT_DURATION;
    }

	void _init() {}

    void _ready() {}

    static void _register_methods() {
        register_property<BulletsAnimation, float_t>("duration", &BulletsAnimation::duration,
            DEFAULT_DURATION, GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT,
            GODOT_PROPERTY_HINT_RANGE, "0.01,300.0,0.01");
        register_property<BulletsAnimation, Ref<Curve>>("scale_curve", &BulletsAnimation::scale_curve,
            Ref<Curve>(), GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT,
            GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Curve");
        register_property<BulletsAnimation, Ref<Curve>>("rotation_degree_curve", &BulletsAnimation::rotation_degree_curve,
            Ref<Curve>(), GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT,
            GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Curve");
        register_property<BulletsAnimation, Ref<Curve>>("alpha_curve", &BulletsAnimation::alpha_curve,
            Ref<Curve>(), GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT,
            GODOT_PROPERTY_HINT_RESOURCE_TYPE, "Curve");
    }
};

#endif