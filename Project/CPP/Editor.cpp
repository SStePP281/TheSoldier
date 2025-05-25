#include "Editor.h"
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Event.hpp>
#include "SpriteManager.h"

void Editor::init(sf::RenderWindow* _window, sf::RenderWindow* _editorWindow, MapManager* _mapMn)
{
	mapManager = _mapMn;
	window = _window;
	editorWindow = _editorWindow;
	nowValue = 1;
	nowLayer = WALL_LAYER;
	windowView = window->getView();
	editorView = editorWindow->getView();

	createTextureButton();
	createSpriteButton();
}

void Editor::createTextureButton()
{
	sf::RectangleShape shape(sf::Vector2f{ TEXTURE_SIZE, TEXTURE_SIZE });
	shape.setScale({ 0.5f ,0.5f });
	Button b({ shape, {} });
	b.setTexture(&Resources::textures);

	int x = 0, y = 0;
	for (; x < (int)Resources::textures.getSize().x / TEXTURE_SIZE * 5; x++, y++)
	{
		b.setPosition({ (x % COUNT_ROW_TEXT) * (float)ICON_SIZE + ICON_SIZE / 2, 
						(y / COUNT_ROW_TEXT) * (float)ICON_SIZE });
		b.setTextureRect({ {(int)(x % TEXTURE_COUNT * TEXTURE_SIZE), 
							(int)(x / TEXTURE_COUNT * TEXTURE_SIZE)},
							{TEXTURE_SIZE, TEXTURE_SIZE} });

		b.setFunc([=]() { nowValue = x + 1; });

		buttons.push_back(std::make_shared<Button>(b));
	}
}

void Editor::createSpriteButton()
{
	int  y = (int)buttons.size() / COUNT_ROW_TEXT * COUNT_ROW_TEXT + COUNT_ROW_TEXT;

	sf::RectangleShape shape(sf::Vector2f{ ICON_SIZE, ICON_SIZE });
	Button b({ shape, {} });
	b.setTexture(&Resources::spriteIcon);

	for (int x = 0; x < spriteDefs.size() - 1; x++, y++)
	{
		b.setPosition({ (float)ICON_SIZE * (x % COUNT_ROW_TEXT) + ICON_SIZE / 2,
						y / COUNT_ROW_TEXT * (float)ICON_SIZE });
		b.setTextureRect({ { ICON_SIZE * x, 0}, {ICON_SIZE, ICON_SIZE} });

		b.setFunc([=]() { nowSpriteDef = spriteDefs[x + 1]; });
		buttons.push_back(std::make_shared<Button>(b));
	}
}

void Editor::takeEditInput(sf::Event& event)
{
	if (!editorWindow->hasFocus()) return;

	editorMousePos = sf::Mouse::getPosition(*editorWindow);

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		editorWindowStateLeftClick();
	}

	if (event.type == sf::Event::MouseWheelScrolled)
	{
		float deltaScrol = -5 * event.mouseWheelScroll.delta;
		editorView.move({ 0, deltaScrol });
	}

	editorWindow->setView(editorView);
}

void Editor::takeWindowInput(sf::Event& event)
{
	if (!window->hasFocus()) return;

	windowMousePos = sf::Mouse::getPosition(*window);

	if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
	{
		windowStateRightClick();
	}
	else
	{
		windowStateNoRightClick();	
	}


	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		windowStateLeftClick();
	}

	if (event.type == sf::Event::MouseWheelScrolled)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
		{
			scrollAndCntr(0.1f * event.mouseWheelScroll.delta);
		}
		else
		{
			windowView.zoom(1.0f - 0.1f * event.mouseWheelScroll.delta);
		}
	}

	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Tab)
		{
			nowLayer++;
			nowLayer = nowLayer % ALL_LAYER;
		}
	}

	window->setView(windowView);
}

void Editor::windowStateRightClick()
{
	if (isFirstMouse)
	{
		lastMousePos = windowMousePos;
		isFirstMouse = false;
	}
	else
	{
		sf::Vector2i deltaMouse = windowMousePos - lastMousePos;

		windowView.setCenter(windowView.getCenter() - (sf::Vector2f)deltaMouse);

		sf::Mouse::setPosition(lastMousePos, *window);
	}
	window->setMouseCursorVisible(false);
}

void Editor::windowStateNoRightClick()
{
	isFirstMouse = true;
	window->setMouseCursorVisible(true);
}

void Editor::scrollAndCntr(float delta)
{
	if (nowLayer != SPRITE_LAYER) return;

	sf::Vector2f worldPos = window->mapPixelToCoords(windowMousePos);
	sf::Vector2i mapPos = getMapPos(worldPos);

	if (mapManager->getNowMap()->isCellEmpty(mapPos)) return;
	mapManager->getNowMap()->rotateSprite(mapPos, delta * 10);
}

void Editor::windowStateLeftClick()
{
	sf::Vector2f worldPos = window->mapPixelToCoords(windowMousePos);
	sf::Vector2i mapPos = getMapPos(worldPos);

	if (nowLayer != SPRITE_LAYER)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
		{
			mapManager->getNowMap()->SetNewOnGrid(mapPos.x, mapPos.y, nowLayer, 0);
		}
		else
		{
			mapManager->getNowMap()->SetNewOnGrid(mapPos.x, mapPos.y, nowLayer, nowValue);
		}
	}
	else
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
		{
			mapManager->getNowMap()->deleteMapSprite(mapPos);
		}
		else
		{
			if (nowSpriteDef.texture != -1)
			{
				if (mapManager->getNowMap()->isCellEmpty(mapPos))
				{
					float nowHp = 10;

					if (nowSpriteDef.type != SpriteType::NPC && nowSpriteDef.type != SpriteType::Decoration)
					{
						nowHp = enemyDefs[nowSpriteDef.texture + 1].maxHealpoint;
					}

					MapSprite spMap = { nowSpriteDef.texture + 1, { mapPos.x + 0.5f, mapPos.y + 0.5f }, -90.0f, nowHp };
					mapManager->getNowMap()->setMapSprite(spMap);
				}
			}
		}
	}
}

void Editor::editorWindowStateLeftClick()
{
	sf::Vector2i worldPos = (sf::Vector2i)editorWindow->mapPixelToCoords(editorMousePos);

	for (auto b : buttons)
	{
		if (b->isClicked(worldPos))
		{
			b->use();
			break;
		}
	}
}

void Editor::drawEditor()
{
	for (auto b : buttons)
	{
		editorWindow->draw(*b.get());
	}
}

sf::Vector2i Editor::getMapPos(sf::Vector2f& worldPos)
{
	return sf::Vector2i((int)floor(worldPos.x - 0.1f) / TEXTURE_SIZE,
						(int)floor(worldPos.y - 0.1f) / TEXTURE_SIZE);
}

int Editor::drawerLayer() { return nowLayer; }