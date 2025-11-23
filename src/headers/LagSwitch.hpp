#pragma once
#include <cstdlib>
#include "Globals.hpp"
#include "inpctrl.hpp"
#include "Helper.hpp"
#include "netctrl.hpp"

inline namespace LagSwitchNamespace {
    inline bool TrafficBlocked = false;
    inline bool CanDisconnect = false;
    inline int LagTimeMilliseconds = 1;
    inline float PacketLossPercentage = 99.5f;
    inline bool customValuesAllowed = false;
    
#ifdef _WIN32
    inline HANDLE fumbleJob = NULL;
    inline PROCESS_INFORMATION fumbleProcess{};
    
    // Initialize job object once
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
    
    // RAII wrapper to ensure cleanup
    struct FumbleGuard {
        ~FumbleGuard() {
            if (fumbleProcess.hProcess) {
                TerminateProcess(fumbleProcess.hProcess, 0);
                CloseHandle(fumbleProcess.hProcess);
                CloseHandle(fumbleProcess.hThread);
                ZeroMemory(&fumbleProcess, sizeof(fumbleProcess));
            }
            if (fumbleJob) {
                CloseHandle(fumbleJob);  // This kills all processes in the job
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
        
        // Full disconnect mode
        if (CanDisconnect) {
            if (ctrl.block()) {
                log("[netctrl] Blocked all traffic.");
                TrafficBlocked = true;
                return true;
            }
            log("[netctrl] Failed to block traffic.");
            return false;
        }
        
#ifdef _WIN32
        // Initialize job object for auto-cleanup
        initFumbleJob();
        
        // Windows - use fumble.exe
        std::stringstream ss;
        ss << "resources\\fumble.exe"
           << " --lag " << lag_ms
           << " --drop " << drop_pct
           << " --filter \"udp.DstPort >= 49152\"";
        
        std::string cmd = ss.str();
        log("Running: " + cmd);
        
        // CreateProcessA needs modifiable buffer
        std::vector<char> cmdBuffer(cmd.begin(), cmd.end());
        cmdBuffer.push_back('\0');
        
        STARTUPINFOA si{};
        si.cb = sizeof(si);
        ZeroMemory(&fumbleProcess, sizeof(fumbleProcess));
        
        if (!CreateProcessA(
            NULL,
            cmdBuffer.data(),
            NULL, NULL, FALSE, 
            CREATE_NO_WINDOW | CREATE_SUSPENDED,  // Start suspended to add to job first
            NULL, NULL, &si, &fumbleProcess
        )) {
            log("Failed to launch fumble.exe. Error: " + std::to_string(GetLastError()));
            return false;
        }
        
        // Add to job object - fumble will die when program exits
        if (fumbleJob) {
            if (!AssignProcessToJobObject(fumbleJob, fumbleProcess.hProcess)) {
                log("Warning: Failed to assign fumble to job object");
            }
        }
        
        // Now resume the process
        ResumeThread(fumbleProcess.hThread);
        
        log("Fumble launched successfully.");
        TrafficBlocked = true;
        return true;
#else
        // Linux - use netctrl
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
    
    inline bool UnblockTraffic() {
        if (!TrafficBlocked) {
            log("Traffic already unblocked");
            return true;
        }
        
        log("Unblocking outbound traffic for " + roblox_process_name);
        
#ifdef _WIN32
        // Windows - kill fumble.exe if running
        if (fumbleProcess.hProcess) {
            log("Terminating fumble process...");
            TerminateProcess(fumbleProcess.hProcess, 0);
            CloseHandle(fumbleProcess.hProcess);
            CloseHandle(fumbleProcess.hThread);
            ZeroMemory(&fumbleProcess, sizeof(fumbleProcess));
        }
#endif
        
        // Disable netctrl (works for both platforms)
        log("Disabling netctrl...");
        ctrl.disable();
        
        TrafficBlocked = false;
        log("Unblocked.");
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