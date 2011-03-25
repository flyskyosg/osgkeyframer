
// RenderOSG2Dlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "OSGRendering.h"
#include "PictureCtrl.h"

// CRenderOSG2Dlg dialog
class CRenderOSG2Dlg : public CDialogEx
{
// Construction
public:
	CRenderOSG2Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_RENDEROSG2_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	//data
	CRenderer* m_renderer;
	CTime m_QueueStartTime;

	HICON m_hIcon;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//Dialog Controls
	CListCtrl m_renderList;
	CButton m_EditBtn;
	CButton m_RmBtn;
	CButton m_StartBtn;
	CButton m_AddBtn;
	CProgressCtrl m_ProgressCtrl;
	CPictureCtrl m_FramePreviewer;
	//UI Strings
	CString m_strElapsedQueueTimeUI;
	CString m_strFrameCounter;
	CString m_strRemainingFiles;
	CString m_OUTPUT;

	afx_msg void OnBnClickedAddButton();
	afx_msg void OnLvnItemchangedRenderlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnInsertitemRenderlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnDeleteitemRenderlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRmbtn();
	afx_msg void OnBnClickedStartbtn();
	afx_msg void OnBnClickedEditbtn();	
	afx_msg void OnDestroy();	
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	//User Messages:
	afx_msg LRESULT OnThreadFinished(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnThreadUpdateProgress(WPARAM wParam,LPARAM lParam);
		

};
