// #ifndef MUSIC_UTILS_H
// #define MUSIC_UTILS_H

// #include "music_meter.h"

// #include <Godot.hpp>
// #include <Reference.hpp>

// using namespace godot;

// class MusicUtils : public Reference {
//     GODOT_CLASS(MusicUtils, Reference)

// // public:

// private:
//     Dictionary notes;
//     // Ref<MusicMeter> _meter = nullptr;
//     MusicMeter* _meter = nullptr;

// public:
//     enum NoteType {
//         WHOLE,
//         HALF,
//         THIRD,
//         QUARTER,
//         SIXTH,
//         EIGHTH,
//         TWELFTH,
//         SIXTEENTH,
//         TWENTYFOURTH,
//         THIRTYSECOND,
//         FOURTYEIGHTH,
//         SIXTYFOURTH,
//         NINETYSIXTH,
//         ONETWENTYEIGHTH
//     };

//     static void _register_methods() {
//         // register_property<MusicUtils, Ref<MusicMeter>>("meter", &MusicUtils::set_meter, &MusicUtils::get_meter);
//         register_method("get_note_time_seconds", &MusicUtils::get_note_time_seconds);
//         register_method("get_events_per_second", &MusicUtils::get_events_per_second);
//         register_method("set_meter", &MusicUtils::set_meter);
//         register_method("get_meter", &MusicUtils::get_meter);
//     }

//     MusicUtils() {}
//     ~MusicUtils() {}
    
//     void _init() {
//         notes[NoteType::WHOLE] = 1.0;
//         notes[NoteType::HALF] = 1.0 / 2.0;
//         notes[NoteType::THIRD] = 1.0 / 3.0;
//         notes[NoteType::QUARTER] = 1.0 / 4.0;
//         notes[NoteType::SIXTH] = 1.0 / 6.0;
//         notes[NoteType::EIGHTH] = 1.0 / 8.0;
//         notes[NoteType::TWELFTH] = 1.0 / 12.0;
//         notes[NoteType::SIXTEENTH] = 1.0 / 16.0;
//         notes[NoteType::TWENTYFOURTH] = 1.0 / 24.0;
//         notes[NoteType::THIRTYSECOND] = 1.0 / 32.0;
//         notes[NoteType::FOURTYEIGHTH] = 1.0 / 48.0;
//         notes[NoteType::SIXTYFOURTH] = 1.0 / 64.0;
//         notes[NoteType::NINETYSIXTH] = 1.0 / 96.0;
//         notes[NoteType::ONETWENTYEIGHTH] = 1.0 / 128.0;
//         _meter = MusicMeter::_new();
//         _meter->init(100.0, 4, 4);
//     }

//     MusicMeter* get_meter() const {
//         return _meter;
//     }

//     void set_meter(MusicMeter* p_meter) {
//         _meter = p_meter;
//     }

//     float get_note_time_seconds(int note_type, int note_count) {
//         MusicMeter* p_meter = get_meter();
//         float sec_per_measure = p_meter->get_seconds_per_measure();
//         NoteType note = static_cast<NoteType>(note_type);
//         return sec_per_measure * float(notes[note]) * note_count;
//     }

//     float get_events_per_second(int note_type, int note_count) {
//         float note_time_seconds = get_note_time_seconds(note_type, note_count);
//         return 1.0f / note_time_seconds;
//     }
// };

// #endif

// TODO: Figure out what to do about the enums not populating properly in 3.x nativescript
