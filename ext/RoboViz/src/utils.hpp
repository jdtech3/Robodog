#ifndef UTILS__H_
#define UTILS__H_

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/gtx/transform.hpp>

// Number of Key hits for a 360 Degree rotation about an axis (multiple of 8)
constexpr unsigned int KEYHITS_PER_ROTATION = 40;

static_assert(
    KEYHITS_PER_ROTATION % 8 == 0,
    "Key hits per rotation should be a multiple of 8"
);

struct MotionVar{

    /* If the difference between the desired and current rotation is
    below this value, then actual is set equal to desired */
    static constexpr float CLAMP_THRESHOLD = 0.01f;

    /* The time (in seconds) for a rotation to converge
    to its desired rotation after one key hit.
    In other words, the cube will be in motion for
    this many seconds after a key is pressed once. */
    static constexpr float CONVERGENCE_TIME = 1.f;
    static float rotFactor;

    struct Bounds{float min, max;};

    float desired;
    float actual;
    const Bounds& pBounds;

    MotionVar(float val, const Bounds& pBounds = {0,0});
    inline float distanceBetweenBounds(){return (pBounds.max)-(pBounds.min);}
    void incrementDesired();
    void decrementDesired();
    void update();
    static void update_factor(float fps);

};

struct MotionVector3D{

    MotionVar x, y, z;

    MotionVector3D(float x, float y, float z, const MotionVar::Bounds& pBounds = {0,0});
    MotionVector3D(glm::vec3 v, const MotionVar::Bounds& pBounds = {0,0});

    inline glm::vec3 getCurrent() const {return glm::vec3(x.actual, y.actual, z.actual);}
    inline glm::vec3 getDesired() const {return glm::vec3(x.desired, y.desired, z.desired);}
    void reset(float x=0, float y=0, float z=0);
    void computeMotion();

};

struct Camera{

    MotionVector3D observerPosition;
    MotionVector3D observerForwards;
    MotionVector3D observerUpwards;
    MotionVector3D rotationState;
    glm::mat4 proj;

    Camera(const glm::mat4& proj);

    glm::mat4 get_vp() const;

};

#endif