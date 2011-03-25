#pragma once

#include <concurrent_queue.h>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/api/win32/GraphicsWindowWin32>

#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>

#include <string>

#include <osgEarthUtil\EarthManipulator>
#include "..\OSGAnimationPathManipulator.h"



#define REQ_SETCAMMATRIX			0x0001
#define REQ_SETCAMVIEWPOINT			0x0002
#define REQ_SETCAM_MANIP			0x0003

#define REQ_STARTANIMATION			0x0004
#define REQ_RESETANIMATION			0x0005
#define REQ_TOGGLEANIMATION			0x0006
#define REQ_STOPANIMATION			0x0007

#define REQ_LOADEARTHFILE			0x0008

#define UM_OSGNOTIFY				WM_USER+1
#define RPLY_ANIMATIONPAUSED		0x1000
#define RPLY_ANIMATIONCOMPLETED		0x1001
#define RPLY_ANIMATIONSTARTED		0x1002
#define RPLY_ANIMATIONTIME			0x1003
#define RPLY_LOADINGEARTH			0x1004
	#define STAGE_LOADING			0x2000
	#define	STAGE_COMPLETED			0x2001



typedef struct{
	byte type;
	LPARAM  lParam;
} osgRequest;

LPARAM FLOAT_TO_LPARAM(double d);

float LPARAM_TO_FLOAT(LPARAM l);

class cOSG
{
public:
	cOSG(HWND placeholder,HWND parent);
	~cOSG();

	void InitOSG();
	void InitManipulators(void);
	void InitSceneGraph(std::string modelName);
	void InitCameraConfig(void);
	void SetupWindow(void);
	void SetupCamera(void);
	void PreFrameUpdate(void);
	void PostFrameUpdate(void);
	static void Render(void* ptr);
	void EnqueueRequest(osgRequest*);
	void EnqueueRequest(byte type, LPARAM lParam);
	osgViewer::Viewer* getViewer() { return mViewer; }

	double m_simTime;

private:  
	std::string m_modelName;
	HWND m_hWnd;
	HWND m_hParentWnd;
	osgViewer::Viewer* mViewer;
	osg::ref_ptr<osg::Group> mRoot;
	osg::ref_ptr<osg::Node> mModel;

	osg::ref_ptr<osgEarthUtil::EarthManipulator> m_earthManip;//osgEarth Camera Manipulator
	osg::ref_ptr<COSGAnimationPathManipulator> m_apm;//Customized AnimationPath Camera Manipulator

	bool m_apm_isPaused;
	bool m_apm_loaded;

	void HandleMsg(osgRequest* &req);
	Concurrency::concurrent_queue<osgRequest*> m_RequestQueue;
};
