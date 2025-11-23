#pragma once
#include <string>
#include <map>
#include "netctrl.hpp"
#include "inpctrl.hpp"

inline netctrl::NetCtrl* g_ctrl = nullptr;
inline netctrl::NetCtrl ctrl;
inline std::string roblox_process_name;
inline unsigned short kb_layout;

inline CrossInput input;

// Debounces
inline bool events[5] {
    false, // Freeze
    false, // Laugh clip
    false, // E-Dance clip
    false, // For binding/changing keys
    false, // For lag switch
};

inline bool enabled[5] {
    true, // Freeze
    true, // Laugh clip
    true, // E-Dance clip
    false, // align
    true, // For lag switch
};

inline std::map<std::string, CrossInput::Key> Binds = {
    {"Freeze", CrossInput::Key::F1}, // Freeze
    {"Laugh", CrossInput::Key::F2}, // Laugh clip
    {"E-Dance", CrossInput::Key::F3}, // Extended Dance
    {"Lag-switch", CrossInput::Key::L} // Lag switch
};