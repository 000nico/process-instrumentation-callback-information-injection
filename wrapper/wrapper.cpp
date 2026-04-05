#include <windows.h>

extern "C" void __attribute__((naked)) callback(){
    __asm__ volatile(
        "push rax\n"
        "push rcx\n"
        "push rdx\n"
        "push r8\n"
        "push r9\n"
        "push r10\n"
        "push r11\n"
        "pushfq\n"
        "mov r10, rsp\n"
        "and rsp, 0xFFFFFFFFFFFFFFF0\n"
        "sub rsp, 0x28\n"
        "movabs rax, 0xDEADBEEFDEADBEEF\n"
        "call rax\n"
        "mov rsp, r10\n"
        "popfq\n"
        "pop r11\n"
        "pop r10\n"
        "pop r9\n"
        "pop r8\n"
        "pop rdx\n"
        "pop rcx\n"
        "pop rax\n"
        "jmp r10\n"
    );
}