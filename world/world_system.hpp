#pragma once

// internal
#include "common.hpp"


// stlib
#include <vector>
#include <random>
#include <string>


#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "renderer/render_system.hpp"
#include "player/player_input_system.hpp"

#include <LDtkLoader/Entity.hpp>
#include <LDtkLoader/Layer.hpp>
#include <LDtkLoader/Level.hpp>
#include <LDtkLoader/Project.hpp>
#include <LDtkLoader/Tile.hpp>


// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	WorldSystem();

	// Creates a window
	GLFWwindow *create_window();

	// starts the game
	void init(RenderSystem *renderer);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Check for collisions
	void handle_collisions();

	// Should the game be over ?
	bool is_over() const;

	// restart level
	void restart_game();

	bool disable_input = true;

	// swap to new level
	void swap_level(int level_index);

	//default skin
	Skin selected_skin;

	void updateMusic();
	void addDeathTimer(Entity entity);

	Entity createEndGameTrigger(vec2 position);

private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_input(int button, int action, int mod);
	void on_mouse_move(vec2 pos);

	

	// place entites in world at game start
	// void create_world();
	void create_world(const ldtk::Layer *layer);

	// swap to new level
	// void swap_level(int level_index);

	// OpenGL window handle
	GLFWwindow *window;

	// Number of fish eaten by the salmon, displayed in the window title
	unsigned int points;

	// Game state
	RenderSystem *renderer;
	float current_speed;
	float next_eel_spawn;
	float next_fish_spawn;
	Entity player;
	//Entity equipped_weapon;
	Entity crosshair;
	PlayerInputSystem playerInputSystem;

	// map of enemy positions and patrol boxes

	// music references
	Mix_Music *background_music;
	Mix_Chunk *salmon_dead_sound;
	Mix_Chunk* salmon_eat_sound;
	Mix_Chunk *grenade_bounce_sound; 
	Mix_Music *rainbowcat_music;

	Entity fps_text;
	Entity bullet_text;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	// LDtk level info
	int level_index;

	float level_height;
	float level_width;

	std::string project_path = ldtk_path("GunCat.ldtk");

	std::vector<std::string> level_names = {"Level_0", "Level_1", "Level_2", "Level_3"};
};
