#include "Particle.h"
#include "cinder/app/App.h"
#include <algorithm>
#include <numeric>

Particle::Particle(const ci::vec2 & position, float radius, float mass, float drag)
{
	this->position = position;
	this->radius = radius;
	this->mass = mass;
	this->drag = drag;
	this->maxspeed = 3.f;
	this->maxforce = 0.5f;
	this->velocity = ci::vec2(0);
	prevPosition = position;
	forces = cinder::vec2(0, 0);
}

void Particle::update()
{
	ci::vec2 temp = position;
	position += velocity + forces / mass;
	prevPosition = temp;
	forces = ci::vec2(0, 0);
}

void Particle::draw()
{
	ci::gl::color(1.f, 1.f, 1.f);
	ci::gl::drawSolidCircle(position, radius);
	ci::gl::color(1.f, 0.f, 0.f);
	ci::gl::drawLine(position, position + (position - prevPosition) * (radius + 5.f));
}

void Particle::flock(std::vector<Particle*>& particles)
{
	ci::vec2 acc = ci::vec2(0);
	acc += separate(particles);
	acc += align(particles);
	acc += cohesion(particles)*.1f;

	velocity += acc;
	
	return;
}

ci::vec2 Particle::steer(ci::vec2 target)
{
	ci::vec2 steerVector;
	ci::vec2 desired = target - position;
	float d = glm::length(desired);
	if (d > 0)
	{
		desired = glm::normalize(desired);
		desired *= std::max(0.f, std::min(maxspeed * (d / 100.f), maxspeed));
		steerVector = glm::clamp(ci::vec2(0), ci::vec2(maxforce), desired - velocity);
	}

	return steerVector;
}

ci::vec2 Particle::separate(std::vector<Particle*> & particles)
{
	ci::vec2 averageVec = (position - ci::app::getWindowCenter()) * -.005f;
	
	averageVec = glm::rotate(averageVec, glm::radians(-45.f));
	float targetSeparation = 30.f;

	std::vector<Particle*> particlesWithinDistance;
	auto it = std::copy_if(particles.begin(), particles.end(), std::back_inserter(particlesWithinDistance), [&](Particle* p){
		return glm::distance(position, p->position) < targetSeparation; });
	
	if (particlesWithinDistance.size() == 0)
		return averageVec;
	
	averageVec = std::accumulate(particlesWithinDistance.begin(), particlesWithinDistance.end(), averageVec, [=](ci::vec2 x, Particle* p){
		ci::vec2 diffVec = position - p->position;
		if (glm::length(diffVec) > 0)
			x += glm::normalize(diffVec) / glm::length(diffVec);
		return x;
	});
	averageVec /= (float)particlesWithinDistance.size();
	
	
	if (glm::length(averageVec) > 0)
	{
		averageVec = glm::normalize(averageVec);
		averageVec *-maxspeed;
		averageVec -= velocity;
		averageVec = glm::clamp(ci::vec2(0), ci::vec2(maxforce), averageVec); 
	}
	return averageVec;
}

ci::vec2 Particle::align(std::vector<Particle*>& particles)
{
	return ci::vec2(0);
}

ci::vec2 Particle::cohesion(std::vector<Particle*>& particles)
{
	ci::vec2 averageVec;
	float neighborDist = 50.f;

	std::vector<Particle*> particlesWithinDistance;
	auto it = std::copy_if(particles.begin(), particles.end(), std::back_inserter(particlesWithinDistance), [&](Particle* p){
							float d = glm::distance(position, p->position);
							return d < neighborDist; });

	averageVec = std::accumulate(particlesWithinDistance.begin(), particlesWithinDistance.end(), ci::vec2(0), [](ci::vec2 x, Particle* p){ x += p->position; return x; });

	return particlesWithinDistance.size() > 0 ? steer(averageVec / (float)particlesWithinDistance.size()) : ci::vec2(0);
}

void Particle::borders(float width, float height)
{
	if ( position.x < -radius )
		position.x = width + radius;
	if (position.y < -radius)
		position.y = height + radius;
	if (position.x > width + radius)
		position.x = -radius;
	if (position.y > height + radius)
		position.y = -radius;
}