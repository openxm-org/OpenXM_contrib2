// ChildView.h : CChildView クラスのインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDVIEW_H__1BE7F53D_38AA_4247_893C_9C2D236C7B1F__INCLUDED_)
#define AFX_CHILDVIEW_H__1BE7F53D_38AA_4247_893C_9C2D236C7B1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CChildView ウィンドウ

class CChildView : public CWnd
{
// コンストラクタ
public:
	CChildView();

// アトリビュート
public:
	struct canvas *can;

// オペレーション
public:
	void DestroyCanvas();
	void PrintAxis(CDC &);

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CChildView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~CChildView();
	void OnPrint(CDC &);

	// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG(CChildView)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnOptNoaxis();
	afx_msg void OnUpdateOptNoaxis(CCmdUI* pCmdUI);
	afx_msg void OnOptPrecise();
	afx_msg void OnUpdateOptPrecise(CCmdUI* pCmdUI);
	afx_msg void OnOptWide();
	afx_msg void OnUpdateOptWide(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ が新しい定義を加える場合には、この行よりも前に追加します。

#endif // !defined(AFX_CHILDVIEW_H__1BE7F53D_38AA_4247_893C_9C2D236C7B1F__INCLUDED_)
