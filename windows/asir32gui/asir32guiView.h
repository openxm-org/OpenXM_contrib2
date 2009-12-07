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
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
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
	void UpdateMetrics();
	void viewHtmlHelp(char *);

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
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDsolv();
	afx_msg void OnGnuplot();
	afx_msg void OnOkdiff();
	afx_msg void OnOkdmodule();
	afx_msg void OnOm();
	afx_msg void OnPfpcoh();
	afx_msg void OnPlucker();
	afx_msg void OnSm1();
	afx_msg void OnTigers();
	afx_msg void OnTodoparametrize();
	afx_msg void OnNoromatrix();
	afx_msg void OnNoromwl();
	afx_msg void OnNnndbf();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // asir32guiView.cpp ファイルがデバッグ環境の時使用されます。
inline CAsir32guiDoc* CAsir32guiView::GetDocument()
   { return (CAsir32guiDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

