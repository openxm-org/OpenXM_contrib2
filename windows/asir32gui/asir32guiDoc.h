// asir32guiDoc.h : CAsir32guiDoc クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////


class CAsir32guiDoc : public CDocument
{
protected: // シリアライズ機能のみから作成します。
	CAsir32guiDoc();
	DECLARE_DYNCREATE(CAsir32guiDoc)

// アトリビュート
public:

// オペレーション
public:

//オーバーライド
	// ClassWizard は仮想関数を生成しオーバーライドします。
	//{{AFX_VIRTUAL(CAsir32guiDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~CAsir32guiDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG(CAsir32guiDoc)
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

