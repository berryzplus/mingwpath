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

/* available option set. */
MINGWPATH_OPTION opts[] = {
	/* Output type options: */
	{MF_DOSSHORT,  _T('d'), _T("dos"),			_T("print DOS (short) form of NAME (C:\\PROGRA~1\\)"), },
	{MF_MIXEDNAME, _T('m'), _T("mixed"),		_T("like --windows, but with regular slashes (C:/WINNT)"), },
	{MF_UNIXNAME,  _T('u'), _T("unix"),			_T("(default) print Unix form of NAME (/c/winnt)"), },
	{MF_WINDOWS,   _T('w'), _T("windows"),		_T("print Windows form of NAME (C:\\WINNT)"), },
	{0xFFFF,       _T('t'), _T("type"),			_T("form: \"dos\", \"mixed\", \"unix\", or \"windows\""), },

	/* Path conversion options: */
	{OF_ABSOLUTE,  _T('a'), _T("absolute"),		_T("output absolute path"), },
	{OF_LONGNAME,  _T('l'), _T("long-name"),	_T("print Windows long form of NAME (with -w, -m only)"), },
	{OF_SHORTNAME, _T('s'), _T("short-name"),	_T("print DOS (short) form of NAME (with -w, -m only)"), },

	/* System information: */
	{PF_ALLUSERS,  _T('A'), _T("allusers"),		_T("use `All Users' instead of current user for -D, -P"), },
	{PF_DESKTOP,   _T('D'), _T("desktop"),		_T("output `Desktop' directory and exit"), },
	{PF_HOMEROOT,  _T('H'), _T("homeroot"),		_T("output `Profiles' directory (home root) and exit"), },
	{PF_SMPROGRA,  _T('P'), _T("smprograms"),	_T("output Start Menu `Programs' directory and exit"), },
	{PF_SYSTEM32,  _T('S'), _T("sysdir"),		_T("output system directory and exit"), },
	{PF_WINDOWS,   _T('W'), _T("windir"),		_T("output `Windows' directory and exit"), },
};

#define OPT_LEN (sizeof(opts) / sizeof (MINGWPATH_OPTION))


extern DWORD
GetShellFolder(int pathFlag, LPTSTR *pszBuff);


void printHelp() {
	int n;
	_ftprintf(stderr, _T("%s version %s(built on %s)\n\n"), 
		_T(PACKAGE_NAME), _T(PACKAGE_VERSION), _T(__DATE__) _T(" ") _T(__TIME__));
	_ftprintf(stderr, _T(" available options:\n"));
	/* convert short option to mode flags */
	for (n = 0; n < OPT_LEN; n++) {
		if (opts[n].shortName == _T('t')) {
			_ftprintf(stderr, _T("  -%c, --%s TYPE    %s.\n"), 
				opts[n].shortName, opts[n].longName, opts[n].description);
		}
		else {
			_ftprintf(stderr, _T("  -%c, --%-12s %s.\n"), 
				opts[n].shortName, opts[n].longName, opts[n].description);
		}
	}
}


int
parseArg(int *pModeFlags, LPTSTR pszPath,int argc, TCHAR* argv[]) {
	int n,m,c,modeFlags,typeDefined;
	size_t len;
	LPTSTR arg, pType, pszTemp;

	modeFlags = *pModeFlags = MF_UNIXNAME;
	typeDefined = 0;
	pszPath[0] = 0;

	/** parse args */
	for (n=1; n < argc; n++) {

		arg = argv[n];
		if (arg[0] == _T('-')) {
			/* parse long option */
			if (arg[1] == _T('-')) {
				/* convert long option to short option */
				for (m = 0; m < OPT_LEN; m++) {
					if (!_tcscmp(&arg[2], opts[m].longName)) {
						arg[1] = opts[m].shortName;
						break;
					}
				}
				if (arg[1] == _T('-')) {
					_ftprintf(stderr, _T(PACKAGE_NAME) _T(": unknown option '%s'."), arg);
					return 1;
				}
				arg[2] = 0;
				continue;
			}
			/* parse short option */
			len = _tcslen(arg);
			for (c=1 ; c < len ; c++) {
				/* parse type option */
				if (arg[c] == _T('t')) {
					/* type has already be defined */
					if (typeDefined) {
						_ftprintf(stderr, _T(PACKAGE_NAME) _T(": type can be set once!"));
						return 1;
					}
					pType = &arg[c];
					if (n + 1 < argc) {
						/* check type: 'dos', 'mixed', 'unix', or 'windows' */
						for (m = 0; m < OPT_LEN; m++) {
							if (!_tcscmp(argv[n + 1], opts[m].longName)) {
								pType[0] = opts[m].shortName;
								break;
							}
							if (opts[m].shortName == _T('t')) {
								break;
							}
						}
						n++;
					}
					/* no type */
					else {
						_ftprintf(stderr, _T(PACKAGE_NAME) _T(": type TYPE missing."));
						return 1;
					}
					/* invalid type */
					if (pType[0] == _T('t')) {
						_ftprintf(stderr, _T(PACKAGE_NAME) _T(": unknown type '%s'."), argv[n + 1]);
						return 1;
					}
				}
				/* convert short option to mode flags */
				for (m = 0; m < OPT_LEN; m++) {
					if (arg[c] == opts[m].shortName) {
						modeFlags |= opts[m].code;
						break;
					}
				}
			}
		}
		else {
			/* copy path from parameter. */
			pszTemp = pszPath;
			for (m = n; m < argc; m++) {
				arg = argv[m];
				_tcscpy_s(pszTemp, BUFFER_LENGTH - (size_t) (pszTemp - pszPath) * sizeof(TCHAR), arg);
				if (m + 1 < argc) {
					len = _tcslen(pszTemp);
					pszTemp[len] = 0x20;
					pszTemp = (LPTSTR) ((size_t) pszTemp + len * sizeof(TCHAR));
				}
			}
			break;
		}
	}

	/* mode is print system information. */
	if (modeFlags & 0xFF00) {
		/* get full path, with auto allocation. */
		if (!GetShellFolder(modeFlags & 0xFF00, &pszTemp)) {
			return 1;
		}
		_tcscpy_s(pszPath, BUFFER_LENGTH, pszTemp);
		free(pszTemp);
		pszTemp = NULL;
	}
	/* mode piped input. */
	else if (!pszPath[0] && 0 < _filelengthi64(_fileno( stdin ))) {
        _fgetts(pszPath, BUFFER_LENGTH, stdin);
		len = _tcslen(pszPath);
		if (0 < len) {
			pszPath[len - 1] = 0;
		}
	}
	/* no input. */
	else if (!pszPath[0]) {
		_ftprintf(stderr, _T(PACKAGE_NAME) _T(": no input\n\n"));
		printHelp();
		return 1;
	}

	*pModeFlags = modeFlags;

	return 0;
}

	
int convertSeperator(LPTSTR lpszPath, size_t size, TCHAR chSrc, TCHAR chDst) {
	int ret = 0;
	int c = 0;
	for (c=0 ; c < size ; c++) {
		if (lpszPath[c] == chSrc) {
			lpszPath[c] = chDst;
			ret++;
		}
	}
	return ret;
}


int
getRoot(LPTSTR pszBashRoot, size_t buffLength) {
	LPTSTR pszTemp, pszTemp2;

	/* get the path of sh.exe */
	pszTemp = (LPTSTR) malloc(buffLength * sizeof(TCHAR));
	SearchPath(NULL, _T("sh"), _T(".exe"), (DWORD) buffLength, pszTemp, NULL);

	/* cut '\\bin\\sh.exe'. */
	pszTemp2 = _tcsstr(pszTemp, _T("\\bin\\sh.exe"));
	pszTemp2[0] = 0;

	/* copy to allocated buffer. */
	_tcscpy_s(pszBashRoot, BUFFER_LENGTH, pszTemp);
	free(pszTemp);

	/* convert seperater '\\' -> '/'. */
	convertSeperator(pszBashRoot, buffLength, _T('\\'), _T('/'));

	return 0;
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


int
convPath(int modeFlags, LPTSTR pszPath, LPTSTR pszRoot) {

	LPTSTR pszTemp;
	size_t len;

	/* convert to absolute path. */
	if (modeFlags & OF_ABSOLUTE) {
		/* get full path, with auto allocation. */
		pszTemp = _tfullpath(NULL, pszPath, 0);
		_tcscpy_s(pszPath, BUFFER_LENGTH, pszTemp);
		free(pszTemp);
		pszTemp = NULL;
	}

	/* get Bash root from ENV ( C:/MinGW/msys/1.0 ). */
	if (getRoot(pszRoot, BUFFER_LENGTH)) {
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
		if (!GetLongPath(pszRoot, &pszTemp)) {
			_ftprintf(stderr, _T(PACKAGE_NAME) _T(": cannot create long name of %s."), pszRoot);
			return 1;
		}
		_tcscpy_s(pszRoot, BUFFER_LENGTH, pszTemp);
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
		if (!GetShortPath(pszRoot, &pszTemp)) {
			_ftprintf(stderr, _T(PACKAGE_NAME) _T(": cannot create short name of %s."), pszRoot);
			return 1;
		}
		_tcscpy_s(pszRoot, BUFFER_LENGTH, pszTemp);
		free(pszTemp);
		pszTemp = NULL;
	}

	/* if the path starts with bash root. */
	if (_tcsstr(pszPath, pszRoot) == pszPath) {
		/* fix bash root. */
		len = _tcslen(pszPath);
		if (len == _tcslen(pszRoot)) {
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
			if (_tcsstr(pszPath, pszRoot) == pszPath) {
				/* fix bash root. */
				len = _tcslen(pszRoot);
				pszTemp = (LPTSTR) ((size_t) pszPath + len * sizeof(TCHAR));
				_tcscpy_s(pszRoot, BUFFER_LENGTH, pszTemp);
				_tcscpy_s(pszPath, BUFFER_LENGTH, pszRoot);
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

    return 0;
}


#ifdef __cplusplus
}
#endif
