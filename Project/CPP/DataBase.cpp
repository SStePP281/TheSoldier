#include "data_base.h"
#pragma warning(disable : 4996)

std::vector<std::pair<int, std::vector<int>>> Data::LoadKeyData(int idKey) {
	std::wifstream file_in("Dialog/dialogKey" + std::to_string(idKey) + ".txt");
	file_in.imbue(std::locale(file_in.getloc(), new std::codecvt_utf8<wchar_t>));
	if (!file_in.is_open()) { return {}; }

	std::vector<std::pair<int, std::vector<int>>> key_2_key;

	std::wstring line;

	while (std::getline(file_in, line)) {
		size_t st = line.find(L'[');
		size_t end = line.find(L']');
		int fkey = std::stoi(line.substr(st + 1, end - st - 1));
		line = line.substr(end + 3, line.size() - end - 4);
		std::vector<int> keys;
		while (line.size() != 0) {
			size_t j = line.find(L',');
			keys.push_back(std::stoi(line.substr(0, j)));
			line = line.substr(j + 1);
		}
		key_2_key.push_back({ fkey, keys });
	}

	file_in.close();

	return key_2_key;
}

std::vector<std::tuple<int, std::wstring, int>> Data::LoadTextData(int idKey) {
	std::wifstream file_in("Dialog/dialogText" + std::to_string(idKey) + ".txt");
	file_in.imbue(std::locale(file_in.getloc(), new std::codecvt_utf8<wchar_t>));
	if (!file_in.is_open()) { return {}; }

	std::vector<std::tuple<int, std::wstring, int>> key_2_text;

	std::wstring line, text;
	while (std::getline(file_in, line)) {
		size_t st = line.find(L'[');
		size_t end = line.find(L']');
		int fkey = std::stoi(line.substr(st + 1, end - st - 1));
		st = line.rfind(L'[');

		text = line.substr(end + 2, st - end - 3);
		end = line.rfind(L']');
		int eKey = std::stoi(line.substr(st + 1, end - st - 1));
		key_2_text.push_back({ fkey, text, eKey });
	}

	file_in.close();

	return key_2_text;
}

template<typename T>
bool loadBinary(std::string file_name, T& data) {
	std::ifstream in(file_name, std::ios::binary);
	if (!in.is_open()) { return false; }

	in.read(reinterpret_cast<char*>(&data), sizeof(T));

	in.close();
	return true;
}

template<typename T>
bool loadBinary(std::string file_name, std::vector<T>& data) {
	std::ifstream in(file_name, std::ios::binary);
	if (!in.is_open()) { return false; }

	size_t size;
	in.read(reinterpret_cast<char*>(&size), sizeof(size));
	data.resize(size);

	for (size_t i = 0; i < size; i++) {
		in.read(reinterpret_cast<char*>(&data[i]), sizeof(T));
	}

	in.close();

	return true;
}

template<typename T>
bool saveBinary(std::string file_name, T& data) {
	std::ofstream out(file_name, std::ios::binary);
	if (!out.is_open()) { return false; }

	out.write(reinterpret_cast<char*>(&data), sizeof(T));

	out.close();

	return true;
}

template<typename T>
bool saveBinary(std::string file_name, std::vector<T>& data) {
	std::ofstream out(file_name, std::ios::binary);
	if (!out.is_open()) { return false; }

	size_t size = data.size();
	out.write(reinterpret_cast<char*>(&size), sizeof(size));

	for (size_t i = 0; i < size; i++) {
		out.write(reinterpret_cast<char*>(&data[i]), sizeof(T));
	}

	out.close();

	return true;
}

std::vector<std::pair<int, int>> Data::GetInvent() {
	std::vector<std::pair<int, int>> inv;
	loadBinary("Data/inventory.inv", inv);
	return inv;
}

void Data::SaveInvent(std::vector<std::pair<int, int>>& inv) {
	saveBinary("Data/inventory.inv", inv);
}

std::vector<QuestData> Data::GetQuest() {
	std::vector<QuestData> quest;
	loadBinary("Data/Quest.qst", quest);
	return quest;
}

void Data::SaveQuest(std::vector<QuestData>& quests) {
	saveBinary("Data/Quest.qst", quests);
}

std::vector<int> Data::GetKeys(int idKey, int key) {
	auto key_2_key = LoadKeyData(idKey);

	for (size_t i = 0; i < key_2_key.size(); i++) {
		if (key == key_2_key[i].first) {
			return key_2_key[i].second;
		}
	}

	return {};
}

std::pair<std::wstring, int> Data::GetText(int id_Key, int key) {
	auto key_2_text = LoadTextData(id_Key);

	for (size_t i = 0; i < key_2_text.size(); i++) {
		if (key == std::get<0>(key_2_text[i])) {
			return { std::get<1>(key_2_text[i]) ,std::get<2>(key_2_text[i]) };
		}
	}

	return {};
}

GameStateData Data::GetGameState() {
	GameStateData data;
	loadBinary("Data/gameState.state", data);
	return data;
}

void Data::SaveGameState(GameStateData& data) {
	saveBinary("Data/gameState.state", data);
}

PlayerDef Data::GetPlayerData() {
	std::ifstream in{ "Data/playerData.plr", std::ios::in | std::ios::binary };
	if (!in.is_open()) { return { 100, 100, 2, 2, 10, 50, 50, 500, 10000, 0, {} }; }

	PlayerDef player_def;
	in.read(reinterpret_cast<char*>(&player_def.max_healpoint), sizeof(player_def.max_healpoint));
	in.read(reinterpret_cast<char*>(&player_def.now_healpoint), sizeof(player_def.now_healpoint));
	in.read(reinterpret_cast<char*>(&player_def.max_energy), sizeof(player_def.max_energy));
	in.read(reinterpret_cast<char*>(&player_def.now_energy), sizeof(player_def.now_energy));
	in.read(reinterpret_cast<char*>(&player_def.defence), sizeof(player_def.defence));
	in.read(reinterpret_cast<char*>(&player_def.max_strenght), sizeof(player_def.max_strenght));
	in.read(reinterpret_cast<char*>(&player_def.now_strenght), sizeof(player_def.now_strenght));
	in.read(reinterpret_cast<char*>(&player_def.patrons_count), sizeof(player_def.patrons_count));
	in.read(reinterpret_cast<char*>(&player_def.money), sizeof(player_def.money));
	in.read(reinterpret_cast<char*>(&player_def.details), sizeof(player_def.details));

	size_t size;
	in.read(reinterpret_cast<char*>(&size), sizeof(size));

	player_def.guns_data.reserve(size);
	for (size_t i = 0; i < size; i++) {
		int id;
		in.read(reinterpret_cast<char*>(&id), sizeof(id));
		player_def.guns_data.push_back(id);
	}

	in.close();

	return player_def;
}

void Data::SavePlayerData(Player* player) {
	std::ofstream out{ "Data/playerData.plr", std::ios::out | std::ios::binary };
	if (!out.is_open()) { return; }

	PlayerDef plDef = player->GetPlayerDef();

	out.write(reinterpret_cast<const char*>(&plDef.max_healpoint), sizeof(plDef.max_healpoint));
	out.write(reinterpret_cast<const char*>(&plDef.now_healpoint), sizeof(plDef.now_healpoint));
	out.write(reinterpret_cast<const char*>(&plDef.max_energy), sizeof(plDef.max_energy));
	out.write(reinterpret_cast<const char*>(&plDef.now_energy), sizeof(plDef.now_energy));
	out.write(reinterpret_cast<const char*>(&plDef.defence), sizeof(plDef.defence));
	out.write(reinterpret_cast<const char*>(&plDef.max_strenght), sizeof(plDef.max_strenght));
	out.write(reinterpret_cast<const char*>(&plDef.now_strenght), sizeof(plDef.now_strenght));
	out.write(reinterpret_cast<const char*>(&plDef.patrons_count), sizeof(plDef.patrons_count));
	out.write(reinterpret_cast<const char*>(&plDef.money), sizeof(plDef.money));
	out.write(reinterpret_cast<const char*>(&plDef.details), sizeof(plDef.details));

	size_t size = plDef.guns_data.size();
	out.write(reinterpret_cast<const char*>(&size), sizeof(size));
	for (const auto& it : plDef.guns_data) {
		out.write(reinterpret_cast<const char*>(&it), sizeof(it));
	}

	out.close();
}

std::vector<GunData> Data::GetGunData() {
	std::ifstream in{ "Data/gunData.gun", std::ios::in | std::ios::binary };
	if (!in.is_open()) { return{}; }

	size_t size;
	in.read(reinterpret_cast<char*>(&size), sizeof(size));
	std::vector<GunData> defs(size);

	for (size_t i = 0; i < size; i++) {
		in.read(reinterpret_cast<char*>(&defs[i].id), sizeof(defs[i].id));
		in.read(reinterpret_cast<char*>(&defs[i].now_count), sizeof(defs[i].now_count));
		in.read(reinterpret_cast<char*>(&defs[i].now_max_ount), sizeof(defs[i].now_max_ount));
		in.read(reinterpret_cast<char*>(&defs[i].now_max_rad), sizeof(defs[i].now_max_rad));
		in.read(reinterpret_cast<char*>(&defs[i].now_damage), sizeof(defs[i].now_damage));
		in.read(reinterpret_cast<char*>(&defs[i].upgrade_count), sizeof(defs[i].upgrade_count));

		size_t improve_size;
		in.read(reinterpret_cast<char*>(&improve_size), sizeof(improve_size));

		defs[i].improve_id.reserve(improve_size);
		for (size_t j = 0; j < improve_size; j++) {
			int id;
			in.read(reinterpret_cast<char*>(&id), sizeof(id));
			defs[i].improve_id.push_back(id);
		}
	}

	in.close();

	return defs;
}

void Data::SaveGunData(std::vector<GunData>& guns) {
	std::ofstream out{ "Data/gunData.gun", std::ios::out | std::ios::binary };
	if (!out.is_open()) { return; }

	size_t size = guns.size();
	out.write(reinterpret_cast<const char*>(&size), sizeof(size));

	for (const auto& def : guns) {
		out.write(reinterpret_cast<const char*>(&def.id), sizeof(def.id));
		out.write(reinterpret_cast<const char*>(&def.now_count), sizeof(def.now_count));
		out.write(reinterpret_cast<const char*>(&def.now_max_ount), sizeof(def.now_max_ount));
		out.write(reinterpret_cast<const char*>(&def.now_max_rad), sizeof(def.now_max_rad));
		out.write(reinterpret_cast<const char*>(&def.now_damage), sizeof(def.now_damage));
		out.write(reinterpret_cast<const char*>(&def.upgrade_count), sizeof(def.upgrade_count));

		size_t improve_size = def.improve_id.size();
		out.write(reinterpret_cast<const char*>(&improve_size), sizeof(improve_size));

		for (const auto& imp : def.improve_id) {
			out.write(reinterpret_cast<const char*>(&imp), sizeof(imp));
		}
	}

	out.close();
}

GameState::GameState() {
	auto& data_base = Data::GetInstance();
	data = data_base.GetGameState();
}

GameState::~GameState() {
	auto& data_base = Data::GetInstance();
	data_base.SaveGameState(data);
}