#include "Game.h"

Game::Game(sf::RenderWindow* _window, MapManager* _mapManager) :
	window{ _window }, mapManager{ _mapManager }, isKeyPressed{false},
	screenMidlePos{ (int)(SCREEN_W / 2), (int)(SCREEN_H / 2) }
{
	init();
	subscribeEvent();
	menu->initStartMenu();
}

void Game::init()
{
	itemManager = new ItemManager();
	uiManager = new UIManager(window);
	dialogSys = new Dialog(window, uiManager, itemManager);
	spManager = new SpriteManager(mapManager->getNowMap(), uiManager, itemManager);
	player = spManager->getPlayer();
	menu = new Menu(window, uiManager, player);
	invent = new Inventory(window, player, uiManager);
	renderer = new Renderer(window, player);
	initPlayer();

	auto& data = Data::getInstance();
	for (auto b : data.getInvent()) 
	{ 
		player->takeItem(itemManager->getItemble(b.first), b.second);
	}

	auto update = [=](float deltaTime) {
		getInput(deltaTime);
		spManager->update(deltaTime);
		};

	auto draw = [=]() {
		renderer->Draw3DView(mapManager->getNowMap(), spManager->getDeteachSprite());
		uiManager->drawPlayerUI(player);
		};

	playState = RenderState(std::move(update), std::move(draw));

}

void Game::subscribeEvent()
{
	auto& event = EventSystem::getInstance();
	event.subscribe<int>("SWAPLOC", [=](const int levelN) {
		auto& state = GameState::getInstance();
		state.data.changerCoef = Random::intRandom(2, 5);
		sf::Vector2f pos = mapManager->nextLocation(levelN);
		spManager->resetMap(mapManager->getNowMap(), pos);
		});

	event.subscribe<int>("RESET_GAME", [=](const int NON) {
		resetGame();
		auto& state = GameState::getInstance();
		state.data.isFirstGame = false;
		currentState = &playState;
		});

	event.subscribe<int>("WIN_GAME", [=](const int NON) {
		resetGame();
		auto& state = GameState::getInstance();
		state.data.isFirstGame = true;
		menu->initStartMenu();
		});

	event.subscribe<RenderState*>("SWAP_STATE", [=](RenderState* state) {
		uiManager->deleteNow();
		currentState = state ? state : &playState;
		});

	event.subscribe<int>("PLAYERDEAD", [=](const int NON) {
		spManager->resetOldPlayer();
		auto& event = EventSystem::getInstance();
		event.trigger<int>("SWAPLOC", BASE_N);
		auto& state = GameState::getInstance();
		state.data.levelNumber--;
		menu->initResetMenu();
		});
}

Game::~Game()
{
	delete dialogSys;
	delete spManager;
	delete itemManager;
	delete uiManager;
	delete invent;
	delete menu;
}

void Game::resetGame()
{
	player->guns[1] = nullptr;
	player->guns[2] = nullptr;
	invent->takeItem(itemManager->getGunByIndex(2), 1);
	player->setGun(itemManager->getGunByIndex(2), 1);
	auto& questM = QuestManager::getInstance();
	questM.deleteAllQuest();
	auto& state = GameState::getInstance();
	state.data.killFirst = false;
	state.data.killSecond = false;
	state.data.killTherd = false;
}

void Game::initPlayer()
{
	player->setInventory(invent);
	player->kick = itemManager->getGunByIndex(0);
	player->setGun(itemManager->getGunByIndex(1), 0);
	player->nowHeal = invent->takeMaxHeal();

	auto& state = GameState::getInstance();
	player->mouseSpeed = state.data.mouseSpeed;

	auto& data = Data::getInstance();
	PlayerDef plDef = data.getPlayerData();

	int i = 1;
	for (auto it : plDef.gunsData)
	{
		player->setGun(itemManager->getGunById(it), i++);
	}

	dialogSys->setPlayer(player);
}

void Game::editor()
{
	sf::Vector2f plPos{};
	spManager->resetMap(mapManager->getNowMap(), plPos);
}

void Game::save()
{
	mapManager->rewriteSprites(spManager->getDeteachSprite());
	auto& event = EventSystem::getInstance();
	event.trigger<int>("SAVE", 0);
}

void Game::getInput(const sf::Event& event, float deltaTime)
{
	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Q)
		{
			invent->useInvent();
		}

		if (event.key.code == sf::Keyboard::H)
		{
			player->heal();
		}
#ifdef REDACT_MODE
		if (event.key.code == sf::Keyboard::P)
		{
			auto& event = EventSystem::getInstance();
			event.trigger<int>("SWAPLOC", BASE_N);
		}
#endif // REDACT_MODE
		if (event.key.code == sf::Keyboard::Escape)
		{
			menu->initGameMenu();
		}
	}

	if (event.type == sf::Event::MouseWheelScrolled)
	{
		player->swapGun(event.mouseWheelScroll.delta > 0);
	}
}

void Game::getInput(float deltaTime)
{
	if (!window->hasFocus()) return;

	float radAng = player->enemy->spMap.angle * PI / 180.0f;
	sf::Vector2f vertParams(cos(radAng), sin(radAng));
	sf::Vector2f horParams(-vertParams.y, vertParams.x);
	sf::Vector2f deltaPos(0, 0);
	bool lShiftPressed = false;

	if (GetAsyncKeyState('A')) { deltaPos -= horParams; }
	else if (GetAsyncKeyState('D')) { deltaPos += horParams; }
	if (GetAsyncKeyState('W')) { deltaPos += vertParams; }
	else if (GetAsyncKeyState('S')) { deltaPos -= vertParams; }
	if (GetAsyncKeyState('R')) { player->reloadingGun(); }
	if (GetAsyncKeyState(VK_LSHIFT)) { lShiftPressed = true; }
	if (GetAsyncKeyState(VK_SPACE)) { player->jump(); }
	if (GetAsyncKeyState('F')) { player->fire(); }
	if (GetAsyncKeyState('E'))
	{
		if (Sprite* sp = player->dialog(); sp)
		{
			if (auto npc = spManager->getNpc(sp->id); npc)
			{
				dialogSys->start(npc);
			}
		}
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		player->fire(0);
	}

	sf::Vector2f mousePos = ((sf::Vector2f)sf::Mouse::getPosition(*window) - (sf::Vector2f)screenMidlePos) / 2.0f;
	
	player->checkBoost(lShiftPressed, deltaTime);
	player->move(deltaPos, deltaTime);
	player->updateMouseData( mousePos, deltaTime);
	sf::Mouse::setPosition(screenMidlePos, *window);
}

void Game::makeCycle(float deltaTime)
{
	currentState->update(deltaTime);
	currentState->draw();
	SoundManager::update();
}
