#ifndef __COMMON_H__
#define __COMMON_H__

#include "AppMacros.h"
#include "util.h"

inline bool isInPlate(const PlateIVec2& iv2) {
	return iv2.col >= 0 && iv2.row >= 0 && iv2.col < PLATE_COL_NUM && iv2.row < PLATE_ROW_NUM;
}

#endif