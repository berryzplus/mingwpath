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


#ifdef __cplusplus
extern "C" {
#endif


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

int appendChar(LPTSTR pszBuff, size_t buffLength, TCHAR ch) {
	LPTSTR pszTemp = _tcschr(pszBuff, 0);
	*pszTemp = ch;
	pszTemp++;
	*pszTemp = 0;
	return 0;
}

DWORD
GetFullPath(LPCTSTR pszPath, LPTSTR *pszBuff) {
	DWORD dwLength = 0;
	dwLength = GetFullPathName(pszPath, 0, NULL, NULL);
	if (0 < dwLength) {
		*pszBuff = (LPTSTR) malloc(sizeof(TCHAR) * dwLength);
		dwLength = GetFullPathName(pszPath, dwLength, *pszBuff, NULL);
		if (!dwLength) {
			free( *pszBuff );
			*pszBuff = NULL;
		}
	}
	return dwLength;
}


DWORD
GetLongPath(LPCTSTR pszPath, LPTSTR *pszBuff) {
	DWORD dwLength = 0;
	dwLength = GetLongPathName(pszPath, NULL, 0);
	if (0 < dwLength) {
		*pszBuff = (LPTSTR) malloc(sizeof(TCHAR) * dwLength);
		dwLength = GetLongPathName(pszPath, *pszBuff, dwLength);
		if (!dwLength) {
			free( *pszBuff );
			*pszBuff = NULL;
		}
	}
	return dwLength;
}


DWORD
GetShortPath(LPCTSTR pszPath, LPTSTR *pszBuff) {
	DWORD dwLength = 0;
	dwLength = GetShortPathName(pszPath, NULL, 0);
	if (0 < dwLength) {
		*pszBuff = (LPTSTR) malloc(sizeof(TCHAR) * dwLength);
		dwLength = GetShortPathName(pszPath, *pszBuff, dwLength);
		if (!dwLength) {
			free( *pszBuff );
			*pszBuff = NULL;
		}
	}
	return dwLength;
}


#ifdef __cplusplus
}
#endif
