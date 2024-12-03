#include "hud_system.hpp"
#include "engine/tiny_ecs_registry.hpp"
#include "renderer/render_system.hpp"

// Define the global UI_System instance
HUD_System HUD_system;

void HUD_System::initializeHUD(int health_count)
{
	createHUDContainer();
	// Initialize health icons
	health_icons.clear();
	for (int i = 0; i < health_count; ++i)
	{
		vec2 position = {i * 45.0f - window_width_px / 2 + 135, -window_height_px / 2 + 80};
		Entity health_icon = createHealthIcon(position, HEALTH_ICON_SIZE);
		health_icons.push_back(health_icon);
	}

	// maybe it's better to add text instead of icons

	//// Initialize bullet icon container if necessary
	
}

// Create a health icon entity
Entity HUD_System::createHealthIcon(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	Motion &motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = scale;

	registry.renderRequests.insert(
		entity, {TEXTURE_ASSET_ID::PLAYER_HEALTH, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE});

	// Attach UIComponent to mark it as part of the UI
	registry.huds.emplace(entity);

	return entity;
}

// Update Health HUD
void HUD_System::updateHealthHUD(int new_health)
{
	// Check if health has decreased, meaning icons need to be removed
	while (health_icons.size() > new_health)
	{
		// Remove the last icon in the vector and delete its entity
		Entity last_icon = health_icons.back();
		registry.remove_all_components_of(last_icon);
		health_icons.pop_back();
	}
}

void HUD_System::deadHUD()
{
	Entity dead_message = Entity();

	Motion &motion = registry.motions.emplace(dead_message);
	motion.position = {0, 0}; 
	motion.scale = {600, 300};

	registry.renderRequests.insert(
		dead_message, {TEXTURE_ASSET_ID::DEAD, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE});

	registry.huds.emplace(dead_message);
}

// create health bar for boss
void HUD_System::initializeBossHealthBar()
{
	vec2 position = {0, window_height_px / 2 - 50}; // Position at the top center
	vec2 scale = {1080, 67}; // Initial width (adjust as needed)

	// Create the foreground
	boss_health_bar = Entity();
	Motion &fg_motion = registry.motions.emplace(boss_health_bar);
	fg_motion.position = position;
	fg_motion.scale = scale;
	registry.renderRequests.insert(
		boss_health_bar, {TEXTURE_ASSET_ID::BOSS_HEALTH_BAR, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE});

	// Create the background
	boss_health_background = Entity();
	Motion &bg_motion = registry.motions.emplace(boss_health_background);
	bg_motion.position = position;
	bg_motion.scale = scale;
	registry.renderRequests.insert(
		boss_health_background,
		{TEXTURE_ASSET_ID::BOSS_HEALTH_BACKGROUND, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE});

	registry.huds.emplace(boss_health_bar);
	registry.huds.emplace(boss_health_background);
}

void HUD_System::updateBossHealthBar(int current_health, int max_health)
{
	if (registry.motions.has(boss_health_bar))
	{
		// Get the boss health bar's motion
		Motion &motion = registry.motions.get(boss_health_bar);

		// Calculate new width based on current health
		float max_width = 1080.0f; // Initial width
		float health_percentage = fmax((float)current_health / max_health, 0); // Normalize health
		motion.scale.x = max_width * health_percentage; // Scale width
	}
}

void HUD_System::removeBossHealthBar() 
{
	registry.remove_all_components_of(boss_health_bar);
	registry.remove_all_components_of(boss_health_background);
}

void HUD_System::createHUDContainer()
{
	// Create a container for the HUD icons
	hud_container = Entity();
	Motion &motion = registry.motions.emplace(hud_container);
	motion.position = {0,-window_height_px/2 + 60};
	motion.scale = {1920, 150};

	registry.renderRequests.insert(
		hud_container, {TEXTURE_ASSET_ID::HUD_CONTAINER, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE});

	registry.huds.emplace(hud_container);
}
