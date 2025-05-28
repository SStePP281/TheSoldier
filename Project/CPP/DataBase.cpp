#include "DataBase.h"
#pragma warning(disable : 4996)

std::vector<std::pair<int, std::vector<int>>> Data::loadKeyData(int idKey)
{
	std::wifstream fileIn("Dialog/dialogKey" + std::to_string(idKey) + ".txt");
	fileIn.imbue(std::locale(fileIn.getloc(), new std::codecvt_utf8<wchar_t>));
	if (!fileIn.is_open()) return {};

	std::vector<std::pair<int, std::vector<int>>> key2key;

	std::wstring line;

	while (std::getline(fileIn, line))
	{
		size_t st = line.find(L'[');
		size_t end = line.find(L']');
		int fkey = std::stoi(line.substr(st + 1, end - st - 1));
		line = line.substr(end + 3, line.size() - end - 4);
		std::vector<int> keys;
		while (line.size() != 0)
		{
			size_t j = line.find(L',');
			keys.push_back(std::stoi(line.substr(0, j)));
			line = line.substr(j + 1);
		}
		key2key.push_back({ fkey, keys });
	}

	fileIn.close();

	return key2key;
}

std::vector<std::tuple<int, std::wstring, int>> Data::loadTextData(int idKey)
{
	std::wifstream fileIn("Dialog/dialogText" + std::to_string(idKey) + ".txt");
	fileIn.imbue(std::locale(fileIn.getloc(), new std::codecvt_utf8<wchar_t>));
	if (!fileIn.is_open()) return {};

	std::vector<std::tuple<int, std::wstring, int>> key2text;

	std::wstring line, text;
	while (std::getline(fileIn, line))
	{
		size_t st = line.find(L'[');
		size_t end = line.find(L']');
		int fkey = std::stoi(line.substr(st + 1, end - st - 1));
		st = line.rfind(L'[');

		text = line.substr(end + 2, st - end - 3);
		end = line.rfind(L']');
		int eKey = std::stoi(line.substr(st + 1, end - st - 1));
		key2text.push_back({ fkey, text, eKey });
	}

	fileIn.close();

	return key2text;
}

template<typename T>
bool loadBinary(std::string filename, T& data)
{
	std::ifstream in(filename, std::ios::binary);
	if (!in.is_open()) return false;

	in.read(reinterpret_cast<char*>(&data), sizeof(T));

	in.close();
	return true;
}

template<typename T>
bool loadBinary(std::string filename, std::vector<T>& data)
{
	std::ifstream in(filename, std::ios::binary);
	if (!in.is_open()) return false;

	size_t size;
	in.read(reinterpret_cast<char*>(&size), sizeof(size));
	data.resize(size);

	for (size_t i = 0; i < size; i++)
	{
		in.read(reinterpret_cast<char*>(&data[i]), sizeof(T));
	}

	in.close();
	return true;
}

template<typename T>
bool saveBinary(std::string filename, T& data)
{
	std::ofstream out(filename, std::ios::binary);
	if (!out.is_open()) return false;

	out.write(reinterpret_cast<char*>(&data), sizeof(T));

	out.close();
	return true;
}

template<typename T>
bool saveBinary(std::string filename, std::vector<T>& data)
{
	std::ofstream out(filename, std::ios::binary);
	if (!out.is_open()) return false;

	size_t size = data.size();
	out.write(reinterpret_cast<char*>(&size), sizeof(size));

	for (size_t i = 0; i < size; i++)
	{
		out.write(reinterpret_cast<char*>(&data[i]), sizeof(T));
	}

	out.close();
	return true;
}

std::vector<std::pair<int, int>> Data::getInvent()
{
	std::vector<std::pair<int, int>> inv;
	loadBinary("Data/inventory.inv", inv);
	return inv;
}

void Data::saveInvent(std::vector<std::pair<int, int>>& inv)
{
	saveBinary("Data/inventory.inv", inv);
}

std::vector<QuestData> Data::getQuest()
{
	std::vector<QuestData> quest;
	loadBinary("Data/Quest.qst", quest);
	return quest;
}

void Data::saveQuest(std::vector<QuestData>& quests)
{
	saveBinary("Data/Quest.qst", quests);
}

std::vector<int> Data::getKeys(int idKey, int key)
{
	auto key2key = loadKeyData(idKey);

	for (size_t i = 0; i < key2key.size(); i++)
	{
		if (key == key2key[i].first) {
			return key2key[i].second;
		}
	}

	return {};
}

std::pair<std::wstring, int> Data::getText(int idKey, int key)
{
	auto key2text = loadTextData(idKey);

	for (size_t i = 0; i < key2text.size(); i++)
	{
		if (key == std::get<0>(key2text[i]))
		{
			return { std::get<1>(key2text[i]) ,std::get<2>(key2text[i]) };
		}
	}

	return {};
}

GameStateData Data::getGameState()
{
	GameStateData data;
	loadBinary("Data/gameState.state", data);
	return data;
}

void Data::saveGameState(GameStateData& data)
{
	saveBinary("Data/gameState.state", data);
}

PlayerDef Data::getPlayerData()
{
	std::ifstream in{ "Data/playerData.plr", std::ios::in | std::ios::binary };
	if (!in.is_open()) return { 100, 100, 2, 2, 10, 50, 50, 500, 10000, 0, {} };

	PlayerDef plDef{ /*180,180,7,7,0,0,0,500,1000,0, {}*/ };
	in.read(reinterpret_cast<char*>(&plDef.maxHp), sizeof(plDef.maxHp));
	in.read(reinterpret_cast<char*>(&plDef.nowHp), sizeof(plDef.nowHp));
	in.read(reinterpret_cast<char*>(&plDef.maxEnergy), sizeof(plDef.maxEnergy));
	in.read(reinterpret_cast<char*>(&plDef.nowEnergy), sizeof(plDef.nowEnergy));
	in.read(reinterpret_cast<char*>(&plDef.defence), sizeof(plDef.defence));
	in.read(reinterpret_cast<char*>(&plDef.maxStrenght), sizeof(plDef.maxStrenght));
	in.read(reinterpret_cast<char*>(&plDef.nowStrenght), sizeof(plDef.nowStrenght));
	in.read(reinterpret_cast<char*>(&plDef.countpantrons), sizeof(plDef.countpantrons));
	in.read(reinterpret_cast<char*>(&plDef.money), sizeof(plDef.money));
	in.read(reinterpret_cast<char*>(&plDef.details), sizeof(plDef.details));

	size_t size;
	in.read(reinterpret_cast<char*>(&size), sizeof(size));
	
	plDef.gunsData.reserve(size);
	for (size_t i = 0; i < size; i++)
	{
		int id;
		in.read(reinterpret_cast<char*>(&id), sizeof(id));
		plDef.gunsData.push_back(id);
	}

	in.close();

	return plDef;
}

void Data::savePlayerData(Player* player)
{
	std::ofstream out{ "Data/playerData.plr", std::ios::out | std::ios::binary };
	if (!out.is_open()) return;

	PlayerDef plDef = player->getPlayerDef();

	out.write(reinterpret_cast<const char*>(&plDef.maxHp), sizeof(plDef.maxHp));
	out.write(reinterpret_cast<const char*>(&plDef.nowHp), sizeof(plDef.nowHp));
	out.write(reinterpret_cast<const char*>(&plDef.maxEnergy), sizeof(plDef.maxEnergy));
	out.write(reinterpret_cast<const char*>(&plDef.nowEnergy), sizeof(plDef.nowEnergy));
	out.write(reinterpret_cast<const char*>(&plDef.defence), sizeof(plDef.defence));
	out.write(reinterpret_cast<const char*>(&plDef.maxStrenght), sizeof(plDef.maxStrenght));
	out.write(reinterpret_cast<const char*>(&plDef.nowStrenght), sizeof(plDef.nowStrenght));
	out.write(reinterpret_cast<const char*>(&plDef.countpantrons), sizeof(plDef.countpantrons));
	out.write(reinterpret_cast<const char*>(&plDef.money), sizeof(plDef.money));
	out.write(reinterpret_cast<const char*>(&plDef.details), sizeof(plDef.details));

	size_t size = plDef.gunsData.size();
	out.write(reinterpret_cast<const char*>(&size), sizeof(size));
	for (auto it : plDef.gunsData)
	{
		out.write(reinterpret_cast<const char*>(&it), sizeof(it));
	}

	out.close();
}

std::vector<GunData> Data::getGunData()
{
	std::ifstream in{ "Data/gunData.gun", std::ios::in | std::ios::binary };
	if (!in.is_open()) return{};

	size_t size;
	in.read(reinterpret_cast<char*>(&size), sizeof(size));
	std::vector<GunData> defs(size);

	for (size_t i = 0; i < size; i++)
	{
		in.read(reinterpret_cast<char*>(&defs[i].id), sizeof(defs[i].id));
		in.read(reinterpret_cast<char*>(&defs[i].nowCount), sizeof(defs[i].nowCount));
		in.read(reinterpret_cast<char*>(&defs[i].nowMaxCount), sizeof(defs[i].nowMaxCount));
		in.read(reinterpret_cast<char*>(&defs[i].nowMaxRad), sizeof(defs[i].nowMaxRad));
		in.read(reinterpret_cast<char*>(&defs[i].nowDamage), sizeof(defs[i].nowDamage));
		in.read(reinterpret_cast<char*>(&defs[i].upgradeCount), sizeof(defs[i].upgradeCount));

		size_t unSize;
		in.read(reinterpret_cast<char*>(&unSize), sizeof(unSize));

		defs[i].improveId.reserve(unSize);
		for (size_t j = 0; j < unSize; j++)
		{
			int id;
			in.read(reinterpret_cast<char*>(&id), sizeof(id));
			defs[i].improveId.push_back(id);
		}
	}

	in.close();

	return defs;
}

void Data::saveGunData(std::vector<GunData>& guns)
{
	std::ofstream out{ "Data/gunData.gun", std::ios::out | std::ios::binary };
	if (!out.is_open()) return;

	size_t size = guns.size();
	out.write(reinterpret_cast<const char*>(&size), sizeof(size));

	for (auto def : guns)
	{
		out.write(reinterpret_cast<char*>(&def.id), sizeof(def.id));
		out.write(reinterpret_cast<char*>(&def.nowCount), sizeof(def.nowCount));
		out.write(reinterpret_cast<char*>(&def.nowMaxCount), sizeof(def.nowMaxCount));
		out.write(reinterpret_cast<char*>(&def.nowMaxRad), sizeof(def.nowMaxRad));
		out.write(reinterpret_cast<char*>(&def.nowDamage), sizeof(def.nowDamage));
		out.write(reinterpret_cast<char*>(&def.upgradeCount), sizeof(def.upgradeCount));

		size_t unSize = def.improveId.size();
		out.write(reinterpret_cast<const char*>(&unSize), sizeof(unSize));

		for (auto im : def.improveId)
		{
			out.write(reinterpret_cast<const char*>(&im), sizeof(im));
		}
	}

	out.close();
}

GameState::GameState()
{
	auto& dataBase = Data::getInstance();
	data = dataBase.getGameState();
}

GameState::~GameState()
{
	auto& dataBase = Data::getInstance();
	dataBase.saveGameState(data);
}
