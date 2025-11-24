#pragma once
#include "Globals.hpp"
#include "Helper.hpp"
#include "raylib.h"
#include <cmath>

// Calculate pixel value for 180° rotation based on sensitivity
inline void calculateSpeedglitchPixels() {
    float base_value = cam_fix_active ? 500.0f : 360.0f;
    float multiplier = (359.0f / 360.0f) * (359.0f / 360.0f); // Slight adjustment for accuracy
    
    speed_pixels_x = static_cast<int>(std::round((base_value / roblox_sensitivity) * multiplier));
    speed_pixels_y = -speed_pixels_x; // Opposite direction
    
    log("Speedglitch pixels calculated: " + std::to_string(speed_pixels_x));
}

// Main speedglitch loop (runs in separate thread)
inline void speedglitchLoop() {
    int sleep1 = 16, sleep2 = 16;
    int last_fps = 0;
    const float EPSILON = 0.008f; // For floating-point comparison
    
    while (true) {
        // Wait until speedglitch is activated
        while (!speedglitch_active.load(std::memory_order_relaxed)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        // Recalculate delays if FPS changed
        if (last_fps != roblox_fps) {
            float delay_float = 1000.0f / static_cast<float>(roblox_fps);
            int delay_floor = static_cast<int>(delay_float);
            int delay_ceil = delay_floor + 1;
            float fractional = delay_float - delay_floor;
            
            // Distribute delays intelligently
            if (fractional < 0.33f - EPSILON) {
                sleep1 = sleep2 = delay_floor;
            }
            else if (fractional > 0.66f + EPSILON) {
                sleep1 = sleep2 = delay_ceil;
            }
            else {
                sleep1 = delay_floor;
                sleep2 = delay_ceil;
            }
            
            last_fps = roblox_fps;
            log("Speedglitch delays updated: sleep1=" + std::to_string(sleep1) + 
                ", sleep2=" + std::to_string(sleep2));
        }
        
        // Perform the speedglitch rotation
        input.moveMouse(speed_pixels_x, 0);  // Rotate 180° one way
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep1));
        input.moveMouse(speed_pixels_y, 0);  // Rotate 180° back
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep2));
    }
}

// Initialize speedglitch system
inline void initSpeedglitch() {
    calculateSpeedglitchPixels();
    speedglitch_thread = std::thread(speedglitchLoop);
    speedglitch_thread.detach(); // Run independently
    log("Speedglitch system initialized");
}

// Macro function to toggle speedglitch
inline void speedglitchMacro() {
    static bool last_key_state = false;
    bool key_pressed = input.isKeyPressed(Binds["Speedglitch"]);
    
    // Toggle on key press (not hold)
    if (key_pressed && !last_key_state) {
        speedglitch_active = !speedglitch_active.load(std::memory_order_relaxed);
        
        if (speedglitch_active) {
            log("Speedglitch activated");
        } else {
            log("Speedglitch deactivated");
        }
    }
    
    last_key_state = key_pressed;
}

// Alternative: Hold-key version of speedglitch
inline void speedglitchMacroHold() {
    bool key_pressed = input.isKeyPressed(Binds["Speedglitch"]);
    
    if (key_pressed) {
        speedglitch_active = true;
    } else {
        speedglitch_active = false;
    }
}

// Update sensitivity (call this when user changes sensitivity)
inline void updateSpeedglitchSensitivity(float new_sensitivity, bool new_cam_fix) {
    roblox_sensitivity = new_sensitivity;
    cam_fix_active = new_cam_fix;
    calculateSpeedglitchPixels();
}

// Update FPS (call this when user changes FPS setting)
inline void updateSpeedglitchFPS(int new_fps) {
    SetTargetFPS(new_fps);
    roblox_fps = new_fps;
    log("Speedglitch FPS updated to: " + std::to_string(new_fps));
}

// Usage Instructions Comment Block
/*
=============================================================================
SPEEDGLITCH USAGE INSTRUCTIONS:
=============================================================================

REQUIREMENTS:
1. Must have COM offset active (use /e dance2 with item equipped first)
2. Enable shiftlock in Roblox settings
3. Set correct Roblox sensitivity in the macro
4. Set correct FPS value
5. Set cam-fix setting correctly for your game

HOW TO USE:
1. Initialize: Call initSpeedglitch() once at startup
2. Configure: Set roblox_sensitivity, roblox_fps, cam_fix_active
3. Activate: Press your bound key to toggle speedglitch
4. In-game: 
   - Press bound key to start rotation
   - Enable shiftlock (default: Shift key in Roblox)
   - Jump and hold W
   - You will gain massive forward velocity while airborne

SETTINGS EXPLANATION:
- roblox_sensitivity: Your Roblox camera sensitivity (0.1 to 4.0)
- roblox_fps: Your stable Roblox FPS (30, 60, 120, 240, etc.)
- cam_fix_active: Set to true if game uses "cam-fix module"
  (Test: 180° rotation should match your sensitivity calculation)

SENSITIVITY CALCULATION:
Without cam-fix: 180° = 360 / sensitivity pixels
With cam-fix:    180° = 500 / sensitivity pixels

Example: 0.5 sensitivity, no cam-fix = 720 pixels for 180°
         After adjustment: ~716 pixels

TROUBLESHOOTING:
- If rotation is wrong angle: Check cam_fix_active setting
- If timing feels off: Verify your actual Roblox FPS matches setting
- If no speed gain: Make sure COM offset is active and shiftlock is on
- If rotation stutters: Your FPS may be unstable

=============================================================================
*/