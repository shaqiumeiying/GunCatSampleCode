#include "menu_system.hpp"
#include "engine/tiny_ecs_registry.hpp"
#include "loader/LoaderSystem.hpp"
#include "renderer/render_system.hpp"
#include "world/world_system.hpp"
#include <chrono>

#include <direct.h>


MenuSystem menuSystem;
std::chrono::time_point<std::chrono::high_resolution_clock> pause_start_time;
std::chrono::time_point<std::chrono::high_resolution_clock> pause_end_time;

MenuSystem::MenuSystem() : current_state(GAME_STATE::MAIN_MENU), esc_key_pressed(false) {}

GAME_STATE MenuSystem::getCurrentState() const { return current_state; }

void MenuSystem::step(float elapsed_ms, GLFWwindow *window, WorldSystem &world, RenderSystem &renderer)
{
	static float scroll_speed = 0.085f;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		if (!esc_key_pressed)
		{
			esc_key_pressed = true;

			if (current_state == GAME_STATE::GAMEPLAY)
			{
				current_state = GAME_STATE::PAUSED;
				is_timer_paused = true;
				pause_start_time = std::chrono::high_resolution_clock::now();
				std::cout << "Game Paused" << std::endl;

			}
			else if (current_state == GAME_STATE::PAUSED)
			{
				current_state = GAME_STATE::GAMEPLAY;
				is_timer_paused = false;
				pause_end_time = std::chrono::high_resolution_clock::now();
				total_pause_duration += std::chrono::duration<float>(pause_end_time - pause_start_time).count();
				std::cout << "total pause time: " << total_pause_duration << std::endl;
				std::cout << "Game Resumed" << std::endl;
			}
			else if (current_state == GAME_STATE::THE_END)
			{
				scroll_speed = 1.0f; // set faster speed
				std::cout << "Speed increased to 1!" << std::endl;
			}
		}
	}
	else if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && menuSystem.current_state == GAME_STATE::MAIN_MENU)
	{
		loader.reset_grenade_data();
		loader.reset_level_save_data();
		loader.reset_lore_data();
		loader.reset_outfit_data();
	}
	else
	{
		esc_key_pressed = false;

		// reset if esc is released
		if (current_state == GAME_STATE::THE_END)
		{
			scroll_speed = 0.085f;
		}
	}

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	mat3 ortho_projection = RenderSystem::createOrthographicProjection(window_width_px, window_height_px);
	mat3 inverse_projection = glm::inverse(ortho_projection);


	if (current_state == GAME_STATE::GAME_OVER)
	{
		// restart game when ENTER is pressed
		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
		{
			current_state = GAME_STATE::GAMEPLAY;
		}
	}

	updateHoverState(renderer, xpos, ypos, inverse_projection);

	static bool was_mouse_pressed = false;

	// Handle state-specific inputs
	if (current_state != GAME_STATE::GAMEPLAY)
	{
		bool is_mouse_pressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

		// press and release mouse button to select
		if (!is_mouse_pressed && was_mouse_pressed)
		{
		handleMouseInput(window, world, renderer, xpos, ypos, true, inverse_projection);
		}
		was_mouse_pressed = is_mouse_pressed;

		if (current_state == GAME_STATE::THE_END)
		{
			if (credits_entity != Entity() && registry.motions.has(credits_entity))
			{
				Motion &motion = registry.motions.get(credits_entity);

				// Scroll the credits upwards
				motion.position.y -= scroll_speed * elapsed_ms; // Moving upward

				if (motion.position.y + motion.scale.y / 2 <= -window_height_px / 2)
				{
					current_state = GAME_STATE::SUMMARY;
					std::cout << "Credits finished. Going to Summary." << std::endl;
				}
			}
		}

	}
}

void deleteScoreFiles()
{ 
	std::string scores_path = data_path() + "/scores/";
#ifdef _WIN32
	// windows 
	std::replace(scores_path.begin(), scores_path.end(), '/', '\\');
	std::string command = "del /Q \"" + scores_path + "*.txt\"";
	;
#else
	// linux
	std::string command = "rm -f \"" + scores_path + "*.txt\"";
#endif

	int result = system(command.c_str());
	if (result != 0)
	{
		std::cerr << "Failed to delete files in " << scores_path << std::endl;
	}
	else
	{
		std::cout << "All .txt files in " << scores_path << " deleted successfully." << std::endl;
	}

}

void MenuSystem::handleMouseInput(GLFWwindow *window, WorldSystem &world, RenderSystem &renderer, double xpos,
								  double ypos, bool left_click,
								  const mat3 &inverse_projection)
{
	if (!left_click)
		return;

	// Convert screen coordinates to world space
	float ndc_x = (2.0f * xpos) / window_width_px - 1.0f;
	float ndc_y = 1.0f - (2.0f * ypos) / window_height_px;
	vec3 mouse_world = inverse_projection * vec3(ndc_x, ndc_y, 1.0f);

	// Iterate through buttons specific to the current state
	if (renderer.cached_entities.find(current_state) != renderer.cached_entities.end())
	{
		for (Entity entity : renderer.cached_entities[current_state])
		{
			if (registry.buttons.has(entity))
			{
				Button &button = registry.buttons.get(entity);
				const Motion &motion = registry.motions.get(entity);
				vec2 position = motion.position;
				vec2 scale = motion.scale;

				float left = position.x - scale.x / 2;
				float right = position.x + scale.x / 2;
				float top = position.y - scale.y / 2;
				float bottom = position.y + scale.y / 2;

				if (mouse_world.x >= left && mouse_world.x <= right && mouse_world.y >= top && mouse_world.y <= bottom)
				{
					// Prevent deselecting an already selected button
					if (button.selected)
					{
						continue; // Skip further processing for this button
					}

					// Mark the clicked button as selected
					button.selected = true;

					// Deselect other buttons
					for (Entity other_entity : renderer.cached_entities[current_state])
					{
						if (other_entity != entity && registry.buttons.has(other_entity))
						{
							registry.buttons.get(other_entity).selected = false;
						}
					}
						// Handle button actions
						TEXTURE_ASSET_ID texture_id = registry.renderRequests.get(entity).used_texture;
						switch (texture_id)
						{
						case TEXTURE_ASSET_ID::MENU_BUTTON:
							current_state = GAME_STATE::MAIN_MENU;
							break;
						case TEXTURE_ASSET_ID::START_BUTTON:
							current_state = GAME_STATE::LEVEL_SELECTION;
							deleteScoreFiles();
							//for debug purposes
							//current_state = GAME_STATE::THE_END;
							break;
						case TEXTURE_ASSET_ID::OPTIONS_BUTTON:
							current_state = GAME_STATE::OPTIONS;
							break;
						case TEXTURE_ASSET_ID::BACK_BUTTON:
							current_state = previous_state; // Return to the previous state
							//std::cout << "Returning to previous state!" << std::endl;
							break;
						case TEXTURE_ASSET_ID::RESUME_BUTTON:
							current_state = GAME_STATE::GAMEPLAY;
							pause_end_time = std::chrono::high_resolution_clock::now();
							total_pause_duration += std::chrono::duration<float>(pause_end_time - pause_start_time).count();
							std::cout << "Game Resumed " << std::endl;
							break;
						case TEXTURE_ASSET_ID::OUTFIT_BUTTON:
							current_state = GAME_STATE::OUTFIT;
							break;
						case TEXTURE_ASSET_ID::LORE_BUTTON:
							previous_state = current_state; // Save the current state
							current_state = GAME_STATE::LORE;
							break;
						case TEXTURE_ASSET_ID::QUIT_BUTTON:
							glfwSetWindowShouldClose(window, GLFW_TRUE);
							break;
						case TEXTURE_ASSET_ID::RESTART_BUTTON:
							world.restart_game();
							current_state = GAME_STATE::GAMEPLAY;
							break;
						case TEXTURE_ASSET_ID::LVL_1_BUTTON:
							//std::cout << "Level 1 selected!" << std::endl;
							world.swap_level(0);
							current_state = GAME_STATE::GAMEPLAY;
							break;
						case TEXTURE_ASSET_ID::LVL_2_BUTTON:
							//std::cout << "Level 2 selected!" << std::endl;
							if(loader.get_level_save_data() >= 1)
							{
								world.swap_level(1);
								current_state = GAME_STATE::GAMEPLAY;
							}
							break;
						case TEXTURE_ASSET_ID::LVL_3_BUTTON:
							//std::cout << "Level 3 selected!" << std::endl;
							if(loader.get_level_save_data() >= 2)
							{
								world.swap_level(2);
								current_state = GAME_STATE::GAMEPLAY;
							}
							break;
						case TEXTURE_ASSET_ID::CAT_SKIN:
							world.selected_skin = Skin::DEFAULT;
							//std::cout << "Cat skin selected!" << std::endl;
							break;
						case TEXTURE_ASSET_ID::CAT_SKIN_XMAS:
							//std::cout << "Xmas Cat skin selected!" << std::endl;
							world.selected_skin = Skin::CAT_SKIN_XMAS;
							break;
						case TEXTURE_ASSET_ID::CAT_SKIN_SLIME:
							//std::cout << "Slime Cat skin selected!" << std::endl;
							world.selected_skin = Skin::CAT_SKIN_SLIME;
							break;
						case TEXTURE_ASSET_ID::CAT_SKIN_SCH:
							//std::cout << "Schrodinger's Cat skin selected!" << std::endl;
							world.selected_skin = Skin::CAT_SKIN_SCH;
							break;
						case TEXTURE_ASSET_ID::CAT_SKIN_RAINBOW:
							//std::cout << "Rainbow Cat skin selected!" << std::endl;
							world.selected_skin = Skin::CAT_SKIN_RAINBOW;
							break;
						case TEXTURE_ASSET_ID::NOTE_1_ICON:
							std::cout << "NOTE_1 Selected" << std::endl;
							current_state = GAME_STATE::NOTE_1;
							break;
						case TEXTURE_ASSET_ID::NOTE_2_ICON:
							std::cout << "NOTE_2 Selected" << std::endl;
							current_state = GAME_STATE::NOTE_2;
							break;
						case TEXTURE_ASSET_ID::NOTE_3_ICON:
							std::cout << "NOTE_3 Selected" << std::endl;
							current_state = GAME_STATE::NOTE_3;
							break;
						case TEXTURE_ASSET_ID::CLOSE:
							std::cout << "Closing NOTE_1 menu" << std::endl;
							current_state = GAME_STATE::LORE;
						default:
							break;
						}
				}
			}
		}
	}
}

void MenuSystem::updateHoverState(RenderSystem &renderer, double xpos, double ypos, const mat3 &inverse_projection)
{
	float ndc_x = (2.0f * xpos) / window_width_px - 1.0f;
	float ndc_y = 1.0f - (2.0f * ypos) / window_height_px;
	vec3 mouse_world = inverse_projection * vec3(ndc_x, ndc_y, 1.0f);

	if (renderer.cached_entities.find(current_state) != renderer.cached_entities.end())
	{
		for (Entity entity : renderer.cached_entities[current_state])
		{
			if (registry.buttons.has(entity))
			{
				Button &button = registry.buttons.get(entity);
				const Motion &motion = registry.motions.get(entity);
				vec2 position = motion.position;
				vec2 scale = motion.scale;

				float left = position.x - scale.x / 2;
				float right = position.x + scale.x / 2;
				float top = position.y - scale.y / 2;
				float bottom = position.y + scale.y / 2;

				button.hovered = (mouse_world.x >= left && mouse_world.x <= right && mouse_world.y >= top &&
								  mouse_world.y <= bottom);
			}
		}
	}
}
