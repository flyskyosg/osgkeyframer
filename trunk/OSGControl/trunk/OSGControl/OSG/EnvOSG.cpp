// EnvOSG.cpp : implementation file
//

#include "stdafx.h"
#include "EnvOSG.h"


// CEnvOSG

IMPLEMENT_DYNAMIC(CEnvOSG, CWnd)

CEnvOSG::CEnvOSG()
{
	RegisterWindowClass();
}

CEnvOSG::~CEnvOSG()
{
}
BOOL CEnvOSG::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();

    if (!(::GetClassInfo(hInst, CEnvOSG_CLASSNAME, &wndcls)))
    {
        // otherwise we need to register a new class
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = CEnvOSG_CLASSNAME;

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

BEGIN_MESSAGE_MAP(CEnvOSG, CWnd)
END_MESSAGE_MAP()



// CEnvOSG message handlers
