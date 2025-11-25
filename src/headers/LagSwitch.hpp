#pragma once
#include <cstdlib>
#include <sstream>
#include "Globals.hpp"
#include "inpctrl.hpp"
#include "Helper.hpp"
#include "netctrl.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <windows.h>
#include <shellapi.h>

#else
#include <unistd.h>
#endif

inline namespace LagSwitchNamespace {
    inline bool TrafficBlocked = false;
    inline bool PreventDisconnection = true;
    inline int LagTimeMilliseconds = 1;
    inline float PacketLossPercentage = 99.5f;
    inline bool customValuesAllowed = false;
    
#ifdef _WIN32
    inline HANDLE fumbleJob = NULL;
    inline PROCESS_INFORMATION fumbleProcess{};
    
    inline void initFumbleJob() {
        if (!fumbleJob) {
            fumbleJob = CreateJobObjectA(NULL, NULL);
            if (fumbleJob) {
                JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = {};
                jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
                SetInformationJobObject(fumbleJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));
                log("Job object created - fumble will auto-close with program");
            } else {
                log("Warning: Failed to create job object. Fumble may persist after exit.");
            }
        }
    }
    
    struct FumbleGuard {
        ~FumbleGuard() {
            if (fumbleProcess.hProcess) {
                TerminateProcess(fumbleProcess.hProcess, 0);
                CloseHandle(fumbleProcess.hProcess);
                CloseHandle(fumbleProcess.hThread);
                ZeroMemory(&fumbleProcess, sizeof(fumbleProcess));
            }
            if (fumbleJob) {
                CloseHandle(fumbleJob);
                fumbleJob = NULL;
            }
        }
    };
    inline FumbleGuard fumbleCleanup;
#endif

    inline bool BlockTraffic() {
        if (TrafficBlocked) {
            log("Traffic already blocked");
            return true;
        }
        
        log("Blocking outbound traffic for " + roblox_process_name);
        
        int lag_ms = customValuesAllowed ? LagTimeMilliseconds : 1;
        float drop_pct = customValuesAllowed ? PacketLossPercentage : 99.5f;
        
        // Use PreventDisconnection instead of CanDisconnect
        if (PreventDisconnection) {
            log("[netctrl] Preventing disconnection by simulating lag/drop...");
#ifdef _WIN32
            initFumbleJob();
            
            std::stringstream ss;
            ss << " --lag " << lag_ms
               << " --drop " << drop_pct
               << " --filter \"udp.DstPort >= 49152\"";
            
            std::string params = ss.str();
            
            SHELLEXECUTEINFOA sei = {};
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
            sei.lpVerb = "runas";  // Request admin elevation
            sei.lpFile = "resources\\fumble.exe";
            sei.lpParameters = params.c_str();
            sei.nShow = SW_HIDE;
            
            log("Launching fumble.exe with elevation: " + params);
            
            if (!ShellExecuteExA(&sei)) {
                DWORD error = GetLastError();
                if (error == ERROR_CANCELLED) {
                    log("User cancelled UAC prompt");
                } else {
                    log("Failed to launch fumble.exe. Error: " + std::to_string(error));
                }
                return false;
            }
            
            if (!sei.hProcess) {
                log("Failed to get fumble process handle");
                return false;
            }
            
            fumbleProcess.hProcess = sei.hProcess;
            fumbleProcess.dwProcessId = GetProcessId(sei.hProcess);
            
            if (fumbleJob) {
                if (!AssignProcessToJobObject(fumbleJob, fumbleProcess.hProcess)) {
                    log("Warning: Failed to assign fumble to job object");
                }
            }
            
            log("Fumble launched successfully with admin privileges.");
            TrafficBlocked = true;
            return true;
#else
            if (ctrl.lag(lag_ms, static_cast<double>(drop_pct))) {
                log("[netctrl] Applied " + std::to_string(lag_ms) + "ms, " +
                    std::to_string(drop_pct) + "% drop");
                TrafficBlocked = true;
                return true;
            }
            log("[netctrl] Failed to apply lag/drop.");
            return false;
#endif
        } else {
            if (ctrl.lag(1, 100)) {
                log("[netctrl] Applied " + std::to_string(lag_ms) + "ms, " + std::to_string(drop_pct) + "% drop");
                TrafficBlocked = true;
                return true;
            }
        }
        
        log("[netctrl] No disconnection prevention active.");
        return false;
    }
    
    inline bool UnblockTraffic() {
        if (!TrafficBlocked) {
            log("Traffic already unblocked");
            return true;
        }
        
        log("Unblocking outbound traffic for " + roblox_process_name);
        
#ifdef _WIN32
        if (fumbleProcess.hProcess) {
            log("Terminating fumble process...");
            TerminateProcess(fumbleProcess.hProcess, 0);
            CloseHandle(fumbleProcess.hProcess);
            CloseHandle(fumbleProcess.hThread);
            ZeroMemory(&fumbleProcess, sizeof(fumbleProcess));
        }
#endif
        
        log("[netctrl] Disabling netctrl...");
        ctrl.disable();
        
        TrafficBlocked = false;
        log("[netctrl] Unblocked.");
        return true;
    }
}

inline void LagSwitch() {
    bool key_pressed = input.isKeyPressed(Binds["Lag-switch"]);

    if (!key_pressed && events[4]) {
        if (LagSwitchNamespace::TrafficBlocked) {
            LagSwitchNamespace::UnblockTraffic();
        } else {
            LagSwitchNamespace::BlockTraffic();
        }
    }
    events[4] = key_pressed;
}