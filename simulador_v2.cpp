#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include "Movils.cpp"
#include "VEC3.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include "linmath.h"

  // ===============================================================
  //              INICIO DE SETTINGS BASE
  // ===============================================================

using namespace std;

static struct a
{
	float x, y, z;
	float r, g, b;
} verticesT[710],verticesL[710];//creo q eran 711

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {

  glViewport(0, 0, width, height);
}

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

  // ===============================================================
  //              FIN DE SETTINGS BASE
  // ===============================================================

  // ===============================================================
  //              INICIO DE SHADERS
  // ===============================================================


const char *vertexShaderSource =
    "#version 330 core\n"
    "uniform mat4 MVP;\n"
    "layout (location = 0) in vec3 vPos;\n"
    "layout (location = 1) in vec3 vCol;\n"
    "out vec3 color;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = MVP * vec4(vPos.x,vPos.y,vPos.z, 1.0);\n"
    "   color = vCol;\n"
    "}\0";

const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "//uniform vec4 ourColor;\n"
    "in vec3 color;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(color,1.0);//ourColor;\n"
    "}\n\0";
  // ===============================================================
  //              FIN DE SHADERS
  // ===============================================================



  // ===============================================================
  //              INICIO FISICA DE LOS CUERPOS
  // ===============================================================
VEC3 pos_1 = {0.0, 0.0, 0.0};
VEC3 vel_1 = {0.0, 0.0, 0.0};

VEC3 pos_2 = {384.403E6, 0.0, 0.0};
VEC3 vel_2 = {0.0, 946.6322, 0.0};

double deltaT = 1.0 * 3600; // segundos en una hora
double t_horas = 0.0f;

Movil mov_1("Tierra", 5.9736E24, pos_1, vel_1);
Movil mov_2("  Luna", 7.35E22, pos_2, vel_2);

void generaPosiciones() {
  t_horas += deltaT / 3600;

  mov_1.limpiaFza();
  mov_2.limpiaFza();

  mov_1.interactua(&mov_2);

  mov_1.mueve(deltaT);
  mov_2.mueve(deltaT);

  // mov_2.impFza();

  VEC3 dr;
  float radio;
  dr = mov_2.r - mov_1.r;

  radio = dr.norma();
}
  // ===============================================================
  //              FIN FISICA DE LOS CUERPOS
  // ===============================================================


    // ===============================================================
  //              INICIO MAIN
  // ===============================================================

int main() {
  // ===============================================================
  //              INICIO DECLARACIÓNES VARIAS
  // ===============================================================

  int success;
  char infoLog[512];
  GLint mvp_location;
  int num_vertices_tierra=710;


  // ===============================================================
  //              FIN DECLARACIÓNES VARIAS
  // ===============================================================

  // ===============================================================
  //              INICIO GARBANZO MAIN
  // ===============================================================


  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
                                        "Simulador", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
glfwSwapInterval(1);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
// ===============================================================
  //              FIN GARBANZO MAIN
  // ===============================================================

  // ===============================================================
  //              INICIO CALCULOS POSICIONES MOVILES
  // ===============================================================
int i=0;
  while (t_horas / 24.0 < 29.6) {
    generaPosiciones();
    verticesT[i]={mov_1.r.x/4e8,mov_1.r.y/4e8,mov_1.r.z/4e8,1.0f,0.0f,0.0f};
    verticesL[i]={mov_2.r.x/4e8,mov_2.r.y/4e8,mov_2.r.z/4e8,0.0f,1.0f,0.0f};
    i++;
  }
  // ===============================================================
  //              FIN CALCULOS POSICIONES MOVILES
  // ===============================================================

  // ===============================================================
  //              INICIO DECLARACIÓN CON VERTEX SHADER DE LA TIERRA
  // ===============================================================


    unsigned int vertexShaderTierra = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderTierra, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShaderTierra);

    glGetShaderiv(vertexShaderTierra, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderTierra, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX Ejes::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

  // ===============================================================
  //              FIN DECLARACIÓN CON VERTEX SHADER DE LA TIERRA
  // ===============================================================

  // ===============================================================
  //              INICIO DECLARACIÓN CON FRAGMENT SHADER DE LA TIERRA
  // ===============================================================
    unsigned int fragmentShaderTierra = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderTierra, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShaderTierra);

    glGetShaderiv(fragmentShaderTierra, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderTierra, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT Ejes::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

  // ===============================================================
  //              FIN DECLARACIÓN CON FRAGMENT SHADER DE LA TIERRA
  // ===============================================================

  // ===============================================================
  //              INICIO VINCULACIÓN CON LOS SHADERS DE LA TIERRA
  // ===============================================================

    unsigned int shaderProgramTierra = glCreateProgram();
    glAttachShader(shaderProgramTierra, vertexShaderTierra);
    glAttachShader(shaderProgramTierra, fragmentShaderTierra);
    glLinkProgram(shaderProgramTierra);
    // check for linking errors
    glGetProgramiv(shaderProgramTierra, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgramTierra, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER Ejes::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

  // ===============================================================
  //              FIN VINCULACIÓN CON LOS SHADERS DE LA TIERRA
  // ===============================================================

  // ===============================================================
  //              INICIO VINCULACIÓN UNIFORMS
  // ===============================================================
    mvp_location = glGetUniformLocation(shaderProgramTierra, "MVP");

  // ===============================================================
  //              FIN VINCULACIÓN UNIFORMS
  // ===============================================================
    glDeleteShader(vertexShaderTierra);
    glDeleteShader(fragmentShaderTierra);

  // ===============================================================
  //              INICIO ASIGNACIÓN VALORES TIERRA
  // ===============================================================
    unsigned int VBO_TIERRA, VAO_TIERRA;

    //if(TIPO == EJES)
    {
      glGenVertexArrays(1, &VAO_TIERRA);
      glGenBuffers(1, &VBO_TIERRA);

      // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
      glBindVertexArray(VAO_TIERRA);

      glBindBuffer(GL_ARRAY_BUFFER, VBO_TIERRA);
      glBufferData(GL_ARRAY_BUFFER, num_vertices_tierra*sizeof(a), verticesT, GL_STATIC_DRAW);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(a), (void*)0);
      glEnableVertexAttribArray(0);

      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(a), (void*)(3*sizeof(float)));
      glEnableVertexAttribArray(1);

    }
  // ===============================================================
  //              FIN ASIGNACIÓN VALORES TIERRA
  // ===============================================================


  // ===============================================================
  //              INICIO RENDER LOOP
  // ===============================================================
  while (!glfwWindowShouldClose(window)) {

  // ===============================================================
  //              INICIO COSAS DE MATRIZ DE ROTACIÓN
  // ===============================================================
    float ratio;
    int width, height;
    mat4x4 m, p, mvp;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    mat4x4_identity(m);
    mat4x4_rotate_X(m, m, (float)glfwGetTime());
    mat4x4_rotate_Y(m, m, (float)glfwGetTime());
    mat4x4_rotate_Z(m, m, (float)glfwGetTime());
    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_mul(mvp, p, m);
  // ===============================================================
  //              FIN COSAS DE MATRIZ DE ROTACIÓN
  // ===============================================================

  // ===============================================================
  //              INICIO TRAZO DE LOS VERTICES
  // ===============================================================

    glBindVertexArray(VAO_TIERRA);
    glUseProgram(shaderProgramTierra);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
    glDrawArrays(GL_LINES,0,num_vertices_tierra);
    glfwSwapBuffers(window);
    glfwPollEvents();
  // ===============================================================
  //              FIN TRAZO DE LOS VERTICES
  // ===============================================================
  }

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
  return 0;
}
