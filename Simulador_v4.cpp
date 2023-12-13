#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Movils.cpp"
#include "VEC3.h"
#include "Resorte_04_Normales_TEX_ejes.cpp"

#include <stb_image.h>
#include <iostream>
#include <cmath>
#include "linmath.h"
#include <learnopengl/filesystem.h>

VEC3 pos_1 = {0.0, 0.0, 0.0};
VEC3 vel_1 = {0.0, 0.0, 0.0};

VEC3 pos_2 = {384.403E6, 0.0, 0.0}; //en miles de km
VEC3 vel_2 = {0.0, 946.6322, 0.0};

VEC3 pos_3 = {20.7E7, 0.0, 0.0}; //20.7E7
VEC3 vel_3 = {0.0, 1E3, 0.0};

double deltaT = 1 * 3600; // segundos en una hora
//double t_horas = 0.0f;

Movil mov_1("Tierra", 5.9736E24, pos_1, vel_1);
Movil mov_2("Luna", 7.35E22, pos_2, vel_2);
Movil mov_3("Satelite", 1500, pos_3, vel_3);


void generaPosiciones() {
  //t_horas += deltaT / 3600;

  mov_1.limpiaFza();
  mov_2.limpiaFza();
  mov_3.limpiaFza();

  mov_1.interactua(&mov_2);
  mov_1.interactua(&mov_3);
  mov_2.interactua(&mov_3);

  mov_1.mueve(deltaT);
  mov_2.mueve(deltaT);
  mov_3.mueve(deltaT);

}
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


const char *vertexShaderSource ="#version 330 core\n"
    "uniform mat4 MVP;\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aCol;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "out vec3 color;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = MVP * vec4(aPos.x,aPos.y,aPos.z, 1.0);\n"
    "   color = aCol;\n"
    "   TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 color;\n"
    "in vec2 TexCoord;\n"

    "uniform sampler2D texture1;\n"
    "void main()\n"
    "{\n"
    "     FragColor = texture(texture1, TexCoord);\n"
    "}\n\0";

int main()
{
    float x,y,z;
    GLint mvp_location, vpos_location, vcol_location;

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Simulador Tierra-Luna", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

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

    mvp_location = glGetUniformLocation(shaderProgram, "MVP");
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    int num_indices;


    int           num_vertices_tierra;
    int           num_indices_tierra;
    VERTICE_TEX_NOR      *vertices_tierra;
    unsigned int *indices_tierra;

    int           num_vertices_luna;
    int           num_indices_luna;
    VERTICE_TEX_NOR      *vertices_luna;
    unsigned int *indices_luna;


    int           num_vertices_sat;
    int           num_indices_sat;
    VERTICE_TEX_NOR      *vertices_sat;
    unsigned int *indices_sat;

    int LUNA = 1;
    int TIERRA = 2;
    int SATELITE=4;


    creaSolRevCaras(TIERRA,
                    &num_vertices_tierra, &vertices_tierra,
                    &num_indices_tierra,  &indices_tierra);

    creaSolRevCaras(LUNA,
                    &num_vertices_luna, &vertices_luna,
                    &num_indices_luna,  &indices_luna);

    creaSolRevCaras(SATELITE,
                    &num_vertices_sat, &vertices_sat,
                    &num_indices_sat,  &indices_sat);

    // ================================================================================
    //                               TIERRA
    // ================================================================================

    unsigned int VBO_tierra, VAO_tierra, EBO_tierra;

    {
      glGenVertexArrays(1, &VAO_tierra);
      glGenBuffers(1,      &VBO_tierra);
      glGenBuffers(1,      &EBO_tierra);

      num_indices = num_indices_tierra;


      glBindVertexArray(VAO_tierra);

      glBindBuffer(GL_ARRAY_BUFFER, VBO_tierra);
      glBufferData(GL_ARRAY_BUFFER, num_vertices_tierra*sizeof(VERTICE_TEX_NOR), vertices_tierra, GL_STATIC_DRAW);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VERTICE_TEX_NOR), (void*)0);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VERTICE_TEX_NOR), (void*)(3*sizeof(float)));
      glEnableVertexAttribArray(1);

      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VERTICE_TEX_NOR), (void*)(6 * sizeof(float)));
      glEnableVertexAttribArray(2);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_tierra);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices_tierra * sizeof(unsigned int), indices_tierra, GL_STATIC_DRAW);

    }
    // ================================================================================
    //                              FIN DE TIERRA
    // ================================================================================
   // ================================================================================
    //                               LUNA
    // ================================================================================

    unsigned int VBO_luna, VAO_luna, EBO_luna;

    //if(TIPO == PRISMA)
    {
      glGenVertexArrays(1, &VAO_luna);
      glGenBuffers(1, &VBO_luna);
      glGenBuffers(1, &EBO_luna);

      num_indices = num_indices_luna;

      glBindVertexArray(VAO_luna);

      glBindBuffer(GL_ARRAY_BUFFER, VBO_luna);
      glBufferData(GL_ARRAY_BUFFER, num_vertices_luna*sizeof(VERTICE_TEX_NOR), vertices_luna, GL_STATIC_DRAW);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VERTICE_TEX_NOR), (void*)0);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VERTICE_TEX_NOR), (void*)(3*sizeof(float)));
      glEnableVertexAttribArray(1);

      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VERTICE_TEX_NOR), (void*)(6 * sizeof(float)));
      glEnableVertexAttribArray(2);


      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_luna);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices_luna * sizeof(unsigned int), indices_luna, GL_STATIC_DRAW);

    }
    // ================================================================================
    //                               FIN DE LUNA
    // ================================================================================
   // ================================================================================
    //                               SATELITE
    // ================================================================================


unsigned int VBO_sat, VAO_sat, EBO_sat;

    //if(TIPO == PRISMA)
    {
      glGenVertexArrays(1, &VAO_sat);
      glGenBuffers(1, &VBO_sat);
      glGenBuffers(1, &EBO_sat);

      num_indices = num_indices_sat;

      glBindVertexArray(VAO_sat);

      glBindBuffer(GL_ARRAY_BUFFER, VBO_sat);
      glBufferData(GL_ARRAY_BUFFER, num_vertices_sat*sizeof(VERTICE_TEX_NOR), vertices_sat, GL_STATIC_DRAW);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VERTICE_TEX_NOR), (void*)0);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VERTICE_TEX_NOR), (void*)(3*sizeof(float)));
      glEnableVertexAttribArray(1);

      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VERTICE_TEX_NOR), (void*)(6 * sizeof(float)));
      glEnableVertexAttribArray(2);


      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_sat);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices_sat * sizeof(unsigned int), indices_sat, GL_STATIC_DRAW);

    }
    // ================================================================================
    //                               FIN DE LUNA
    // ================================================================================

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);


    unsigned int texture1, texture2,texture3;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load(FileSystem::getPath("resources/textures/tierra.jpg").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    // ----------------------------
    //            texture2
    // ----------------------------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    data = stbi_load(FileSystem::getPath("resources/textures/luna.jpg").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // ----------------------------
    //            texture3
    // ----------------------------
    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    data = stbi_load(FileSystem::getPath("resources/textures/wood.png").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);



    unsigned int usar_textura;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // be sure to activate the shader before any calls to glUniform
        glUseProgram(shaderProgram);

		float ratio;
		int width, height;
		mat4x4 m, p, mvp,pers,mk,mtr;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);
		//glClear(GL_COLOR_BUFFER_BIT);

		//mat4x4_identity(m);
		//mat4x4_rotate_X(m, m, (float)glfwGetTime());
		//mat4x4_rotate_Y(m, m, (float)glfwGetTime());

		//mat4x4_ortho( p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
		//mat4x4_mul(mvp, p, m);

		float tr_y[2] = { 0.3f, -0.3f}; //{ 0.3f, -0.3f};
		float tr_x[2] = { 0.3f, -0.3f}; //{ 0.3f, -0.3f};
		float tr_z[2] = {-6.0f, -5.0f}; //{-6.0f, -5.0f};

		mat4x4_perspective(pers,30.0*atan(1.0)/45.0,800.0/600.0,
                           0.0f,10.0f);

for(int t= 0; t < 3; t++)
    //2 -> SATELITE
    //0 -> TIERRA
    //1 ->LUNA
        {
        generaPosiciones();
          mat4x4_identity(m);
		  //mat4x4_rotate_X(m, m, (t == 1?0.5:1.0) * (float)glfwGetTime());
		//  mat4x4_rotate_Y(m, m, (t == 1?0.5:1.0) * (float)glfwGetTime());
          mat4x4_rotate_Z(m, m, (t == 1?0.5:1.0) * (float)glfwGetTime());

          mat4x4_dup(mk,m);
        if(t==0){
            x=mov_1.r.x/4e8;
            y=mov_1.r.y/4e8;
        }
        if(t==1){
            x=mov_2.r.x/4e8;
            y=mov_2.r.y/4e8;
        }
        if(t==2){
            x=mov_3.r.x/4e8;
            y=mov_3.r.y/4e8;
        }
        z=-6.0; //estamos asumiendo q todos estan en el mismo plano
		  mat4x4_translate(mtr,x,y,z);

          mat4x4_mul(mk,mtr,mk);
		  mat4x4_mul(mvp, pers, mk);

		  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);

          if(t == 0){
            num_indices=num_indices_tierra;
              usar_textura = texture1;
              glBindTexture(GL_TEXTURE_2D, usar_textura);
              glBindVertexArray(VAO_tierra);
          }
          if(t==1)
              {
              num_indices=num_indices_luna;
              usar_textura = texture2;
              glBindTexture(GL_TEXTURE_2D, usar_textura);
              glBindVertexArray(VAO_luna);

              }


          if(t==2)
              {
              num_indices=num_indices_sat;
              usar_textura = texture3;
              glBindTexture(GL_TEXTURE_2D, usar_textura);
              glBindVertexArray(VAO_sat);

              }
              glDrawElements(GL_TRIANGLES,num_indices,GL_UNSIGNED_INT,0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }



      glDeleteVertexArrays(1, &VAO_tierra);
      glDeleteBuffers(1,      &VBO_tierra);
      glDeleteBuffers(1,      &EBO_tierra);

      glDeleteVertexArrays(1, &VAO_luna);
      glDeleteBuffers(1,      &VBO_luna);
      glDeleteBuffers(1,      &EBO_luna);

    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
