#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

struct ShaderProgramSource
{ 
    std::string VertexSource;
    std::string FragentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
   
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*) alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "[OpenGL] Failed to compile  " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragementShader) 
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragementShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{ 
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit())
        return -1;
   
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "Goblin Engine [OpenGL]", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current [Valid OpenGL Context] */
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error!" << std::endl;
    }
    
    /* Debugger for OpenGL*/
    std::cout << "[OpenGL] Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "[OpenGL] Active Program:  " << GL_ACTIVE_PROGRAM << std::endl;
    std::cout << "[OpenGL] FrameBuffer: " << GL_FRAMEBUFFER << std::endl;
    std::cout << "[OpenGL] Gl2D id:  " << GL_2D << std::endl;
    std::cout << "[OpenGL] Buffer Size: " << GL_BUFFER_SIZE << std::endl;
    std::cout << "[OpenGL] Rgb: " << GL_RGB << std::endl;
    std::cout << "[OpenGL] Vram: " << GL_ANGLE_texture_usage << std::endl;
    /* Debugger for Engine*/
    std::cout << "\n[Goblin] Thread ID: " << std::this_thread::get_id() << std::endl;

    
    /* Posistions of 2d triangle in a float array */
    /* Vertex is the Whole Blob of data , Normals, Textures, etc */
 float positions[6] = {
        -0.5f, -0.5f,
         0.5f,  0.5f,
         0.5f, -0.5f,

    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    std::cout << "[Goblin] Vertex Shader: \n";
    std::cout << source.VertexSource << std::endl; 
    std::cout << "[Goblin] Fragment Shader: \n";
    std::cout << source.FragentSource << std::endl;

   unsigned int shader = CreateShader(source.VertexSource, source.FragentSource);
   glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_0))
        {
            glfwTerminate();
            exit(EXIT_SUCCESS);
        } 

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        //std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}
