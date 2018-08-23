#pragma once

#include "Particle.h"
#include "UpdateParams.h"
#include <vector>

class ParticleSystem{
public:
	~ParticleSystem();

	void update(const UpdateParams&);
	void draw();

	void addParticle(Particle *particle);
	void destroyParticle(Particle *particle);

	std::vector<Particle*> particles;
};