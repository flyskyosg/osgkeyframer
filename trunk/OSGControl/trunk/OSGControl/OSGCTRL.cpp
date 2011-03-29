// OSGCTRL.cpp : implementation file
//

#include "stdafx.h"
#include "OSGControl.h"
#include "OSGCTRL.h"
#include "afxdialogex.h"

#include "SetAutoTimeDlg.h"
#include "Interpolator\CRSpline.h"
#include "SetAutoTimeDlg.h"

#include "osgEarthUtil\Viewpoint"

// COSGCTRL dialog

IMPLEMENT_DYNAMIC(COSGCTRL, CDialog)

//COSGCTRL::COSGCTRL(CWnd* pParent /*=NULL*/)
//	: CDialogEx(COSGCTRL::IDD, pParent)
//{
//
//}
COSGCTRL::COSGCTRL()
: m_sAnimationTime(_T("")),
m_animationRunning(false)
{

}
COSGCTRL::~COSGCTRL()
{
}

void COSGCTRL::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM2, m_osgPlaceHolder);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Control(pDX, IDC_DELETE_BTN, m_DeleteBtn);
	DDX_Control(pDX, IDC_CAPTURE_BTN, m_CaptureBtn);
	DDX_Control(pDX, IDC_AUTOTIME_BTN, m_AutotimeBtn);
	DDX_Control(pDX, IDC_CLEARALL_BTN, m_ClearallBtn);
	DDX_Control(pDX, IDC_IMPORT_BTN, m_ImportBtn);
	DDX_Control(pDX, IDC_EXPORT_BTN, m_ExportBtn);
	DDX_Control(pDX, IDC_PLAY_BTN, m_PlayBtn);
	DDX_Control(pDX, IDC_RESET_BTN, m_ResetBtn);

	DDX_Text(pDX, IDC_ANIMTIME, m_sAnimationTime);
	DDX_Control(pDX, IDC_PAUSE_BTN, m_PauseBtn);
	DDX_Control(pDX, IDC_STOP_BTN, m_StopBtn);
	DDX_Control(pDX, IDC_ANIMTIME, m_TimerLabel);
	DDX_Control(pDX, IDC_LOADEARTH_BTN, m_LoadEarthBtn);
}


BEGIN_MESSAGE_MAP(COSGCTRL, CDialog)

	ON_BN_CLICKED(IDC_CAPTURE_BTN, &COSGCTRL::OnBnClickedCaptureBtn)
	ON_BN_CLICKED(IDC_DELETE_BTN, &COSGCTRL::OnBnClickedDeleteBtn)
	ON_BN_CLICKED(IDC_AUTOTIME_BTN, &COSGCTRL::OnBnClickedAutotimeBtn)
	ON_BN_CLICKED(IDC_EXPORT_BTN, &COSGCTRL::OnBnClickedExportBtn)
	ON_BN_CLICKED(IDC_PLAY_BTN, &COSGCTRL::OnBnClickedPlayBtn)
	ON_BN_CLICKED(IDC_RESET_BTN, &COSGCTRL::OnBnClickedResetBtn)
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &COSGCTRL::OnLvnItemchangedList1)
	ON_WM_SIZE()
	ON_MESSAGE(UM_OSGNOTIFY, OnOSGNotify)

	ON_BN_CLICKED(IDC_CLEARALL_BTN, &COSGCTRL::OnBnClickedClearallBtn)
	ON_BN_CLICKED(IDC_PAUSE_BTN, &COSGCTRL::OnBnClickedPauseBtn)
	ON_BN_CLICKED(IDC_STOP_BTN, &COSGCTRL::OnBnClickedStopBtn)
	ON_BN_CLICKED(IDC_IMPORT_BTN, &COSGCTRL::OnBnClickedImportBtn)
	ON_BN_CLICKED(IDC_LOADEARTH_BTN, &COSGCTRL::OnBnClickedLoadearthBtn)
END_MESSAGE_MAP()

int COSGCTRL::CRHGetDialogID()
{
	return(IDD);
}

// COSGCTRL message handlers

BOOL COSGCTRL::OnInitDialog()
{
	CRHGenericChildDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_osgPlaceHolder.SetFocus();

	m_pOSG = new cOSG(m_osgPlaceHolder.m_hWnd,this->m_hWnd);
	// Init the osg class
	m_pOSG->InitOSG();
	
	// Start the thread to do OSG Rendering
	mThreadHandle = (HANDLE)_beginthread(&cOSG::Render, 0, m_pOSG); 

	//setup ListBox

	//set columns: Keyframe#, time, quat		
	m_ListCtrl.InsertColumn(0, "keyframe",0,80);
	m_ListCtrl.InsertColumnTrait(TIME_COL, "Time", LVCFMT_LEFT, 100, 1, new CGridColumnTraitEdit());
	m_ListCtrl.InsertColumnTrait(DURATION_COL, "Duration", LVCFMT_LEFT, 100, 1, new CGridColumnTraitEdit());
	m_ListCtrl.InsertColumn(DISTANCE_COL, "Distance Traveled",0,120);

	//setup RECTS for resizing
	m_ListCtrl.GetClientRect(&rList);
	m_CaptureBtn.GetClientRect(&rCaptureBtn);
	m_PlayBtn.GetClientRect(&rPlayBtn);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COSGCTRL::OnBnClickedCaptureBtn()
{
	// The user has requested to capture a keyframe
	//capture control point
	osg::Matrixd m = m_pOSG->getViewer()->getCameraManipulator()->getMatrix();    	
	KeyFrame* key = new KeyFrame;
	key->ctrlPoint = osg::AnimationPath::ControlPoint(m.getTrans(), m.getRotate());
	key->viewPoint = ( (osgEarth::Util::EarthManipulator*) m_pOSG->getViewer()->getCameraManipulator())->getViewpoint();
	key->duration = 1.0;

	m_ListCtrl.AddItem(key);

}

void COSGCTRL::OnBnClickedDeleteBtn()
{
	// TODO: Add your control notification handler code here
	m_ListCtrl.DeleteItem(m_selectedItem);
}

void autoTime(std::vector<KeyFrame*> &keys, float time)
{
	//note Rotations are not considered in timing
	//if position doesn't change, but camera angle does, it will happen instantaneously!
	double totaldist = 0;
	double totaltime = 0;

	if(keys.size() == 0)
		return;

	keys[0]->distFromPrev = 0;
	//determine distances between keyframes
	for(int i = 1; i < keys.size();i++)
	{
		totaldist += keys[i]->distFromPrev;
	}

	//set times in relation to distance traveled
	for(int i =0; i< keys.size()-1;i++)
	{	
		keys[i]->timeStamp = totaltime;
		keys[i]->duration = totaltime;
		totaltime += (keys[i+1]->distFromPrev / totaldist) * time;
		keys[i]->duration = totaltime - keys[i]->duration;
	}

	//set last time
	keys[keys.size()-1]->timeStamp = time;
	keys[keys.size()-1]->duration = totaltime - keys[keys.size()-1]->timeStamp;
	
}
void COSGCTRL::OnBnClickedAutotimeBtn()
{
	CSetAutoTimeDlg dlg;
	dlg.m_duration = 10.0;
	if(dlg.DoModal() == IDCANCEL)
		return;

	//collect keyframe pointers from listcontrol
	std::vector<KeyFrame*> keys;	
	for(int i = 0; i < m_ListCtrl.GetItemCount(); i++)
	{
		keys.push_back((KeyFrame*) m_ListCtrl.GetItemData(i));
	}

	//run autoTime
	autoTime(keys,dlg.m_duration);

	//update listctrl
	CString str;
	for(int i = 0; i < m_ListCtrl.GetItemCount(); i++)
	{
		str.Format("%f",keys[i]->timeStamp);
			m_ListCtrl.SetItemText(i,TIME_COL,str);

		str.Format("%f",keys[i]->distFromPrev);
			m_ListCtrl.SetItemText(i,DISTANCE_COL,str);

		str.Format("%f",keys[i]->duration);
			m_ListCtrl.SetItemText(i,DURATION_COL,str);
	}
}

bool COSGCTRL::exportKEYfile(CString filename)
{
	osgDB::ofstream fout;

	fout.open(filename);
	fout.precision(15);
	for(int i = 0; i < m_ListCtrl.GetItemCount();i++)
	{
		KeyFrame* key = (KeyFrame*) m_ListCtrl.GetItemData(i);
		osg::Vec3 pos = key->ctrlPoint.getPosition();
		osg::Quat rot = key->ctrlPoint.getRotation();

		fout<<key->duration<<" "
			<<pos.x()<<" "
			<<pos.y()<<" "
			<<pos.z()<<" "
			<<rot.x()<<" "
			<<rot.y()<<" "
			<<rot.z()<<" "
			<<rot.w()<<" "
			<<key->viewPoint.x()<<" "
			<<key->viewPoint.y()<<" "
			<<key->viewPoint.z()<<" "
			<<key->viewPoint.getHeading()<<" "
			<<key->viewPoint.getPitch()<<" "
			<<key->viewPoint.getRange()
			<<std::endl;
	}
	fout.close();
	return true;
}

bool COSGCTRL::exportPATHfile(CString filename)
{
	osgDB::ofstream fout;	
	osg::ref_ptr<osg::AnimationPath> ap = new osg::AnimationPath();
	osg::ref_ptr<osg::AnimationPath> ap2 = new osg::AnimationPath();
	
	interpolateKeyFrames(ap,30.0); //PATH file contains list of interpolated frames

	osg::AnimationPath::TimeControlPointMap map = ap->getTimeControlPointMap();
	int i = 0;
	for(osg::AnimationPath::TimeControlPointMap::iterator iter = map.begin(); iter != map.end();iter++)
	{
		ap2->insert(i++,iter->second);			
	}

	if(fout)
	{
		fout.open(filename);
		fout.precision( 15 );
		ap2->write(fout);
		fout.close();
	}
	else	
		return false;	

	return true;

}
void COSGCTRL::OnBnClickedExportBtn()
{
	CFileDialog fd(false,".path","myPath1",6UL,".path files (*.path)|*.path|keyframes (*.key)|*.key||",this);
	//get filename
	if(fd.DoModal() == IDCANCEL) 
		return;

	if(!fd.GetFileExt().CompareNoCase("path") )
	{
		//Export .PATH file
		if( !exportPATHfile(fd.GetPathName()) )
			MessageBox("Error writing to file.","Error",MB_ICONERROR | MB_OK);
	}
	else if(!fd.GetFileExt().CompareNoCase("key") )
	{
		//Export .KEY file
		if( !exportKEYfile(fd.GetPathName()) )
			MessageBox("Error writing to file.","Error",MB_ICONERROR | MB_OK);
	}
}

bool COSGCTRL::importKEYfile(CString filename)
{
	osgDB::ifstream in(filename);
	m_ListCtrl.ResetList();

	while(!in.eof())
	{
		KeyFrame *key = new KeyFrame;
		osg::Vec3 pos = key->ctrlPoint.getPosition();
		osg::Quat rot = key->ctrlPoint.getRotation();
		double heading,pitch,range,x,y,z;

		in>>key->duration
			>>pos.x()
			>>pos.y()
			>>pos.z()
			>>rot.x()
			>>rot.y()
			>>rot.z()
			>>rot.w()
			>>x
			>>y
			>>z
			>>heading
			>>pitch
			>>range;
		
		if(!in.eof())
		{
			key->ctrlPoint.setPosition(pos);
			key->ctrlPoint.setRotation(rot);
			key->viewPoint.setFocalPoint(osg::Vec3(x,y,z));
			key->viewPoint.setHeading(heading);
			key->viewPoint.setPitch(pitch);
			key->viewPoint.setRange(range);

			m_ListCtrl.AddItem(key);
		}
		
	}
	m_ListCtrl.fixTimes(true);
	in.close();

	return true;
}
void COSGCTRL::OnBnClickedImportBtn()
{
	CFileDialog fd(true,NULL,NULL,6UL,"keyframes (*.key)|*.key||",this);
	//get filename
	if(fd.DoModal() == IDCANCEL) 
		return;

	if( !importKEYfile(fd.GetPathName()) )
			MessageBox("Error reading file.","Error",MB_ICONERROR | MB_OK);

}
void COSGCTRL::interpolateKeyFrames(osg::ref_ptr<osg::AnimationPath> ap, float fps)
{
	CRSpline spline;
	//build list of keyframes
	for(int i = 0; i < m_ListCtrl.GetItemCount();i++)
	{
		KeyFrame* key = (KeyFrame*) m_ListCtrl.GetItemData(i);
		osg::Vec3 pos = key->ctrlPoint.getPosition();
		osg::Quat rot = key->ctrlPoint.getRotation();		
		spline.AddSplinePoint(vec3(key->timeStamp,pos.x(),pos.y(),pos.z(),rot.x(),rot.y(),rot.z(),rot.w()));		
	}

	//get frame timedeltas based on 30 fps
	float finaltime = ((KeyFrame*)m_ListCtrl.GetItemData(m_ListCtrl.GetItemCount()-1))->timeStamp;
	float tdelta= 1.0 / fps;

	//build list of interpolated frames
	for(float i = 0.0; i < finaltime ; i += tdelta)
	{
		vec3 interp =spline.GetInterpolatedSplineKeyFrame(i);
		KeyFrame key;
		key.ctrlPoint.setPosition(osg::Vec3(interp.x,interp.y,interp.z));
		key.ctrlPoint.setRotation(osg::Quat(interp.rx,interp.ry,interp.rz,interp.ra));
		ap->insert(i,key.ctrlPoint);
	}

	//add last frame
	vec3 interp =spline.GetInterpolatedSplineKeyFrame(finaltime);
	KeyFrame key;
	key.ctrlPoint.setPosition(osg::Vec3(interp.x,interp.y,interp.z));
	key.ctrlPoint.setRotation(osg::Quat(interp.rx,interp.ry,interp.rz,interp.ra));
	ap->insert(finaltime,key.ctrlPoint);
}
void COSGCTRL::OnBnClickedPlayBtn()
{
	osg::ref_ptr<osg::AnimationPath> ap = new osg::AnimationPath();

	//for linear interpolation, use built-in interpolator
	//for(int i = 0; i < m_ListCtrl.GetItemCount();i++)
	//{
	//	KeyFrame* key = (KeyFrame*) m_ListCtrl.GetItemData(i);
	//	ap->insert(key->timeStamp,key->ctrlPoint);
	//}

	//m_Viewer.m_OSG->EnqueueRequest(REQ_STARTANIMATION,(LPARAM)ap);

	interpolateKeyFrames(ap,30.0);

	//send animation path to OSG
	ap->ref(); //manual +1 to reference count, make sure to do -1 on the other side
	m_pOSG->EnqueueRequest(REQ_STARTANIMATION,(LPARAM) ap.get());

	
	if(m_selectedItem != -1)
	{
		//deselect the item
		m_ListCtrl.SetItemState(m_selectedItem,0,LVIS_FOCUSED | LVIS_SELECTED);		
		m_selectedItem = 0;
		m_ListCtrl.SetItemState(m_selectedItem,LVIS_FOCUSED | LVIS_SELECTED,LVIS_FOCUSED | LVIS_SELECTED);
	}
}

void COSGCTRL::OnBnClickedResetBtn()
{
	m_pOSG->EnqueueRequest(REQ_RESETANIMATION,0);
}

void COSGCTRL::OnBnClickedPauseBtn()
{
	m_pOSG->EnqueueRequest(REQ_TOGGLEANIMATION,0);
	// TODO: Add your control notification handler code here
}

void COSGCTRL::OnBnClickedClearallBtn()
{
	m_ListCtrl.ResetList();
}

void COSGCTRL::OnBnClickedStopBtn()
{
	m_pOSG->EnqueueRequest(REQ_STOPANIMATION,0);
}

void COSGCTRL::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(m_animationRunning)
		return;

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	if(pNMLV->uNewState == LVIS_FOCUSED + LVIS_SELECTED)
	{				
		m_selectedItem = pNMLV->iItem;
		//set camera to viewPoint (EarthManipulator specific!)
		osgEarth::Util::Viewpoint* viewPoint;
		KeyFrame* key = (KeyFrame*) m_ListCtrl.GetItemData(m_selectedItem);
		viewPoint = new osgEarth::Util::Viewpoint(key->viewPoint);
		m_pOSG->EnqueueRequest(REQ_SETCAMVIEWPOINT,(LPARAM) viewPoint);
		
		//enable delete button
		m_DeleteBtn.EnableWindow(true);
	}
	else
	{
		//the item was deselected
		m_DeleteBtn.EnableWindow(false);
	}
	*pResult = 0;
}

int COSGCTRL::findItem(float time)
{
	int i = m_ListCtrl.GetItemCount()-1;

	KeyFrame* key =(KeyFrame*) m_ListCtrl.GetItemData(i);
	if((key->timeStamp + key->duration) == time)
			return i;

	for(i = 0; i < m_ListCtrl.GetItemCount()-1;i++)
	{
		key = (KeyFrame*) m_ListCtrl.GetItemData(i);
		if((key->timeStamp + key->duration) > time)
			return i;
	}

	return -1;
}

LRESULT COSGCTRL::OnOSGNotify(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch(wParam)
	{
	case RPLY_ANIMATIONPAUSED:
		{

			break;
		}
	case RPLY_ANIMATIONCOMPLETED:
		{
			m_ListCtrl.EnableWindow(true);
			m_DeleteBtn.EnableWindow(true);			
			m_CaptureBtn.EnableWindow(true);
			m_AutotimeBtn.EnableWindow(true);
			m_ClearallBtn.EnableWindow(true);
			m_ImportBtn.EnableWindow(true);			
			m_PlayBtn.EnableWindow(true);

			m_PauseBtn.EnableWindow(false);
			m_ResetBtn.EnableWindow(false);
			m_StopBtn.EnableWindow(false);

			m_ListCtrl.SetItemState(m_selectedItem,LVIS_FOCUSED | LVIS_SELECTED,LVIS_FOCUSED | LVIS_SELECTED);
			m_animationRunning = false;
			break;
		}
	case RPLY_ANIMATIONSTARTED:
		{
			m_ListCtrl.EnableWindow(false);
			m_DeleteBtn.EnableWindow(false);			
			m_CaptureBtn.EnableWindow(false);
			m_AutotimeBtn.EnableWindow(false);
			m_ClearallBtn.EnableWindow(false);
			m_ImportBtn.EnableWindow(false);			
			m_PlayBtn.EnableWindow(false);	

			m_PauseBtn.EnableWindow(true);
			m_ResetBtn.EnableWindow(true);
			m_StopBtn.EnableWindow(true);

			m_animationRunning = true;
			break;
		}
	case RPLY_ANIMATIONTIME:	
		{
			float animTime = LPARAM_TO_FLOAT(lParam);
			int item = findItem(animTime);
			if(m_selectedItem != item)
			{
				//deselect prev item
				m_ListCtrl.SetItemState(m_selectedItem,0,LVIS_FOCUSED | LVIS_SELECTED);
				//select new item
				m_selectedItem = item;
				m_ListCtrl.SetItemState(m_selectedItem,LVIS_FOCUSED | LVIS_SELECTED,LVIS_FOCUSED | LVIS_SELECTED);
			}

			//update timer
			m_sAnimationTime.Format("%f",animTime);
			UpdateData(0);
			break;
		}
	case RPLY_LOADINGEARTH:
		{

			break;
		}
	}
	return 0;	
}

void COSGCTRL::OnSize(UINT nType, int cx, int cy)
{
	CRHGenericChildDialog::OnSize(nType, cx, cy);

	if(m_osgPlaceHolder.m_hWnd)
	{
		m_osgPlaceHolder.SetWindowPos(&CWnd::wndTop,0,0,cx-150,(cy-rList.bottom) - 5,0);
		m_ListCtrl.SetWindowPos(&CWnd::wndTop,0,cy-rList.bottom,rList.right +4,rList.bottom+4,0);

		int x,y;
		x = rList.right+7;
		y = cy-rList.bottom;
		m_CaptureBtn.SetWindowPos(&CWnd::wndTop,x,y,rCaptureBtn.right,rCaptureBtn.bottom,0);
		m_PlayBtn.SetWindowPos(&CWnd::wndTop,cx-rPlayBtn.right,y,rPlayBtn.right,rPlayBtn.bottom,0);		
		y +=rCaptureBtn.bottom +3;

		m_AutotimeBtn.SetWindowPos(&CWnd::wndTop,x,y,rCaptureBtn.right,rCaptureBtn.bottom,0);
		m_PauseBtn.SetWindowPos(&CWnd::wndTop,cx-rPlayBtn.right,y,rPlayBtn.right,rPlayBtn.bottom,0);
		y +=rCaptureBtn.bottom +3;

		m_DeleteBtn.SetWindowPos(&CWnd::wndTop,x,y,rCaptureBtn.right,rCaptureBtn.bottom,0);
		m_ResetBtn.SetWindowPos(&CWnd::wndTop,cx-rPlayBtn.right,y,rPlayBtn.right,rPlayBtn.bottom,0);
		y +=rCaptureBtn.bottom +3;

		m_ClearallBtn.SetWindowPos(&CWnd::wndTop,x,y,rCaptureBtn.right,rCaptureBtn.bottom,0);
		m_StopBtn.SetWindowPos(&CWnd::wndTop,cx-rPlayBtn.right,y,rPlayBtn.right,rPlayBtn.bottom,0);
		y +=rCaptureBtn.bottom +3;

		m_ImportBtn.SetWindowPos(&CWnd::wndTop,x,y,rCaptureBtn.right,rCaptureBtn.bottom,0);y +=rCaptureBtn.bottom +3;
		m_ExportBtn.SetWindowPos(&CWnd::wndTop,x,y,rCaptureBtn.right,rCaptureBtn.bottom,0);y +=rCaptureBtn.bottom +3;
		
		RECT rect;
		m_LoadEarthBtn.GetClientRect(&rect);
		m_LoadEarthBtn.SetWindowPos(&CWnd::wndTop,cx-rect.right,0,rect.right,rect.bottom,0);

		m_TimerLabel.GetClientRect(&rect);
		m_TimerLabel.SetWindowPos(&CWnd::wndTop,cx-rect.right,cy-rect.bottom,rect.right,rect.bottom,0);
		
		Invalidate(0);

		
	}	
}
void COSGCTRL::OnDestroy()
{
	CRHGenericChildDialog::OnDestroy();

	//free list control
	m_ListCtrl.ResetList();

	delete m_pOSG;
	// TODO: Add your message handler code here
}


void COSGCTRL::OnBnClickedLoadearthBtn()
{
	CFileDialog fd(true,NULL,NULL,6UL,"EARTH model (*.earth)|*.earth||",this);
	//get filename
	if(fd.DoModal() == IDCANCEL) 
		return;

	std::string *str = new std::string(fd.GetPathName());
	m_pOSG->EnqueueRequest(REQ_LOADEARTHFILE,(LPARAM)str);
}
