#ifndef __AISTEP_H__
#define __AISTEP_H__

#include <vector>
#include <thread>
#include <mutex>


#include "Step.h"

class MainScene;
class Step;

class AI
{
public:
	AI(MainScene *game,int searchLevel);
	~AI();
public:

	enum AIState
	{
		idle,
		calcing,
		newRes
	};

	//价格表
	static int _prices[];

	void calcAiThreadFunc();

	 //搜索深度为
	Step getStep(int level);
	 //计算分数
	int  calcScore();
	
	int  getMinScore(int level, int curMaxScore);
	int  getMaxScore(int level, int curMinScore);
	 
	 //得到所有可能的Step
	void genAllPossibleSteps(std::vector<Step*> &steps);

	void genVehiclePossibleSteps(Stone* stone, std::vector<Step*> &steps);
	void genHorsePossibleSteps(Stone* stone, std::vector<Step*> &steps);
	void genElephantPossibleSteps(Stone* stone, std::vector<Step*> &steps);
	void genScholarPossibleSteps(Stone* stone, std::vector<Step*> &steps);
	void genBossPossibleSteps(Stone* stone, std::vector<Step*> &steps);
	void genGunPossibleSteps(Stone* stone, std::vector<Step*> &steps);
	void genSoldierPossibleSteps(Stone* stone, std::vector<Step*> &steps);

	void testTargetCanMoveAndAddVectorOrDelete(const PlateIVec2 &target,Stone*stone, std::vector<Step*> &steps);

	void deleteAllPossibleSteps(const std::vector<Step*> &steps);


	 int  getProgress();
	 void fakeMove(const Step& step);
	 void unfakeMove(const Step& step);

	
	 int alphaBetaSearch(int alpha,int beta,int level);
	 
public:
	int    _searchLevel;
	int		_maxLevel;
	int    _progress;
	std::mutex _progressMutex;

	Step	_moveStep;

	std::mutex _aiStateMutex;
	AIState _aiState;
	MainScene*  _game;
	int _level;

	std::thread* _calcThread;

	bool _shouleExit;
};

#endif