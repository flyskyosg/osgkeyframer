#pragma once

// CEnvOSG
#define CEnvOSG_CLASSNAME    _T("CEnvOSG")  // Window class name
class CEnvOSG : public CWnd
{
	DECLARE_DYNAMIC(CEnvOSG)

public:
	CEnvOSG();
	virtual ~CEnvOSG();
	void startOSG();

private: 
	BOOL RegisterWindowClass();

protected:
	DECLARE_MESSAGE_MAP()
public:

};


