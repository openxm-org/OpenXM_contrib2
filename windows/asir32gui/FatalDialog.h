#if !defined(AFX_FATALDIALOG_H__DF52D853_7AB2_11D2_9A87_009027045022__INCLUDED_)
#define AFX_FATALDIALOG_H__DF52D853_7AB2_11D2_9A87_009027045022__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FatalDialog.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CFatalDialog ダイアログ

class CFatalDialog : public CDialog
{
// コンストラクション
public:
	CFatalDialog(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CFatalDialog)
	enum { IDD = IDD_FATAL };
	CString	m_errmsg;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CFatalDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CFatalDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_FATALDIALOG_H__DF52D853_7AB2_11D2_9A87_009027045022__INCLUDED_)
