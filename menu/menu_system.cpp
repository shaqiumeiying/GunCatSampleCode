#include "menu_system.hpp"
#include "engine/tiny_ecs_registry.hpp"
#include "renderer/render_system.hpp"

MenuSystem menuSystem;

MenuSystem::MenuSystem() : current_state(GAME_STATE::MAIN_MENU), esc_key_pressed(false) {}

GAME_STATE MenuSystem::getCurrentState() const { return current_state; }

void MenuSystem::step(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		if (!esc_key_pressed)
		{
			esc_key_pressed = true;

			if (current_state == GAME_STATE::GAMEPLAY)
			{
				current_state = GAME_STATE::PAUSED;
				std::cout << "Game Paused" << std::endl;
			}
			else if (current_state == GAME_STATE::PAUSED)
			{
				current_state = GAME_STATE::GAMEPLAY;
				std::cout << "Game Resumed" << std::endl;
			}
		}
	}
	else
	{
		esc_key_pressed = false;
	}

	// main menu -> level select
	if (current_state == GAME_STATE::MAIN_MENU)
	{
		// Go to level selection when ENTER is pressed
		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
		{
			current_state = GAME_STATE::LEVEL_SELECTION;
		}
	}

	// Level select -> gameplay
	if (current_state == GAME_STATE::LEVEL_SELECTION)
	{
		// Handle mouse input
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			// TODO: Figure out how to handle mouse input to a specific button item
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			mat3 ortho_projection = RenderSystem::createOrthographicProjection(window_width_px, window_height_px);
			mat3 inverse_projection = glm::inverse(ortho_projection);
			handleMouseInput(xpos, ypos, true, inverse_projection);
		}
	}

	
	if (current_state == GAME_STATE::PAUSED)
	{
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			// pause menu press q to quit the game
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	}
}

void MenuSystem::handleMouseInput(double xpos, double ypos, bool left_click, const mat3 &inverse_projection)
{
	if (!left_click)
		return;

	// NDC COORD
	float ndc_x = (2.0f * xpos) / window_width_px - 1.0f;
	float ndc_y = 1.0f - (2.0f * ypos) / window_height_px;

	// NDC -> World
	vec3 mouse_world = inverse_projection * vec3(ndc_x, ndc_y, 1.0f);
	// std::cout << "Mouse world: " << mouse_world.x << ", " << mouse_world.y << std::endl;
	for (Entity entity : registry.buttons.entities)
	{
		// std::cout << "Button entity: " << entity << std::endl;

		const Motion &motion = registry.motions.get(entity);
		vec2 position = motion.position;
		vec2 scale = motion.scale;

		float left = position.x - scale.x / 2;
		// std::cout << "Left: " << left << std::endl;
		float right = position.x + scale.x / 2;
		// std::cout << "Right: " << right << std::endl;
		float top = position.y - scale.y / 2;
		// std::cout << "Top: " << top << std::endl;
		float bottom = position.y + scale.y / 2;
		// std::cout << "Bottom: " << bottom << std::endl;

		if (mouse_world.x >= left && mouse_world.x <= right && mouse_world.y >= top && mouse_world.y <= bottom)
		{
			if (registry.renderRequests.has(entity))
			{
				TEXTURE_ASSET_ID texture_id = registry.renderRequests.get(entity).used_texture;
				// std::cout << "Button clicked: " << static_cast<int>(texture_id) << std::endl;
				if (texture_id == TEXTURE_ASSET_ID::LEVEL_BUTTON)
				{
				std::cout << "Level selected!" << std::endl;
				current_state = GAME_STATE::GAMEPLAY;
				}
			}
		}
	}
}

