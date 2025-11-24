#pragma once
#include <cstdlib>
#include "Globals.hpp"
#include "inpctrl.hpp"
#include "Helper.hpp"
#include "netctrl.hpp"

inline namespace LagSwitchNamespace {
    inline bool TrafficBlocked = false;
    inline bool PreventDisconnection = true;  // replaces CanDisconnect
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
            ss << "resources\\fumble.exe"
               << " --lag " << lag_ms
               << " --drop " << drop_pct
               << " --filter \"udp.DstPort >= 49152\"";
            
            std::string cmd = ss.str();
            log("Running: " + cmd);
            
            std::vector<char> cmdBuffer(cmd.begin(), cmd.end());
            cmdBuffer.push_back('\0');
            
            STARTUPINFOA si{};
            si.cb = sizeof(si);
            ZeroMemory(&fumbleProcess, sizeof(fumbleProcess));
            
            if (!CreateProcessA(
                NULL,
                cmdBuffer.data(),
                NULL, NULL, FALSE,
                CREATE_NO_WINDOW | CREATE_SUSPENDED,
                NULL, NULL, &si, &fumbleProcess
            )) {
                log("Failed to launch fumble.exe. Error: " + std::to_string(GetLastError()));
                return false;
            }
            
            if (fumbleJob) {
                if (!AssignProcessToJobObject(fumbleJob, fumbleProcess.hProcess)) {
                    log("Warning: Failed to assign fumble to job object");
                }
            }
            
            ResumeThread(fumbleProcess.hThread);
            log("Fumble launched successfully.");
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