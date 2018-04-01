#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "ParticleSystem.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CinderProjectApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseMove(MouseEvent event) override;
	void update() override;
	void draw() override;

private:
	void setupParticleSystem(ParticleSystem&);
	void setupForces();

	ParticleSystem mParticleSystem;

	vec2 attrPosition;
	float attrFactor, repulsionFactor, repulsionRadius;
};

void CinderProjectApp::setup()
{
	setWindowSize(800, 600);

	setupParticleSystem(mParticleSystem);
	setupForces();
}

void CinderProjectApp::setupForces()
{
	attrPosition = getWindowCenter();
	attrFactor = 0.05f;
	repulsionRadius = 100.f;
	repulsionFactor = -5.f;
}

void CinderProjectApp::setupParticleSystem(ParticleSystem &ps)
{
	int numParticles = 100;
	for (int i = 0; i < numParticles; i++)
	{
		float x = ci::randFloat(0.f, getWindowWidth());
		float y = ci::randFloat(0.f, getWindowHeight());
		float radius = ci::randFloat(5.f, 15.f);
		float mass = radius * radius;
		float drag = 0.95f;
		Particle *particle = new Particle(vec2(x, y), radius, mass, drag);
		ps.addParticle(particle);
	}
}

void CinderProjectApp::mouseDown( MouseEvent event )
{
	for (std::vector<Particle*>::iterator it = mParticleSystem.particles.begin(); it != mParticleSystem.particles.end(); ++it)
	{
		vec2 repulsionForce = (*it)->position - vec2(event.getPos());
		repulsionForce *= math<float>::max(0.f, repulsionRadius - repulsionForce.size());
		(*it)->forces += repulsionForce;
	}
}

void CinderProjectApp::mouseMove(MouseEvent event)
{
	attrPosition.x = event.getPos().x;
	attrPosition.y = event.getPos().y;
}

void CinderProjectApp::update()
{
	for (std::vector<Particle*>::iterator it = mParticleSystem.particles.begin(); it != mParticleSystem.particles.end(); ++it)
	{
		vec2 attrForce = attrPosition - (*it)->position;
		attrForce *= attrFactor;
		(*it)->forces += attrForce;
	}
	mParticleSystem.update();
}

void CinderProjectApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::setMatricesWindow(getWindowWidth(), getWindowHeight());
	mParticleSystem.draw();
}

CINDER_APP( CinderProjectApp, RendererGl )
