#include <fltKernel.h>

#define dprintf(...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, __VA_ARGS__)

bool gThreadExiting = false;
HANDLE gThreadHandle = nullptr;
PVOID gThreadObject = nullptr;

void
Sleep(unsigned long milliseconds)
{
    LARGE_INTEGER interval;
    interval.QuadPart = -(10000ll * milliseconds);
    KeDelayExecutionThread(KernelMode, FALSE, &interval);
}

__declspec(naked) void LetMeGG()
{
    _asm {
	    sub rsp, 0x48
        mov eax, 0x5
        mov byte ptr ss:[rsp + 0x10], 0x67
        mov byte ptr ss:[rsp + 0x11], 0
        push rax
        lea rax, qword ptr ss:[rsp + 0x18]
        mov qword ptr ss:[rsp + 0x40], rax
        mov qword ptr ss:[rsp + 0x30], rax
        pop rax
        mov word ptr ss:[rsp + 0x20], 0x1
        mov word ptr ss:[rsp + 0x30], 0x1
        lea rdx, ss:[rsp + 0x20]
        lea rcx, ss:[rsp + 0x30]
        int 0x2D
	    nop
	    add rsp, 0x48
	    ret
    }
}

void
ThreadFunction(_In_ PVOID StartContext)
{
    while (1)
    {
        if (gThreadExiting)
        {
            break;
        }

        __try
        {
            LetMeGG();
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }

        dprintf("LetMeGG\n");
        Sleep(1);
    }
}

void
ThreadStop()
{
    gThreadExiting = true;
    if (gThreadObject)
    {
        KeWaitForSingleObject(gThreadObject, Executive, KernelMode, FALSE, nullptr);
    }
    if (gThreadHandle)
    {
        ZwClose(gThreadHandle);
    }
    gThreadObject = nullptr;
    gThreadHandle = nullptr;
}

void
DriverUnLoad(_In_ struct _DRIVER_OBJECT *DriverObject)
{
    ThreadStop();
    dprintf("free world\n");
}

EXTERN_C
NTSTATUS
DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    dprintf("new world\n");
    DriverObject->DriverUnload = DriverUnLoad;

    OBJECT_ATTRIBUTES oa;
    InitializeObjectAttributes(&oa, nullptr, OBJ_KERNEL_HANDLE, nullptr, nullptr);
    auto ns = PsCreateSystemThread(&gThreadHandle, GENERIC_ALL, &oa, nullptr, nullptr, ThreadFunction, nullptr);
    if (ns >= 0)
    {
        ns = ObReferenceObjectByHandle(gThreadHandle, 0, nullptr, KernelMode, &gThreadObject, nullptr);
        if (ns < 0)
        {
            ThreadStop();
        }
    }

    return STATUS_SUCCESS;
}