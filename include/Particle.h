#pragma once

#include "cinder/gl/gl.h"
#include "cinder/Vector.h"

class Particle
{
	public:
		Particle(const std::map<std::string, std::string>& config, ci::vec2 init_pos, ci::vec2 init_Vel);

		void update();
		void draw();

		void flock(std::vector<Particle*>& particles);
		ci::vec2 steer(ci::vec2 target);
		void borders(float width, float height);
		ci::vec2 separate(std::vector<Particle*> & particles);
		ci::vec2 align(std::vector<Particle*>& particles);
		ci::vec2 cohesion(std::vector<Particle*>& particles);

		ci::vec2 position, prevPosition;
		ci::vec2 forces;
		ci::vec2 velocity;

		float radius;
		float mass;
		float drag;
		float maxspeed;
		float maxforce;
};