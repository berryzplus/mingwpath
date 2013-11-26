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



int
parseArgs(int *pModeFlags, LPTSTR pszPath,int argc, TCHAR* argv[]);
extern int 
convertSeperator(LPTSTR lpszPath, size_t size, TCHAR chSrc, TCHAR chDst);
extern int
getBashRoot(LPTSTR pszBashRoot, size_t buffLength);
extern DWORD
GetLongPath(LPCTSTR pszPath, LPTSTR *pszBuff);
extern DWORD
GetShortPath(LPCTSTR pszPath, LPTSTR *pszBuff);


int _tmain(int argc, TCHAR* argv[]) {

	int modeFlags = MF_UNIXNAME;
	LPTSTR pszPath, pszTemp, pszBashRoot;
	size_t len;

	pszPath = (LPTSTR) malloc(BUFFER_LENGTH * sizeof(TCHAR));

	/** parse args */
	if (parseArgs(&modeFlags, (LPTSTR) pszPath, argc, argv)) {
		return 1;
	}

	/* convert to absolute path. */
	if (modeFlags & OF_ABSOLUTE) {
		/* get full path, with auto allocation. */
		pszTemp = _tfullpath(NULL, pszPath, 0);
		_tcscpy_s(pszPath, BUFFER_LENGTH, pszTemp);
		free(pszTemp);
		pszTemp = NULL;
	}

	/* get Bash root from ENV ( C:/MinGW/msys/1.0 ). */
	pszBashRoot = (LPTSTR) malloc(BUFFER_LENGTH * sizeof(TCHAR));
	if (getBashRoot(pszBashRoot, BUFFER_LENGTH)) {
		_ftprintf(stderr, _T(PACKAGE_NAME) _T(": unknown error, you may not using mingw."));
		return 1;
	}

	/* mode is print windows(long) filename. */
	if (modeFlags & OF_LONGNAME) {
		if (!GetLongPath(pszPath, &pszTemp)) {
			_ftprintf(stderr, _T(PACKAGE_NAME) _T(": cannot create long name of %s."), pszPath);
			return 1;
		}
		_tcscpy_s(pszPath, BUFFER_LENGTH, pszTemp);
		if (!GetLongPath(pszBashRoot, &pszTemp)) {
			_ftprintf(stderr, _T(PACKAGE_NAME) _T(": cannot create long name of %s."), pszBashRoot);
			return 1;
		}
		_tcscpy_s(pszBashRoot, BUFFER_LENGTH, pszTemp);
		free(pszTemp);
		pszTemp = NULL;
	}

	/* mode is print dos filename. */
	if (modeFlags & OF_SHORTNAME) {
		if (!GetShortPath(pszPath, &pszTemp)) {
			_ftprintf(stderr, _T(PACKAGE_NAME) _T(": cannot create short name of %s."), pszPath);
			return 1;
		}
		_tcscpy_s(pszPath, BUFFER_LENGTH, pszTemp);
		if (!GetShortPath(pszBashRoot, &pszTemp)) {
			_ftprintf(stderr, _T(PACKAGE_NAME) _T(": cannot create short name of %s."), pszBashRoot);
			return 1;
		}
		_tcscpy_s(pszBashRoot, BUFFER_LENGTH, pszTemp);
		free(pszTemp);
		pszTemp = NULL;
	}

	/* if the path starts with bash root. */
	if (_tcsstr(pszPath, pszBashRoot) == pszPath) {
		/* fix bash root. */
		len = _tcslen(pszPath);
		if (len == _tcslen(pszBashRoot)) {
			pszPath[len + 0] = '\\';
			pszPath[len + 1] = '\0';
		}
	}

	/* mode 'windows' or 'dos'. */
	if (modeFlags & 2) {
		/* fix style '/c/path/to/dir' -> 'C:/path/to/dir'. */
		if (_istalpha(pszPath[1]) && 
			(pszPath[0] == _T('/') || pszPath[0] == _T('\\')) && 
			(pszPath[2] == _T('/') || pszPath[2] == _T('\\'))) {
			pszPath[0] = _totupper(pszPath[1]);
			pszPath[1] = _T(':');
			/* pszPath[2] = _T('\\'); */
		}
		/* fix style 'c:/path/to/dir' -> 'C:/path/to/dir'. */
		else if (_istlower(pszPath[0]) && pszPath[1] == _T(':')) {
			pszPath[0] = _totupper(pszPath[0]);
		}
		/* convert '/' -> '\\'. */
		convertSeperator(pszPath, BUFFER_LENGTH, _T('/'), _T('\\'));
	}
	/* mode 'mixed' or 'unix'. */
	else {
		/* mode 'unix'. */
		if (!(modeFlags & 4)) {
			/* if the path starts with bash root. */
			if (_tcsstr(pszPath, pszBashRoot) == pszPath) {
				/* fix bash root. */
				len = _tcslen(pszBashRoot);
				pszTemp = (LPTSTR) ((size_t) pszPath + len * sizeof(TCHAR));
				_tcscpy_s(pszBashRoot, BUFFER_LENGTH, pszTemp);
				_tcscpy_s(pszPath, BUFFER_LENGTH, pszBashRoot);
			}
			/* mode 'unix' and starts with 'C:'. */
			if (!(modeFlags & 4) && _istalpha(pszPath[0]) && pszPath[1] == _T(':')) {
				pszPath[1] = _totlower(pszPath[0]);
				pszPath[0] = _T('/');
			}
		}
		/* convert seperater '\\' -> '/'. */
		convertSeperator(pszPath, BUFFER_LENGTH, _T('\\'), _T('/'));
	}

	/* generate final output. */
	_ftprintf(stdout, pszPath);

    return 0;
}


#ifdef __cplusplus
}
#endif
