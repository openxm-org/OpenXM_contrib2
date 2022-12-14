// asir32guiView.cpp : CAsir32guiView クラスの動作の定義を行います。
//

#include "stdafx.h"

#include "asir32gui.h"
#include "asir32guiDoc.h"
#include "asir32guiView.h"
#include "FatalDialog.h"
#include <direct.h>
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// const TextBufferSize     = 32768;
const int TextBufferSize     = 16384;

CAsir32guiView *theView;

extern "C"
{
char *prev_hist(void), *next_hist(void), *search_hist(char *p);
void read_input_history(),write_input_history();
void init_input_history();
void add_hist(char *p), reset_traverse(void);
void asir_terminate(void);
void Main(int,char*[]);
void loadfile(char *);
void put_line(char *);
void send_intr(void);
void insert_to_theView(char *);
BOOL get_rootdir(char *,int,char *);
const char *get_locale_lang_iso639();
const char *get_helpdir();
extern int bigfloat, prtime, prresult;
extern char *asir_libdir;
extern int asirgui_kind;

void insert_to_theView(char *str)
{
	(void)theView->Insert(str);
}
void flush_log()
{
	if ( theView->Logging )
		theView->OnFileLog();
}
}

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiView

IMPLEMENT_DYNCREATE(CAsir32guiView, CEditView)

BEGIN_MESSAGE_MAP(CAsir32guiView, CEditView)
	//{{AFX_MSG_MAP(CAsir32guiView)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_LOG, OnFileLog)
	ON_COMMAND(ID_ASIRHELP, OnAsirhelp)
	ON_COMMAND(ID_VISIT_SITE, OnVisitSite)
	ON_UPDATE_COMMAND_UI(ID_FILE_LOG, OnUpdateFileLog)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(IDR_FONT, OnFont)
	ON_WM_CREATE()
	ON_COMMAND(ID_CONTRIBHELP, OnContribhelp)
	ON_WM_SIZE()
	ON_COMMAND(ID_DSOLV, OnDsolv)
	ON_COMMAND(ID_GNUPLOT, OnGnuplot)
	ON_COMMAND(ID_OKDIFF, OnOkdiff)
	ON_COMMAND(ID_OKDMODULE, OnOkdmodule)
	ON_COMMAND(ID_OM, OnOm)
	ON_COMMAND(ID_PFPCOH, OnPfpcoh)
	ON_COMMAND(ID_PLUCKER, OnPlucker)
	ON_COMMAND(ID_SM1, OnSm1)
	ON_COMMAND(ID_TIGERS, OnTigers)
	ON_COMMAND(ID_TODOPARAMETRIZE, OnTodoparametrize)
	ON_COMMAND(ID_NOROMATRIX, OnNoromatrix)
	ON_COMMAND(ID_NOROMWL, OnNoromwl)
	ON_COMMAND(ID_NNNDBF, OnNnndbf)
	ON_COMMAND(ID_NOROPD, OnNoropd)
	ON_COMMAND(ID_NSTWLG, OnNstwistedlog)
	//}}AFX_MSG_MAP
	// 標準印刷コマンド
//	ON_COMMAND(ID_FILE_PRINT, CEditView::OnFilePrint)
//	ON_COMMAND(ID_FILE_PRINT_DIRECT, CEditView::OnFilePrint)
//	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiView クラスの構築/消滅

CAsir32guiView::CAsir32guiView()
{
	// TODO: この場所に構築用のコードを追加してください。
	static int tmpView_created = 0;

	init_input_history();
	read_input_history();
	DebugMode = 0;
	DebugInMain = 0;
//	MaxLineLength = 80;
	LogStart = 0;
	Logging = 0;
	Logfp = NULL;
	ResetIndex();
	theView = this;
#if _MSC_VER > 1300
	/* VS2003.NET or later */
	(AfxGetApp())->EnableHtmlHelp();
#endif
}

CAsir32guiView::~CAsir32guiView()
{
	write_input_history();
}

BOOL CAsir32guiView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//       修正してください。

	m_dwDefaultStyle = dwStyleDefault & ~ES_AUTOHSCROLL ;
	m_dwDefaultStyle &= ~WS_HSCROLL;
	return CCtrlView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiView クラスの描画

void CAsir32guiView::OnDraw(CDC* pDC)
{
	CAsir32guiDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: この場所にネイティブ データ用の描画コードを追加します。
}

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiView クラスの印刷

BOOL CAsir32guiView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// デフォルトの印刷準備
	return DoPreparePrinting(pInfo);
}

void CAsir32guiView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷前の特別な初期化処理を追加してください。
}

void CAsir32guiView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷後の後処理を追加してください。
}

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiView クラスの診断

#ifdef _DEBUG
void CAsir32guiView::AssertValid() const
{
	CEditView::AssertValid();
}

void CAsir32guiView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}

CAsir32guiDoc* CAsir32guiView::GetDocument() // 非デバッグ バージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAsir32guiDoc)));
	return (CAsir32guiDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiView クラスのメッセージ ハンドラ

void CAsir32guiView::PutChar(int c)
{
  int i,len;
  char buf[2];

  if ( EndPos >= sizeof(Buffer)-1 ) {
      Beep(); return;
  }
  for ( i = EndPos-1; i >= CurrentPos; i-- )
      Buffer[i+1] = Buffer[i];
  Buffer[CurrentPos] = (char)c;
//  buf[0] = (char)c; buf[1] = 0;
//  Insert(buf,FALSE);
  len = GetWindowTextLength();
  if ( len+1 >= TextBufferSize )
      DeleteTop();
  CEditView::OnChar(c, 1, 0);
  CurrentPos++;
  EndPos++;
}

void CAsir32guiView::DeleteChar(int count) {
  int i,s,e;
  char null = 0;

  if ( count > (EndPos-CurrentPos) )
    count = EndPos - CurrentPos;
  if ( count > 0 ) {
    EndPos -= count;
    for ( i = CurrentPos; i < EndPos; i++ )
      Buffer[i] = Buffer[i+count];
    GetEditCtrl().GetSel(s,e);
    GetEditCtrl().SetSel(s,s+count);
    GetEditCtrl().ReplaceSel(&null);
  }
}

void CAsir32guiView::Insert(char *buf,BOOL wrap)
{
	char t[BUFSIZ*4];
	char *p,*q,c;
	int len;
	int i;

	if ( wrap ) {
		i = GetEditCtrl().LineLength(-1);
		for ( p = t, q = buf; (c = *p++ = *q++) != 0; )
			if ( c == '\r' ) {
				*p++ = *q++; i = 0;
			} else if ( (*q != '\r') && (++i == MaxLineLength) ) {
				*p++ = '\r'; *p++ = '\r'; *p++ = '\n'; i = 0;
			}
		p = t;
	} else
		p = buf;
	len = GetWindowTextLength();
	if ( strlen(p) + len >= TextBufferSize )
		DeleteTop();
	GetEditCtrl().ReplaceSel(p);
}

void CAsir32guiView::DeleteTop()
{
  const char *p,*q;
  const char *buf;
  char null = 0;
  int len,len0;

  buf = LockBuffer();
  len0 = len = strlen(buf);
  for ( p = buf; len >= TextBufferSize/2; ) {
    q = strchr(p,'\n');
    if ( !q ) {
       len = 0; break;
    } else {
      q++; len -= (q-p); p = q;
    }
  }
  if ( Logging ) {
    if ( len < LogStart )
    	LogStart -= len;
    else {
    	fwrite(buf+LogStart,1,len-LogStart,Logfp);
    	LogStart = 0;	
    }
  }
  UnlockBuffer();
  if ( q ) {
	GetEditCtrl().SetSel(0,len);
	GetEditCtrl().ReplaceSel(&null);
  }
  UpdateCursor(EndPos);
}

void CAsir32guiView::UpdateCursor(int pos)
{
  int s;
  int len;

  if ( pos < 0 || pos > EndPos )
    Beep();
  else {
    len = GetWindowTextLength();
    CurrentPos = pos;
    s = len-EndPos+CurrentPos;
    GetEditCtrl().SetSel(s,s);
  }
}

void CAsir32guiView::ReplaceString(char *str)
{
  UpdateCursor(0); DeleteChar(EndPos);
  Insert(str,FALSE);
  strcpy(Buffer,str);
  EndPos = CurrentPos = strlen(str);
}

void CAsir32guiView::Beep(void) {
  ::MessageBeep(0xffffffff);
}

void CAsir32guiView::Paste(void) {
    char buf[2*BUFSIZ];
    const char *src;
    char c;
    int i,j,l;
    HGLOBAL hClip;

    if ( asirgui_kind == ASIRGUI_MESSAGE ) {
        Beep(); return;
    }

    if ( OpenClipboard() == FALSE ) {
        Beep(); return;
    }
    hClip = GetClipboardData(CF_TEXT);
    src = (const char *)::GlobalLock(hClip);
    if ( !src || (l=strlen(src)) >= sizeof(Buffer)-EndPos ) {
    	::CloseClipboard();
    	Beep(); return;
    }
    for ( i = j = 0; i < l; i++ )
        if ( (c = src[i]) != '\n' && c != '\r' )
             buf[j++] = c;
    buf[j] = 0;
    ::GlobalUnlock(hClip);
    ::CloseClipboard();
    GetEditCtrl().ReplaceSel(buf);
    l = strlen(buf);
    for ( i = EndPos-1; i >= CurrentPos; i-- )
	Buffer[i+l] = Buffer[i];
    for ( j = 0, i = CurrentPos; j < l; j++, i++ )
	Buffer[i] = buf[j];
    EndPos += l; CurrentPos += l;
}

#define CTRL(c) (c)&0x1f

void CAsir32guiView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	char *exp_result;

	switch ( nChar ) {
		case VK_LEFT:
			UpdateCursor(CurrentPos-1); return;
		case VK_RIGHT:
			UpdateCursor(CurrentPos+1); return;
		case VK_DELETE:
			DeleteChar(1); return;
		case VK_UP:
			if ( (exp_result = prev_hist()) != 0 )
				ReplaceString(exp_result);
			else
				Beep();
			return;
		case VK_DOWN:
			if ( (exp_result = next_hist()) != 0 )
				ReplaceString(exp_result);
			else
				Beep();
			return;
		default:
			break;
	}
	CEditView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CAsir32guiView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	char *exp_result;
	char tmp[BUFSIZ];
	int i;
	
	if ( asirgui_kind == ASIRGUI_MESSAGE ) {
		Beep(); return;
	}

	switch ( nChar ) {
	case CTRL('M'): case CTRL('J'):
		UpdateCursor(EndPos);
		Buffer[EndPos] = 0;
		reset_traverse();
		if ( Buffer[0] == '!' ) {
		exp_result = search_hist(Buffer+1);
		if ( exp_result )
			ReplaceString(exp_result);
		else {
			UpdateCursor(0); DeleteChar(EndPos); Beep();
		}
		return;
		} else {
			add_hist(Buffer);
			put_line(Buffer);
			ResetIndex();
			nChar = CTRL('M');
		}
		break;
	case CTRL('N'):
		if ( (exp_result = next_hist()) != 0 )
		ReplaceString(exp_result);
		else
		Beep();
		return;
	case CTRL('P'):
		if ( (exp_result = prev_hist()) != 0 )
		ReplaceString(exp_result);
		else
		Beep();
		return;
	case CTRL('A'):
		UpdateCursor(0); return;
	case CTRL('E'):
		UpdateCursor(EndPos); return;
	case CTRL('F'):
		UpdateCursor(CurrentPos+1); return;
	case CTRL('B'):
		UpdateCursor(CurrentPos-1); return;
	case CTRL('K'):
		DeleteChar(EndPos-CurrentPos); return;
	case CTRL('U'):
		UpdateCursor(0); DeleteChar(EndPos); return;
	case CTRL('D'):
		DeleteChar(1); return;
	case CTRL('H'):
		if ( CurrentPos > 0 ) {
		UpdateCursor(CurrentPos-1); DeleteChar(1);
		} else
		Beep();
		return;
	case CTRL('C'):
		send_intr();
		/* wake up the engine */
		tmp[0] = 0;
		put_line(tmp);
		return;
	case CTRL('Y'):
		Paste(); return;
	case CTRL('I'):
		return;
	default :
		if ( nChar >= 0x20 && nChar < 0x7f )
			PutChar(nChar);
		else
			Beep();
		return;
	}
	CEditView::OnChar(nChar, nRepCnt, nFlags);
}

void CAsir32guiView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
    GetEditCtrl().Copy(); UpdateCursor(EndPos);
	CEditView::OnLButtonUp(nFlags, point);
}

void CAsir32guiView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
    Paste();
	
//	CEditView::OnRButtonDown(nFlags, point);
}

void CAsir32guiView::OnFileOpen() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	char cmd[BUFSIZ*2]; // XXX
	char *p;
	static char errmsg[BUFSIZ];
	static char prevdir[BUFSIZ];

	if ( !prevdir[0] ) {
		get_rootdir(prevdir,sizeof(prevdir),errmsg);
		strcat(prevdir,"\\lib");	
	}

	_chdir(prevdir);
	CFileDialog	fileDialog(TRUE);
	fileDialog.m_ofn.lpstrInitialDir = prevdir;
	if ( fileDialog.DoModal() == IDOK ) {
		CString pathName = fileDialog.GetPathName();
		sprintf(cmd,"load(\"%s\");",pathName);
		for ( p = cmd; *p; p++ )
			if ( *p == '\\' )
				*p = '/';
		put_line(cmd);
		strcpy(cmd,pathName);
		p = strrchr(cmd,'\\');
		if ( p ) {
			*p = 0;
			strcpy(prevdir,cmd);
		}
	}
}

void CAsir32guiView::viewHtmlHelp(char *help)
{
	char *helpfile;
	char fmt[] = "%s\\%shelp.chm";
	const char *dir = get_helpdir();
	helpfile = (char *)malloc(sizeof(fmt) + strlen(dir) + strlen(help));
	sprintf(helpfile,fmt,dir,help);
#if _MSC_VER < 1300
    /* Visual C++ 6.0 */
	::HtmlHelp(NULL, helpfile, HH_DISPLAY_TOPIC, 0);
#else
	CWinApp *top = AfxGetApp();
	top->SetHelpMode(AFX_HELP_TYPE::afxHTMLHelp);
	if (top->m_pszHelpFilePath) {
		free((void*)top->m_pszHelpFilePath);
	}
	top->m_pszHelpFilePath = helpfile;
	HtmlHelp(HH_DISPLAY_TOPIC, 0);
#endif
}

void CAsir32guiView::viewWeb(char *url)
{
	ShellExecute(NULL,"open",url,NULL,NULL,SW_SHOWNORMAL);
}

void CAsir32guiView::viewWebHelp(char *key)
{
	char *base="http://www.math.kobe-u.ac.jp/OpenXM/Current/doc/";
	char *url = (char *)malloc(strlen(base)+strlen(key)+1);
	sprintf(url,"%s%s",base,key);
	viewWeb(url);
	free(url);
}

void CAsir32guiView::OnAsirhelp() 
{
	const char *lang = get_locale_lang_iso639();
	if(strcmp(lang,"ja")==0) {
		viewWebHelp("index-doc-asir-ja.html");
	}else {
		viewWebHelp("index-doc-asir.html");
	}
}

void CAsir32guiView::OnVisitSite() 
{
	const char *lang = get_locale_lang_iso639();
	if(strcmp(lang,"ja")==0) {
		viewWeb("http://www.math.kobe-u.ac.jp/Asir/asir-ja.html");
	}else {
		viewWeb("http://www.math.kobe-u.ac.jp/Asir/");
	}
}

void CAsir32guiView::OnFileLog() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if ( !Logging ) {
		CFileDialog	fileDialog(TRUE);
		if ( fileDialog.DoModal() == IDOK ) {
			CString logfile = fileDialog.GetPathName();
			Logfp = fopen(logfile,"ab");
			if ( Logfp ) {
				LogStart = GetWindowTextLength();
				Logging = 1;
			}
		}
	} else {
		const char *buf;
		buf = LockBuffer();
		fwrite(buf+LogStart,1,strlen(buf)-LogStart,Logfp);
		UnlockBuffer();
		fclose(Logfp);
		Logging = 0;
	}
}

void CAsir32guiView::OnUpdateFileLog(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	if ( Logging ) 
		pCmdUI->SetCheck(1);	
	else
		pCmdUI->SetCheck(0);	
}

void CAsir32guiView::OnEditPaste() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
    Paste();
}

void CAsir32guiView::UpdateMetrics()
{
   TEXTMETRIC tm;
   RECT r;

   GetEditCtrl().GetRect(&r);
   CDC *pDC = GetDC();
   pDC->SelectObject(GetFont());
   pDC->GetTextMetrics(&tm);
   MaxLineLength = (r.right-r.left)/tm.tmAveCharWidth-1;
}

void CAsir32guiView::OnFont() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	INT_PTR ret;
	LOGFONT lf;

	GetFont()->GetLogFont(&lf);
	CFontDialog fd(&lf,CF_EFFECTS | CF_SCREENFONTS | CF_FIXEDPITCHONLY);
	static CFont *f = 0;

	ret = fd.DoModal();
	if ( ret == IDOK ) {
		fd.GetCurrentFont(&lf);
		if ( f )
			delete f;
		f = new CFont;
		f->CreateFontIndirect(&lf);
		SetFont(f);
		UpdateMetrics();
	}
}

int CAsir32guiView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEditView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: この位置に固有の作成用コードを追加してください

	LOGFONT logFont; memset(&logFont, 0, sizeof(LOGFONT));
	logFont.lfHeight = 20;
	logFont.lfCharSet = DEFAULT_CHARSET;
	strcpy(logFont.lfFaceName, "Terminal");
	CFont *f = new CFont;
	f->CreateFontIndirect(&logFont);
	SetFont(f);
	UpdateMetrics();
	GetEditCtrl().LimitText(nMaxSize);
	GetEditCtrl().SetTabStops(m_nTabStops);	

	return 0;
}

void CAsir32guiView::OnContribhelp() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	viewHtmlHelp("cman");
}

void CAsir32guiView::OnSize(UINT nType, int cx, int cy) 
{
	CEditView::OnSize(nType, cx, cy);
	
	// TODO: この位置にメッセージ ハンドラ用のコードを追加してください
	UpdateMetrics();
}

void CAsir32guiView::OnDsolv() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	viewHtmlHelp("dsolv");
}

void CAsir32guiView::OnGnuplot() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	viewHtmlHelp("gnuplot");
}

void CAsir32guiView::OnOkdiff() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	viewHtmlHelp("ok_diff");
}

void CAsir32guiView::OnOkdmodule() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	viewHtmlHelp("ok_dmodule");
}

void CAsir32guiView::OnOm() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	viewHtmlHelp("om");
}

void CAsir32guiView::OnPfpcoh() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	viewHtmlHelp("pfpcoh");
}

void CAsir32guiView::OnPlucker() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	viewHtmlHelp("plucker");
}

void CAsir32guiView::OnSm1() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	viewHtmlHelp("sm1");
}

void CAsir32guiView::OnTigers() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	viewHtmlHelp("tigers");
}

void CAsir32guiView::OnTodoparametrize() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	viewHtmlHelp("todo_parametrize");
}

void CAsir32guiView::OnNoromatrix() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	viewHtmlHelp("noro_matrix");
}

void CAsir32guiView::OnNoromwl() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	viewHtmlHelp("noro_mwl");
}

void CAsir32guiView::OnNoropd() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	viewHtmlHelp("noro_pd");
}

void CAsir32guiView::OnNnndbf() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	viewHtmlHelp("nn_ndbf");
}

void CAsir32guiView::OnNstwistedlog() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	viewHtmlHelp("ns_twistedlog");
}

BOOL CAsir32guiView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	
	BOOL ret = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	FILE *fp = NULL;
	char *temp;
	char hndname[_MAX_PATH];

	if ( asirgui_kind == ASIRGUI_MAIN ) {
		if ( temp = getenv("TEMP") ) {
			sprintf(hndname,"%s\\asirgui.hnd", temp);
			fp = fopen(hndname,"w");
			if ( fp ) {
 				fprintf(fp,"%d",(unsigned int)theView->m_hWnd);
 				fclose(fp);
			}
		}
	}
	return ret;
}
