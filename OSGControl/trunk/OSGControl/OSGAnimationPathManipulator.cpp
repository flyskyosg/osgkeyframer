#include "StdAfx.h"
#include "OSGAnimationPathManipulator.h"


COSGAnimationPathManipulator::COSGAnimationPathManipulator( osg::AnimationPath* animationPath )
	:AnimationPathManipulator(animationPath)
{	
}

COSGAnimationPathManipulator::COSGAnimationPathManipulator( const std::string& filename )
	:AnimationPathManipulator(filename)
{		
}

bool COSGAnimationPathManipulator::animationCompleted(double time)
{
	return getAnimationTime(time) == _animationPath->getPeriod();
}

double COSGAnimationPathManipulator::getAnimationTime(double reftime)
{	
	double animTime;
	if(_isPaused)
		animTime = _pauseTime + _timeOffset;
	else
		animTime = reftime + _timeOffset;

	if(animTime > _animationPath->getPeriod())
		animTime = _animationPath->getPeriod();

	return animTime;
}
