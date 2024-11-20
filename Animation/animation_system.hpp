#pragma once
#include "common.hpp"
#include "engine/components.hpp"
#include "engine/tiny_ecs_registry.hpp"

// Animation system
class AnimationSystem
{
public:
	// Variables
	static const int CAT_IDLE_WALK_FRAME = 6;
	static const int CAT_JUMP_FRAME = 1;
	static const int ENEMY_BOSS_IDLE_FRAME = 10;
	static const int ENEMY_BOSS_JUMP_FRAME = 1;
	static const int ENEMY_FLYER_IDLE_FRAME = 8;
	static const int ENEMY_FLYER_DEATH_FRAME = 9;
	static const int WEAPON_ATTACK_FRAME = 5;
	static const int GRENADE_LAUNCHER_ATTACK_FRAME = 7;
	static const int GRENADE_EXPLODE_FRAME = 12;
	static const int CAT_SPIN_FRAME = 9;
	

	static void applyAnimation(Entity entity, float elapsed_ms, int &frame_current, GLfloat &frame_width);
	static void handlePlayerAnimation(Entity entity, float elapsed_ms, int &frame_current, GLfloat &frame_width);
	static void handleEnemyAnimation(Entity entity, float elapsed_ms, int &frame_current, GLfloat &frame_width);
	static void handleWeaponAnimation(Entity entity, float elapsed_ms, int &frame_current, GLfloat &frame_width);
	static void handleGrenadeAnimation(Entity entity, float elapsed_ms, int &frame_current, GLfloat &frame_width);

	static void setAnimation(GEOMETRY_BUFFER_ID &currGeometry, TEXTURE_ASSET_ID &currTexture, int &currFrame,
							 GEOMETRY_BUFFER_ID targetGeometry, TEXTURE_ASSET_ID targetTexture, int total_frames,
							 GLfloat &frame_width);

	static void updateAnimationFrame(float &frame_counter, int &currFrame, int total_frame, int frame_time, bool loop);

	static void setSprite(TEXTURE_ASSET_ID &currTexture, TEXTURE_ASSET_ID targetTexture);
};
