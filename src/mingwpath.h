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

#include "config.h"


#define MF_UNIXNAME		0x0000
#define MF_MIXEDNAME	0x0004
#define MF_WINDOWS		0x0006
#define MF_DOSSHORT		0x000E

#define OF_ABSOLUTE		0x0001
#define OF_LONGNAME		0x0010
#define OF_SHORTNAME	0x0008

#define PF_ALLUSERS		0x0400
#define PF_DESKTOP		0x0100
#define PF_HOMEROOT		0x0800
#define PF_SMPROGRA		0x0200
#define PF_SYSTEM32		0x4000
#define PF_WINDOWS		0x2000


typedef struct __mingpath_option {
	int  code;
	TCHAR shortName;
	LPCTSTR longName;
	LPCTSTR description;
} MINGWPATH_OPTION, *LPMINGPATH_OPTION;


#define BUFFER_LENGTH	4096
