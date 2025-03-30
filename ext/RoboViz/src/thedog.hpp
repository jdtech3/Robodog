#ifndef THEDOG__H_
#define THEDOG__H_

#include <cmath>
#include <filesystem>
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/gtx/transform.hpp>

#include "GLider/GLider.hpp"
#include "utils.hpp"

constexpr float BODY_X =  2.f;
constexpr float BODY_Y =  3.f;
constexpr float BODY_Z =  0.5f;
constexpr float LEG_S  =  0.25f;
constexpr float LEG_L0 =  0.5f;
constexpr float LEG_L1 =  1.f;
constexpr float LEG_L2 =  1.f;

class RectPrism{
private:
    std::size_t nvals;
    gli::VertexArray                vertexArray;
    gli::Buffer<gli::VertexBuffer>  vertexBuffer;
    gli::Buffer<gli::IndexBuffer>   indexBuffer;
    gli::ShaderProgram              shaders;
public:
    glm::mat4 model;
    glm::vec4 color;
    RectPrism(const glm::vec3& center, const glm::vec3& lengths, const glm::vec4& color, const std::filesystem::path& vertex_shader, const std::filesystem::path& fragment_shader);
    virtual void draw(const Camera& camera);

};

class Body : public RectPrism{
public:
    Body(const std::filesystem::path& vertex_shader, const std::filesystem::path& fragment_shader);
};

class Legs{
public:
    RectPrism prism[3];
    glm::vec3 theta[4];

    Legs(const std::filesystem::path& vertex_shader, const std::filesystem::path& fragment_shader);
    void draw(const Camera& camera);

    static glm::vec3 ik_BR(glm::vec3 r);
    static glm::vec3 ik_FR(glm::vec3 r);
    static glm::vec3 ik_FL(glm::vec3 r);
    static glm::vec3 ik_BL(glm::vec3 r);

private:
    void draw_single_leg(const Camera& camera, const glm::vec3& theta, const glm::mat4& model);
};

#endif