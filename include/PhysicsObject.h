#pragma once

#include "sge_utils.h"

class PhysicsObject {

friend class PhysicsEngine;

public:

    PhysicsObject();
    float mass;
    glm::vec3 appliedForce;
    glm::vec3 acceleration;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 rotateAxis;
    float angularAccel;
    float angularRate;

    std::vector <glm::vec3> boundary;

    void setVelocity(glm::vec3 newVel);
    void setAccel(glm::vec3 newAcc);
    virtual void move(glm::vec3 dPos) = 0;

    void setRotateAxis(glm::vec3 rAxis);
    void setAngularRate(float newRate);

    void update(float dt);

    bool collidable = true;

private:

    float gravity = 0;

    void checkCollision(PhysicsObject* obj);

};

class PhysicsEngine {

public:

    static PhysicsEngine& get();

    void init(std::vector<PhysicsObject*> *phOb);
    void start();
    void stop();
    void update();

    float gravity = 9.81;


private:
    PhysicsEngine();

    static PhysicsEngine* _singleton;

    SgrTime_t startTime;
    SgrTime_t lastUpdate;

    std::vector<PhysicsObject*> *physObjects = nullptr;

    bool running;

    void collisionsUpdate();
};