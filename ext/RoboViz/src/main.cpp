#include <cmath>
#include <vector>
#include <array>
#include <cstdio>
#include <iostream>
#include <filesystem>

#include "GLider/GLider.hpp"
#include "util.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

// Number of Key hits for a 360 Degree rotation about an axis (multiple of 8)
constexpr unsigned int KEYHITS_PER_ROTATION = 40;

static_assert(
    KEYHITS_PER_ROTATION % 8 == 0,
    "Key hits per rotation should be a multiple of 8"
);

// Macors ======================================================================

#ifndef M_PI
constexpr float M_PI = 3.14159265358979323846f;
#endif

constexpr float BODY_X =  2.f;
constexpr float BODY_Y =  3.f;
constexpr float BODY_Z =  0.5f;
constexpr float LEG_S =  0.25f;
constexpr float LEG_L =  1.f;

// Structures For Data Organization ============================================

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

    MotionVar(float val, const Bounds& pBounds = {0,0}):
        desired{val},
        actual{val},
        pBounds{pBounds}
    {}

    inline float distanceBetweenBounds(){return (pBounds.max)-(pBounds.min);}

    void incrementDesired(){
        desired += 1;
        if(pBounds.max != 0 && desired >= pBounds.max){
            desired -=  distanceBetweenBounds();
            actual -= distanceBetweenBounds();
        }
    }

    void decrementDesired(){
        desired -= 1;
        if(pBounds.min != 0 && desired <= pBounds.min){
            desired += distanceBetweenBounds();
            actual += distanceBetweenBounds();
        }
    }

    void update(){
        float delta = desired - actual;
        if(std::abs(delta) > CLAMP_THRESHOLD) actual += rotFactor*delta;
        else actual = desired;
    }

    static void update_factor(float fps){
        rotFactor = (std::log(1.f/MotionVar::CLAMP_THRESHOLD)/MotionVar::CONVERGENCE_TIME)/fps;
    }

};
float MotionVar::rotFactor = 0.f;

struct MotionVector3D{

    MotionVar x, y, z;

    MotionVector3D(float x, float y, float z, const MotionVar::Bounds& pBounds = {0,0}):
        x{x, pBounds},
        y{y, pBounds},
        z{z, pBounds}
    {}

    MotionVector3D(glm::vec3 v, const MotionVar::Bounds& pBounds = {0,0}):
        x{v.x, pBounds},
        y{v.y, pBounds},
        z{v.z, pBounds}
    {}

    glm::vec3 getCurrent() const {return glm::vec3(x.actual, y.actual, z.actual);}
    glm::vec3 getDesired() const {return glm::vec3(x.desired, y.desired, z.desired);}
    void reset(float x=0, float y=0, float z=0) {
        this->x.desired = x;
        this->y.desired = y;
        this->z.desired = z;
    }

    void computeMotion(){
        x.update();
        y.update();
        z.update();
    }

};

struct Camera{

    MotionVector3D observerPosition;
    MotionVector3D observerForwards;
    MotionVector3D observerUpwards;
    MotionVector3D rotationState;
    glm::mat4 proj;

    Camera(const glm::mat4& proj):
        observerPosition{ 0.f, 0.f, 5.f, {-50.f, 50.f}},
        observerForwards{ 0.f, 0.f, -1.f, {-50.f, 50.f}},
        observerUpwards{ 0.f, 1.f, 0.f, {-50.f, 50.f}},
        rotationState{ 0.f,  0.f, 0.f, {-KEYHITS_PER_ROTATION/2.f, KEYHITS_PER_ROTATION/2.f}},
        proj{proj}
    {}

    glm::mat4 get_vp() const{
        return proj*
        glm::lookAt(
            observerPosition.getCurrent(),
            observerPosition.getCurrent() + observerForwards.getCurrent(),
            observerUpwards.getCurrent()
        );
    }

};

struct RectPrism{
    std::size_t nvals;

    gli::VertexArray                vertexArray;
    gli::Buffer<gli::VertexBuffer>  vertexBuffer;
    gli::Buffer<gli::IndexBuffer>   indexBuffer;
    gli::ShaderProgram              shaders;
    glm::mat4 model;
    glm::vec4 color;

    RectPrism(const glm::vec3& center, const glm::vec3& lengths, const glm::vec4& color, const std::filesystem::path& vertex_shader, const std::filesystem::path& fragment_shader){

        model = glm::mat4(1);
        this->color = color;


        glm::vec2 a(center.x-lengths.x/2, center.x+lengths.x/2);
        glm::vec2 b(center.y-lengths.y/2, center.y+lengths.y/2);
        glm::vec2 c(center.z-lengths.z/2, center.z+lengths.z/2);

        glm::vec3 n = lengths;

        const float vertices[] = {
            a.x, b.x, c.x, -n.x, -n.y, -n.z,
            a.x, b.x, c.y, -n.x, -n.y,  n.z,
            a.x, b.y, c.y, -n.x,  n.y,  n.z,
            a.x, b.y, c.x, -n.x,  n.y, -n.z,
            a.y, b.x, c.x,  n.x, -n.y, -n.z,
            a.y, b.x, c.y,  n.x, -n.y,  n.z,
            a.y, b.y, c.y,  n.x,  n.y,  n.z,
            a.y, b.y, c.x,  n.x,  n.y, -n.z,
        };

        const unsigned char indices[] = {
            0, 1, 3, 1, 2, 3,
            6, 5, 7, 5, 4, 7,
            1, 0, 5, 0, 4, 5,
            2, 6, 3, 3, 6, 7,
            0, 3, 4, 3, 7, 4,
            2, 1, 6, 1, 5, 6
        };

        nvals = sizeof(indices)/sizeof(const unsigned char);

        vertexBuffer.feedData(vertices,  sizeof(vertices)/sizeof(float), gli::UseDynamicDraw);
        indexBuffer.feedData(indices, sizeof(indices)/sizeof(const unsigned char), gli::UseDynamicDraw);
        gli::Layout verBufLayout(2);
        verBufLayout.push<float>(gli::D3, false);
        verBufLayout.push<float>(gli::D3, false);
        vertexArray.readBufferData(vertexBuffer, verBufLayout);
        shaders.compileFile(gli::VertexShader, vertex_shader.string().c_str());
        shaders.compileFile(gli::FragmentShader, fragment_shader.string().c_str());
        shaders.link();
        shaders.validate();
        shaders.bindAttribLocation(0, "position");
        shaders.bindAttribLocation(1, "normal");

    }

    virtual void draw(const Camera& camera){
        shaders.setUniform("u_color", color);
        shaders.setUniform("u_model", model, false);
        shaders.setUniform("u_vp", camera.get_vp(), false);
        shaders.setUniform("orientation", glm::vec3(
            (2.f*M_PI/KEYHITS_PER_ROTATION)*(camera.rotationState.x.actual)+M_PI/4.f,
            (2.f*M_PI/KEYHITS_PER_ROTATION)*(camera.rotationState.y.actual)+M_PI/4.f,
            (2.f*M_PI/KEYHITS_PER_ROTATION)*(camera.rotationState.z.actual)+(0)
        ));

        shaders.bind();
        vertexArray.bind();
        indexBuffer.bind();
        indexBuffer.draw<unsigned char>(gli::DrawTriangles, nvals, 0);
    }

};

struct Legs : public RectPrism{
    glm::vec3 theta0, theta1, theta2, theta3;

    Legs(const std::filesystem::path& vertex_shader, const std::filesystem::path& fragment_shader):
        RectPrism(
            glm::vec3(0,0,0),
            glm::vec3(LEG_S,LEG_S,LEG_L),
            glm::vec4(0.f, 1.f, 0.f, 1.f),
            vertex_shader,
            fragment_shader),
        theta0{0.f, 0.f, 0.f},
        theta1{0.f, 0.f, 0.f},
        theta2{0.f, 0.f, 0.f},
        theta3{0.f, 0.f, 0.f}
    {}

    void draw_single_leg(const Camera& camera, const glm::vec3& start, const glm::vec3& theta) {
        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(start.x,start.y,start.z));
        model = glm::rotate(model, theta[0], glm::vec3(0.f, -1.f, 0.f));
        model = glm::rotate(model, theta[1], glm::vec3(-1.f, 0.f, 0.f));
        model = glm::translate(model, glm::vec3(0.f,0.f,(LEG_S-LEG_L)/2));
        color = glm::vec4(0.f, 1.f, 0.f, 1.f);
        RectPrism::draw(camera);

        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(start.x,start.y,start.z));
        model = glm::rotate(model, theta[0], glm::vec3(0.f, -1.f, 0.f));
        model = glm::rotate(model, theta[1], glm::vec3(-1.f, 0.f, 0.f));
        model = glm::translate(model, glm::vec3(0.f,0.f,(LEG_S-LEG_L)));
        model = glm::rotate(model, theta[2], glm::vec3(1.f, 0.f, 0.f));
        model = glm::translate(model, glm::vec3(0.f,0.f,(LEG_S-LEG_L)/2));
        color = glm::vec4(0.f, 0.f, 1.f, 1.f);
        RectPrism::draw(camera);
    }

    virtual void draw(const Camera& camera) override{
        glm::vec3 start, theta;
        start = glm::vec3(BODY_X/2,-BODY_Y/2,-BODY_Z/2);
        theta = theta0;
        draw_single_leg(camera, start, theta);
        start = glm::vec3(BODY_X/2,BODY_Y/2,-BODY_Z/2);
        theta = theta1;
        theta[1] *= -1.f;
        theta[2] *= -1.f;
        draw_single_leg(camera, start, theta);
        start = glm::vec3(-BODY_X/2,BODY_Y/2,-BODY_Z/2);
        theta = theta2;
        theta[0] *= -1.f;
        theta[1] *= -1.f;
        theta[2] *= -1.f;
        draw_single_leg(camera, start, theta);
        start = glm::vec3(-BODY_X/2,-BODY_Y/2,-BODY_Z/2);
        theta = theta3;
        theta[0] *= -1.f;
        draw_single_leg(camera, start, theta);
    }

    static glm::vec3 ik(const glm::vec3& r){
        float L1 = LEG_L - LEG_S;
        float L2 = LEG_L - LEG_S;
        float L1_2 = L1*L1;
        float L2_2 = L2*L2;
        float rlen = glm::length(r);
        float rlen2 = rlen*rlen;
        float a1 = glm::atan(r.x, -r.z);
        float a2 = glm::acos( (L1_2 + rlen2 - L2_2)/(2*L1*rlen) );
        float a3 = glm::acos( (L1_2 + L2_2 - rlen2)/(2*L1*L2) );
        float a4 = glm::acos( r.y/rlen );
        return glm::vec3(
            a1,
            a2+a4-glm::radians(90.f),
            glm::radians(180.f)-a3
        );
    }
};

int main(int argc, char* argv[]){

    (void)argc;

    try{

        SDL sdl(3,1);
        
        const SDL_DisplayMode* dm = SDL_GetDesktopDisplayMode(1);

        if (dm == nullptr)
            throw std::runtime_error(SDL_GetError());

        int q = std::min(dm->h, dm->w) * 3.f/4.f;

        SDL::OpenGLWindow window{"Cube", q, q};

        gli::initialize((GLADloadfunc)SDL_GL_GetProcAddress);

        auto exepath = std::filesystem::path(argv[0]).remove_filename();
        Camera camera(glm::perspective(70.f*3.14159f/180.f, (float)dm->w/(float)dm->h, 0.001f, 1000.f));

        RectPrism body(
            glm::vec3(0,0,0),
            glm::vec3(BODY_X,BODY_Y,BODY_Z),
            glm::vec4(1.f, 0.f, 0.f, 1.f),
            exepath / "../share/RoboViz/cube_vs.glsl",
            exepath / "../share/RoboViz/cube_fs.glsl"
        );

        Legs legs(
            exepath / "../share/RoboViz/cube_vs.glsl",
            exepath / "../share/RoboViz/cube_fs.glsl"
        );

        glm::vec3 leg0_pos(0.f, 0.f, -2*(LEG_L-LEG_S) + 0.01);

        PRINT_DEBUG("Cube Created\n");

        {
            int buffers, samples;
            SDL_GL_GetAttribute( SDL_GL_MULTISAMPLEBUFFERS, &buffers );
            SDL_GL_GetAttribute( SDL_GL_MULTISAMPLESAMPLES, &samples );
            PRINT_DEBUG("buf = %d, samples = %d\n", buffers, samples);
        }

        // Setup MVP =============================

        gli::enable(gli::Capability_NI::CullFace);
        gli::enable(gli::Capability_NI::DepthTest);
        gli::depthRange(0.01, 1000.0);

        bool keepRunning = true;
        SDL_Event event;

        gli::FrameRate fps;
        auto fps_print = std::chrono::steady_clock::now();

        /* Loop until the user closes the window */
        while (keepRunning)
        {

            // Compute motion ===========

            camera.rotationState.computeMotion();
            camera.observerPosition.computeMotion();

            /* Render here */
            gli::clear(gli::ColorBufferBit | gli::DepthBufferBit);

            body.draw(camera);
            legs.draw(camera);

            /* Swap front and back buffers */
            window.swap();

            /* Poll for and process events */
            while(SDL_PollEvent(&event)){
                
                #define shiftModifiersActived event.key.mod & SDL_KMOD_SHIFT

                switch(event.type){
                case SDL_EVENT_QUIT:
                    keepRunning = false;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    switch(event.key.key){
                    case SDLK_LEFT:
                        if(shiftModifiersActived)
                            camera.observerPosition.x.decrementDesired();
                        else
                            camera.rotationState.y.decrementDesired();
                        break;
                    case SDLK_RIGHT:
                        if(shiftModifiersActived)
                            camera.observerPosition.x.incrementDesired();
                        else
                            camera.rotationState.y.incrementDesired();
                        break;
                    case SDLK_UP:
                        if(shiftModifiersActived)
                            camera.observerPosition.y.incrementDesired();
                        else
                            camera.rotationState.x.decrementDesired();
                        break;
                    case SDLK_DOWN:
                        if(shiftModifiersActived)
                            camera.observerPosition.y.decrementDesired();
                        else
                            camera.rotationState.x.incrementDesired();
                        break;
                    case SDLK_PERIOD:
                        if(shiftModifiersActived)
                            camera.observerPosition.z.incrementDesired();
                        else
                            camera.rotationState.z.decrementDesired();
                        break;
                    case SDLK_COMMA:
                        if(shiftModifiersActived)
                            camera.observerPosition.z.decrementDesired();
                        else
                            camera.rotationState.z.incrementDesired();
                        break;
                    case SDLK_SPACE:
                        if(shiftModifiersActived){
                            camera.observerPosition.reset(0,0,5);
                        }else{
                            camera.rotationState.reset();
                        }
                        break;
                    case SDLK_Q:
                        leg0_pos[0] += 0.01;
                        break;
                    case SDLK_A:
                        leg0_pos[0] -= 0.01;
                        break;
                    case SDLK_W:
                        leg0_pos[1] += 0.01;
                        break;
                    case SDLK_S:
                        leg0_pos[1] -= 0.01;
                        break;
                    case SDLK_E:
                        leg0_pos[2] += 0.01;
                        break;
                    case SDLK_D:
                        leg0_pos[2] -= 0.01;
                        break;
                    case SDLK_ESCAPE:
                        keepRunning = false;
                        break;
                    }
                    break;
                }

                #undef shiftModifiersActived

            } // while(SDL_PollEvent(&event))

            legs.theta0 = Legs::ik(leg0_pos);

            /* FPS related stuff */
            fps.compute();
            MotionVar::update_factor(fps());

            auto ms_since_fps_report = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - fps_print).count();
            if(ms_since_fps_report >= 10){
                std::printf("fps: %5.0f\r", fps());
                fps_print = std::chrono::steady_clock::now();
            }
            
            
        } // while(keepRunning)

    }catch(const std::exception& ex){
        std::printf("%s occured!\n", typeid(ex).name());
        std::printf("%s", ex.what());
    }

    return 0;

}