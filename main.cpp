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
void getObj(const char* file, bool gouraud, std::vector<float>& vertices, std::vector<float>& normals);
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

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    bool gouraud = true;
    bool perspective = true;
    bool zbuffer = false;

    glm::mat4 persp;
    float aspectRatio = 1.0f; // CHANGE THIS
    if (perspective)
        persp = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    else persp = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f); 

    std::vector<float> v;
    std::vector<float> n;
    const char* file = "al.obj";
    getObj(file, gouraud, v, n);
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

    float tx, ty, tz, rx, ry, rz, scale;
    float tx2, ty2, tz2, rx2, ry2, rz2, scale2;
    tx = ty = tz = rx = ry = rz = 0.0f;
    tx2 = ty2 = tz2 = rx2 = ry2 = rz2 = 0.0f;
    scale = scale2 = 1.0f;
    bool move = false;

    float rot = 0.0f;

    GLuint perspLoc = glGetUniformLocation(shaderProgram, "persp");
    glUniform1i(modelLoc, 1, GL_FALSE, glm::value_ptr(perspective));

    GLuint modelLoc = glGetUniformLocation(shaderProgram, "gour");
    glUniform1i(modelLoc, 1, GL_FALSE, glm::value_ptr(gouraud));

    GLuint modelLoc = glGetUniformLocation(shaderProgram, "zbuf");
    glUniform1i(modelLoc, 1, GL_FALSE, glm::value_ptr(zbuffer));

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
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

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            move = true;
        else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            move = false;

        glm::vec3 pos = glm::vec3(0.5f, 0.5f, 0.0f);
        model = glm::translate(model, pos);
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

        if (move)
        {
            keyInput(window, tx, ty, tz, rx, ry, rz, scale);
        }
        pos = pos + glm::vec3(tx, ty, tz);
        model = glm::translate(model, glm::vec3(tx, ty, tz));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        model = glm::rotate(model, glm::radians(rx), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rz), glm::vec3(0.0f, 0.0f, 1.0f));

        if (!move)
        {
            rot += 0.1f;
            glm::vec3 axis = pos - glm::vec3(-0.5f, -0.5f, 0.0f);
            model = glm::rotate(model, glm::radians(rot), axis);
        }
        
        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, numVertices);

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

void getObj(const char* file, bool gouraud, std::vector<float>& vertices, std::vector<float>& normals)
{
    std::string line;
    std::string word;
    std::vector<float> verts;
    std::vector<float> textures;
    std::vector<std::vector>> norms;
    std::vector<int> faces;

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
                norms.push_back();
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
            std::vector<int> faceTexts;
            while (stream >> word)
            {
                if (word.find('/') != std::string::npos)
                    faceVerts.push_back(std::stoi(word.substr(0, word.find('/'))));
                    word = word.substr(word.find('/') + 1, word.size() - word.find('/') - 1);
                    faceTexts.push_back(std::stoi(word.substr(0, word.find('/'))));
                else
                    faceVerts.push_back(std::stoi(word));
            }
            for (int i = 1; i < faceVerts.size() - 1; i++)
            {
                faces.push_back(faceVerts.at(0) - 1);
                faces.push_back(faceVerts.at(i) - 1);
                faces.push_back(faceVerts.at(i + 1) - 1);

                glm::vec3 a = glm::vec3(verts.at(faceVerts.at(0) - 1) * 3, verts.at(faceVerts.at(0) - 1) * 3 + 1, verts.at(faceVerts.at(0) - 1) * 3 + 2);
                glm::vec3 b = glm::vec3(verts.at(faceVerts.at(i) - 1) * 3, verts.at(faceVerts.at(i) - 1) * 3 + 1, verts.at(faceVerts.at(i) - 1) * 3 + 2);
                glm::vec3 c = glm::vec3(verts.at(faceVerts.at(i + 1) - 1) * 3, verts.at(faceVerts.at(i + 1) - 1) * 3 + 1, verts.at(faceVerts.at(i + 1) - 1) * 3 + 2);

                glm::vec3 ab = glm::vec3(b.x - a.x, b.y - a.y, b.z - a.z);
                glm::vec3 ac = glm::vec3(c.x - a.x, c.y - a.y, c.z - a.z);

                glm::vec3 norm = glm::cross(ab, ac);
            }
        }
        else continue;
    }
    for (int i = 0; i < faces.size(); i++)
    {
        vertices.push_back(verts.at(faces.at(i) * 3));
        vertices.push_back(verts.at(faces.at(i) * 3 + 1));
        vertices.push_back(verts.at(faces.at(i) * 3 + 2));
    }

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