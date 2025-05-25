#pragma once
#ifndef DATA
#define DATA

#include <fstream>
#include <string>
#include <tuple>
#include <codecvt>
#include <locale>
#include "Player.h"
#include "Quest.h"
#include "CONST.h"

class Data
{
public:
	Data(const Data&) = delete;
	Data& operator=(const Data&) = delete;

	static Data& getInstance()
	{
		static Data instance;
		return instance;
	}

	GameStateData getGameState();
	void saveGameState(GameStateData& data);

	PlayerDef getPlayerData();
	void savePlayerData(Player* player);

	std::vector<GunData> getGunData();
	void saveGunData(std::vector<GunData>& guns);

	std::vector<std::pair<int, int>> getInvent();
	void saveInvent(std::vector<std::pair<int, int>>& inv);

	std::vector<QuestData> getQuest();
	void saveQuest(std::vector<QuestData>& quests);

	std::vector<int> getKeys(int startKey, int key);
	std::pair<std::wstring, int> getText(int startKey, int key);
private:
	Data() = default;
	~Data() = default;

	std::vector<std::pair<int, std::vector<int>>> loadKeyData(int startKey);
	std::vector<std::tuple<int, std::wstring, int>> loadTextData(int startKey);
};

class GameState
{
public:
	GameState(const GameState&) = delete;
	GameState& operator=(const GameState&) = delete;

	static GameState& getInstance()
	{
		static GameState instanceGame;
		return instanceGame;
	}

	GameStateData data;
private:
	GameState();
	~GameState();
};

#endif // !DATA