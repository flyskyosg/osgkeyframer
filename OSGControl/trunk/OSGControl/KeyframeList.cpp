#include "StdAfx.h"
#include "KeyframeList.h"


CKeyframeList::CKeyframeList(void)
{
}


CKeyframeList::~CKeyframeList(void)
{
}

bool CKeyframeList::OnEditComplete(int nRow, int nCol, CWnd* pEditor, LV_DISPINFO* pLVDI)
{
	float num = atof(pLVDI->item.pszText);
	//TODO: clarify input
	switch(nCol)
	{
	case TIME_COL:
		{
			if(nRow == 0 || num < 0.0)
				num = 0.0;//1st number must have a time of 0 and be greater than zero

			//gather data pointers				
			((KeyFrame*) GetItemData(nRow))->timeStamp = num;

			fixTimes(false);			
			break;
		}
	case DURATION_COL:
		{
			if(nRow == GetItemCount()-1)
				num = 0.0; //last keyframe can only have duration of 0
			//update changed data:
			((KeyFrame*) GetItemData(nRow))->duration = num;

			fixTimes(true);
			break;
		}
	}

	return false;
}
void CKeyframeList::fixTimes(bool duration)
{
	//TODO:This could be more efficient...
	CString str;
	if(duration)
	{
		float time = 0;
		int i;
		for(i = 0; i < GetItemCount()-1;i++){
				KeyFrame* key = (KeyFrame*) GetItemData(i);
				key->timeStamp = time;
								
				str.Format("%f",time);
					SetItemText(i,TIME_COL,str);
				str.Format("%f",key->duration);
					SetItemText(i,DURATION_COL,str);

				time += key->duration;
		}
		//take care of last item
		KeyFrame* key = (KeyFrame*) GetItemData(i);
		key->timeStamp = time;
		key->duration = 0;						
		str.Format("%f",time);
			SetItemText(i,TIME_COL,str);
		str.Format("%f",key->duration);
			SetItemText(i,DURATION_COL,str);

	}
	else
	{	
		int i;
		for(i = 0; i < GetItemCount()-1;i++){
				KeyFrame* key1 = (KeyFrame*) GetItemData(i);
				KeyFrame* key2 = (KeyFrame*) GetItemData(i+1);
				key1->duration = key2->timeStamp - key1->timeStamp;
								
				str.Format("%f",key1->timeStamp);
					SetItemText(i,TIME_COL,str);
				str.Format("%f",key1->duration);
					SetItemText(i,DURATION_COL,str);				
		}

		//take care of last item
		KeyFrame* key = (KeyFrame*) GetItemData(i);
		key->duration = 0;
		str.Format("%f",key->timeStamp);
			SetItemText(i,TIME_COL,str);
		str.Format("%f",key->duration);
			SetItemText(i,DURATION_COL,str);
	}
}
void CKeyframeList::ResetList()
{
	for(int i= 0; i < GetItemCount(); i++){
		delete (KeyFrame*) GetItemData(i);		
	}
	DeleteAllItems();
}
void CKeyframeList::DeleteItem(int item)
{
	delete (KeyFrame*) GetItemData(item);
	CListCtrl::DeleteItem(item);

	if(item == 0)	
		((KeyFrame*)GetItemData(0))->distFromPrev = 0;	
	else 
		if(item != GetItemCount())
		{		
			osg::Vec3 a = ((KeyFrame*)GetItemData(item))->ctrlPoint.getPosition();
			osg::Vec3 b = ((KeyFrame*)GetItemData(item-1))->ctrlPoint.getPosition();

			a = b-a;
			((KeyFrame*)GetItemData(item))->distFromPrev = sqrt(a.x() * a.x() + a.y() * a.y() + a.z() * a.z());
		}

	fixTimes(true);
}	

void CKeyframeList::AddItem(KeyFrame* key)
{
	//setup item data
	int nItem = GetItemCount();	
	CString str; str.Format("%d",nItem);

	if(nItem == 0)
		key->distFromPrev = 0;
	else
	{		
		osg::Vec3 a = key->ctrlPoint.getPosition();
		osg::Vec3 b = ((KeyFrame*)GetItemData(nItem-1))->ctrlPoint.getPosition();

		a = b-a;
		key->distFromPrev = sqrt(a.x() * a.x() + a.y() * a.y() + a.z() * a.z());
	}

	//set and update list control
	InsertItem(nItem,str);
	SetItemData(nItem, (DWORD_PTR) key);	

	//fixTimes(true);	

	str.Format("%f",key->distFromPrev);
		SetItemText(nItem,DISTANCE_COL,str);	
}