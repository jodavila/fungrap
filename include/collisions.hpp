//Collisions tests
#pragma once
// Forward declarations
class Ball;
class Cube;
class Plane;
class Cylinder;

class collisor {
public:
    collisor() = default;
    ~collisor() = default;
    bool SphereToPlane(Ball &ball, Plane &plane);
    bool SphereToCube(Ball &ball, Cube &cube);
    bool SphereToCylinder(Ball &ball, Cylinder &cylinder);
    bool SphereToSphere(Ball &ball1, Ball &ball2);
    bool SphereToCylinderBottom(Ball &ball, Cylinder &cylinder);

};