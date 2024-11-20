#include "common.hpp"

#include "particle_system.hpp"
std::vector<Entity> ParticleSystem::spawnParticles(vec2 position, vec2 velocity, vec2 scale, float ttl, vec3 color, int num)
{
	// Seed random number generator
	srand(static_cast<unsigned int>(time(nullptr)));

	std::vector<Entity> particles;

	for (int i = 0; i <= num ; ++i)
	{
		// Generate a random angle in the range [-30, 30] degrees
		float angle_deg = -60.f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 120.f));
		float angle_rad = angle_deg * (M_PI / 180.0f); // Convert degrees to radians

		// Rotate the velocity vector
		vec2 new_velocity = rotate(velocity, angle_rad);

		// Create the particle with the new velocity
		Entity particle = ParticleSystem::createParticle(position, new_velocity, scale, ttl, color);
		particles.push_back(particle);
	}
	return particles;
}

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
	Particle& particle = registry.particles.emplace(entity);
	particle.counter_ttl = ttl;

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
		if (particle.counter_ttl < 0)
		{
			registry.remove_all_components_of(entity);
		}
	}
}

