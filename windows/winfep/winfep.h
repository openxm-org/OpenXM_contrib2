// winfep.h : WINFEP アプリケーションのメイン ヘッダー ファイルです。
//

#if !defined(AFX_WINFEP_H__B77E3870_08DD_4C5C_9BF2_1A0C4A8FBACA__INCLUDED_)
#define AFX_WINFEP_H__B77E3870_08DD_4C5C_9BF2_1A0C4A8FBACA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// メイン シンボル

/////////////////////////////////////////////////////////////////////////////
// CWinfepApp:
// このクラスの動作の定義に関しては winfep.cpp ファイルを参照してください。
//

class CWinfepApp : public CWinApp
{
public:
	CWinfepApp();

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CWinfepApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// インプリメンテーション

	//{{AFX_MSG(CWinfepApp)
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_WINFEP_H__B77E3870_08DD_4C5C_9BF2_1A0C4A8FBACA__INCLUDED_)
