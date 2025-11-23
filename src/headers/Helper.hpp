#pragma once
#include <iostream>
#include <string>

#include "Globals.hpp"
#include "inpctrl.hpp"

inline void log(std::string text) {
    std::cout << "[3RU] " << text << std::endl;
}

inline void typeSlash() {
    input.holdKey(CrossInput::Key::LShift);
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    
    input.holdKey(CrossInput::Key::Dot);
    std::this_thread::sleep_for(std::chrono::milliseconds(65));
    
    input.releaseKey(CrossInput::Key::Dot);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    
    input.releaseKey(CrossInput::Key::LShift);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

inline void bindToMacro(std::string macro_name) {
    if (!events[3]) { 
        events[3] = true;
        CrossInput::Key userKey = input.getCurrentPressedKey(5000); // 5 sec timeout
        if (userKey != static_cast<CrossInput::Key>(0)) {
            std::cout << "[3RU] [procctrl] Bound: " << input.getKeyName(userKey) << std::endl;
            Binds[macro_name] = userKey;
        }
        events[3] = false;
    }
}

inline unsigned short GetIDFromCodeName(std::string CodeName) {
    if (CodeName == "Freeze") {
        return 0;
    } else if (CodeName == "Laugh") {
        return 1;
    } else if (CodeName == "E-Dance") {
        return 2;
    } else if (CodeName == "Lag-switch") {
        return 4;
    } else return 2000;
}