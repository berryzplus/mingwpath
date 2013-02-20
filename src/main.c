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

typedef struct __mingpath_option {
	int  code;
	TCHAR shortName;
	LPCTSTR longName;
//	TCHAR description[58];
} MINGWPATH_OPTION, *LPMINGPATH_OPTION;


MINGWPATH_OPTION opts[] = {
	// Output type options:
	{MF_DOSSHORT,  _T('d'), _T("dos"),			}, // _T("print DOS (short) form of NAME (C:\\PROGRA~1\\)"), },
	{MF_MIXEDNAME, _T('m'), _T("mixed"),		}, // _T("like --windows, but with regular slashes (C:/WINNT)"), },
	{MF_UNIXNAME,  _T('u'), _T("unix"),			}, // _T("(default) print Unix form of NAME (/c/winnt)"), },
	{MF_WINDOWS,   _T('w'), _T("windows"),		}, // _T("print Windows form of NAME (C:\\WINNT)"), },
	{-1,           _T('t'), _T("type"),			}, // _T("print TYPE form: "dos", "mixed", "unix", or "windows""), },

	// Path conversion options:
	{OF_ABSOLUTE,  _T('a'), _T("absolute"),		}, // _T("output absolute path"), },
	{OF_LONGNAME,  _T('l'), _T("long-name"),	}, // _T("print Windows long form of NAME (with -w, -m only)"), },
	{OF_SHORTNAME, _T('s'), _T("short-name"),	}, // _T("print DOS (short) form of NAME (with -w, -m only)"), },

	// System information:
	{PF_ALLUSERS,  _T('A'), _T("allusers"),		}, // _T("use `All Users' instead of current user for -D, -P"), },
	{PF_DESKTOP,   _T('D'), _T("desktop"),		}, // _T("output `Desktop' directory and exit"), },
	{PF_HOMEROOT,  _T('H'), _T("homeroot"),		}, // _T("output `Profiles' directory (home root) and exit"), },
	{PF_SMPROGRA,  _T('P'), _T("smprograms"),	}, // _T("output Start Menu `Programs' directory and exit"), },
	{PF_SYSTEM32,  _T('S'), _T("sysdir"),		}, // _T("output system directory and exit"), },
	{PF_WINDOWS,   _T('W'), _T("windir"),		}, // _T("output `Windows' directory and exit"), },
};
#define OPT_LEN (sizeof(opts) / sizeof (MINGWPATH_OPTION))

int _tmain(int argc, TCHAR* argv[]) {

	int n,m,c;
	int modeFlags = MF_UNIXNAME;
	TCHAR pszPathIn[MAX_PATH + 1];

	pszPathIn[0] = 0;

	for (n=1; n < argc; n++ ) {

		LPTSTR arg = argv[n];
		if (arg[0] == _T('-')) {

			/* parse long option */
			if (arg[1] == _T('-')) {
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
			}

			/* parse type option */
			LPTSTR pType = _tcschr(arg, _T('t'));
			if (pType) {
				if (n + 1 < argc) {
					/* check type: 'dos', 'mixed', 'unix', or 'windows' */
					if (!_tcscmp(argv[n + 1], _T("dos"))) {
						*pType = _T('d');
					}
					else if (!_tcscmp(argv[n + 1], _T("mixed"))) {
						*pType = _T('m');
					}
					else if (!_tcscmp(argv[n + 1], _T("unix"))) {
						*pType = _T('u');
					}
					else if (!_tcscmp(argv[n + 1], _T("windows"))) {
						*pType = _T('w');
					}
					/* invalid type */
					if (*pType == _T('t')) {
						_ftprintf(stderr, _T(PACKAGE_NAME) _T(": unknown type '%s'."), argv[n + 1]);
						return 1;
					}
					/* invalid option */
					if (_tcschr(arg, _T('t'))) {
						_ftprintf(stderr, _T(PACKAGE_NAME) _T(": type can be set once!"));
						return 1;
					}
					n++;
				}
				/* no type */
				else {
						_ftprintf(stderr, _T(PACKAGE_NAME) _T(": type TYPE missing."));
					return 1;
				}
			}

			size_t len = _tcslen(arg);
			for (c=1 ; c < len ; c++) {
				for (m = 0; m < OPT_LEN; m++) {
					if (arg[c] == opts[m].shortName) {
						modeFlags |= opts[m].code;
						break;
					}
				}
			}
		}
		else if (argv[n][0] == 0) {
			continue;
		}
		else if (!pszPathIn[0]) {
			/* copy path from parameter. */
			_tcscpy(pszPathIn, argv[n]);
		}

	}

	/* mode is print system information. */
	if (modeFlags & 0xFF00 && GetShellFolderPath(modeFlags, pszPathIn)) {
		_ftprintf(stderr, _T(PACKAGE_NAME) _T(": cannot query system folder."));
		return 1;
	}
	/* mode piped input. */
	else if (!pszPathIn[0] && 0 < _filelengthi64(STDIN_FILENO)) {
        _fgetts(pszPathIn, sizeof(pszPathIn), stdin);
	}
	/* no input. */
	else if (!pszPathIn[0]) {
		_ftprintf(stderr, _T(PACKAGE_NAME) _T(": no input."));
		return 1;
	}

	/* convert seperater '\\' -> '/'. */
	convertSeperator(pszPathIn, sizeof(pszPathIn), _T('\\'), _T('/'));

	/* fix style '/c/path/to/dir' -> 'C:/path/to/dir'. */
	if (pszPathIn[0] == _T('/') && _istlower(pszPathIn[1]) && pszPathIn[2] == _T('/')) {
		pszPathIn[0] = _totupper(pszPathIn[1]);
		pszPathIn[1] = _T(':');
	}
	/* fix style 'c:/path/to/dir' -> 'C:/path/to/dir'. */
	else if (_istlower(pszPathIn[0]) && pszPathIn[1] == _T(':')) {
		pszPathIn[0] = _totupper(pszPathIn[0]);
	}
	/* convert to absolute path. */
	else if (modeFlags & OF_ABSOLUTE) {
		/* get full path, with auto allocation. */
		LPTSTR pszTemp = _tfullpath(NULL, pszPathIn, 0);
		_tcscpy(pszPathIn, pszTemp);
		free(pszTemp);

		/* convert seperater '\\' -> '/'. */
		convertSeperator(pszPathIn, sizeof(pszPathIn), _T('\\'), _T('/'));
	}

	TCHAR pszPathOut[MAX_PATH + 1];

	/* mode 'm' or 'w' only. */
	if (modeFlags & 4) {

		/* mode is print dos filename. */
		if (modeFlags & OF_SHORTNAME
			&& !GetShortPathName(pszPathIn, pszPathOut, sizeof(pszPathOut))) {
			_ftprintf(stderr, _T(PACKAGE_NAME) _T(": cannot create short name of %s."), pszPathIn);
			return 1;
		}
		else if (modeFlags & OF_LONGNAME
			&& !GetLongPathName(pszPathIn, pszPathOut, sizeof(pszPathOut))) {
			_ftprintf(stderr, _T(PACKAGE_NAME) _T(": cannot create long name of %s."), pszPathIn);
			return 1;
		}
		else {
			_tcscpy(pszPathOut, pszPathIn);
		}
	}
	/* mode 'unix' and starts with 'C:'. */
	else if (_istalpha(pszPathIn[0]) && pszPathIn[1] == _T(':')) {

		/* get Bash root from ENV ( C:/MinGW/msys/1.0 ). */
		TCHAR pszBashRoot[MAX_PATH + 1];
		if (getBashRoot(pszBashRoot, sizeof(pszBashRoot))) {
			_ftprintf(stderr, _T(PACKAGE_NAME) _T(": unknown error, you may not using mingw."));
			return 1;
		}

		/* get a length of bash root directory.  */
		size_t rootLength = _tcslen(pszBashRoot);

		LPTSTR pszTemp;

		/* fix bash root. */
		if (_tcsstr(pszPathIn, pszBashRoot) == pszPathIn) {

			pszTemp = &pszPathIn[rootLength];

			if (*pszTemp == 0 || (*pszTemp == _T('/') && *(pszTemp + 1) == 0)) {
				_tcscpy(pszPathOut, _T("/"));
			}
			else if (*pszTemp == _T('/')) {
				if (_tcsstr(++pszTemp, _T("etc")) == pszTemp
					|| _tcsstr(pszTemp, _T("home")) == pszTemp
					|| _tcsstr(pszTemp, _T("var")) == pszTemp
				) {
					_tcscpy(pszPathOut, --pszTemp);
				}
				else {
					_tcscpy(pszPathOut, _T("/usr"));
					_tcscpy(&pszPathOut[4], --pszTemp);
				}
			}
			else {
				_tcscpy(pszPathOut, pszPathIn);
			}
		}
		else {
			pszPathIn[1] = _totlower(pszPathIn[0]);
			pszPathIn[0] = _T('/');
			_tcscpy(pszPathOut, pszPathIn);
		}
	}

	/* convert '/' -> '\\'. */
	if (modeFlags & 2) {
		convertSeperator(pszPathOut, sizeof(pszPathOut), _T('/'), _T('\\'));
	}

	/* generate final output. */
	_ftprintf(stdout, pszPathOut);

    return 0;
}

