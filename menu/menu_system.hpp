#pragma once
#include "common.hpp"

// Forward declaration to avoid circular dependency
class WorldSystem;
class RenderSystem;

enum class GAME_STATE
{
	MAIN_MENU,
	OPTIONS,
	OUTFIT,
	LORE,
	NOTE_1,
	NOTE_2,
	NOTE_3,
	LEVEL_SELECTION,
	GAMEPLAY,
	GAME_OVER,
	PAUSED,
	THE_END,
	SUMMARY
};

class MenuSystem
{
private:
	bool esc_key_pressed;

public:
	MenuSystem();
	//void step(GLFWwindow *window);
	GAME_STATE getCurrentState() const;
	void step(float elapsed_ms, GLFWwindow *window, WorldSystem &world, RenderSystem &renderer);
	void handleMouseInput(GLFWwindow *window, WorldSystem &world, RenderSystem &renderer, double xpos, double ypos,
						  bool left_click, const mat3 &inverse_projection);
	void updateHoverState(RenderSystem &renderer, double xpos, double ypos, const mat3 &inverse_projection);
	GAME_STATE current_state;
	GAME_STATE previous_state;
	Entity credits_entity;
	bool is_timer_paused = false;
	float total_pause_duration = 0.0f;
};

extern MenuSystem menuSystem;
