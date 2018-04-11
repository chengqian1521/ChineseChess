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
		TARGETISFRIEND, //Ŀ��Ϊ�ѷ�����
		CanMOVETOK,		//��������
		CanMOVEERR,		//��������
		CanKILLOK,		//����ɱ��
		CanKILLERR,		//����ɱ��
	};
public:
	static SceneGame* create(int level);



	//�жϸò��Ƿ�����������
	bool canMoveByRule(const Step& step);




	//����PlatePoint�õ�����   �ɹ���������ָ��,ʧ�ܷ���nullptr;
	Stone* &getStonePointerRefByPlatePoint(const PlateIVec2& platePoint){
		return _stonesTable[platePoint.row][platePoint.col];
	}
	

	//����CCPoint   �õ�����   �ɹ���������ָ��,ʧ�ܷ���Nullptr
	Stone* getStoneByVec2(const Vec2& vec2);
	//��Ļ����ת���ɶ�ӦPlatePoint, ת���ɹ�����true,ʧ�ܷ���false
	bool screenPointToPlatePoint(const Vec2& screen_pt, PlateIVec2& platePoint /*out*/);

	//�õ�������֮�����ӵĸ���,����ͬһ��ֱ���Ϸ���-1
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

	//�ƶ�����,�ƶ����ΪMoveStepResult
	void moveStep(const Step& step);
	void moveStepBegin(const Step& step);
	void moveStepFinshed(const Step& step);
	CanMoveResult canMove(const Step& step);

	//����ҵ����
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
	std::stack<Step> steps;//��¼�ߵ�ÿһ������Ϣ,���ڻ��幦��

	bool _isRedFirstCurrent;

	bool _isRedOnBelow;

	bool _isMoving;

	bool _isComputerCanCalculate;
	std::mutex _isComputerCanCalculateMutex;




	Sprite*  _stoneSelectedEffect;
	Stone *  _stoneSelected;

	std::vector<Stone*>  _stones; //�����ϵ�����

	Sprite* _plate; //���̱���

	bool _isOpenAI = true;

	int _level;//�����Ѷ�

	EventListenerTouchOneByOne* _listener;//���̴�������

	AI* _ai;

	//Ai���������

	ProgressTimer* _progress;
	LabelAtlas   * _progressText;

	//���浱����֡�����AI���㡣
	Stone*  _stonesTable[PLATE_ROW_NUM][PLATE_COL_NUM] = { {nullptr} };

};
#endif
