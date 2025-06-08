#ifndef DATA
#define DATA

#include <fstream>
#include <string>
#include <tuple>
#include <codecvt>
#include <locale>
#include "player.h"
#include "quest.h"
#include "const.h"

class Data {
public:
	Data(const Data&) = delete;
	Data& operator=(const Data&) = delete;

	static Data& GetInstance() {
		static Data instance;
		return instance;
	}

	GameStateData GetGameState();
	void SaveGameState(GameStateData& data);

	PlayerDef GetPlayerData();
	void SavePlayerData(Player* player);

	std::vector<GunData> GetGunData();
	void SaveGunData(std::vector<GunData>& guns);

	std::vector<std::pair<int, int>> GetInvent();
	void SaveInvent(std::vector<std::pair<int, int>>& inv);

	std::vector<QuestData> GetQuest();
	void SaveQuest(std::vector<QuestData>& quests);

	std::vector<int> GetKeys(int startKey, int key);
	std::pair<std::wstring, int> GetText(int startKey, int key);
private:
	Data() = default;
	~Data() = default;

	std::vector<std::pair<int, std::vector<int>>> LoadKeyData(int startKey);
	std::vector<std::tuple<int, std::wstring, int>> LoadTextData(int startKey);
};

class GameState {
public:
	GameState(const GameState&) = delete;
	GameState& operator=(const GameState&) = delete;

	static GameState& GetInstance() {
		static GameState instanceGame;
		return instanceGame;
	}

	GameStateData data;
private:
	GameState();
	~GameState();
};

#endif // !DATA