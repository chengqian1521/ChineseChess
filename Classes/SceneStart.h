#ifndef __SCENESTART_H__
#define __SCENESTART_H__
 // !__SceneStart_H__
#include "cocos2d.h"
USING_NS_CC;
class SceneStart:public Scene
{
public:
	CREATE_FUNC(SceneStart);
	bool init();

	void createBackground();

private:
	Sprite* _start;
	Sprite* _level;
	Sprite* _difficulty;
	int    _curLevel=3;
	int    _minLevel = 1;
	int    _maxLevel;
};

#endif