#ifndef __SCENEGAME_H__
#define __SCENEGAME_H__
#include "cocos2d.h"
#include "AppMacros.h"
#include "PlateIVec2.h"
#include <vector>
#include <stack> 
#include "Step.h"

USING_NS_CC;

class Stone;
class AI;
class SceneGame :public Scene
{
public:
	enum CanMoveResult
	{
		TARGETISFRIEND, //目标为友方棋子
		CanMOVETOK,		//可以走棋
		CanMOVEERR,		//不能走棋
		CanKILLOK,		//可以杀棋
		CanKILLERR,		//不能杀棋
	};
public:
	static SceneGame* create(int level);



	//判断该步是否符合象棋规则
	bool canMoveByRule(const Step& step);




	//根据PlatePoint得到棋子   成功返回棋子指针,失败返回nullptr;
	Stone* &getStonePointerRefByPlatePoint(const PlateIVec2& platePoint){
		return _stonesTable[platePoint.row][platePoint.col];
	}
	

	//根据CCPoint   得到棋子   成功返回棋子指针,失败返回Nullptr
	Stone* getStoneByVec2(const Vec2& vec2);
	//屏幕坐标转换成对应PlatePoint, 转换成功返回true,失败返回false
	bool screenPointToPlatePoint(const Vec2& screen_pt, PlateIVec2& platePoint /*out*/);

	//得到两个点之间棋子的个数,不在同一条直线上返回-1
	int getStoneCountInTwoPlatePoint(const PlateIVec2& ppt1, const PlateIVec2& ppt2);

private:
	bool init(int level);
	void createBackground();
	void createPlate();
	void addCtrlPanel();
	void createStones();
	void addProgressBar();


	void onClickThisPoint(const PlateIVec2&clinkedPlatePoint);
	void onClickWithNoSelected(const PlateIVec2& clinkedPlatePoint);
	void onClickWithSelected(const PlateIVec2& clinkedPlatePoint);

	bool canMoveVehicle(const Step& step);
	bool canMoveHorse(const Step& step);
	bool canMoveElephant(const Step& step);
	bool canMoveScholar(const Step& step);
	bool canMoveBoss(const Step& step);
	bool canMoveGun(const Step& step);
	bool canMoveSoldier(const Step& step);

	//移动棋子,移动结果为MoveStepResult
	void moveStep(const Step& step);
	void moveStepBegin(const Step& step);
	void moveStepFinshed(const Step& step);
	CanMoveResult canMove(const Step& step);

	//悔棋业务处理
	void regretItemCallback(Ref*);

	virtual void onEnter()override;
	virtual void onExit()override;

	virtual void update(float delta);

	void playerWin();

public:
	static int sm_r;
	static Vec2 sm_offVec2;

private:
public:
	std::stack<Step> steps;//记录走的每一步的信息,用于悔棋功能

	bool _isRedFirstCurrent;

	bool _isRedOnBelow;

	bool _isMoving;

	bool _isComputerCanCalculate;
	std::mutex _isComputerCanCalculateMutex;




	Sprite*  _stoneSelectedEffect;
	Stone *  _stoneSelected;

	std::vector<Stone*>  _stones; //棋盘上的棋子

	Sprite* _plate; //棋盘背景

	bool _isOpenAI = true;

	int _level;//单机难度

	EventListenerTouchOneByOne* _listener;//棋盘触摸监听

	AI* _ai;

	//Ai计算进度条

	ProgressTimer* _progress;
	LabelAtlas   * _progressText;

	//保存当期棋局。便于AI运算。
	Stone*  _stonesTable[PLATE_ROW_NUM][PLATE_COL_NUM] = { {nullptr} };

};
#endif
