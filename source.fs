#version 330 core
flat in int zbuffer;
flat in int gouraud;
out vec4 FragColor;
uniform vec3 viewPos;
void main()
{
    if ( zbuffer == 1 ) 
    {
        FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
    } 
    else 
    {
    	FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
}