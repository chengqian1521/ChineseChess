#ifndef __PLATEPOINT_H__
#define __PLATEPOINT_H__

#include "common.h"


//整数型二维向量
class PlateIVec2
{
public:
	PlateIVec2(int row=0, int col=0);
	
	PlateIVec2& operator+=(const PlateIVec2& iv2) {
		this->col += iv2.col;
		this->row += iv2.row;
		return*this;
	}

	virtual ~PlateIVec2();
	Vec2 toScreenPoint()const;
	PlateIVec2 toRotate()const;
public:
	bool operator==(const PlateIVec2& platePoint)const;
public:
	int row;
	int col;
	
};

inline bool isInPlate(const PlateIVec2& iv2) {
	return iv2.col >= 0 && iv2.row >= 0 && iv2.col < PLATE_COL_NUM && iv2.row < PLATE_ROW_NUM;
}


#endif