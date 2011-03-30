#include "afxwin.h"
#include <osg/Notify>

#define WM_USER_THREAD_FINISHED			(WM_USER+0x101)
#define WM_USER_THREAD_UPDATE_PROGRESS	(WM_USER+0x102)

#define FILES_REMAINING					0x100
#define FRAME_CAPTURED					0x101
#define RENDER_INIT						0x102
#define RENDER_COMPLETED				0x103
#define OSG_NOTIFY_MSG					0x104

typedef struct THREADINFOSTRUCT {
	HWND	hWnd;
	char**	argv;
	int		argc;

} THREADINFOSTRUCT;

typedef struct FILESREMAININGSTRUCT {
	USHORT filesRemaining; 
} FILESREMAININGSTRUCT;

typedef struct FRAMECAPTUREDSTRUCT {
	USHORT lastFrame; 
	CString FilePath; //Path of the last loaded image
} FRAMECAPTUREDSTRUCT;

typedef struct RENDERINITSTRUCT {		
	USHORT totalFrames;	
} RENDERINITSTRUCT;

typedef struct OSGNOTIFYSTRUCT {		
	CString message;	
} OSGNOTIFYSTRUCT;

class WinNotifyHandler : public osg::NotifyHandler
{
private:
	HWND m_receiver;
public:
	WinNotifyHandler(HWND receiver);
    void notify(osg::NotifySeverity severity, const char *message);
	void changeReceiver(HWND receiver){m_receiver = receiver;}
};


class CRenderer
{
public:
	~CRenderer();
	CRenderer(HWND parent);
	HWND m_parentHWND;

	CString m_strPathPath;
	CString m_strEarthPath;
	CString m_strSavePath;
	CString m_strPrefix;
	CString m_strAA;
	CString m_strFileType;

	CWinThread * m_hThread; 	
	bool m_killThread; //rendering loop checks this to know when to exit

	//thread "main" function
	static UINT ThreadFunc(LPVOID lParam);

private:
	
	WinNotifyHandler notifyHandler;	

	//OSG Rendering function
	int Render(int argc, char** argv);
};