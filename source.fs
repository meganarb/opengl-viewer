#version 330 core
in vec3 FragPos;
in bool gouraud;
in bool zbuffer;
out vec4 FragColor;
uniform vec3 viewPos;
void main()
{
    FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}