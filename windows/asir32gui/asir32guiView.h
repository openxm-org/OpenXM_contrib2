// asir32guiView.h : CAsir32guiView クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

class CAsir32guiView : public CEditView
{
public: // シリアライズ機能のみから作成します。
	CAsir32guiView();
	DECLARE_DYNCREATE(CAsir32guiView);

// アトリビュート
public:
	CAsir32guiDoc* GetDocument();

// オペレーション
public:

	int DebugMode;
	int DebugInMain;
	int CurrentPos,EndPos;
	int MaxLineLength;
	int LogStart;
	int Logging;
	FILE *Logfp;
	char Buffer[BUFSIZ*2];

// オーバーライド
	// ClassWizard は仮想関数を生成しオーバーライドします。
	//{{AFX_VIRTUAL(CAsir32guiView)
	public:
	virtual void OnDraw(CDC* pDC);  // このビューを描画する際にオーバーライドされます。
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~CAsir32guiView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void Paste(void);
	void Insert(char *,BOOL wrap = TRUE);
	void ResetIndex(void) {EndPos = CurrentPos = 0;}
	void PutChar(int);
	void DeleteChar(int);
	void DeleteTop(void);
	void UpdateCursor(int);
	void Beep(void);
	void ReplaceString(char *);
	void UpdateLineLength(int);

protected:

// 生成されたメッセージ マップ関数
public:
	//{{AFX_MSG(CAsir32guiView)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnFileOpen();
	afx_msg void OnFileLog();
	afx_msg void OnAsirhelp();
	afx_msg void OnUpdateFileLog(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnFont();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContribhelp();
	afx_msg void OnUpdateContribhelp(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // asir32guiView.cpp ファイルがデバッグ環境の時使用されます。
inline CAsir32guiDoc* CAsir32guiView::GetDocument()
   { return (CAsir32guiDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

