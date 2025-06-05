#include "Editor.h"
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Event.hpp>
#include "SpriteManager.h"

Editor::Editor(sf::RenderWindow* window, sf::RenderWindow* editor_window, MapManager* map_manager) :
	map_manager{ map_manager }, window{window}, editor_window{editor_window}, now_value{1}, now_layer{kWallLayer},
	window_view{window->getView()}, editor_view{editor_window->getView()}
{
	CreateTextureButton();
	CreateSpriteButton();
}

void Editor::CreateTextureButton()
{
	sf::RectangleShape shape(sf::Vector2f{ kTextureSize, kTextureSize });
	shape.setScale({ 0.5f ,0.5f });
	Group base_grop(shape, {});
	Button b(base_grop);
	b.SetTexture(&Resources::textures);

	int x = 0, y = 0;
	for (; x < (int)Resources::textures.getSize().x / kTextureSize * 5; x++, y++)
	{
		b.SetPosition({ (x % kCountRowTaxt) * (float)kIconSize + kIconSize / 2, 
						(y / kCountRowTaxt) * (float)kIconSize });
		b.SetTextureRect({ {(int)(x % kTextureCount * kTextureSize), 
							(int)(x / kTextureCount * kTextureSize)},
							{kTextureSize, kTextureSize} });

		b.SetFunc([=]() { now_value = x + 1; });

		buttons.push_back(std::make_shared<Button>(b));
	}
}

void Editor::CreateSpriteButton()
{
	int  y = (int)buttons.size() / kCountRowTaxt * kCountRowTaxt + kCountRowTaxt;

	sf::RectangleShape shape(sf::Vector2f{ kIconSize, kIconSize });
	Group base_group(shape, {});
	Button b(base_group);
	b.SetTexture(&Resources::sprite_icon);

	for (int x = 0; x < spriteDefs.size() - 1; x++, y++)
	{
		b.SetPosition({ (float)kIconSize * (x % kCountRowTaxt) + kIconSize / 2,
						y / kCountRowTaxt * (float)kIconSize });
		b.SetTextureRect({ { kIconSize * x, 0}, {kIconSize, kIconSize} });

		b.SetFunc([=]() { now_sprite_def = spriteDefs[x + 1]; });
		buttons.push_back(std::make_shared<Button>(b));
	}
}

void Editor::TakeEditInput(sf::Event& event)
{
	if (!editor_window->hasFocus()) return;

	editor_mouse_position = sf::Mouse::getPosition(*editor_window);

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		EditorWindowStateLeftClick();
	}

	if (event.type == sf::Event::MouseWheelScrolled)
	{
		float delta_scrol = -5 * event.mouseWheelScroll.delta;
		editor_view.move({ 0, delta_scrol });
	}

	editor_window->setView(editor_view);
}

void Editor::TakeWindowInput(sf::Event& event)
{
	if (!window->hasFocus()) return;

	window_mouse_position = sf::Mouse::getPosition(*window);

	if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
	{
		WindowStateRightClick();
	}
	else
	{
		WindowStateNoRightClick();	
	}


	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		WindowStateLeftClick();
	}

	if (event.type == sf::Event::MouseWheelScrolled)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
		{
			ScrollAndCntr(0.1f * event.mouseWheelScroll.delta);
		}
		else
		{
			window_view.zoom(1.0f - 0.1f * event.mouseWheelScroll.delta);
		}
	}

	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Tab)
		{
			now_layer++;
			now_layer = now_layer % kAllLayerCount;
		}
	}

	window->setView(window_view);
}

void Editor::WindowStateRightClick()
{
	if (is_first_mouse)
	{
		last_mouse_pos = window_mouse_position;
		is_first_mouse = false;
	}
	else
	{
		sf::Vector2i delta_mouse = window_mouse_position - last_mouse_pos;

		window_view.setCenter(window_view.getCenter() - (sf::Vector2f)delta_mouse);

		sf::Mouse::setPosition(last_mouse_pos, *window);
	}

	window->setMouseCursorVisible(false);
}

void Editor::WindowStateNoRightClick()
{
	is_first_mouse = true;
	window->setMouseCursorVisible(true);
}

void Editor::ScrollAndCntr(float delta)
{
	if (now_layer != kSpeiteLayer) return;

	sf::Vector2f world_position = window->mapPixelToCoords(window_mouse_position);
	sf::Vector2i mapPosition = GetMapPos(world_position);

	if (map_manager->GetNowMap()->IsCellEmpty(mapPosition)) return;
	map_manager->GetNowMap()->RotateSprite(mapPosition, delta * 10);
}

void Editor::WindowStateLeftClick()
{
	sf::Vector2f world_position = window->mapPixelToCoords(window_mouse_position);
	sf::Vector2i map_position = GetMapPos(world_position);

	if (now_layer != kSpeiteLayer)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
		{
			map_manager->GetNowMap()->SetNewOnGrid(map_position.x, map_position.y, now_layer, 0);
		}
		else
		{
			map_manager->GetNowMap()->SetNewOnGrid(map_position.x, map_position.y, now_layer, now_value);
		}
	}
	else
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
		{
			map_manager->GetNowMap()->DeleteMapSprite(map_position);
		}
		else
		{
			if (now_sprite_def.texture != -1)
			{
				if (map_manager->GetNowMap()->IsCellEmpty(map_position))
				{
					float now_healpoint = 10;

					if (now_sprite_def.type != SpriteType::NPC && now_sprite_def.type != SpriteType::Decoration)
					{
						now_healpoint = enemyDefs[now_sprite_def.texture + 1].maxHealpoint;
					}

					MapSprite map_sprite = { now_sprite_def.texture + 1, { map_position.x + 0.5f, map_position.y + 0.5f }, -90.0f, now_healpoint };
					map_manager->GetNowMap()->SetMapSprite(map_sprite);
				}
			}
		}
	}
}

void Editor::EditorWindowStateLeftClick()
{
	sf::Vector2i world_positon = (sf::Vector2i)editor_window->mapPixelToCoords(editor_mouse_position);

	for (auto b : buttons)
	{
		if (b->IsClicked(world_positon))
		{
			b->Use();
			break;
		}
	}
}

void Editor::DrawEditor()
{
	for (auto b : buttons)
	{
		editor_window->draw(*b.get());
	}
}

sf::Vector2i Editor::GetMapPos(const sf::Vector2f& world_position)
{
	return sf::Vector2i((int)floor(world_position.x - 0.1f) / kTextureSize,
						(int)floor(world_position.y - 0.1f) / kTextureSize);
}

int Editor::DrawerLayer() { return now_layer; }