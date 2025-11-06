// template based on material from learnopengl.com

#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
std::vector<float> getObj(const char* file);
std::string getSource(std::string file);
void keyInput(GLFWwindow* window, float& tx, float& ty, float& tz, float& rx, float& ry, float& rz, float& s);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

std::string vs = getSource("source.vs");
const char* vertexShaderSource = vs.c_str();

std::string fs = getSource("source.fs");
const char* fragmentShaderSource = fs.c_str();

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "meow", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // // glew: load all OpenGL function pointers
    glewInit();


    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    

    bool gouraud = true;
    bool perspective = false;
    float zbuffer = 1.0f;

    glm::mat4 persp;
    float aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT; 
    if (perspective)
        persp = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    else persp = glm::ortho(-11.067f, 11.067f, -8.3f, 8.3f, 0.1f, 100.0f);

    const char* file = "al.obj";
    std::vector<float> v = getObj(file);
    size_t s = v.size();
    
    
    float vertices[100000];
    for (int i = 0; i < s; i++)
    {
        vertices[i] = v.at(i);
    }

    unsigned int numVertices = sizeof(vertices) / 3;

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
 

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    glUseProgram(shaderProgram);

    GLint perspLoc = glGetUniformLocation(shaderProgram, "persp");
    glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(persp));
    std::cout << perspLoc;

    GLuint gourLoc = glGetUniformLocation(shaderProgram, "gour");
    glUniform1i(gourLoc, gouraud);

    GLuint zbufLoc = glGetUniformLocation(shaderProgram, "zbuf");
    glUniform1i(zbufLoc, zbuffer);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);

        
        //model = glm::scale(model, glm::vec3(0.1f));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -20.0f));

  
        
        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
        // glBindVertexArray(0); // unbind our VA no need to unbind it every time 


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

std::vector<float> getObj(const char* file)
{
    std::string line;
    std::string word;
    std::vector<float> verts;
    std::vector<int> faces;
    std::vector<float> triangles;

    std::ifstream obj(file);
    if (!obj.is_open())
    {
        std::cout << "file not open" << std::endl;
        return {};
    }
    
    while (std::getline(obj, line))
    {
        std::istringstream stream(line);
        stream >> word;

        if (word == "v")
        {
            while (stream >> word)
                verts.push_back(stof(word));
            //std::cout << word << std::endl;
        }
        else if (word == "f")
        {
            std::vector<int> faceVerts;
            while (stream >> word)
            {
                //std::cout << word.substr(0, word.find('/')) << std::endl;
                if (word.find('/') != std::string::npos)
                    faceVerts.push_back(std::stoi(word.substr(0, word.find('/'))));
                else
                    faceVerts.push_back(std::stoi(word));
            }
            for (int i = 1; i < faceVerts.size() - 1; i++)
            {
                faces.push_back(faceVerts.at(0) - 1);
                faces.push_back(faceVerts.at(i) - 1);
                faces.push_back(faceVerts.at(i + 1) - 1);
            }
        }
        else continue;
    }
    for (int i = 0; i < faces.size(); i++)
    {
        //std::cout << verts.at(faces.at(i));
        triangles.push_back(verts.at(faces.at(i) * 3));
        triangles.push_back(verts.at(faces.at(i) * 3 + 1));
        triangles.push_back(verts.at(faces.at(i) * 3 + 2));
    }

    if (faces.size() == 0)
        return verts;

    return triangles;
}

std::string getSource(std::string file)
{
    std::ifstream vs(file);
    if (!vs.is_open()) return "";

    std::stringstream out;
    out << vs.rdbuf();
    return out.str();
}

void keyInput(GLFWwindow* window, float& tx, float& ty, float& tz, float& rx, float& ry, float& rz, float& s)
{
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        tx += 0.001f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        tx -= 0.001f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        tz -= 0.0001f;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        tz += 0.0001f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        ty += 0.001f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        ty -= 0.001f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
        s += 0.0001f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
        s = std::max(s - 0.0001f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        ry += 0.05f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        ry -= 0.05f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        rx -= 0.05f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        rx += 0.05f;
}