; CLW ファイルは MFC ClassWizard の情報を含んでいます。

[General Info]
Version=1
LastClass=CAsir32guiView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "asir32gui.h"
LastPage=0

ClassCount=7
Class1=CAsir32guiApp
Class2=CAsir32guiDoc
Class3=CAsir32guiView
Class4=CMainFrame

ResourceCount=6
Resource1=IDD_FORMVIEW
Resource2=IDR_MAINFRAME
Class5=CAboutDlg
Class6=CFatalDialog
Class7=CAsirEdit
Resource3=IDD_DEBUG
Resource4=IDD_ABOUTBOX
Resource5=IDR_DEBUGFRAME
Resource6=IDD_FATAL

[CLS:CAsir32guiApp]
Type=0
HeaderFile=asir32gui.h
ImplementationFile=asir32gui.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC
LastObject=CAsir32guiApp

[CLS:CAsir32guiDoc]
Type=0
HeaderFile=asir32guiDoc.h
ImplementationFile=asir32guiDoc.cpp
Filter=N
BaseClass=CDocument
VirtualFilter=DC
LastObject=CAsir32guiDoc

[CLS:CAsir32guiView]
Type=0
HeaderFile=asir32guiView.h
ImplementationFile=asir32guiView.cpp
Filter=C
LastObject=CAsir32guiView
BaseClass=CEditView
VirtualFilter=VWC

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=CMainFrame
BaseClass=CFrameWnd
VirtualFilter=fWC



[CLS:CAboutDlg]
Type=0
HeaderFile=asir32gui.cpp
ImplementationFile=asir32gui.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_OPEN
Command2=ID_FILE_LOG
Command3=ID_APP_EXIT
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_VIEW_TOOLBAR
Command9=ID_VIEW_STATUS_BAR
Command10=IDR_FONT
Command11=ID_ASIRHELP
Command12=ID_CONTRIBHELP
Command13=ID_APP_ABOUT
CommandCount=13

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_OPEN
Command2=ID_FILE_NEW
Command3=ID_EDIT_PASTE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_NEXT_PANE
Command7=ID_PREV_PANE
Command8=ID_EDIT_COPY
Command9=ID_EDIT_PASTE
Command10=ID_EDIT_CUT
Command11=ID_EDIT_UNDO
CommandCount=11

[TB:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_OPEN
Command2=ID_FILE_LOG
CommandCount=2

[DLG:IDD_DEBUG]
Type=1
Class=?
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_ASIREDIT,edit,1352732740
Control4=IDC_EDIT1,edit,1350631552

[CLS:CAsirEdit]
Type=0
HeaderFile=AsirEdit.h
ImplementationFile=AsirEdit.cpp
BaseClass=CEdit
Filter=W
VirtualFilter=WC
LastObject=CAsirEdit

[DLG:IDD_FORMVIEW]
Type=1
Class=?
ControlCount=2
Control1=IDC_STATIC,static,1342308352
Control2=IDC_EDIT1,edit,1353781444

[DLG:IDD_FATAL]
Type=1
Class=CFatalDialog
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDC_NOTFOUND,static,1342308865

[CLS:CFatalDialog]
Type=0
HeaderFile=FatalDialog.h
ImplementationFile=FatalDialog.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CFatalDialog

[MNU:IDR_DEBUGFRAME]
Type=1
Class=?
Command1=ID_EDIT_UNDO
Command2=ID_EDIT_CUT
Command3=ID_EDIT_COPY
Command4=ID_EDIT_PASTE
Command5=ID_ASIRHELP
Command6=ID_APP_ABOUT
CommandCount=6

[TB:IDR_DEBUGFRAME]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_BUTTON32773
Command3=ID_BUTTON32774
CommandCount=3

[ACL:IDR_DEBUGFRAME]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_FILE_NEW
Command3=ID_EDIT_PASTE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_NEXT_PANE
Command7=ID_PREV_PANE
Command8=ID_EDIT_COPY
Command9=ID_EDIT_PASTE
Command10=ID_EDIT_CUT
Command11=ID_EDIT_UNDO
CommandCount=11

