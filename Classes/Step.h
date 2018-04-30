#ifndef __STEP_H__
#define __STEP_H__
#include "common.h"
#include "PlateIVec2.h"
class Stone;


class Step
{
public:
	Step(Stone *srcStone,const PlateIVec2& srcPlatePonit, Stone *targetStone,
		  const PlateIVec2& targetPlatePoint);
	Step(){
	}
	virtual ~Step();
public:
	Stone *_srcStone;

	Stone *_targetStone;//��������ƶ�û��ɱ��,��ΪNULL

	PlateIVec2 _targetPlatePoint;
	PlateIVec2 _srcPlatePoint; //���ڻ���
};
#endif