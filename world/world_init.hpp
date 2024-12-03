#pragma once

#include "common.hpp"
#include "engine/tiny_ecs.hpp"
#include "renderer/render_system.hpp"

// These are hardcoded to the dimensions of the entity texture
// BB = bounding box
const float PLAYER_BB_WIDTH  = 122.f;
const float PLAYER_BB_HEIGHT = 116.f;
const float FLOOR_BB_WIDTH = 64.f;
const float FLOOR_BB_HEIGHT = 64.f;
const float ENEMY_BOSS_BB_WIDTH = 768.f;
const float ENEMY_BOSS_BB_HEIGHT = 512.f;
const float ENEMY_FLYER_BB_WIDTH = 104.f;
const float ENEMY_FLYER_BB_HEIGHT = 116.f;
const float ENEMY_CHARGER_BB_WIDTH = 110.f;
const float ENEMY_CHARGER_BB_HEIGHT = 97.f;
const float ENEMY_BULLET_BB_WIDTH = 53.f;
const float ENEMY_BULLET_BB_HEIGHT = 53.f;
const float WEAPON_BB_WIDTH = 164.f;
const float WEAPON_BB_HEIGHT = 67.f;
const float WALL_BB_WIDTH = (1024.f / 4) * 1.25;
const float WALL_BB_HEIGHT = 1024.f * 1.25;
const float GRENADE_BB_WIDTH = 50.f;
const float GRENADE_BB_HEIGHT = 50.f;
const float LORE_BB_WIDTH = 80.f;
const float LORE_BB_HEIGHT = 80.f;

enum class ObstacleType
{
	WALL,
	PLATFORM
};

// Struct to hold obstacle properties
struct ObstacleProps
{
	vec2 scale;
	TEXTURE_ASSET_ID textureId;
};

// Define a vector of obstacle properties, indexed by ObstacleType
extern std::vector<ObstacleProps> obstacleProps;

Entity createLine(vec2 position, vec2 size);
// the player
Entity createPlayer(RenderSystem *renderer, vec2 pos, Skin selected_skin);

// the prey
Entity createFish(RenderSystem* renderer, vec2 position);

// the wall
Entity createObstacle(RenderSystem *renderer, vec2 position, ObstacleType type, glm::vec2 cell_scale);


// the crosshair
Entity createCrosshair();

Entity createWeapon(RenderSystem *renderer, vec2 pos, float damage, float rate_of_fire, float recoil, int magazine_capacity, float reload_time, WEAPON_TYPE weapon_type);
 
Entity createEnemyFlyer(vec2 pos, vec2 left_point, vec2 right_point);
Entity createEnemyBoid(vec2 pos, vec2 left_point, vec2 right_point);
Entity createEnemyCharger(vec2 pos, vec2 left_point, vec2 right_point);
Entity createEnemyBoss(vec2 pos, vec2 left_point, vec2 right_point);

Entity createBackground(RenderSystem *renderer, vec2 pos, vec2 scale, TEXTURE_ASSET_ID level_bg);

Entity createLine(vec2 position, vec2 scale);

Entity createLevelExit(int index, vec2 left_point, vec2 right_point);

Entity createAlphaBox(float alpha, vec2 left_point, vec2 right_point);

Entity createKillBox(int damage, vec2 left_point, vec2 right_point);

Entity createText(std::string info, vec2 pos, float scale, vec3 color);

Entity createLore(RenderSystem *renderer, vec2 pos, COLLECTABLE_TYPE type);

Entity createCollectableLauncher(RenderSystem *renderer, vec2 pos, COLLECTABLE_TYPE type);
