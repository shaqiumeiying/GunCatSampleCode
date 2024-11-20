#pragma once

// internal
#include "common.hpp"
#include "engine/tiny_ecs.hpp"
#include "engine/components.hpp"
#include "engine/tiny_ecs_registry.hpp"
#include "renderer/render_system.hpp"


// stlib
#include <vector>
#include <random>

class ParticleSystem
{
public:
	ParticleSystem() = default;
	static std::vector<Entity> spawnParticles(vec2 position, vec2 velocity, vec2 scale, float ttl, vec3 color,
													   int num);
	static Entity createParticle(vec2 position, vec2 velocity, vec2 scale, float ttl, vec3 color);
	void step(float elapsed_ms);
};
