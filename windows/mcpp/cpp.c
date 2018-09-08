/* $OpenXM: OpenXM_contrib2/windows/mcpp/cpp.c,v 1.1 2010/02/26 17:42:36 ohara Exp $ */

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif

extern int __argc;
extern char **__argv;

void _setargv();
int main(int ac, char *av[]);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow )
{
#if _MSC_VER < 1900
	_setargv();
#endif
	main(__argc,__argv);
	return 0;
}
