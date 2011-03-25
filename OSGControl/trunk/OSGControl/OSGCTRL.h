#pragma once
#include "ChildDialog\GenericChildDialog.h"
#include "OSG\MFC_OSG.h"
#include "OSG\EnvOSG.h"
#include "KeyFrameList.h"
#include "CGridListCtrlEx\CGridColumnTraitEdit.h"

#include "afxwin.h"
#include "afxcmn.h"

// COSGCTRL dialog

class COSGCTRL : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(COSGCTRL)

public:
	//COSGCTRL(CWnd* pParent = NULL);   // standard constructor
	virtual ~COSGCTRL();

	COSGCTRL();							 // added
	virtual int CRHGetDialogID();        // added

	//Helper functions:
	void interpolateKeyFrames(osg::ref_ptr<osg::AnimationPath> ap, float fps);
	int findItem(float time);
	bool exportKEYfile(CString filename);
	bool importKEYfile(CString filename);
	bool exportPATHfile(CString filename);

	bool m_animationRunning;

// Dialog Data
	enum { IDD = IDD_OSGCONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
public:
	RECT rList;
	RECT rCaptureBtn;
	RECT rPlayBtn;

	HANDLE mThreadHandle;
	cOSG* m_pOSG;
	int m_selectedItem;
	CString m_sAnimationTime;
	//dialog items
	CKeyframeList m_ListCtrl;
	CEnvOSG m_osgPlaceHolder;
	CButton m_DeleteBtn;
	CButton m_CaptureBtn;
	CButton m_AutotimeBtn;
	CButton m_ClearallBtn;
	CButton m_ImportBtn;
	CButton m_ExportBtn;

	CButton m_PlayBtn;
	CButton m_ResetBtn;
	CButton m_PauseBtn;
	CButton m_StopBtn;
	CStatic m_TimerLabel;

	CButton m_LoadEarthBtn;
	

	//Message Handlers
	virtual BOOL OnInitDialog();	
	afx_msg void OnBnClickedCaptureBtn();
	afx_msg void OnBnClickedDeleteBtn();
	afx_msg void OnBnClickedAutotimeBtn();
	afx_msg void OnBnClickedExportBtn();
	afx_msg void OnBnClickedPlayBtn();
	afx_msg void OnBnClickedResetBtn();
	afx_msg void OnDestroy();
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg LRESULT OnOSGNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);		
	afx_msg void OnBnClickedClearallBtn();	
	afx_msg void OnBnClickedPauseBtn();
	afx_msg void OnBnClickedStopBtn();	
	afx_msg void OnBnClickedImportBtn();
	afx_msg void OnBnClickedLoadearthBtn();
	
};
