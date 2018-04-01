#include "Particle.h"

Particle::Particle(const ci::vec2 & position, float radius, float mass, float drag)
{
	this->position = position;
	this->radius = radius;
	this->mass = mass;
	this->drag = drag;
	prevPosition = position;
	forces = cinder::vec2(0, 0);
}

void Particle::update()
{
	ci::vec2 temp = position;
	ci::vec2 vel = (position - prevPosition) * drag;
	position += vel + forces / mass;
	prevPosition = temp;
	forces = ci::vec2(0, 0);
}

void Particle::draw()
{
	ci::gl::color(1.f, 1.f, 1.f);
	ci::gl::drawSolidCircle(position, radius);
}

