// asir32gui.h : ASIR32GUI アプリケーションのメイン ヘッダー ファイル
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // メイン シンボル

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiApp:
// このクラスの動作の定義に関しては asir32gui.cpp ファイルを参照してください。
//

class CAsir32guiApp : public CWinApp
{
public:
	CAsir32guiApp();
	void CreateNewDoc() { OnFileNew(); }
// オーバーライド
	// ClassWizard は仮想関数を生成しオーバーライドします。
	//{{AFX_VIRTUAL(CAsir32guiApp)
	public:
	virtual BOOL OnIdle(LONG);
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// インプリメンテーション

	//{{AFX_MSG(CAsir32guiApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
#include "asir32gui_io.h"
