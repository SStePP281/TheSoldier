#include "UIManeger.h"
#include <sstream>
#include <string>
#include <iomanip>
#include <SFML/Graphics.hpp>

UIManager::UIManager(sf::RenderWindow* _window) : 
	window{ _window }, choseBut{ nullptr }, keyButton{ -1 } {}

std::wstring UIManager::splitText(std::wstring text, int maxLen, int textSize)
{
	std::wstring result, word, curLine;
	std::wistringstream stream(text);

	while (stream >> word)
	{
		sf::Text tempText;
		tempText.setFont(Resources::UIFont);
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

std::wstring UIManager::toMax(std::wstring str, float maxW, float textSize)
{
	sf::Text text(L"", Resources::UIFont, (int)textSize);
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

void UIManager::initDialog(std::map<int, std::wstring, std::greater<int>>& variants,
	std::wstring& npcName)
{
	background = sf::Sprite(Resources::dialogBackground);
	background.setScale({ (float)SCREEN_W / Resources::dialogBackground.getSize().x,
		(float)SCREEN_H / Resources::dialogBackground.getSize().y });

	sf::RectangleShape nameBase{ {DIALOG_W, TEXTSIZE + 10} };
	nameBase.setFillColor(sf::Color(100, 100, 100));
	sf::Text nameText(npcName, Resources::UIFont, TEXTSIZE);
	Group g(nameBase, nameText);
	g.setPosition({ (float)SCREEN_W / 2, INTERVAL});
	buttons.push_back(g);

	sf::RectangleShape dataBase{ {DIALOG_W, DIALOG_H / 1.5f} };
	dataBase.setFillColor(sf::Color(100, 100, 100));
	sf::Text dataText(splitText(variants[-1], (int)DIALOG_W, 40), Resources::UIFont, TEXTSIZE - 10);
	Group g1(dataBase, dataText);
	g1.setPosition({ (float)SCREEN_W / 2, g.getPosition().y + g1.getSize().y / 2 + INTERVAL});
	buttons.push_back(g1);

	sf::Vector2f pos((float)SCREEN_W / 2, g1.getPosition().y + g1.getSize().y / 2 + INTERVAL);
	
	for (auto var : variants)
	{
		if (var.first == -1) continue;
		sf::Text text(var.second, Resources::UIFont, TEXTSIZE);
		sf::RectangleShape rect({ DIALOG_W, TEXTSIZE + 10 });
		rect.setFillColor(sf::Color(50, 50, 50));
		Group g2(rect, text);
		g2.setPosition(pos);
		
		buttons.push_back(Button(g2));
		buttons.back().setFunc([=]() { keyButton = var.first;});

		pos.y +=  INTERVAL + g2.shape.getSize().y / 2;
	}
}

void UIManager::initResetMenu()
{
	background = sf::Sprite(Resources::menuBackground);
	background.setScale({ (float)SCREEN_W / Resources::menuBackground.getSize().x,
		(float)SCREEN_H / Resources::menuBackground.getSize().y });

	sf::Text text(L"ПРОДОЛЖИТЬ ИГРУ", Resources::UIFont, 50);
	sf::RectangleShape shape({ text.getLocalBounds().width + 20.0f, 60.0f });
	shape.setFillColor(sf::Color(100, 100, 100));
	Button button(Group(shape, text));
	button.setPosition({ (float)SCREEN_W / 2.0f, 2.0f * (float)SCREEN_H / 3.0f - 35.0f });
	button.setFunc([=]() { keyButton = 0;});
	buttons.push_back(button);

	button.setString(L"ВЫХОД");
	button.setPosition({ (float)SCREEN_W / 2.0f, 2.0f * (float)SCREEN_H / 3.0f + 35.0f });
	button.setFunc([=]() { keyButton = 1;});
	buttons.push_back(button);
}

void UIManager::initStartMenu()
{
	background = sf::Sprite(Resources::menuBackground);
	background.setScale({ (float)SCREEN_W / Resources::menuBackground.getSize().x,
		(float)SCREEN_H / Resources::menuBackground.getSize().y });
	
	sf::Text text(L"ПРОДОЛЖИТЬ ИГРУ", Resources::UIFont, 50);
	sf::RectangleShape shape( {text.getLocalBounds().width + 20.0f, 60.0f});
	shape.setFillColor(sf::Color(100, 100, 100));
	Button button(Group(shape, text));
	button.setPosition({ (float)SCREEN_W / 2.0f, 2.0f * (float)SCREEN_H / 3.0f - 35.0f });
	button.setFunc([=]() { keyButton = 0;});
	auto& state = GameState::getInstance();
	if (!state.data.isFirstGame) buttons.push_back(button);

	button.setString(L"НОВАЯ ИГРА");
	button.setPosition({ (float)SCREEN_W / 2.0f, 2.0f * (float)SCREEN_H / 3.0f + 35.0f });
	button.setFunc([=]() { keyButton = 1;});
	buttons.push_back(button);

	button.setString(L"ВЫХОД");
	button.setPosition({ (float)SCREEN_W / 2.0f, 2.0f * (float)SCREEN_H / 3.0f + 105.0f });
	button.setFunc([=]() { keyButton = 2;});
	buttons.push_back(button);
}

void UIManager::initGameMenu()
{
	background = sf::Sprite(Resources::inventoryBackground);
	background.setScale({ (float)SCREEN_W / Resources::inventoryBackground.getSize().x,
		(float)SCREEN_H / Resources::inventoryBackground.getSize().y });

	sf::Text text(L"ПРОДОЛЖИТЬ", Resources::UIFont, 50);
	sf::RectangleShape shape({ text.getLocalBounds().width + 20.0f, 60.0f });
	shape.setFillColor(sf::Color(100, 100, 100));
	Button button(Group(shape, text));
	button.setPosition({ (float)SCREEN_W / 2.0f, 2.0f * (float)SCREEN_H / 3.0f - 35.0f });
	button.setFunc([=]() { keyButton = 0; });
	buttons.push_back(button);

	button.setString(L"НАСТРОЙКИ");
	button.setPosition({ (float)SCREEN_W / 2.0f, 2.0f * (float)SCREEN_H / 3.0f + 35.0f });
	button.setFunc([=]() { keyButton = 1;});
	buttons.push_back(button);

	button.setString(L"ВЫХОД");
	button.setPosition({ (float)SCREEN_W / 2.0f, 2.0f * (float)SCREEN_H / 3.0f + 105.0f });
	button.setFunc([=]() { keyButton = 2;});
	buttons.push_back(button);
}

void UIManager::initSetting()
{
	background = sf::Sprite(Resources::inventoryBackground);
	background.setScale({ (float)SCREEN_W / Resources::inventoryBackground.getSize().x,
		(float)SCREEN_H / Resources::inventoryBackground.getSize().y });

	sf::Text text(L"ВЫХОД", Resources::UIFont, 50);
	sf::RectangleShape shape({ text.getLocalBounds().width + 20.0f, 60.0f});
	shape.setFillColor(sf::Color(100, 100, 100));
	Button button(Group(shape, text));
	button.setPosition({ (float)SCREEN_W / 2.0f, 80.0f });
	button.setFunc([=]() { keyButton = 0; });
	buttons.push_back(button);

	text.setString(L"+");
	sf::RectangleShape funcShape({ 60.0f, 60.0f });
	funcShape.setFillColor(sf::Color(50, 50, 50));
	Button funcButton(Group(funcShape, text));
	
	auto& state = GameState::getInstance();

	text.setString(L"ГРОМКОСТЬ ЕФФЕКТОВ: " + std::to_wstring(state.data.effectVolume));
	shape = sf::RectangleShape({ text.getLocalBounds().width + 20.0f, 60.0f });
	shape.setFillColor(sf::Color(100, 100, 100));
	button = Button(Group(shape, text));
	button.setPosition({ (float)SCREEN_W / 2.0f, 150.0f});
	buttons.push_back(button);
	funcButton.setPosition({ button.getPosition().x + button.getSize().x / 2 + 30.0f, button.getPosition().y });
	funcButton.setFunc([&]() {state.data.effectVolume++;keyButton = 1;});
	buttons.push_back(funcButton);
	funcButton.setPosition({ button.getPosition().x - button.getSize().x / 2 - 30.0f, button.getPosition().y });
	funcButton.setFunc([&]() {state.data.effectVolume--;keyButton = 1;});
	funcButton.setString(L"-");
	buttons.push_back(funcButton);

	button.setString(L"ГРОМКОСТЬ МУЗЫКИ: " + std::to_wstring(state.data.soundVolume));
	button.setPosition({ (float)SCREEN_W / 2.0f, 220.0f});
	buttons.push_back(button);
	funcButton.setString(L"+");
	funcButton.setPosition({ button.getPosition().x + button.getSize().x / 2 + 30.0f, button.getPosition().y });
	funcButton.setFunc([&]() {state.data.soundVolume++;keyButton = 1;});
	buttons.push_back(funcButton);
	funcButton.setPosition({ button.getPosition().x - button.getSize().x / 2 - 30.0f, button.getPosition().y });
	funcButton.setFunc([&]() {state.data.soundVolume--; keyButton = 1;});
	funcButton.setString(L"-");
	buttons.push_back(funcButton);

	std::wstring std = std::to_wstring(state.data.mouseSpeed + 1.0f);
	button.setString(L"CКОРОСТЬ МЫШИ: " + std.substr(0, std.find(L".") + 3));
	button.setPosition({ (float)SCREEN_W / 2.0f, 290.0f });
	buttons.push_back(button);
	funcButton.setString(L"+");
	funcButton.setPosition({ button.getPosition().x + button.getSize().x / 2 + 30.0f, button.getPosition().y });
	funcButton.setFunc([&]() {state.data.mouseSpeed += 0.01f; keyButton = 2;});
	buttons.push_back(funcButton);
	funcButton.setPosition({ button.getPosition().x - button.getSize().x / 2 - 30.0f, button.getPosition().y });
	funcButton.setFunc([&]() {state.data.mouseSpeed -= 0.01f; keyButton = 2;});
	funcButton.setString(L"-");
	buttons.push_back(funcButton);
	
	funcButton.setFunc(nullptr);
	shape = sf::RectangleShape({shape.getSize().x / 2, 30.0f});
	shape.setFillColor(sf::Color(100, 100, 100));
	text = sf::Text(L"", Resources::UIFont, 20);
	button = Button(Group(shape, text));
	funcShape.setSize({ 30.0f, 30.0f });
	funcButton = Button(Group(funcShape, text));
	sf::Vector2f pos{ (float)SCREEN_W / 2.0f - shape.getSize().x / 2 - 15.0f, 350.0f };

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
		button.setString(keys[i].second);
		funcButton.setString(keys[i].first);
		button.setPosition(pos);
		funcButton.setPosition({ pos.x - button.getSize().x / 2 - funcShape.getSize().x / 2, pos.y });
		buttons.push_back(button);
		buttons.push_back(funcButton);

		if (i % 2 == 0)
		{
			pos.x += shape.getSize().x + 15.0f + funcButton.getSize().x; 
		}
		else
		{
			pos.x -= shape.getSize().x + 15.0f + funcButton.getSize().x;
			pos.y += 40.0f;
		}
	}
}

void UIManager::initQuest(Quest* quest, Player* player)
{
	background = sf::Sprite(Resources::tradeBackground);
	background.setScale({ (float)SCREEN_W / Resources::tradeBackground.getSize().x,
		(float)SCREEN_H / Resources::tradeBackground.getSize().y });

	sf::Text text(L"Баланс: " + std::to_wstring(player->money) + L" | Запчасти: " + std::to_wstring(player->details), Resources::UIFont, 50);
	sf::RectangleShape shape({ text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f});
	shape.setFillColor(sf::Color(70, 70, 70));
	shape.setPosition({ (float)SCREEN_W / 2, shape.getSize().y / 2 + 10.0f });
	Button button(Group(shape, text));
	buttons.push_back(button);

	text.setString(L"Взять новый квест");
	shape.setSize({text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f});
	Button newQuest(Group(shape, text));
	newQuest.setFunc([=]() {keyButton = -300; });

	text.setCharacterSize(30);
	shape.setSize({DIALOG_W / 3, DIALOG_H / 3 - 20.0f});
	shape.setPosition({shape.getSize().x, shape.getSize().y / 2 + 90.0f});
	Button totalQuest(Group(shape, text));

	auto& questM = QuestManager::getInstance();
	auto total = questM.quests;
	std::wstringstream oss;
	for (size_t i = 0; i < total.size(); i++)
	{
		if (total[i]) // квест есть
		{
			if (total[i]->data.type == KillMonster)
			{
				oss << L"Охотник на монстров";
				oss << L"\n";
				oss << L"Убей " << total[i]->data.target << L" монстров";
			}
			else if (total[i]->data.type == CollectionMoney)
			{
				oss << L"Капиталист";
				oss << L"\n";
				oss << L"Заработай " << total[i]->data.target << L" монет";
			}
			else if (total[i]->data.type == CollectionDetails)
			{
				oss << L"Сбор запчастей";
				oss << L"\n";
				oss << L"Собери " << total[i]->data.target << L" запчастей";
			}

			oss << L"\n";
			oss << L"Статус: " << (total[i]->isCompleted() ? L"Завершен" : L"В процессе");
			oss << L"\n";
			oss << L"Награда: " << total[i]->data.rewardCoins << L" монет";
			oss << L"\n";
			oss << L"Прогресс: " << total[i]->data.progress << L" / " << total[i]->data.target;

			totalQuest.setString(oss.str());
			totalQuest.setFunc([=]() {keyButton = (int)(i + 1);});

			if (total[i] == quest)
			{
				totalQuest.setFillColor(sf::Color::Red);
			}

			buttons.push_back(totalQuest);
			totalQuest.setFillColor(sf::Color(70, 70, 70));
			oss.clear();
			oss.str(L"");
		}
		else // квеста нет
		{
			newQuest.setPosition(totalQuest.getPosition());
			buttons.push_back(newQuest);
		}

		totalQuest.move({0, totalQuest.getSize().y + 5.0f});
	}

	if (quest)
	{
		Button dataButton(Group(shape, text));
		if (quest->isCompleted())
		{
			dataButton.setString(L"Получить награду");
			dataButton.setFunc([=]() {keyButton = -200;});
		}
		else
		{
			dataButton.setString(L"Задание не выполнено");
		}

		dataButton.setSize({dataButton.group.text.getLocalBounds().width + 10.0f, dataButton.group.text.getLocalBounds().height + 10.0f});
		dataButton.setPosition({ totalQuest.getPosition().x + totalQuest.getSize().x / 2 + dataButton.getSize().x / 2 + 10.0f , SCREEN_H / 2});
		buttons.push_back(dataButton);
	}

	text.setCharacterSize(50);
	text.setString(L"В\nЫ\nХ\nО\nД");
	shape.setSize({ text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f });
	shape.setPosition({ (float)SCREEN_W - shape.getSize().x - 5.0f, (float)SCREEN_H / 2 });
	button = Button(Group(shape, text));
	button.setFunc([=]() { keyButton = -100; });
	buttons.push_back(button);
}

void UIManager::initTrade(std::map<int, Itemble*>& variants, Player* player)
{
	background = sf::Sprite(Resources::tradeBackground);
	background.setScale({ (float)SCREEN_W / Resources::tradeBackground.getSize().x,
		(float)SCREEN_H / Resources::tradeBackground.getSize().y });

	float interval = 5.0f;
	float size = 44.0f;

	float maxName = 0.0f, maxDisc = 0.0f, maxCost = 0.0f;
	sf::Text text(L"", Resources::UIFont, (int)size - 25);
	
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
	
	sf::Vector2f pos((float)SCREEN_W / 2, size + interval * 2.0f);
	sf::Vector2f textPos(((float)SCREEN_W - DIALOG_W) / 2 - ICON_SIZE / 2, size + interval * 2.0f);

	sf::RectangleShape rect({ DIALOG_W, size });
	sf::RectangleShape texture({ ICON_SIZE, ICON_SIZE });
	sf::RectangleShape texture1({ ICON_SIZE, size });
	Button b5({texture, {}});
	Button b6({texture1, {}});
	b5.setTexture(&Resources::itembleIcon);
	b6.setFillColor(sf::Color(70,70,70));

	rect.setFillColor(sf::Color(50, 50, 50));
	Group g2(rect, text);

	for (auto var : variants)
	{
		if (!var.second) continue;

		std::wstring line = toMax(var.second->name, maxName, size - 25) + L" | " + 
			toMax(std::to_wstring(var.second->cost) + L" руб", maxCost, size - 25) + L" | " +
			toMax(var.second->disc, maxDisc, size - 25);

		g2.setString(line);
		g2.setPosition(pos);

		b6.setPosition(textPos);
		b5.setPosition(textPos);
		b5.setTextureRect({ {ICON_SIZE * var.second->id, 0}, {ICON_SIZE, ICON_SIZE} });

		buttons.push_back(Button(g2));
		buttons.back().setFunc([=]() { keyButton = var.first;});
		buttons.push_back(b6);
		buttons.push_back(b5);

		pos.y += interval + g2.shape.getSize().y;
		textPos.y = pos.y;
	}

	sf::RectangleShape rect2(rect);
	rect2.setSize({ DIALOG_W, size - 20.0f });
	Group g9(rect2, text);
	g9.setString(L"Баланс: " + std::to_wstring(player->money) + L" | Запчасти: " + std::to_wstring(player->details));
	g9.setPosition({ (float)SCREEN_W / 2, g9.getSize().y / 2 + interval});
	buttons.push_back(g9);

	sf::Text text1(text);
	text1.setString(L"К\nУ\nП\nИ\nТ\nЬ");
	text1.setCharacterSize(50);
	sf::RectangleShape rect1({ size + 10.0f, 350.0f });
	rect1.setFillColor(sf::Color(70, 70, 70));
	Group g3(rect1, text1);
	g3.setPosition({ ((float)SCREEN_W + DIALOG_W) / 2 + g3.getSize().x, g3.getSize().y / 2 + 5.0f});

	buttons.push_back(Button(g3));
	buttons.back().setFunc([=]() { keyButton = -200; });

	g3.setString(L"В\nЫ\nХ\nО\nД");
	g3.setPosition({ ((float)SCREEN_W + DIALOG_W) / 2 + g3.getSize().x , (float)SCREEN_H - g3.getSize().y/2 - 5.0f});
	buttons.push_back(Button(g3));
	buttons.back().setFunc([=]() { keyButton = -100; });
}

void UIManager::initMechanic(Player* player, Gun* choose)
{
	sf::Text text(L"Баланс: " + std::to_wstring(player->money) + L" | Запчасти: " + std::to_wstring(player->details) + L" | Одно улучшение стоит 50 руб и 15 деталей", Resources::UIFont, 30);
	sf::RectangleShape shape({ DIALOG_W, text.getLocalBounds().height + 5.0f});
	shape.setFillColor(sf::Color(70, 70, 70));
	shape.setPosition({ (float)SCREEN_W / 2, shape.getSize().y / 2 + 5.0f });
	Button button(Group(shape, text));
	buttons.push_back(button);

	sf::RectangleShape dataShape({ DIALOG_W / 4 + 15, DIALOG_H / 2 });
	dataShape.setFillColor(sf::Color(50, 50, 50));
	Group dataGroup(dataShape, {});
	dataGroup.setPosition({ shape.getPosition().x - shape.getSize().x / 2 + dataShape.getSize().x / 2, shape.getPosition().y / 2 + INTERVAL + dataGroup.getSize().y / 2});

	sf::RectangleShape textureShape({ ICON_SIZE, ICON_SIZE });
	textureShape.setScale({ 2, 2 });
	Group textureGroup(textureShape, {});
	
	std::wostringstream oss;
	for (size_t i = 1; i <= 2; i++)
	{
		if (player->guns[i])
		{
			if (player->guns[i] == choose) dataGroup.shape.setFillColor(sf::Color::Red);
			buttons.push_back(dataGroup);
			buttons.back().setFunc([=]() { keyButton = (int)i; });

			textureGroup.setPosition({ dataGroup.getPosition().x, dataGroup.getPosition().y - dataGroup.getSize().y / 2 + ICON_SIZE + 20.0f });
			buttons.push_back(Button(textureGroup));
			buttons.back().setTexture(&Resources::itembleIcon);
			buttons.back().setTextureRect({ {ICON_SIZE * player->guns[i]->id, 0},{ICON_SIZE, ICON_SIZE} });

			oss << L"Урон: " << std::fixed << std::setprecision(2) << player->guns[i]->damage << "\n";
			oss << L"Обойма: " << std::fixed << std::setprecision(2) << player->guns[i]->maxCount << "\n";
			oss << L"Разброс: " << std::fixed << std::setprecision(2) << player->guns[i]->maxImpRad << "\n";
			oss << L"Количество улучшений: " << player->guns[i]->upgradeCount << L"/ 5" << "\n";

			text.setCharacterSize(20);
			text.setString(oss.str());
			shape.setSize({ text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f });
			shape.setPosition({ textureGroup.getPosition().x, textureGroup.getPosition().y + textureGroup.getSize().y + 60.0f });
			buttons.push_back(Group(shape, text));

			oss.clear();
			oss.str(L"");
			dataGroup.move({ 0, dataGroup.getSize().y + 10.0f });
			dataGroup.shape.setFillColor(sf::Color(50, 50, 50));
		}
	}
	text.setCharacterSize(30);

	text.setString(L"У\nЛ\nУ\nЧ\nШ\nИ\nТ\nЬ");
	shape.setSize({ text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f });
	shape.setPosition({ ((float)SCREEN_W + DIALOG_W) / 2 + shape.getSize().x, (float)SCREEN_H / 2 - shape.getSize().y / 2 - 5.0f });
	button = Button(Group(shape, text));
	button.setFunc([=]() { keyButton = -200; });
	buttons.push_back(button);

	text.setString(L"В\nЫ\nХ\nО\nД");
	shape.setPosition({ ((float)SCREEN_W + DIALOG_W) / 2 + shape.getSize().x, (float)SCREEN_H / 2 + shape.getSize().y / 2 + 5.0f });
	button = Button(Group(shape, text));
	button.setFunc([=]() { keyButton = -100; });
	buttons.push_back(button);

	if (choose)
	{
		sf::RectangleShape percShape({ DIALOG_W / 2 + 15.0f, DIALOG_H / 4 });
		percShape.setPosition(dataGroup.getPosition().x + dataGroup.getSize().x / 2 + percShape.getSize().x / 2 + 10.0f, (float)SCREEN_H / 3 - percShape.getSize().y / 3 + 10.0f);
		percShape.setFillColor(sf::Color(50, 50, 50));
		Group percGroup(percShape, text);

		percGroup.setString(L"УВЕЛИЧИТЬ УРОН НА +3");
		buttons.push_back(percGroup);
		buttons.back().setFunc([=]() {keyButton = 101;});
		percGroup.move({0.0f, percGroup.getSize().y + 10.0f});

		percGroup.setString(L"УВЕЛИЧИТЬ РАЗМЕР ОБОЙМЫ НА +5");
		buttons.push_back(percGroup);
		buttons.back().setFunc([=]() {keyButton = 102;});
		percGroup.move({ 0.0f, percGroup.getSize().y + 10.0f });

		percGroup.setString(L"УМЕНЬШИТЬ РАЗБРОС НА +2");
		buttons.push_back(percGroup);
		buttons.back().setFunc([=]() {keyButton = 103;});
	}
}

void UIManager::initChanger(int coef, Player* player)
{
	background = sf::Sprite(Resources::tradeBackground);
	background.setScale({ (float)SCREEN_W / Resources::tradeBackground.getSize().x,
		(float)SCREEN_H / Resources::tradeBackground.getSize().y });

	sf::Text text(L"Баланс: " + std::to_wstring(player->money) + L" | Запчасти: " + std::to_wstring(player->details), Resources::UIFont, 50);
	sf::RectangleShape shape({ text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f });
	shape.setFillColor(sf::Color(70, 70, 70));
	shape.setPosition({ (float)SCREEN_W / 2, shape.getSize().y + 10.0f });
	Button button(Group(shape, text));
	buttons.push_back(button);

	text.setString(L"Текущий курс: 1 деталь к " + std::to_wstring(coef) + L" монетам");
	shape.setSize({ text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f });
	shape.move({ 0, shape.getSize().y + 10.0f });
	button = Button(Group(shape, text));
	buttons.push_back(button);

	text.setString(L"Производится обмен 10 деталей сразу");
	shape.setSize({ text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f });
	shape.move({ 0, shape.getSize().y + 5.0f });
	button = Button(Group(shape, text));
	buttons.push_back(button);

	text.setString(L"О\nБ\nМ\nЕ\nН\nЯ\nТ\nЬ");
	shape.setSize({ text.getLocalBounds().width + 10.0f, text.getLocalBounds().height + 10.0f });
	shape.setPosition({ shape.getSize().x + 5.0f, (float)SCREEN_H / 2});
	button = Button(Group(shape, text));
	button.setFunc([=]() { keyButton = -200; });
	buttons.push_back(button);

	text.setString(L"В\nЫ\nХ\nО\nД");
	shape.setPosition({ (float)SCREEN_W - shape.getSize().x - 5.0f, (float)SCREEN_H / 2 });
	button = Button(Group(shape, text));
	button.setFunc([=]() { keyButton = -100; });
	buttons.push_back(button);
}

void UIManager::initInvent(std::map<Itemble*, int>& items, Itemble* choose, Player* player)
{
	background = sf::Sprite(Resources::inventoryBackground);
	background.setScale({ (float)SCREEN_W / Resources::inventoryBackground.getSize().x,
		(float)SCREEN_H / Resources::inventoryBackground.getSize().y });

	sf::RectangleShape baseShape({ DIALOG_W / 2 + 15, DIALOG_H + 10});
	baseShape.setFillColor(sf::Color(70, 70, 70));
	Group baseGroup(baseShape, {});
	baseGroup.setPosition({ INTERVAL + baseShape.getSize().x / 2 - 5,
							INTERVAL + baseShape.getSize().y / 2 - 5});
	buttons.push_back(baseGroup);
	
	sf::RectangleShape invShape({ DIALOG_W / 4, ICON_SIZE / 2  });
	invShape.setFillColor(sf::Color(50, 50, 50));
	sf::Text invText(L"", Resources::UIFont, ICON_SIZE / 2 - 20);
	Group invGroup(invShape, invText);
	Button invBut(invGroup);

	sf::Vector2f pos{ DIALOG_W / 8 + INTERVAL, INTERVAL + invGroup.getSize().y / 2 };
	int i = 0;

	for (auto pair : items)
	{
		invBut.setPosition(pos);
		invBut.setString(pair.first->name + L" | " + std::to_wstring(pair.second) + L" шт");
		invBut.setFunc([=]() { keyButton = pair.first->id;});

		buttons.push_back(invBut);

		if (i % 2 == 0)
		{
			pos.x = DIALOG_W / 8 + invGroup.getSize().x + INTERVAL + 5;
		}
		else
		{
			pos.y += invGroup.getSize().y + 5;
			pos.x = DIALOG_W / 8 + INTERVAL;
		}
		i++;
	}

	if (choose)
	{
		sf::RectangleShape dataShape({ DIALOG_W / 4 + 15, DIALOG_H / 3 });
		dataShape.setFillColor(sf::Color(70, 70, 70));
		Group dataGroup(dataShape, {});
		dataGroup.setPosition({ baseGroup.getPosition().x, INTERVAL + dataGroup.getSize().y / 2 });
		dataGroup.move({ dataGroup.getSize().x / 2 + baseGroup.getSize().x / 2 + INTERVAL, 0 });
		buttons.push_back(dataGroup);

		sf::RectangleShape textureShape({ ICON_SIZE, ICON_SIZE });
		textureShape.setScale({ 2, 2 });
		Group textureGroup(textureShape, {});
		textureGroup.setPosition({ dataGroup.getPosition().x, INTERVAL + ICON_SIZE / 2 + 10 });
		buttons.push_back(Button(textureGroup));
		buttons.back().setTexture(&Resources::itembleIcon);
		buttons.back().setTextureRect({ {ICON_SIZE * choose->id, 0},{ICON_SIZE, ICON_SIZE} });

		Group dataGroup1(invGroup);
		dataGroup1.setPosition(textureGroup.getPosition());
		dataGroup1.move({ 0, textureGroup.getSize().y / 2 + dataGroup1.getSize().y + 5 });
		dataGroup1.setString(choose->name);
		buttons.push_back(dataGroup1);

		float oldSize = dataGroup1.getSize().y;
		dataGroup1.setSize({ dataGroup1.getSize().x, dataGroup1.getSize().y * 2 });
		dataGroup1.move({ 0, oldSize / 2 + dataGroup1.getSize().y / 2 + 5 });

		oldSize = dataGroup.getSize().y;
		dataGroup.setSize({ dataGroup.getSize().x, DIALOG_H * 2 / 3 - INTERVAL });
		dataGroup.move({ 0, oldSize / 2 + dataGroup.getSize().y / 2 + INTERVAL });
		buttons.push_back(Button(dataGroup));

		Group makeGroup(invGroup);
		sf::Vector2f pos = dataGroup.getPosition();
		pos.y -= dataGroup.getSize().y / 2 - 5 - makeGroup.getSize().y / 2;
		makeGroup.setPosition(pos);

		std::wostringstream oss;
		if (auto item = dynamic_cast<Item*>(choose); item)
		{
			makeGroup.setString(L"Использовать");
			buttons.push_back(Button(makeGroup));
			buttons.back().setFunc([&]() { keyButton = 100;});

			oss << choose->disc;
		}
		else if (auto gun = dynamic_cast<Gun*>(choose); gun)
		{
			if (player->guns[1] != nullptr)
			{
				makeGroup.setString(L"Надеть вместо " + player->guns[1]->name);
			}
			else
			{
				makeGroup.setString(L"Надеть на первый слот");
			}
			buttons.push_back(makeGroup);
			buttons.back().setFunc([&]() { keyButton = 100;});
			makeGroup.move({ 0, makeGroup.getSize().y + 5 });

			if (player->guns[2] != nullptr)
			{
				makeGroup.setString(L"Надеть вместо " + player->guns[2]->name);
			}
			else
			{
				makeGroup.setString(L"Надеть на второй слот");
			}
			buttons.push_back(makeGroup);
			buttons.back().setFunc([&]() { keyButton = 101;});

			int i = 102;
			for (auto it : gun->improvement)
			{
				if (!it.second) continue;

				makeGroup.move({ 0, makeGroup.getSize().y + 5 });
				makeGroup.setString(L"Снять " + it.second->name);
				buttons.push_back(Button(makeGroup));
				buttons.back().setFunc([=]() { keyButton = i;});
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
				makeGroup.setString(L"Надеть на " + player->guns[1]->name);
				buttons.push_back(Button(makeGroup));
				buttons.back().setFunc([&]() { keyButton = 100;});
				makeGroup.move({ 0, makeGroup.getSize().y + 5 });
			}

			if (player->guns[2] != nullptr)
			{
				makeGroup.setString(L"Надеть на " + player->guns[2]->name);
				buttons.push_back(Button(makeGroup));
				buttons.back().setFunc([&]() { keyButton = 101;});
			}

			oss << choose->disc;
		}

		dataGroup1.setString(splitText(oss.str(), (int)dataGroup1.getSize().x, dataGroup1.text.getCharacterSize()));
		buttons.push_back(dataGroup1);
	}
}

void UIManager::deleteNow() 
{ 
	buttons.clear();
	choseBut = nullptr;
}

int UIManager::checkButton()
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
	sf::Vector2i worldPos = (sf::Vector2i)window->mapPixelToCoords(mousePos);

	for (size_t i = 0; i < buttons.size(); i++)
	{
		if (buttons[i].isClicked(worldPos))
		{
			if (choseBut) choseBut->setFillColor(sf::Color(70, 70, 70));
			choseBut = &buttons[i];
			choseBut->setFillColor(sf::Color::Red);
			buttons[i].use();
			return keyButton;
		}
	}
	return -1;
}

void UIManager::drawNow()
{
	window->draw(background);
	for (auto b : buttons)
	{
		window->draw(b);
	}
}

void UIManager::drawPlayerUI(Player* player)
{
	sf::Text weaponInfo;

	if (!player->kick->isCanUsed())
	{
		sf::Vector2f deltaPos{};
		player->kick->drawWeapon(window, deltaPos);
	}
	else
	{
		auto gun = player->getNowGun();
		gun->drawWeapon(window, player->shakeDelta);

		if (gun->isReset)
		{
			weaponInfo = sf::Text(std::to_string(player->patrons), Resources::UIFont, 30);
			auto b = weaponInfo.getLocalBounds();
			weaponInfo.setOrigin({ b.width / 2, b.height / 2 });
			weaponInfo.setPosition({ (float)SCREEN_W / 2, (float)SCREEN_H / 2 - weaponInfo.getCharacterSize() / 4 });
			weaponInfo.setPosition({ (float)SCREEN_W - b.width / 2 - 20, (float)SCREEN_H - 30 });
			weaponInfo.setFillColor({ 0, 0, 0 });
			window->draw(weaponInfo);

			weaponInfo.setString(std::to_string(gun->nowCount) + " / " + std::to_string(gun->maxCount));
			b = weaponInfo.getLocalBounds();
			weaponInfo.setOrigin({ b.width / 2, b.height / 2 });
			weaponInfo.setPosition({ (float)SCREEN_W / 2, (float)SCREEN_H / 2 - weaponInfo.getCharacterSize() / 4 });
			weaponInfo.setPosition({ (float)SCREEN_W - b.width / 2 - 20, (float)SCREEN_H - 60 });
			window->draw(weaponInfo);
		}
	}
	float baseX = 300;
	sf::RectangleShape baseShape({ baseX, 40 });
	baseShape.setFillColor(sf::Color(128, 128, 128));
	sf::Text text("", Resources::UIFont, 30);
	Group group1(baseShape, text);
	group1.setPosition({ 170, (float)SCREEN_H - 120 });
	window->draw(group1.shape);

	Group group2(group1);
	group2.setPosition({ group2.getPosition().x, group2.getPosition().y + 40 });
	window->draw(group2.shape);

	Group group3(group1);
	group3.setPosition({ group3.getPosition().x, group3.getPosition().y + 80 });
	window->draw(group3.shape);

	std::wostringstream oss;
	oss << std::fixed << std::setprecision(2) << player->enemy->spMap.nowHealPoint;
	oss << " / ";
	oss << std::fixed << std::setprecision(2) << player->enemy->enemyDef.maxHealpoint;
	std::wstring str = oss.str();

	group1.shape.setFillColor(sf::Color(255, 23, 23));
	float newXH = baseX * (player->enemy->spMap.nowHealPoint <= 0 ? 0 :
		player->enemy->spMap.nowHealPoint) / player->enemy->enemyDef.maxHealpoint;
	group1.shape.setSize({ newXH, 40 });
	group1.setString(str);
	window->draw(group1.shape);
	window->draw(group1.text);

	oss.str(L"");
	oss.clear();
	oss << std::fixed << std::setprecision(2) << player->nowStrenght;
	oss << " / ";
	oss << std::fixed << std::setprecision(2) << player->maxStrenght;
	str = oss.str();

	group2.shape.setFillColor(sf::Color(70, 130, 80));
	float newXD = baseX * player->nowStrenght / player->maxStrenght;
	group2.shape.setSize({ newXD, 40 });
	group2.setString(str);
	window->draw(group2.shape);
	window->draw(group2.text);

	oss.str(L"");
	oss.clear();
	oss << std::fixed << std::setprecision(2) << player->nowEnergy;
	oss << " / ";
	oss << std::fixed << std::setprecision(2) << player->maxEnergy;
	str = oss.str();

	group3.shape.setFillColor(sf::Color(44, 148, 15));
	float newXB = baseX * player->nowEnergy / player->maxEnergy;
	group3.shape.setSize({ newXB, 40 });
	group3.setString(str);
	window->draw(group3.shape);
	window->draw(group3.text);

	if (player->nowHeal)
	{
		sf::RectangleShape rect({ ICON_SIZE, ICON_SIZE });
		rect.setTexture(&Resources::itembleIcon);
		rect.setTextureRect({ {player->nowHeal->id * ICON_SIZE, 0},{ICON_SIZE, ICON_SIZE} });
		auto b = rect.getLocalBounds();
		rect.setOrigin({ b.width / 2, b.height / 2 });
		if (weaponInfo.getString() != "")
		{
			rect.setPosition({ (float)SCREEN_W - ICON_SIZE - weaponInfo.getLocalBounds().width,  (float)SCREEN_H - ICON_SIZE / 2 });
		}
		else
		{
			rect.setPosition({ (float)SCREEN_W - b.width / 2 - 20, (float)SCREEN_H - ICON_SIZE / 2 });
		}
		window->draw(rect);
	}

	sf::CircleShape aim(player->getNowGun()->nowRad, 16);
	aim.setOrigin({ aim.getRadius(), aim.getRadius() });
	aim.setFillColor(sf::Color(0, 0, 0, 0));
	aim.setOutlineColor(sf::Color::Black);
	aim.setOutlineThickness(1.5f);
	aim.setPosition({ (float)SCREEN_W / 2, (float)SCREEN_H / 2 });
	window->draw(aim);
}