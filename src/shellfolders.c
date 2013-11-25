/* Copyright (C) 2013 Minato Hamano
 *
 * This file is a part of mingwpath.
 *
 * The mingwpath is free software:
 * you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * The mingwpath is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mingwpath.h"


#define SHELL32DLL "shell32.dll"
#define SHELL_FUNC "SHGetFolderPath"

#ifdef _UNICODE
# define SH_GET_FOLDER_PATH SHELL_FUNC "W"
#else
# define SH_GET_FOLDER_PATH SHELL_FUNC "A"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* prototype of SHGetFolderPath function. */
typedef HRESULT (CALLBACK* PROC_SH_GFP)(HWND, int, HANDLE, DWORD, LPTSTR);


DWORD 
GetShellFolder(int pathFlags, LPTSTR *pszBuff) {

	int nFolder = 0;
    HRESULT hr = 0;
	DWORD dwLength = 0;
    HMODULE hDLL   = NULL;
    PROC_SH_GFP pfnSHGetFolderPath = NULL;

	if (pathFlags & PF_ALLUSERS) {
		if (pathFlags & PF_DESKTOP) {
			nFolder = CSIDL_COMMON_DESKTOPDIRECTORY;
		}
		else if (pathFlags & PF_SMPROGRA) {
			nFolder = CSIDL_COMMON_PROGRAMS;
		}
		else {
			return 0;
		}
	}
	else if (pathFlags & PF_DESKTOP) {
		nFolder = CSIDL_DESKTOPDIRECTORY;
	}
	else if (pathFlags & PF_SMPROGRA) {
		nFolder = CSIDL_PROGRAMS;
	}
	else if (pathFlags & PF_HOMEROOT) {
		nFolder = CSIDL_PROFILE;
	}
	else if (pathFlags & PF_SYSTEM32) {
		nFolder = CSIDL_SYSTEM;
	}
	else if (pathFlags & PF_WINDOWS) {
		nFolder = CSIDL_WINDOWS;
	}
	else {
		return 0;
	}

	*pszBuff = 0;

	hDLL = LoadLibrary(_T(SHELL32DLL));
    if (hDLL) {
        pfnSHGetFolderPath = (PROC_SH_GFP) GetProcAddress(hDLL, SH_GET_FOLDER_PATH);
        if(pfnSHGetFolderPath) {
			*pszBuff = (LPTSTR) malloc(sizeof(TCHAR) * MAX_PATH);
        	memset(*pszBuff, 0, sizeof(TCHAR) * MAX_PATH);
            hr = pfnSHGetFolderPath(NULL, nFolder, NULL, 0, *pszBuff);
        }
        FreeLibrary(hDLL);
    }
	if (SUCCEEDED(hr)) {
		dwLength = (DWORD) (_tcslen(*pszBuff) + 1);
	}
	else if (*pszBuff) {
		free(*pszBuff);
		*pszBuff = NULL;
	}
	return dwLength;
}


#ifdef __cplusplus
}
#endif
