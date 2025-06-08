#ifndef UIM
#define UIM

#include "player.h"
#include "event_system.h"
#include "data_base.h"
#include "sfml_extantion.h"
#include "weapon.h"
#include "const.h"
#include <sstream>
#include <string>
#include <iomanip>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Mouse.hpp>
#include <map>

class UIManager {
public:
	UIManager(sf::RenderWindow* window);
	void DrawPlayerUI(const Player* const player);
	void InitDialog(const std::map<int, std::wstring, std::greater<int>>& variants,
		const std::wstring& npc_name);
	void InitResetMenu();
	void InitStartMenu();
	void InitGameMenu();
	void InitSetting();
	void InitQuest(const Quest* const quest, const Player* const player);
	void InitTrade(const std::map<int, Itemble*>& variants, const Player* const player);
	void InitMechanic(const Player* const player, const Gun* const choose);
	void InitChanger(int coef, const Player* const player);
	void InitInvent(const std::map<Itemble*, int>& items, const Itemble* const choose, const Player* const player);
	void DeleteNow();
	void DrawNow();
	int CheckButton();
private:
	const std::wstring SplitText(const std::wstring& text, int max_len, int text_size) const;
	const std::wstring ToMax(const std::wstring& text, float max_wight, float text_size) const;

	sf::RenderWindow* window_;
	sf::Sprite background_;
	int choose_button_;
	std::vector<Button> buttons_;
	int key_button_;
	std::function<void(Player* player)> player_ui_;
};

#endif // !UIM
