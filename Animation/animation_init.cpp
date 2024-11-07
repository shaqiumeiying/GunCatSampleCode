#include "animation_init.hpp"
#include "renderer/render_system.hpp"


// Function to initialize all animations
void initializeAnimations(RenderSystem &renderSystem)
{
	initCatIdle(renderSystem);
	initCatWalk(renderSystem);
	initCatJump(renderSystem);
	initEnemyFlyer(renderSystem);
	initWeaponIdle(renderSystem);
	initWeaponAttack(renderSystem);
}
const std::vector<uint16_t> textured_indices = {0, 3, 1, 1, 3, 2};

// CAT IDLE
void initCatIdle(RenderSystem &renderSystem)
{
	std::vector<TexturedVertex> cat_idle(4);
	cat_idle[0].position = {-1.f / 2, +1.f / 2, 0.f};
	cat_idle[1].position = {+1.f / 2, +1.f / 2, 0.f};
	cat_idle[2].position = {+1.f / 2, -1.f / 2, 0.f};
	cat_idle[3].position = {-1.f / 2, -1.f / 2, 0.f};
	cat_idle[0].texcoord = {0.f, 1.f}; // Bottom left
	cat_idle[1].texcoord = {1.f / 6, 1.f}; // Bottom right
	cat_idle[2].texcoord = {1.f / 6, 0.f}; // Top right
	cat_idle[3].texcoord = {0.f, 0.f}; // Top left
	renderSystem.bindVBOandIBO(GEOMETRY_BUFFER_ID::CAT_IDLE, cat_idle, textured_indices);
}

void initCatWalk(RenderSystem &renderSystem)
{
	std::vector<TexturedVertex> cat_walk(4);
	cat_walk[0].position = {-1.f / 2, +1.f / 2, 0.f};
	cat_walk[1].position = {+1.f / 2, +1.f / 2, 0.f};
	cat_walk[2].position = {+1.f / 2, -1.f / 2, 0.f};
	cat_walk[3].position = {-1.f / 2, -1.f / 2, 0.f};
	cat_walk[0].texcoord = {0.f, 1.f}; // Bottom left
	cat_walk[1].texcoord = {1.f / 6, 1.f}; // Bottom right
	cat_walk[2].texcoord = {1.f / 6, 0.f}; // Top right
	cat_walk[3].texcoord = {0.f, 0.f}; // Top left
	renderSystem.bindVBOandIBO(GEOMETRY_BUFFER_ID::CAT_WALK, cat_walk, textured_indices);
}

void initCatJump(RenderSystem &renderSystem)
{
	std::vector<TexturedVertex> cat_jump(4);
	cat_jump[0].position = {-1.f / 2, +1.f / 2, 0.f};
	cat_jump[1].position = {+1.f / 2, +1.f / 2, 0.f};
	cat_jump[2].position = {+1.f / 2, -1.f / 2, 0.f};
	cat_jump[3].position = {-1.f / 2, -1.f / 2, 0.f};
	cat_jump[0].texcoord = {0.f, 1.f}; // Bottom left
	cat_jump[1].texcoord = {1.f, 1.f}; // Bottom right
	cat_jump[2].texcoord = {1.f, 0.f}; // Top right
	cat_jump[3].texcoord = {0.f, 0.f}; // Top left
	renderSystem.bindVBOandIBO(GEOMETRY_BUFFER_ID::CAT_JUMP, cat_jump, textured_indices);
}

void initEnemyFlyer(RenderSystem &renderSystem)
{
	std::vector<TexturedVertex> enemy_flyer(4);
	enemy_flyer[0].position = {-1.f / 2, +1.f / 2, 0.f};
	enemy_flyer[1].position = {+1.f / 2, +1.f / 2, 0.f};
	enemy_flyer[2].position = {+1.f / 2, -1.f / 2, 0.f};
	enemy_flyer[3].position = {-1.f / 2, -1.f / 2, 0.f};
	enemy_flyer[0].texcoord = {0.f, 1.f};
	enemy_flyer[1].texcoord = {0.125, 1.f};
	enemy_flyer[2].texcoord = {0.125, 0.f};
	enemy_flyer[3].texcoord = {0.f, 0.f}; 
	renderSystem.bindVBOandIBO(GEOMETRY_BUFFER_ID::ENEMY_FLYER, enemy_flyer, textured_indices);
}

void initWeaponIdle(RenderSystem &renderSystem)
{
	std::vector<TexturedVertex> weapon_idle(4);
	weapon_idle[0].position = {-1.f / 2, +1.f / 2, 0.f};
	weapon_idle[1].position = {+1.f / 2, +1.f / 2, 0.f};
	weapon_idle[2].position = {+1.f / 2, -1.f / 2, 0.f};
	weapon_idle[3].position = {-1.f / 2, -1.f / 2, 0.f};
	weapon_idle[0].texcoord = {0.f, 1.f};
	weapon_idle[1].texcoord = {1.f, 1.f};
	weapon_idle[2].texcoord = {1.f, 0.f};
	weapon_idle[3].texcoord = {0.f, 0.f};
	renderSystem.bindVBOandIBO(GEOMETRY_BUFFER_ID::WEAPON_IDLE, weapon_idle, textured_indices);
}

void initWeaponAttack(RenderSystem &renderSystem)
{
	std::vector<TexturedVertex> weapon_attack(4);
	weapon_attack[0].position = {-1.f / 2, +1.f / 2, 0.f};
	weapon_attack[1].position = {+1.f / 2, +1.f / 2, 0.f};
	weapon_attack[2].position = {+1.f / 2, -1.f / 2, 0.f};
	weapon_attack[3].position = {-1.f / 2, -1.f / 2, 0.f};
	weapon_attack[0].texcoord = {0.f, 1.f};
	weapon_attack[1].texcoord = {1.f / 5, 1.f};
	weapon_attack[2].texcoord = {1.f / 5, 0.f};
	weapon_attack[3].texcoord = {0.f, 0.f};
	renderSystem.bindVBOandIBO(GEOMETRY_BUFFER_ID::WEAPON_ATTACK, weapon_attack, textured_indices);
}
