#version 330 core
layout (location = 0) in vec3 aPos;
flat out int zbuffer;
flat out int gouraud;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform int gour;
uniform int zbuf;
void main()
{
   vec4 pos = proj * view * model * vec4(aPos, 1.0);
   zbuffer = zbuf;
   gouraud = gour;
   gl_Position = pos;
}

