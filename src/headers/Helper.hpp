#pragma once
#include <iostream>
#include <string>

#include "Globals.hpp"
#include "inpctrl.hpp"

#ifdef _WIN32
#ifndef NETCTRL_WINDOWS_INCLUDED
#define NETCTRL_WINDOWS_INCLUDED
#define WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif

// Rename Windows functions to avoid conflicts with Raylib
#define Rectangle Win32Rectangle
#define CloseWindow Win32CloseWindow
#define ShowCursor Win32ShowCursor
#define DrawText Win32DrawText
#define DrawTextEx Win32DrawTextEx
#define LoadImage Win32LoadImage

#include <windows.h>
#include <shellapi.h>

// Restore original names after Windows header is included
#undef Rectangle
#undef CloseWindow
#undef ShowCursor
#undef DrawText
#undef DrawTextEx
#undef LoadImage

#endif
#else
#include <unistd.h>
#endif


inline bool isElevated() {
#if defined(_WIN32)
    BOOL isAdmin = FALSE;
    PSID adminGroup;

    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&ntAuthority, 2, 
        SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
        0,0,0,0,0,0, &adminGroup))
    {
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }
    return isAdmin;
#else
    return geteuid() == 0;
#endif
}

#if defined(__linux__)
inline bool hasX11Display() {
    const char* d = getenv("DISPLAY");
    return d && d[0] != '\0';
}

inline void runXhostPlus() {
    if (!hasX11Display())
        return;
    if (isElevated()) {
        // Program was run with sudo — run command as normal user
        const char* normalUser = getenv("SUDO_USER");
        if (!normalUser) normalUser = "root"; // fallback
        std::string cmd = "sudo -u ";
        cmd += normalUser;
        cmd += " xhost +";
        system(cmd.c_str());
    } else {
        // Program is run normally — run xhost + normally
        system("xhost +");
    }
}
#endif

inline bool TryElevate(const char* password)
{
    // Already elevated?
    if (isElevated())
        return true;
        
#if defined(__linux__)
    // Get own executable path
    char exePath[4096] = {0};
    readlink("/proc/self/exe", exePath, sizeof(exePath)-1);
    
    // Test if password is correct first
    std::string testCmd = 
        "echo \"" + std::string(password) + "\" | sudo -S -p '' true 2>&1";
    int testResult = system(testCmd.c_str());
    
    // If password test failed, return false
    if (testResult != 0)
        return false;
    
    // Password is correct, now elevate and restart
    std::string cmd =
        "echo \"" + std::string(password) + "\" | sudo -S -p '' \"" + std::string(exePath) + "\" &";
    system(cmd.c_str());
    
    // Give the elevated process a moment to start
    usleep(100000); // 100ms
    
    exit(0); // stop current instance
    
//#elif defined(_WIN32)
  //  wchar_t exePath[MAX_PATH];
    //GetModuleFileNameW(NULL, exePath, MAX_PATH);

    //SHELLEXECUTEINFOW sei = {0};  // <-- C-style init works with MinGW
    //sei.cbSize = sizeof(sei);
    //sei.lpVerb = L"runas";
    //sei.lpFile = exePath;
    //sei.nShow = SW_SHOWNORMAL;

    //if (!ShellExecuteExW(&sei))
        //return false;

    //exit(0);
#else
    return false;
#endif
}




inline void log(std::string text) {
    std::cout << "[3RU] " << text << std::endl;
}

inline void RunSilent(const std::string &cmd) {
#ifdef _WIN32
    std::string finalCmd = cmd + " >nul 2>&1";
#else
    std::string finalCmd = cmd + " >/dev/null 2>&1";
#endif
    system(finalCmd.c_str());
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

inline void showMessageBox(const std::string& title, const std::string& msg) {
#if defined(_WIN32)
    MessageBoxA(NULL, msg.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION);
#elif defined(__linux__)
    std::string cmd =
        "zenity --info --title=\"" + title + "\" --text=\"" + msg + "\"";
    system(cmd.c_str());
#else
    #error "Unsupported platform"
#endif
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
    } else if (CodeName == "Buckey-clip") {
        return 5;
    } else if (CodeName == "") {
        return 5;
    } else return 2000;
}