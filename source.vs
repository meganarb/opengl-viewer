#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 FragPos;
out float zbuffer;
out float gouraud;
uniform mat4 model;
uniform mat4 persp;
uniform float gour;
uniform float zbuf;
void main()
{
   vec4 pos = persp * model * vec4(aPos, 1.0);
   FragPos = vec3(pos);
   zbuffer = zbuf;
   gouraud = gour;
   gl_Position = pos;
}

