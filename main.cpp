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
std::vector<float> getObj(const char* file, bool gouraud);
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

    

    int gouraud = 1;
    bool perspective = false;
    int zbuffer = 1;

    glm::mat4 projection;
    float aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT; 
    if (perspective)
        projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    else projection = glm::ortho(-11.067f, 11.067f, -8.3f, 8.3f, 0.1f, 100.0f);

    const char* file = "al.obj";
    std::vector<float> v = getObj(file, true);
    size_t s = v.size();
    
    
    float vertices[100000];
    for (int i = 0; i < s; i++)
    {
        vertices[i] = v.at(i);
    }

    unsigned int numVertices = (sizeof(vertices) / sizeof(float)) / 3;

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

    GLint projLoc = glGetUniformLocation(shaderProgram, "proj");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    GLuint gourLoc = glGetUniformLocation(shaderProgram, "gour");
    glUniform1i(gourLoc, gouraud);

    GLuint zbufLoc = glGetUniformLocation(shaderProgram, "zbuf");
    glUniform1i(zbufLoc, zbuffer);

    glEnable(GL_DEPTH_TEST);
   
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
        //glDepthFunc(GL_LESS);

        // draw our first triangle
        glUseProgram(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);

        
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -30.0f));
        model = glm::scale(model, glm::vec3(2.0f));
        model = glm::rotate(model,glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  
        
        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

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

std::vector<float> getObj(const char* file, bool gouraud)
{
    std::string line;
    std::string word;
    std::vector<float> verts;
    std::vector<int> faces;

    std::vector<float> triangles;
    std::vector<float> textures;
    std::vector<float> vertNorms;
    //std::vector<std::tuple<int, float, float, float>> vertNorms;

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
            {
                verts.push_back(stof(word));
                vertNorms.push_back(0.0f);
                vertNorms.push_back(0.0f);
                vertNorms.push_back(0.0f);
                vertNorms.push_back(0.0f);
            }
        }
        else if (word == "vt")
        {
            stream >> word;
            textures.push_back(stof(word));
            stream >> word;
            textures.push_back(stof(word));
        }
        else if (word == "f")
        {
            std::vector<int> faceVerts;
            while (stream >> word)
            {
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

    int i = 0;
    int a = 0;
    while (i < faces.size())
    {
        glm::vec3 vert1 = glm::vec3(verts.at(faces.at(i) * 3), verts.at(faces.at(i) * 3 + 1), verts.at(faces.at(i) * 3 + 2));
        glm::vec3 vert2 = glm::vec3(verts.at(faces.at(i + 1) * 3), verts.at(faces.at(i + 1) * 3 + 1), verts.at(faces.at(i + 1) * 3 + 2));
        glm::vec3 vert3 = glm::vec3(verts.at(faces.at(i + 2) * 3), verts.at(faces.at(i + 2) * 3 + 1), verts.at(faces.at(i + 2) * 3 + 2));

        glm::vec3 norm = glm::cross(vert2 - vert1, vert3 - vert1);
        /*
        std::cout << vert1.x << " " << vert1.y << " " << vert1.z << "; ";
        std::cout << vert2.x << " " << vert2.y << " " << vert2.z << "; ";
        std::cout << vert3.x << " " << vert3.y << " " << vert3.z << "; ";
        std::cout << norm.x << " " << norm.y << " " << norm.z << std::endl;
        */
        
        for (int j = i; j < i + 3; j++)
        {
            vertNorms.at(faces.at(j) * 4) += 1.0f;
            vertNorms.at(faces.at(j) * 4 + 1) += norm.x;
            vertNorms.at(faces.at(j) * 4 + 2) += norm.y;
            vertNorms.at(faces.at(j) * 4 + 3) += norm.z;
        }
        

        i += 3;
    }

    for (int j = 0; j < vertNorms.size(); j = j + 4)
    {
        float n = vertNorms.at(j);
        if (n == 0.0f) continue;
        vertNorms.at(j + 1) = vertNorms.at(j + 1) / n;
        vertNorms.at(j + 2) = vertNorms.at(j + 2) / n;
        vertNorms.at(j + 3) = vertNorms.at(j + 3) / n;
    }

    for (int i = 0; i < vertNorms.size(); i++)
        std::cout << vertNorms.at(i) << " ";


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