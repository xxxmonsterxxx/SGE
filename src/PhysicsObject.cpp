#include "PhysicsObject.h"


PhysicsObject::PhysicsObject() : mass(0), appliedForce(0), acceleration(0), velocity(0), angularRate(0), angularAccel(0), rotateAxis(0) { ; }

void PhysicsObject::update(float dt)
{
    glm::vec3 appliedForceAccel(0);
    if (mass > 0)
        appliedForceAccel = appliedForce / mass;
    velocity += (acceleration + appliedForceAccel + glm::vec3(0,-gravity,0)) * dt;

    glm::vec3 dPos = velocity * dt;
    move(dPos);
}


PhysicsEngine* PhysicsEngine::_singleton = nullptr;

PhysicsEngine::PhysicsEngine() : running(false) { ; }

PhysicsEngine& PhysicsEngine::get()
{
    if (!_singleton)
    _singleton = new PhysicsEngine();

    return *_singleton;
}

void PhysicsEngine::init(std::vector<PhysicsObject*> *phOb)
{
    physObjects = phOb;
}

void PhysicsEngine::start()
{
    startTime = SgrTime::now();
    lastUpdate = startTime;
    running = true;
}

void PhysicsEngine::update()
{
    float dt = getTimeDuration(lastUpdate, SgrTime::now());

    if (physObjects) {
        for (auto phOb : *physObjects) {
            phOb->gravity = gravity;
            phOb->update(dt);
        }
    }

    lastUpdate = SgrTime::now();
}