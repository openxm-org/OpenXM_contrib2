// FatalDialog.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "asir32gui.h"
#include "FatalDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFatalDialog ダイアログ


CFatalDialog::CFatalDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFatalDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFatalDialog)
	m_errmsg = _T("");
	//}}AFX_DATA_INIT
}


void CFatalDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFatalDialog)
	DDX_Text(pDX, IDC_NOTFOUND, m_errmsg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFatalDialog, CDialog)
	//{{AFX_MSG_MAP(CFatalDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFatalDialog メッセージ ハンドラ

void CFatalDialog::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
	
	CDialog::OnOK();
}

BOOL CFatalDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	SetDlgItemText(IDC_NOTFOUND,m_errmsg);	

	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}
