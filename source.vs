#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 FragPos;
out bool zbuffer;
out bool gouraud;
uniform mat4 model = mat4(1.0f);
uniform mat4 persp = mat4(1.0f);
uniform bool gour = false;
uniform bool zbuf = false;
void main()
{
   FragPos = vec3(persp * model * vec4(aPos, 1.0));
   gl_Position = vec4(FragPos, 1.0f);
   gouraud = gour;
   zbuffer = zbuf;
}

