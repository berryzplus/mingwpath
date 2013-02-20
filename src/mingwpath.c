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

BOOL SHGetFolderPathSimple(int nFolder, LPTSTR pszPath);

int convertSeperator(LPTSTR lpszPath, size_t size, TCHAR chSrc, TCHAR chDst) {
	int c = 0;
	int ret = 0;
	for (c=0 ; c < size ; c++) {
		if (lpszPath[c] == chSrc) {
			lpszPath[c] = chDst;
			ret++;
		}
	}
	return ret;
}

int inline appendChar(LPTSTR pszBuff, size_t buffLength, TCHAR ch) {
	LPTSTR pszTemp = _tcschr(pszBuff, 0);
	*pszTemp = ch;
	pszTemp++;
	*pszTemp = 0;
	return 0;
}

int getBashRoot(LPTSTR pszBashRoot, size_t buffLength) {

	LPTSTR pszTemp;

	if (!GetEnvironmentVariable(_T("WD"), pszBashRoot, buffLength)) {
		return 1;
	}

	/* fix style 'C:\\MinGW\\msys\\1.0\\\\bin\\' -> 'C:\\MinGW\\msys\\1.0\\bin\\'. */
	if (_tcsstr(pszBashRoot, _T("\\\\"))){
		pszTemp = _tfullpath(NULL, pszBashRoot, 0);
		_tcscpy(pszBashRoot, pszTemp);
		free(pszTemp);
	}

	/* fix style 'C:\\MinGW\\msys\\1.0\\bin\\' -> 'C:/MinGW/msys/1.0'. */
	if (pszTemp = _tcsstr(pszBashRoot, _T("\\bin\\"))) {
		*pszTemp = 0;
	}

	/* convert seperater '\\' -> '/'. */
	convertSeperator(pszBashRoot, buffLength, _T('\\'), _T('/'));

	return 0;
}


int GetShellFolderPath(int pathFlags, LPTSTR pszPath) {

	int nFolder;

	if (pathFlags & PF_ALLUSERS) {
		if (pathFlags & PF_DESKTOP) {
			nFolder = CSIDL_COMMON_DESKTOPDIRECTORY;
		}
		else if (pathFlags & PF_SMPROGRA) {
			nFolder = CSIDL_COMMON_PROGRAMS;
		}
		else {
			return 1;
		}
	}
	else if (pathFlags & PF_DESKTOP) {
		nFolder = CSIDL_DESKTOPDIRECTORY;
	}
	else if (pathFlags & PF_SMPROGRA) {
		nFolder = CSIDL_PROGRAMS;
	}
	else if (pathFlags & PF_SYSTEM32) {
		nFolder = CSIDL_SYSTEM;
	}
	else if (pathFlags & PF_WINDOWS) {
		nFolder = CSIDL_WINDOWS;
	}
	else {
		return 1;
	}

	return !SHGetFolderPathSimple(nFolder, pszPath);
}

