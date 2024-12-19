#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <string.h>

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

// Vertex Buffer Object, Vertex Array Object, Shader
GLuint VBO, VAO, shader;

// Vertex Shader
static const char *vShader = "\n\
#version 330\n\
\n\
layout (location = 0) in vec3 pos;\n\
void main() { \n\
gl_Position = vec4(0.4 * pos.x, 0.4 * pos.y, pos.z, 1.0); \n\
}";

// Fragment Shader
static const char *fShader = "\n\
#version 330\n\
\n\
out vec4 colour;\n\
void main() { \n\
colour = vec4(1.0, 0.0, 0.0, 0.0); \n\
}";

void AddShader(GLuint theProgram, const char *shaderCode, GLenum shaderType) {
  // Create an empty shader for our shader type
  GLuint theShader = glCreateShader(shaderType);

  // Create a pointer to the first character of the shader code
  const GLchar *theCode[1];
  theCode[0] = shaderCode;

  GLint codeLength[1];
  codeLength[0] = strlen(shaderCode);

  glShaderSource(theShader, 1, theCode, codeLength);
  glCompileShader(theShader);

  GLint result = 0;
  GLchar eLog[1024] = {0};

  glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
  if (!result) {
    glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
    printf("Error compiling the '%d' shader: '%s'\n", shaderType, eLog);
    return;
  }

  glAttachShader(theProgram, theShader);
};

void CompilerShaders() {
  shader = glCreateProgram();

  if (!shader) {
    printf("Failed to create shader program.");
    // Bad error handling here but whatever for now.
    return;
  }
  AddShader(shader, vShader, GL_VERTEX_SHADER);
  AddShader(shader, fShader, GL_FRAGMENT_SHADER);
  GLint result = 0;
  GLchar eLog[1024] = {0};

  // Link the shader which creates the executables on the graphics card
  glLinkProgram(shader);
  glGetProgramiv(shader, GL_LINK_STATUS, &result);
  if (!result) {
    glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
    printf("Error linking program: '%s'\n", eLog);
    return;
  }

  // Validate the shader
  glValidateProgram(shader);
  glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
  if (!result) {
    glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
    printf("Error validating program: '%s'\n", eLog);
    return;
  }
}

void CreateTriangle() {
  // clang-format off
  GLfloat vertices[] = {
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     0.0f,  1.0f, 0.0f};
  // clang-format on

  // Generate the VAO and get the ID back
  glGenVertexArrays(1, &VAO);

  // Bind the VAO so that further actions taken are taken on bound VAO ID
  glBindVertexArray(VAO);

  // Generate the VBO
  glGenBuffers(1, &VBO);

  // Bind the VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // Setting to 0 to basically unbind
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int main() {
  // Initialize GLFW
  if (!glfwInit()) {
    printf("Failed to initialze GLFW.");
    glfwTerminate();
    // Standard 1 for exiting on a failure
    return 1;
  }

  // Setup GLFW window properties
  // OpenGL version (3.3)
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // This core profile isn't backward compatible so we will
  // throw errors whenever we use deprecated features
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // Enable forward compatibility
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow *mainWindow =
      glfwCreateWindow(WIDTH, HEIGHT, "Window", NULL, NULL);
  if (!mainWindow) {
    printf("GLFW window creation failed...");
    // Standard 1 for exiting on a failure
    glfwTerminate();
    return 1;
  }

  // Buffer size information
  int bufferWidth, bufferHeight;
  glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

  // Set the context for GLEW to use
  glfwMakeContextCurrent(mainWindow);

  // Allow modern extensions features
  glewExperimental = GL_TRUE;

  if (glewInit() != GLEW_OK) {
    printf("Failed to initialize GLEW");
    glfwDestroyWindow(mainWindow);
    glfwTerminate();
    return 1;
  }

  // Setup viewport size
  glViewport(0, 0, bufferWidth, bufferHeight);

  // Shaders
  CreateTriangle();
  CompilerShaders();

  // main loop
  while (!glfwWindowShouldClose(mainWindow)) {

    // Event handling
    glfwPollEvents();

    // 0.0 is a double (64-bit floating point)
    // 0.0f is a float (32-bit floating point)
    // We avoid automatic conversion from the compiler making things a bit
    // faster
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the shader (in this case the Triangle)
    glUseProgram(shader);

    glBindVertexArray(VAO);

    // mode, the very frist point in the triangle and number of points we want
    // to draw
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);

    // Unassign the shader
    glUseProgram(0);

    glfwSwapBuffers(mainWindow);
  }

  // Exit
  return 0;
}
