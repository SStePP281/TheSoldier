#include "UIManeger.h"
#include <sstream>
#include <string>
#include <iomanip>
#include <SFML/Graphics.hpp>

UIManager::UIManager(sf::RenderWindow* _window) : 
	window{ _window }, choseBut{ -1 }, keyButton{ -1 } {}

std::wstring UIManager::SplitText(std::wstring text, int maxLen, int textSize)
{
	std::wstring result, word, curLine;
	std::wistringstream stream(text);

	while (stream >> word)
	{
		sf::Text tempText;
		tempText.setFont(Resources::ui_font);
		tempText.setCharacterSize(textSize);
		std::wstring testLine = curLine.empty() ? word : curLine + L" " + word;
		tempText.setString(testLine);

		if (tempText.getLocalBounds().width > maxLen)
		{
			result += curLine + L"\n";
			curLine = word;
		}
		else
		{
			curLine = testLine;
		}
	}

	if (!curLine.empty())
	{
		result += curLine;
	}

	return result;
}

std::wstring UIManager::ToMax(std::wstring str, float maxW, float textSize)
{
	sf::Text text(L"", Resources::ui_font, (int)textSize);
	std::wstring result = str;
	while (true)
	{
		text.setString(result);

		if (text.getLocalBounds().width > maxW)
		{
			break;
		}

		result += L" ";
	}
	return result;
}

void UIManager::InitDialog(std::map<int, std::wstring, std::greater<int>>& variants,
	const std::wstring& npcName)
{
	background = sf::Sprite(Resources::dialog_background);
	background.setScale({ (float)kScreenWight / Resources::dialog_background.getSize().x,
		(float)kScreenHeight / Resources::dialog_background.getSize().y });

	sf::RectangleShape nameBase{ {kDialogWight, kTextSize + 10} };
	nameBase.setFillColor(sf::Color(100, 100, 100));
	sf::Text nameText(npcName, Resources::ui_font, kTextSize);
	Group g(nameBase, nameText);
	g.SetPosition({ (float)kScreenWight / 2, kInterval});
	buttons.push_back(g);

	sf::RectangleShape dataBase{ {kDialogWight, kDialogHeight / 1.5f} };
	dataBase.setFillColor(sf::Color(100, 100, 100));
	sf::Text dataText(SplitText(variants[-1], (int)kDialogWight, 40), Resources::ui_font, kTextSize - 10);
	Group g1(dataBase, dataText);
	g1.SetPosition({ (float)kScreenWight / 2, g.GetPosition().y + g1.GetSize().y / 2 + kInterval});
	buttons.push_back(g1);

	sf::Vector2f pos((float)kScreenWight / 2, g1.GetPosition().y + g1.GetSize().y / 2 + kInterval);
	
	for (auto var : variants)
	{
		if (var.first == -1) continue;
		sf::Text text(var.second, Resources::ui_font, kTextSize);
		sf::RectangleShape rect({ kDialogWight, kTextSize + 10 });
		rect.setFillColor(sf::Color(50, 50, 50));
		Group g2(rect, text);
		g2.SetPosition(pos);
		
		buttons.push_back(Button(g2));
		buttons.back().SetFunc([=]() { keyButton = var.first;});

		pos.y +=  kInterval + g2.shape.getSize().y / 2;
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
	button.SetFunc([=]() { keyButton = 0;});
	buttons.push_back(button);

	button.SetString(L"ВЫХОД");
	button.SetPosition({ (float)kScreenWight / 2.0f, 2.0f * (float)kScreenHeight / 3.0f + 35.0f });
	button.SetFunc([=]() { keyButton = 1;});
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
	button.SetFunc([=]() { keyButton = 0;});
	auto& state = GameState::GetInstance();
	if (!state.data.isFirstGame) buttons.push_back(button);

	button.SetString(L"НОВАЯ ИГРА");
	button.SetPosition({ (float)kScreenWight / 2.0f, 2.0f * (float)kScreenHeight / 3.0f + 35.0f });
	button.SetFunc([=]() { keyButton = 1;});
	buttons.push_back(button);

	button.SetString(L"ВЫХОД");
	button.SetPosition({ (float)kScreenWight / 2.0f, 2.0f * (float)kScreenHeight / 3.0f + 105.0f });
	button.SetFunc([=]() { keyButton = 2;});
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
	button.SetFunc([=]() { keyButton = 0; });
	buttons.push_back(button);

	button.SetString(L"НАСТРОЙКИ");
	button.SetPosition({ (float)kScreenWight / 2.0f, 2.0f * (float)kScreenHeight / 3.0f + 35.0f });
	button.SetFunc([=]() { keyButton = 1;});
	buttons.push_back(button);

	button.SetString(L"ВЫХОД");
	button.SetPosition({ (float)kScreenWight / 2.0f, 2.0f * (float)kScreenHeight / 3.0f + 105.0f });
	button.SetFunc([=]() { keyButton = 2;});
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
	button.SetFunc([=]() { keyButton = 0; });
	buttons.push_back(button);

	text.setString(L"+");
	sf::RectangleShape funcShape({ 60.0f, 60.0f });
	funcShape.setFillColor(sf::Color(50, 50, 50));
	base_group = Group(shape, text);
	Button funcButton(base_group);
	
	auto& state = GameState::GetInstance();

	text.setString(L"ГРОМКОСТЬ ЕФФЕКТОВ: " + std::to_wstring(state.data.effectVolume));
	shape = sf::RectangleShape({ text.getLocalBounds().width + 20.0f, 60.0f });
	shape.setFillColor(sf::Color(100, 100, 100));
	base_group = Group(shape, text);
	button = Button(base_group);
	button.SetPosition({ (float)kScreenWight / 2.0f, 150.0f});
	buttons.push_back(button);
	funcButton.SetPosition({ button.GetPosition().x + button.GetSize().x / 2 + 30.0f, button.GetPosition().y });
	funcButton.SetFunc([&]() {state.data.effectVolume++;keyButton = 1;});
	buttons.push_back(funcButton);
	funcButton.SetPosition({ button.GetPosition().x - button.GetSize().x / 2 - 30.0f, button.GetPosition().y });
	funcButton.SetFunc([&]() {state.data.effectVolume--;keyButton = 1;});
	funcButton.SetString(L"-");
	buttons.push_back(funcButton);

	button.SetString(L"ГРОМКОСТЬ МУЗЫКИ: " + std::to_wstring(state.data.soundVolume));
	button.SetPosition({ (float)kScreenWight / 2.0f, 220.0f});
	buttons.push_back(button);
	funcButton.SetString(L"+");
	funcButton.SetPosition({ button.GetPosition().x + button.GetSize().x / 2 + 30.0f, button.GetPosition().y });
	funcButton.SetFunc([&]() {state.data.soundVolume++;keyButton = 1;});
	buttons.push_back(funcButton);
	funcButton.SetPosition({ button.GetPosition().x - button.GetSize().x / 2 - 30.0f, button.GetPosition().y });
	funcButton.SetFunc([&]() {state.data.soundVolume--; keyButton = 1;});
	funcButton.SetString(L"-");
	buttons.push_back(funcButton);

	std::wstring std = std::to_wstring(state.data.mouseSpeed + 1.0f);
	button.SetString(L"CКОРОСТЬ МЫШИ: " + std.substr(0, std.find(L".") + 3));
	button.SetPosition({ (float)kScreenWight / 2.0f, 290.0f });
	buttons.push_back(button);
	funcButton.SetString(L"+");
	funcButton.SetPosition({ button.GetPosition().x + button.GetSize().x / 2 + 30.0f, button.GetPosition().y });
	funcButton.SetFunc([&]() {state.data.mouseSpeed += 0.01f; keyButton = 2;});
	buttons.push_back(funcButton);
	funcButton.SetPosition({ button.GetPosition().x - button.GetSize().x / 2 - 30.0f, button.GetPosition().y });
	funcButton.SetFunc([&]() {state.data.mouseSpeed -= 0.01f; keyButton = 2;});
	funcButton.SetString(L"-");
	buttons.push_back(funcButton);
	
	funcButton.SetFunc(nullptr);
	shape = sf::RectangleShape({shape.getSize().x / 2, 30.0f});
	shape.setFillColor(sf::Color(100, 100, 100));
	text = sf::Text(L"", Resources::ui_font, 20);
	base_group = Group(shape, text);
	button = Button(base_group);
	funcShape.setSize({ 30.0f, 30.0f });
	Group func_group(funcShape, text);
	funcButton = Button(func_group);
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
		funcButton.SetString(keys[i].first);
		button.SetPosition(pos);
		funcButton.SetPosition({ pos.x - button.GetSize().x / 2 - funcShape.getSize().x / 2, pos.y });
		buttons.push_back(button);
		buttons.push_back(funcButton);

		if (i % 2 == 0)
		{
			pos.x += shape.getSize().x + 15.0f + funcButton.GetSize().x; 
		}
		else
		{
			pos.x -= shape.getSize().x + 15.0f + funcButton.GetSize().x;
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
	Button newQuest(base_group);
	newQuest.SetFillColor(sf::Color(50, 50, 50));
	newQuest.SetFunc([=]() {keyButton = -300; });

	text.setCharacterSize(30);
	shape.setSize({kDialogWight / 3, kDialogHeight / 3 - 20.0f});
	shape.setPosition({shape.getSize().x, shape.getSize().y / 2 + 90.0f});
	base_group = Group(shape, text);
	Button totalQuest(base_group);
	totalQuest.SetFillColor(sf::Color(50, 50, 50));

	auto& questM = QuestManager::GetInstance();
	auto total = questM.quests;
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

			totalQuest.SetString(oss.str());
			totalQuest.SetFunc([=]() {keyButton = (int)(i + 1);});

			if (total[i] == quest)
			{
				totalQuest.SetFillColor(sf::Color(128, 128, 0));
			}

			buttons.push_back(totalQuest);
			totalQuest.SetFillColor(sf::Color(50, 50, 50));
			oss.clear();
			oss.str(L"");
		}
		else // квеста нет
		{
			newQuest.SetPosition(totalQuest.GetPosition());
			buttons.push_back(newQuest);
		}

		totalQuest.Move({0, totalQuest.GetSize().y + 5.0f});
	}

	if (quest)
	{
		base_group = Group(shape, text);
		Button dataButton(base_group);
		dataButton.SetFillColor(sf::Color(50, 50, 50));
		if (quest->IsCompleted())
		{
			dataButton.SetString(L"Получить награду");
			dataButton.SetFunc([=]() {keyButton = -200;});
		}
		else
		{
			dataButton.SetString(L"Задание не выполнено");
		}

		dataButton.SetSize({dataButton.group.text.getLocalBounds().width + 10.0f, dataButton.group.text.getLocalBounds().height + 10.0f});
		dataButton.SetPosition({ totalQuest.GetPosition().x + totalQuest.GetSize().x / 2 + dataButton.GetSize().x / 2 + 10.0f , kScreenHeight / 2});
		buttons.push_back(dataButton);
	}

	text.setCharacterSize(50);
	text.setString(L"В\nЫ\nХ\nО\nД");
	shape.setSize({ text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f });
	shape.setFillColor(sf::Color(50, 50, 50));
	shape.setPosition({ (float)kScreenWight - shape.getSize().x - 5.0f, (float)kScreenHeight / 2 });
	base_group = Group(shape, text);
	button = Button(base_group);
	button.SetFunc([=]() { keyButton = -100; });
	buttons.push_back(button);
}

void UIManager::InitTrade(const std::map<int, Itemble*>& variants, Player* player)
{
	background = sf::Sprite(Resources::trade_background);
	background.setScale({ (float)kScreenWight / Resources::trade_background.getSize().x,
		(float)kScreenHeight / Resources::trade_background.getSize().y });

	float interval = 5.0f;
	float size = 44.0f;

	float maxName = 0.0f, maxDisc = 0.0f, maxCost = 0.0f;
	sf::Text text(L"", Resources::ui_font, (int)size - 25);
	
	for (auto pair : variants)
	{
		if (!pair.second) continue;
		
		text.setString(pair.second->name);
		maxName = std::fmaxf(text.getLocalBounds().width, maxName);

		text.setString(pair.second->disc);
		maxDisc = std::fmaxf(text.getLocalBounds().width, maxDisc);

		text.setString(std::to_wstring(pair.second->cost) + L" руб");
		maxCost = std::fmaxf(text.getLocalBounds().width, maxCost);
	}
	
	sf::Vector2f pos((float)kScreenWight / 2, size + interval * 2.0f);
	sf::Vector2f textPos(((float)kScreenWight - kDialogWight) / 2 - kIconSize / 2, size + interval * 2.0f);

	sf::RectangleShape rect({ kDialogWight, size });
	sf::RectangleShape texture({ kIconSize, kIconSize });
	sf::RectangleShape texture1({ kIconSize, size });
	Group base_group (texture, {});
	Button b5(base_group);
	base_group = Group(texture1, {});
	Button b6(base_group);
	b5.SetTexture(&Resources::itemble_icon);
	b6.SetFillColor(sf::Color(70,70,70));

	rect.setFillColor(sf::Color(50, 50, 50));
	Group g2(rect, text);

	for (auto var : variants)
	{
		if (!var.second) continue;

		std::wstring line = ToMax(var.second->name, maxName, size - 25) + L" | " + 
			ToMax(std::to_wstring(var.second->cost) + L" руб", maxCost, size - 25) + L" | " +
			ToMax(var.second->disc, maxDisc, size - 25);

		g2.SetString(line);
		g2.SetPosition(pos);

		b6.SetPosition(textPos);
		b5.SetPosition(textPos);
		b5.SetTextureRect({ {kIconSize * var.second->id, 0}, {kIconSize, kIconSize} });

		buttons.push_back(Button(g2));
		buttons.back().SetFunc([=]() { keyButton = var.first;});
		buttons.push_back(b6);
		buttons.push_back(b5);

		pos.y += interval + g2.shape.getSize().y;
		textPos.y = pos.y;
	}

	sf::RectangleShape rect2(rect);
	rect2.setSize({ kDialogWight, size - 20.0f });
	Group g9(rect2, text);
	g9.SetString(L"Баланс: " + std::to_wstring(player->money) + L" | Запчасти: " + std::to_wstring(player->details));
	g9.SetPosition({ (float)kScreenWight / 2, g9.GetSize().y / 2 + interval});
	buttons.push_back(g9);

	sf::Text text1(text);
	text1.setString(L"К\nУ\nП\nИ\nТ\nЬ");
	text1.setCharacterSize(50);
	sf::RectangleShape rect1({ size + 10.0f, 350.0f });
	rect1.setFillColor(sf::Color(50, 50, 50));
	Group g3(rect1, text1);
	g3.SetPosition({ ((float)kScreenWight + kDialogWight) / 2 + g3.GetSize().x, g3.GetSize().y / 2 + 5.0f});

	buttons.push_back(Button(g3));
	buttons.back().SetFunc([=]() { keyButton = -200; });

	g3.SetString(L"В\nЫ\nХ\nО\nД");
	g3.SetPosition({ ((float)kScreenWight + kDialogWight) / 2 + g3.GetSize().x , (float)kScreenHeight - g3.GetSize().y/2 - 5.0f});
	buttons.push_back(Button(g3));
	buttons.back().SetFunc([=]() { keyButton = -100; });
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

	sf::RectangleShape dataShape({ kDialogWight / 4 + 15, kDialogHeight / 2 });
	dataShape.setFillColor(sf::Color(50, 50, 50));
	Group dataGroup(dataShape, {});
	dataGroup.SetPosition({ shape.getPosition().x - shape.getSize().x / 2 + dataShape.getSize().x / 2, shape.getPosition().y / 2 + kInterval + dataGroup.GetSize().y / 2});

	sf::RectangleShape textureShape({ kIconSize, kIconSize });
	textureShape.setScale({ 2, 2 });
	Group textureGroup(textureShape, {});
	
	std::wostringstream oss;
	for (size_t i = 1; i <= 2; i++)
	{
		if (player->guns[i])
		{
			if (player->guns[i] == choose) dataGroup.shape.setFillColor(sf::Color(128, 128, 0));
			buttons.push_back(dataGroup);
			buttons.back().SetFunc([=]() { keyButton = (int)i; });

			textureGroup.SetPosition({ dataGroup.GetPosition().x, dataGroup.GetPosition().y - dataGroup.GetSize().y / 2 + kIconSize + 20.0f });
			buttons.push_back(Button(textureGroup));
			buttons.back().SetTexture(&Resources::itemble_icon);
			buttons.back().SetTextureRect({ {kIconSize * player->guns[i]->id, 0},{kIconSize, kIconSize} });

			oss << L"Урон: " << std::fixed << std::setprecision(2) << player->guns[i]->damage << "\n";
			oss << L"Обойма: " << std::fixed << std::setprecision(2) << player->guns[i]->maxCount << "\n";
			oss << L"Разброс: " << std::fixed << std::setprecision(2) << player->guns[i]->maxImpRad << "\n";
			oss << L"Количество улучшений: " << player->guns[i]->upgradeCount << L"/ 5" << "\n";

			text.setCharacterSize(20);
			text.setString(oss.str());
			shape.setSize({ text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f });
			shape.setPosition({ textureGroup.GetPosition().x, textureGroup.GetPosition().y + textureGroup.GetSize().y + 60.0f });
			base_group = Group(shape, text);
			buttons.push_back(Button(base_group));

			oss.clear();
			oss.str(L"");
			dataGroup.Move({ 0, dataGroup.GetSize().y + 10.0f });
			dataGroup.shape.setFillColor(sf::Color(50, 50, 50));
		}
	}
	text.setCharacterSize(30);

	text.setString(L"У\nЛ\nУ\nЧ\nШ\nИ\nТ\nЬ");
	shape.setSize({ text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f });
	shape.setPosition({ ((float)kScreenWight + kDialogWight) / 2 + shape.getSize().x, (float)kScreenHeight / 2 - shape.getSize().y / 2 - 5.0f });
	shape.setFillColor(sf::Color(50, 50, 50));
	base_group = Group(shape, text);
	button = Button(base_group);
	button.SetFunc([=]() { keyButton = -200; });
	buttons.push_back(button);

	text.setString(L"В\nЫ\nХ\nО\nД");
	shape.setPosition({ ((float)kScreenWight + kDialogWight) / 2 + shape.getSize().x, (float)kScreenHeight / 2 + shape.getSize().y / 2 + 5.0f });
	base_group = Group(shape, text);
	button = Button(base_group);
	button.SetFunc([=]() { keyButton = -100; });
	buttons.push_back(button);

	if (choose)
	{
		sf::RectangleShape percShape({ kDialogWight / 2 + 15.0f, kDialogHeight / 4 });
		percShape.setPosition(dataGroup.GetPosition().x + dataGroup.GetSize().x / 2 + percShape.getSize().x / 2 + 10.0f, (float)kScreenHeight / 3 - percShape.getSize().y / 3 + 10.0f);
		percShape.setFillColor(sf::Color(50, 50, 50));
		Group percGroup(percShape, text);

		percGroup.SetString(L"УВЕЛИЧИТЬ УРОН НА +3");
		buttons.push_back(percGroup);
		buttons.back().SetFunc([=]() {keyButton = 101;});
		percGroup.Move({0.0f, percGroup.GetSize().y + 10.0f});

		percGroup.SetString(L"УВЕЛИЧИТЬ РАЗМЕР ОБОЙМЫ НА +5");
		buttons.push_back(percGroup);
		buttons.back().SetFunc([=]() {keyButton = 102;});
		percGroup.Move({ 0.0f, percGroup.GetSize().y + 10.0f });

		percGroup.SetString(L"УМЕНЬШИТЬ РАЗБРОС НА +2");
		buttons.push_back(percGroup);
		buttons.back().SetFunc([=]() {keyButton = 103;});
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
	button.SetFunc([=]() { keyButton = -200; });
	buttons.push_back(button);

	text.setString(L"В\nЫ\nХ\nО\nД");
	shape.setPosition({ (float)kScreenWight - shape.getSize().x - 5.0f, (float)kScreenHeight / 2 });
	base_group = Group(shape, text);
	button = Button(base_group);
	button.SetFunc([=]() { keyButton = -100; });
	buttons.push_back(button);
}

void UIManager::InitInvent(const std::map<Itemble*, int>& items, Itemble* choose, Player* player)
{
	background = sf::Sprite(Resources::inventory_background);
	background.setScale({ (float)kScreenWight / Resources::inventory_background.getSize().x,
		(float)kScreenHeight / Resources::inventory_background.getSize().y });

	sf::RectangleShape baseShape({ kDialogWight / 2 + 15, kDialogHeight + 10});
	baseShape.setFillColor(sf::Color(70, 70, 70));
	Group baseGroup(baseShape, {});
	baseGroup.SetPosition({ kInterval + baseShape.getSize().x / 2 - 5,
							kInterval + baseShape.getSize().y / 2 - 5});
	buttons.push_back(baseGroup);
	
	sf::RectangleShape invShape({ kDialogWight / 4, kIconSize / 2  });
	invShape.setFillColor(sf::Color(50, 50, 50));
	sf::Text invText(L"", Resources::ui_font, kIconSize / 2 - 20);
	Group invGroup(invShape, invText);
	Button invBut(invGroup);

	sf::Vector2f pos{ kDialogWight / 8 + kInterval, kInterval + invGroup.GetSize().y / 2 };
	int i = 0;

	for (auto pair : items)
	{
		invBut.SetPosition(pos);
		invBut.SetString(pair.first->name + L" | " + std::to_wstring(pair.second) + L" шт");
		invBut.SetFunc([=]() { keyButton = pair.first->id;});

		buttons.push_back(invBut);

		if (i % 2 == 0)
		{
			pos.x = kDialogWight / 8 + invGroup.GetSize().x + kInterval + 5;
		}
		else
		{
			pos.y += invGroup.GetSize().y + 5;
			pos.x = kDialogWight / 8 + kInterval;
		}
		i++;
	}

	if (choose)
	{
		sf::RectangleShape dataShape({ kDialogWight / 4 + 15, kDialogHeight / 3 });
		dataShape.setFillColor(sf::Color(70, 70, 70));
		Group dataGroup(dataShape, {});
		dataGroup.SetPosition({ baseGroup.GetPosition().x, kInterval + dataGroup.GetSize().y / 2 });
		dataGroup.Move({ dataGroup.GetSize().x / 2 + baseGroup.GetSize().x / 2 + kInterval, 0 });
		buttons.push_back(dataGroup);

		sf::RectangleShape textureShape({ kIconSize, kIconSize });
		textureShape.setScale({ 2, 2 });
		Group textureGroup(textureShape, {});
		textureGroup.SetPosition({ dataGroup.GetPosition().x, kInterval + kIconSize / 2 + 10 });
		buttons.push_back(Button(textureGroup));
		buttons.back().SetTexture(&Resources::itemble_icon);
		buttons.back().SetTextureRect({ {kIconSize * choose->id, 0},{kIconSize, kIconSize} });

		Group dataGroup1(invGroup);
		dataGroup1.SetPosition(textureGroup.GetPosition());
		dataGroup1.Move({ 0, textureGroup.GetSize().y / 2 + dataGroup1.GetSize().y + 5 });
		dataGroup1.SetString(choose->name);
		buttons.push_back(dataGroup1);

		float oldSize = dataGroup1.GetSize().y;
		dataGroup1.SetSize({ dataGroup1.GetSize().x, dataGroup1.GetSize().y * 2 });
		dataGroup1.Move({ 0, oldSize / 2 + dataGroup1.GetSize().y / 2 + 5 });

		oldSize = dataGroup.GetSize().y;
		dataGroup.SetSize({ dataGroup.GetSize().x, kDialogHeight * 2 / 3 - kInterval });
		dataGroup.Move({ 0, oldSize / 2 + dataGroup.GetSize().y / 2 + kInterval });
		buttons.push_back(Button(dataGroup));

		Group makeGroup(invGroup);
		sf::Vector2f pos = dataGroup.GetPosition();
		pos.y -= dataGroup.GetSize().y / 2 - 5 - makeGroup.GetSize().y / 2;
		makeGroup.SetPosition(pos);

		std::wostringstream oss;
		if (auto item = dynamic_cast<Item*>(choose); item)
		{
			makeGroup.SetString(L"Использовать");
			buttons.push_back(Button(makeGroup));
			buttons.back().SetFunc([&]() { keyButton = 100;});

			oss << choose->disc;
		}
		else if (auto gun = dynamic_cast<Gun*>(choose); gun)
		{
			if (player->guns[1] != nullptr)
			{
				makeGroup.SetString(L"Надеть вместо " + player->guns[1]->name);
			}
			else
			{
				makeGroup.SetString(L"Надеть на первый слот");
			}
			buttons.push_back(makeGroup);
			buttons.back().SetFunc([&]() { keyButton = 100;});
			makeGroup.Move({ 0, makeGroup.GetSize().y + 5 });

			if (player->guns[2] != nullptr)
			{
				makeGroup.SetString(L"Надеть вместо " + player->guns[2]->name);
			}
			else
			{
				makeGroup.SetString(L"Надеть на второй слот");
			}
			buttons.push_back(makeGroup);
			buttons.back().SetFunc([&]() { keyButton = 101;});

			int i = 102;
			for (auto it : gun->improvement)
			{
				if (!it.second) continue;

				makeGroup.Move({ 0, makeGroup.GetSize().y + 5 });
				makeGroup.SetString(L"Снять " + it.second->name);
				buttons.push_back(Button(makeGroup));
				buttons.back().SetFunc([=]() { keyButton = i;});
				i++;
			}

			oss << L"Урон: " << std::fixed << std::setprecision(2) << gun->damage;
			oss << " | ";
			oss << L"Обойма: " << std::fixed << std::setprecision(2) << gun->maxCount;
			oss << " | ";
			oss << L"Разброс: " << std::fixed << std::setprecision(2) << gun->maxImpRad;
		}
		else if (auto imp = dynamic_cast<Improve*>(choose); imp)
		{
			if (player->guns[1] != nullptr)
			{
				makeGroup.SetString(L"Надеть на " + player->guns[1]->name);
				buttons.push_back(Button(makeGroup));
				buttons.back().SetFunc([&]() { keyButton = 100;});
				makeGroup.Move({ 0, makeGroup.GetSize().y + 5 });
			}

			if (player->guns[2] != nullptr)
			{
				makeGroup.SetString(L"Надеть на " + player->guns[2]->name);
				buttons.push_back(Button(makeGroup));
				buttons.back().SetFunc([&]() { keyButton = 101;});
			}

			oss << choose->disc;
		}

		dataGroup1.SetString(SplitText(oss.str(), (int)dataGroup1.GetSize().x, dataGroup1.text.getCharacterSize()));
		buttons.push_back(dataGroup1);
	}
}

void UIManager::DeleteNow() 
{ 
	buttons.clear();
	choseBut = -1;
}

int UIManager::CheckButton()
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
	sf::Vector2i worldPos = (sf::Vector2i)window->mapPixelToCoords(mousePos);

	for (size_t i = 0; i < buttons.size(); i++)
	{
		if (buttons[i].IsClicked(worldPos))
		{
			if (choseBut != -1) buttons[choseBut].SetFillColor(sf::Color(50, 50, 50));
			choseBut = i;
			buttons[i].SetFillColor(sf::Color(128, 128, 0));
			buttons[i].Use();

			return keyButton;
		}
	}
	return -1;
}

void UIManager::DrawNow()
{
	window->draw(background);
	for (auto b : buttons)
	{
		window->draw(b);
	}
}

void UIManager::DrawPlayerUI(Player* player)
{
	sf::Text weaponInfo;

	if (!player->kick->IsCanUsed())
	{
		sf::Vector2f deltaPos{};
		player->kick->DrawWeapon(window, deltaPos);
	}
	else
	{
		auto gun = player->GetNowGun();
		gun->DrawWeapon(window, player->shake_delta);

		if (gun->isReset)
		{
			weaponInfo = sf::Text(std::to_string(player->patrons), Resources::ui_font, 30);
			auto b = weaponInfo.getLocalBounds();
			weaponInfo.setOrigin({ b.width / 2, b.height / 2 });
			weaponInfo.setPosition({ (float)kScreenWight / 2, (float)kScreenHeight / 2 - weaponInfo.getCharacterSize() / 4 });
			weaponInfo.setPosition({ (float)kScreenWight - b.width / 2 - 20, (float)kScreenHeight - 30 });
			weaponInfo.setFillColor({ 0, 0, 0 });
			window->draw(weaponInfo);

			weaponInfo.setString(std::to_string(gun->nowCount) + " / " + std::to_string(gun->maxCount));
			b = weaponInfo.getLocalBounds();
			weaponInfo.setOrigin({ b.width / 2, b.height / 2 });
			weaponInfo.setPosition({ (float)kScreenWight / 2, (float)kScreenHeight / 2 - weaponInfo.getCharacterSize() / 4 });
			weaponInfo.setPosition({ (float)kScreenWight - b.width / 2 - 20, (float)kScreenHeight - 60 });
			window->draw(weaponInfo);
		}
	}
	float baseX = 300;
	sf::RectangleShape baseShape({ baseX, 40 });
	baseShape.setFillColor(sf::Color(128, 128, 128));
	sf::Text text("", Resources::ui_font, 30);
	Group group1(baseShape, text);
	group1.SetPosition({ 170, (float)kScreenHeight - 120 });
	window->draw(group1.shape);

	Group group2(group1);
	group2.SetPosition({ group2.GetPosition().x, group2.GetPosition().y + 40 });
	window->draw(group2.shape);

	Group group3(group1);
	group3.SetPosition({ group3.GetPosition().x, group3.GetPosition().y + 80 });
	window->draw(group3.shape);

	std::wostringstream oss;
	oss << std::fixed << std::setprecision(2) << player->enemy->map_sprite.nowHealPoint;
	oss << " / ";
	oss << std::fixed << std::setprecision(2) << player->enemy->enemy_def.maxHealpoint;
	std::wstring str = oss.str();

	group1.shape.setFillColor(sf::Color(255, 23, 23));
	float newXH = baseX * (player->enemy->map_sprite.nowHealPoint <= 0 ? 0 :
		player->enemy->map_sprite.nowHealPoint) / player->enemy->enemy_def.maxHealpoint;
	group1.shape.setSize({ newXH, 40 });
	group1.SetString(str);
	window->draw(group1.shape);
	window->draw(group1.text);

	oss.str(L"");
	oss.clear();
	oss << std::fixed << std::setprecision(2) << player->now_strenght;
	oss << " / ";
	oss << std::fixed << std::setprecision(2) << player->max_strenght;
	str = oss.str();

	group2.shape.setFillColor(sf::Color(70, 130, 80));
	float newXD = baseX * player->now_strenght / player->max_strenght;
	group2.shape.setSize({ newXD, 40 });
	group2.SetString(str);
	window->draw(group2.shape);
	window->draw(group2.text);

	oss.str(L"");
	oss.clear();
	oss << std::fixed << std::setprecision(2) << player->now_energy;
	oss << " / ";
	oss << std::fixed << std::setprecision(2) << player->max_energy;
	str = oss.str();

	group3.shape.setFillColor(sf::Color(44, 148, 15));
	float newXB = baseX * player->now_energy / player->max_energy;
	group3.shape.setSize({ newXB, 40 });
	group3.SetString(str);
	window->draw(group3.shape);
	window->draw(group3.text);

	if (player->now_heal)
	{
		sf::RectangleShape rect({ kIconSize, kIconSize });
		rect.setTexture(&Resources::itemble_icon);
		rect.setTextureRect({ {player->now_heal->id * kIconSize, 0},{kIconSize, kIconSize} });
		auto b = rect.getLocalBounds();
		rect.setOrigin({ b.width / 2, b.height / 2 });
		if (weaponInfo.getString() != "")
		{
			rect.setPosition({ (float)kScreenWight - kIconSize - weaponInfo.getLocalBounds().width,  (float)kScreenHeight - kIconSize / 2 });
		}
		else
		{
			rect.setPosition({ (float)kScreenWight - b.width / 2 - 20, (float)kScreenHeight - kIconSize / 2 });
		}
		window->draw(rect);
	}

	sf::CircleShape aim(player->GetNowGun()->nowRad, 16);
	aim.setOrigin({ aim.getRadius(), aim.getRadius() });
	aim.setFillColor(sf::Color(0, 0, 0, 0));
	aim.setOutlineColor(sf::Color::Black);
	aim.setOutlineThickness(1.5f);
	aim.setPosition({ (float)kScreenWight / 2, (float)kScreenHeight / 2 });
	window->draw(aim);
}