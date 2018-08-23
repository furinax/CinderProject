#include "ParticleSystem.h"
#include "cinder/app/App.h"

ParticleSystem::~ParticleSystem(){
	for_each(particles.begin(), particles.end(), [](Particle* p) {delete p; });
	particles.clear();
}

void ParticleSystem::update(const UpdateParams& updateParams){
	for_each(particles.begin(), particles.end(), [&](Particle* p) { 
		p->flock(particles); 
		p->update(updateParams);
		p->borders(ci::app::getWindowWidth(), ci::app::getWindowHeight()); 
	});

	std::vector<Particle*> deadParticles;
	std::copy_if(particles.begin(), particles.end(), std::back_inserter(deadParticles), [](Particle* p){ return p->isDead; });
	for_each(deadParticles.begin(), deadParticles.end(), [this](Particle* p){ this->destroyParticle(p); });
}

void ParticleSystem::draw(){
	for_each(particles.begin(), particles.end(), [](Particle* p) {p->draw();});
}

void ParticleSystem::addParticle(Particle *particle){
	particles.push_back(particle);
}

void ParticleSystem::destroyParticle(Particle *particle){
	std::vector<Particle*>::iterator it = std::find(particles.begin(), particles.end(), particle);
	delete *it;
	particles.erase(it);
}