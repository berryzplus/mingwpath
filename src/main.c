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


void printHelp();
int  parseArg(int *pModeFlags, LPTSTR pszPath, int argc, TCHAR* argv[]);
int  convPath(int modeFlags, LPTSTR pszPath, LPTSTR pszRoot);


int _tmain(int argc, TCHAR* argv[]) {

	int ret, modeFlags;
	LPTSTR pszPath, pszRoot;

	/** print help */
	if (argc < 2 || !_tcscmp(argv[1], _T("-h"))) {
		printHelp();
		return 0;
	}

	/* allocate buffer for input. */
	pszPath = (LPTSTR) malloc(BUFFER_LENGTH * sizeof(TCHAR));

	/** parse args */
	modeFlags = MF_UNIXNAME;
	ret = parseArg(&modeFlags, pszPath, argc, argv);
	if (!ret) {
		/* allocate buffer for virtual unix root. */
		pszRoot = (LPTSTR) malloc(BUFFER_LENGTH * sizeof(TCHAR));

		/* execute conversion phase. */
		ret = convPath(modeFlags, pszPath, pszRoot);
		if (!ret) {
			/* generate final output. */
			_ftprintf(stdout, pszPath);
		}

		/* free buffer for virtual unix root. */
		free(pszRoot);
	}

	/* free buffer for input. */
	free(pszPath);

    return ret;
}


#ifdef __cplusplus
}
#endif
