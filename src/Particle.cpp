#include "Particle.h"
#include "cinder/app/App.h"
#include <algorithm>
#include <numeric>

Particle::Particle(const std::map<std::string, std::string>& config, 
	ci::vec2 init_pos = ci::vec2(0), 
	ci::vec2 init_vel= ci::vec2(0))
{
	
	this->radius = std::stof(config.find("radius")->second);
	this->mass = std::stof(config.find("mass")->second);
	this->drag = std::stof(config.find("drag")->second);
	this->maxspeed = std::stof(config.find("maxspeed")->second);
	this->maxforce = std::stof(config.find("maxforce")->second);
	this->gravity = std::stof(config.find("gravity")->second);
	this->lifespan = std::stoi(config.find("lifespan")->second);
	this->jitter = std::stof(config.find("jitter")->second);
	this->separation = std::stof(config.find("separation")->second);
	this->attraction = std::stof(config.find("attraction")->second);
	this->position = init_pos;
	this->velocity = init_vel;
	prevPosition = position;
	forces = cinder::vec2(0, 0);
	t = 0;
	isDead = false;
}

void Particle::update()
{
	ci::vec2 temp = position;
	position += velocity + forces / mass;
	prevPosition = temp;
	forces += ci::vec2(0, this->gravity);
	forces = ci::vec2(0, 0);

	//if (t++ > lifespan)
	//	isDead = true;
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

void Particle::applyForce(ci::vec2 f)
{
	forces += f;
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
	ci::vec2 averageVec;

	std::vector<Particle*> particlesWithinDistance;
	auto it = std::copy_if(particles.begin(), particles.end(), std::back_inserter(particlesWithinDistance), [&](Particle* p){
		return glm::distance(position, p->position) < separation; });
	
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

	std::vector<Particle*> particlesWithinDistance;
	auto it = std::copy_if(particles.begin(), particles.end(), std::back_inserter(particlesWithinDistance), [&](Particle* p){
							float d = glm::distance(position, p->position);
							return d < attraction; });

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