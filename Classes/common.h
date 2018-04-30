#ifndef __COMMON_H__
#define __COMMON_H__


#include "cocos2d.h"
USING_NS_CC;

#define PLATE_ROW_NUM  10
#define PLATE_COL_NUM  9

#define MAX_LEVEL   8

#define  winSize cocos2d::Director::getInstance()->getWinSize()
#define  center Vec2(winSize.width/2,winSize.height/2)

#define IMAGE_Loading__AnimCoil  "Loading__AnimCoil.png"
#define IMAGE_Labelatlas  "Labelatlas.png"


extern const cocos2d::Size g_winSize;
#endif