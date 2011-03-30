#pragma once
#include "afxeditbrowsectrl.h"


// CAdddlg dialog

class CAdddlg : public CDialog
{
	DECLARE_DYNAMIC(CAdddlg)

public:
	CAdddlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAdddlg();

// Dialog Data
	enum { IDD = IDD_ADDDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_ExampleText;
	CString m_strPathPath;
	CString m_strEarthPath;
	CString m_strSavePath;
	CString m_strPrefix;
	CString m_strAA;
	CString m_strFileType;
	CMFCEditBrowseCtrl m_PathCtrl;
	CMFCEditBrowseCtrl m_EarthCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangePrefixedit();
};
