// asir32guiDoc.cpp : CAsir32guiDoc クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "asir32gui.h"

#include "asir32guiDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" {
	void flush_log();
	void put_line(char *);
	extern int asirgui_kind;
}

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiDoc

IMPLEMENT_DYNCREATE(CAsir32guiDoc, CDocument)

BEGIN_MESSAGE_MAP(CAsir32guiDoc, CDocument)
	//{{AFX_MSG_MAP(CAsir32guiDoc)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiDoc クラスの構築/消滅

CAsir32guiDoc::CAsir32guiDoc()
{
	// TODO: この位置に１回だけ構築用のコードを追加してください。

}

CAsir32guiDoc::~CAsir32guiDoc()
{
}

BOOL CAsir32guiDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: この位置に再初期化処理を追加してください。
	// (SDI ドキュメントはこのドキュメントを再利用します。)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiDoc シリアライゼーション

void CAsir32guiDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: この位置に保存用のコードを追加してください。
	}
	else
	{
		// TODO: この位置に読み込み用のコードを追加してください。
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiDoc クラスの診断

#ifdef _DEBUG
void CAsir32guiDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAsir32guiDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiDoc コマンド

extern "C" {
	void terminate_asir();
}

BOOL CAsir32guiDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	
	if ( asirgui_kind == ASIRGUI_MESSAGE )
		return FALSE;

	if ( asirgui_kind == ASIRGUI_DEBUG ) {
		put_line("quit");
		return FALSE;
	}

	if( IDNO == ::MessageBox(NULL,"Really quit?","Asir",
		MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON2) ) {
		return FALSE;
	}
	flush_log();
	terminate_asir();
	return TRUE;
//	return CDocument::CanCloseFrame(pFrame);
}

BOOL CAsir32guiDoc::SaveModified() 
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	
	return TRUE;
//	return CDocument::SaveModified();
}
