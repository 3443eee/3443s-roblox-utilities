#include "raylib.h"
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define Rectangle Win32Rectangle
    #define CloseWindow Win32CloseWindow
    #define ShowCursor Win32ShowCursor  
    #define DrawText Win32DrawText
    #define DrawTextEx Win32DrawTextEx
    #define LoadImage Win32LoadImage
#endif

#include "procctrl.hpp"
#include "imgui.h"
#include "rlImGui.h"
#include "inpctrl.hpp"

#ifdef _WIN32
    #undef Rectangle
    #undef CloseWindow
    #undef ShowCursor
    #undef DrawText
    #undef DrawTextEx
    #undef LoadImage
#endif

#include <iostream>
#include <string>
#include <thread>
#include <cstdlib>
#include <cstdio>
#include <map>

CrossInput input;

std::string roblox_process_name;
const char *layout_name;

unsigned short kb_layout;

void log(std::string text) {
    std::cout << "[3RU] " << text << std::endl;
}

bool events[4] {
    false, // Freeze
    false, // Laugh clip
    false, // E-Dance clip
    false, // For binding/changing keys
};

std::map<std::string, CrossInput::Key> Binds = {
    {"Freeze", CrossInput::Key::F1}, // Freeze
    {"Laugh", CrossInput::Key::F2}, // Laugh clip
    {"E-Dance", CrossInput::Key::F3} // Extended Dance
};

void bindToMacro(std::string macro_name) {
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

void typeSlash() {
    input.holdKey(CrossInput::Key::LShift);
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    
    input.holdKey(CrossInput::Key::Dot);
    std::this_thread::sleep_for(std::chrono::milliseconds(65));
    
    input.releaseKey(CrossInput::Key::Dot);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    
    input.releaseKey(CrossInput::Key::LShift);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

void freezeMacro() {
    bool key_pressed = input.isKeyPressed(Binds["Freeze"]);

    if (key_pressed && !events[0]) {
        log("Freeze triggered for " + roblox_process_name);
        procctrl::suspend_processes_by_name(roblox_process_name);
    }

    if (!key_pressed && events[0]) {
        log("Unfreeze triggered for " + roblox_process_name);
        procctrl::resume_processes_by_name(roblox_process_name);
    }
    events[0] = key_pressed;
}

void laughClip() {
    bool key_pressed = input.isKeyPressed(Binds["Laugh"]);
    if (key_pressed && !events[1]) {
        events[1] = true;
        log("Laugh clip triggered");

        input.pressKey(CrossInput::Key::Slash);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (kb_layout == 1) {
            typeSlash();
            input.typeText("e lqugh");
            input.pressKey(CrossInput::Key::Enter);
        } else {
            input.typeText("/e laugh");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(248));
        input.holdKey(CrossInput::Key::S);
        procctrl::suspend_processes_by_name(roblox_process_name);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        procctrl::resume_processes_by_name(roblox_process_name);

        input.holdKey(CrossInput::Key::Space);
        input.holdKey(CrossInput::Key::LShift);

        std::this_thread::sleep_for(std::chrono::milliseconds(35));

        input.releaseKey(CrossInput::Key::Space);
        input.releaseKey(CrossInput::Key::LShift);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(40));

        input.releaseKey(CrossInput::Key::S);

        events[1] = false;
        log("Laugh clip finished");
    }
}

void extendedDanceClip() {
    bool key_pressed = input.isKeyPressed(Binds["E-Dance"]);
    if (key_pressed && !events[2]) {
        events[2] = true;
        log("Extended Dance clip triggered");

        input.pressKey(CrossInput::Key::Slash);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (kb_layout == 1) {
            typeSlash();
            input.typeText("e dqnce");
            input.holdKey(CrossInput::Key::LShift);
            input.pressKey(CrossInput::Key::Num2);
            input.releaseKey(CrossInput::Key::LShift);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            input.pressKey(CrossInput::Key::Enter);
        } else {
            input.typeText("/e dance2");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(815));

        input.holdKey(CrossInput::Key::D);
        procctrl::suspend_processes_by_name(roblox_process_name);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        procctrl::resume_processes_by_name(roblox_process_name);

        input.pressKey(CrossInput::Key::LShift);

        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        input.releaseKey(CrossInput::Key::D);

        events[2] = false;
        log("Extended Dance clip finished");
    }
}

std::string current_option = "0";
std::string current_top_panel = "Macros";


int main() {
    // Initialize Raylib window
    InitWindow(500, 400, "3443's Roblox Utilities");
    SetWindowState(FLAG_WINDOW_UNDECORATED);
    SetTargetFPS(60);

    // Initialize rlImGui
    rlImGuiSetup(true);
    ImGuiStyle& style = ImGui::GetStyle();

    // Make all buttons red
    style.Colors[ImGuiCol_Button] = ImVec4(0.8f, 0.1f, 0.1f, 1.0f);       // normal
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.2f, 0.2f, 1.0f); // hover
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.7f, 0.0f, 0.0f, 1.0f);  // pressed

    // Tab colors (make them red like your buttons)
    style.Colors[ImGuiCol_Tab] = ImVec4(0.8f, 0.1f, 0.1f, 1.0f);              // inactive tab
    style.Colors[ImGuiCol_TabHovered] = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);       // tab on hover
    style.Colors[ImGuiCol_TabActive] = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);        // active/selected tab
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.6f, 0.1f, 0.1f, 1.0f);     // inactive when window unfocused
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.8f, 0.2f, 0.2f, 1.0f); // active when window unfocused

    // Make tabs square (remove rounding)
    style.TabRounding = 0.0f;
    if (!input.init()) {
        std::cerr << "Failed to initialize input system!\n";
        return 1;
    }

    kb_layout = 1;
#ifdef _WIN32
    roblox_process_name = "notepad.exe";
#else
    roblox_process_name = "sober";
#endif

    Vector2 dragOffset = {0};
    bool isDragging = false;

    while (!WindowShouldClose()) {
        freezeMacro();
        laughClip();
        extendedDanceClip();
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
        BeginDrawing();
        ClearBackground(DARKGRAY);
        
        // Begin ImGui frame
        rlImGuiBegin();
        
        // Fullscreen window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(GetScreenWidth(), GetScreenHeight()));
        ImGui::Begin("3443's Roblox Utilities", nullptr,
                    ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoCollapse);
        ImGui::Text("3443's Roblox Utilities");
        ImGui::Separator();
        // Tab Bar
        if (ImGui::BeginTabBar("MainTabBar")) {
            
            // MACRO TAB
            if (ImGui::BeginTabItem("Macros")) {
                
                // ---------- LAYOUT ----------
                ImVec2 window_size = ImGui::GetContentRegionAvail();
                float left_width = 200.0f;
                
                // LEFT PANEL: scrollable buttons
                ImGui::BeginChild("Left Panel", ImVec2(left_width, window_size.y), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
                
                // Helper lambda for buttons with expandable frame
                auto DrawOptionButton = [&](const char* label) {
                    if (ImGui::Button(label, ImVec2(-1, 20))) {
                        // Toggle panel visibility
                        if (current_option == label)
                            current_option = "";
                        else
                            current_option = label;
                    }
                    if (current_option == label) {
                        ImGui::BeginChild(std::string(label + std::string("_frame")).c_str(), ImVec2(0, 100), true);
                        ImGui::TextWrapped("Keybind: ");
                        // keybind settings inside frame
                        if (std::string(label) == "Freeze") {
                            ImGui::Text("Current keybind %s", input.getKeyName(Binds["Freeze"]).c_str());
                            if (ImGui::Button("Change", ImVec2(-1, 20.0f))) {
                                bindToMacro("Freeze");
                            }
                        } else if (std::string(label) == "Laugh Clip") {
                            ImGui::Text("Current keybind %s", input.getKeyName(Binds["Laugh"]).c_str());
                            if (ImGui::Button("Change", ImVec2(-1, 20.0f))) {
                                bindToMacro("Laugh");
                            }
                        } else if (std::string(label) == "Extended Dance Clip") {
                            ImGui::Text("Current keybind %s", input.getKeyName(Binds["E-Dance"]).c_str());
                            if (ImGui::Button("Change", ImVec2(-1, 20.0f))) {
                                bindToMacro("E-Dance");
                            }
                        }
                        ImGui::EndChild();
                    }
                };
                
                // Draw buttons
                DrawOptionButton("Freeze");
                DrawOptionButton("Laugh Clip");
                DrawOptionButton("Extended Dance Clip");
                
                ImGui::EndChild();
                
                // RIGHT PANEL: Use 0,0 to auto-fill remaining space
                ImGui::SameLine();
                ImGui::BeginChild("Right Panel", ImVec2(0, 0), true);
                if (current_option == "Freeze") {
                    ImGui::Text("Freeze information:");
                    ImGui::TextWrapped("This macro freezes the roblox/sober process.\nIt allows for some pretty cool glitches.");
                } else if (current_option == "Laugh Clip") {
                    ImGui::Text("Laugh Clip information:");
                    ImGui::TextWrapped("This macro allows you to clip through walls of 1+ studs of thickness.");
                } else if (current_option == "Extended Dance Clip") {
                    ImGui::Text("Extended Dance Clip information:");
                    ImGui::TextWrapped("This macro allows you to clip through walls of 1+ studs of thickness.\nPlease set up the camera like below:"); 
                }
                ImGui::EndChild();
                
                ImGui::EndTabItem();
            }
            
            // Settings
            if (ImGui::BeginTabItem("Lag-switch")) {
                ImGui::Text("Settings content here");
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Setiings")) {
                ImGui::Text("Settings content here");
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
        
        ImGui::End();
        
        // End ImGui frame
        rlImGuiEnd();
        EndDrawing();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Cleanup
    input.cleanup();
    rlImGuiShutdown();
    CloseWindow();
    return 0;
}