#include "common.hpp"

#include "particle_system.hpp"


std::vector<Entity> ParticleSystem::spawnParticles(vec2 position, vec2 velocity, vec2 scale, float ttl, vec3 color,
												   int num, float angle_range)
{
	// Create a random number generator
	std::random_device rd; // Seed for randomness
	std::mt19937 gen(rd()); // Mersenne Twister generator
	std::uniform_real_distribution<float> angle_dist(-angle_range,
													 angle_range); // Random angle in [-angle_range, angle_range]
	std::uniform_real_distribution<float> scale_dist(0.0f, 1.0f); // Random scale in [0, 1]

	std::vector<Entity> particles;

	for (int i = 0; i < num; ++i) // Use <, not <= to match `num` particles
	{
		// Generate a random angle
		float angle_deg = angle_dist(gen);
		float angle_rad = angle_deg * (M_PI / 180.0f); // Convert degrees to radians

		// Rotate the velocity vector
		vec2 new_velocity = rotate(velocity, angle_rad);

		// Scale the velocity by a random factor between 0 and 1
		float random_scale = scale_dist(gen);
		new_velocity *= random_scale;

		// Create the particle with the new velocity
		Entity particle = ParticleSystem::createParticle(position, new_velocity, scale, ttl, color);
		particles.push_back(particle);
	}
	return particles;
}


//std::vector<Entity> ParticleSystem::spawnParticles(vec2 position, vec2 velocity, vec2 scale, float ttl, vec3 color, int num, float angle_range)
//{
//	// Seed random number generator
//	srand(static_cast<unsigned int>(time(nullptr)));
//
//	std::vector<Entity> particles;
//
//	for (int i = 0; i <= num ; ++i)
//	{
//		// Generate a random angle in the range [-30, 30] degrees
//		float angle_deg = -angle_range + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (angle_range*2)));
//		float angle_rad = angle_deg * (M_PI / 180.0f); // Convert degrees to radians
//
//		// Rotate the velocity vector
//		vec2 new_velocity = rotate(velocity, angle_rad);
//
//		// Create the particle with the new velocity
//		Entity particle = ParticleSystem::createParticle(position, new_velocity, scale, ttl, color);
//		particles.push_back(particle);
//	}
//	return particles;
//}

Entity ParticleSystem::createParticle(vec2 position, vec2 velocity, vec2 scale, float ttl, vec3 color) 
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity, {TEXTURE_ASSET_ID::TEXTURE_COUNT, EFFECT_ASSET_ID::EGG, GEOMETRY_BUFFER_ID::PARTICLE});

	// Create motion
	Motion &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = velocity;
	motion.position = position;
	motion.scale = scale;

	// Create particle
	Particle &particle = registry.particles.emplace(entity, Particle{ttl});

	// Set color
	registry.colors.insert(entity, color);

	// Put it in the gravity system
	registry.gravities.emplace(entity);

	return entity;
}

void ParticleSystem::step(float elapsed_ms)
{
	//clear particles after ttl expires
	for (Entity entity : registry.particles.entities)
	{
		Particle &particle = registry.particles.get(entity);
		particle.counter_ttl -= elapsed_ms;
		if (particle.counter_ttl <= 0)
		{
			std::cout << particle.counter_ttl << std::endl;
			registry.remove_all_components_of(entity);
		}
	}
}

