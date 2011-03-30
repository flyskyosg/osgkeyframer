
// RenderOSG2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "RenderOSG2.h"
#include "RenderOSG2Dlg.h"
#include "afxdialogex.h"

#include "Adddlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()



// CRenderOSG2Dlg dialog
#define TIMER_UPDATECOUNTER_UI 0x001




CRenderOSG2Dlg::CRenderOSG2Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRenderOSG2Dlg::IDD, pParent),
	  m_renderer(0)
	  , m_strElapsedQueueTimeUI(_T(""))
	  , m_strFrameCounter(_T(""))
	  , m_strRemainingFiles(_T(""))
	  , m_OUTPUT(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRenderOSG2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RENDERLIST, m_renderList);
	DDX_Control(pDX, IDC_EDITBTN, m_EditBtn);
	DDX_Control(pDX, IDC_RMBTN, m_RmBtn);
	DDX_Control(pDX, IDC_STARTBTN, m_StartBtn);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressCtrl);
	DDX_Control(pDX, IDC_FRMPREVIEW, m_FramePreviewer);
	DDX_Text(pDX, IDC_QTIME, m_strElapsedQueueTimeUI);
	DDX_Text(pDX, IDC_FRAMECTR_STATIC, m_strFrameCounter);
	DDX_Text(pDX, IDC_REMAININGFILESSTATIC, m_strRemainingFiles);
	DDX_Control(pDX, IDC_ADDBUTTON, m_AddBtn);
	DDX_Text(pDX, IDC_OUTPUTWINDOW, m_OUTPUT);
}

BEGIN_MESSAGE_MAP(CRenderOSG2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ADDBUTTON, &CRenderOSG2Dlg::OnBnClickedAddButton)

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_RENDERLIST, &CRenderOSG2Dlg::OnLvnItemchangedRenderlist)
	ON_NOTIFY(LVN_INSERTITEM, IDC_RENDERLIST, &CRenderOSG2Dlg::OnLvnInsertitemRenderlist)
	ON_NOTIFY(LVN_DELETEITEM, IDC_RENDERLIST, &CRenderOSG2Dlg::OnLvnDeleteitemRenderlist)
	ON_BN_CLICKED(IDC_RMBTN, &CRenderOSG2Dlg::OnBnClickedRmbtn)
	ON_BN_CLICKED(IDC_STARTBTN, &CRenderOSG2Dlg::OnBnClickedStartbtn)
	ON_BN_CLICKED(IDC_EDITBTN, &CRenderOSG2Dlg::OnBnClickedEditbtn)
	ON_MESSAGE(WM_USER_THREAD_FINISHED, OnThreadFinished)
	ON_MESSAGE(WM_USER_THREAD_UPDATE_PROGRESS, OnThreadUpdateProgress)

	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CRenderOSG2Dlg message handlers

BOOL CRenderOSG2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	LVCOLUMN lvColumn;

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_CENTER;
	lvColumn.cx = 150;
	lvColumn.pszText = _T(".earth");
	m_renderList.InsertColumn(0, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 145;
	lvColumn.pszText = _T(".path");
	m_renderList.InsertColumn(1, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 90;
	lvColumn.pszText = _T("Prefix");
	m_renderList.InsertColumn(2, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 150;
	lvColumn.pszText = _T("Output Folder");
	m_renderList.InsertColumn(3, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_RIGHT;
	lvColumn.cx = 40;
	lvColumn.pszText = _T("AA");
	m_renderList.InsertColumn(4, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_RIGHT;
	lvColumn.cx = 60;
	lvColumn.pszText = _T("Format");
	m_renderList.InsertColumn(5, &lvColumn);

	m_renderList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	//Setup Progress bar
	m_ProgressCtrl.SetRange(0,1000);
	m_ProgressCtrl.SetPos(0);

	m_renderer = new CRenderer(m_hWnd);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRenderOSG2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRenderOSG2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRenderOSG2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRenderOSG2Dlg::OnBnClickedAddButton()
{
	// TODO: Add your control notification handler code here

	CAdddlg dlg;

	if( dlg.DoModal() == IDOK)
	{
		LVITEM lvItem;
		int nItem;

		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = 0;
		lvItem.iSubItem = 0;
		lvItem.pszText = dlg.m_strEarthPath.GetBuffer();
		nItem = m_renderList.InsertItem(&lvItem);

		m_renderList.SetItemText(nItem, 1, dlg.m_strPathPath);
		m_renderList.SetItemText(nItem, 2, dlg.m_strPrefix);
		m_renderList.SetItemText(nItem, 3, dlg.m_strSavePath);
		m_renderList.SetItemText(nItem, 4, dlg.m_strAA);
		m_renderList.SetItemText(nItem, 5, dlg.m_strFileType);
	}
}


void CRenderOSG2Dlg::OnLvnItemchangedRenderlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	if(m_renderList.GetSelectedCount())
	{
		m_EditBtn.EnableWindow();
		m_RmBtn.EnableWindow();
	}
	else
	{
		m_EditBtn.EnableWindow(FALSE);
		m_RmBtn.EnableWindow(FALSE);
	}

	*pResult = 0;
}


void CRenderOSG2Dlg::OnLvnInsertitemRenderlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	m_StartBtn.EnableWindow();
	*pResult = 0;
}


void CRenderOSG2Dlg::OnLvnDeleteitemRenderlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	if(m_renderList.GetItemCount() == 1)
		m_StartBtn.EnableWindow(FALSE);

	*pResult = 0;
}


void CRenderOSG2Dlg::OnBnClickedRmbtn()
{
	// TODO: Add your control notification handler code here
	if(m_renderList.GetSelectedCount())
	{
		m_renderList.DeleteItem(m_renderList.GetSelectionMark());
	}
}


void CRenderOSG2Dlg::OnBnClickedStartbtn()
{
	//Disable Buttons
	m_AddBtn.EnableWindow(false);
	m_StartBtn.EnableWindow(false);
	m_EditBtn.EnableWindow(false);
	m_RmBtn.EnableWindow(false);
	m_renderList.EnableWindow(false);

	
	int nItem = 0;

	m_renderer->m_strEarthPath = m_renderList.GetItemText(nItem,0);
	m_renderer->m_strPathPath = m_renderList.GetItemText(nItem,1);
	m_renderer->m_strPrefix = m_renderList.GetItemText(nItem,2);
	m_renderer->m_strSavePath = m_renderList.GetItemText(nItem,3);
	m_renderer->m_strAA = m_renderList.GetItemText(nItem,4);
	m_renderer->m_strFileType = m_renderList.GetItemText(nItem,5);

	m_renderer->m_hThread = AfxBeginThread(CRenderer::ThreadFunc,m_renderer,THREAD_PRIORITY_NORMAL,0,0);

	//Setup timer
	SetTimer(TIMER_UPDATECOUNTER_UI,1000,0);
	m_QueueStartTime = CTime::GetCurrentTime();
	m_strElapsedQueueTimeUI = "00:00:00";
	UpdateData(false);
	//create thread and run render code
}


void CRenderOSG2Dlg::OnBnClickedEditbtn()
{
	// TODO: Add your control notification handler code here

	CAdddlg dlg;

	int nItem = m_renderList.GetSelectionMark();
	dlg.m_strEarthPath = m_renderList.GetItemText(nItem,0);
	dlg.m_strPathPath = m_renderList.GetItemText(nItem,1);
	dlg.m_strPrefix = m_renderList.GetItemText(nItem,2);
	dlg.m_strSavePath = m_renderList.GetItemText(nItem,3);
	dlg.m_strAA = m_renderList.GetItemText(nItem,4);
	dlg.m_strFileType = m_renderList.GetItemText(nItem,5);
	
	if( dlg.DoModal() )
	{
		m_renderList.SetItemText(nItem,0,dlg.m_strEarthPath.GetBuffer());
		m_renderList.SetItemText(nItem,1,dlg.m_strPathPath.GetBuffer());
		m_renderList.SetItemText(nItem,2,dlg.m_strPrefix.GetBuffer());
		m_renderList.SetItemText(nItem,3,dlg.m_strSavePath.GetBuffer());
		m_renderList.SetItemText(nItem,4,dlg.m_strAA.GetBuffer());
		m_renderList.SetItemText(nItem,5,dlg.m_strFileType.GetBuffer());
	}
}

LRESULT CRenderOSG2Dlg::OnThreadFinished(WPARAM wParam,LPARAM lParam)
{
	
	return 0;
}

LRESULT CRenderOSG2Dlg::OnThreadUpdateProgress(WPARAM wParam,LPARAM lParam)
{
	switch(lParam)
	{
	case FRAME_CAPTURED:
		{
			FRAMECAPTUREDSTRUCT *msg = (FRAMECAPTUREDSTRUCT*) wParam;
			m_ProgressCtrl.SetPos(msg->lastFrame);	
			m_strFrameCounter.Format("Frame %06d",msg->lastFrame);
			m_FramePreviewer.Load(msg->FilePath);
			delete msg;			
			break;
		}
	case FILES_REMAINING:
		{
			FILESREMAININGSTRUCT *msg = (FILESREMAININGSTRUCT*) wParam;
			m_strRemainingFiles.Format("Files to Load: %d",msg->filesRemaining);
			delete msg;			
			break;
		}
	case RENDER_INIT:
		{
			RENDERINITSTRUCT *msg = (RENDERINITSTRUCT*) wParam;
			m_ProgressCtrl.SetRange(0,msg->totalFrames);
			delete msg;			
			break;
		}
	case RENDER_COMPLETED:
		{
			m_AddBtn.EnableWindow(true);
			m_StartBtn.EnableWindow(true);
			m_EditBtn.EnableWindow(true);
			m_RmBtn.EnableWindow(true);
			m_renderList.EnableWindow(true);
			KillTimer(TIMER_UPDATECOUNTER_UI);
			m_ProgressCtrl.SetPos(0);
			m_FramePreviewer.FreeData();
			m_strFrameCounter = "";
			Invalidate();
			break;
		}
	case OSG_NOTIFY_MSG:
		{
			OSGNOTIFYSTRUCT *msg = (OSGNOTIFYSTRUCT*) wParam;
			m_OUTPUT.AppendFormat("%s\r\n", msg->message);
			delete msg;			
			break;
		}
	default:
		ASSERT(true);
	}
	UpdateData(false);
	return 0;
}

void CRenderOSG2Dlg::OnDestroy()
{
	CDialogEx::OnDestroy();	

	// TODO: Add your message handler code here
	delete m_renderer;
	m_renderer = 0;
}


void CRenderOSG2Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch( nIDEvent)
	{
	case TIMER_UPDATECOUNTER_UI:
		{
			CTimeSpan elapsed = CTime::GetCurrentTime() - m_QueueStartTime;
			this->m_strElapsedQueueTimeUI = elapsed.Format("%H:%M:%S");
			break;
		}
	default:
		break;
	}
	UpdateData(FALSE);
	CDialogEx::OnTimer(nIDEvent);
}
