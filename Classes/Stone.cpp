#include "Stone.h"
#include"SceneGame.h"
#include <string>
Stone* Stone::create(int id,bool isRedOnBelow){
	Stone* pRet = new Stone();
	if (pRet&&pRet->init(id, isRedOnBelow)){
		pRet->autorelease();
	}
	else{
		delete pRet;
		pRet = NULL;
	}

	return pRet;
}
bool Stone::init(int id, bool isRedOnBelow){
	Sprite::init();
	/*
	����Ϊ��ת�Գ�
	��   ��     ��
	5    6      7
	��
	4
	�� �� �� ʿ  �� ʿ  ��  ��  ��
	0  1  2  3  8	   11  10   9
	*/
	static struct {
		Stone::Type _chessType;
		int  _rowNum;
		int  _colNum;
	} proper[] = {
		{ Stone::Type::VEHICLE, 0, 0 },
		{ Stone::Type::HORSE, 0, 1 },
		{ Stone::Type::ELEPHANT, 0, 2 },
		{ Stone::Type::SCHOLAR, 0, 3 },
		{ Stone::Type::GUN, 2, 1 },
		{ Stone::Type::SOLDIER, 3, 0 },
		{ Stone::Type::SOLDIER, 3, 2 },
		{ Stone::Type::SOLDIER, 3, 4 },
		{ Stone::Type::BOSS, 0, 4 },
	};

	if (id <= 8)
	{
		
		_platePoint = PlateIVec2(proper[id]._rowNum,
								 proper[id]._colNum);
		_type=proper[id]._chessType;

	}
	else if (8 < id && id <= 15){

		
		_platePoint = PlateIVec2( proper[id - 9]._rowNum,
								 8 - proper[id - 9]._colNum );

		_type= proper[id - 9]._chessType;

	}
	else if (id > 15 && id <= 24){
		
		_platePoint = PlateIVec2( 9 - proper[id - 16]._rowNum,
								 8 - proper[id - 16]._colNum);
		_type = proper[id - 16]._chessType;

	}
	else{
		
		_platePoint = PlateIVec2( 9 - proper[id - 25]._rowNum,
								 proper[id - 25]._colNum);
		_type = proper[id - 25]._chessType;
	}


	//��ʼ����������
	this->_id = id;
	this->_isRed = ((id<16) == isRedOnBelow);
	this->_isLive = true;

	this->setPosition(_platePoint.toScreenPoint());
	
	
	//��ʼ������ͼƬ
	
	static char* picNames[] = {
		"rche.png",
		"rma.png",
		"rxiang.png",
		"rshi.png",
		"rshuai.png",
		"rpao.png",
		"rbing.png",

		"bche.png",
		"bma.png",
		"bxiang.png",
		"bshi.png",
		"bjiang.png",
		"bpao.png",
		"bzu.png",
	};
	int idx = (_isRed ? 0 : 1) * 7 + _type;
	std::string picName(picNames[idx]);
	

	Texture2D* texture = Director::getInstance()->getTextureCache()->
		addImage(picName);

	this->setTexture(texture);
	this->setTextureRect(Rect(Vec2(0,0),texture->getContentSize()));
	this->setScale(SceneGame::sm_r/texture->getContentSize().width);
	

	return true;
}
