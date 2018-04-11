#include "SceneStart.h"
#include "AppMacros.h"
#include <sstream>
#include <vector>
#include "SceneGame.h"

bool SceneStart::init(){
	Scene::init();


	createBackground();
	Sprite* difficulty = Sprite::create("difficulty.jpg");
	_difficulty = difficulty;
	difficulty->setPosition(winSize.width / 4, winSize.height / 2);
	addChild(difficulty);
	
	_maxLevel = MAX_LEVEL;
	_level = Sprite::create();
	SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();
	for (int i = _minLevel; i <= _maxLevel; ++i){
		std::ostringstream os;
		os << "d" << i << ".png";
		Texture2D* textTure = Director::getInstance()->getTextureCache()->addImage(os.str());
		SpriteFrame* frame = SpriteFrame::createWithTexture(textTure,Rect(Vec2(0,0),textTure->getContentSize()));
		std::ostringstream spriteFrameName;

		spriteFrameName << "level" << i;
		
		frameCache->addSpriteFrame(frame, spriteFrameName.str());
	}
	_level->setSpriteFrame(frameCache->getSpriteFrameByName("level3"));
	_level->setPosition(winSize.width / 2, winSize.height / 2);
	addChild(_level);


	_start = Sprite::create("start.jpg");
	_start->setPosition(winSize.width * 3 / 4, winSize.height / 2);
	addChild(_start);
	
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();

	listener->onTouchBegan=[&](Touch*touch, Event*){
		Vec2 touchPoint = touch->getLocation();
		if (_start->getBoundingBox().containsPoint(touchPoint)){
			//ÓÎÏ·¿ªÊ¼
			Director::getInstance()->replaceScene(SceneGame::create(_curLevel));
		
		}
		else if (_level->getBoundingBox().containsPoint(touchPoint)
				 || _difficulty->getBoundingBox().containsPoint(touchPoint)){

			if (_curLevel < _maxLevel)
				++_curLevel;		
			else
				_curLevel = _minLevel;
			
			std::ostringstream os;
			os << "level" << _curLevel;
			SpriteFrame* frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(os.str());
			_level->setSpriteFrame(frame);

		}
		return false;
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener,this);


	return true;
}
void SceneStart::createBackground(){
	Sprite* background = Sprite::create("floor.jpg");
	background->setScaleX(winSize.width / background->getContentSize().width);
	background->setScaleY(winSize.height / background->getContentSize().height);
	background->setPosition(winSize.width / 2, winSize.height / 2);
	this->addChild(background);
}