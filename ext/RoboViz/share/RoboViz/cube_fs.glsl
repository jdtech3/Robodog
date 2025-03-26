#version 140

in vec3 fnormal;

uniform vec4 u_color;

void main(){
    vec3 nnormal = normalize(fnormal);
    vec3 lightDir = vec3(0.0f,0.0f,1.0f);

    float ambient = 0.5f;
    float diffuse = max(dot(nnormal, lightDir), 0.0f) + ambient;

    gl_FragColor = u_color*diffuse;
}