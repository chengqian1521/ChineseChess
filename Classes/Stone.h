#ifndef __CHESSPIECE_H__
#define __CHESSPIECE_H__
#include "cocos2d.h"
#include "AppMacros.h"
#include "PlateIVec2.h"

USING_NS_CC;
class Stone:public Sprite
{
public:
	enum Type
	{

		VEHICLE,//��

		HORSE,//��

		ELEPHANT,//��

		SCHOLAR,//ʿ

		BOSS,//����

		GUN,//��

		SOLDIER,//��
	};
	static Stone* create(int id,bool isRedOnBelow);
	bool init(int id,bool isRedOnBelow);

	
	inline bool isLive()const{
		return _isLive;
	}
	inline bool isRed()const{
		return _isRed;
	}
	inline void isLive(bool isLive){
		_isLive = isLive;
	}
private:
	
	bool _isRed;
	bool _isLive;
		
	CC_SYNTHESIZE_PASS_BY_REF(Type, _type, Type);
	CC_SYNTHESIZE_PASS_BY_REF(int,_id , Id);
	CC_SYNTHESIZE_PASS_BY_REF(PlateIVec2, _platePoint, PlatePoint);
}; 

#endif