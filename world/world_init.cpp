#include "world_init.hpp"
#include "engine/tiny_ecs_registry.hpp"
#include "iostream"

Entity createPlayer(RenderSystem* renderer, vec2 pos, Skin selected_skin)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SALMON);
	registry.meshPtrs.emplace(entity, &mesh);

	registry.healths.emplace(entity);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({PLAYER_BB_WIDTH, PLAYER_BB_HEIGHT});

	// adding gravity to player
	registry.gravities.emplace(entity);
	// adding friction to player
	registry.frictions.emplace(entity);

	// adding cooldown to player
	registry.spinCoolDown.emplace(entity);

	//adding animation
	Animation& anim = registry.animations.emplace(entity);

	registry.players.emplace(entity);

	// create an empty Player component for our character
	switch (selected_skin)
	{
	case Skin::DEFAULT:
		registry.renderRequests.insert(entity,{TEXTURE_ASSET_ID::CAT_IDLE, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::CAT_IDLE});
		break;
	case Skin::CAT_SKIN_XMAS:
		registry.renderRequests.insert(entity, {TEXTURE_ASSET_ID::CAT_IDLE_XMAS, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::CAT_IDLE});
		break;
	case Skin::CAT_SKIN_SLIME:
		registry.renderRequests.insert(
			entity, {TEXTURE_ASSET_ID::CAT_IDLE_SLIME, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::CAT_IDLE});
		break;
	case Skin::CAT_SKIN_SCH:
		registry.renderRequests.insert(
			entity, {TEXTURE_ASSET_ID::CAT_IDLE_SCH_ALIVE, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::CAT_IDLE});
		break;
	case Skin::CAT_SKIN_RAINBOW:
		registry.renderRequests.insert(
			entity, {TEXTURE_ASSET_ID::CAT_IDLE_RAINBOW, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::CAT_IDLE});
		break;
	}
	

	registry.opacities.emplace(entity, 1.0f);

	return entity;
}

Entity createLine(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity, {TEXTURE_ASSET_ID::TEXTURE_COUNT, EFFECT_ASSET_ID::EGG, GEOMETRY_BUFFER_ID::DEBUG_LINE});

	// Create motion
	Motion &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {0, 0};
	motion.position = position;
	motion.scale = scale;

	registry.debugComponents.emplace(entity);
	return entity;
}

std::vector<ObstacleProps> obstacleProps = {
	{vec2({WALL_BB_WIDTH, WALL_BB_HEIGHT}), TEXTURE_ASSET_ID::WALL}, // WALL
	{vec2({FLOOR_BB_WIDTH, FLOOR_BB_HEIGHT}), TEXTURE_ASSET_ID::FLOOR} // PLATFORM
};

Entity createObstacle(RenderSystem *renderer, vec2 position, ObstacleType type, glm::vec2 cell_scale)

{
	auto entity = Entity();

	//// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	//Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	//registry.meshPtrs.emplace(entity, &mesh);

	// Add wall
	registry.obstacles.emplace(entity);

	// Initialize the motion

	auto &motion = registry.motions.emplace(entity);
	motion.angle = 0.0f;
	motion.velocity = {0, 0};
	motion.position = position;


	//const auto &properties = obstacleProps[static_cast<int>(type)];
	motion.scale = cell_scale;

	// can be ignored eventually. just process the collision information
	// I left it one for now, for debugging purposes (collisions)
	//registry.renderRequests.insert(
	//	entity, {TEXTURE_ASSET_ID::LAB_TILESET, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::LAB_TILESET});

	return entity;
}


Entity createCrosshair()
{ 
	// TODO: add render stuff

	auto entity = Entity();
	auto& crosshair = registry.crosshairs.emplace(entity);

	return entity;
}

Entity createWeapon(RenderSystem *renderer, vec2 pos, float damage, float rate_of_fire, float recoil,
					int magazine_capacity, float reload_time, WEAPON_TYPE weapon_type)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	Motion &motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.scale = vec2{WEAPON_BB_WIDTH, WEAPON_BB_HEIGHT};

	auto &weapon = registry.weapons.emplace(entity);
	weapon.damage = damage;
	weapon.rate_of_fire = rate_of_fire;
	weapon.recoil = recoil;
	weapon.magazine_capactity = magazine_capacity;
	weapon.reload_time_ms = reload_time;
	weapon.reload_timer = reload_time;
	weapon.round_count = magazine_capacity;
	weapon.weapon_type = weapon_type;

	// adding animation
	Animation &anim = registry.animations.emplace(entity);
	if (weapon_type == RIFLE)
	{
		registry.renderRequests.insert(entity, {TEXTURE_ASSET_ID::RIFLE_IDLE, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::RIFLE_IDLE});
	}
	else if (weapon_type == GRENADE_LAUNCHER)
	{
		registry.renderRequests.insert(entity,
			{TEXTURE_ASSET_ID::GRENADE_LAUNCHER_IDLE, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::GRENADE_LAUNCHER_IDLE});
	}
	

	return entity;
}

std::vector<Entity> createEnemyHealthBar(vec2 pos) 
{
	auto inner_entity = Entity();
	auto outer_entity = Entity();

	registry.motions.emplace(inner_entity, Motion{pos, 0.f, {0.f, 0.f}, {77.f, 6.f}, 0.f});
	registry.renderRequests.insert(
		inner_entity, {TEXTURE_ASSET_ID::ENEMY_HEALTH_BAR_INNER, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE});
	registry.enemyHealthBars.emplace(inner_entity);

	registry.motions.emplace(outer_entity, Motion{pos, 0.f, {0.f, 0.f}, {83.f, 13.f}, 0.f});
	registry.renderRequests.insert(
		outer_entity,
		{TEXTURE_ASSET_ID::ENEMY_HEALTH_BAR_OUTER, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE});
	registry.enemyHealthBars.emplace(outer_entity);

	return {inner_entity, outer_entity};
}

Entity createEnemyFlyer(vec2 pos, vec2 left_point, vec2 right_point)
{ 
	auto entity = Entity();

	// Setting initial motion values
	Motion &motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = {0.f, 0.f};
	motion.scale = vec2({ ENEMY_FLYER_BB_WIDTH, ENEMY_FLYER_BB_HEIGHT });

	// adding animation
	Animation &anim = registry.animations.emplace(entity);

	registry.patrolBoxes.emplace(entity, PatrolBox{left_point, right_point});
	registry.deadlys.emplace(entity, Deadly{1});

	auto health_bar_entities = createEnemyHealthBar(pos - vec2{0.f, abs(motion.scale.y) / 2});
	registry.enemies.emplace(entity,
							 Enemy{ENEMY_TYPE::FLYER, random_float(0.f, PATROL_TURN_WAIT),
								   random_float(0.f, SLIME_SHOOT_COOLDOWN), false, 
								   health_bar_entities[0], health_bar_entities[1],
								   33.f,
								   {0.f, 0.f}});
	registry.healths.emplace(entity);
	registry.flyerEnemies.emplace(entity);
	registry.renderRequests.insert(entity, {TEXTURE_ASSET_ID::ENEMY_FLYER, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::ENEMY_FLYER});

	return entity;
}

Entity createEnemyBoid(vec2 pos, vec2 left_point, vec2 right_point)
{
	auto entity = Entity();

	// Setting initial motion values
	Motion &motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = {random_int(0.f, 0.f), random_int(0.f, 0.f)};
	motion.scale = vec2({ENEMY_FLYER_BB_WIDTH, ENEMY_FLYER_BB_HEIGHT});

	// adding animation
	Animation &anim = registry.animations.emplace(entity);

	registry.patrolBoxes.emplace(entity, PatrolBox{left_point, right_point});
	registry.deadlys.emplace(entity, Deadly{1});
	registry.healths.emplace(entity);
	registry.boidEnemies.emplace(entity);

	auto health_bar_entities = createEnemyHealthBar(pos - vec2{0.f, abs(motion.scale.y) / 2});
	registry.enemies.emplace(entity, 
							 Enemy{ENEMY_TYPE::BOID, 
							 0.f, 
							 random_float(0.f, BOID_SLIME_SHOOT_COOLDOWN),
							 false, 
							 health_bar_entities[0], 
							 health_bar_entities[1], 
							 35.f, 
							 {0.f, 0.f}});
	registry.renderRequests.insert(
		entity, {TEXTURE_ASSET_ID::ENEMY_FLYER, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::ENEMY_FLYER});

	return entity;
}

Entity createEnemyCharger(vec2 pos, vec2 left_point, vec2 right_point)
{
	auto entity = Entity();

	// Setting initial motion values
	Motion &motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = {random_int(0.f, 0.f), random_int(0.f, 0.f)};
	motion.scale = vec2({ENEMY_CHARGER_BB_WIDTH, ENEMY_CHARGER_BB_HEIGHT});

	// adding animation
	Animation &anim = registry.animations.emplace(entity);

	registry.patrolBoxes.emplace(entity, PatrolBox{left_point, right_point});

	registry.deadlys.emplace(entity, Deadly{1});

	auto health_bar_entities = createEnemyHealthBar(pos - vec2{0.f, abs(motion.scale.y) / 2});
	registry.enemies.emplace(
		entity, Enemy{ENEMY_TYPE::CHARGER, 0.f, 0.f, false, health_bar_entities[0], health_bar_entities[1], 38.f, {0.f, 10.f}});
	registry.healths.emplace(entity);
	registry.chargerEnemies.emplace(entity);
	registry.renderRequests.insert(
		entity, {TEXTURE_ASSET_ID::ENEMY_CHARGER, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::ENEMY_CHARGER});

	return entity;
}

Entity createEnemyBoss(vec2 pos, vec2 left_point, vec2 right_point)
{
	auto entity = Entity();

	// Setting initial motion values
	Motion &motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f , 0.f };
	motion.scale = vec2({ENEMY_BOSS_BB_WIDTH, ENEMY_BOSS_BB_HEIGHT});

	// adding animation
	Animation &anim = registry.animations.emplace(entity);

	registry.patrolBoxes.emplace(entity, PatrolBox{left_point, right_point});
	registry.deadlys.emplace(entity, Deadly{2});
	registry.enemies.emplace(entity, Enemy{ENEMY_TYPE::BOSS});
	registry.healths.emplace(entity, Health{5}); // boss starts with health of 5
	registry.bossEnemies.emplace(
		entity,
		BossEnemy{pos, 0.f, 0.f, vec3{250.f, 0.f, 0.f},
				  std::vector<vec3>{vec3{322.f, 0.f, 79.f}, vec3{322.f, 0.f, 51.f},
									vec3{322.f, 0.f, 30.f}, vec3{322.f, 0.f, 20.f},
									vec3{322.f, 0.f, 28.f}, vec3{322.f, 0.f, 51.f}, 
									vec3{322.f, 0.f, 61.f}, vec3{322.f, 0.f, 85.f}, 
									vec3{355.f, 0.f, 110.f}, vec3{325.f, 0.f, 81.f}}});
	registry.renderRequests.insert(
		entity, {TEXTURE_ASSET_ID::ENEMY_BOSS_IDLE, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::ENEMY_BOSS_IDLE});

	return entity;
}

Entity createBackground(RenderSystem *renderer, vec2 pos, vec2 scale, TEXTURE_ASSET_ID level_bg)
{
	auto entity = Entity();

	Motion &motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = scale;

	registry.renderRequests.insert(entity, {level_bg, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE});

	registry.backgrounds.emplace(entity);

	return entity;
}

Entity createLevelExit(int index, vec2 left_point, vec2 right_point) 
{
	auto entity = Entity();

	registry.level_out.emplace(entity, LevelOut{index});
	registry.tbox.emplace(entity, TriggerBox{left_point, right_point});

	return entity;
}

Entity createAlphaBox(float opacity, vec2 left_point, vec2 right_point)
{
	auto entity = Entity();

	registry.alpha_box.emplace(entity,AlphaBox{opacity});
	registry.tbox.emplace(entity, TriggerBox{left_point, right_point});

	return entity;
}

Entity createKillBox(int damage, vec2 left_point, vec2 right_point)
{
	auto entity = Entity();

	registry.deadlys.emplace(entity, Deadly{damage});
	registry.tbox.emplace(entity, TriggerBox{left_point, right_point});

	return entity;
}

Entity createText(std::string info, vec2 pos, float scale, vec3 color)
{
	auto entity = Entity();

	Text &text = registry.texts.emplace(entity);
	text.info = info;
	text.color = color;
	std::cout << "TEXT COMPONENT LENGTH" << registry.texts.size() << std::endl;

	Motion &motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = vec2({scale, scale});

	return entity;
}

Entity createCollectableLauncher(RenderSystem *renderer, vec2 pos, COLLECTABLE_TYPE type)
{
	auto entity = Entity();

	//// Store a reference to the potentially re-used mesh object
	//Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	//registry.meshPtrs.emplace(entity, &mesh);

	Motion &motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.scale = vec2{WEAPON_BB_WIDTH, WEAPON_BB_HEIGHT};

	registry.collectables.insert(entity, Collectable{type});

	registry.renderRequests.insert(entity,
								   {TEXTURE_ASSET_ID::GRENADE_LAUNCHER_IDLE, EFFECT_ASSET_ID::TEXTURED,
									GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}

Entity createLore(RenderSystem* renderer, vec2 pos, COLLECTABLE_TYPE type) 
{
	auto entity = Entity();

	//// Store a reference to the potentially re-used mesh object
	//Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	//registry.meshPtrs.emplace(entity, &mesh);

	Motion &motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.scale = vec2{LORE_BB_WIDTH, LORE_BB_HEIGHT};

	registry.collectables.insert(entity, Collectable{type});

	registry.renderRequests.insert(
		entity, {TEXTURE_ASSET_ID::NOTE_PICKUP, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE});

	return entity;
}
