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


using namespace std;

static struct
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

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource =
    "#version 330 core\n"
    "uniform mat4 MVP;\n"
    "layout (location = 0) in vec3 vPos;\n"
    "layout (location = 1) in vec3 vCol;\n"
    "out vec3 color;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = MVP * vec4(vPos.x,vPos.y,vPos.z, 1.0);\n"
    "   color = aCol;\n"
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

// FISICA DE LOS CUERPOS
VEC3 pos_1 = {0.0, 0.0, 0.0};
VEC3 vel_1 = {0.0, 0.0, 0.0};

VEC3 pos_2 = {384.403E6, 0.0, 0.0};
VEC3 vel_2 = {0.0, 946.6322, 0.0};

double deltaT = 1.0 * 3600; // segundos en una hora
double t_horas = 0.0f;

Movil mov_1("Tierra", 5.9736E24, pos_1, vel_1);
Movil mov_2("  Luna", 7.35E22, pos_2, vel_2);

void generaPosiciones() {
  // Eventualmente habra que sacar este while del metodo y usar el metodo nomas
  // pa q actualize los valores. quiero que lo haga en cada frame.

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

int main() {

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

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  // ===============================================================
  //              Uso del buffer de profundidd
  // ===============================================================

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
    // ===============================================================
  //              FIN CONFIGURACIÓN BASICA
  // ===============================================================

	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location, vpos_location, vcol_location;


int i=0;
  while (t_horas / 24.0 < 29.6) {
    generaPosiciones();
    verticesT[i]={mov_1.r.x/4e8,mov_1.r.y/4e8,mov_1.r.z/4e8,1.0f,0.0f,0.0f};
    verticesL[i]={mov_2.r.x/4e8,mov_2.r.y/4e8,mov_2.r.z/4e8,0.0f,1.0f,0.0f};
    i++;
    // metelo a un arreglo pa luego darselo al gpu
  }
/*
  for(int i=0;i<710;i++){
    cout<<verticesL[i].x<<" "<<verticesL[i].y<<" "<<verticesL[i].z<<endl;
  }
*/

  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verticesT), verticesT, GL_STATIC_DRAW);

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertex_shader);

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragment_shader);

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  mvp_location = glGetUniformLocation(program, "MVP");
  vpos_location = glGetAttribLocation(program, "vPos");
  vcol_location = glGetAttribLocation(program, "vCol");

  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, // decia 2
                        sizeof(verticesT[0]), (void *)0);
  glEnableVertexAttribArray(vcol_location);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(verticesT[0]), (void *)(sizeof(float) * 3));

  while (!glfwWindowShouldClose(window)) {
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

    glUseProgram(program);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
    glDrawArrays(GL_POINTS,0,709);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
  return 0;
}
