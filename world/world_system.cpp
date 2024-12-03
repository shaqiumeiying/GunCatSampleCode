// Header
// Header
#include "world_system.hpp"
#include "world_init.hpp"
#include "HUD/hud_system.hpp"
#include "level_system.hpp";
#include "player/player_input_system.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <string>
#include <iostream>
#include <tuple>
#include <cmath>      // For sin and cos
#include <cstdlib>    // For rand and srand
#include <ctime>      // For seeding random numbers
#include <chrono>	  // For high-resolution clock
#include <fstream>    // For file I/O


#include "loader/LoaderSystem.hpp"
#include "weapons/bullet_system.hpp"
#include "physics/physics_system.hpp"
#include "weapons/weapon_system.hpp"
#include "menu/menu_system.hpp"
#include "renderer/particle_system.hpp"
#include "main.h"
#include "player/player_input_system.hpp"

constexpr float TILE_PIXEL = 64.f;
//float level_height;
//float level_width;
int curr_level;

bool isRestarting = false;

std::chrono::time_point<std::chrono::high_resolution_clock> level_start_time;
float level_elapsed_time = 0.0f;

////////////////////// Load LDtk level data///////////////////////////
const ldtk::Level *loadLDtkFile(const std::string level_name, const std::string &project_path = ldtk_path("GunCat.ldtk"))
{
	try
	{
		static ldtk::Project project;
		project.loadFromFile(project_path);

		const auto &world = project.getWorld();
		const auto &level = world.getLevel(level_name);

		return &level; // Return the level pointer
	}
	catch (const std::invalid_argument &e)
	{
		std::cerr << "Error loading LDtk data: " << e.what() << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << "Unexpected error: " << e.what() << std::endl;
	}
	return nullptr; // Return nullptr on error
}


// load Player position from layer
vec2 getPlayerPosition(const ldtk::Level *level)
{
	if (!level)
	{
		std::cerr << "Level not loaded." << std::endl;
		return {100, window_height_px - 400}; // Default position
	}

	for (const auto &layer : level->allLayers())
	{
		if (layer.getName() == "Entities" && layer.hasEntity("Player"))
		{
			const auto &players = layer.getEntitiesByName("Player");
			if (!players.empty())
			{
				const auto &player = players[0].get();
				auto pos = player.getPosition();
				return {pos.x, pos.y};
			}
		}
	}
	return {100, window_height_px - 400}; // Default if Player is not found
}

// Load collectable spawns
std::vector<std::tuple<COLLECTABLE_TYPE, vec2>> getCollectablePos(const ldtk::Level *level)
{
	std::vector<std::tuple<COLLECTABLE_TYPE, vec2>> collectables; // This is the output vector

	for (const auto &layer : level->allLayers())
	{
		if (layer.getName() == "Entities" && layer.hasEntity("Collectable"))
		{
			const auto &entities = layer.getEntitiesByName("Collectable");
			for (const auto &collectable_ref : entities) 
			{
				const auto &entity = collectable_ref.get();
				auto pos_ref = entity.getPosition();
				vec2 pos = {pos_ref.x, pos_ref.y};
				int type = entity.getField<int>("type").value_or(-1); 
				collectables.push_back(std::make_tuple((COLLECTABLE_TYPE)type, pos));
			}
		}
	}

	return collectables; // Ensure you return the populated vector
}

// load enemy position and corresponding patrol boxes
// tuple (patrol_box, list of tuple(enemy type, enemy position))
std::unordered_map<int, std::tuple<std::tuple<vec2, vec2>, std::vector<std::tuple<ENEMY_TYPE, vec2>>>> getEnemySpawnData(const ldtk::Level *level)
{
	std::unordered_map<int, std::tuple<std::tuple<vec2, vec2>, std::vector<std::tuple<ENEMY_TYPE, vec2>>>> enemySpawnData;

	if (!level)
	{
		std::cerr << "Level not loaded." << std::endl;
		return enemySpawnData; // Return empty vector if level not loaded
	}

	bool flyers_populated = false;
	bool boids_populated = false;
	bool chargers_populated = false;
	bool boss_populated = false;

	auto populate_data = [&](auto &enemies, ENEMY_TYPE enemy_type)
	{
		for (const auto &entity_ref : enemies)
		{
			const auto &entity = entity_ref.get();
			// Extract the position using __worldX and __worldY attributes
			auto point = entity.getPosition();
			vec2 pos = {point.x - 32.f, point.y - 32.f}; // align with background
			if (enemy_type == ENEMY_TYPE::BOSS)
			{
				// boss uses top left corner point as pivot, apply offset
				pos.x += ENEMY_BOSS_BB_WIDTH / 2;
				pos.y += ENEMY_BOSS_BB_HEIGHT / 2 + 5.f;
			}
			else if (enemy_type == ENEMY_TYPE::CHARGER)
			{
				// chargers use bottom center as pivot, apply offset
				pos.y -= ENEMY_CHARGER_BB_HEIGHT / 2;
			}

			int id = entity.getField<int>("Enemy_Patrol_Box_id").value();
			if (enemySpawnData.find(id) != enemySpawnData.end())
			{
				std::get<1>(enemySpawnData[id]).push_back(std::make_tuple(enemy_type, pos));
			}
			else
			{
				enemySpawnData[id] =
					std::make_tuple(std::make_tuple(vec2{}, vec2{}),
									std::vector<std::tuple<ENEMY_TYPE, vec2>>{std::make_tuple(enemy_type, pos)});
			}
		}
	};

	for (const auto &layer : level->allLayers())
	{
		if (layer.getName() != "Entities")
		{
			continue;
		}

		if (layer.hasEntity("Enemy_Patrol_Box"))
		{
			for (const auto &entity_ref : layer.getEntitiesByName("Enemy_Patrol_Box"))
			{
				std::cout << "loading patrol box" << std::endl;
				const auto &entity = entity_ref.get();
				// Extract the position using __worldX and __worldY attributes
				auto pos = entity.getPosition();
				auto width = entity.getSize().x; // get size
				auto height = entity.getSize().y;

				// -32px to align with background
				vec2 topLeft{pos.x - 32.f, pos.y - 32.f};
				vec2 bottomRight{pos.x + width - 32.f, pos.y + height - 32.f};

				int id = entity.getField<int>("Enemy_Patrol_Box_id").value();
				if (enemySpawnData.find(id) != enemySpawnData.end())
				{
					std::get<0>(enemySpawnData[id]) = std::make_tuple(topLeft, bottomRight);
				}
				else
				{
					enemySpawnData[id] = std::make_tuple(std::make_tuple(topLeft, bottomRight),
														 std::vector<std::tuple<ENEMY_TYPE, vec2>>());
				}
			}
		}

		if (layer.hasEntity("Enemy_Flyer") && !flyers_populated)
		{
			const auto &enemy_flyers = layer.getEntitiesByName("Enemy_Flyer");
			populate_data(enemy_flyers, ENEMY_TYPE::FLYER);
			flyers_populated = true;
		}
		if (layer.hasEntity("Enemy_Boid") && !boids_populated)
		{
			const auto &enemy_boids = layer.getEntitiesByName("Enemy_Boid");
			populate_data(enemy_boids, ENEMY_TYPE::BOID);
			boids_populated = true;
		}
		if (layer.hasEntity("Enemy_Charger") && !chargers_populated)
		{
			const auto &enemy_chargers = layer.getEntitiesByName("Enemy_Charger");
			populate_data(enemy_chargers, ENEMY_TYPE::CHARGER);
			chargers_populated = true;
		}
		if (layer.hasEntity("Enemy_Boss") && !boss_populated)
		{
			const auto &enemy_boss = layer.getEntitiesByName("Enemy_Boss");
			populate_data(enemy_boss, ENEMY_TYPE::BOSS);
			boss_populated = true;
		}

		break;
	}
	return enemySpawnData;
}

//load level exit
std::vector<std::tuple<int, std::tuple<vec2,vec2>>> getLevelExits(const ldtk::Level *level) {
	std::vector<std::tuple<int, std::tuple<vec2, vec2>>> exits;
	for (const auto &layer : level->allLayers())
	{
		if (layer.getName() == "Entities" && layer.hasEntity("Level_Exit"))
		{
			for (const auto &entity_ref : layer.getEntitiesByName("Level_Exit"))
			{
				std::cout << "loading exit box" << std::endl;
				const auto &entity = entity_ref.get();
				// Extract the position using __worldX and __worldY attributes
				auto pos = entity.getPosition();
				auto width = entity.getSize().x; // get size
				auto height = entity.getSize().y;

				// -32px to align with background
				vec2 topLeft{pos.x - 32.f, pos.y - 32.f};
				vec2 bottomRight{pos.x + width - 32.f, pos.y + height - 32.f};

				int index = entity.getField<int>("to_level").value();
				exits.push_back(std::make_tuple(index, std::make_tuple(topLeft, bottomRight)));
			}
		}
	}
	return exits;
}

// load alpha box
std::vector<std::tuple<float, std::tuple<vec2, vec2>>> getAlphaBox(const ldtk::Level *level)
{
	std::vector<std::tuple<float, std::tuple<vec2, vec2>>> alpha_boxes;
	for (const auto &layer : level->allLayers())
	{
		if (layer.getName() == "Entities" && layer.hasEntity("Alpha_Box"))
		{
			for (const auto &entity_ref : layer.getEntitiesByName("Alpha_Box"))
			{
				std::cout << "loading alpha box" << std::endl;
				const auto &entity = entity_ref.get();
				// Extract the position using __worldX and __worldY attributes
				auto pos = entity.getPosition();
				auto width = entity.getSize().x; // get size
				auto height = entity.getSize().y;

				// -32px to align with background
				vec2 topLeft{pos.x - 32.f, pos.y - 32.f};
				vec2 bottomRight{pos.x + width - 32.f, pos.y + height - 32.f};

				float alpha = entity.getField<float>("alpha").value();
				alpha_boxes.push_back(std::make_tuple(alpha, std::make_tuple(topLeft, bottomRight)));
			}
		}
	}
	return alpha_boxes;
}


// load kill box
std::vector<std::tuple<int, std::tuple<vec2, vec2>>> getKillBox(const ldtk::Level *level)
{
	std::vector<std::tuple<int, std::tuple<vec2, vec2>>> kill_boxes;
	for (const auto &layer : level->allLayers())
	{
		if (layer.getName() == "Entities" && layer.hasEntity("Kill_Box"))
		{
			for (const auto &entity_ref : layer.getEntitiesByName("Kill_Box"))
			{
				std::cout << "loading kill box" << std::endl;
				const auto &entity = entity_ref.get();
				// Extract the position using __worldX and __worldY attributes
				auto pos = entity.getPosition();
				auto width = entity.getSize().x; // get size
				auto height = entity.getSize().y;

				// -32px to align with background
				vec2 topLeft{pos.x - 32.f, pos.y - 32.f};
				vec2 bottomRight{pos.x + width - 32.f, pos.y + height - 32.f};

				int damage = entity.getField<int>("damage").value();
				kill_boxes.push_back(std::make_tuple(damage, std::make_tuple(topLeft, bottomRight)));
			}
		}
	}
	return kill_boxes;
}

//// load pickup box
//std::vector<std::tuple<int, std::tuple<vec2, vec2>>> getPickupBox(const ldtk::Level *level)
//{
//	std::vector<std::tuple<int, std::tuple<vec2, vec2>>> pickup_boxes;
//	for (const auto &layer : level->allLayers())
//	{
//		if (layer.getName() == "Entities" && layer.hasEntity("PickUp_Box"))
//		{
//			for (const auto &entity_ref : layer.getEntitiesByName("Pickup_Box"))
//			{
//				std::cout << "loading kill box" << std::endl;
//				const auto &entity = entity_ref.get();
//				// Extract the position using __worldX and __worldY attributes
//				auto pos = entity.getPosition();
//				auto width = entity.getSize().x; // get size
//				auto height = entity.getSize().y;
//
//				// -32px to align with background
//				vec2 topLeft{pos.x - 32.f, pos.y - 32.f};
//				vec2 bottomRight{pos.x + width - 32.f, pos.y + height - 32.f};
//
//				int type = entity.getField<int>("type").value();
//				pickup_boxes.push_back(std::make_tuple(type, std::make_tuple(topLeft, bottomRight)));
//			}
//		}
//	}
//	return pickup_boxes;
//}


////////////////////// Load LDtk level data///////////////////////////


// create the world
WorldSystem::WorldSystem() : selected_skin(Skin::DEFAULT)
{
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
	
	// destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (salmon_dead_sound != nullptr)
		Mix_FreeChunk(salmon_dead_sound);
	if (salmon_eat_sound != nullptr)
		Mix_FreeChunk(salmon_eat_sound);

	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char *desc) {
		fprintf(stderr, "%d: %s", error, desc);
	}
}

GLFWwindow *WorldSystem::create_window()
{
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	// Set OpenGL version hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *video_mode = glfwGetVideoMode(primary_monitor);

	// Create the fullscreen window using predefined `window_width_px` and `window_height_px`
	window = glfwCreateWindow(window_width_px, window_height_px, "GunCat", primary_monitor, nullptr);
	if (window == nullptr)
	{
		fprintf(stderr, "Failed to create GLFW window");
		return nullptr;
	}

	// Set window callbacks for input handling
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2, int _3)
	{ ((WorldSystem *)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow *wnd, double _0, double _1) {
		((WorldSystem *)glfwGetWindowUserPointer(wnd))->on_mouse_move({_0, _1});
	};
	auto mouse_button_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2)
	{ ((WorldSystem *)glfwGetWindowUserPointer(wnd))->on_mouse_input(_0, _1, _2); };

	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, mouse_button_redirect);

	////////////////////////////////////////
	// Load SDL audio components
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}
	Mix_AllocateChannels(16);

	background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
	rainbowcat_music = Mix_LoadMUS(audio_path("nyan_cat.wav").c_str());
	salmon_dead_sound = Mix_LoadWAV(audio_path("death_sound.wav").c_str());
	salmon_eat_sound = Mix_LoadWAV(audio_path("eat_sound.wav").c_str());
	grenade_bounce_sound = Mix_LoadWAV(audio_path("grenade_bounce_sound.wav").c_str());

	if (background_music == nullptr || salmon_dead_sound == nullptr || salmon_eat_sound == nullptr)
	{
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n %s \n make sure the data directory is present",
				audio_path("music.wav").c_str(), audio_path("nyan_cat").c_str(), audio_path("death_sound.wav").c_str(),
				audio_path("eat_sound.wav").c_str());
		return nullptr;
	}

	return window;
}


void WorldSystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	// disabled for debug purposes
	Mix_PlayMusic(background_music, -1);
	fprintf(stderr, "Loaded music\n");

	curr_level = -1;
	// Set all states to default
    //restart_game();

}

//TODO: might want to move this to a dedicated level_system file

// void WorldSystem::create_world() {
// 	// floor
// 	createObstacle(renderer, {window_width_px / 2, window_height_px + 50}, ObstacleType::PLATFORM );
// 	// ceiling
// 	createObstacle(renderer, {window_width_px / 2, -125}, ObstacleType::PLATFORM);
// 	// right wall
// 	createObstacle(renderer, {window_width_px + 120, window_height_px - 150}, ObstacleType::WALL);
// 	// left wall
// 	createObstacle(renderer, {-130, window_height_px - 150}, ObstacleType::WALL);

//Create Layer

void WorldSystem::create_world(const ldtk::Layer* layer)
{
	float cell_size = layer->getCellSize();

	vec2 cell_scale = {cell_size, cell_size};
	std::cout << cell_size << std::endl;
	
	for (const ldtk::Tile &tile : layer->allTiles())
	{
		vec2 pos = vec2(tile.getPosition().x, tile.getPosition().y);
		//std::cout << pos.x << " " << pos.y << std::endl;

		createObstacle(renderer, pos, ObstacleType::PLATFORM , cell_scale);
	}

	//// floor
	//createFloor(renderer, {window_width_px / 2, window_height_px + 50});
	//// ceiling
	//createCeiling(renderer, {window_width_px / 2, -125});
	//// right wall
	//registry.rightWalls.emplace(createWall(renderer, {window_width_px + 120, window_height_px - 150}));
	//// left wall
	//registry.leftWalls.emplace(createWall(renderer, {-130, window_height_px - 150}));
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update)
{
	if (curr_level == -1)
		return false;
	Entity player = registry.players.entities[0];
	// Updating window title with points
	std::stringstream title_ss;
	Weapon &weapon = registry.weapons.get(weapon_system.equipped_weapon);
	title_ss << "GunCat";

	registry.texts.get(bullet_text).info = "Ammo: " + std::to_string(weapon.round_count) + "/" + std::to_string(weapon.magazine_capactity);

	if (playerInputSystem.fps_toggle)
	{
		registry.texts.get(fps_text).info = "FPS: " + std::to_string(static_cast<int>(fps));
	}
	else
	{
		registry.texts.get(fps_text).info = "";
	}

	glfwSetWindowTitle(window, title_ss.str().c_str());

	//reset level if player falls out of bounds
	Motion &player_motion = registry.motions.get(player);
	if (player_motion.position.y > level_height || player_motion.position.y < 0)
	{
		restart_game();
		return false;
	}


	// since world goes first this should be overwritten by invincibilty system
	auto &opacity = registry.opacities.get(player);
	opacity = 1.0;

	//do something if player touches trigger box
	for (Entity entity : registry.tbox.entities) {
		const auto tbox = registry.tbox.get(entity);
		float player_y = player_motion.position.y;
		float player_x = player_motion.position.x;

		float lbox_top = tbox.top_left_point.y;
		float lbox_bottom = tbox.bottom_right_point.y;
		float lbox_left = tbox.top_left_point.x;
		float lbox_right = tbox.bottom_right_point.x;

		if (player_y > lbox_top && player_y < lbox_bottom && player_x > lbox_left && player_x < lbox_right) {
			if (registry.alpha_box.has(entity))
			{
				opacity = 0.5;
			}
			if (registry.deadlys.has(entity))
			{
				registry.collisions.emplace_with_duplicates(entity, player);
				registry.collisions.emplace_with_duplicates(player, entity);
			}
			if (registry.level_out.has(entity))
			{

				int level_index = registry.level_out.get(entity).level_index;

				auto level_end_time = std::chrono::high_resolution_clock::now();
				level_elapsed_time = std::chrono::duration<float>(level_end_time - level_start_time).count() - menuSystem.total_pause_duration;

				/*std::cout << "pause time: " << menuSystem.total_pause_duration << std::endl;
				std::cout << "total time: " << level_elapsed_time << std::endl;*/

				std::cout << "current level index: " << curr_level << std::endl;
				std::cout << "next level index: " << level_index << std::endl;

				// 0 based
				std::ofstream outFile(score_path(curr_level + 1));
				if (outFile.is_open())
				{

					if (curr_level == 3)
					{
						outFile << "Secret Level" << ": " << level_elapsed_time << " seconds\n";
						std::cout << "Secret level unlocked" << std::endl;
					}
					else
					{
						outFile << "Level " << curr_level + 1 << ": " << level_elapsed_time << " seconds\n";
					}

					std::cout << "saving to file: " << score_path(curr_level + 1) << std::endl;
					
					outFile.close();

					swap_level(registry.level_out.get(entity).level_index);
					level_start_time = std::chrono::high_resolution_clock::now();
				}
				else
				{
					std::cerr << "Error: Could not open file to save level time.\n";
				}

				return false;
			}
		}
	}

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());
	float line_width = 4.0f;
	if (debugging.in_debug_mode)
	{
		for (Entity &entity : registry.enemies.entities)
		{
			Motion &motion = registry.motions.get(entity);
			float height = abs(motion.scale.y);
			float width = abs(motion.scale.x);
			Entity Line1 = createLine(motion.position + vec2(0, height / 2), {width, line_width});
			Entity Line2 = createLine(motion.position + vec2(0, (-height / 2)), {width, line_width});
			Entity Line3 = createLine(motion.position + vec2(width / 2, 0), {line_width, height});
			Entity Line4 = createLine(motion.position + vec2(-width / 2, 0), {line_width, height});
		}

		PhysicsSystem physicsSystem;
		Motion &player_motion = registry.motions.get(player);
		vec2 player_bounding_box = physicsSystem.get_bounding_box_2(player_motion);


		Entity e1 = createLine(player_motion.position + vec2(0, player_bounding_box.x / 2), {player_bounding_box.x, line_width});

		Entity e2 = createLine(player_motion.position - vec2(0, player_bounding_box.x / 2), {player_bounding_box.x, line_width});
	
		Entity e3 = createLine(player_motion.position - vec2(player_bounding_box.x / 2, 0),
							 {line_width, player_bounding_box.x});
 
		Entity e4 = createLine(player_motion.position + vec2(player_bounding_box.x / 2, 0),
							 {line_width, player_bounding_box.x});
	 
		// patrol box debug lines
		for (PatrolBox &box : registry.patrolBoxes.components)
		{
			float height = box.bottom_right_point.y - box.top_left_point.y;
			float width = box.bottom_right_point.x - box.top_left_point.x;
			Entity top = createLine(box.top_left_point + vec2(width / 2, 0), {width, line_width});
			Entity bottom = createLine(vec2(box.top_left_point.x, box.bottom_right_point.y) + vec2(width / 2, 0),
									   {width, line_width});
			Entity right = createLine(vec2(box.bottom_right_point.x, box.top_left_point.y) + vec2(0, height / 2),
									  {line_width, height});
			Entity left = createLine(box.top_left_point + vec2(0, height / 2), {line_width, height});
		}
	}

	// Process DeathTimers
	auto &deathTimers = registry.deathTimers;
	for (uint i = 0; i < deathTimers.components.size(); ++i)
	{
		Entity entity = deathTimers.entities[i];
		DeathTimer &timer = deathTimers.components[i];

		// Reduce the timer by the elapsed time
		timer.time_remaining -= elapsed_ms_since_last_update / 1000.0f; // Convert milliseconds to seconds

		// Check if the timer has expired
		if (timer.time_remaining <= 0.0f)
		{
			// Remove the entity after the death timer expires
			registry.remove_all_components_of(entity);
		}
	}

	//if player is dead true, press enter to restart
	if (registry.players.has(player) && registry.players.get(player).is_dead && (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS))
	{
		restart_game();
	}

	// update enemy health bar positions
	for (auto& entity : registry.enemies.entities)
	{
		if (registry.deathTimers.has(entity))
			continue;
		
		auto &enemy = registry.enemies.get(entity);
		if (enemy.enemy_type == ENEMY_TYPE::BULLET || enemy.enemy_type == ENEMY_TYPE::BOSS)
			continue;

		auto &motion = registry.motions.get(entity);
		if (length(motion.velocity) < 1.f)
			continue;

		auto &inner_bar_motion = registry.motions.get(enemy.health_bar_inner);
		auto &outer_bar_motion = registry.motions.get(enemy.health_bar_outer);
		auto &enemy_health = registry.healths.get(entity);

		vec2 pos = motion.position - vec2{0.f, abs(motion.scale.y) / 2};

		outer_bar_motion.position = pos;

		float bar_length = 77.f * (enemy_health.health / 3.f);
		inner_bar_motion.scale.x = bar_length;
		inner_bar_motion.position = pos + vec2{-(77.f - bar_length) / 2, 0.f};
	}

	return true;
}

void WorldSystem::swap_level(int level_index) {
	curr_level = level_index;
	loader.set_level_save_data(curr_level);
	restart_game();
}

// Reset the world state to its initial state, useful for debugging
void WorldSystem::restart_game() {

	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");
	PlayerInputSystem::clear_inputs();
	disable_input = false;
	isRestarting = true;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all fish, eels, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
	    registry.remove_all_components_of(registry.motions.entities.back());

	registry.tbox.clear();
	registry.level_out.clear();
	registry.alpha_box.clear();

	// Debugging for memory/component leaks
	registry.list_all_components();

	const ldtk::Level *level = loadLDtkFile(level_names[curr_level]);
	//float 
	level_width = static_cast<float>(level->size.x);
	//float 
	level_height = static_cast<float>(level->size.y);
	vec2 playerPosition = getPlayerPosition(level);

	TEXTURE_ASSET_ID level_bg = static_cast<TEXTURE_ASSET_ID>(curr_level);
	// Create background with calculated scaling factor
	createBackground(renderer, 
		{level_width / 2 - 32, level_height/2 - 32}, 
		{level_width, level_height}, level_bg);

	// create a new Player
	player = createPlayer(renderer, playerPosition, selected_skin);
	registry.players.get(player).is_dead = false;
  
	fps_text = createText("FPS: ", {0, 0}, 0.5, {1, 1, 0});
	bullet_text = createText("Ammo: ", {window_width_px - 185, window_height_px - 90}, 0.45, {0,0,0});

	// crate a new Crosshair
	if(registry.crosshairs.size() == 0) crosshair = createCrosshair();
	std::cout << "crosshair" << crosshair <<std::endl;
	weapon_system.set_crosshair(crosshair);

	
	create_world(&level->getLayer("Wall"));

	//create weapon
	weapon_system.available_weapons.clear(); 
	//weapon_system.available_weapons.push_back(createWeapon(renderer, {-1, -1}, 0.0, 0.0, 0.0, 0, 0.0, NO_WEAPON));
	weapon_system.available_weapons.push_back(
		createWeapon(renderer, {-1000, -1000}, 200.0, 250.0, 400.0, 9, 2500.0, RIFLE));
	weapon_system.available_weapons.push_back(
		createWeapon(renderer, {-1050, -1000}, 300.0, 200.0, 0.0, 1, 4500.0, GRENADE_LAUNCHER));
	
	weapon_system.set_equipped_weapon(weapon_system.available_weapons[0]);

	/*equipped_weapon = weapon_system.get_equipped_weapon();*/
	// Initialize UI with health and bullet counts
	int player_health = registry.healths.get(player).health;
	// int bullet_count = registry.weapons.get(equipped_weapon).round_count;
	HUD_system.initializeHUD(player_health);

	// create enemies - location from LDtk
	auto enemyPositions = getEnemySpawnData(level);

	// Spawn each enemy at its respective position
	for (const auto &patrolBoxId : getEnemySpawnData(level))
	{
		auto patrolBox = std::get<0>(patrolBoxId.second);
		for (auto& enemyData : std::get<1>(patrolBoxId.second))
		{
			vec2 pos = std::get<1>(enemyData);
			ENEMY_TYPE enemy_type = std::get<0>(enemyData);
			switch (enemy_type)
			{
			case ENEMY_TYPE::FLYER:
				createEnemyFlyer(pos, std::get<0>(patrolBox), std::get<1>(patrolBox));
				break;
			case ENEMY_TYPE::BOID:
				createEnemyBoid(pos, std::get<0>(patrolBox), std::get<1>(patrolBox));
				break;
			case ENEMY_TYPE::CHARGER:
				createEnemyCharger(pos, std::get<0>(patrolBox), std::get<1>(patrolBox));
				break;
			case ENEMY_TYPE::BOSS:
				createEnemyBoss(pos, std::get<0>(patrolBox), std::get<1>(patrolBox));
				HUD_system.initializeBossHealthBar();
				break;
			default:
				break;
			}
		}
	}

	// Spawn collectables at respective position
	for (const auto &collectableData : getCollectablePos(level))
	{
		vec2 pos = std::get<1>(collectableData);
		COLLECTABLE_TYPE collectable_type = std::get<0>(collectableData);
		switch (collectable_type)
		{
		case COLLECTABLE_TYPE::LORE1:
			if (!loader.is_lore_found(0))
			{
				createLore(renderer, pos, collectable_type);
			}
			break;
		case COLLECTABLE_TYPE::LORE2:
			if (!loader.is_lore_found(1))
			{
				createLore(renderer, pos, collectable_type);
			}
			break;
		case COLLECTABLE_TYPE::LORE3:
			if (!loader.is_lore_found(2))
			{
				createLore(renderer, pos, collectable_type);
			}
			break;
		case COLLECTABLE_TYPE::GRENADE_LAUNCHER:
			if (!loader.is_grenade_found())
			{
				createCollectableLauncher(renderer, pos, collectable_type);
			}
			break;
		default:
			break;
		}
	}

	//create level out
	std::vector<std::tuple<int, std::tuple<vec2, vec2>>> levelExits = getLevelExits(level);

	for (const auto &levelExit : levelExits) 
	{
		createLevelExit(std::get<0>(levelExit), std::get<0>(std::get<1>(levelExit)), std::get<1>(std::get<1>(levelExit)));
	}

	//create alpha boxes
	std::vector<std::tuple<float, std::tuple<vec2, vec2>>> alphaBoxes = getAlphaBox(level);

	for (const auto &alphaBox : alphaBoxes)
	{
		createAlphaBox(std::get<0>(alphaBox), std::get<0>(std::get<1>(alphaBox)),
						std::get<1>(std::get<1>(alphaBox)));
	}

	// create kill boxes
	std::vector<std::tuple<int, std::tuple<vec2, vec2>>> killBoxes = getKillBox(level);

	for (const auto &killBox : killBoxes)
	{
		createKillBox(std::get<0>(killBox), std::get<0>(std::get<1>(killBox)),
					   std::get<1>(std::get<1>(killBox)));
	}

	glfwPollEvents();

	// start timer
	level_start_time = std::chrono::high_resolution_clock::now();
	menuSystem.is_timer_paused = false;
	menuSystem.total_pause_duration = 0.0f;

	isRestarting = false;
}


bool leftCollide(Motion& motion, Motion& other_motion, const float& left_most_collision){
	return (motion.velocity.x < 0 &&
			left_most_collision - (other_motion.position.x + other_motion.scale.x / 2) <= 0 &&
			other_motion.position.y - other_motion.scale.y/2 - motion.position.y <= abs(motion.scale.y/2) - 32 &&
			motion.position.y - (other_motion.position.y + other_motion.scale.y / 2) <= abs(motion.scale.y/2) - 20 &&
			other_motion.position.x - motion.position.x < 0);
}

bool leftCollideNonMesh(Motion &motion, Motion &other_motion)
{
	return (motion.velocity.x < 0 &&
			abs(motion.position.x - (other_motion.position.x + other_motion.scale.x / 2)) <= abs(motion.scale.x / 2) &&
			!(other_motion.position.y - other_motion.scale.y / 2 - motion.position.y >= motion.scale.y / 2 - 8) &&
			!(motion.position.y - (other_motion.position.y + other_motion.scale.y / 2) >= motion.scale.y / 2 - 8));
}

bool rightCollideNonMesh(Motion& motion, Motion& other_motion) {
	return (motion.velocity.x > 0 &&
			abs((other_motion.position.x - other_motion.scale.x / 2) - motion.position.x) <= abs(motion.scale.x / 2) &&
			!(other_motion.position.y - other_motion.scale.y / 2 - motion.position.y >= motion.scale.y / 2 - 8) &&
			!(motion.position.y - (other_motion.position.y + other_motion.scale.y / 2) >= motion.scale.y / 2 - 8));
}

bool rightCollide(Motion &motion, Motion &other_motion, const float &right_most_collision)
{
	return (motion.velocity.x > 0 &&
			(other_motion.position.x - other_motion.scale.x / 2) - right_most_collision <= 0 &&
			other_motion.position.y - other_motion.scale.y/2 - motion.position.y <= abs(motion.scale.y/2) - 32 &&
			motion.position.y - (other_motion.position.y + other_motion.scale.y / 2) <= abs(motion.scale.y/2) - 20 &&
			other_motion.position.x - motion.position.x > 0);
}

bool topCollide(Motion &motion, Motion &other_motion) {
	return (motion.velocity.y > 0 &&
			other_motion.position.y - other_motion.scale.y / 2 - motion.position.y >= motion.scale.y / 2 - TILE_PIXEL &&
			other_motion.position.x - other_motion.scale.x / 2 < motion.position.x + abs(motion.scale.x / 2) - 10 &&
			motion.position.x - abs(motion.scale.x / 2) < other_motion.position.x + other_motion.scale.x / 2 - 10);
}

bool bottomCollide(Motion &motion, Motion &other_motion){
	return (motion.velocity.y < 0 &&
			motion.position.y - (other_motion.position.y + other_motion.scale.y / 2) >= motion.scale.y / 2 - TILE_PIXEL &&
			other_motion.position.x - other_motion.scale.x / 2 < motion.position.x + abs(motion.scale.x / 2) - 10 &&
			motion.position.x - abs(motion.scale.x / 2) < other_motion.position.x + other_motion.scale.x / 2 -  10);
}

// Compute collisions between entities
void WorldSystem::handle_collisions() 
{
	// Loop over all collisions detected by the physics system
	Player &player_info = registry.players.get(player);

	// Reset Player Info Collision For Jumps
	player_info.is_on_ground = false;
	player_info.left_wall_jump = false;
	player_info.right_wall_jump = false;

	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		// Skip collisions with background
		if ((registry.renderRequests.has(entity) && registry.backgrounds.has(entity)) ||
			(registry.renderRequests.has(entity_other) && registry.backgrounds.has(entity_other)))
		{
			continue;
		}

		// player collision
		if (registry.players.has(entity)) {

			Motion &motion = registry.motions.get(entity);

			if (registry.endGameTriggers.has(entity_other))
			{
				Player &player = registry.players.get(entity);
				if (!player.is_dead)
				{
					// stop timer here
					auto level_end_time = std::chrono::high_resolution_clock::now();
					level_elapsed_time = std::chrono::duration<float>(level_end_time - level_start_time).count();

					std::ofstream outFile(score_path(3));
					if (outFile.is_open())
					{
						outFile << "Level " << 3 << ": " << level_elapsed_time << " seconds\n";
						outFile.close();

						/*swap_level(registry.level_out.get(entity).level_index);*/
						// level_start_time = std::chrono::high_resolution_clock::now();
					}
					// Player interacts with the end game trigger
					menuSystem.current_state = GAME_STATE::THE_END;
					std::cout << "The player has reached the end!" << std::endl;

					registry.texts.remove(bullet_text);
				}
				
			}

			if (registry.obstacles.has(entity_other))
			{
				Motion &motion = registry.motions.get(entity);
				Motion other_motion = registry.motions.get(entity_other);

				float left_most_collision = collisionsRegistry.components[i].left_most_collision;
				float right_most_collision = collisionsRegistry.components[i].right_most_collision;
				float upper_most_collision = collisionsRegistry.components[i].upper_most_collision;
				float bottom_most_collision = collisionsRegistry.components[i].bottom_most_collision;

				// falling
				if (topCollide(motion, other_motion))
				{
					motion.position.y = mix(motion.position.y, motion.position.y - (bottom_most_collision - (other_motion.position.y - other_motion.scale.y / 2)), 0.3f);

					motion.velocity.y = 0;
					player_info.is_on_ground = true;
				}
				else if(bottomCollide(motion, other_motion)){
					motion.position.y = mix(motion.position.y, motion.position.y - (upper_most_collision - (other_motion.position.y + other_motion.scale.y / 2)), 0.3f);

					motion.velocity.y = 0;
				}
				else if(leftCollide(motion, other_motion, left_most_collision)){
					motion.position.x += (other_motion.position.x + other_motion.scale.x / 2) - left_most_collision;

					motion.velocity.x = -1; // intentionally not 0 to enable wall jump
					player_info.left_wall_jump = true;
				}
				else if(rightCollide(motion, other_motion, right_most_collision)){
					motion.position.x -= right_most_collision - (other_motion.position.x - other_motion.scale.x / 2);
					motion.velocity.x = 1; // intentionally not 0 to enable wall jump
					player_info.right_wall_jump = true;
				}
			}

			if (registry.deadlys.has(entity_other))
			{
				if(debugging.in_invincibility_mode) continue;
				if(registry.invincibilityTimers.has(entity)) continue;
				if(registry.deathTimers.has(entity_other)) continue;

				Health &health = registry.healths.get(entity);
				health.health -= registry.deadlys.get(entity_other).damage;
				// Update health UI WITH new health value
				HUD_system.updateHealthHUD(health.health);

				if(health.health <= 0)
				{
					HUD_system.updateHealthHUD(0);
					disable_input = true;
					player_info.is_dead = true;
					//restart_game();
					//  Set Game Over State
					HUD_system.deadHUD();

					// Change cat animation to DEATH
					if (registry.animations.has(entity))
					{
						Animation &animation = registry.animations.get(entity);
						animation.type = DEATH;
					}
				}



				registry.invincibilityTimers.emplace(entity); // default is 500

				if(registry.enemyBullets.has(entity_other))
				{
					registry.remove_all_components_of(entity_other);
				}
			}
			if (registry.collectables.has(entity_other))
			{
				Collectable& collect = registry.collectables.get(entity_other);
				COLLECTABLE_TYPE type = collect.type;

				switch (type)
				{
				case COLLECTABLE_TYPE::LORE1:
					loader.set_lore_found(0);
					registry.remove_all_components_of(entity_other);
					break;
				case COLLECTABLE_TYPE::LORE2:
					loader.set_lore_found(1);
					registry.remove_all_components_of(entity_other);
					break;
				case COLLECTABLE_TYPE::LORE3:
					loader.set_lore_found(2);
					registry.remove_all_components_of(entity_other);
					break;
				case COLLECTABLE_TYPE::GRENADE_LAUNCHER:
					loader.set_grenade_found();
					registry.remove_all_components_of(entity_other);
					break;
				default:
					break;
				}
			}

		}
		else if (registry.bullets.has(entity))
		{
			if (registry.deadlys.has(entity_other) || registry.obstacles.has(entity_other))
			{
				if (registry.deadlys.has(entity_other) && registry.healths.has(entity_other))
				{
					Health &health = registry.healths.get(entity_other);
					health.health -= registry.bullets.get(entity).damage;

					ParticleSystem::spawnParticles(registry.motions.get(entity).position +
													   (registry.motions.get(entity).velocity * 0.05f),
												   registry.motions.get(entity).velocity * -1.f, vec2(15.f, 15.f), 3000,
												   vec3(0.455, 0.851, 0.365), 7, 30.f);

					if (registry.enemies.has(entity_other))
					{
						auto &enemy = registry.enemies.get(entity_other);
						if (enemy.enemy_type == ENEMY_TYPE::BOSS)
						{
							HUD_system.updateBossHealthBar(health.health, 5);
						}

						if (health.health <= 0)
						{
							addDeathTimer(entity_other);
							if (enemy.enemy_type == ENEMY_TYPE::BOSS)
							{
								ParticleSystem::spawnParticles(registry.motions.get(entity_other).position,
															   vec2(0.f, -1600.f), vec2(15.f, 15.f), 3000,
															   vec3(0.455, 0.851, 0.365), 120, 90.f);
								HUD_system.removeBossHealthBar();
								if (registry.endGameTriggers.entities.empty())
								{
									// Create an EndGameTrigger entity at the boss's position
									vec2 boss_position = registry.motions.get(entity_other).position;
									createEndGameTrigger(boss_position);
								}
							}
							else if (enemy.enemy_type != ENEMY_TYPE::BOSS && enemy.enemy_type != ENEMY_TYPE::BULLET)
							{
								registry.remove_all_components_of(enemy.health_bar_inner);
								registry.remove_all_components_of(enemy.health_bar_outer);
							}
						}
					}
				}
				registry.remove_all_components_of(entity);
			}
			continue;
		}
		else if (registry.enemyBullets.has(entity) && !registry.texts.has(entity_other))
		{
			//This is not DRY but works, it's the same code as hitting something with a bullet above.
			EnemyBullet &enemy_bullet = registry.enemyBullets.get(entity);
			if (enemy_bullet.reflected)
			{
				if (registry.enemies.has(entity_other) && registry.healths.has(entity_other))
				{
					auto &enemy = registry.enemies.get(entity_other);
					Health &enemy_health = registry.healths.get(entity_other);
					enemy_health.health -= 1;

					ParticleSystem::spawnParticles(registry.motions.get(entity).position +
													   (registry.motions.get(entity).velocity * 0.05f),
												   registry.motions.get(entity).velocity * -1.f, vec2(15.f, 15.f), 3000,
												   vec3(0.455, 0.851, 0.365), 7, 30.f);

					if (enemy.enemy_type == ENEMY_TYPE::BOSS)
					{
						HUD_system.updateBossHealthBar(enemy_health.health, 5);
					}

					if (enemy_health.health <= 0)
					{
						addDeathTimer(entity_other);
						if (enemy.enemy_type == ENEMY_TYPE::BOSS)
						{
							ParticleSystem::spawnParticles(registry.motions.get(entity_other).position,
														   vec2(0.f, -1600.f), vec2(15.f, 15.f), 3000,
														   vec3(0.455, 0.851, 0.365), 120, 90.f);
							HUD_system.removeBossHealthBar();
							if (registry.endGameTriggers.entities.empty())
							{
								// Create an EndGameTrigger entity at the boss's position
								vec2 boss_position = registry.motions.get(entity_other).position;
								createEndGameTrigger(boss_position);
							}
						}
						else if (enemy.enemy_type != ENEMY_TYPE::BOSS && enemy.enemy_type != ENEMY_TYPE::BULLET)
						{
							registry.remove_all_components_of(enemy.health_bar_inner);
							registry.remove_all_components_of(enemy.health_bar_outer);
						}
					}
					registry.remove_all_components_of(entity);
				}
			}
			else
			{
				// non reflected bullet -- normal
				if (registry.enemies.has(entity_other) || registry.enemyBullets.has(entity_other) ||
					registry.weapons.has(entity_other) || registry.enemyHealthBars.has(entity_other))
				{
					continue;
				}

				if (debugging.in_debug_mode && registry.debugComponents.has(entity_other))
				{
					continue;
				}

				if (registry.bullets.has(entity_other))
				{
					registry.remove_all_components_of(entity_other);
				}

				if (!registry.players.has(entity_other))
				{
					registry.remove_all_components_of(entity);
				}
			}
		}
		else if (registry.bounces.has(entity))
		{
			// TODO: should move bounce behavior out of world_system.cpp
			Motion &motion = registry.motions.get(entity);

			if (registry.obstacles.has(entity_other))
			{
				// TODO: should move bounce behavior out of world_system.cppd
				Motion &motion = registry.motions.get(entity);
				Motion other_motion = registry.motions.get(entity_other);

				float left_most_collision = collisionsRegistry.components[i].left_most_collision;
				float right_most_collision = collisionsRegistry.components[i].right_most_collision;
				float upper_most_collision = collisionsRegistry.components[i].upper_most_collision;
				float bottom_most_collision = collisionsRegistry.components[i].bottom_most_collision;

				// falling
				if (topCollide(motion, other_motion))
				{
					/*motion.position.y =
						mix(motion.position.y,
							motion.position.y -
								(bottom_most_collision - (other_motion.position.y - other_motion.scale.y / 2)),
							0.3f);*/
					Mix_PlayChannel(-1, grenade_bounce_sound, 0);
					motion.position.y -= 25.0f;
					motion.velocity.y *= -0.8;
					//player_info.is_on_ground = true;ada
				}
				else if (bottomCollide(motion, other_motion))
				{
					/*motion.position.y =
						mix(motion.position.y,
							motion.position.y - 
								(upper_most_collision - (other_motion.position.y + other_motion.scale.y / 2)),
							0.3f);*/
					Mix_PlayChannel(-1, grenade_bounce_sound, 0);
					motion.position.y += 25.0f;

					motion.velocity.y *= -0.8;
				}
				if (leftCollideNonMesh(motion, other_motion))
				{
					Mix_PlayChannel(-1, grenade_bounce_sound, 0);
					motion.position.x += 25.0f;
					
					motion.velocity.x *= -0.8; // intentionally not 0 to enable wall jump
					//player_info.left_wall_jump = true;
				}
				else if (rightCollideNonMesh(motion, other_motion))
				{
					Mix_PlayChannel(-1, grenade_bounce_sound, 0);
					motion.position.x -= 25.f;
					motion.velocity.x *= -0.8; // intentionally not 0 to enable wall jump
					//player_info.right_wall_jump = true;
				}
			}
		}
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {

	// if current state is not GAMEPLAY, ignore key input

	if ((menuSystem.getCurrentState() != GAME_STATE::GAMEPLAY) || disable_input || isRestarting)
	{
		return;
	}

	Motion &motion = registry.motions.get(player);
	playerInputSystem.processKeyInput(player, motion, key, action, window);
  
	// Resetting game
	//if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	//{
	//	int w, h;
	//	glfwGetWindowSize(window, &w, &h);

 //       restart_game();
	//}

	// Debugging
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		debugging.in_debug_mode = !debugging.in_debug_mode;
	}
	if (key == GLFW_KEY_I && action == GLFW_PRESS)
	{
		debugging.in_invincibility_mode = !debugging.in_invincibility_mode;
	}
}


void WorldSystem::on_mouse_input(int button, int action, int mod) {

	if ((menuSystem.getCurrentState() != GAME_STATE::GAMEPLAY) || disable_input || isRestarting)
	{
		return; // Ignore world's mouse input in non-GAMEPLAY states
	}
	Weapon &weapon = registry.weapons.get(weapon_system.equipped_weapon);
	Motion &motion = registry.motions.get(player);
	Crosshair &ch = registry.crosshairs.get(crosshair);
	playerInputSystem.processMouseInput(weapon_system.equipped_weapon, motion, weapon, ch, button, action, window);

}

void WorldSystem::on_mouse_move(vec2 mouse_position)
{
	if (disable_input)
	{
		return; // Ignore world's mouse input in non-GAMEPLAY states
	}
	Entity camera_entity = registry.cameras.entities[0];
	vec2 camera_pos = registry.cameras.get(camera_entity).position;

	// adjust calculations by camera position
	float tx = -camera_pos.x + window_width_px / 2.0f; 
	float ty = -camera_pos.y + window_height_px / 2.0f;

	// Convert the mouse position from screen space to world space
	mat3 view = {{1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {tx, ty, 1.f}};

	mat3 inverse_view = inverse(view); // Get the inverse of the view matrix, not 100% sure why but it works

	// Convert mouse position from screen space to world coordinates
	vec3 mouse_world = inverse_view * vec3(mouse_position, 1.0f);

	Motion &motion = registry.motions.get(player);

	vec2 player_position = motion.position;

	vec2 difference = vec2(mouse_world.x, mouse_world.y) - player_position;

	Motion &gun_motion = registry.motions.get(weapon_system.equipped_weapon);


	Crosshair &ch = registry.crosshairs.get(crosshair);
	ch.direction = normalize(difference);

	ch.angle = atan2(difference.y, difference.x);

	ch.position = vec2(mouse_world.x, mouse_world.y);

	if (abs(ch.angle) > 1.5)
	{
		motion.scale = vec2{-PLAYER_BB_WIDTH, PLAYER_BB_HEIGHT};
		gun_motion.scale = vec2{WEAPON_BB_WIDTH, -WEAPON_BB_HEIGHT};
	}
	else
	{
		motion.scale = vec2{PLAYER_BB_WIDTH, PLAYER_BB_HEIGHT};
		gun_motion.scale = vec2{WEAPON_BB_WIDTH, WEAPON_BB_HEIGHT};
	}
	
	// update weapon angle
	weapon_system.updateWeaponAngle(weapon_system.equipped_weapon, ch.angle);

}


void WorldSystem::addDeathTimer(Entity entity)
{
	if (!registry.animations.has(entity))
	{
		registry.animations.emplace(entity);
	}

	// Set death animation
	Animation &animation = registry.animations.get(entity);
	animation.type = DEATH;

	// death timer duration based on enemy type
	float death_timer_duration = 0.75f; // Default timer duration
	if (registry.enemies.has(entity))
	{
		Enemy &enemy = registry.enemies.get(entity);
		switch (enemy.enemy_type)
		{
		case ENEMY_TYPE::BOSS:
			if (registry.motions.has(entity))
			{
				Motion &motion = registry.motions.get(entity);
				motion.position.y -= 65.f;
			}
			death_timer_duration = 1.85f; // Boss timer duration
			break;
		default:
			death_timer_duration = 0.75f;
			break;
		}
	}

	// Add a death timer if it doesn't already exist
	if (!registry.deathTimers.has(entity))
	{
		registry.deathTimers.emplace(entity, DeathTimer{death_timer_duration});
		std::cout << "Death Timer Added: " << death_timer_duration << " seconds" << std::endl;
	}
}

void WorldSystem::updateMusic()
{
	static Skin previous_skin = selected_skin;
	static Mix_Music *currently_playing = background_music; // Track the currently playing music

	if (previous_skin != selected_skin)
	{
		Mix_Music *new_music = nullptr;

		if (selected_skin == Skin::CAT_SKIN_RAINBOW)
		{
			new_music = rainbowcat_music;
		}
		else
		{
			new_music = background_music;
		}

		if (new_music != currently_playing)
		{
			Mix_HaltMusic();
			if (Mix_PlayMusic(new_music, -1) == -1)
			{
				//fprintf(stderr, "Failed to play music: %s\n", Mix_GetError());
			}
			else
			{
				//fprintf(stderr, "Now playing nyan cat\n");
				currently_playing = new_music; // Update the currently playing music
			}
		}

		// Update the previous_skin tracker
		previous_skin = selected_skin;
  }
}

Entity WorldSystem::createEndGameTrigger(vec2 position)
{
	Entity end_trigger = Entity();
	Motion &motion = registry.motions.emplace(end_trigger);
	motion.position = position;
	motion.scale = {192.f, 192.f};
	registry.renderRequests.insert(
		end_trigger, {TEXTURE_ASSET_ID::END_GAME_TRIGGER, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE});
	registry.endGameTriggers.emplace(end_trigger);
	return end_trigger;
}

