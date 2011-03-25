// SetAutoTimeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OSGControl.h"
#include "SetAutoTimeDlg.h"
#include "afxdialogex.h"


// CSetAutoTimeDlg dialog

IMPLEMENT_DYNAMIC(CSetAutoTimeDlg, CDialogEx)

CSetAutoTimeDlg::CSetAutoTimeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetAutoTimeDlg::IDD, pParent)
	, m_duration(10)
{

}

CSetAutoTimeDlg::~CSetAutoTimeDlg()
{
}

void CSetAutoTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_duration);
}


BEGIN_MESSAGE_MAP(CSetAutoTimeDlg, CDialogEx)
END_MESSAGE_MAP()


// CSetAutoTimeDlg message handlers
