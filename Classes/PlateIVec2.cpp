#include "PlateIVec2.h"
#include "MainScene.h"



PlateIVec2::PlateIVec2(int row, int col) :row(row), col(col)
{

}


PlateIVec2::~PlateIVec2()
{

}

cocos2d::Vec2 PlateIVec2::toScreenPoint() const
{
	return Vec2(col, row)*MainScene::sm_r + MainScene::sm_offVec2;
}

bool PlateIVec2::operator==(const PlateIVec2& platePoint)const{
	return this->row == platePoint.row &&this->col == platePoint.col;
}

PlateIVec2 PlateIVec2::toRotate() const
{
	return PlateIVec2(9 - this->row, 8 - this->col);
}