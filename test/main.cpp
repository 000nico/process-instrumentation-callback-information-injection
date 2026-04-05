#include "../syringe/syringe.hpp"
#include "payload/bin/shellcode.hpp"
#include <iostream>

int main(){

    HWND hwnd = FindWindowA(nullptr, "Untitled - Notepad");
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);

    std::cout << "[test] pid = " << pid << std::endl;
    
    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    PICII::inject(handle, 0x1000, (unsigned char*)bin_payload_bin, bin_payload_bin_len, true);

    Sleep(2000);

    PICII::exit(handle, true);
    CloseHandle(handle);
}