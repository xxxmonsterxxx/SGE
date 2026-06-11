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

void PhysicsObject::checkCollision(PhysicsObject* obj)
{
    bool collision = false;

    if (!collidable || !obj->collidable)
        return;

    float dist = glm::length(obj->position - position);
    float max_sizes = this->boundary.getMaxDiagonale() * glm::length(this->getScale()) + obj->boundary.getMaxDiagonale() * glm::length(obj->getScale());
    if (dist > max_sizes)
        return;

    collision = boundary.isCollide(getModel(), obj->boundary, obj->getModel());

    if (collision) {
        appliedForce = glm::vec3(0);
        acceleration = glm::vec3(0);

        obj->appliedForce = glm::vec3(0);
        obj->acceleration = glm::vec3(0);
    }
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
    if (!running)
        return;

    float dt = getTimeDuration(lastUpdate, SgrTime::now());

    if (!physObjects)
        return;

    collisionsUpdate();

    for (auto phOb : *physObjects) {
        if (!phOb->physics_enabled)
            continue;

        phOb->gravity = gravity;
        phOb->update(dt);
    }

    lastUpdate = SgrTime::now();
}

void PhysicsEngine::collisionsUpdate()
{
    for (size_t i = 0; i < physObjects->size(); i++)
        for (size_t j = i + 1; j < physObjects->size(); j++)
            (*physObjects)[i]->checkCollision((*physObjects)[j]);
}