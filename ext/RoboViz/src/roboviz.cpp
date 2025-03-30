#include <cmath>
#include <cstdio>
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/gtx/transform.hpp>

#include "GLider/GLider.hpp"
#include "sdl3_wrapper.hpp"
#include "utils.hpp"
#include "thedog.hpp"

int main(int argc, char* argv[]){

    (void)argc;

    try{

        SDL sdl(3,1);
        
        const SDL_DisplayMode* dm = SDL_GetDesktopDisplayMode(1);
        if (dm == nullptr)
            throw std::runtime_error(SDL_GetError());
        int win_size = std::min(dm->h, dm->w) * 3.f/4.f;

        SDL::OpenGLWindow window{"Cube", win_size, win_size};
        gli::initialize((GLADloadfunc)SDL_GL_GetProcAddress);

        auto exepath = std::filesystem::path(argv[0]).remove_filename();
        Camera camera(glm::perspective(glm::radians(70.f), 1.f, 0.001f, 1000.f));

        Body body(
            exepath / "../share/RoboViz/cube_vs.glsl",
            exepath / "../share/RoboViz/cube_fs.glsl"
        );

        Legs legs(
            exepath / "../share/RoboViz/cube_vs.glsl",
            exepath / "../share/RoboViz/cube_fs.glsl"
        );

        glm::vec3 leg_rel_pos[] = {
            glm::vec3( LEG_L0, 0.f, -LEG_L1-LEG_L2),
            glm::vec3( LEG_L0, 0.f, -LEG_L1-LEG_L2),
            glm::vec3(-LEG_L0, 0.f, -LEG_L1-LEG_L2),
            glm::vec3(-LEG_L0, 0.f, -LEG_L1-LEG_L2)
        };

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
                        for(auto& l : leg_rel_pos) l.x += 0.01;
                        break;
                    case SDLK_A:
                        for(auto& l : leg_rel_pos) l.x -= 0.01;
                        break;
                    case SDLK_W:
                        for(auto& l : leg_rel_pos) l.y += 0.01;
                        break;
                    case SDLK_S:
                        for(auto& l : leg_rel_pos) l.y -= 0.01;
                        break;
                    case SDLK_E:
                        for(auto& l : leg_rel_pos) l.z += 0.01;
                        break;
                    case SDLK_D:
                        for(auto& l : leg_rel_pos) l.z -= 0.01;
                        break;
                    case SDLK_ESCAPE:
                        keepRunning = false;
                        break;
                    }
                    break;
                }

                #undef shiftModifiersActived

            } // while(SDL_PollEvent(&event))

            legs.theta[0] = Legs::ik_BR(leg_rel_pos[0]);
            legs.theta[1] = Legs::ik_FR(leg_rel_pos[1]);
            legs.theta[2] = Legs::ik_FL(leg_rel_pos[2]);
            legs.theta[3] = Legs::ik_BL(leg_rel_pos[3]);

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