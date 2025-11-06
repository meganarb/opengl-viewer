#version 330 core
in vec3 FragPos;
in float zbuffer;
in float gouraud;
out vec4 FragColor;
uniform vec3 viewPos;
void main()
{
    if(zbuffer > 0.5f) {
        FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
    } else {
    	FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}