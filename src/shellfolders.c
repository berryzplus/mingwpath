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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef _UNICODE
# define SH_GET_FOLDER_PATH "SHGetFolderPath" "W"
#else
# define SH_GET_FOLDER_PATH "SHGetFolderPath" "A"
#endif

typedef HRESULT (CALLBACK* PROC_SH_GFP)(HWND, int, HANDLE, DWORD, LPTSTR);

BOOL SHGetFolderPathSimple(int nFolder, LPTSTR pszPath)
{
    HRESULT hr   = S_FALSE;
    HMODULE hDLL = NULL;
    PROC_SH_GFP pfnSHGetFolderPath = NULL;

    if(!pszPath) {
        return FALSE;
    }

    hDLL = LoadLibrary(_T("shell32.dll"));

    if (hDLL) {

        pfnSHGetFolderPath = (PROC_SH_GFP) GetProcAddress(hDLL, SH_GET_FOLDER_PATH);

        if(pfnSHGetFolderPath) {
            hr = pfnSHGetFolderPath(NULL, nFolder, NULL, 0, pszPath);
        }

        FreeLibrary(hDLL);
    }

    return SUCCEEDED(hr);
}

