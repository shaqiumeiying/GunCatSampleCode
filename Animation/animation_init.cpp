#include "animation_init.hpp"
#include "renderer/render_system.hpp"


// Function to initialize all animations
void initializeAnimations(RenderSystem &renderSystem)
{
	initCatIdle(renderSystem);
	initCatWalk(renderSystem);
	initCatJump(renderSystem);
	initCatSpin(renderSystem);
	initEnemyBossIdle(renderSystem);
	initEnemyBossJump(renderSystem);
	initEnemyFlyer(renderSystem);
	initWeaponIdle(renderSystem);
	initWeaponAttack(renderSystem);
	initEnemyFlyerDeath(renderSystem);
	initGrenadeLauncherWeaponIdle(renderSystem);
	initGrenadeLauncherWeaponAttack(renderSystem);
	initExplode(renderSystem);
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

void initCatSpin(RenderSystem &renderSystem)
{
	std::vector<TexturedVertex> cat_spin(4);
	cat_spin[0].position = {-1.f / 2, +1.f / 2, 0.f};
	cat_spin[1].position = {+1.f / 2, +1.f / 2, 0.f};
	cat_spin[2].position = {+1.f / 2, -1.f / 2, 0.f};
	cat_spin[3].position = {-1.f / 2, -1.f / 2, 0.f};
	cat_spin[0].texcoord = {0.f, 1.f}; // Bottom left
	cat_spin[1].texcoord = {0.111f, 1.f}; // Bottom right
	cat_spin[2].texcoord = {0.111f, 0.f}; // Top right
	cat_spin[3].texcoord = {0.f, 0.f}; // Top left
	renderSystem.bindVBOandIBO(GEOMETRY_BUFFER_ID::CAT_SPIN, cat_spin, textured_indices);
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


void initEnemyFlyerDeath(RenderSystem &renderSystem)
{
	std::vector<TexturedVertex> enemy_flyer_death(4);
	enemy_flyer_death[0].position = {-1.f / 2, +1.f / 2, 0.f};
	enemy_flyer_death[1].position = {+1.f / 2, +1.f / 2, 0.f};
	enemy_flyer_death[2].position = {+1.f / 2, -1.f / 2, 0.f};
	enemy_flyer_death[3].position = {-1.f / 2, -1.f / 2, 0.f};
	enemy_flyer_death[0].texcoord = {0.f, 1.f};
	enemy_flyer_death[1].texcoord = {1.f / 9, 1.f};
	enemy_flyer_death[2].texcoord = {1.f / 9, 0.f};
	enemy_flyer_death[3].texcoord = {0.f, 0.f};
	renderSystem.bindVBOandIBO(GEOMETRY_BUFFER_ID::ENEMY_FLYER_DEATH, enemy_flyer_death, textured_indices);
}

void initEnemyBossIdle(RenderSystem& renderSystem)
{
	std::vector<TexturedVertex> enemy_boss_idle(4);
	enemy_boss_idle[0].position = {-1.f / 2, +1.f / 2, 0.f};
	enemy_boss_idle[1].position = {+1.f / 2, +1.f / 2, 0.f};
	enemy_boss_idle[2].position = {+1.f / 2, -1.f / 2, 0.f};
	enemy_boss_idle[3].position = {-1.f / 2, -1.f / 2, 0.f};
	enemy_boss_idle[0].texcoord = {0.f, 1.f};
	enemy_boss_idle[1].texcoord = {0.1f, 1.f};
	enemy_boss_idle[2].texcoord = {0.1f, 0.f};
	enemy_boss_idle[3].texcoord = {0.f, 0.f};
	renderSystem.bindVBOandIBO(GEOMETRY_BUFFER_ID::ENEMY_BOSS_IDLE, enemy_boss_idle, textured_indices);
}

void initEnemyBossJump(RenderSystem &renderSystem)
{
	std::vector<TexturedVertex> enemy_boss_jump(4);
	enemy_boss_jump[0].position = {-1.f / 2, +1.f / 2, 0.f};
	enemy_boss_jump[1].position = {+1.f / 2, +1.f / 2, 0.f};
	enemy_boss_jump[2].position = {+1.f / 2, -1.f / 2, 0.f};
	enemy_boss_jump[3].position = {-1.f / 2, -1.f / 2, 0.f};
	enemy_boss_jump[0].texcoord = {0.f, 1.f};
	enemy_boss_jump[1].texcoord = {1.f, 1.f};
	enemy_boss_jump[2].texcoord = {1.f, 0.f};
	enemy_boss_jump[3].texcoord = {0.f, 0.f};
	renderSystem.bindVBOandIBO(GEOMETRY_BUFFER_ID::ENEMY_BOSS_JUMP, enemy_boss_jump, textured_indices);
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
	renderSystem.bindVBOandIBO(GEOMETRY_BUFFER_ID::RIFLE_IDLE, weapon_idle, textured_indices);
	
}

void initGrenadeLauncherWeaponIdle(RenderSystem &renderSystem)
{
	std::vector<TexturedVertex> gweapon_idle(4);
	gweapon_idle[0].position = {-1.f / 2, +1.f / 2, 0.f};
	gweapon_idle[1].position = {+1.f / 2, +1.f / 2, 0.f};
	gweapon_idle[2].position = {+1.f / 2, -1.f / 2, 0.f};
	gweapon_idle[3].position = {-1.f / 2, -1.f / 2, 0.f};
	gweapon_idle[0].texcoord = {0.f, 1.f};
	gweapon_idle[1].texcoord = {1.f, 1.f};
	gweapon_idle[2].texcoord = {1.f, 0.f};
	gweapon_idle[3].texcoord = {0.f, 0.f};
	renderSystem.bindVBOandIBO(GEOMETRY_BUFFER_ID::GRENADE_LAUNCHER_IDLE, gweapon_idle, textured_indices);
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
	renderSystem.bindVBOandIBO(GEOMETRY_BUFFER_ID::RIFLE_ATTACK, weapon_attack, textured_indices);
}

void initGrenadeLauncherWeaponAttack(RenderSystem &renderSystem)
{
	std::vector<TexturedVertex> gweapon_attack(4);
	gweapon_attack[0].position = {-1.f / 2, +1.f / 2, 0.f};
	gweapon_attack[1].position = {+1.f / 2, +1.f / 2, 0.f};
	gweapon_attack[2].position = {+1.f / 2, -1.f / 2, 0.f};
	gweapon_attack[3].position = {-1.f / 2, -1.f / 2, 0.f};
	gweapon_attack[0].texcoord = {0.f, 1.f};
	gweapon_attack[1].texcoord = {1.f / 7, 1.f};
	gweapon_attack[2].texcoord = {1.f / 7, 0.f};
	gweapon_attack[3].texcoord = {0.f, 0.f};
	renderSystem.bindVBOandIBO(GEOMETRY_BUFFER_ID::GRENADE_LAUNCHER_ATTACK, gweapon_attack, textured_indices);
}

void initExplode(RenderSystem &renderSystem)
{
	std::vector<TexturedVertex> grenade_explode(4);
	grenade_explode[0].position = {-4.f , +4.f, 0.f};
	grenade_explode[1].position = {+4.f, +4.f, 0.f};
	grenade_explode[2].position = {+4.f, -4.f, 0.f};
	grenade_explode[3].position = {-4.f, -4.f, 0.f};
	grenade_explode[0].texcoord = {0.f, 1.f};
	grenade_explode[1].texcoord = {1.f / 12, 1.f};
	grenade_explode[2].texcoord = {1.f / 12, 0.f};
	grenade_explode[3].texcoord = {0.f, 0.f};
	renderSystem.bindVBOandIBO(GEOMETRY_BUFFER_ID::GRENADE_EXPLODE, grenade_explode, textured_indices);
}

