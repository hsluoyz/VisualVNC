; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CVisualVNCView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "VisualVNC.h"
LastPage=0

ClassCount=7
Class1=CVisualVNCApp
Class2=CVisualVNCDoc
Class3=CVisualVNCView
Class4=CMainFrame
Class12=COutputBar

ResourceCount=12
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource7=IDD_VISUALVNC_FORM
Resource8=IDR_THEME_MENU (English (U.S.))
Resource9=IDD_ABOUTBOX (English (U.S.))
Resource10=IDR_CONTEXT_MENU (English (U.S.))
Resource11=IDR_PASTE_MENU (English (U.S.))
Class5=CAboutDlg
Class6=CWorkspaceBar
Class7=CWorkspaceBar2
Resource12=IDR_MAINFRAME (English (U.S.))

[CLS:CVisualVNCApp]
Type=0
HeaderFile=VisualVNC.h
ImplementationFile=VisualVNC.cpp
Filter=N

[CLS:CVisualVNCDoc]
Type=0
HeaderFile=VisualVNCDoc.h
ImplementationFile=VisualVNCDoc.cpp
Filter=N

[CLS:CVisualVNCView]
Type=0
HeaderFile=VisualVNCView.h
ImplementationFile=VisualVNCView.cpp
Filter=D
BaseClass=CFormView
VirtualFilter=VWC
LastObject=CVisualVNCView


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
BaseClass=CFrameWnd
VirtualFilter=fWC
LastObject=CMainFrame




[CLS:CAboutDlg]
Type=0
HeaderFile=VisualVNC.cpp
ImplementationFile=VisualVNC.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg

[CLS:CWorkspaceBar]
Type=0
HeaderFile=WorkspaceBar.h
ImplementationFile=WorkspaceBar.cpp
Filter=W

[CLS:CWorkspaceBar2]
Type=0
HeaderFile=WorkspaceBar2.h
ImplementationFile=WorkspaceBar2.cpp
Filter=W

[CLS:COutputBar]
Type=0
HeaderFile=OutputBar.h
ImplementationFile=OutputBar.cpp
Filter=W

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_PRINT
Command6=ID_FILE_PRINT_PREVIEW
Command7=ID_FILE_PRINT_SETUP
Command8=ID_FILE_MRU_FILE1
Command9=ID_APP_EXIT
Command10=ID_EDIT_UNDO
Command11=ID_EDIT_CUT
Command12=ID_EDIT_COPY
Command13=ID_EDIT_PASTE
Command14=ID_VIEW_TOOLBAR
Command15=ID_VIEW_STATUS_BAR
CommandCount=16
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command16=ID_APP_ABOUT

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
CommandCount=14
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE

[DLG:IDD_VISUALVNC_FORM]
Type=1
Class=CVisualVNCView
ControlCount=2
Control1=IDC_EDIT_POLICY,edit,1350635588
Control2=IDC_EDIT_CONSOLE,edit,1350635588

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_PRINT
Command6=ID_FILE_PRINT_PREVIEW
Command7=ID_FILE_PRINT_SETUP
Command8=ID_FILE_MRU_FILE1
Command9=ID_APP_EXIT
Command10=ID_EDIT_UNDO
Command11=ID_EDIT_CUT
Command12=ID_EDIT_COPY
Command13=ID_EDIT_PASTE
Command14=ID_VIEW_STATUS_BAR
Command15=ID_VIEW_APPLOOK_2000
Command16=ID_VIEW_APPLOOK_XP
Command17=ID_VIEW_APPLOOK_2003
Command18=ID_VIEW_APPLOOK_VS2005
Command19=ID_VIEW_APPLOOK_WIN_XP
Command20=ID_VIEW_APPLOOK_2007
Command21=ID_VIEW_APPLOOK_2007_1
Command22=ID_VIEW_APPLOOK_2007_2
Command23=ID_VIEW_APPLOOK_2007_3
Command24=ID_VIEW_APPLOOK_VS2008
Command25=ID_VIEW_APPLOOK_VS2010
Command26=ID_VIEW_APPLOOK_2010_1
Command27=ID_VIEW_APPLOOK_2010_2
Command28=ID_VIEW_APPLOOK_2010_3
Command29=ID_VIEW_APPLOOK_WIN7
Command30=ID_VIEW_APPLOOK_VS2012_LIGHT
Command31=ID_VIEW_APPLOOK_VS2012_DARK
Command32=ID_APP_ABOUT
CommandCount=32

[MNU:IDR_CONTEXT_MENU (English (U.S.))]
Type=1
Class=?
Command1=ID_EDIT_CUT
Command2=ID_EDIT_COPY
Command3=ID_EDIT_PASTE
CommandCount=3

[MNU:IDR_PASTE_MENU (English (U.S.))]
Type=1
Class=?
Command1=ID_EDIT_PASTE
Command2=ID_EDIT_PASTE_SPECIAL
CommandCount=2

[MNU:IDR_THEME_MENU (English (U.S.))]
Type=1
Class=?
Command1=ID_VIEW_APPLOOK_2007
Command2=ID_VIEW_APPLOOK_2007_1
Command3=ID_VIEW_APPLOOK_2007_2
Command4=ID_VIEW_APPLOOK_2007_3
Command5=ID_VIEW_APPLOOK_2010_1
Command6=ID_VIEW_APPLOOK_2010_2
Command7=ID_VIEW_APPLOOK_2010_3
Command8=ID_VIEW_APPLOOK_WIN7
CommandCount=8

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_COMPANY_URL,button,1342242816

