// winfepDlg.h : ヘッダー ファイル
//

#if !defined(AFX_WINFEPDLG_H__E284CDDE_5B01_4A61_A9D5_43C3E6FC5CFE__INCLUDED_)
#define AFX_WINFEPDLG_H__E284CDDE_5B01_4A61_A9D5_43C3E6FC5CFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CWinfepDlg ダイアログ

class CWinfepDlg : public CDialog
{
// 構築
public:
	CWinfepDlg(CWnd* pParent = NULL);	// 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CWinfepDlg)
	enum { IDD = IDD_WINFEP_DIALOG };
	CEdit	m_currentline;
	CStatic	m_after;
	CStatic	m_before;
	CStatic	m_currentfile;
	//}}AFX_DATA

	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CWinfepDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV のサポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	HICON m_hIcon;

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CWinfepDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSend();
	afx_msg void OnChangeFilename();
	afx_msg void OnFileopen();
	afx_msg void OnAsir();
	afx_msg void OnSkip();
	afx_msg void OnNext();
	afx_msg void OnPrev();
	afx_msg void OnPrev10();
	afx_msg void OnNext10();
	virtual void OnOK();
	afx_msg void OnQuit();
	afx_msg void OnIntr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void show_line(int);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_WINFEPDLG_H__E284CDDE_5B01_4A61_A9D5_43C3E6FC5CFE__INCLUDED_)
