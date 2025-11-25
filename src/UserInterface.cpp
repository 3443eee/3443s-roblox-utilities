#include "UserInterface.hpp"
#include "raylib.h"
#include "LoadTextures.hpp"
#include "imgui.h"
#include "rlImGui.h"
#include "Globals.hpp"
#include "inpctrl.hpp"
#include "Helper.hpp"
#include "LagSwitch.hpp"
#include "Speedglitch.hpp"

ImVec4 orange = ImVec4(1.0f, 0.55f, 0.1f, 1.0f);

ImVec4 HSVtoRGB(float h, float s, float v) {
    float r, g, b;
    
    int i = (int)(h * 6.0f);
    float f = h * 6.0f - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);
    
    switch (i % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    
    return ImVec4(r, g, b, 1.0f);
}

void applyThemeColor(const ImVec4& color) {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // --- Buttons ---
    style.Colors[ImGuiCol_Button] = color;
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(color.x * 1.25f, color.y * 2.0f, color.z * 2.0f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(color.x * 0.7f, color.y * 0.7f, color.z * 0.7f, 1.0f);
    
    // --- Tabs ---
    style.Colors[ImGuiCol_Tab] = color;
    style.Colors[ImGuiCol_TabHovered] = ImVec4(color.x * 1.25f, color.y * 2.0f, color.z * 2.0f, 1.0f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(color.x * 1.25f, color.y * 3.0f, color.z * 3.0f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(color.x * 0.75f, color.y * 1.0f, color.z * 1.0f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = color;
    style.TabRounding = 0.0f;
    
    // --- Checkboxes ---
    style.Colors[ImGuiCol_CheckMark] = ImVec4(color.x * 1.25f, color.y * 2.0f, color.z * 2.0f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(color.x * 0.75f, color.y * 1.0f, color.z * 1.0f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = color;
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(color.x * 1.25f, color.y * 2.0f, color.z * 2.0f, 1.0f);
    
    // --- Sliders ---
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(color.x * 1.25f, color.y * 2.0f, color.z * 2.0f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(color.x * 1.25f, color.y * 3.0f, color.z * 3.0f, 1.0f);
    style.GrabMinSize = 8.0f;
    
    // --- Scrollbars ---
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(color.x * 0.25f, color.y * 0.5f, color.z * 0.5f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrab] = color;
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(color.x * 1.25f, color.y * 2.0f, color.z * 2.0f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(color.x * 1.25f, color.y * 3.0f, color.z * 3.0f, 1.0f);
    
    // --- Combo / selectable items ---
    style.Colors[ImGuiCol_Header] = color;
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(color.x * 1.25f, color.y * 2.0f, color.z * 2.0f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(color.x * 0.875f, color.y * 0.0f, color.z * 0.0f, 1.0f);
    
    style.ScrollbarRounding = 0.0f;
    style.ScrollbarSize = 12.0f;
}

void initUI() {
    // Initialize rlImGui
    
    rlImGuiSetup(true);
    applyThemeColor(themeColor);
}

void UpdateUI() {
    //rainbow theme
    if (rainbowThemeEnabled) {
        // Update hue
        rainbowHue += rainbowSpeed;
        if (rainbowHue > 1.0f) rainbowHue = 0.0f;

        // Convert HSV -> RGB
        themeColor = HSVtoRGB(rainbowHue, rainbowSaturation, rainbowValue);
        applyThemeColor(themeColor);
    }

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
    if (!is_elevated) {
#ifdef _WIN32
        ImGui::TextWrapped(
            "This program needs administrator privileges to run.\n"
            "Please run this program as administrator."
        );
        if (ImGui::Button("Exit", ImVec2(80.0f, 20.0f))) {
            exit(0);
        }
        ImGui::End();
        return;
#else
        ImGui::TextWrapped(
            "This program needs administrator privileges to run.\n"
            "Please enter your password to elevate:"
        );

        ImGui::Spacing();

        ImGui::InputText("##password", passwordBuffer, sizeof(passwordBuffer),
                        ImGuiInputTextFlags_Password);

        ImGui::Spacing();

        if (ImGui::Button("Elevate", ImVec2(80.0f, 20.0f)))
        {
            if (!TryElevate(passwordBuffer))
                elevationFailed = true;
        }

        if (elevationFailed)
        {
            ImGui::Spacing();
            ImGui::TextColored(
                ImVec4(1.0f, 0.2f, 0.2f, 1.0f),
                "Elevation failed. Incorrect password?"
            );
        }

        ImGui::End();
        return;
#endif   
    }
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
                std::string CodeName;

                if (std::string(label) == "Freeze") {
                    CodeName = "Freeze";
                } else if (std::string(label) == "Extended Laugh Clip") {
                    CodeName = "Laugh";
                } else if (std::string(label) == "Extended Dance Clip") {
                    CodeName = "E-Dance";
                }  else if (std::string(label) == "Buckey clip") {
                    CodeName = "Buckey-clip";
                } else if (std::string(label) == "Speed glitch") {
                    CodeName = "Speedglitch";
                } else if (std::string(label) == "Spam key") {
                    CodeName = "Spam-Key";
                }
                // Determine if this option is enabled
                bool isEnabled = enabled[GetIDFromCodeName(CodeName)];

                // Set button colors based on enabled state
                if (isEnabled) {
                    ImGui::PushStyleColor(ImGuiCol_Button, themeColor);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(themeColor.x * 1.25f, themeColor.y * 2.0f, themeColor.z * 2.0f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(themeColor.x * 0.7f, themeColor.y * 0.7f, themeColor.z * 0.7f, 1.0f)); // Changed this line
                } else {
                    // Dimmed when disabled (roughly 40-50% of original brightness)
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(themeColor.x * 0.44f, themeColor.y * 0.7f, themeColor.z * 0.7f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(themeColor.x * 0.56f, themeColor.y * 1.0f, themeColor.z * 1.0f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(themeColor.x * 0.375f, themeColor.y * 0.5f, themeColor.z * 0.5f, 1.0f));
                }


                if (ImGui::Button(label, ImVec2(-1, 20))) {
                    // Toggle panel visibility
                    if (current_option == label)
                        current_option = "";
                    else
                        current_option = label;
                }

                ImGui::PopStyleColor(3); // restore colors

                // Right-click toggle
                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                    enabled[GetIDFromCodeName(CodeName)] = !enabled[GetIDFromCodeName(CodeName)];
                }

                if (current_option == label) {
                    if (CodeName == "Spam-Key") {
                        ImGui::BeginChild(std::string(label + std::string("_frame")).c_str(), ImVec2(0, 120), true);
                    } else {
                        ImGui::BeginChild(std::string(label + std::string("_frame")).c_str(), ImVec2(0, 80), true);
                    }

                    ImGui::TextWrapped("Enabled: %s", enabled[GetIDFromCodeName(CodeName)] ? "Yes" : "No");
                    ImGui::Separator();
                    ImGui::Text("Current keybind %s", input.getKeyName(Binds[CodeName]).c_str());
                    if (ImGui::Button("Change", ImVec2(-1, 20.0f))) {
                        bindToMacro(CodeName);
                    }
                    if (CodeName == "Spam-Key") {
                        ImGui::Text("Current spam key %s", input.getKeyName(SpamKey).c_str());
                        if (ImGui::Button("Bind spam key", ImVec2(-1, 20.0f))) {
                            BindSpamKey();
                        }
                    }
                    ImGui::EndChild();
                }
            };

            // Draw buttons
            DrawOptionButton("Freeze");
            DrawOptionButton("Extended Laugh Clip");
            DrawOptionButton("Extended Dance Clip");
            DrawOptionButton("Buckey clip");
            DrawOptionButton("Speed glitch");
            DrawOptionButton("Spam key");

            ImGui::EndChild();

            // RIGHT PANEL: Use 0,0 to auto-fill remaining space
            ImGui::SameLine();
            ImGui::BeginChild("Right Panel", ImVec2(0, 0), true);
            if (current_option == "Freeze") {
                ImGui::Text("Freeze information:");
                ImGui::Separator();
                ImGui::TextWrapped("This macro freezes the roblox/sober process.\nIt allows for some pretty cool glitches.\n\nHere's a list of some glitches you can use it with.\n*All types of Lag high jumps\n*Extended dance/laugh clips\n*Cheer head glide\netc..");
            } else if (current_option == "Extended Laugh Clip") {
                ImGui::Text("Extended Laugh Clip information:");
                ImGui::Separator();
                ImGui::TextWrapped("This macro allows you to clip through walls of 1+ studs of thickness.\n\nTo use this macro, please set up yourself and the camera like below:\n");
                float windowWidth = ImGui::GetContentRegionAvail().x;
                float imageWidth = 248.0f;
                float offset = (windowWidth - imageWidth) * 0.5f;

                if (offset > 0.0f)
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

                rlImGuiImageSize(&LoadedTextures[1], 248, 140);
                ImGui::TextWrapped("After this, it's pretty straightforward, just trigger the macro.\nJust remember, higher fps is better!\n");
            } else if (current_option == "Extended Dance Clip") {
                ImGui::Text("Extended Dance Clip information:");
                ImGui::Separator();
                ImGui::TextWrapped("This macro allows you to clip through walls of 1+ studs of thickness.\n\nTo use this macro, please set up yourself and the camera like below:\n");
                float windowWidth = ImGui::GetContentRegionAvail().x;
                float imageWidth = 248.0f;
                float offset = (windowWidth - imageWidth) * 0.5f;

                if (offset > 0.0f)
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

                rlImGuiImageSize(&LoadedTextures[0], 248, 140);
                ImGui::TextWrapped("After this, it's pretty straightforward, just trigger the macro.\nYou might not get it the first attempt, try messing with the distance between you and the wall.\n");
            } else if (current_option == "Buckey clip") {
                ImGui::Text("Buckey Clip information:");
                ImGui::Separator();
                ImGui::TextWrapped("This macro allows you to clip through walls of 4+ studs of thickness.\n\nTo use this macro, please set up yourself and the camera like below:\n");
                float windowWidth = ImGui::GetContentRegionAvail().x;
                float imageWidth = 248.0f;
                float offset = (windowWidth - imageWidth) * 0.5f;

                if (offset > 0.0f)
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

                rlImGuiImageSize(&LoadedTextures[2], 248, 140);
                ImGui::TextWrapped("After this, it's pretty straightforward, just trigger the macro! And you should clip through.");
            } else if (current_option == "Speed glitch") {
                ImGui::Text("Speed glitch information:");
                ImGui::Separator();
                ImGui::TextWrapped("This macro allows you to travel EXTREMELY fast! (800+ studs/s)");
                ImGui::TextWrapped("I won't go too much into details, look at this video to figure out how it works:");
                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x); // Fill horizontally

                static char url[] = "https://www.youtube.com/watch?v=5rmeivUegHc";
                ImGui::InputText("##url", url, sizeof(url), ImGuiInputTextFlags_ReadOnly);

                ImGui::PopItemWidth();

                ImGui::Spacing();
                ImGui::TextColored(orange,"Also, if you're willing to use this,\nPLEASE look in the 'Roblox specific'\nsection in settings.");
            } else if (current_option == "Spam key") {
                ImGui::Text("Spam key information:");
                ImGui::Separator();
                ImGui::TextWrapped("This macro allows you to spam a key in your keyboard very fast. You can use it to gear clip for example.\n\nTo gear clip, please set up yourself and the camera like below:\n");
                float windowWidth = ImGui::GetContentRegionAvail().x;
                float imageWidth = 248.0f;
                float offset = (windowWidth - imageWidth) * 0.5f;

                if (offset > 0.0f)
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

                rlImGuiImageSize(&LoadedTextures[3], 248, 140);
                ImGui::TextWrapped("After this, it's pretty straightforward, just trigger the macro and hold W! This glitch can be rng, but you'll get it, especially with low fps.");
            } else {
                ImGui::Text("Welcome to 3443's roblox utilities!");
                ImGui::Separator();
                ImGui::TextWrapped("↓ Here are some things to know!");
                ImGui::TextColored(orange, "/ Right click to toggle macro");
                ImGui::TextColored(orange, "/ Left click to view macro info");
                ImGui::TextWrapped("Check out the Settings tab, it might be useful to you.");
            }
            ImGui::EndChild();
            
            ImGui::EndTabItem();
        }
        
        // Settings
        if (ImGui::BeginTabItem("Lag-switch")) {
            ImGui::Text("Lag switch settings:");
            ImGui::Separator();
            ImGui::Checkbox("Enabled", &enabled[4]);
            ImGui::Text("Current keybind %s", input.getKeyName(Binds["Lag-switch"]).c_str());

            if (LagSwitchNamespace::TrafficBlocked == true) {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1.0f,0.5f,0,1.0f), "                     Lag-switch currently enabled!");
            }
            
            if (ImGui::Button("Change", ImVec2(80.0f, 20.0f))) {
                bindToMacro("Lag-switch");
            }

            ImGui::Checkbox("Prevent disconnection (More stable to disable on windows)", &LagSwitchNamespace::PreventDisconnection);
            ImGui::Checkbox("Allow advanced settings", &LagSwitchNamespace::customValuesAllowed);
            if (LagSwitchNamespace::customValuesAllowed) {
                ImGui::SliderFloat("Packet loss %", &LagSwitchNamespace::PacketLossPercentage, 80.0f, 100.0f);
                ImGui::InputInt("Lag Time (ms)", &LagSwitchNamespace::LagTimeMilliseconds);
            }
            
            ImGui::EndTabItem();
        }

        //if (ImGui::BeginTabItem("Subplace joiner")) {
          //  ImGui::EndTabItem();
        //}

        if (ImGui::BeginTabItem("Settings")) {
            // ==== GLOBAL SETTINGS ====
            ImGui::Text("Global settings:");
            ImGui::Separator();

            // Keyboard layout
            ImGui::Text("Keyboard layout:");
            ImGui::PushItemWidth(100);
            if (ImGui::Combo("##kb_layout", (int*)&kb_layout, string_kb_layouts, IM_ARRAYSIZE(string_kb_layouts))) {
                printf("Selected layout: %s (index %d)\n", string_kb_layouts[kb_layout], kb_layout);
            }
            ImGui::SameLine();
            if (ImGui::Button("Bind chat key")) {
                BindVariable(&ChatKey);
            }
            ImGui::SameLine();
            ImGui::Text(("Current: " + std::string(input.getKeyName(ChatKey))).c_str());
            ImGui::PopItemWidth();

            ImGui::Spacing();
            ImGui::Separator();

            // ==== ROBLOX SETTINGS ====
            ImGui::Text("Roblox specific:");

            // Start 2 columns
            ImGui::Columns(2, nullptr, false);

            // ---------------- LEFT COLUMN ----------------
            ImGui::PushItemWidth(100);

            // FPS
            ImGui::Text("FPS:");
            ImGui::SameLine(120); // align value
            if (ImGui::InputInt("##fps", &roblox_fps)) {
                updateSpeedglitchFPS(roblox_fps);
            }

            // Executable Name
            ImGui::Text("Process:");
            ImGui::SameLine(120);

            static char process_name_buffer[256] = "";
            static bool buffer_initialized = false;

            if (!buffer_initialized) {
                strncpy(process_name_buffer, roblox_process_name.c_str(), sizeof(process_name_buffer) - 1);
                buffer_initialized = true;
            }

            if (ImGui::InputText("##exec", process_name_buffer, sizeof(process_name_buffer))) {
                roblox_process_name = process_name_buffer;
            }

            ImGui::PopItemWidth();

            ImGui::NextColumn();

            // ---------------- RIGHT COLUMN ----------------

            // Checkbox
            ImGui::Text("Cam-Fix:");
            ImGui::SameLine(120);
            if (ImGui::Checkbox("##camfix", &cam_fix_active)) {
                updateSpeedglitchSensitivity(roblox_sensitivity, cam_fix_active);
            }

            ImGui::PushItemWidth(100);

            // Sensitivity
            ImGui::Text("Sensitivity:");
            ImGui::SameLine(120);
            if (ImGui::InputFloat("##sens", &roblox_sensitivity, 0.1f, 0.5f, "%.2f")) {
                if (roblox_sensitivity < 0.1f) roblox_sensitivity = 0.1f;
                if (roblox_sensitivity > 4.0f) roblox_sensitivity = 4.0f;
                updateSpeedglitchSensitivity(roblox_sensitivity, cam_fix_active);
            }

            ImGui::PopItemWidth();

            ImGui::Columns(1); // end columns
            ImGui::Spacing();
            ImGui::Separator();

            // ==== THEME ====
            ImGui::Text("Theme color:");
            if (ImGui::ColorEdit3("##theme_color", (float*)&themeColor)) {
                applyThemeColor(themeColor);
            }

            ImGui::Text("Presets:");

            auto ColorPresetButton = [&](const char* id, const ImVec4& color) {
                ImGui::PushStyleColor(ImGuiCol_Button, color);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(color.x * 1.2f, color.y * 1.2f, color.z * 1.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(color.x * 0.8f, color.y * 0.8f, color.z * 0.8f, 1.0f));
                bool clicked = ImGui::Button(id, ImVec2(20, 20));
                ImGui::PopStyleColor(3);
                return clicked;
            };

            const ImVec4 presetColors[] = {
                {0.6f, 0.08f, 0.08f, 1.0f},   // red
                {0.08f, 0.3f, 0.6f, 1.0f},    // blue
                {0.08f, 0.5f, 0.15f, 1.0f},   // green
                {0.45f, 0.15f, 0.6f, 1.0f},   // purple
                {0.7f, 0.35f, 0.08f, 1.0f},   // orange
                {0.08f, 0.5f, 0.55f, 1.0f},   // cyan
                {0.65f, 0.15f, 0.4f, 1.0f},   // pink
                {0.65f, 0.55f, 0.08f, 1.0f},   // yellow
                {0.08f, 0.45f, 0.4f, 1.0f},   // teal
                {0.55f, 0.08f, 0.55f, 1.0f},  // magenta
                {0.4f, 0.65f, 0.08f, 1.0f},   // lime
                {50/255.f, 50/255.f, 50/255.f, 1.0f}, // silver
            };

            for (int i = 0; i < IM_ARRAYSIZE(presetColors); i++) {
                char id[16];
                sprintf(id, "##c%d", i);
                if (ColorPresetButton(id, presetColors[i])) {
                    themeColor = presetColors[i];
                    applyThemeColor(themeColor);
                }
                if (i != IM_ARRAYSIZE(presetColors) - 1)
                    ImGui::SameLine();
            }

            ImGui::Spacing();

            ImGui::Checkbox("Rainbow Theme", &rainbowThemeEnabled);
            if (rainbowThemeEnabled) {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1.f, 0.5f, 0.f, 1.f), "(cycling)");

                ImGui::Text("Rainbow Theme Settings:");
                ImGui::SliderFloat("Hue Speed", &rainbowSpeed, 0.001f, 0.02f, "%.3f");
                ImGui::SliderFloat("Saturation", &rainbowSaturation, 0.0f, 1.0f, "%.2f");
                ImGui::SliderFloat("Value", &rainbowValue, 0.0f, 1.0f, "%.2f");
            }

            ImGui::Separator();
            ImGui::Text("Made with love <3 -3443");

            ImGui::EndTabItem();
        }

        
        ImGui::EndTabBar();
    }
    
    ImGui::End();
}