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
        (2.f*M_PI/KEYHITS_PER_ROTATION)*(camera.rotationState.x.actual)-M_PI/2.f,
        (2.f*M_PI/KEYHITS_PER_ROTATION)*(camera.rotationState.y.actual),
        (2.f*M_PI/KEYHITS_PER_ROTATION)*(camera.rotationState.z.actual)
    ));

    shaders.bind();
    vertexArray.bind();
    indexBuffer.bind();
    indexBuffer.draw<unsigned char>(gli::DrawTriangles, nvals, 0);
}

Body::Body(const std::filesystem::path& vertex_shader, const std::filesystem::path& fragment_shader):
    RectPrism(
        glm::vec3(0,0,0),
        glm::vec3(BODY_X,BODY_Y,BODY_Z),
        glm::vec4(1.f, 0.f, 0.f, 1.f),
        vertex_shader,
        fragment_shader)
{}

Legs::Legs(const std::filesystem::path& vertex_shader, const std::filesystem::path& fragment_shader):
    prism{
        RectPrism(
            glm::vec3(0,0,0),
            glm::vec3(LEG_L0,LEG_S,LEG_S),
            glm::vec4(0.f, 1.f, 0.f, 1.f),
            vertex_shader,
            fragment_shader),
        RectPrism(
            glm::vec3(0,0,0),
            glm::vec3(LEG_S,LEG_S,LEG_L1),
            glm::vec4(0.f, 0.f, 1.f, 1.f),
            vertex_shader,
            fragment_shader),
        RectPrism(
            glm::vec3(0,0,0),
            glm::vec3(LEG_S,LEG_S,LEG_L2),
            glm::vec4(1.f, 1.f, 0.f, 1.f),
            vertex_shader,
            fragment_shader)},
    theta{
        glm::vec3(0.f, glm::radians(90.f), glm::radians(180.f)),
        glm::vec3(0.f, glm::radians(90.f), glm::radians(180.f)),
        glm::vec3(0.f, glm::radians(90.f), glm::radians(180.f)),
        glm::vec3(0.f, glm::radians(90.f), glm::radians(180.f))}
{}

void Legs::draw_single_leg(const Camera& camera, const glm::vec3& theta, const glm::mat4& model) {

    glm::mat4 m(model);
    m = glm::translate( m, glm::vec3(BODY_X/2,-BODY_Y/2,-BODY_Z/2));
    m = glm::rotate(    m, theta[0], glm::vec3(0.f, -1.f, 0.f));
    m = glm::translate( m, glm::vec3(LEG_S/2,0.f,0.f));
    prism[0].model = m;
    prism[0].draw(camera);

    m = model;
    m = glm::translate( m, glm::vec3(BODY_X/2,-BODY_Y/2,-BODY_Z/2));
    m = glm::rotate(    m, theta[0], glm::vec3(0.f, -1.f, 0.f));
    m = glm::translate( m, glm::vec3(LEG_L0, 0.f, 0.f));
    m = glm::rotate(    m, theta[1]-glm::radians(90.f), glm::vec3(-1.f, 0.f, 0.f));
    m = glm::translate( m, glm::vec3(0.f,0.f,(-LEG_L1)/2));
    prism[1].model = m;
    prism[1].draw(camera);

    m = glm::mat4(model);
    m = glm::translate(m, glm::vec3(BODY_X/2,-BODY_Y/2,-BODY_Z/2));
    m = glm::rotate(m, theta[0], glm::vec3(0.f, -1.f, 0.f));
    m = glm::translate(m, glm::vec3(LEG_L0, 0.f, 0.f));
    m = glm::rotate(m, theta[1]-glm::radians(90.f), glm::vec3(-1.f, 0.f, 0.f));
    m = glm::translate(m, glm::vec3(0.f,0.f,(-LEG_L1)));
    m = glm::rotate(m, theta[2]+glm::radians(180.f), glm::vec3(-1.f, 0.f, 0.f));
    m = glm::translate(m, glm::vec3(0.f,0.f,(-LEG_L2)/2));
    prism[2].model = m;
    prism[2].draw(camera);
}

void Legs::draw(const Camera& camera){
    glm::vec3 start, theta_tmp;
    start = glm::vec3(BODY_X/2,-BODY_Y/2,-BODY_Z/2);
    draw_single_leg(camera, theta[0], glm::mat4(
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f));
    start = glm::vec3(BODY_X/2,-BODY_Y/2,-BODY_Z/2);
    draw_single_leg(camera, theta[1], glm::mat4(
        1.f,  0.f, 0.f, 0.f,
        0.f, -1.f, 0.f, 0.f,
        0.f,  0.f, 1.f, 0.f,
        0.f,  0.f, 0.f, 1.f));
    start = glm::vec3(BODY_X/2,-BODY_Y/2,-BODY_Z/2);
    draw_single_leg(camera, theta[2], glm::mat4(
        -1.f,  0.f, 0.f, 0.f,
         0.f, -1.f, 0.f, 0.f,
         0.f,  0.f, 1.f, 0.f,
         0.f,  0.f, 0.f, 1.f));
    start = glm::vec3(BODY_X/2,-BODY_Y/2,-BODY_Z/2);
    draw_single_leg(camera, theta[3], glm::mat4(
        -1.f, 0.f, 0.f, 0.f,
         0.f, 1.f, 0.f, 0.f,
         0.f, 0.f, 1.f, 0.f,
         0.f, 0.f, 0.f, 1.f));
}

glm::vec3 Legs::ik_BR(glm::vec3 r){
    const float L0 = LEG_L0;
    const float L1 = LEG_L1;
    const float L2 = LEG_L2;
    const float L1_2 = L1*L1;
    const float L2_2 = L2*L2;
    const float a1 = glm::acos(L0*glm::inversesqrt(r.x*r.x + r.z*r.z)) - glm::atan(-r.z, r.x);
    r.x -= L0*glm::cos(a1);
    r.z -= L0*glm::sin(a1);
    const float rlen = glm::length(r);
    const float rlen2 = rlen*rlen;
    const float a2 = glm::acos(glm::clamp( (L1_2 + rlen2 - L2_2)/(2*L1*rlen)  , -1.f, 1.f ));
    const float a3 = glm::acos(glm::clamp( (L1_2 + L2_2 - rlen2)/(2*L1*L2)    , -1.f, 1.f ));
    const float a4 = glm::acos(glm::clamp( r.y/rlen                           , -1.f, 1.f ));
    // return glm::vec3(
    //     glm::clamp(a1       , glm::radians(-45.f), glm::radians(45.f)),
    //     glm::clamp(a2+a4    , glm::radians(90.f-45.f), glm::radians(90.f+45.f)),
    //     glm::clamp(a3       , glm::radians(90.f-45.f), glm::radians(90.f+45.f))
    // );
    return glm::vec3(
        a1,
        a2+a4,
        a3
    );
}

glm::vec3 Legs::ik_FR(glm::vec3 r){
    r.y *= -1;
    return ik_BR(r);
}

glm::vec3 Legs::ik_FL(glm::vec3 r){
    r.x *= -1;
    r.y *= -1;
    return ik_BR(r);
}

glm::vec3 Legs::ik_BL(glm::vec3 r){
    r.x *= -1;
    return ik_BR(r);
}