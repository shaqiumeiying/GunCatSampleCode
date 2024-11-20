#include "hud_system.hpp"
#include "engine/tiny_ecs_registry.hpp"
#include "renderer/render_system.hpp"

// Define the global UI_System instance
HUD_System HUD_system;

void HUD_System::initializeHUD(int health_count)
{
	// Initialize health icons
	health_icons.clear();
	for (int i = 0; i < health_count; ++i)
	{
		vec2 position = {i * 64.0f - window_width_px / 2 + 32, -window_height_px / 2 + 32};
		Entity health_icon = createHealthIcon(position, HEALTH_ICON_SIZE);
		health_icons.push_back(health_icon);
	}

	// maybe it's better to add text instead of icons
	
	// Initialize bullet icons
	//bullet_icons.clear();
	//for (int i = 0; i < bullet_count; ++i)
	//{
	//	vec2 position = {i * 24.0f - window_width_px / 2 + 40, -window_height_px / 2 + 115};
	//	Entity bullet_icon = createBulletIcon(position, BULLET_ICON_SIZE);
	//	bullet_icons.push_back(bullet_icon);
	//}

	//// Initialize bullet icon container if necessary
	//createBulletIconContainer();
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

//// Initialize bullet UI
// void UI_System::initializeBulletUI(int bullet_count)
//{
//	bullet_icons.clear();
//	for (int i = 0; i < bullet_count; ++i)
//	{
//		vec2 position = {i * 24.0f - window_width_px / 2 + 40, -window_height_px / 2 + 115};
//		Entity health_icon = createBulletIcon(position, BULLET_ICON_SIZE);
//		bullet_icons.push_back(health_icon);
//	}
//	createBulletIconContainer();
// }

// Create a bullet icon entity
//Entity HUD_System::createBulletIcon(vec2 position, vec2 scale)
//{
//	Entity entity = Entity();
//
//	Motion &motion = registry.motions.emplace(entity);
//	motion.position = position;
//	motion.scale = scale;
//
//	registry.renderRequests.insert(
//		entity, {TEXTURE_ASSET_ID::BULLET_ROUND, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE});
//
//	// Attach UIComponent to mark it as part of the UI
//	registry.huds.emplace(entity);
//
//	return entity;
//}
//
//// Create Bullet icon container
//void HUD_System::createBulletIconContainer()
//{
//	// Create a container for the bullet icons
//	Entity entity = Entity();
//
//	Motion &motion = registry.motions.emplace(entity);
//	motion.position = {-window_width_px / 2 + 140, -window_height_px / 2 + 100};
//	motion.scale = BULLET_CONTAINER_SIZE;
//
//	registry.renderRequests.insert(
//		entity, {TEXTURE_ASSET_ID::BULLET_CONTAINER, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE});
//
//	// Attach UIComponent to mark it as part of the UI
//	registry.huds.emplace(entity);
//}
//
//// Update Bullet UI
//void HUD_System::updateBulletHUD(int new_bullet, bool reload)
//{
//	// If reloading, reset the bullet icons completely
//	if (reload)
//	{
//		// Remove all bullet icon entities from the ECS
//		for (Entity icon : bullet_icons)
//		{
//			registry.remove_all_components_of(icon);
//		}
//
//		// Clear the vector after removing the entities
//		bullet_icons.clear();
//
//		for (int i = 0; i < new_bullet; ++i)
//		{
//			vec2 position = {i * 24.0f - window_width_px / 2 + 40, -window_height_px / 2 + 115};
//			Entity bullet_icon = createBulletIcon(position, BULLET_ICON_SIZE);
//			bullet_icons.push_back(bullet_icon);
//		}
//	}
//	else
//	{
//		// Only adjust icons if adding or removing due to firing
//		while (bullet_icons.size() > new_bullet)
//		{
//			// Remove the last icon in the vector and delete its entity
//			Entity last_icon = bullet_icons.back();
//			registry.remove_all_components_of(last_icon);
//			bullet_icons.pop_back();
//		}
//	}
//}
