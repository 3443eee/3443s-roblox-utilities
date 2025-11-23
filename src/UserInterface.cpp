#include "UserInterface.hpp"
#include "imgui.h"
#include "rlImGui.h"
#include "Globals.hpp"
#include "inpctrl.hpp"
#include "Helper.hpp"
#include "LagSwitch.hpp"

void initUI() {
    // Initialize rlImGui
    rlImGuiSetup(true);
    ImGuiStyle& style = ImGui::GetStyle();

    // --- Buttons ---
    style.Colors[ImGuiCol_Button] = ImVec4(0.8f, 0.1f, 0.1f, 1.0f);       // normal
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.2f, 0.2f, 1.0f); // hover
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.7f, 0.0f, 0.0f, 1.0f);  // pressed

    // --- Tabs ---
    style.Colors[ImGuiCol_Tab] = ImVec4(0.8f, 0.1f, 0.1f, 1.0f);              
    style.Colors[ImGuiCol_TabHovered] = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);       
    style.Colors[ImGuiCol_TabActive] = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);        
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.6f, 0.1f, 0.1f, 1.0f);     
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.8f, 0.2f, 0.2f, 1.0f); 

    style.TabRounding = 0.0f; // Make tabs square

    // --- Checkboxes ---
    style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.6f, 0.1f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.8f, 0.1f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);

    // --- Sliders ---
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
    style.GrabMinSize = 8.0f;

    // --- Scrollbars ---
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.2f, 0.05f, 0.05f, 1.0f);       // track background
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.8f, 0.1f, 0.1f, 1.0f);       // normal grab
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
    
    style.ScrollbarRounding = 0.0f;  // make scrollbar square
    style.ScrollbarSize = 12.0f;     // adjust thickness
}

void UpdateUI() {
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
                std::string CodeName;

                if (std::string(label) == "Freeze") {
                    CodeName = "Freeze";
                } else if (std::string(label) == "Laugh Clip") {
                    CodeName = "Laugh";
                } else if (std::string(label) == "Extended Dance Clip") {
                    CodeName = "E-Dance";
                }

                // Determine if this option is enabled
                bool isEnabled = enabled[GetIDFromCodeName(CodeName)];

                // Set button colors based on enabled state
                if (isEnabled) {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.0f, 0.0f, 1.0f));
                } else {
                    // Even less red / dimmed when disabled
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.07f, 0.07f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.1f, 0.1f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.05f, 0.05f, 1.0f));
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
                    ImGui::BeginChild(std::string(label + std::string("_frame")).c_str(), ImVec2(0, 100), true);
                    ImGui::TextWrapped("Keybind: ");
                    ImGui::Text("Current keybind %s", input.getKeyName(Binds[CodeName]).c_str());
                    if (ImGui::Button("Change", ImVec2(-1, 20.0f))) {
                        bindToMacro(CodeName);
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
            ImGui::Text("Lag switch settings:");
            ImGui::Separator();
            ImGui::Text("Current keybind %s", input.getKeyName(Binds["Lag-switch"]).c_str());

            if (LagSwitchNamespace::TrafficBlocked == true) {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1.0f,0.5f,0,1.0f), "                     Lag-switch currently enabled!");
            }
            
            if (ImGui::Button("Change", ImVec2(80.0f, 20.0f))) {
                bindToMacro("Lag-switch");
            }

            ImGui::Checkbox("Can disconnect? (More stable to enable on windows)", &LagSwitchNamespace::CanDisconnect);
            ImGui::Checkbox("Allow advanced settings", &LagSwitchNamespace::customValuesAllowed);
            if (LagSwitchNamespace::customValuesAllowed) {
                ImGui::SliderFloat("Packet loss %", &LagSwitchNamespace::PacketLossPercentage, 80.0f, 100.0f);
                ImGui::InputInt("Lag Time (ms)", &LagSwitchNamespace::LagTimeMilliseconds);
            }
            
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Settings")) {
            ImGui::Text("Settings content here");
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
    }
    
    ImGui::End();
}