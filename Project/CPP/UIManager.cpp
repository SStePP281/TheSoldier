#include "UIManeger.h"
#include <sstream>
#include <string>
#include <iomanip>
#include <SFML/Graphics/CircleShape.hpp>

UIManager::UIManager(sf::RenderWindow* window) : 
	window{ window }, chose_but{ -1 }, key_button{ -1 } {}

std::wstring UIManager::SplitText(std::wstring text, int max_len, int text_size)
{
	std::wstring result, word, cur_line;
	std::wistringstream stream(text);

	while (stream >> word)
	{
		sf::Text temp_text;
		temp_text.setFont(Resources::ui_font);
		temp_text.setCharacterSize(text_size);
		std::wstring test_line = cur_line.empty() ? word : cur_line + L" " + word;
		temp_text.setString(test_line);

		if (temp_text.getLocalBounds().width > max_len)
		{
			result += cur_line + L"\n";
			cur_line = word;
		}
		else
		{
			cur_line = test_line;
		}
	}

	if (!cur_line.empty())
	{
		result += cur_line;
	}

	return result;
}

std::wstring UIManager::ToMax(std::wstring str, float max_wight, float text_size)
{
	sf::Text text(L"", Resources::ui_font, (int)text_size);
	std::wstring result = str;

	while (true)
	{
		text.setString(result);

		if (text.getLocalBounds().width > max_wight)
		{
			break;
		}

		result += L" ";
	}

	return result;
}

void UIManager::InitDialog(std::map<int, std::wstring, std::greater<int>>& variants,
	const std::wstring& npc_name)
{
	background = sf::Sprite(Resources::dialog_background);
	background.setScale({ (float)kScreenWight / Resources::dialog_background.getSize().x,
		(float)kScreenHeight / Resources::dialog_background.getSize().y });

	sf::RectangleShape name_base{ {kDialogWight, kTextSize + 10} };
	name_base.setFillColor(sf::Color(100, 100, 100));
	sf::Text name_text(npc_name, Resources::ui_font, kTextSize);
	Group group(name_base, name_text);
	group.SetPosition({ (float)kScreenWight / 2, kInterval});
	buttons.push_back(group);

	sf::RectangleShape data_base{ {kDialogWight, kDialogHeight / 1.5f} };
	data_base.setFillColor(sf::Color(100, 100, 100));
	sf::Text data_text(SplitText(variants[-1], (int)kDialogWight, 40), Resources::ui_font, kTextSize - 10);
	Group group_1(data_base, data_text);
	group_1.SetPosition({ (float)kScreenWight / 2, group.GetPosition().y + group_1.GetSize().y / 2 + kInterval});
	buttons.push_back(group_1);

	sf::Vector2f position((float)kScreenWight / 2, group_1.GetPosition().y + group_1.GetSize().y / 2 + kInterval);
	
	for (auto& var : variants)
	{
		if (var.first == -1) continue;
		sf::Text text(var.second, Resources::ui_font, kTextSize);
		sf::RectangleShape rect({ kDialogWight, kTextSize + 10 });
		rect.setFillColor(sf::Color(50, 50, 50));
		Group group_2(rect, text);
		group_2.SetPosition(position);
		
		buttons.push_back(Button(group_2));
		buttons.back().SetFunc([=]() { key_button = var.first;});

		position.y +=  kInterval + group_2.shape.getSize().y / 2;
	}
}

void UIManager::InitResetMenu()
{
	background = sf::Sprite(Resources::menu_background);
	background.setScale({ (float)kScreenWight / Resources::menu_background.getSize().x,
		(float)kScreenHeight / Resources::menu_background.getSize().y });

	sf::Text text(L"ПРОДОЛЖИТЬ ИГРУ", Resources::ui_font, 50);
	sf::RectangleShape shape({ text.getLocalBounds().width + 20.0f, 60.0f });
	shape.setFillColor(sf::Color(100, 100, 100));
	Group base_group(shape, text);
	Button button(base_group);
	button.SetPosition({ (float)kScreenWight / 2.0f, 2.0f * (float)kScreenHeight / 3.0f - 35.0f });
	button.SetFunc([=]() { key_button = 0;});
	buttons.push_back(button);

	button.SetString(L"ВЫХОД");
	button.SetPosition({ (float)kScreenWight / 2.0f, 2.0f * (float)kScreenHeight / 3.0f + 35.0f });
	button.SetFunc([=]() { key_button = 1;});
	buttons.push_back(button);
}

void UIManager::InitStartMenu()
{
	background = sf::Sprite(Resources::menu_background);
	background.setScale({ (float)kScreenWight / Resources::menu_background.getSize().x,
		(float)kScreenHeight / Resources::menu_background.getSize().y });
	
	sf::Text text(L"ПРОДОЛЖИТЬ ИГРУ", Resources::ui_font, 50);
	sf::RectangleShape shape( {text.getLocalBounds().width + 20.0f, 60.0f});
	shape.setFillColor(sf::Color(100, 100, 100));
	Group base_group(shape, text);
	Button button(base_group);
	button.SetPosition({ (float)kScreenWight / 2.0f, 2.0f * (float)kScreenHeight / 3.0f - 35.0f });
	button.SetFunc([=]() { key_button = 0;});
	auto& state = GameState::GetInstance();
	if (!state.data.isFirstGame) buttons.push_back(button);

	button.SetString(L"НОВАЯ ИГРА");
	button.SetPosition({ (float)kScreenWight / 2.0f, 2.0f * (float)kScreenHeight / 3.0f + 35.0f });
	button.SetFunc([=]() { key_button = 1;});
	buttons.push_back(button);

	button.SetString(L"ВЫХОД");
	button.SetPosition({ (float)kScreenWight / 2.0f, 2.0f * (float)kScreenHeight / 3.0f + 105.0f });
	button.SetFunc([=]() { key_button = 2;});
	buttons.push_back(button);
}

void UIManager::InitGameMenu()
{
	background = sf::Sprite(Resources::inventory_background);
	background.setScale({ (float)kScreenWight / Resources::inventory_background.getSize().x,
		(float)kScreenHeight / Resources::inventory_background.getSize().y });

	sf::Text text(L"ПРОДОЛЖИТЬ", Resources::ui_font, 50);
	sf::RectangleShape shape({ text.getLocalBounds().width + 20.0f, 60.0f });
	shape.setFillColor(sf::Color(100, 100, 100));
	Group base_group(shape, text);
	Button button(base_group);
	button.SetPosition({ (float)kScreenWight / 2.0f, 2.0f * (float)kScreenHeight / 3.0f - 35.0f });
	button.SetFunc([=]() { key_button = 0; });
	buttons.push_back(button);

	button.SetString(L"НАСТРОЙКИ");
	button.SetPosition({ (float)kScreenWight / 2.0f, 2.0f * (float)kScreenHeight / 3.0f + 35.0f });
	button.SetFunc([=]() { key_button = 1;});
	buttons.push_back(button);

	button.SetString(L"ВЫХОД");
	button.SetPosition({ (float)kScreenWight / 2.0f, 2.0f * (float)kScreenHeight / 3.0f + 105.0f });
	button.SetFunc([=]() { key_button = 2;});
	buttons.push_back(button);
}

void UIManager::InitSetting()
{
	background = sf::Sprite(Resources::inventory_background);
	background.setScale({ (float)kScreenWight / Resources::inventory_background.getSize().x,
		(float)kScreenHeight / Resources::inventory_background.getSize().y });

	sf::Text text(L"ВЫХОД", Resources::ui_font, 50);
	sf::RectangleShape shape({ text.getLocalBounds().width + 20.0f, 60.0f});
	shape.setFillColor(sf::Color(100, 100, 100));
	Group base_group(shape, text);
	Button button(base_group);
	button.SetPosition({ (float)kScreenWight / 2.0f, 80.0f });
	button.SetFunc([=]() { key_button = 0; });
	buttons.push_back(button);

	text.setString(L"+");
	sf::RectangleShape func_shape({ 60.0f, 60.0f });
	func_shape.setFillColor(sf::Color(50, 50, 50));
	base_group = Group(shape, text);
	Button func_button(base_group);
	
	auto& state = GameState::GetInstance();

	text.setString(L"ГРОМКОСТЬ ЕФФЕКТОВ: " + std::to_wstring(state.data.effectVolume));
	shape = sf::RectangleShape({ text.getLocalBounds().width + 20.0f, 60.0f });
	shape.setFillColor(sf::Color(100, 100, 100));
	base_group = Group(shape, text);
	button = Button(base_group);
	button.SetPosition({ (float)kScreenWight / 2.0f, 150.0f});
	buttons.push_back(button);
	func_button.SetPosition({ button.GetPosition().x + button.GetSize().x / 2 + 30.0f, button.GetPosition().y });
	func_button.SetFunc([&]() {state.data.effectVolume++;key_button = 1;});
	buttons.push_back(func_button);
	func_button.SetPosition({ button.GetPosition().x - button.GetSize().x / 2 - 30.0f, button.GetPosition().y });
	func_button.SetFunc([&]() {state.data.effectVolume--;key_button = 1;});
	func_button.SetString(L"-");
	buttons.push_back(func_button);

	button.SetString(L"ГРОМКОСТЬ МУЗЫКИ: " + std::to_wstring(state.data.soundVolume));
	button.SetPosition({ (float)kScreenWight / 2.0f, 220.0f});
	buttons.push_back(button);
	func_button.SetString(L"+");
	func_button.SetPosition({ button.GetPosition().x + button.GetSize().x / 2 + 30.0f, button.GetPosition().y });
	func_button.SetFunc([&]() {state.data.soundVolume++;key_button = 1;});
	buttons.push_back(func_button);
	func_button.SetPosition({ button.GetPosition().x - button.GetSize().x / 2 - 30.0f, button.GetPosition().y });
	func_button.SetFunc([&]() {state.data.soundVolume--; key_button = 1;});
	func_button.SetString(L"-");
	buttons.push_back(func_button);

	std::wstring str = std::to_wstring(state.data.mouseSpeed + 1.0f);
	button.SetString(L"CКОРОСТЬ МЫШИ: " + str.substr(0, str.find(L".") + 3));
	button.SetPosition({ (float)kScreenWight / 2.0f, 290.0f });
	buttons.push_back(button);
	func_button.SetString(L"+");
	func_button.SetPosition({ button.GetPosition().x + button.GetSize().x / 2 + 30.0f, button.GetPosition().y });
	func_button.SetFunc([&]() {state.data.mouseSpeed += 0.01f; key_button = 2;});
	buttons.push_back(func_button);
	func_button.SetPosition({ button.GetPosition().x - button.GetSize().x / 2 - 30.0f, button.GetPosition().y });
	func_button.SetFunc([&]() {state.data.mouseSpeed -= 0.01f; key_button = 2;});
	func_button.SetString(L"-");
	buttons.push_back(func_button);
	
	func_button.SetFunc(nullptr);
	shape = sf::RectangleShape({shape.getSize().x / 2, 30.0f});
	shape.setFillColor(sf::Color(100, 100, 100));
	text = sf::Text(L"", Resources::ui_font, 20);
	base_group = Group(shape, text);
	button = Button(base_group);
	func_shape.setSize({ 30.0f, 30.0f });
	Group func_group(func_shape, text);
	func_button = Button(func_group);
	sf::Vector2f pos{ (float)kScreenWight / 2.0f - shape.getSize().x / 2 - 15.0f, 350.0f };

	std::vector<std::pair<std::wstring, std::wstring>> keys = {
		{L"A", L"Move left"},
		{L"D", L"Move right"},
		{L"W", L"Move forward"}, 
		{L"Scroll", L"To swap gun"}, 
		{L"E", L"To interact"},
		{L"F", L"To break door"},
		{L"Q", L"To open inventory"},
		{L"H", L"To healing"},
		{L"Space", L"To jump"},
		{L"Shift", L"To run"},
		{L"LMB", L"To fire"},
		{L"ESC", L"To open menu"},
		};

	for (size_t i = 0; i < keys.size(); i++)
	{
		button.SetString(keys[i].second);
		func_button.SetString(keys[i].first);
		button.SetPosition(pos);
		func_button.SetPosition({ pos.x - button.GetSize().x / 2 - func_shape.getSize().x / 2, pos.y });
		buttons.push_back(button);
		buttons.push_back(func_button);

		if (i % 2 == 0)
		{
			pos.x += shape.getSize().x + 15.0f + func_button.GetSize().x; 
		}
		else
		{
			pos.x -= shape.getSize().x + 15.0f + func_button.GetSize().x;
			pos.y += 40.0f;
		}
	}
}

void UIManager::InitQuest(Quest* quest, Player* player)
{
	background = sf::Sprite(Resources::trade_background);
	background.setScale({ (float)kScreenWight / Resources::trade_background.getSize().x,
		(float)kScreenHeight / Resources::trade_background.getSize().y });

	sf::Text text(L"Баланс: " + std::to_wstring(player->money) + L" | Запчасти: " + std::to_wstring(player->details), Resources::ui_font, 50);
	sf::RectangleShape shape({ text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f});
	shape.setFillColor(sf::Color(70, 70, 70));
	shape.setPosition({ (float)kScreenWight / 2, shape.getSize().y / 2 + 10.0f });
	Group base_group(shape, text);
	Button button(base_group);
	buttons.push_back(button);

	text.setString(L"Взять новый квест");
	shape.setSize({text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f});
	base_group = Group(shape, text);
	Button new_quest(base_group);
	new_quest.SetFillColor(sf::Color(50, 50, 50));
	new_quest.SetFunc([=]() {key_button = -300; });

	text.setCharacterSize(30);
	shape.setSize({kDialogWight / 3, kDialogHeight / 3 - 20.0f});
	shape.setPosition({shape.getSize().x, shape.getSize().y / 2 + 90.0f});
	base_group = Group(shape, text);
	Button total_quest(base_group);
	total_quest.SetFillColor(sf::Color(50, 50, 50));

	auto& quest_manager = QuestManager::GetInstance();
	auto total = quest_manager.quests;
	std::wstringstream oss;
	for (size_t i = 0; i < total.size(); i++)
	{
		if (total[i]) // квест есть
		{
			if (total[i]->data.type == QuestType::KillMonster)
			{
				oss << L"Охотник на монстров";
				oss << L"\n";
				oss << L"Убей " << total[i]->data.target << L" монстров";
			}
			else if (total[i]->data.type == QuestType::CollectionMoney)
			{
				oss << L"Капиталист";
				oss << L"\n";
				oss << L"Заработай " << total[i]->data.target << L" монет";
			}
			else if (total[i]->data.type == QuestType::CollectionDetails)
			{
				oss << L"Сбор запчастей";
				oss << L"\n";
				oss << L"Собери " << total[i]->data.target << L" запчастей";
			}

			oss << L"\n";
			oss << L"Статус: " << (total[i]->IsCompleted() ? L"Завершен" : L"В процессе");
			oss << L"\n";
			oss << L"Награда: " << total[i]->data.rewardCoins << L" монет";
			oss << L"\n";
			oss << L"Прогресс: " << total[i]->data.progress << L" / " << total[i]->data.target;

			total_quest.SetString(oss.str());
			total_quest.SetFunc([=]() {key_button = (int)(i + 1);});

			if (total[i] == quest)
			{
				total_quest.SetFillColor(sf::Color(128, 128, 0));
			}

			buttons.push_back(total_quest);
			total_quest.SetFillColor(sf::Color(50, 50, 50));
			oss.clear();
			oss.str(L"");
		}
		else // квеста нет
		{
			new_quest.SetPosition(total_quest.GetPosition());
			buttons.push_back(new_quest);
		}

		total_quest.Move({0, total_quest.GetSize().y + 5.0f});
	}

	if (quest)
	{
		base_group = Group(shape, text);
		Button data_button(base_group);
		data_button.SetFillColor(sf::Color(50, 50, 50));
		if (quest->IsCompleted())
		{
			data_button.SetString(L"Получить награду");
			data_button.SetFunc([=]() {key_button = -200;});
		}
		else
		{
			data_button.SetString(L"Задание не выполнено");
		}

		data_button.SetSize({data_button.group.text.getLocalBounds().width + 10.0f, data_button.group.text.getLocalBounds().height + 10.0f});
		data_button.SetPosition({ total_quest.GetPosition().x + total_quest.GetSize().x / 2 + data_button.GetSize().x / 2 + 10.0f , kScreenHeight / 2});
		buttons.push_back(data_button);
	}

	text.setCharacterSize(50);
	text.setString(L"В\nЫ\nХ\nО\nД");
	shape.setSize({ text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f });
	shape.setFillColor(sf::Color(50, 50, 50));
	shape.setPosition({ (float)kScreenWight - shape.getSize().x - 5.0f, (float)kScreenHeight / 2 });
	base_group = Group(shape, text);
	button = Button(base_group);
	button.SetFunc([=]() { key_button = -100; });
	buttons.push_back(button);
}

void UIManager::InitTrade(const std::map<int, Itemble*>& variants, Player* player)
{
	background = sf::Sprite(Resources::trade_background);
	background.setScale({ (float)kScreenWight / Resources::trade_background.getSize().x,
		(float)kScreenHeight / Resources::trade_background.getSize().y });

	float interval = 5.0f;
	float size = 44.0f;

	float max_name = 0.0f, max_disc = 0.0f, max_cost = 0.0f;
	sf::Text text(L"", Resources::ui_font, (int)size - 25);
	
	for (auto& pair : variants)
	{
		if (!pair.second) continue;
		
		text.setString(pair.second->name);
		max_name = std::fmaxf(text.getLocalBounds().width, max_name);

		text.setString(pair.second->disc);
		max_disc = std::fmaxf(text.getLocalBounds().width, max_disc);

		text.setString(std::to_wstring(pair.second->cost) + L" руб");
		max_cost = std::fmaxf(text.getLocalBounds().width, max_cost);
	}
	
	sf::Vector2f pos((float)kScreenWight / 2, size + interval * 2.0f);
	sf::Vector2f text_pos(((float)kScreenWight - kDialogWight) / 2 - kIconSize / 2, size + interval * 2.0f);

	sf::RectangleShape rect({ kDialogWight, size });
	sf::RectangleShape texture({ kIconSize, kIconSize });
	sf::RectangleShape texture_1({ kIconSize, size });
	Group base_group (texture, {});
	Button button_5(base_group);
	base_group = Group(texture_1, {});
	Button button_6(base_group);
	button_5.SetTexture(&Resources::itemble_icon);
	button_6.SetFillColor(sf::Color(70,70,70));

	rect.setFillColor(sf::Color(50, 50, 50));
	Group group_2(rect, text);

	for (auto& var : variants)
	{
		if (!var.second) continue;

		std::wstring line = ToMax(var.second->name, max_name, size - 25) + L" | " + 
			ToMax(std::to_wstring(var.second->cost) + L" руб", max_cost, size - 25) + L" | " +
			ToMax(var.second->disc, max_disc, size - 25);

		group_2.SetString(line);
		group_2.SetPosition(pos);

		button_6.SetPosition(text_pos);
		button_5.SetPosition(text_pos);
		button_5.SetTextureRect({ {kIconSize * var.second->id, 0}, {kIconSize, kIconSize} });

		buttons.push_back(Button(group_2));
		buttons.back().SetFunc([=]() { key_button = var.first;});
		buttons.push_back(button_6);
		buttons.push_back(button_5);

		pos.y += interval + group_2.shape.getSize().y;
		text_pos.y = pos.y;
	}

	sf::RectangleShape rect_2(rect);
	rect_2.setSize({ kDialogWight, size - 20.0f });
	Group group_9(rect_2, text);
	group_9.SetString(L"Баланс: " + std::to_wstring(player->money) + L" | Запчасти: " + std::to_wstring(player->details));
	group_9.SetPosition({ (float)kScreenWight / 2, group_9.GetSize().y / 2 + interval});
	buttons.push_back(group_9);

	sf::Text text_1(text);
	text_1.setString(L"К\nУ\nП\nИ\nТ\nЬ");
	text_1.setCharacterSize(50);
	sf::RectangleShape rect_1({ size + 10.0f, 350.0f });
	rect_1.setFillColor(sf::Color(50, 50, 50));
	Group group_3(rect_1, text_1);
	group_3.SetPosition({ ((float)kScreenWight + kDialogWight) / 2 + group_3.GetSize().x, group_3.GetSize().y / 2 + 5.0f});

	buttons.push_back(Button(group_3));
	buttons.back().SetFunc([=]() { key_button = -200; });

	group_3.SetString(L"В\nЫ\nХ\nО\nД");
	group_3.SetPosition({ ((float)kScreenWight + kDialogWight) / 2 + group_3.GetSize().x , (float)kScreenHeight - group_3.GetSize().y/2 - 5.0f});
	buttons.push_back(Button(group_3));
	buttons.back().SetFunc([=]() { key_button = -100; });
}

void UIManager::InitMechanic(Player* player, Gun* choose)
{
	sf::Text text(L"Баланс: " + std::to_wstring(player->money) + L" | Запчасти: " + std::to_wstring(player->details) + L" | Одно улучшение стоит 50 руб и 15 деталей", Resources::ui_font, 30);
	sf::RectangleShape shape({ kDialogWight, text.getLocalBounds().height + 5.0f});
	shape.setFillColor(sf::Color(70, 70, 70));
	shape.setPosition({ (float)kScreenWight / 2, shape.getSize().y / 2 + 5.0f });
	Group base_group(shape, text);
	Button button(base_group);
	buttons.push_back(button);

	sf::RectangleShape data_shape({ kDialogWight / 4 + 15, kDialogHeight / 2 });
	data_shape.setFillColor(sf::Color(50, 50, 50));
	Group data_group(data_shape, {});
	data_group.SetPosition({ shape.getPosition().x - shape.getSize().x / 2 + data_shape.getSize().x / 2, shape.getPosition().y / 2 + kInterval + data_group.GetSize().y / 2});

	sf::RectangleShape texture_shape({ kIconSize, kIconSize });
	texture_shape.setScale({ 2, 2 });
	Group texture_group(texture_shape, {});
	
	std::wostringstream oss;
	for (size_t i = 1; i <= 2; i++)
	{
		if (player->guns[i])
		{
			if (player->guns[i] == choose) data_group.shape.setFillColor(sf::Color(128, 128, 0));
			buttons.push_back(data_group);
			buttons.back().SetFunc([=]() { key_button = (int)i; });

			texture_group.SetPosition({ data_group.GetPosition().x, data_group.GetPosition().y - data_group.GetSize().y / 2 + kIconSize + 20.0f });
			buttons.push_back(Button(texture_group));
			buttons.back().SetTexture(&Resources::itemble_icon);
			buttons.back().SetTextureRect({ {kIconSize * player->guns[i]->id, 0},{kIconSize, kIconSize} });

			oss << L"Урон: " << std::fixed << std::setprecision(2) << player->guns[i]->damage << "\n";
			oss << L"Обойма: " << std::fixed << std::setprecision(2) << player->guns[i]->max_count << "\n";
			oss << L"Разброс: " << std::fixed << std::setprecision(2) << player->guns[i]->max_imp_rad << "\n";
			oss << L"Количество улучшений: " << player->guns[i]->upgrade_count << L"/ 5" << "\n";

			text.setCharacterSize(20);
			text.setString(oss.str());
			shape.setSize({ text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f });
			shape.setPosition({ texture_group.GetPosition().x, texture_group.GetPosition().y + texture_group.GetSize().y + 60.0f });
			base_group = Group(shape, text);
			buttons.push_back(Button(base_group));

			oss.clear();
			oss.str(L"");
			data_group.Move({ 0, data_group.GetSize().y + 10.0f });
			data_group.shape.setFillColor(sf::Color(50, 50, 50));
		}
	}
	text.setCharacterSize(30);

	text.setString(L"У\nЛ\nУ\nЧ\nШ\nИ\nТ\nЬ");
	shape.setSize({ text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f });
	shape.setPosition({ ((float)kScreenWight + kDialogWight) / 2 + shape.getSize().x, (float)kScreenHeight / 2 - shape.getSize().y / 2 - 5.0f });
	shape.setFillColor(sf::Color(50, 50, 50));
	base_group = Group(shape, text);
	button = Button(base_group);
	button.SetFunc([=]() { key_button = -200; });
	buttons.push_back(button);

	text.setString(L"В\nЫ\nХ\nО\nД");
	shape.setPosition({ ((float)kScreenWight + kDialogWight) / 2 + shape.getSize().x, (float)kScreenHeight / 2 + shape.getSize().y / 2 + 5.0f });
	base_group = Group(shape, text);
	button = Button(base_group);
	button.SetFunc([=]() { key_button = -100; });
	buttons.push_back(button);

	if (choose)
	{
		sf::RectangleShape perc_shape({ kDialogWight / 2 + 15.0f, kDialogHeight / 4 });
		perc_shape.setPosition(data_group.GetPosition().x + data_group.GetSize().x / 2 + perc_shape.getSize().x / 2 + 10.0f, (float)kScreenHeight / 3 - perc_shape.getSize().y / 3 + 10.0f);
		perc_shape.setFillColor(sf::Color(50, 50, 50));
		Group perc_group(perc_shape, text);

		perc_group.SetString(L"УВЕЛИЧИТЬ УРОН НА +3");
		buttons.push_back(perc_group);
		buttons.back().SetFunc([=]() {key_button = 101;});
		perc_group.Move({0.0f, perc_group.GetSize().y + 10.0f});

		perc_group.SetString(L"УВЕЛИЧИТЬ РАЗМЕР ОБОЙМЫ НА +5");
		buttons.push_back(perc_group);
		buttons.back().SetFunc([=]() {key_button = 102;});
		perc_group.Move({ 0.0f, perc_group.GetSize().y + 10.0f });

		perc_group.SetString(L"УМЕНЬШИТЬ РАЗБРОС НА +2");
		buttons.push_back(perc_group);
		buttons.back().SetFunc([=]() {key_button = 103;});
	}
}

void UIManager::InitChanger(int coef, Player* player)
{
	background = sf::Sprite(Resources::trade_background);
	background.setScale({ (float)kScreenWight / Resources::trade_background.getSize().x,
		(float)kScreenHeight / Resources::trade_background.getSize().y });

	sf::Text text(L"Баланс: " + std::to_wstring(player->money) + L" | Запчасти: " + std::to_wstring(player->details), Resources::ui_font, 50);
	sf::RectangleShape shape({ text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f });
	shape.setFillColor(sf::Color(70, 70, 70));
	shape.setPosition({ (float)kScreenWight / 2, shape.getSize().y + 10.0f });
	Group base_group(shape, text);
	Button button(base_group);
	buttons.push_back(button);

	text.setString(L"Текущий курс: 1 деталь к " + std::to_wstring(coef) + L" монетам");
	shape.setSize({ text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f });
	shape.move({ 0, shape.getSize().y + 10.0f });
	base_group = Group(shape, text);
	button = Button(base_group);
	buttons.push_back(button);

	text.setString(L"Производится обмен 10 деталей сразу");
	shape.setSize({ text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f });
	shape.move({ 0, shape.getSize().y + 5.0f });
	base_group = Group(shape, text);
	button = Button(base_group);
	buttons.push_back(button);

	text.setString(L"О\nБ\nМ\nЕ\nН\nЯ\nТ\nЬ");
	shape.setSize({ text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f });
	shape.setPosition({ shape.getSize().x + 5.0f, (float)kScreenHeight / 2});
	base_group = Group(shape, text);
	button = Button(base_group);
	button.SetFunc([=]() { key_button = -200; });
	buttons.push_back(button);

	text.setString(L"В\nЫ\nХ\nО\nД");
	shape.setPosition({ (float)kScreenWight - shape.getSize().x - 5.0f, (float)kScreenHeight / 2 });
	base_group = Group(shape, text);
	button = Button(base_group);
	button.SetFunc([=]() { key_button = -100; });
	buttons.push_back(button);
}

void UIManager::InitInvent(const std::map<Itemble*, int>& items, Itemble* choose, Player* player)
{
	background = sf::Sprite(Resources::inventory_background);
	background.setScale({ (float)kScreenWight / Resources::inventory_background.getSize().x,
		(float)kScreenHeight / Resources::inventory_background.getSize().y });

	sf::RectangleShape base_shape({ kDialogWight / 2 + 15, kDialogHeight + 10});
	base_shape.setFillColor(sf::Color(70, 70, 70));
	Group base_group(base_shape, {});
	base_group.SetPosition({ kInterval + base_shape.getSize().x / 2 - 5,
							kInterval + base_shape.getSize().y / 2 - 5});
	buttons.push_back(base_group);
	
	sf::RectangleShape inv_shape({ kDialogWight / 4, kIconSize / 2  });
	inv_shape.setFillColor(sf::Color(50, 50, 50));
	sf::Text inv_text(L"", Resources::ui_font, kIconSize / 2 - 20);
	Group inv_group(inv_shape, inv_text);
	Button inv_button(inv_group);

	sf::Vector2f pos{ kDialogWight / 8 + kInterval, kInterval + inv_group.GetSize().y / 2 };
	int i = 0;

	for (auto pair : items)
	{
		inv_button.SetPosition(pos);
		inv_button.SetString(pair.first->name + L" | " + std::to_wstring(pair.second) + L" шт");
		inv_button.SetFunc([=]() { key_button = pair.first->id;});

		buttons.push_back(inv_button);

		if (i % 2 == 0)
		{
			pos.x = kDialogWight / 8 + inv_group.GetSize().x + kInterval + 5;
		}
		else
		{
			pos.y += inv_group.GetSize().y + 5;
			pos.x = kDialogWight / 8 + kInterval;
		}
		i++;
	}

	if (choose)
	{
		sf::RectangleShape data_shape({ kDialogWight / 4 + 15, kDialogHeight / 3 });
		data_shape.setFillColor(sf::Color(70, 70, 70));
		Group data_group(data_shape, {});
		data_group.SetPosition({ base_group.GetPosition().x, kInterval + data_group.GetSize().y / 2 });
		data_group.Move({ data_group.GetSize().x / 2 + base_group.GetSize().x / 2 + kInterval, 0 });
		buttons.push_back(data_group);

		sf::RectangleShape texture_shape({ kIconSize, kIconSize });
		texture_shape.setScale({ 2, 2 });
		Group texture_group(texture_shape, {});
		texture_group.SetPosition({ data_group.GetPosition().x, kInterval + kIconSize / 2 + 10 });
		buttons.push_back(Button(texture_group));
		buttons.back().SetTexture(&Resources::itemble_icon);
		buttons.back().SetTextureRect({ {kIconSize * choose->id, 0},{kIconSize, kIconSize} });

		Group data_group_1(inv_group);
		data_group_1.SetPosition(texture_group.GetPosition());
		data_group_1.Move({ 0, texture_group.GetSize().y / 2 + data_group_1.GetSize().y + 5 });
		data_group_1.SetString(choose->name);
		buttons.push_back(data_group_1);

		float old_size = data_group_1.GetSize().y;
		data_group_1.SetSize({ data_group_1.GetSize().x, data_group_1.GetSize().y * 2 });
		data_group_1.Move({ 0, old_size / 2 + data_group_1.GetSize().y / 2 + 5 });

		old_size = data_group.GetSize().y;
		data_group.SetSize({ data_group.GetSize().x, kDialogHeight * 2 / 3 - kInterval });
		data_group.Move({ 0, old_size / 2 + data_group.GetSize().y / 2 + kInterval });
		buttons.push_back(Button(data_group));

		Group make_group(inv_group);
		sf::Vector2f position = data_group.GetPosition();
		position.y -= data_group.GetSize().y / 2 - 5 - make_group.GetSize().y / 2;
		make_group.SetPosition(position);

		std::wostringstream oss;
		if (auto item = dynamic_cast<Item*>(choose); item)
		{
			make_group.SetString(L"Использовать");
			buttons.push_back(Button(make_group));
			buttons.back().SetFunc([&]() { key_button = 100;});

			oss << choose->disc;
		}
		else if (auto gun = dynamic_cast<Gun*>(choose); gun)
		{
			if (player->guns[1] != nullptr)
			{
				make_group.SetString(L"Надеть вместо " + player->guns[1]->name);
			}
			else
			{
				make_group.SetString(L"Надеть на первый слот");
			}
			buttons.push_back(make_group);
			buttons.back().SetFunc([&]() { key_button = 100;});
			make_group.Move({ 0, make_group.GetSize().y + 5 });

			if (player->guns[2] != nullptr)
			{
				make_group.SetString(L"Надеть вместо " + player->guns[2]->name);
			}
			else
			{
				make_group.SetString(L"Надеть на второй слот");
			}
			buttons.push_back(make_group);
			buttons.back().SetFunc([&]() { key_button = 101;});

			int i = 102;
			for (auto it : gun->improvement)
			{
				if (!it.second) continue;

				make_group.Move({ 0, make_group.GetSize().y + 5 });
				make_group.SetString(L"Снять " + it.second->name);
				buttons.push_back(Button(make_group));
				buttons.back().SetFunc([=]() { key_button = i;});
				i++;
			}

			oss << L"Урон: " << std::fixed << std::setprecision(2) << gun->damage;
			oss << " | ";
			oss << L"Обойма: " << std::fixed << std::setprecision(2) << gun->max_count;
			oss << " | ";
			oss << L"Разброс: " << std::fixed << std::setprecision(2) << gun->max_imp_rad;
		}
		else if (auto imp = dynamic_cast<Improve*>(choose); imp)
		{
			if (player->guns[1] != nullptr)
			{
				make_group.SetString(L"Надеть на " + player->guns[1]->name);
				buttons.push_back(Button(make_group));
				buttons.back().SetFunc([&]() { key_button = 100;});
				make_group.Move({ 0, make_group.GetSize().y + 5 });
			}

			if (player->guns[2] != nullptr)
			{
				make_group.SetString(L"Надеть на " + player->guns[2]->name);
				buttons.push_back(Button(make_group));
				buttons.back().SetFunc([&]() { key_button = 101;});
			}

			oss << choose->disc;
		}

		data_group_1.SetString(SplitText(oss.str(), (int)data_group_1.GetSize().x, data_group_1.text.getCharacterSize()));
		buttons.push_back(data_group_1);
	}
}

void UIManager::DeleteNow() 
{ 
	buttons.clear();
	chose_but = -1;
}

int UIManager::CheckButton()
{
	sf::Vector2i mouse_position = sf::Mouse::getPosition(*window);
	sf::Vector2i world_position = (sf::Vector2i)window->mapPixelToCoords(mouse_position);

	for (size_t i = 0; i < buttons.size(); i++)
	{
		if (buttons[i].IsClicked(world_position))
		{
			if (chose_but != -1) buttons[chose_but].SetFillColor(sf::Color(50, 50, 50));
			chose_but = i;
			buttons[i].SetFillColor(sf::Color(128, 128, 0));
			buttons[i].Use();

			return key_button;
		}
	}

	return -1;
}

void UIManager::DrawNow()
{
	window->draw(background);

	for (auto& b : buttons)
	{
		window->draw(b);
	}
}

void UIManager::DrawPlayerUI(Player* player)
{
	sf::Text weapon_info;

	if (!player->kick->IsCanUsed())
	{
		sf::Vector2f delta_position{};
		player->kick->DrawWeapon(window, delta_position);
	}
	else
	{
		auto gun = player->GetNowGun();
		gun->DrawWeapon(window, player->shake_delta);

		if (gun->is_reset)
		{
			weapon_info = sf::Text(std::to_string(player->patrons), Resources::ui_font, 30);
			auto b = weapon_info.getLocalBounds();
			weapon_info.setOrigin({ b.width / 2, b.height / 2 });
			weapon_info.setPosition({ (float)kScreenWight / 2, (float)kScreenHeight / 2 - weapon_info.getCharacterSize() / 4 });
			weapon_info.setPosition({ (float)kScreenWight - b.width / 2 - 20, (float)kScreenHeight - 30 });
			weapon_info.setFillColor({ 0, 0, 0 });
			window->draw(weapon_info);

			weapon_info.setString(std::to_string(gun->now_count) + " / " + std::to_string(gun->max_count));
			b = weapon_info.getLocalBounds();
			weapon_info.setOrigin({ b.width / 2, b.height / 2 });
			weapon_info.setPosition({ (float)kScreenWight / 2, (float)kScreenHeight / 2 - weapon_info.getCharacterSize() / 4 });
			weapon_info.setPosition({ (float)kScreenWight - b.width / 2 - 20, (float)kScreenHeight - 60 });
			window->draw(weapon_info);
		}
	}
	float baseX = 300;
	sf::RectangleShape base_shape({ baseX, 40 });
	base_shape.setFillColor(sf::Color(128, 128, 128));
	sf::Text text("", Resources::ui_font, 30);
	Group group_1(base_shape, text);
	group_1.SetPosition({ 170, (float)kScreenHeight - 120 });
	window->draw(group_1.shape);

	Group group_2(group_1);
	group_2.SetPosition({ group_2.GetPosition().x, group_2.GetPosition().y + 40 });
	window->draw(group_2.shape);

	Group group_3(group_1);
	group_3.SetPosition({ group_3.GetPosition().x, group_3.GetPosition().y + 80 });
	window->draw(group_3.shape);

	std::wostringstream oss;
	oss << std::fixed << std::setprecision(2) << player->enemy->map_sprite.nowHealPoint;
	oss << " / ";
	oss << std::fixed << std::setprecision(2) << player->enemy->enemy_def.maxHealpoint;
	std::wstring str = oss.str();

	group_1.shape.setFillColor(sf::Color(255, 23, 23));
	float new_healpoint = baseX * (player->enemy->map_sprite.nowHealPoint <= 0 ? 0 :
		player->enemy->map_sprite.nowHealPoint) / player->enemy->enemy_def.maxHealpoint;
	group_1.shape.setSize({ new_healpoint, 40 });
	group_1.SetString(str);
	window->draw(group_1.shape);
	window->draw(group_1.text);

	oss.str(L"");
	oss.clear();
	oss << std::fixed << std::setprecision(2) << player->now_strenght;
	oss << " / ";
	oss << std::fixed << std::setprecision(2) << player->max_strenght;
	str = oss.str();

	group_2.shape.setFillColor(sf::Color(70, 130, 80));
	float new_armore = baseX * player->now_strenght / player->max_strenght;
	group_2.shape.setSize({ new_armore, 40 });
	group_2.SetString(str);
	window->draw(group_2.shape);
	window->draw(group_2.text);

	oss.str(L"");
	oss.clear();
	oss << std::fixed << std::setprecision(2) << player->now_energy;
	oss << " / ";
	oss << std::fixed << std::setprecision(2) << player->max_energy;
	str = oss.str();

	group_3.shape.setFillColor(sf::Color(44, 148, 15));
	float new_energy = baseX * player->now_energy / player->max_energy;
	group_3.shape.setSize({ new_energy, 40 });
	group_3.SetString(str);
	window->draw(group_3.shape);
	window->draw(group_3.text);

	if (player->now_heal)
	{
		sf::RectangleShape rect({ kIconSize, kIconSize });
		rect.setTexture(&Resources::itemble_icon);
		rect.setTextureRect({ {player->now_heal->id * kIconSize, 0},{kIconSize, kIconSize} });
		auto b = rect.getLocalBounds();
		rect.setOrigin({ b.width / 2, b.height / 2 });
		if (weapon_info.getString() != "")
		{
			rect.setPosition({ (float)kScreenWight - kIconSize - weapon_info.getLocalBounds().width,  (float)kScreenHeight - kIconSize / 2 });
		}
		else
		{
			rect.setPosition({ (float)kScreenWight - b.width / 2 - 20, (float)kScreenHeight - kIconSize / 2 });
		}

		window->draw(rect);
	}

	sf::CircleShape aim(player->GetNowGun()->now_rad, 16);
	aim.setOrigin({ aim.getRadius(), aim.getRadius() });
	aim.setFillColor(sf::Color(0, 0, 0, 0));
	aim.setOutlineColor(sf::Color::Black);
	aim.setOutlineThickness(1.5f);
	aim.setPosition({ (float)kScreenWight / 2, (float)kScreenHeight / 2 });
	window->draw(aim);
}