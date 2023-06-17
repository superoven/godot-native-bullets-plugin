#ifndef MUSIC_METER_H
#define MUSIC_METER_H

#include <Godot.hpp>
#include <Node.hpp>

#include <limits>

using namespace godot;


class MusicMeter : public Node {
	GODOT_CLASS(MusicMeter, Node)

    float_t bpm;
    int time_sig_top;
    int time_sig_bottom;

public:
    MusicMeter() {
    }

    MusicMeter(float _bpm, int _time_sig_top, int _time_sig_bottom) {
        bpm = _bpm;
        time_sig_top = _time_sig_top;
        time_sig_bottom = _time_sig_bottom;
    }

    float_t get_seconds_per_measure() {
       return (60.0 * float(time_sig_bottom)) / float(bpm);
    }

   	// virtual float_t get_seconds_per_measure();

    static void _register_methods() {
        register_property<MusicMeter, float>("bpm", &MusicMeter::bpm, 100.0);
        register_property<MusicMeter, int>("time_sig_top", &MusicMeter::time_sig_top, 4);
        register_property<MusicMeter, int>("time_sig_bottom", &MusicMeter::time_sig_bottom, 4);

        register_method("get_seconds_per_measure", &MusicMeter::get_seconds_per_measure);
    }
};

#endif