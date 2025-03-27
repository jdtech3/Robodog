#include "thedog.hpp"

RectPrism::RectPrism(const glm::vec3& center, const glm::vec3& lengths, const glm::vec4& color, const std::filesystem::path& vertex_shader, const std::filesystem::path& fragment_shader){

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

void RectPrism::draw(const Camera& camera){
    shaders.setUniform("u_color", color);
    shaders.setUniform("u_model", model, false);
    shaders.setUniform("u_vp", camera.get_vp(), false);
    shaders.setUniform("orientation", glm::vec3(
        (2.f*M_PI/KEYHITS_PER_ROTATION)*(camera.rotationState.x.actual),
        (2.f*M_PI/KEYHITS_PER_ROTATION)*(camera.rotationState.y.actual),
        (2.f*M_PI/KEYHITS_PER_ROTATION)*(camera.rotationState.z.actual)
    ));

    shaders.bind();
    vertexArray.bind();
    indexBuffer.bind();
    indexBuffer.draw<unsigned char>(gli::DrawTriangles, nvals, 0);
}

Legs::Legs(const std::filesystem::path& vertex_shader, const std::filesystem::path& fragment_shader):
    RectPrism(
        glm::vec3(0,0,0),
        glm::vec3(LEG_S,LEG_S,LEG_L),
        glm::vec4(0.f, 1.f, 0.f, 1.f),
        vertex_shader,
        fragment_shader),
    theta{
        glm::vec3(0.f, glm::radians(90.f), glm::radians(180.f)),
        glm::vec3(0.f, glm::radians(90.f), glm::radians(180.f)),
        glm::vec3(0.f, glm::radians(90.f), glm::radians(180.f)),
        glm::vec3(0.f, glm::radians(90.f), glm::radians(180.f))
    }{}

void Legs::draw_single_leg(const Camera& camera, const glm::vec3& start, const glm::vec3& theta) {
    model = glm::mat4(1.f);
    model = glm::translate(model, glm::vec3(start.x,start.y,start.z));
    model = glm::rotate(model, theta[0], glm::vec3(0.f, -1.f, 0.f));
    model = glm::rotate(model, theta[1]-glm::radians(90.f), glm::vec3(-1.f, 0.f, 0.f));
    model = glm::translate(model, glm::vec3(0.f,0.f,(LEG_S-LEG_L)/2));
    color = glm::vec4(0.f, 1.f, 0.f, 1.f);
    RectPrism::draw(camera);

    model = glm::mat4(1.f);
    model = glm::translate(model, glm::vec3(start.x,start.y,start.z));
    model = glm::rotate(model, theta[0], glm::vec3(0.f, -1.f, 0.f));
    model = glm::rotate(model, theta[1]-glm::radians(90.f), glm::vec3(-1.f, 0.f, 0.f));
    model = glm::translate(model, glm::vec3(0.f,0.f,(LEG_S-LEG_L)));
    model = glm::rotate(model, glm::radians(180.f)-theta[2], glm::vec3(1.f, 0.f, 0.f));
    model = glm::translate(model, glm::vec3(0.f,0.f,(LEG_S-LEG_L)/2));
    color = glm::vec4(0.f, 0.f, 1.f, 1.f);
    RectPrism::draw(camera);
}

void Legs::draw(const Camera& camera){
    glm::vec3 start, theta_tmp;
    start = glm::vec3(BODY_X/2,-BODY_Y/2,-BODY_Z/2);
    theta_tmp = theta[0];
    draw_single_leg(camera, start, theta_tmp);
    start = glm::vec3(BODY_X/2,BODY_Y/2,-BODY_Z/2);
    theta_tmp = theta[1];
    theta_tmp[1] = glm::radians(180.f)-theta_tmp[1];
    theta_tmp[2] *= -1.f;
    draw_single_leg(camera, start, theta_tmp);
    start = glm::vec3(-BODY_X/2,BODY_Y/2,-BODY_Z/2);
    theta_tmp = theta[2];
    theta_tmp[0] *= -1.f;
    theta_tmp[1] = glm::radians(180.f)-theta_tmp[1];
    theta_tmp[2] *= -1.f;
    draw_single_leg(camera, start, theta_tmp);
    start = glm::vec3(-BODY_X/2,-BODY_Y/2,-BODY_Z/2);
    theta_tmp = theta[3];
    theta_tmp[0] *= -1.f;
    draw_single_leg(camera, start, theta_tmp);
}

glm::vec3 Legs::ik(const glm::vec3& r){
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
        a2+a4,
        a3
    );
}