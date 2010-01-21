// winfepDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "winfep.h"
#include "winfepDlg.h"
#include <process.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HWND asirhwnd;
FILE *infile;
char *filebuf;
char *before,*after;
int nlines, currentline;
char **lineptr;

extern "C" { 
void get_asirhwnd()
{
	char *temp;
	if ( temp = getenv("TEMP") ) {
		int len;
		char *name;
		FILE *fp;
		len = strlen(temp);
		name = (char *)malloc(len+BUFSIZ);
		sprintf(name,"%s\\asirgui.hnd",temp);
		fp = fopen(name,"r");
		if ( fp ) {
			fscanf(fp,"%d",&asirhwnd);
			fclose(fp);
		}
	}
}


}

/////////////////////////////////////////////////////////////////////////////
// アプリケーションのバージョン情報で使われている CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard は仮想関数のオーバーライドを生成します
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV のサポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// メッセージ ハンドラがありません。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinfepDlg ダイアログ

CWinfepDlg::CWinfepDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWinfepDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWinfepDlg)
	//}}AFX_DATA_INIT
	// メモ: LoadIcon は Win32 の DestroyIcon のサブシーケンスを要求しません。
    get_asirhwnd();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWinfepDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWinfepDlg)
	DDX_Control(pDX, IDC_CURRENTLINE, m_currentline);
	DDX_Control(pDX, IDC_AFTER, m_after);
	DDX_Control(pDX, IDC_BEFORE, m_before);
	DDX_Control(pDX, IDC_CURRENTFILE, m_currentfile);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWinfepDlg, CDialog)
	//{{AFX_MSG_MAP(CWinfepDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(Send, OnSend)
	ON_BN_CLICKED(IDC_FILEOPEN, OnFileopen)
	ON_BN_CLICKED(IDC_BUTTON3, OnAsir)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_PREV, OnPrev)
	ON_BN_CLICKED(IDC_PREV10, OnPrev10)
	ON_BN_CLICKED(IDC_NEXT10, OnNext10)
	ON_BN_CLICKED(IDC_QUIT, OnQuit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinfepDlg メッセージ ハンドラ

BOOL CWinfepDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "バージョン情報..." メニュー項目をシステム メニューへ追加します。

	// IDM_ABOUTBOX はコマンド メニューの範囲でなければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログ用のアイコンを設定します。フレームワークはアプリケーションのメイン
	// ウィンドウがダイアログでない時は自動的に設定しません。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンを設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンを設定
	
	// TODO: 特別な初期化を行う時はこの場所に追加してください。
	
	return TRUE;  // TRUE を返すとコントロールに設定したフォーカスは失われません。
}

void CWinfepDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// もしダイアログボックスに最小化ボタンを追加するならば、アイコンを描画する
// コードを以下に記述する必要があります。MFC アプリケーションは document/view
// モデルを使っているので、この処理はフレームワークにより自動的に処理されます。

void CWinfepDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画用のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// クライアントの矩形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンを描画します。
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// システムは、ユーザーが最小化ウィンドウをドラッグしている間、
// カーソルを表示するためにここを呼び出します。
HCURSOR CWinfepDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CWinfepDlg::OnSend() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	if ( !infile ) return;
    if ( currentline >= 0 && currentline < nlines ) {
	  CString str;
	  m_currentline.GetWindowText(str);
      LPCSTR line = (LPCSTR)str;
      for ( int i = 0; line[i]; i++ ) {
	if ( line[i] == '\t' )
        	::SendMessage(asirhwnd,WM_CHAR,' ',8);
	else
        	::SendMessage(asirhwnd,WM_CHAR,line[i],1);
      }
      ::SendMessage(asirhwnd,WM_CHAR,'\n',1);
      show_line(++currentline);
    } else
	show_line(currentline);
}

void CWinfepDlg::OnChangeFilename() 
{
	// TODO: これが RICHEDIT コントロールの場合、コントロールは、 lParam マスク
	// 内での論理和の ENM_CHANGE フラグ付きで CRichEditCrtl().SetEventMask()
	// メッセージをコントロールへ送るために CDialog::OnInitDialog() 関数をオーバー
	// ライドしない限りこの通知を送りません。
	
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	
	
}

void CWinfepDlg::OnFileopen() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CFileDialog fileDialog(TRUE);
	char *prev;
	struct _stat fs;
	int i,j,n,nl;
	if ( fileDialog.DoModal() == IDOK ) {
		CString pathName = fileDialog.GetPathName();
		if ( infile ) fclose(infile);
		infile = fopen(pathName,"r");
		m_currentfile.SetWindowText(pathName);
		_fstat(_fileno(infile),&fs);
		filebuf = (char *)realloc(filebuf,fs.st_size+1);
		before = (char *)realloc(before,fs.st_size+1);
		after = (char *)realloc(after,fs.st_size+1);
		n = fread(filebuf,1,fs.st_size,infile);
		filebuf[n] = 0;
		for ( nl = 0, i = 0; i < n; i++ )
			if ( filebuf[i] == '\n' ) nl++;
		nlines = nl;
		lineptr = (char **)realloc(lineptr,(nl+1)*sizeof(char *));
		prev = filebuf;
		for ( i = 0, j = 0; i < n; i++ )
			if ( filebuf[i] == '\n' ) {
				filebuf[i] = 0;
				lineptr[j++] = prev;
				prev = filebuf+i+1;
			}
		currentline = 0;
		show_line(0);
	}
}

void CWinfepDlg::OnAsir() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください	CFileDialog fileDialog(TRUE);
	CFileDialog fileDialog(TRUE);
	if ( fileDialog.DoModal() == IDOK ) {
		CString pathName = fileDialog.GetPathName();
		_spawnl(_P_NOWAIT,pathName,pathName,NULL); 
		Sleep(5000);
		get_asirhwnd();
	}
}

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

void CWinfepDlg::show_line(int i)
{
	int j,nafter,nbefore;
	char *cur;

	before[0] = after[0] = 0;
	nbefore = MIN(5,i);
	if ( nbefore < 0 ) nbefore = 0;
	for ( j = 0; j < 5-nbefore; j++ )
		strcat(before,"\n");
	for ( j = i-nbefore; j<i; j++ ) {
		strcat(before,lineptr[j]); strcat(before,"\n");
	} 
	nafter = MIN(5,nlines-i-1);
	if ( nafter < 0 ) nafter = 0;
	for ( j = i+1; j < i+1+nafter; j++ ) {
		strcat(after,lineptr[j]); strcat(after,"\n");
	}
	m_before.SetWindowText(before);
	if ( i < 0 ) cur = "<Beginning of File>";
	else if ( i == nlines ) cur = "<End of File>";
	else cur = lineptr[i];	
	m_currentline.SetWindowText(cur);
	m_after.SetWindowText(after);
}


void CWinfepDlg::OnNext() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	if ( !infile ) return;
    if ( currentline == nlines ) return;
    show_line(++currentline);	
}

void CWinfepDlg::OnPrev() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	if ( !infile ) return;
	if ( currentline < 0 ) return;
	show_line(--currentline);
}

void CWinfepDlg::OnPrev10() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	if ( !infile ) return;
	currentline -= 10;
	if ( currentline < 0 ) currentline = -1;
	show_line(currentline);
}

void CWinfepDlg::OnNext10() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	if ( !infile ) return;
    currentline += 10;
	if ( currentline >= nlines ) currentline = nlines;
	show_line(currentline);
}

void CWinfepDlg::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
    OnSend();
}

void CWinfepDlg::OnQuit() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CDialog::OnOK();
	
}
