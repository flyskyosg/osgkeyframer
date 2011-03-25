#pragma once
#include "osgGA\AnimationPathManipulator"


class COSGAnimationPathManipulator : public osgGA::AnimationPathManipulator 
{
public:

	COSGAnimationPathManipulator( osg::AnimationPath* animationPath=0 );
    COSGAnimationPathManipulator( const std::string& filename );
	virtual const char* className() const { return "CustomAnimationPath"; }

	bool animationCompleted(double time);
	bool animationIsPaused() {return _isPaused; }
	double getAnimationTime(double reftime);
};

