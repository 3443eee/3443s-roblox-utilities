#include "raylib.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
// Define these BEFORE including Globals.hpp
#define Rectangle Win32Rectangle
#define CloseWindow Win32CloseWindow
#define ShowCursor Win32ShowCursor  
#define DrawText Win32DrawText
#define DrawTextEx Win32DrawTextEx
#define LoadImage Win32LoadImage
#include <windows.h>  // Include Windows headers here with renames active
#undef Rectangle
#undef CloseWindow
#undef ShowCursor
#undef DrawText
#undef DrawTextEx
#undef LoadImage
#endif

#include "Globals.hpp"
#include "procctrl.hpp"
#include "netctrl.hpp"
#include "LagSwitch.hpp"
#include "Macros.hpp"
#include "Helper.hpp"
#include "UserInterface.hpp"
#include "imgui.h"
#include "rlImGui.h"

#include <iostream>
#include <string>
#include <thread>
#include <cstdlib>
#include <cstdio>
#include <map>

int main() {
    // Initialize Raylib window
    InitWindow(500, 400, "3443's Roblox Utilities");

    // No window border for windows :p
#ifdef _WIN32
    SetWindowState(FLAG_WINDOW_UNDECORATED);
#endif

    SetTargetFPS(60);

    //Initializes the user interface.
    initUI();
    
    //Initlializes the ctrl object for netctrl
    
    g_ctrl = &ctrl;

    //Initializes the input object.
    if (!input.init()) {
        std::cerr << "Failed to initialize input system!\n";
        return 1;
    }

#ifdef _WIN32
    roblox_process_name = "notepad.exe";
#else
    roblox_process_name = "sober";
#endif

    //temporary
    kb_layout = 1;

    Vector2 dragOffset = {0};
    bool isDragging = false;

    while (!WindowShouldClose()) {
        freezeMacro();
        laughClip();
        extendedDanceClip();
        LagSwitch();
        
        // Dragging the window for windows.
#ifdef _WIN32
        Vector2 mousePos = GetMousePosition();
        Vector2 windowPos = GetWindowPosition();
        Vector2 mouseScreenPos = {windowPos.x + mousePos.x, windowPos.y + mousePos.y};
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            isDragging = true;
            // Store the offset from window position to mouse in screen coordinates
            dragOffset.x = mouseScreenPos.x - windowPos.x;
            dragOffset.y = mouseScreenPos.y - windowPos.y;
        }
        
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            isDragging = false;
        }
        
        if (isDragging)
        {
            // Set window position based on screen mouse position minus offset
            SetWindowPosition(
                (int)(mouseScreenPos.x - dragOffset.x),
                (int)(mouseScreenPos.y - dragOffset.y)
            );
        }
#endif

        BeginDrawing();
        ClearBackground(DARKGRAY);
        
        // Begin ImGui frame
        rlImGuiBegin();
        
        //Updates the imgui window.
        UpdateUI();
        
        // End ImGui frame
        rlImGuiEnd();
        EndDrawing();
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // no 100% cpu usage
    }

    // Cleanup
    input.cleanup();
    rlImGuiShutdown();
    CloseWindow();
    return 0;
}