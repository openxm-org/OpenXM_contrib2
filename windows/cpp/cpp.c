// cpp.cpp : アプリケーション用のエントリ ポイントの定義
//

#include "stdafx.h"

extern int __argc;
extern char **__argv;

void _setargv();

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow )
{
 	// TODO: この位置にコードを記述してください。
	_setargv();

	cpp_main(__argc,__argv);
	return 0;
}



