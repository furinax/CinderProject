#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Log.h"
#include "cinder/params/Params.h"

#include "boost/range/irange.hpp"

#include "ParticleSystem.h"
#include "Config.h"
#include "UpdateParams.h"

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

	void enableFileLogging();

	
private:
	void setupConfig();
	void setupParticleSystem(ParticleSystem&);
	void setupForces();
	void setupParams();

	ParticleSystem mParticleSystem;

	vec2 attrPosition;
	float attrFactor, repulsionFactor, repulsionRadius;
	params::InterfaceGl		mParams;
	Config mConfig;
	UpdateParams mUpdateParams;

	int mTotalParticles = 0;
};

void CinderProjectApp::setup()
{
	setWindowSize(800, 600);

	enableFileLogging();

	setupConfig();
	setupParticleSystem(mParticleSystem);
	setupForces();
	setupParams();
}

void CinderProjectApp::setupConfig()
{
	mConfig.load();
}

void CinderProjectApp::setupParams(){
	mParams = params::InterfaceGl("Parameters", ci::vec2(200, 150));
	mParams.addSeparator();
	mParams.addParam("Total particles", &mTotalParticles, "readonly=1");
	mParams.maximize();
}

void CinderProjectApp::enableFileLogging()
{
	log::makeLogger<log::LoggerFile>("/tmp/cinder/cinder.log");
	CI_LOG_I("Logger Initiated");
}

void CinderProjectApp::setupForces()
{
	attrPosition = getWindowCenter();
	attrFactor = 0.05f;
	repulsionRadius = 50.f;
	repulsionFactor = -.005f;
}

void CinderProjectApp::setupParticleSystem(ParticleSystem &ps)
{
	for (auto i : boost::irange(0, 50))
	{
		float x = ci::randFloat(0.f, getWindowWidth());
		float y = ci::randFloat(0.f, getWindowHeight());
		
		Particle *particle = new Particle(mConfig.config, vec2(x, y), vec2(0,0));
		ps.addParticle(particle);
	}
}

void CinderProjectApp::mouseDown( MouseEvent event )
{
	for (std::vector<Particle*>::iterator it = mParticleSystem.particles.begin(); it != mParticleSystem.particles.end(); ++it)
	{
		vec2 repulsionForce = (*it)->position - vec2(event.getPos());
		repulsionForce *= repulsionFactor * math<float>::max(0.f, repulsionRadius - repulsionForce.size());
		(*it)->forces += repulsionForce;
	}
}

void CinderProjectApp::mouseMove(MouseEvent event)
{
	attrPosition.x = event.getPos().x;
	attrPosition.y = event.getPos().y;
	mUpdateParams.attrPosition = attrPosition;
}

void CinderProjectApp::update()
{
	mParticleSystem.update(mUpdateParams);
	mTotalParticles = mParticleSystem.particles.size();
}

void CinderProjectApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::setMatricesWindow(getWindowWidth(), getWindowHeight());
	mParams.draw();
	mParticleSystem.draw();
}

CINDER_APP( CinderProjectApp, RendererGl )
