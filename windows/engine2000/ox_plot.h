// ox_plot.h : OX_PLOT アプリケーションのメイン ヘッダー ファイル
//

#if !defined(AFX_OX_PLOT_H__59DD85A9_D89D_4970_A079_E159432A107F__INCLUDED_)
#define AFX_OX_PLOT_H__59DD85A9_D89D_4970_A079_E159432A107F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // メイン シンボル

/////////////////////////////////////////////////////////////////////////////
// COx_plotApp:
// このクラスの動作の定義に関しては ox_plot.cpp ファイルを参照してください。
//

class COx_plotApp : public CWinApp
{
public:
	COx_plotApp();

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(COx_plotApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// インプリメンテーション

public:
	//{{AFX_MSG(COx_plotApp)
	afx_msg void OnAppAbout();
	afx_msg void OnAppExit();
	//}}AFX_MSG
	afx_msg void OnFilePrintSetup();
	DECLARE_MESSAGE_MAP()
};

extern COx_plotApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_OX_PLOT_H__59DD85A9_D89D_4970_A079_E159432A107F__INCLUDED_)
