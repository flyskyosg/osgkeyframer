// MFC_OSG.cpp : implementation of the cOSG class
//
#include "stdafx.h"
#include "MFC_OSG.h"
#include "osgEarthUtil\Viewpoint"



LPARAM FLOAT_TO_LPARAM(double d)
{
	// Convert double to float - this allows caller to pass double as parameter
	// without being concerned about warning messages. We explicitly cast here
	// to avoid those messages.
	const float f = (float)d;
	// This function relies on the fact that in Win32 a float is the same size
	// as LPARAM. If this assert fails (i.e. Win64) then a different method of
	// packing a float into an LPARAM will be needed.
	ASSERT(sizeof(float)==sizeof(LPARAM));
	// Return the float binary data directly as an LPARAM
	return *((LPARAM *)&f);
}

float LPARAM_TO_FLOAT(LPARAM l)
{
	// This function relies on the fact that in Win32 a float is the same size
	// as LPARAM. If this assert fails (i.e. Win64) then a different method of
	// packing a float into an LPARAM will be needed.
	ASSERT(sizeof(float)==sizeof(LPARAM));
	// Return the LPARAM binary data directly as a float
	return *((float *)&l);
}
cOSG::cOSG(HWND placeholder,HWND parent) :
	m_hWnd(placeholder),
	m_hParentWnd(parent),		
	m_simTime(DBL_MAX),
	m_apm_isPaused(false),
	m_apm_loaded(false)
{
}

cOSG::~cOSG()
{
	mViewer->setDone(true);

	Sleep(1000);
	mViewer->setSceneData(0);
	mRoot = 0;
	mModel = 0;
	mViewer->stopThreading();	
	delete mViewer;
}

void cOSG::EnqueueRequest(osgRequest* req)
{
	m_RequestQueue.push(req);
}
void cOSG::EnqueueRequest(byte type, LPARAM lParam)
{
	osgRequest* req = new osgRequest();

	req->lParam = lParam;
	req->type = type;

	EnqueueRequest(req);
}

void cOSG::InitOSG()
{
	// Init different parts of OSG
	InitManipulators();    
	InitCameraConfig();

	//InitSceneGraph();

	//mViewer->getViewerBase()->setRunMaxFrameRate(30);
	//mViewer->getViewerBase()->setThreadingModel(osgViewer::ViewerBase::ThreadingModel::CullThreadPerCameraDrawThreadPerContext);
	//mViewer->getViewerBase()->setRunFrameScheme(osgViewer::ViewerBase::FrameScheme::ON_DEMAND);
}

void cOSG::InitManipulators(void)
{    
	m_earthManip = new osgEarth::Util::EarthManipulator();
}


void cOSG::InitSceneGraph(std::string modelName)
{
	mViewer->setSceneData(0);
	// Init the main Root Node/Group
	mRoot  = new osg::Group;

	// Load the Model from the model name
	mModel = osgDB::readNodeFile(modelName);

	// Optimize the model
	osgUtil::Optimizer optimizer;
	optimizer.optimize(mModel.get());
	optimizer.reset();

	// Add the model to the scene
	mRoot->addChild(mModel.get());

	// Set the Scene Data
	mViewer->setSceneData(mRoot.get());
	// Realize the Viewer
	mViewer->realize();
}

void cOSG::InitCameraConfig(void)
{
	// Local Variable to hold window size data
	RECT rect;

	// Create the viewer for this window
	mViewer = new osgViewer::Viewer();
	
	// Add a Stats Handler to the viewer
	mViewer->addEventHandler(new osgViewer::StatsHandler);

	 // add the screen capture handler
	mViewer->addEventHandler(new osgViewer::ScreenCaptureHandler);
	
	// Get the current window size
	::GetWindowRect(m_hWnd, &rect);

	// Init the GraphicsContext Traits
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

	// Init the Windata Variable that holds the handle for the Window to display OSG in.
	osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(m_hWnd);

	// Setup the traits parameters
	traits->x = 0;
	traits->y = 0;
	traits->width = rect.right - rect.left;
	traits->height = rect.bottom - rect.top;
	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->setInheritedWindowPixelFormat = true;
	traits->inheritedWindowData = windata;

	// Create the Graphics Context
	osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits.get());

	// Init a new Camera (Master for this View)
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;

	// Assign Graphics Context to the Camera
	camera->setGraphicsContext(gc);

	// Set the viewport for the Camera
	camera->setViewport(new osg::Viewport(traits->x, traits->y, traits->width, traits->height));

	// Add the Camera to the Viewer
	mViewer->addSlave(camera.get());

	// Add the Camera Manipulator to the Viewer
	mViewer->setCameraManipulator(m_earthManip);	

	// Correct aspect ratio
	double fovy,aspectRatio,z1,z2;
	mViewer->getCamera()->getProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);
	aspectRatio=double(traits->width)/double(traits->height);
	mViewer->getCamera()->setProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);
}

void cOSG::PreFrameUpdate()
{
	// Do any preframe updates in this routine
	//check if there are any requests waiting in the queue
	osgRequest* req;
	while(!m_RequestQueue.empty())
	{
		if(m_RequestQueue.try_pop(req))
			HandleMsg(req);
	}
}
void cOSG::HandleMsg(osgRequest* &req)
{
	switch(req->type)
	{
	case REQ_SETCAMMATRIX:
		{
			osg::Matrixd* m = (osg::Matrixd*) req->lParam;
			mViewer->getCameraManipulator()->setByMatrix(*m);
			delete m;
			break;
		}	
	case REQ_SETCAMVIEWPOINT:
		{
			if(!m_apm_loaded){
				osgEarth::Util::Viewpoint* vp = (osgEarth::Util::Viewpoint*) req->lParam;
				m_earthManip->setViewpoint(*vp);	
				delete vp;
			}
			break;
		}
	case REQ_STARTANIMATION:
		{
			osg::ref_ptr<osg::AnimationPath> ap = (osg::AnimationPath*)req->lParam;
			ap->unref();
			m_apm = new COSGAnimationPathManipulator(ap);			
			m_apm->getAnimationPath()->setLoopMode(osg::AnimationPath::LoopMode::NO_LOOPING);
			
			mViewer->setCameraManipulator(m_apm);
			mViewer->setReferenceTime();
			m_apm_loaded = true;
			//Alert Control that an Animation has begun
			PostMessage(m_hParentWnd,UM_OSGNOTIFY,RPLY_ANIMATIONSTARTED,0);
			break;
		}
	case REQ_RESETANIMATION:
		{
			mViewer->setReferenceTime();
			break;
		}
	case REQ_TOGGLEANIMATION:
		{	
			mViewer->getEventQueue()->keyPress('p');
			break;
		}
	case REQ_STOPANIMATION:
		{		
			//swap manipulators
			mViewer->setCameraManipulator(m_earthManip,false);
			m_apm = 0;
			m_apm_loaded = false;
			//Alert Control that an Animation has ended
			PostMessage(m_hParentWnd,UM_OSGNOTIFY,RPLY_ANIMATIONCOMPLETED,0);	
			break;
		}
	case REQ_LOADEARTHFILE:
		{
			PostMessage(m_hParentWnd,UM_OSGNOTIFY,RPLY_LOADINGEARTH,STAGE_LOADING);			
			m_modelName = *(std::string*)req->lParam;
			InitSceneGraph(m_modelName);
			delete (std::string*)req->lParam;
			PostMessage(m_hParentWnd,UM_OSGNOTIFY,RPLY_LOADINGEARTH,STAGE_COMPLETED);
		}
	}

	delete req;
	req = 0;
}
void cOSG::PostFrameUpdate()
{
	double reftime = mViewer->getFrameStamp()->getReferenceTime();
	//Do any postframe updates in this routine
	if(m_apm_loaded && !m_apm->animationIsPaused())
	{
		if( m_apm->animationCompleted( reftime ))
		{			
			//post last animationtime reply
			PostMessage(m_hParentWnd,UM_OSGNOTIFY,RPLY_ANIMATIONTIME,FLOAT_TO_LPARAM(m_apm->getAnimationTime(reftime)));

			//swap manipulators
			mViewer->setCameraManipulator(m_earthManip,false);
			m_apm = 0;
			m_apm_loaded = false;

			//Alert Control that an Animation has ended			
			PostMessage(m_hParentWnd,UM_OSGNOTIFY,RPLY_ANIMATIONCOMPLETED,0);			
		}
		else
		{
			PostMessage(m_hParentWnd,UM_OSGNOTIFY,RPLY_ANIMATIONTIME,FLOAT_TO_LPARAM(m_apm->getAnimationTime(reftime)));
		}

	}
	//if we need to turn off the animation
}

void cOSG::Render(void* ptr)
{
	cOSG* osg = (cOSG*)ptr;

	osgViewer::Viewer* viewer = osg->getViewer(); 

	// You have two options for the main viewer loop
	//      viewer->run()   or
	//      while(!viewer->done()) { viewer->frame(); }

	//viewer->run();
	
	while(!viewer->done())
	{		
		osg->PreFrameUpdate();		
		viewer->frame();		
		osg->PostFrameUpdate();
		Sleep(10);         // Use this command if you need to allow other processes to have cpu time
	}

	// For some reason this has to be here to avoid issue: 
	// if you have multiple OSG windows up 
	// and you exit one then all stop rendering
   // AfxMessageBox(_T("Exit Rendering Thread"));

	viewer->releaseGLObjects();
	osg::FrameStamp now = *viewer->getFrameStamp();
	viewer->getDatabasePager()->updateSceneGraph(now);


	_endthread();
}
