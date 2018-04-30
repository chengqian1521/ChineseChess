
#include "MainScene.h"

#include "Stone.h"
#include "Ai.h"
#include "SceneStart.h"

#include "common.h"

#include <thread>
#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif // DEBUG
int MainScene::sm_r = 0;
Vec2 MainScene::sm_offVec2(0, 0);

MainScene* MainScene::create(int level) {
	MainScene* pRet = new MainScene();
	if (pRet&&pRet->init(level)) {
		pRet->autorelease();
	}
	else {
		delete pRet;
		pRet = nullptr;
	}
	return pRet;
}
bool MainScene::init(int level) {
	Scene::init();
	_level = level;
	_isRedOnBelow = true;
	_isRedFirstCurrent = true; //Ĭ�Ϻ췽����
	_stoneSelected = nullptr;//һ��ʼû��ѡ������



	_isMoving = false;
	_isComputerCanCalculate = false;

	createBackground();
	createPlate();
	addCtrlPanel();
	createStones();
	addProgressBar();

	_stoneSelectedEffect = Sprite::create("selected.png");
	_stoneSelectedEffect->setVisible(false);
	_stoneSelectedEffect->setScale(sm_r / _stoneSelectedEffect->getContentSize()
								   .width);
	_plate->addChild(_stoneSelectedEffect);

	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	_listener = listener;
	listener->onTouchBegan = [&](Touch* touch, Event*event) {
		return true;
	};
	listener->onTouchEnded = [&](Touch* pTouch, Event*) {


		if (pTouch->getStartLocation().getDistanceSq(pTouch->getLocation()) > 25) {
			return;
		}


		//����ת��
		PlateIVec2 clinkedPlatePoint;
		Vec2 vec2(pTouch->getLocation());
		if (!MainScene::screenPointToPlatePoint(vec2, clinkedPlatePoint)) {
			//����ת��ʧ��,ֱ�ӷ���
			return;
		}

		onClickThisPoint(clinkedPlatePoint);
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, _plate);
	return true;
}

void MainScene::createBackground() {
	Sprite* background = Sprite::create("floor.jpg");
	background->setScaleX(winSize.width / background->getContentSize().width);
	background->setScaleY(winSize.height / background->getContentSize().height);
	background->setPosition(winSize.width / 2, winSize.height / 2);
	this->addChild(background);
}
void MainScene::createPlate() {

	_plate = Sprite::create("background.png");
	int adjust = winSize.height / 24;
	_plate->setAnchorPoint(Vec2(0, 0));
	_plate->setPosition(Vec2(adjust, adjust));


	_plate->setScale((winSize.height - 2 * adjust) / _plate->getContentSize().height);
	addChild(_plate);


	MainScene::sm_r = _plate->getContentSize().height / 10;
	sm_offVec2.x = sm_r;
	sm_offVec2.y = sm_r / 2;

}
void MainScene::addCtrlPanel() {
	Menu* menu = Menu::create();
	menu->setPosition(menu->getPosition() + Size(winSize.width*.7f / 2, 0));
	addChild(menu);

	MenuItemImage * regretItem = MenuItemImage::create(std::string("regret.jpg"),
													   std::string("regret.jpg"),
													   std::bind(&MainScene::regretItemCallback, this,
																 std::placeholders::_1));
	auto closeItemCallback = [&](Ref*) {

		Director::getInstance()->popScene();
	};
	MenuItemImage* closeGameItem = MenuItemImage::create(std::string("CloseNormal.png"),
														 std::string("CloseSelected.png"),
														 closeItemCallback);
	closeGameItem->setPosition(closeGameItem->getPosition() - Vec2(0, winSize.height / 3));
	auto newSituationItemCallback = [&](Ref*) {

		Director::getInstance()->replaceScene(SceneStart::create());
	};
	MenuItemImage* newSituationItem = MenuItemImage::create(std::string("new.jpg"),
															std::string("new.jpg"),
															newSituationItemCallback);

	newSituationItem->setPosition(newSituationItem->getPosition() + Vec2(0, winSize.height / 5));
	menu->addChild(newSituationItem);
	menu->addChild(regretItem);
	menu->addChild(closeGameItem);

	//menu->alignItemsVertically();

}
void MainScene::createStones() {

	for (int i = 0; i < 32; i++) {
		Stone* stone = Stone::create(i, _isRedOnBelow);
		_stones.push_back(stone);
		_plate->addChild(stone);
		_stonesTable[stone->getPlatePoint().row][stone->getPlatePoint().col] = stone;
	}

}
void MainScene::addProgressBar() {
	_progress = ProgressTimer::create(Sprite::create(IMAGE_Loading__AnimCoil));
	_progress->setPosition(g_winSize.width * .7f, g_winSize.height * .5f);
	_progress->setScale(0.5f);
	_progress->setPercentage(0);
	_progress->setAnchorPoint(Vec2(.5f,.5f));
	//_progress->setMidpoint(Vec2(.5f, .5f));
	_progress->setType(ProgressTimer::Type::RADIAL);

	_progressText = LabelAtlas::create("", IMAGE_Labelatlas, 24, 32, '0');
	_progressText->setPosition(g_winSize.width * .7f, g_winSize.height * .5f);
	_progressText->setAnchorPoint(Vec2(.5f, .5f));
	addChild(_progressText);
	addChild(_progress);
}

Stone* MainScene::getStoneByVec2(const Vec2& vec2) {
	PlateIVec2  platePoint;
	if (!screenPointToPlatePoint(vec2, platePoint)) {
		return nullptr;
	}
	return getStonePointerRefByPlatePoint(platePoint);
}
bool MainScene::screenPointToPlatePoint(const Vec2& screen_pt, PlateIVec2 & platePoint) {


	if (!_plate->getBoundingBox().containsPoint(screen_pt)) {
		return false;
	}
	Vec2 pointInPlate = _plate->convertToNodeSpace(screen_pt);

	platePoint.row = pointInPlate.y / MainScene::sm_r;
	platePoint.col = (pointInPlate.x - MainScene::sm_r / 2) / MainScene::sm_r;

	return true;
}
int MainScene::getStoneCountInTwoPlatePoint(const PlateIVec2& ppt1,
											const PlateIVec2& ppt2) {
	int countRet = 0;
	if (ppt1.col == ppt2.col&&ppt1.row == ppt2.row) {
		return 0;
	}
	if (ppt1.col != ppt2.col&&ppt1.row != ppt2.row) {
		return -1;
	}
	if (ppt1.col == ppt2.col&&ppt1.row != ppt2.row) {
		int max_row = ppt1.row > ppt2.row ? ppt1.row : ppt2.row;
		int min_row = ppt1.row < ppt2.row ? ppt1.row : ppt2.row;
		while (min_row < max_row - 1) {
			PlateIVec2 platePoint = PlateIVec2(min_row + 1, ppt2.col);
			if (getStonePointerRefByPlatePoint(platePoint))
				++countRet;

			++min_row;
		}
		return countRet;
	}
	if (ppt1.col != ppt2.col&&ppt1.row == ppt2.row) {
		int max_col = ppt1.col > ppt2.col ? ppt1.col : ppt2.col;
		int min_col = ppt1.col < ppt2.col ? ppt1.col : ppt2.col;
		while (min_col < max_col - 1) {
			PlateIVec2 p(ppt1.row, 1 + min_col);
			if (getStonePointerRefByPlatePoint(p))
				++countRet;
			++min_col;
		}
		return countRet;
	}
	return -1;
}
void MainScene::onEnter() {
	Scene::onEnter();

	_ai = new AI(this, _level);
	scheduleUpdate();
}
void MainScene::onExit() {
	unscheduleUpdate();

	delete _ai;
	Scene::onExit();
}
void MainScene::onClickThisPoint(const PlateIVec2&clinkedPlatePoint) {

	if (_isComputerCanCalculate)
		return;

	if (_isMoving)
		return;

	if (_ai->_aiState == AI::calcing)
		return;

	if (_stoneSelected) //֮ǰѡ��������
		onClickWithSelected(clinkedPlatePoint);
	else 				//֮ǰû��ѡ������
		onClickWithNoSelected(clinkedPlatePoint);

}
void MainScene::onClickWithNoSelected(const PlateIVec2& clinkedPlatePoint) {
	//ȡ�����λ�õ�����
	Stone* clinkedStone = getStonePointerRefByPlatePoint(clinkedPlatePoint);

	if (!clinkedStone)
		return;//����Ҳûѡ�С�

	if (clinkedStone->isRed() != _isRedOnBelow)
		return;//��ǰû���ֵ��ⷽ�ߣ�����ѡ

	//��ͼȥѡ��
	if (clinkedStone->isRed() == _isRedFirstCurrent) {
		_stoneSelectedEffect->setVisible(true);
		_stoneSelectedEffect->setPosition(clinkedStone->getPosition());
		_stoneSelected = clinkedStone;
	}
}
void MainScene::onClickWithSelected(const PlateIVec2& clinkedPlatePoint) {
	//ȡ�����λ�õ�����
	Stone* clinkedStone = getStonePointerRefByPlatePoint(clinkedPlatePoint);

	//��ͼȥ����
	Step step(_stoneSelected, _stoneSelected->getPlatePoint(), clinkedStone, clinkedPlatePoint);

	CanMoveResult res = canMove(step);
	switch (res)
	{
	case MainScene::TARGETISFRIEND:
		//��ѡ
		_stoneSelected = nullptr;
		onClickWithNoSelected(clinkedPlatePoint);
		break;
	case MainScene::CanMOVETOK:
	case MainScene::CanKILLOK:
		moveStep(step);

	case MainScene::CanMOVEERR:
	case MainScene::CanKILLERR:
		_stoneSelected = nullptr;
		_stoneSelectedEffect->setVisible(false);
		break;
	default:
		break;
	}
}

void MainScene::moveStep(const Step& step) {
	moveStepBegin(step);
}

MainScene::CanMoveResult MainScene::canMove(const Step& step) {
	if (step._targetStone)//Ŀ��λ�������ӡ�����ɱ��
	{
		if (step._srcStone->isRed() == step._targetStone->isRed())//�������ѷ�����һ������	
			return CanMoveResult::TARGETISFRIEND;
		else //�����˵з�����
			if (canMoveByRule(step))
				return CanMoveResult::CanKILLOK;//����ɱ��			
			else
				return CanMoveResult::CanKILLERR;//ɱ��ʧ��
	}
	else
		//Ŀ��Ϊ�գ���������
		if (canMoveByRule(step))
			return CanMoveResult::CanMOVETOK;
		else
			return CanMoveResult::CanKILLERR;



}

void MainScene::moveStepBegin(const Step& step) {
	_stoneSelectedEffect->setPosition(step._srcPlatePoint.toScreenPoint());
	_stoneSelectedEffect->setVisible(true);

	if (step._targetStone)
		step._targetStone->isLive(false);

	step._srcStone->setPlatePoint(step._targetPlatePoint);

	getStonePointerRefByPlatePoint(step._srcPlatePoint) = nullptr;
	getStonePointerRefByPlatePoint(step._targetPlatePoint) = step._srcStone;


	//�ֵ��Է�����
	_isRedFirstCurrent = !_isRedFirstCurrent;
	//��ǰ�ⲽ�����ߵģ���ʱ��ִ�������ƶ�������ʱ�򣬾Ϳ�����Aiȥ�����ˡ�
	if (_isRedFirstCurrent != _isRedOnBelow) {

		//������Ai�߳�ȥ����Step��
		_isComputerCanCalculateMutex.lock();
		_isComputerCanCalculate = true;
		_isComputerCanCalculateMutex.unlock();
	}


	//���������ƶ�����
	MoveTo* moveTo = MoveTo::create(1.0f, step._targetPlatePoint.toScreenPoint());

	CallFunc* moveFinshed = CallFunc::create(std::bind(
		&MainScene::moveStepFinshed, this, step));
	Sequence* seq = Sequence::create(moveTo, moveFinshed, nullptr);
	_isMoving = true;
	step._srcStone->runAction(seq);

}
void MainScene::moveStepFinshed(const Step& step) {
	_isMoving = false;


	if (step._targetStone) {
		step._targetStone->setVisible(false);
		if (step._targetStone->getType() == Stone::BOSS) {
			if (step._srcStone->isRed() == _isRedOnBelow) {
				//���ʤ��
				//��������Ӯ�ˡ�,���ų�����Ч��
				playerWin();
			}
			else {
				//�Է�ʤ��
				//�����������ˡ�
				Sprite* youLose = Sprite::create("shuijiemian.png");
				youLose->setPosition(center);
				addChild(youLose);
			}

			//�ر����̴���
			_eventDispatcher->removeEventListener(_listener);
		}
	}

	steps.push(step);
	_stoneSelected = nullptr;
}

void MainScene::playerWin() {
	Sprite* youWin = Sprite::create("yingjiemian.png");
	youWin->setLocalZOrder(99);

	youWin->setPosition(center);
	addChild(youWin);

	ParticleFlower* flower1 = ParticleFlower::create();
	ParticleFlower* flower2 = ParticleFlower::create();
	ParticleFireworks* fire2 = ParticleFireworks::create();
	ParticleFireworks* fire1 = ParticleFireworks::create();

	flower1->setTexture(Director::getInstance()->getTextureCache()->addImage("stars.png"));
	fire1->setTexture(Director::getInstance()->getTextureCache()->addImage("stars.png"));
	flower2->setTexture(Director::getInstance()->getTextureCache()->addImage("stars.png"));
	fire2->setTexture(Director::getInstance()->getTextureCache()->addImage("stars.png"));

	flower1->setPosition(flower1->getPosition() + Vec2(-200, 180));
	fire1->setPosition(fire1->getPosition() + Vec2(-200, 180));
	flower2->setPosition(flower2->getPosition() + Vec2(200, -180));
	fire2->setPosition(fire2->getPosition() + Vec2(200, -180));
	addChild(flower1, 100);
	addChild(fire1, 100);
	addChild(flower2, 100);
	addChild(fire2, 100);

	MoveBy* by1 = MoveBy::create(3, Vec2(winSize.width / 2, 0));
	MoveBy* by2 = MoveBy::create(3, Vec2(winSize.width / 2, 0));
	MoveBy* by3 = MoveBy::create(3, Vec2(-winSize.width / 2, 0));
	MoveBy* by4 = MoveBy::create(3, Vec2(-winSize.width / 2, 0));
	flower1->runAction(RepeatForever::create(Sequence::create(by1, by1->reverse(), nullptr)));
	fire1->runAction(RepeatForever::create(Sequence::create(by2, by2->reverse(), nullptr)));
	flower2->runAction(RepeatForever::create(Sequence::create(by3, by3->reverse(), nullptr)));
	fire2->runAction(RepeatForever::create(Sequence::create(by4, by4->reverse(), nullptr)));

}

bool MainScene::canMoveByRule(const Step& step) {

	switch (step._srcStone->getType())
	{
	case Stone::VEHICLE:
		return canMoveVehicle(step);
	case Stone::HORSE:
		return canMoveHorse(step);
	case Stone::ELEPHANT:
		return canMoveElephant(step);
	case Stone::SCHOLAR:
		return canMoveScholar(step);
	case Stone::BOSS:
		return canMoveBoss(step);
	case Stone::GUN:
		return canMoveGun(step);
	case Stone::SOLDIER:
		return canMoveSoldier(step);
	}

	return false;
}
bool MainScene::canMoveVehicle(const Step& step) {

	const PlateIVec2& p = step._srcStone->getPlatePoint();
	const PlateIVec2& target = step._targetPlatePoint;
	return (getStoneCountInTwoPlatePoint(p, target) == 0);

}
bool MainScene::canMoveHorse(const Step& step) {
	Stone* stone = step._srcStone;
	const PlateIVec2& target = step._targetPlatePoint;
	const PlateIVec2& srcPt = stone->getPlatePoint();
	int dRow = abs(srcPt.row - target.row);
	int dCol = abs(srcPt.col - target.col);

	int flag = dRow * 10 + dCol;
	
	//�����յ�ǰ������
	if (flag != 12 && flag != 21)
		return false;//��Ϊ������


	//���ŵ�
	PlateIVec2 bpt;

	//�����ж��Ƿ������ŵ�
	if (flag == 12)
		bpt = PlateIVec2(srcPt.row, (srcPt.col + target.col) / 2);
	else
		bpt = PlateIVec2((srcPt.row + target.row) / 2, srcPt.col);


	if (!getStonePointerRefByPlatePoint(bpt))
		return true;
	return false;
}
bool MainScene::canMoveElephant(const Step& step) {
	Stone* stone = step._srcStone;
	PlateIVec2 target = step._targetPlatePoint;
	PlateIVec2 srcPt = stone->getPlatePoint();
	int dRow = abs(srcPt.row - target.row);
	int dCol = abs(srcPt.col - target.col);
	int flag = dRow * 10 + dCol;
	//ֻ��������
	if (flag != 22)
		return false;
	

	//�ж�����
	Stone* between = getStonePointerRefByPlatePoint(PlateIVec2((srcPt.row + target.row) / 2, (srcPt.col + target.col) / 2));
	
	if (between)
		return false;


	//�ж��Ƿ����
	if (stone->isRed() == _isRedOnBelow) {
		if (target.row <= 4)
			return true;

	}
	else {
		if (target.row >= 5)
			return true;

	}
	return false;
}
bool MainScene::canMoveScholar(const Step& step) {
	Stone* stone = step._srcStone;
	PlateIVec2 target = step._targetPlatePoint;
	PlateIVec2 srcPt = stone->getPlatePoint();
	int dRow = abs(srcPt.row - target.row);
	int dCol = abs(srcPt.col - target.col);
	int flag = dRow * 10 + dCol;
	//ֻ���߿���
	if (flag != 11) {
		return false;
	}

	//���ܳ��Ź���
	if (stone->isRed() == _isRedOnBelow) {
		if (target.col <= 5 && target.col >= 3 && target.row <= 2) {
			return true;
		}

	}
	else {

		if (target.row >= 7 && target.col <= 5 && target.col >= 3)
			return true;
	}

	return false;
}
bool MainScene::canMoveBoss(const Step& step) {

	Stone* srcStone = step._srcStone;
	PlateIVec2 target = step._targetPlatePoint;
	//�Ͻ�������Էɹ�ȥɱ
	Stone* tatgetStone = getStonePointerRefByPlatePoint(target);

	if (tatgetStone&&tatgetStone->getType() == Stone::BOSS) {
		return canMoveVehicle(step);
	}


	PlateIVec2 srcPt = srcStone->getPlatePoint();
	int dRow = abs(srcPt.row - target.row);
	int dCol = abs(srcPt.col - target.col);
	int flag = dRow * 10 + dCol;
	//ֻ����һ��
	if (flag != 10 && flag != 1) {
		return false;
	}

	//�����߾Ź���
	if (srcStone->isRed() == _isRedOnBelow) {
		if (flag == 10) {
			//������
			if (target.row <= 2)
				return true;
		}
		else
		{
			//������
			if (target.col <= 5 && target.col >= 3)
				return true;
		}

	}
	else {
		if (flag == 10) {
			//������
			if (target.row >= 7)
				return true;
		}
		else
		{
			//������
			if (target.col <= 5 && target.col >= 3)
				return true;
		}

	}


	return false;
}
bool MainScene::canMoveGun(const Step& step) {
	Stone* stone = step._srcStone;
	PlateIVec2 target = step._targetPlatePoint;

	if (step._targetStone) {
		return getStoneCountInTwoPlatePoint(stone->getPlatePoint(), target) == 1;
	}
	else {
		return getStoneCountInTwoPlatePoint(stone->getPlatePoint(), target) == 0;
	}
}
bool MainScene::canMoveSoldier(const Step& step) {
	Stone* stone = step._srcStone;
	PlateIVec2 target = step._targetPlatePoint;


	PlateIVec2 srcPt = stone->getPlatePoint();
	int dRow = abs(srcPt.row - target.row);
	int dCol = abs(srcPt.col - target.col);
	int flag = dRow * 10 + dCol;
	if (flag != 10 && flag != 1)
		return false;
	

	if (stone->isRed() == this->_isRedOnBelow) {
		if (flag == 10) {
			//�����ߣ�����ֻ����ǰ��
			if (srcPt.row < target.row) {
				return true;
			}
		}
		else {
			//�����˲���������
			if (srcPt.row >= 5) {
				return true;
			}
		}
	}
	else {

		if (flag == 10) {
			//������
			if (srcPt.row > target.row) {
				return true;
			}
		}
		else {
			//������
			if (srcPt.row < 5) {
				return true;
			}
		}
	}

	return false;
}


void MainScene::regretItemCallback(Ref*) {

	if (_isMoving)
		return;

	if (_ai->_aiState == AI::calcing)
		return;	//�������ڼ��㣬���ܻ���

	if (_isRedOnBelow != _isRedFirstCurrent)
		return;


	//Ӧ��ͬʱ�ָ������ߵĺ�����ߵģ��ָ�����
	for (int i = 0; i < 2; ++i) {
		if (steps.empty())
			return;

		//ȡ�����һ��
		const Step &lastStep = steps.top();
		steps.pop();

		//����ָ�����
		if (lastStep._targetStone) {
			//���ӻָ�
			lastStep._targetStone->setVisible(true);
			lastStep._targetStone->isLive(true);
		}

		lastStep._srcStone->setPlatePoint(lastStep._srcPlatePoint);
		this->getStonePointerRefByPlatePoint(lastStep._srcPlatePoint) = lastStep._srcStone;
		this->getStonePointerRefByPlatePoint(lastStep._targetPlatePoint) = lastStep._targetStone;

		lastStep._srcStone->setPosition(lastStep._srcPlatePoint.toScreenPoint());

		_stoneSelected = lastStep._srcStone;
		_stoneSelectedEffect->setPosition(_stoneSelected->getPosition());
		_stoneSelectedEffect->setVisible(true);
	}

}
void MainScene::update(float delta) {

	//ÿһ֡��ȥ���Ai�߳��Ƿ������µĽ��
	_ai->_aiStateMutex.lock();
	if (_ai->_aiState == AI::AIState::newRes) {

		_progress->setPercentage(0);
		_progressText->setString("");
		//����ȥ����
		if (!_isMoving) {
			_ai->_aiState = AI::AIState::idle;
			moveStep(_ai->_moveStep);
		}
	}
	else if (_ai->_aiState == AI::AIState::calcing) {
		//��������İٷֱȣ����½�������
		_ai->_progressMutex.lock();
		_progress->setPercentage(_ai->getProgress());
		std::stringstream ss;
		ss << _ai->getProgress();
		_progressText->setString(ss.str());
		_ai->_progressMutex.unlock();
	}
	else {
		_progress->setPercentage(0);
		_progressText->setString("");
	}
	_ai->_aiStateMutex.unlock();
}
