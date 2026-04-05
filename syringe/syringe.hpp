# pragma once
#include <windows.h>

namespace PICII {
    bool inject(HANDLE handle, size_t size, unsigned char* payload, unsigned int lenght, bool debug);
    bool exit(HANDLE handle, bool debug);
}

typedef NTSTATUS (NTAPI *fnNtSetInformationProcess_t)(
    HANDLE,
    ULONG,
    PVOID,
    ULONG
);

struct PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION {
    ULONG Version;
    ULONG Reserved;
    PVOID Callback;
};