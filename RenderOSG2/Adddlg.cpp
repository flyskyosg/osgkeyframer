// Adddlg.cpp : implementation file
//

#include "stdafx.h"
#include "RenderOSG2.h"
#include "Adddlg.h"
#include "afxdialogex.h"


// CAdddlg dialog

IMPLEMENT_DYNAMIC(CAdddlg, CDialog)

CAdddlg::CAdddlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAdddlg::IDD, pParent)
	, m_ExampleText(_T("Prefix_01234"))
	, m_strPathPath(_T(""))
	, m_strEarthPath(_T(""))
	, m_strSavePath(_T(""))
	, m_strPrefix(_T(""))
	, m_strAA(_T("0"))
	, m_strFileType(_T("JPG"))
{
	
}

CAdddlg::~CAdddlg()
{
}

void CAdddlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EXTEXT, m_ExampleText);
	DDX_Text(pDX, IDC_PATHBROWSE, m_strPathPath);
	DDX_Text(pDX, IDC_EARTHBROWSE, m_strEarthPath);
	DDX_Text(pDX, IDC_SAVETOBROWSE, m_strSavePath);
	DDX_Text(pDX, IDC_PREFIXEDIT, m_strPrefix);
	DDX_CBString(pDX, IDC_AACOMBO, m_strAA);
	DDX_CBString(pDX, IDC_FILETYPECOMBO, m_strFileType);
	DDX_Control(pDX, IDC_PATHBROWSE, m_PathCtrl);
	DDX_Control(pDX, IDC_EARTHBROWSE, m_EarthCtrl);
}


BEGIN_MESSAGE_MAP(CAdddlg, CDialog)
	ON_EN_CHANGE(IDC_PREFIXEDIT, &CAdddlg::OnEnChangePrefixedit)
END_MESSAGE_MAP()


// CAdddlg message handlers


BOOL CAdddlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// TODO:  Add extra initialization here
	m_PathCtrl.EnableFileBrowseButton(NULL,_T(".path files (*.path)|*.path||"));
	m_EarthCtrl.EnableFileBrowseButton(NULL,_T("osgEarth files (*.earth)|*.earth||"));



	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CAdddlg::OnEnChangePrefixedit()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if(m_PathCtrl.m_hWnd){
		UpdateData(TRUE);
		m_ExampleText = m_strPrefix + _T("_01234");
		UpdateData(FALSE);
	}
}
