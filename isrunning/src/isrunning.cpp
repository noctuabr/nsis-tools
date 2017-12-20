/*
 * Copyright (c) 2007-2017, Noctua Tecnologia da Informação
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NOCTUA TECNOLOGIA DA INFORMAÇÃO BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <windows.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>
#include <string.h>

/**
 * Name of the function K32GetProcessImageFileName().
 */
#ifdef _UNICODE
	#define K32GetProcessImageFileName_NAME "K32GetProcessImageFileNameW"
#else
	#define K32GetProcessImageFileName_NAME "K32GetProcessImageFileNameA"
#endif

/**
 * Type of the function K32GetProcessImageFileName().
 */
typedef DWORD (WINAPI *GetProcessImageFileName_t)(HANDLE, LPTSTR, DWORD);

/**
 * Pointer to the function K32GetProcessImageFileName().
 */
GetProcessImageFileName_t GetProcessImageFileNameP = NULL;

/**
 * Find the required functions.
 *
 * @return true on success or false otherwise.
 */
bool FindFunctions() {

	HMODULE  module = LoadLibrary(_T("kernel32.dll"));
	if (module) {
		GetProcessImageFileNameP = (GetProcessImageFileName_t)GetProcAddress(module, K32GetProcessImageFileName_NAME);
		FreeLibrary(module);
		return (GetProcessImageFileNameP != NULL);
	} else {
		return false;
	}
}

/**
 * Verifies if a given executable is running.
 *
 * @param[in] path Full path of the executable using device path instead of drive
 * path.
 * @return true if the process is running or false otherwise.
 */
bool FindProcByExecutable(const TCHAR * path) {
	DWORD procIdList[4096];
	DWORD procIDListSize;

	if (EnumProcesses(procIdList, sizeof(procIdList), &procIDListSize)) {
		procIDListSize = procIDListSize / sizeof(DWORD);
		for (DWORD i = 0; i < procIDListSize; i++) {
			HANDLE proc;
			proc = OpenProcess(PROCESS_QUERY_INFORMATION, false, procIdList[i]);
			if (proc) {
				TCHAR procPath[MAX_PATH * 2];
				if (GetProcessImageFileNameP(proc, procPath, sizeof(procPath) / sizeof(TCHAR)) > 0) {
					if (_tcsicmp(procPath, path) == 0) {
						CloseHandle(proc);
						return true;
					}
				}
				CloseHandle(proc);
			}
		}
		return false;
	} else {
		return false;
	}
}

/**
 * Converts a given drive path (e.g.: "x:\...") into a device path (e.g.: 
 * "\Device\HarddiskN\...").
 *
 * @param[in] path The full path of the executable.
 * @param[out] devicePath The buffer that will receive the converted path.
 * @param[out] devicePathSize Size of devicePath in characters. 
 * @return true on success or false otherwise.
 */
bool PathToDevicePath(const TCHAR * path, TCHAR * devicePath, DWORD devicePathSize) {
	TCHAR drive[4];
	TCHAR device[MAX_PATH];

	// Extract the drive letter from x:\...
	DWORD pathLen = _tcslen(path);
	if (pathLen < 4) {
		return false;
	}
	if (path[1] != _T(':')) {
		return false;
	}
	drive[0] = path[0];
	drive[1] = _T(':');
	drive[2] = 0;
	if (QueryDosDevice(drive, device, sizeof(device) / sizeof(TCHAR)) == 0) {
		return false;
	}

	// Compose the device path
	if (_tcscpy_s(devicePath, devicePathSize, device)) {
		return false;
	}
	return (_tcscat_s(devicePath, devicePathSize, path + 2) == 0);
}

/**
 * Main function.
 *
 * @param[in] argc Number of command line arguments.
 * @param[in] argv Command line arguments.
 * @return The return code.
 */
int _tmain(int argc, TCHAR ** argv) {
	TCHAR devicePath[MAX_PATH * 2];

	// TODO Convert the return codes into constants in order to make the 
	// code easier to read.
	if (argc != 2) {
		return 1;
	}
	if (!FindFunctions()) {
		return 4;
	}
	if (!PathToDevicePath(argv[1], devicePath, sizeof(devicePath) / sizeof(TCHAR))) {
		return 2;
	}
	if (FindProcByExecutable(devicePath)) {
		return 0;
	} else {
		return 3;
	}
}

