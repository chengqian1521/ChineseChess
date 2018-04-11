#include "AI.h"
#include "vector"
#include "cocos2d.h"
#include "Step.h"
#include "SceneGame.h"
#include "Stone.h"
#include "common.h"

#include <algorithm>

#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif // WIN32


int AI::_prices[] = {

	200,//车

	70,//马

	10,//象

	11,//士

	15000,//将军

	80,//炮

	20//卒
};

//根据当前局面产生一个step
AI::AI(SceneGame *game, int searchLevel) :_game(game), _searchLevel(searchLevel) {
	_maxLevel = 0;

	_aiState = idle;
	_shouleExit = false;
	_calcThread = new std::thread(&AI::calcAiThreadFunc, this);
	_progress = 0;

}

AI::~AI() {

	_shouleExit = true;
	_calcThread->join();
	delete _calcThread;
}

void AI::calcAiThreadFunc() {

	while (!_shouleExit) {
	

		if (this->_game->_isComputerCanCalculate) {


			_aiState = calcing;
		
			_progress = 0;
			_moveStep = getStep(_searchLevel);
			_progress = 0;

			_aiState = newRes;
		
			this->_game->_isComputerCanCalculateMutex.lock();
			this->_game->_isComputerCanCalculate = false;
			this->_game->_isComputerCanCalculateMutex.unlock();
		}
		else {
		
#ifdef WIN32
			Sleep(16);
#else
			usleep(16000);
#endif
		}

	}

}

int  AI::getProgress() {

	return _progress;
}

int  AI::calcScore() {
	int computScore = 0;
	int playerScore = 0;

	for (auto stone : this->_game->_stones) {

		if (stone->isLive()) {
			if (stone->isRed())
				playerScore += _prices[stone->getType()];
			else {
				if (stone->getType() == Stone::BOSS)
				  computScore += 1500000;
				else
				  computScore += _prices[stone->getType()];
			}
		}

#if 0

		else {
			if (!stone->isRed() && stone->getType() == Stone::BOSS) {
				//电脑的老将挂了，这是电脑最不想看到的结果
				return INT_MIN;
			}
		}
#endif

	}


	return computScore - playerScore;
}

//得到所有可能的Step
void AI::genAllPossibleSteps(std::vector<Step*> &steps)
{


	int min_ind = _game->_isRedFirstCurrent ? 0 : 16;
	int max_ind = _game->_isRedFirstCurrent ? 15 : 31;

	for (int i = min_ind; i <= max_ind; ++i) {
		Stone* stone = _game->_stones[i];
		if (!stone->isLive())
			continue;
		Stone::Type type = stone->getType();
		switch (type)
		{
		case Stone::VEHICLE:
			genVehiclePossibleSteps(stone,steps);
			break;
		case Stone::HORSE:
			genHorsePossibleSteps(stone, steps);
			break;
		case Stone::ELEPHANT:
			genElephantPossibleSteps(stone, steps);
			break;
		case Stone::SCHOLAR:
			genScholarPossibleSteps(stone, steps);
			break;
		case Stone::BOSS:
			genBossPossibleSteps(stone, steps);
			break;
		case Stone::GUN:
			genGunPossibleSteps(stone, steps);
			break;
		case Stone::SOLDIER:
			genSoldierPossibleSteps(stone, steps);
			break;
		default:
			break;
		}


	}


}
void AI::genVehiclePossibleSteps(Stone* stone, std::vector<Step*> &steps) {
	
	PlateIVec2 target;
	target.row = stone->getPlatePoint().row;
	for (int col = 0; col < PLATE_COL_NUM; ++col)
	{
		target.col = col;
		testTargetCanMoveAndAddVectorOrDelete(target,stone,steps);
	}
	target.col = stone->getPlatePoint().col;
	for (int row = 0; row < PLATE_ROW_NUM; ++row) {
		target.row = row;
		testTargetCanMoveAndAddVectorOrDelete(target, stone, steps);
	}
}
void AI::genHorsePossibleSteps(Stone* stone, std::vector<Step*> &steps) {
	PlateIVec2 target;
	
	int rowBegin = std::max(stone->getPlatePoint().row - 2,0);
	int rowEnd = std::min(stone->getPlatePoint().row + 2,PLATE_ROW_NUM-1);


	int colBegin = std::max(0, stone->getPlatePoint().col - 2);
	int colEnd = std::min(PLATE_COL_NUM - 1, stone->getPlatePoint().col + 2);

	for (int col = colBegin; col <= colEnd; ++col)
	{
		target.col = col;
		for (int row = rowBegin; row <= rowEnd; ++row) {
			target.row = row;
			testTargetCanMoveAndAddVectorOrDelete(target, stone, steps);
		}
	}
}
void AI::genElephantPossibleSteps(Stone* stone, std::vector<Step*> &steps) {

	//
	static int col_offs[] = {2,-2};
	static int row_offs[] = { 2,-2 };

	for(auto col_off: col_offs)
		for (auto row_off : row_offs)
		{
			PlateIVec2 target1(stone->getPlatePoint());
			target1 += PlateIVec2(row_off, col_off);
			if (isInPlate(target1))
				testTargetCanMoveAndAddVectorOrDelete(target1, stone, steps);
		}
	

}
void AI::genScholarPossibleSteps(Stone* stone, std::vector<Step*> &steps) {
	static int col_offs[] = { 1,-1 };
	static int row_offs[] = { 1,-1 };

	for (auto col_off : col_offs)
		for (auto row_off : row_offs)
		{
			PlateIVec2 target1(stone->getPlatePoint());
			target1 += PlateIVec2(row_off, col_off);
			if (isInPlate(target1))
				testTargetCanMoveAndAddVectorOrDelete(target1, stone, steps);
		}

}
void AI::genBossPossibleSteps(Stone* stone, std::vector<Step*> &steps) {
	static int col_offs[] = { 1,0,-1 };
	static int row_offs[] = { 1,0,-1 };

	for (auto col_off : col_offs)
		for (auto row_off : row_offs)
		{
			PlateIVec2 target1(stone->getPlatePoint());
			target1 += PlateIVec2(row_off, col_off);
			if (isInPlate(target1))
				testTargetCanMoveAndAddVectorOrDelete(target1, stone, steps);
		}

}
void AI::genGunPossibleSteps(Stone* stone, std::vector<Step*> &steps) {
	//炮可车生成的可能走的范围一样
	genVehiclePossibleSteps(stone, steps);
}
void AI::genSoldierPossibleSteps(Stone* stone, std::vector<Step*> &steps) {
	//卒和老蒋 最大走的范围一样
	genBossPossibleSteps(stone, steps);
}
void AI::testTargetCanMoveAndAddVectorOrDelete(const PlateIVec2 &target, Stone*stone, std::vector<Step*> &steps) {
	Stone* targetStone = _game->getStonePointerRefByPlatePoint(target);
	if (targetStone&&stone->isRed() == targetStone->isRed())
		return;
	Step *step = new Step(stone,
						  stone->getPlatePoint(),
						  targetStone,
						  target);
	if (_game->canMoveByRule(*step))
		steps.push_back(step);
	else
		delete step;
	
}
void AI::deleteAllPossibleSteps(const std::vector<Step*> &steps) {
	for (auto p : steps)
		delete p;
}

void AI::fakeMove(const Step& step) {
	if (step._targetStone)
		step._targetStone->isLive(false);

	step._srcStone->setPlatePoint(step._targetPlatePoint);
	_game->getStonePointerRefByPlatePoint(step._srcPlatePoint) = nullptr;
	_game->getStonePointerRefByPlatePoint(step._targetPlatePoint) = step._srcStone;
	_game->_isRedFirstCurrent = !_game->_isRedFirstCurrent;

}
void AI::unfakeMove(const Step& step) {
	if (step._targetStone)
		step._targetStone->isLive(true);
	step._srcStone->setPlatePoint(step._srcPlatePoint);
	_game->getStonePointerRefByPlatePoint(step._srcPlatePoint) = step._srcStone;
	_game->getStonePointerRefByPlatePoint(step._targetPlatePoint) = step._targetStone;
	_game->_isRedFirstCurrent = !_game->_isRedFirstCurrent;
}

////////////////////////////////////////////////////
//搜索深度为level
Step AI::getStep(int level) {

	int highScore = INT_MIN;

	//产生所有可能的移动
	std::vector<Step*> allSteps;
	genAllPossibleSteps(allSteps);


	Step *stepRet;
	int score;

	int i = 0;

	for (auto step : allSteps) {
		
		_progress = 100 * i++ / allSteps.size();
		
		fakeMove(*step);
		score = getMinScore(level - 1, highScore);
		unfakeMove(*step);

		if (score > highScore) {
			highScore = score;
			stepRet = step;
		}

	}

	Step temp = *stepRet;

	deleteAllPossibleSteps(allSteps);
	return temp;
}

int  AI::getMinScore(int level, int curMaxScore) {

	if (!level)
		return calcScore();

	int minScore = INT_MAX;
	//产生所有可能的移动
	std::vector<Step*> allSteps;
	genAllPossibleSteps(allSteps);



	int score;
	for (auto step : allSteps) {
		fakeMove(*step);
		score = getMaxScore(level - 1, minScore);
		unfakeMove(*step);

		//剪枝
		if (score <= curMaxScore) {
			minScore = score;
			break;
		}
		
		if (score < minScore)
			minScore = score;
		
	}
	deleteAllPossibleSteps(allSteps);
	return minScore;
}

int  AI::getMaxScore(int level, int curMinScore) {

	if (!level)
		return calcScore();

	int maxScore = INT_MIN;
	//产生所有可能的移动
	std::vector<Step*> allSteps;
	genAllPossibleSteps(allSteps);


	int score;
	for (auto step : allSteps) {
		fakeMove(*step);
		score = getMinScore(level - 1, maxScore);
		unfakeMove(*step);


		//剪枝
		if (score >= curMinScore) {
			maxScore = score;
			break;
		}
		
		if (score > maxScore)
			maxScore = score;
		
	}
	deleteAllPossibleSteps(allSteps);
	return maxScore;
}

#if 0
int AI::alphaBetaSearch(int alpha, int beta, int level) {

	if (!level)
		return calcScore();

	//产生所有可能的移动
	std::vector<Step> allSteps;
	getAllPossibleSteps(allSteps);
	std::vector<Step>::iterator ib = allSteps.begin();
	std::vector<Step>::iterator ie = allSteps.end();

	while (ib != ie) {
		Step& step = *ib;
		fakeMove(step);
		int score = -alphaBetaSearch(-beta, -alpha, level - 1);
		unfakeMove(step);

		//剪枝
		if (score >= beta) return beta;

		//获得最小值
		if (score > alpha) {
			alpha = score;
			//记录最可能的有利的移动步骤
			if (level == _maxLevel) {
				_moveStep = step;
			}
		}

		++ib;
	}

	return alpha;
}

#endif