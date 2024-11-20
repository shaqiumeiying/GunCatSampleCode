#pragma once
#include "common.hpp"

enum class GAME_STATE
{
	MAIN_MENU,
	LEVEL_SELECTION,
	GAMEPLAY,
	PAUSED
};

class MenuSystem
{
private:
	GAME_STATE current_state;
	bool esc_key_pressed;

public:
	MenuSystem();

	void step(GLFWwindow *window);
	GAME_STATE getCurrentState() const;
	void handleMouseInput(double xpos, double ypos, bool left_click, const mat3 &inverse_projection);
};

extern MenuSystem menuSystem;
