#include "Particle.h"
#include "cinder/app/App.h"

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
	//acc = (position - ci::app::getWindowCenter()) * -.01f;
	acc += separate(particles);
	acc += align(particles);
	acc += cohesion(particles)*.1f;


	velocity += acc;
	
	return;
}

ci::vec2 Particle::steer(ci::vec2 target, bool slowdown)
{
	ci::vec2 x;
	ci::vec2 desired = target - position;
	float d = glm::length(desired);
	if (d > 0)
	{
		desired = glm::normalize(desired);
		if ((slowdown) && (d < 100.0))
			desired *= (maxspeed * (d / 100.0));
		else
			desired *= maxspeed;
		x = desired - velocity;
		x = glm::clamp(ci::vec2(0), ci::vec2(maxforce), x);
	}
	else
	{
		x = ci::vec2(0);
	}

	return x;
}

ci::vec2 Particle::separate(std::vector<Particle*> & particles)
{
	ci::vec2 x = (position - ci::app::getWindowCenter()) * -.005f;
	x = glm::rotate(x, glm::radians(-45.f));

	float targetSeparation = 30.f;
	int count = 0;
	for (std::vector<Particle*>::iterator it = particles.begin(); it != particles.end(); ++it) {
		ci::vec2 diffVec = position - (*it)->position;
		float l = glm::length(diffVec);
		if (l > 0 && l < targetSeparation)
		{
			x += glm::normalize(diffVec) / l;
			count++;
		}
	}

	if (count > 0) {
		x /= (float)count;
	}

	if (glm::length(x) > 0)
	{
		x = glm::normalize(x);
		x *-maxspeed;
		x -= velocity;
		x = glm::clamp(ci::vec2(0), ci::vec2(maxforce), x);
	}

	return x;
}

ci::vec2 Particle::align(std::vector<Particle*>& particles)
{
	return ci::vec2(0);
}

ci::vec2 Particle::cohesion(std::vector<Particle*>& particles)
{
	ci::vec2 x;
	float neighborDist = 50.f;
	int count = 0;
	for (std::vector<Particle*>::iterator it = particles.begin(); it != particles.end(); ++it)
	{
		float d = glm::distance(position, (*it)->position);
		if (d > 0 && d < neighborDist) {
			x += (*it)->position;
			count++;
		}
	}

	if (count > 0) {
		x /= (float)count;
		return steer(x, false);
	}
	return ci::vec2(0);
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