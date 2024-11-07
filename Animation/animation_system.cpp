#include "animation_system.hpp"

void AnimationSystem::applyAnimation(Entity entity, float elapsed_ms, int &frame_current, GLfloat &frame_width)
{
	if (registry.players.has(entity))
	{
		handlePlayerAnimation(entity, elapsed_ms, frame_current, frame_width);
	}
	else if (registry.enemies.has(entity))
	{
		handleEnemyAnimation(entity, elapsed_ms, frame_current, frame_width);
	}
	else if (registry.weapons.has(entity))
	{
		handleWeaponAnimation(entity, elapsed_ms, frame_current, frame_width);
	}
}

// handle player animation
void AnimationSystem::handlePlayerAnimation(Entity entity, float elapsed_ms, int &frame_current, GLfloat &frame_width)
{
	auto &player = registry.players.get(entity);
	auto &animation = registry.animations.get(entity);
	auto &renderRequest = registry.renderRequests.get(entity);

	animation.frame_counter -= elapsed_ms;

	switch (animation.type)
	{
	case IDLE:
		if (player.is_on_ground)
			setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
						 GEOMETRY_BUFFER_ID::CAT_IDLE, TEXTURE_ASSET_ID::CAT_IDLE, CAT_IDLE_WALK_FRAME, frame_width);
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

	default:
		break;
	}

	frame_current = animation.frame_current;
}

// handle enemy animation
void AnimationSystem::handleEnemyAnimation(Entity entity, float elapsed_ms, int &frame_current, GLfloat &frame_width)
{
	auto &enemy = registry.enemies.get(entity);
	auto &animation = registry.animations.get(entity);	
	auto &renderRequest = registry.renderRequests.get(entity);

	animation.frame_counter -= elapsed_ms;
	int animation_type = animation.type;

	setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
				 GEOMETRY_BUFFER_ID::ENEMY_FLYER, TEXTURE_ASSET_ID::ENEMY_FLYER, ENEMY_FLYER_IDLE_FRAME, frame_width);

	updateAnimationFrame(animation.frame_counter, animation.frame_current, ENEMY_FLYER_IDLE_FRAME, 75, true);
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
		setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
					 GEOMETRY_BUFFER_ID::WEAPON_IDLE, TEXTURE_ASSET_ID::WEAPON_IDLE, 1, frame_width);
		updateAnimationFrame(animation.frame_counter, animation.frame_current, WEAPON_ATTACK_FRAME, 1, false);
		break;

	case ATTACK:
		setAnimation(renderRequest.used_geometry, renderRequest.used_texture, animation.frame_current,
					 GEOMETRY_BUFFER_ID::WEAPON_ATTACK, TEXTURE_ASSET_ID::WEAPON_ATTACK, 5, frame_width);
		updateAnimationFrame(animation.frame_counter, animation.frame_current, WEAPON_ATTACK_FRAME, 45, false);
		break;
	default:
		break;
	}

	frame_current = animation.frame_current;
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

//// update animation frame
//void AnimationSystem::updateAnimationFrame(float &frame_counter, int &currFrame, int total_frame, int frame_time)
//{
//	if (frame_counter <= 0)
//	{
//		currFrame = (currFrame + 1) % total_frame;
//		frame_counter = frame_time;
//	}
//}

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


