#pragma once
#include <Windows.h>
#include <winternl.h>
#include <stdio.h>
#include "Protect.h"
#include "config.h"

//Here are the fun stuff where the magic happens

#define baseOperation 0x7980

#define COMMAND_MAGIC baseOperation*0x5478

#define EFI_VARIABLE_NON_VOLATILE                          0x00000001
#define EFI_VARIABLE_BOOTSERVICE_ACCESS                    0x00000002
#define EFI_VARIABLE_RUNTIME_ACCESS                        0x00000004
#define EFI_VARIABLE_HARDWARE_ERROR_RECORD                 0x00000008
#define EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS            0x00000010
#define EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS 0x00000020
#define EFI_VARIABLE_APPEND_WRITE                          0x00000040
#define ATTRIBUTES (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)

#define SE_SYSTEM_ENVIRONMENT_PRIVILEGE (22L)
#define RTL_CONSTANT_STRING(s) { sizeof(s) - sizeof((s)[0]), sizeof(s), (PWSTR)s }

extern GUID DummyGuid;
// Hmu on discord (Chase.#1803) if you need any help :)

typedef NTSTATUS(*mRtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN Client, PBOOLEAN WasEnabled);
typedef NTSTATUS(*mNtSetSystemEnvironmentValueEx)(PUNICODE_STRING VariableName, LPGUID VendorGuid, PVOID Value, ULONG ValueLength, ULONG Attributes);
typedef NTSTATUS(*mNtQuerySystemInformation)(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);
extern mRtlAdjustPrivilege myRtlAdjustPrivilege;
extern mNtSetSystemEnvironmentValueEx myNtSetSystemEnvironmentValueEx;
extern mNtQuerySystemInformation myNtQuerySystemInformation;
// Hmu on discord (Chase.#1803) if you need any help :)

typedef struct _MemoryCommand {
	int magic;
	int operation;
	unsigned long long data[6];
} MemoryCommand;

constexpr auto STATUS_INFO_LENGTH_MISMATCH = 0xC0000004;
// Hmu on discord (Chase.#1803) if you need any help :)
constexpr auto SystemModuleInformation = 11;
constexpr auto SystemHandleInformation = 16;
constexpr auto SystemExtendedHandleInformation = 64;

typedef struct _RTL_PROCESS_MODULE_INFORMATION
{
	HANDLE Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR FullPathName[256];
} RTL_PROCESS_MODULE_INFORMATION, * PRTL_PROCESS_MODULE_INFORMATION;
// Hmu on discord (Chase.#1803) if you need any help :)
typedef struct _RTL_PROCESS_MODULES
{
	ULONG NumberOfModules;
	RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, * PRTL_PROCESS_MODULES;

uintptr_t GetKernelModuleAddress(char* module_name);
uintptr_t GetKernelModuleExport(uintptr_t kernel_module_base, char* function_name);
NTSTATUS SetSystemEnvironmentPrivilege(BOOLEAN Enable, PBOOLEAN WasEnabled);
// Hmu on discord (Chase.#1803) if you need any help :)
namespace Driver
{
	bool	initialize();
	extern uintptr_t currentProcessId;
	extern HANDLE driverH;

	void SendCommand(MemoryCommand* cmd);
	NTSTATUS copy_memory(uintptr_t src_process_id, uintptr_t src_address, uintptr_t dest_process_id, uintptr_t dest_address, size_t size);
	NTSTATUS read_memory(uintptr_t process_id, uintptr_t address, uintptr_t buffer, size_t size);
	NTSTATUS write_memory(uintptr_t process_id, uintptr_t address, uintptr_t buffer, size_t size);
	uintptr_t GetBaseAddress(uintptr_t pid);

	template <typename T>
	T read(const uintptr_t process_id, const uintptr_t address, PNTSTATUS out_status = 0)
	{
		Protect(_ReturnAddress());
		T buffer{ };
		Unprotect(read_memory);
		NTSTATUS status = read_memory(process_id, address, uintptr_t(&buffer), sizeof(T));
		Protect(read_memory);
		if (out_status)
			*out_status = status;
		Unprotect(_ReturnAddress());
		return buffer;
	}
	// Hmu on discord (Chase.#1803) if you need any help :)
	template <typename T>
	void write(const uintptr_t process_id, const uintptr_t address, const T& buffer, PNTSTATUS out_status = 0)
	{
		Protect(_ReturnAddress());
		Unprotect(write_memory);
		NTSTATUS status = write_memory(process_id, address, uintptr_t(&buffer), sizeof(T));
		Protect(write_memory);
		if (out_status)
			*out_status = status;
		Unprotect(_ReturnAddress());
	}
}// Hmu on discord (Chase.#1803) if you need any help :)

