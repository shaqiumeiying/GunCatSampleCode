#include "animation_system.hpp"

void AnimationSystem::applyAnimation(Entity entity, float elapsed_ms, int &frame_current, GLfloat &frame_width,
									 WorldSystem &world)
{
	if (registry.players.has(entity))
	{
		handlePlayerAnimation(entity, elapsed_ms, frame_current, frame_width, world);
	}
	else if (registry.enemies.has(entity))
	{
		handleEnemyAnimation(entity, elapsed_ms, frame_current, frame_width);
	}
	else if (registry.weapons.has(entity))
	{
		handleWeaponAnimation(entity, elapsed_ms, frame_current, frame_width);
	}
	else if (registry.grenades.has(entity))
	{
		handleGrenadeAnimation(entity, elapsed_ms, frame_current, frame_width);
	}
}

// handle player animation
void AnimationSystem::handlePlayerAnimation(Entity entity, float elapsed_ms, int &frame_current, GLfloat &frame_width,
											WorldSystem &world)
{
	auto &player = registry.players.get(entity);
	auto &animation = registry.animations.get(entity);
	auto &renderRequest = registry.renderRequests.get(entity);

	animation.frame_counter -= elapsed_ms;


	/*if (animation.type == DEATH && player.is_dead)
	{
		setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
					 GEOMETRY_BUFFER_ID::CAT_DEATH, TEXTURE_ASSET_ID::CAT_DEATH, 1, frame_width);
		return;
	}*/

	switch (world.selected_skin)
	{
	case Skin::DEFAULT:
		switch (animation.type)

		{
		case IDLE:
			if (player.is_on_ground)
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::CAT_IDLE, TEXTURE_ASSET_ID::CAT_IDLE, CAT_IDLE_WALK_FRAME,frame_width);
        updateAnimationFrame(animation.frame_counter, animation.frame_current, CAT_IDLE_WALK_FRAME, 100, true);
        break;

		case WALK:
			if (player.is_on_ground)
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::CAT_WALK, TEXTURE_ASSET_ID::CAT_WALK, CAT_IDLE_WALK_FRAME, frame_width);
        updateAnimationFrame(animation.frame_counter, animation.frame_current, CAT_IDLE_WALK_FRAME, 100, true);
        break;

		case JUMP:
			if (!player.is_on_ground)
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::CAT_JUMP, TEXTURE_ASSET_ID::CAT_JUMP, CAT_JUMP_FRAME, frame_width);
        updateAnimationFrame(animation.frame_counter, animation.frame_current, CAT_JUMP_FRAME, 1, false);
        break;
		case SPIN: 
			if (player.reflect_active)
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::CAT_SPIN, TEXTURE_ASSET_ID::CAT_SPIN, CAT_SPIN_FRAME, frame_width);
			updateAnimationFrame(animation.frame_counter, animation.frame_current, CAT_SPIN_FRAME, 50, false);
			break;
		case DEATH:
			if (player.is_dead)
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::CAT_DEATH, TEXTURE_ASSET_ID::CAT_DEATH, 1, frame_width);
			return;
		default:
			break;
		}
	case Skin::CAT_SKIN_XMAS:
		switch (animation.type)
		{
		case IDLE:
			if (player.is_on_ground)
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::CAT_IDLE, TEXTURE_ASSET_ID::CAT_IDLE_XMAS, CAT_IDLE_WALK_FRAME,
							 frame_width);
			updateAnimationFrame(animation.frame_counter, animation.frame_current, CAT_IDLE_WALK_FRAME, 100, true);
			break;

		case WALK:
			if (player.is_on_ground)
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::CAT_WALK, TEXTURE_ASSET_ID::CAT_WALK_XMAS, CAT_IDLE_WALK_FRAME,
							 frame_width);
			updateAnimationFrame(animation.frame_counter, animation.frame_current, CAT_IDLE_WALK_FRAME, 100, true);
			break;

		case JUMP:
			if (!player.is_on_ground)
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::CAT_JUMP, TEXTURE_ASSET_ID::CAT_JUMP_XMAS, CAT_JUMP_FRAME,
							 frame_width);
			updateAnimationFrame(animation.frame_counter, animation.frame_current, CAT_JUMP_FRAME, 1, false);
			break;
		case SPIN:
			if (player.reflect_active)
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::CAT_SPIN, TEXTURE_ASSET_ID::CAT_SPIN_XMAS, CAT_SPIN_FRAME,
							 frame_width);
			updateAnimationFrame(animation.frame_counter, animation.frame_current, CAT_SPIN_FRAME, 50, false);
			break;
		case DEATH:
			if (player.is_dead)
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::CAT_DEATH, TEXTURE_ASSET_ID::CAT_DEATH_XMAS, 1, frame_width);
			return;
		default:
			break;
		}
	case Skin::CAT_SKIN_SLIME:
		switch (animation.type)
		{
		case IDLE:
			if (player.is_on_ground)
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::CAT_IDLE, TEXTURE_ASSET_ID::CAT_IDLE_SLIME, CAT_IDLE_WALK_FRAME,
							 frame_width);
			updateAnimationFrame(animation.frame_counter, animation.frame_current, CAT_IDLE_WALK_FRAME, 100, true);
			break;

		case WALK:
			if (player.is_on_ground)
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::CAT_WALK, TEXTURE_ASSET_ID::CAT_WALK_SLIME, CAT_IDLE_WALK_FRAME,
							 frame_width);
			updateAnimationFrame(animation.frame_counter, animation.frame_current, CAT_IDLE_WALK_FRAME, 100, true);
			break;

		case JUMP:
			if (!player.is_on_ground)
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::CAT_JUMP, TEXTURE_ASSET_ID::CAT_JUMP_SLIME, CAT_JUMP_FRAME,
							 frame_width);
			updateAnimationFrame(animation.frame_counter, animation.frame_current, CAT_JUMP_FRAME, 1, false);
			break;
		case SPIN:
			if (player.reflect_active)
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::CAT_SPIN, TEXTURE_ASSET_ID::CAT_SPIN_SLIME, CAT_SPIN_FRAME,
							 frame_width);
			updateAnimationFrame(animation.frame_counter, animation.frame_current, CAT_SPIN_FRAME, 50, false);
			break;
		case DEATH:
			if (player.is_dead)
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::CAT_DEATH, TEXTURE_ASSET_ID::CAT_DEATH_SLIME, 1, frame_width);
			return;
		default:
			break;
		}
	case Skin::CAT_SKIN_SCH:
		{
			// Static variables for toggling logic
			static float toggle_timer = 0.0f;
			static float random_interval = 0.0f;
			static bool is_alive = true; // Tracks alive/dead
			static std::default_random_engine rng(std::random_device{}());
			static std::uniform_real_distribution<float> interval_distribution(500.0f, 2000.0f);

			toggle_timer += elapsed_ms;

			// random interval
			if (random_interval == 0.0f)
			{
				random_interval = interval_distribution(rng);
			}

			// toggling only after the interval has elapsed
			if (toggle_timer >= random_interval)
			{
				is_alive = !is_alive;

				// reset the timer and MAKE a new random interval
				toggle_timer = 0.0f;
				random_interval = interval_distribution(rng);
			}
			switch (animation.type)
			{
			case IDLE:
				if (player.is_on_ground)
					setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
								 GEOMETRY_BUFFER_ID::CAT_IDLE,
								 is_alive ? TEXTURE_ASSET_ID::CAT_IDLE_SCH_ALIVE : TEXTURE_ASSET_ID::CAT_IDLE_SCH_DEAD,
								 CAT_IDLE_WALK_FRAME, frame_width);
				updateAnimationFrame(animation.frame_counter, animation.frame_current, CAT_IDLE_WALK_FRAME, 100, true);
				break;

			case WALK:
				if (player.is_on_ground)
					setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
								 GEOMETRY_BUFFER_ID::CAT_WALK,
								 is_alive ? TEXTURE_ASSET_ID::CAT_WALK_SCH_ALIVE : TEXTURE_ASSET_ID::CAT_WALK_SCH_DEAD,
								 CAT_IDLE_WALK_FRAME, frame_width);
				updateAnimationFrame(animation.frame_counter, animation.frame_current, CAT_IDLE_WALK_FRAME, 100, true);
				break;

			case JUMP:
				if (!player.is_on_ground)
					setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
								 GEOMETRY_BUFFER_ID::CAT_JUMP,
								 is_alive ? TEXTURE_ASSET_ID::CAT_JUMP_SCH_ALIVE : TEXTURE_ASSET_ID::CAT_JUMP_SCH_DEAD,
								 CAT_JUMP_FRAME, frame_width);
				updateAnimationFrame(animation.frame_counter, animation.frame_current, CAT_JUMP_FRAME, 1, false);
				break;

			case SPIN:
				if (player.reflect_active)
					setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
								 GEOMETRY_BUFFER_ID::CAT_SPIN,
								 is_alive ? TEXTURE_ASSET_ID::CAT_SPIN_SCH_ALIVE : TEXTURE_ASSET_ID::CAT_SPIN_SCH_DEAD,
								 CAT_SPIN_FRAME, frame_width);
				updateAnimationFrame(animation.frame_counter, animation.frame_current, CAT_SPIN_FRAME, 50, false);
				break;
			case DEATH:
				if (player.is_dead)
					setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
								 GEOMETRY_BUFFER_ID::CAT_DEATH, TEXTURE_ASSET_ID::CAT_DEATH_SCH, 1, frame_width);
				return;
			default:
				break;
			}
			break;
		}
	case Skin::CAT_SKIN_RAINBOW:
		switch (animation.type)
		{
		case IDLE:
			if (player.is_on_ground)
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::CAT_IDLE, TEXTURE_ASSET_ID::CAT_IDLE_RAINBOW, CAT_IDLE_WALK_FRAME,
							 frame_width);
			updateAnimationFrame(animation.frame_counter, animation.frame_current, CAT_IDLE_WALK_FRAME, 100, true);
			break;

		case WALK:
			if (player.is_on_ground)
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::CAT_WALK, TEXTURE_ASSET_ID::CAT_WALK_RAINBOW, CAT_IDLE_WALK_FRAME,
							 frame_width);
			updateAnimationFrame(animation.frame_counter, animation.frame_current, CAT_IDLE_WALK_FRAME, 100, true);
			break;

		case JUMP:
			if (!player.is_on_ground)
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::CAT_JUMP, TEXTURE_ASSET_ID::CAT_JUMP_RAINBOW, CAT_JUMP_FRAME,
							 frame_width);
			updateAnimationFrame(animation.frame_counter, animation.frame_current, CAT_JUMP_FRAME, 1, false);
			break;
		case SPIN:
			if (player.reflect_active)
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::CAT_SPIN, TEXTURE_ASSET_ID::CAT_SPIN_RAINBOW, CAT_SPIN_FRAME,
							 frame_width);
			updateAnimationFrame(animation.frame_counter, animation.frame_current, CAT_SPIN_FRAME, 50, false);
			break;
		case DEATH:
			if (player.is_dead)
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::CAT_DEATH, TEXTURE_ASSET_ID::CAT_DEATH_RAINBOW, 1, frame_width);
			return;
		default:
			break;
		}
		break;
	}
	frame_current = animation.frame_current;
}

// handle enemy animation
void AnimationSystem::handleEnemyAnimation(Entity entity, float elapsed_ms, int &frame_current, GLfloat &frame_width)
{
	// enemy bullets do not have animation
	//if (registry.enemyBullets.has(entity))
	//	return;
	
	auto &enemy = registry.enemies.get(entity);
	auto &animation = registry.animations.get(entity);	
	auto &renderRequest = registry.renderRequests.get(entity);

	animation.frame_counter -= elapsed_ms;
	int animation_type = animation.type;

	switch (enemy.enemy_type)
	{
	case ENEMY_TYPE::FLYER:
	case ENEMY_TYPE::BOID:
		switch (animation.type)
		{
		case IDLE:
			setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
						 GEOMETRY_BUFFER_ID::ENEMY_FLYER, TEXTURE_ASSET_ID::ENEMY_FLYER, ENEMY_FLYER_IDLE_FRAME,
						 frame_width);
			updateAnimationFrame(animation.frame_counter, animation.frame_current, ENEMY_FLYER_IDLE_FRAME, 75, true);
			break;

		case DEATH:
			setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
						 GEOMETRY_BUFFER_ID::ENEMY_FLYER_DEATH, TEXTURE_ASSET_ID::ENEMY_FLYER_DEATH,
						 ENEMY_FLYER_DEATH_FRAME, frame_width);
			updateAnimationFrame(animation.frame_counter, animation.frame_current, ENEMY_FLYER_DEATH_FRAME, 100, false);
			break;
		default:
			assert(false, "animation not supported");
			break;
		}
		break;
	case ENEMY_TYPE::CHARGER:
		switch (animation.type)
		{
		case IDLE:
			setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
						 GEOMETRY_BUFFER_ID::ENEMY_CHARGER, TEXTURE_ASSET_ID::ENEMY_CHARGER, ENEMY_CHARGER_IDLE_FRAME,
						 frame_width);
			updateAnimationFrame(animation.frame_counter, animation.frame_current, ENEMY_CHARGER_IDLE_FRAME, 75, true);
			break;

		case ATTACK:
			setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
						 GEOMETRY_BUFFER_ID::ENEMY_CHARGER_ATTACK, TEXTURE_ASSET_ID::ENEMY_CHARGER_ATTACK, ENEMY_CHARGER_ATTACK_FRAME,
						 frame_width);
			updateAnimationFrame(animation.frame_counter, animation.frame_current, ENEMY_CHARGER_ATTACK_FRAME, 75, true);
			break;

		case DEATH:
			setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
						 GEOMETRY_BUFFER_ID::ENEMY_CHARGER_DEATH, TEXTURE_ASSET_ID::ENEMY_CHARGER_DEATH,
						 ENEMY_CHARGER_DEATH_FRAME, frame_width);
			updateAnimationFrame(animation.frame_counter, animation.frame_current, ENEMY_CHARGER_DEATH_FRAME, 100, false);
			break;
		default:
			assert(false, "animation not supported");
			break;
		}
		break;
	case ENEMY_TYPE::BOSS:
		switch (animation.type)
		{
		case IDLE:
		
			setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
						 GEOMETRY_BUFFER_ID::ENEMY_BOSS_IDLE, TEXTURE_ASSET_ID::ENEMY_BOSS_IDLE, ENEMY_BOSS_IDLE_FRAME,
						 frame_width);

			updateAnimationFrame(animation.frame_counter, animation.frame_current, ENEMY_BOSS_IDLE_FRAME, 100, true);
			break;
		case DEATH:
			setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
						 GEOMETRY_BUFFER_ID::ENEMY_BOSS_DEATH, TEXTURE_ASSET_ID::ENEMY_BOSS_DEATH,
						 ENEMY_BOSS_DEATH_FRAME, frame_width);
			updateAnimationFrame(animation.frame_counter, animation.frame_current, ENEMY_BOSS_DEATH_FRAME, 100, false);
			break;
		case JUMP:
			setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
						 GEOMETRY_BUFFER_ID::ENEMY_BOSS_JUMP, TEXTURE_ASSET_ID::ENEMY_BOSS_JUMP, ENEMY_BOSS_JUMP_FRAME,
						 frame_width);

			updateAnimationFrame(animation.frame_counter, animation.frame_current, ENEMY_BOSS_JUMP_FRAME, 1, false);
			break;
		default:
			assert(false, "animation not supported");
			break;
		}
		break;
	default:
		assert(false, "animation not supported");
	}

	frame_current = animation.frame_current;
}



// handle weapon animation
void AnimationSystem::handleWeaponAnimation(Entity entity, float elapsed_ms, int &frame_current, GLfloat &frame_width)
{
	auto &weapon = registry.weapons.get(entity);
	auto &animation = registry.animations.get(entity);
	auto &renderRequest = registry.renderRequests.get(entity);

	animation.frame_counter -= elapsed_ms;

		switch (animation.type)
		{
		case IDLE:
			if (weapon.weapon_type == RIFLE)
			{
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::RIFLE_IDLE, TEXTURE_ASSET_ID::RIFLE_IDLE, 1, frame_width);
				updateAnimationFrame(animation.frame_counter, animation.frame_current, WEAPON_ATTACK_FRAME, 1, false);
			}
			else if (weapon.weapon_type == GRENADE_LAUNCHER)
			{
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
									 GEOMETRY_BUFFER_ID::GRENADE_LAUNCHER_IDLE, TEXTURE_ASSET_ID::GRENADE_LAUNCHER_IDLE, 1, frame_width);
				updateAnimationFrame(animation.frame_counter, animation.frame_current, 1, 1, false);
			}

			break;

		case ATTACK:
			if (weapon.weapon_type == RIFLE)
			{
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::RIFLE_ATTACK, TEXTURE_ASSET_ID::RIFLE_ATTACK, 5, frame_width);
				updateAnimationFrame(animation.frame_counter, animation.frame_current, WEAPON_ATTACK_FRAME, 45, false);
			}
			else if (weapon.weapon_type == GRENADE_LAUNCHER)
			{
				setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
							 GEOMETRY_BUFFER_ID::GRENADE_LAUNCHER_ATTACK, TEXTURE_ASSET_ID::GRENADE_LAUNCHER_ATTACK,
							 GRENADE_LAUNCHER_ATTACK_FRAME,
							 frame_width);
				updateAnimationFrame(animation.frame_counter, animation.frame_current, GRENADE_LAUNCHER_ATTACK_FRAME,
									 45, false);
			}
		default:
			break;
		}

	frame_current = animation.frame_current;
}

void AnimationSystem::handleGrenadeAnimation(Entity entity, float elapsed_ms, int &frame_current, GLfloat &frame_width)
{
	auto &grenade = registry.grenades.get(entity);
	auto &animation = registry.animations.get(entity);
	auto &renderRequest = registry.renderRequests.get(entity);
	auto &motion = registry.motions.get(entity);
	

	animation.frame_counter -= elapsed_ms;

	if (animation.type == EXPLODE)
	{
		motion.angle = 0;
		setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
					 GEOMETRY_BUFFER_ID::GRENADE_EXPLODE, TEXTURE_ASSET_ID::GRENADE_EXPLODE, GRENADE_EXPLODE_FRAME,
					 frame_width);
		updateAnimationFrame(animation.frame_counter, animation.frame_current, GRENADE_EXPLODE_FRAME, 25, false);
	}
	frame_current = animation.frame_current;

	if (frame_current == GRENADE_EXPLODE_FRAME - 1)
	{
		grenade.exploded = true;
	}
	
}

// set animation along with corresponding geometry and texture
void AnimationSystem::setAnimation(GEOMETRY_BUFFER_ID &currGeometry, TEXTURE_ASSET_ID &currTexture, int &currFrame,
								   GEOMETRY_BUFFER_ID targetGeometry, TEXTURE_ASSET_ID targetTexture, int total_frames,
								   GLfloat &frame_width)
{
	if (currGeometry != targetGeometry)
	{
		currFrame = 0;
		currGeometry = targetGeometry;
		currTexture = targetTexture;
	}
	frame_width = 1.f / total_frames;
}

// with loop
void AnimationSystem::updateAnimationFrame(float &frame_counter, int &currFrame, int total_frame, int frame_time,
										   bool loop = true)
{
	if (frame_counter <= 0)
	{
		if (loop)
		{
			currFrame = (currFrame + 1) % total_frame;
		}
		else
		{
			if (currFrame < total_frame - 1)
			{
				currFrame++;
			}
		}
		frame_counter = frame_time;
	}
}


// set sprite if static
void AnimationSystem::setSprite(TEXTURE_ASSET_ID &currTexture, TEXTURE_ASSET_ID targetTexture)
{
	//if (currTexture != targetTexture)
	//{
	//	currTexture = targetTexture;
	//}
}
