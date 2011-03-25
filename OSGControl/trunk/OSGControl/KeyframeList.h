#pragma once
#include "L:\projects\test\test\cgridlistctrlex\cgridlistctrlex.h"
#include "osg\AnimationPath"
#include "osgEarthUtil\EarthManipulator"

#define LABEL_COL		0
#define TIME_COL		1
#define DURATION_COL	2
#define	DISTANCE_COL	3
typedef struct 
{
	osg::AnimationPath::ControlPoint ctrlPoint;
	float timeStamp;
	float duration;
	double distFromPrev;
	osgEarthUtil::Viewpoint viewPoint;
}KeyFrame;

class CKeyframeList :
	public CGridListCtrlEx
{
public:
	CKeyframeList(void);
	~CKeyframeList(void);

	void AddItem(KeyFrame* key);
	void fixTimes(bool duration);
	void AddToList();
	void DeleteItem(int item);
	void ResetList();
	void SelectItem(int item);

protected:
	virtual bool OnEditComplete(int nRow, int nCol, CWnd* pEditor, LV_DISPINFO* pLVDI);
};

