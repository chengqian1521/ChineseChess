#include "Step.h"


Step::Step(Stone *srcStone, const PlateIVec2& srcPlatePonit, Stone *targetStone,
		   const PlateIVec2& targetPlatePoint) :
		   _srcStone(srcStone), _srcPlatePoint(srcPlatePonit), _targetStone(targetStone),
		   _targetPlatePoint(targetPlatePoint)
{

}


Step::~Step()
{
}
