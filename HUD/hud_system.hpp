#pragma once

#include <vector>
#include "common.hpp"

class HUD_System
{
public:
	vec2 HEALTH_ICON_SIZE = {64.0f / 1.5, 64.0f / 1.5};
	vec2 BULLET_ICON_SIZE = {24.f, 24.f};
	vec2 BULLET_CONTAINER_SIZE = {300.f / 1.25, 100.f / 1.25};

	void initializeHUD(int health_count);

	// Health UI
	std::vector<Entity> health_icons;
	void updateHealthHUD(int new_health);
	// Bullet UI
	//std::vector<Entity> bullet_icons;
	//void updateBulletHUD(int new_bullet_count, bool reload);


private:
	Entity createHealthIcon(vec2 position, vec2 scale);
	//Entity createBulletIcon(vec2 position, vec2 scale);
	//void createBulletIconContainer();
};

// Declare an external instance of UI_System
extern HUD_System HUD_system;
