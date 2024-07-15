# External-Skeleton-v2
Template for External Cheats written in C++

## Documentation:

mem.h:

{

	#pragma once
	#include <Windows.h>
	
	namespace mem
	{
		void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess );
		void NopEx(BYTE* dst, unsigned int size, HANDLE hProcess);
	}

}

mem.cpp:

{

	#include "mem.h"
	
	
	void mem::PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess)
	{
	
		DWORD oldprotect;
		// Each page of memory is given permission on what memory you can do if you have the handle open. What access permission are
		// you grantet? Ask the OS to give us permissions. Part of the Security system.
	
		// VirtualProtectEx: Changes the protection on a region of committed
		// pages in the virtual address space of a specified process.
	
		// [in] hProcess: 
		// A handle to the process whose memory protection is to be changed. 
		// The handle must have the PROCESS_VM_OPERATION access right.
		// For more information, see Process Security and Access Rights.
	
		// [in] lpAddress:
		// A pointer to the base address of the region of pages whose access protection attributes are to be changed.
	
		// [in] dwSize:
		// The size of the region whose access protection attributes are changed, in bytes.
		// The region of affected pages includes all pages containing one or more bytes in the range from the lpAddress parameter to (lpAddress+dwSize).
		// This means that a 2-byte range straddling a page boundary causes the protection attributes of both pages to be changed.
	
		// [in] flNewProtect:
		// The memory protection option. This parameter can be one of the memory protection constants. ->
		// PAGE_EXECUTE_READWRITE : Enables execute, read-only, or read/write access to the committed region of pages.
		// -> https://learn.microsoft.com/en-us/windows/win32/Memory/memory-protection-constants
	
		// [out] lpflOldProtect:
		// A pointer to a variable that receives the previous access protection of the first page in the specified region of pages.
		// ! If this parameter is NULL or does not point to a valid variable, the function fails. !
	
		// VirtualProtectEx return value:
		// If the function succeeds, the return value is nonzero.
		// If the function fails, the return value is zero.To get extended error information, call GetLastError.
		// https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualprotectex
	
		VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
		WriteProcessMemory(hProcess, dst, src, size, nullptr);
		VirtualProtectEx(hProcess, dst, size, oldprotect, &oldprotect);
	}
	
	
	void mem::NopEx(BYTE* dst, unsigned int size, HANDLE hProcess)
	{
	
		// Get new byte array on the heap
		BYTE* nopArray = new BYTE[size];
	
		// set each byte of that array to 0x90 -> no operation instruction
		// memset: Sets a buffer to a specified character.
	
		// dest:
		// Pointer to destination.
	
		// c:
		// Character to set.
	
		//count:
		// Number of characters.
	
		// memset return value:
		// The value of dest.
		// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/memset-wmemset?view=msvc-170
	
		memset(nopArray, 0x90, size);
	
		PatchEx(dst, nopArray, size, hProcess);
		delete[] nopArray;
	}

}

proc.cpp ( i know its differentt but it should still explain):
{

	#include "proc.h"
	
	// Everything said here is from Rake with an addition of the Microsoft´s MSDN 
	
	// Credits:
	// https://guidedhacking.com/
	// https://learn.microsoft.com/en-us/search/
	
	// Why DWORD? The Process Id is a DWORD in the Windows API. 
	// Why const wchar_t? The default programm settings for visual studio is to use unicode as the default character set
	
	DWORD GetProcId(const wchar_t* procName)
	{
	
		// We create a variable process id to store the process id we find. we assign it the value 0 for error checking.
		// DWORD: Allocates and optionally initializes a double word (4 bytes) of storage for each initializer.
		// https://learn.microsoft.com/en-us/cpp/assembler/masm/dword?view=msvc-170#syntax
	
		DWORD procId = 0;
	
		// Check out Windows MSDN to understand the code ( via. link )
	
		// TH32CS_SNAPPROCESS -> this argument is what we want to get an snapshot of. In this case we
		// want an snapshot of the processes. When we get that Handle we store it in hSnap. The return value of CreateToolhelp32Snapshot
		// is either a sucess Handle or if it fails it will be the following return value: INVALID_HANDLE_VALUE
		// CreateToolhelp32Snapshot: Takes a snapshot of the specified processes, as well
		// as the heaps, modules, and threads used by these processes.
		// TH32CS_SNAPPROCESS: 0x00000002 -> Includes all processes in the system in the snapshot. 
		// To enumerate the processes, see Process32First. 
		// https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-createtoolhelp32snapshot
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	
		// if statement for error checking:
		// If the function succeeds, it returns an open handle to the specified snapshot.
		// If the function fails, it returns INVALID_HANDLE_VALUE.
		// To get extended error information, call GetLastError.Possible error codes include ERROR_BAD_LENGTH.
		if (hSnap != INVALID_HANDLE_VALUE)
		{
			// PROCESSENTRY32 is a struct that contains important members ,for instance th32ProcessID,
			// where we can store our information in with the function
			// Process32First and Process32Next
	
			// PROCESSENTRY32: Describes an entry from a list of the processes 
			// residing in the system address space when a snapshot was taken.
			//https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/ns-tlhelp32-processentry32
			PROCESSENTRY32 procEntry{};
	
			// The MSDN documentation tells us to set the dwSize in order to wokr correctly.
			// The size of the structure, in bytes. Before calling the Process32First function,
			//  set this member to sizeof(PROCESSENTRY32).
			//  If you do not initialize dwSize, Process32First fails.
			// https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/ns-tlhelp32-processentry32
			procEntry.dwSize = sizeof(procEntry);
	
			//Process32First grabs the first process in the snapshot and stores it in the procEntry
			// Process32First: Retrieves information about the first process encountered in a system snapshot.
			// https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-process32first
			// [in] hSnapshot: A handle to the snapshot returned from a previous call to the CreateToolhelp32Snapshot function.
			// [in, out] lppe: A pointer to a PROCESSENTRY32 structure. It contains process information such as the name of the executable file, 
			// the process identifier, and the process identifier of the parent process.
			//https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-process32first
			
			// Process32First return value:
			// Returns TRUE if the first entry of the process list has been copied to the buffer or FALSE otherwise. 
			// The ERROR_NO_MORE_FILES error value is returned by the GetLastError function 
			// if no processes exist or the snapshot does not contain process information.
	
			BOOL resultFirstProcess;
			if ((resultFirstProcess = Process32First(hSnap, &procEntry)) && resultFirstProcess != ERROR_NO_MORE_FILES )
			{
				BOOL resultNextProcess;
				//Then it is going to loop thorugh all of them using Process32Next Function
				do
				{
					// Then its going to do the if statement where it compares the file name against our given process Name
					// This is just a string compare that uses wide chars and its case insensetive. 
					// The arguments and return value of _wcsicmp are wide-character strings.
					// 0 -> string1 identical to string2 - (see other return values via link)
					// _wcsicmp and wcscmp behave identically except that wcscmp doesn't 
					// convert its arguments to lowercase before comparing them.
					// You'll need to call setlocale for _wcsicmp to work with Latin 1 characters.
					// Call setlocale with any locale other than the C locale before the call to _wcsicmp.
					// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/stricmp-wcsicmp-mbsicmp-stricmp-l-wcsicmp-l-mbsicmp-l?view=msvc-170
					if (!_wcsicmp(procEntry.szExeFile, procName))
					{
						// When our Process is found it is going to assign procId to the processID of our procEntry
						//  and then it is going to break out of the loop
						// th32ProcessID: The process identifier.
						procId = procEntry.th32ProcessID;
						goto PROCESS_FOUND;
					}
					// Process32Next: Retrieves information about the next process recorded in a system snapshot.
					// [in] hSnapshot: A handle to the snapshot returned from a previous call to the CreateToolhelp32Snapshot function.
					// [out] lppe: A pointer to a PROCESSENTRY32 structure.
					
					// Process32Next return value: 
					// Returns TRUE if the next entry of the process list has been copied to the buffer or FALSE otherwise.
					// The ERROR_NO_MORE_FILES error value is returned by the GetLastError function
					// if no processes exist or the snapshot does not contain process information.
					// https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-process32next
					
				} while ((resultNextProcess = Process32Next(hSnap, &procEntry)) && resultNextProcess != ERROR_NO_MORE_FILES);
				std::cout << "Error Code: " << GetLastError() << std::endl;
			}
			else { std::cout << "Error Code: " << GetLastError() << std::endl; }
		}
		else { std::cout << "Error Code: " << GetLastError() << std::endl; }
	
	PROCESS_FOUND:
	
		// Then we close the handle to the snapshot and that is going to stop memory leaks 
		// and then we return the procId
		// CloseHandle: Closes an open object handle.
		// [in] hObject: A valid handle to an open object.
		// CloseHandle return value:
		// If the function succeeds, the return value is nonzero.
		// If the function fails, the return value is zero.
		// https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
		CloseHandle(hSnap);
		return procId;
	}
	
	// Why does it return a uintptr_t? This is a architecture independent variable 
	// because when you are compiling for x86 it is goign to compile for a 32bit variable
	// and when you are compiling for x64 then its is going to compile for a 64bit variable
	// So in order words it doesnt matter for which build you are compiling since it automatically converts to the
	// correct version -> either x86 or x64.
	
	uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
	{
		uintptr_t modBaseAddr = 0;
	
		// TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32 will get you 32bit and 64 bit modules
		// TH32CS_SNAPMODULE 0x00000008: Includes all modules of the process specified in th32ProcessID in the snapshot. 
		// TH32CS_SNAPMODULE32 0x00000010: Includes all 32-bit modules of the process specified in th32ProcessID in the snapshot when called from a 64-bit process.
		//  This flag can be combined with TH32CS_SNAPMODULE or TH32CS_SNAPALL. If the function fails with ERROR_BAD_LENGTH, retry the function until it succeeds. 
		// https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-createtoolhelp32snapshot
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	
		if (hSnap != INVALID_HANDLE_VALUE)
		{
			// MODULEENTRY32 structure: Describes an entry from a list of the modules belonging to the specified process.
			MODULEENTRY32 modEntry{};
	
			// dwSize: The size of the structure, in bytes.
			// Before calling the Module32First function, set this member to sizeof(MODULEENTRY32).
			// If you do not initialize dwSize, Module32First fails.
			modEntry.dwSize = sizeof(modEntry);
	
			// Module32First: Retrieves information about the first module associated with a process.
			// [in] hSnapshot: A handle to the snapshot returned from a previous call to the CreateToolhelp32Snapshot function.
			// [in, out] lpme: A pointer to a MODULEENTRY32 structure.
			// https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-module32first
	
			BOOL resultFirstModule;
			if ((resultFirstModule = Module32First(hSnap, &modEntry)) && resultFirstModule != ERROR_NO_MORE_FILES)
			{
				BOOL resultNextModule;
				do
				{
					if (!_wcsicmp(modEntry.szModule, modName))
					{
						// modBaseAddr: The base address of the module in the context of the owning process.
						modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
						goto MODULE_FOUND;
					}
					// Module32Next: Retrieves information about the next module associated with a process or thread.
					// [in] hSnapshot: A handle to the snapshot returned from a previous call to the CreateToolhelp32Snapshot function.
					// [out] lpme: A pointer to a MODULEENTRY32 structure.
					// Module32Next return value:
					// Returns TRUE if the next entry of the module list has been copied to the buffer or FALSE otherwise. 
					// The ERROR_NO_MORE_FILES error value is returned by the GetLastError function if no more modules exist.
					// https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-module32next
					
				} while ((resultNextModule = Module32Next(hSnap, &modEntry)) && resultNextModule != ERROR_NO_MORE_FILES);
				std::cout << "Error Code: " << GetLastError() << std::endl;
			}
			else { std::cout << "Error Code: " << GetLastError() << std::endl; }
		}
	
	MODULE_FOUND:
		CloseHandle(hSnap);
		return modBaseAddr;
	}
	
	
	// FindDMAAddy -> basically DMA is for Dynamic Memory Allocation its kinda a stupid name that fleep used
	// and everybody knows about this name so I am going to continue to use it - Rake
	
	// This function takes a handle to the process. And a base pointer -> most likely something like ac_client.exe 
	// And then it is going to take a vector of unsignend integers that are going to be our offsets
	uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
	{
	
		// grabs the value of ptr and puts it in addr 
		uintptr_t addr = ptr;
	
		// The reason why we use std::vector is that you can dynamically get the size. Which is the number of offsets
		for (size_t i = 0; i < offsets.size(); ++i)
		{
			// Go to -> MSDN it is going to explain this function.
			// Why are we type casting into a byte* -> well the functions only
			// takes a parameter that has the Type LPCVOID -> Void measn data type is not important
			// LPCVOID -> Reads by BYTES
			// ReadProcessMemory is going to read what is in the address and then store it into addr
			// -> basically derefrencing the pointer
	
			// BOOL ReadProcessMemory(
			//		[in]  HANDLE  hProcess,
			//		[in]  LPCVOID lpBaseAddress,
			//		[out] LPVOID  lpBuffer,
			//		[in]  SIZE_T  nSize,
			//		[out] SIZE_T* lpNumberOfBytesRead
			//	);
			
			// [in] hProcess: 
			// A handle to the process with memory that is being read. 
			// The handle must have PROCESS_VM_READ access to the process.
			
			// [in] lpBaseAddress: 
			// A pointer to the base address in the specified process from which to read. 
			// Before any data transfer occurs, the system verifies that all data in the base address and memory
			// of the specified size is accessible for read access, and if it is not accessible the function fails.
			
			// [out] lpBuffer: 
			// A pointer to a buffer that receives the contents from the address space of the specified process.
	
			// [in] nSize:
			// The number of bytes to be read from the specified process.
	
			// [out] lpNumberOfBytesRead:
			// A pointer to a variable that receives the number of bytes transferred into the specified buffer. 
			// If lpNumberOfBytesRead is NULL, the parameter is ignored.
			
			// ReadProcessMemory return value:
			// If the function succeeds, the return value is nonzero.
			// If the function fails, the return value is 0 (zero).To get extended error information, call GetLastError.
			// The function fails if the requested read operation crosses into an area of the process that is inaccessible.
	
			// Note:
			// ReadProcessMemory copies the data in the specified address range from the address space 
			// of the specified process into the specified buffer of the current process.
			//  Any process that has a handle with PROCESS_VM_READ access can call the function.
			// https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-readprocessmemory
	
			ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0); 
			// and then it is going to add the offset and then store it in addr
			addr += offsets[i];
			// it is going to loop back in ->
			// read pointer - put it into addr - add the offset - repeadeatly do that
		}
		// at the end it is going to return the addr -> address of the actual variable you searched 
		// for instance health value or ammo value
		return addr;
	}

}

entry.cpp:

{

	// Everything said here is from Rake with an addition of the Microsoft´s MSDN 
	
	// Credits:
	// https://guidedhacking.com/
	// https://learn.microsoft.com/en-us/search/
	
	// <> tell visual studio to look for these headers in the external dependencies
	// Where those are defined: 
	// projectname -> properties -> Configuration Properties -> VC++ Directories -> General -> include Directories
	#include <iostream> 
	
	// Anything with quotation marks means it is inside the project folder
	#include "proc.h"
	#include "mem.h"
	
	int main()
	{
	
	    // define multiple variables
	    bool bOption1{false}, bOption2{ false }, bOption3{ false }, bOption4{ false };
	
	    //Get ProcId of the target Process
	    // Where to find process Id? -> https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/finding-the-process-id
	    
	    // Why is the process Id stored as an DWORD?
	    // 1. See member variable dwProcessId https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-process_information 
	    // 2. See return data type from function https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getprocessid
	    // L Macro means Unicode -> converts it to Unicode
	    DWORD procId = GetProcId(L"ac_client.exe");
	
	    // Error checking
	    if (!procId)
	        std::cout << "Error Code: " << GetLastError() << std::endl;
	
	    //Get module base adress
	    uintptr_t moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");
	
	    // Get Handle to Process
	
	    // Set it to zero especially with Windows API functions -> a lot of 
	    // Windows Api functions will return 0 as a error checking mechanism.
	    HANDLE hProcess = 0; 
	
	    // OpenProcess: Opens an existing local process object.
	    
	    // [in] dwDesiredAccess: 
	    // The access to the process object. 
	    // This access right is checked against the security descriptor for the process. 
	    // This parameter can be one or more of the process access rights.
	    // If the caller has enabled the SeDebugPrivilege privilege, the requested access 
	    // is granted regardless of the contents of the security descriptor.
	    
	    // [in] bInheritHandle:
	    // If this value is TRUE, processes created by this process will inherit the handle.
	    // Otherwise, the processes do not inherit this handle.
	    
	    // [in] dwProcessId:
	    // The identifier of the local process to be opened.
	    // If the specified process is the System Idle Process (0x00000000), the function fails and the last error code is 
	    // ERROR_INVALID_PARAMETER. If the specified process is the System process or one of the Client Server Run-Time 
	    // Subsystem (CSRSS) processes, this function fails and the last error code is ERROR_ACCESS_DENIED because their access 
	    // restrictions prevent user-level code from opening them.
	    
	    // OpenProcess return value:
	    // If the function succeeds, the return value is an open handle to the specified process.
	    // If the function fails, the return value is NULL.To get extended error information, call GetLastError.
	    // https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-openprocess
	
	    // PROCESS_ALL_ACCESS is a bitfield that gives you the DesiredAcces of a process -> read write etc...
	    // To understand it more go to MSDN
	    
	    // PROCESS_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED (0x000F0000L) | SYNCHRONIZE (0x00100000L) | 0xFFFF):
	    // ll possible access rights for a process object.Windows Server 2003 and Windows XP: 
	    // The size of the PROCESS_ALL_ACCESS flag increased on Windows Server 2008 and Windows Vista.
	    // If an application compiled for Windows Server 2008 and Windows Vista is run on Windows 
	    // Server 2003 or Windows XP, the PROCESS_ALL_ACCESS flag is too large and the function specifying this flag fails with ERROR_ACCESS_DENIED.
	    // To avoid this problem, specify the minimum set of access rights required for the operation.
	    // If PROCESS_ALL_ACCESS must be used, set _WIN32_WINNT to the minimum operating system targeted by your application (for example, #define _WIN32_WINNT _WIN32_WINNT_WINXP). 
	    // For more information, see Using the Windows Headers (https://learn.microsoft.com/en-us/windows/win32/winprog/using-the-windows-headers). 
	    // https://learn.microsoft.com/en-us/windows/win32/procthread/process-security-and-access-rights
	
	    hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
	
	    // Resolve base address of pointer chain
	    // cheat engine -> ac_client.exe+0x10f4f4 
	    // address of module + relative offset(for instance localpalyer)
	    uintptr_t dynamicPtrBaseAddr = moduleBase + 0x10f4f4;
	
	    // Error Checking -> If the function fails, the return value is NULL. 
	    // And Read- WriteProcessmemory like it better if u check the handle before.
	    // Or else you might get an warning
	    if (hProcess != NULL)
	    {
	        // Error Checking
	        std::cout << "DynamicPtrBaseAddr = " << "0x" << std::hex << dynamicPtrBaseAddr << std::endl;
	        uintptr_t healthAddr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, { 0xf8 });
	        int newValue = 1337;
	
	        DWORD dwExit = 0;
	        // GetExitCodeProcess:  Retrieves the termination status of the specified process.
	
	        // [in] hProcess:
	        // A handle to the process.
	
	        // [out] lpExitCode:
	        // A pointer to a variable to receive the process termination status.
	
	        // This function returns immediately. If the process has not 
	        // terminated and the function succeeds, the status returned is STILL_ACTIVE (a macro for STATUS_PENDING (minwinbase.h)). 
	        // If the process has terminated and the function succeeds, the status returned is one of the following values:
	        // The exit value specified in the ExitProcess or TerminateProcess function.
	        // The return value from the main or WinMain function of the process.
	        // The exception value for an unhandled exception that caused the process to terminate.
	
	        // GetExitCodeProcess return value: 
	        // If the function succeeds, the return value is nonzero.
	        // If the function fails, the return value is zero.To get extended error information, call GetLastError.
	        // https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getexitcodeprocess
	
	        while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE)
	        {
	            // We are doing & 1 in order to excecute the function only once 
	            // GetAsyncKeyState: Determines whether a key is up or down at the time the function is called, 
	            // and whether the key was pressed after a previous call to GetAsyncKeyState.
	
	            // [in] vKey:
	            //Type: int
	            //The virtual - key code.For more information, see Virtual Key Codes. -> https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	            //You can use left - and right - distinguishing constants to specify certain keys.See the Remarks section for further information.
	
	            // GetAsyncKeyState return value: 
	            // If the function succeeds, the return value specifies whether the key was pressed since the last call to GetAsyncKeyState, and whether the key is currently up or down.
	            // If the most significant bit is set, the key is down, and if the least significant bit is set, the key was pressed after the previous call to GetAsyncKeyState.
	            // However, you should not rely on this last behavior; for more information, see the Remarks.
	
	            // The return value is zero for the following cases:
	            // The current desktop is not the active desktop
	            // The foreground thread belongs to another process and the desktop does not allow the hook or the journal record.
	            // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getasynckeystate
	
	            if (GetAsyncKeyState(VK_NUMPAD1) & 1)
	            {
	                bOption1 = !bOption1;
	            }
	
	            if (GetAsyncKeyState(VK_NUMPAD2) & 1)
	            {
	                bOption2 = !bOption2;
	
	                if (bOption2)
	                {
	                    // \xFF\x06 -> inc [esi];
	                    mem::PatchEx((BYTE*)(moduleBase + 0x637e9), (BYTE*)"\xFF\x06", 2, hProcess);
	                }
	                else
	                {
	                    // \xFF\x0E -> dex [esi];
	                    mem::PatchEx((BYTE*)(moduleBase + 0x637e9), (BYTE*)"\xFF\x0E", 2, hProcess);
	                }
	            }
	
	            if (GetAsyncKeyState(VK_NUMPAD3) & 1)
	            {
	                bOption3 = !bOption3;
	
	                if (bOption3)
	                {
	                    mem::NopEx((BYTE*)(moduleBase + 0x63786), 10, hProcess);
	                }
	                else
	                {
	                    mem::PatchEx((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10, hProcess);
	                }
	            }
	
	            if (GetAsyncKeyState(VK_INSERT) & 1)
	            {
	                return 0;
	            }
	
	            //continues write or freeze
	            if (bOption1)
	            {
	                mem::PatchEx((BYTE*)healthAddr, (BYTE*)&newValue, sizeof(newValue), hProcess);
	            }
	            Sleep(10);
	        }
	
	
	        char input{};
	        input = getchar();
	
	
	        //Resolve our ammo pointer chain
	        // A vector is just a container that is dynamic -> you can add things and remove them or sort it at runtime
	        //unsigned int for error checking
	        //std::vector<unsigned int> ammoOffsets{ 0x374, 0x14, 0x0 };
	        //uintptr_t ammoAddr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, ammoOffsets);
	
	        //std::cout << "ammoAddr = " << "0x" << std::hex << ammoAddr << std::endl;
	
	        //Read Ammo value
	
	        //We need a buffer to put it in to:
	        //int ammoValue = 0;
	
	
	        // Why nullptr? Because the parameter is taking a SIZE_T*. Anytime a pointer is required
	        // but we dont need it -> use nullptr. 0 or NULL also works but type have to match in order to prevent problems
	        //ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);
	        //std::cout << "Current ammo = " << std::dec << ammoValue << std::endl;
	
	        //Write to it
	        //int newAmmo = 1337;
	        // Why are we using BYTE* for our addresses? Because we know its a pointer. ammoAddr is a uintptr_t.
	        // uintptr_t is a unsigned int -> its not a pointer so we are going to typecast it into one. An we are 
	        // going to call it a Byte -> Byte unsigned char. Byte is easy to read.
	
	
	        // WriteProcessMemory: Writes data to an area of memory in a specified process.
	        // The entire area to be written to must be accessible or the operation fails.
	
	        // [in] hProcess:
	        // A handle to the process memory to be modified.
	        // The handle must have PROCESS_VM_WRITE and PROCESS_VM_OPERATION access to the process.
	
	        // [in] lpBaseAddress:
	        // A pointer to the base address in the specified process to which data is written.
	        // Before data transfer occurs, the system verifies that all data in the base address and memory of the specified size is accessible for write access, and if it is not accessible, the function fails.
	
	        // [in] lpBuffer:
	        // A pointer to the buffer that contains data to be written in the address space of the specified process.
	
	        // [in] nSize:
	        // The number of bytes to be written to the specified process.
	
	        // [out] lpNumberOfBytesWritten:
	        // A pointer to a variable that receives the number of bytes transferred into the specified process.
	        // This parameter is optional.
	        // If lpNumberOfBytesWritten is NULL, the parameter is ignored.
	
	        // WriteProcessMemory return value:
	        // If the function succeeds, the return value is nonzero.
	        // If the function fails, the return value is 0 (zero). 
	        // To get extended error information, call GetLastError. 
	        // The function fails if the requested write operation crosses into an area of the process that is inaccessible.
	        // https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-writeprocessmemory
	        //WriteProcessMemory(hProcess, (BYTE*)ammoAddr, &newAmmo, sizeof(newAmmo), nullptr);
	
	        //Read out again
	        //ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);
	        //std::cout << "Current ammo = " << ammoValue << std::endl;
	    }
	    else { std::cout << "Error Code: " << GetLastError() << std::endl; }
	
	    
	    // Wait for key input - Error checking purposes
	    //char input{};
	    //input = getchar();
	
	    return 0;
	}

}


