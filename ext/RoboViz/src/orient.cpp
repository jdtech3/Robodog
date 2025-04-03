#include <cmath>
#include <cstdio>
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <asio.hpp>

#include "GLider/GLider.hpp"
#include "sdl3_wrapper.hpp"
#include "utils.hpp"
#include "thedog.hpp"

// #define ORIENT_USE_SERIAL

void setupTCPClient(asio::ip::tcp::socket& sock, const std::string& interface_name, const asio::ip::tcp::endpoint& end_point){

    struct Option_BindToDevice{
        std::string interface_name;
        int level(const asio::ip::tcp&) const{return SOL_SOCKET;}
        int name(const asio::ip::tcp&) const{return SO_BINDTODEVICE;}
        const char* data(const asio::ip::tcp&) const{return interface_name.c_str();}
        std::size_t size(const asio::ip::tcp&) const{return interface_name.size();}
    };

    if(!sock.is_open()) sock.open(asio::ip::tcp::v4());
    if(!sock.is_open()) throw std::runtime_error("socket cannot be opened");
    sock.set_option(Option_BindToDevice(interface_name));
    std::printf("conntecing... ");
    sock.connect(end_point);
    std::printf("connected\n");
}

void setupSerial(asio::serial_port& serial, const char* port){

    serial.open(port);
    if(!serial.is_open())
        throw std::runtime_error("Serial cannot be opened");

    typedef asio::serial_port_base aspb;

    serial.set_option(aspb::baud_rate(115200));
    serial.set_option(aspb::flow_control(aspb::flow_control::none));
    serial.set_option(aspb::character_size(8));
    serial.set_option(aspb::parity(aspb::parity::even));
    serial.set_option(aspb::stop_bits(aspb::stop_bits::one));

    SDL_Delay(100);
    std::uint8_t c;
    SDL_Log("Trying to contact...");
    asio::read(serial, asio::buffer(&c, 1));
    while(c != 0xFF){
        std::putchar(c);
        asio::read(serial, asio::buffer(&c, 1));
    }
    asio::read(serial, asio::buffer(&c, 1));
    if(c != 0) throw std::runtime_error("Cannot Initialize MPU DMP");
    SDL_Log("MPU DMP Initialized");
}

void retrieveOrientation(asio::serial_port& asio_comm, glm::quat& orientation){
    char c = 1;
    asio::write(asio_comm, asio::buffer(&c, 1));
    struct {float w,x,y,z; } buf;
    asio::read(asio_comm, asio::buffer(&buf, sizeof(buf)));
    orientation.w = buf.w;
    orientation.x = buf.x;
    orientation.y = buf.y;
    orientation.z = buf.z;
}

void retrieveOrientation(asio::ip::tcp::socket& asio_comm, glm::quat& orientation){
    if(asio_comm.available() >= 4*sizeof(float)){
        struct {float w,x,y,z; } buf;
        do{
            asio::read(asio_comm, asio::buffer(&buf, sizeof(buf)));
        }while(asio_comm.available() >= 4*sizeof(float));
        orientation.w = buf.w;
        orientation.x = buf.x;
        orientation.y = buf.y;
        orientation.z = buf.z;
    }
}

int main(int argc, char* argv[]){

    #ifdef ORIENT_USE_SERIAL
    if(argc < 2){
        std::printf("Please Enter Serial Port\n");
        return 1;
    }
    #else
    if(argc < 4){
        std::printf("Please Enter interface, ip address, and port\n");
        return 1;
    }
    #endif

    try{

        asio::io_context io;

        #ifdef ORIENT_USE_SERIAL
        asio::serial_port asio_comm(io);
        #else
        asio::ip::tcp::socket asio_comm(io);
        #endif
        glm::quat dog_orientation;

        #ifdef ORIENT_USE_SERIAL
        setupSerial(asio_comm, argv[1]);
        #else
        setupTCPClient(asio_comm, argv[1], asio::ip::tcp::endpoint(asio::ip::make_address_v4(argv[2]), std::atoi(argv[3])));
        #endif

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

            body.draw(camera, dog_orientation);
            legs.draw(camera, dog_orientation);

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
                    case SDLK_ESCAPE:
                        keepRunning = false;
                        break;
                    }
                    break;
                }

                #undef shiftModifiersActived

            } // while(SDL_PollEvent(&event))

            retrieveOrientation(asio_comm, dog_orientation);

            glm::vec3 BR_foot( BODY_X/2.f, -BODY_Y/2.f, 0);
            glm::vec3 FR_foot( BODY_X/2.f,  BODY_Y/2.f, 0);
            glm::vec3 FL_foot(-BODY_X/2.f,  BODY_Y/2.f, 0);
            glm::vec3 BL_foot(-BODY_X/2.f, -BODY_Y/2.f, 0);
            glm::vec3 body_center(0.f, 0.f, (LEG_L1+LEG_L2)/glm::sqrt(2.f));
            
            glm::vec3 BR_body = dog_orientation*BR_foot;
            glm::vec3 FR_body = dog_orientation*FR_foot;
            glm::vec3 FL_body = dog_orientation*FL_foot;
            glm::vec3 BL_body = dog_orientation*BL_foot;
            BR_body += body_center;
            FR_body += body_center;
            FL_body += body_center;
            BL_body += body_center;

            BR_foot += glm::vec3( LEG_L0, (-LEG_L1+LEG_L2)/glm::sqrt(2.f), 0.f);
            FR_foot += glm::vec3( LEG_L0, (-LEG_L1+LEG_L2)/glm::sqrt(2.f), 0.f);
            FL_foot += glm::vec3(-LEG_L0, (+LEG_L1-LEG_L2)/glm::sqrt(2.f), 0.f);
            BL_foot += glm::vec3(-LEG_L0, (+LEG_L1-LEG_L2)/glm::sqrt(2.f), 0.f);

            legs.theta[0] = Legs::ik_BR(BR_foot - BR_body);
            legs.theta[1] = Legs::ik_FR(FR_foot - FR_body);
            legs.theta[2] = Legs::ik_FL(FL_foot - FL_body);
            legs.theta[3] = Legs::ik_BL(BL_foot - BL_body);

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