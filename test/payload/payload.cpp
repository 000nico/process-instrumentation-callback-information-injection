#include <windows.h>
#include <winternl.h>
#include "../main.hpp"

extern "C" void payload(){
    volatile uintptr_t structPlaceHolder = 0xAAAAAAAAAAAAAAAA;

    payloadStruct* ps = (payloadStruct*)structPlaceHolder;
    ps->flag = 9;
}