#pragma once


// CSetAutoTimeDlg dialog

class CSetAutoTimeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetAutoTimeDlg)

public:
	CSetAutoTimeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetAutoTimeDlg();

// Dialog Data
	enum { IDD = IDD_AUTOTIME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double m_duration;
};
