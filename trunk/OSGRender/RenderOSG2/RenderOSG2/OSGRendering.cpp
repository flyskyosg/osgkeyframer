#include "stdafx.h"
#include "OSGRendering.h"

#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osg/CoordinateSystemNode>



#include <osg/Switch>
#include <osgText/Text>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Renderer>


#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>

#include <osgEarthUtil/SkyNode>
#include <osgEarth/MapNode>

#include <iostream>


WinNotifyHandler::WinNotifyHandler(HWND receiver):
	m_receiver(receiver)
	{
	}
void WinNotifyHandler::notify(osg::NotifySeverity severity, const char *message)
{
	//send Progressmessage to UI
	OSGNOTIFYSTRUCT *msg = new OSGNOTIFYSTRUCT;
	msg->message.Format("%s",message);				
	PostMessage(m_receiver,WM_USER_THREAD_UPDATE_PROGRESS,(WPARAM)msg,OSG_NOTIFY_MSG);
}



/** Capture the frame buffer and write image to disk*/
class WindowCaptureCallback : public osg::Camera::DrawCallback
{
public:    
	WindowCaptureCallback(GLenum readBuffer, const std::string& name):
		_readBuffer(readBuffer),
		_fileName(name)
		{
			_image = new osg::Image;
		}
		~ WindowCaptureCallback(){}

	void WindowCaptureCallback::setFileName(const std::string& name)
		{
			_fileName = name;
		}

	virtual void operator () (osg::RenderInfo& renderInfo) const
		{            
			glReadBuffer(_readBuffer);

			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
			osg::GraphicsContext* gc = renderInfo.getState()->getGraphicsContext();
			if (gc->getTraits())
			{
				GLenum pixelFormat;

				if (gc->getTraits()->alpha)
					pixelFormat = GL_RGBA;
				else 
					pixelFormat = GL_RGB;

				int width = gc->getTraits()->width;
				int height = gc->getTraits()->height;

				_image->readPixels(0, 0, width, height, pixelFormat, GL_UNSIGNED_BYTE);
			}			
							
			if (!_fileName.empty())
			{
				std::cout << "Writing to: " << _fileName << std::endl;
				osgDB::writeImageFile(*_image, _fileName);
			}
		}
	
protected:    
	GLenum                      _readBuffer;
	std::string                 _fileName;
	osg::ref_ptr<osg::Image>    _image;
	mutable OpenThreads::Mutex  _mutex;
};


/** Do Culling only while loading PagedLODs*/
class CustomRenderer : public osgViewer::Renderer
{
public:
	CustomRenderer(osg::Camera* camera) 
		: osgViewer::Renderer(camera),
		  _cullOnly(true)
		{
			//setTargetFrameRate(1);
			//setMinimumTimeAvailableForGLCompileAndDeletePerFrame(1);
		}

	/** Set flag to omit drawing in renderingTraversals */
	void setCullOnly(bool on) { _cullOnly = on; }

	virtual void operator () (osg::GraphicsContext* /*context*/)
		{
			if (_graphicsThreadDoesCull)
			{
				if (_cullOnly)
					cull();
				else
					cull_draw();
			}
		}

	virtual void cull()
		{
			osgUtil::SceneView* sceneView = _sceneView[0].get();
			if (!sceneView || _done ) return;
			
			updateSceneView(sceneView);
			
			osgViewer::View* view = dynamic_cast<osgViewer::View*>(_camera->getView());
			if (view) sceneView->setFusionDistance(view->getFusionDistanceMode(), view->getFusionDistanceValue());

			sceneView->inheritCullSettings(*(sceneView->getCamera()));
			sceneView->cull();
		}
	
	bool _cullOnly;
};

int CRenderer::Render(int argc, char** argv)
{
	int totalFrames = 0;
	char *app = "app";
	// use an ArgumentParser object to manage the program arguments.
	osg::ArgumentParser arguments(&argc,&app);
		
	osgViewer::Viewer viewer(arguments);

	osg::ref_ptr<osgGA::AnimationPathManipulator> apm = new osgGA::AnimationPathManipulator( (LPCSTR)m_strPathPath);
	if (apm || !apm->valid()) 
	{
		totalFrames = apm->getAnimationPath()->getLastTime();
		viewer.setCameraManipulator( apm );
	}

	// load the data
	osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile( (LPCSTR) m_strEarthPath);
	if (!loadedModel) 
	{
		std::cout << arguments.getApplicationName() <<": No data loaded" << std::endl;
		return 1;
	}

	//add sky
	//create SkyNode
	osg::Group* m_Root = new osg::Group();
	osgEarth::MapNode* mapnode = osgEarth::MapNode::findMapNode(loadedModel);
	osgEarth::Util::SkyNode* m_Sky= new osgEarth::Util::SkyNode( mapnode->getMap() );
	m_Sky->setDateTime( 2011, 6, 6, 20 );
	m_Root->addChild( m_Sky );	
	m_Root->addChild(loadedModel);
	//attach viewer to Sky
		m_Sky->attach( &viewer );

	{
		osg::DisplaySettings* ds = osg::DisplaySettings::instance();
		osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits(ds);

		if (viewer.getCamera()->getGraphicsContext() && viewer.getCamera()->getGraphicsContext()->getTraits()) 
		{
			//use viewer settings for window size
			osg::ref_ptr<const osg::GraphicsContext::Traits> src_traits = viewer.getCamera()->getGraphicsContext()->getTraits();
			traits->screenNum = src_traits->screenNum;
			traits->displayNum = src_traits->displayNum;
			traits->hostName = src_traits->hostName;
			traits->width = src_traits->width;
			traits->height = src_traits->height;
			traits->red = src_traits->red;
			traits->green = src_traits->green;
			traits->blue = src_traits->blue;
			traits->alpha = src_traits->alpha;
			traits->depth = src_traits->depth;
			traits->pbuffer = true;
		} 
		else 
		{
			//viewer would use fullscreen size (unknown here) pbuffer will use 4096 x4096 (or best avaiable)
			traits->width = 1920;
			traits->height = 1080;
			traits->pbuffer = true;
		}
		osg::ref_ptr<osg::GraphicsContext> pbuffer = osg::GraphicsContext::createGraphicsContext(traits.get());
		if (pbuffer.valid())
		{
			osg::notify(osg::NOTICE)<<"Pixel buffer has been created successfully."<<std::endl;
			osg::ref_ptr<osg::Camera> camera = new osg::Camera(*viewer.getCamera());
			camera->setGraphicsContext(pbuffer.get());
			camera->setViewport(0,0,traits->width,traits->height);
			GLenum buffer = pbuffer->getTraits()->doubleBuffer ? GL_BACK : GL_FRONT;
			camera->setDrawBuffer(buffer);
			camera->setReadBuffer(buffer);
			viewer.setCamera(camera.get());
		}
		else
		{
			osg::notify(osg::NOTICE)<<"Pixel buffer has not been created successfully."<<std::endl;
		}
		

		// Correct aspect ratio
		double fovy,aspectRatio,z1,z2;
		viewer.getCamera()->getProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);
		aspectRatio=double(traits->width)/double(traits->height);
		viewer.getCamera()->setProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);

	}

	// optimize the scene graph, remove redundant nodes and state etc.
	osgUtil::Optimizer optimizer;
	optimizer.optimize(loadedModel.get());

	// Install custom renderer
	osg::ref_ptr<CustomRenderer> customRenderer = new CustomRenderer(viewer.getCamera());
	viewer.getCamera()->setRenderer(customRenderer.get());

	// Override threading model
	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

	// Set the final SceneData to show
	viewer.setSceneData( m_Root );

	// Realize GUI
	viewer.realize();
	double simTime = 0.0;	

	//send Progressmessage to UI
	RENDERINITSTRUCT *msg = new RENDERINITSTRUCT;				
	msg->totalFrames = totalFrames;
	PostMessage(m_parentHWND,WM_USER_THREAD_UPDATE_PROGRESS,(WPARAM)msg,RENDER_INIT);

	GLenum buffer = viewer.getCamera()->getGraphicsContext()->getTraits()->doubleBuffer ? GL_BACK : GL_FRONT;
	
	osg::ref_ptr<WindowCaptureCallback> winCap;
	osg::FrameStamp expireStamp; 
	osg::Camera::DrawCallback* tempCallback;
	while( !viewer.done() && totalFrames > (int)simTime && !m_killThread)
	{	
		CString name;
		int filesRemaining = 0;
		name.Format(_T("%s\\%s_%06d.%s"), this->m_strSavePath , this->m_strPrefix,(int) simTime, this->m_strFileType);
		winCap = new WindowCaptureCallback(buffer, (LPCSTR) name);
	
		//set referenceTime to our simulationTime	
		viewer.getFrameStamp()->setReferenceTime(simTime);
		viewer.getFrameStamp()->setFrameNumber(simTime);

		//--- Load PageLOD tiles ---
		// Initiate the first PagedLOD request
		viewer.eventTraversal();
		viewer.updateTraversal();
		viewer.renderingTraversals();
		//Loop until osgDB has loaded the highest LOD
		while(!viewer.done() && viewer.getDatabasePager()->getRequestsInProgress() && !m_killThread){
			viewer.updateTraversal();
			viewer.renderingTraversals();
			if(filesRemaining != viewer.getDatabasePager()->getFileRequestListSize())
			{
				filesRemaining = viewer.getDatabasePager()->getFileRequestListSize();

				//send Progressmessage to UI
				FILESREMAININGSTRUCT *msg = new FILESREMAININGSTRUCT;				
				msg->filesRemaining = filesRemaining;
				PostMessage(m_parentHWND,WM_USER_THREAD_UPDATE_PROGRESS,(WPARAM)msg,FILES_REMAINING);				
			}

			Sleep(20);
		}			
		
		customRenderer->setCullOnly(false);

		tempCallback = viewer.getCamera()->getFinalDrawCallback();
		viewer.getCamera()->setFinalDrawCallback(winCap);
		//Do cull and draw to render the scene correctly  	
		viewer.renderingTraversals();
		
		viewer.getCamera()->setFinalDrawCallback(tempCallback);
		customRenderer->setCullOnly(true);

		simTime+=1.0;

		if((int)simTime % 20 == 0){
			//update the scenegraph, remove items that aren't used
			expireStamp.setFrameNumber(simTime-5);
			expireStamp.setReferenceTime(simTime-5);
			viewer.getDatabasePager()->updateSceneGraph(expireStamp);
		}
		//send Progressmessage to UI
		FRAMECAPTUREDSTRUCT *msg = new FRAMECAPTUREDSTRUCT;
		msg->lastFrame = simTime;		
		msg->FilePath = name;
		PostMessage(m_parentHWND,WM_USER_THREAD_UPDATE_PROGRESS,(WPARAM)msg,FRAME_CAPTURED);
	}
	//TODO: Clean up

	//send Progressmessage to UI
	PostMessage(m_parentHWND,WM_USER_THREAD_UPDATE_PROGRESS,(WPARAM)0,RENDER_COMPLETED);
	return 0;
}

UINT CRenderer::ThreadFunc(LPVOID lParam)
{
	CRenderer* render = (CRenderer*) lParam;
	render->Render(0,0);

	return 0;
}

CRenderer::CRenderer(HWND parent):
	m_parentHWND(parent),
	m_strPathPath(""),
	m_strEarthPath(""),
	m_strSavePath(""),
	m_strPrefix(""),
	m_strAA(""),
	m_strFileType(""),
	m_hThread(0),	
	m_killThread(false),
	notifyHandler(m_parentHWND)
{	
	osg::setNotifyHandler(&notifyHandler);
}

CRenderer::~CRenderer()
{
	//send signal to rendering loop
	m_killThread = true;

	//wait for thread to quit
	WaitForSingleObject(m_hThread, INFINITE); 
}
