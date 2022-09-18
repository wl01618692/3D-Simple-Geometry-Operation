//===========================================================================//
//                                                                           //
// Copyright(c) ECS 175 (2020)                                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <array>
#include "util.hpp"

#include "shaders.h"
#include "gui.hpp"

// GLM included in gui.hpp
glm::mat4 matrix(1.086396, -0.993682, -0.687368, -0.685994, // x0, y0, z0, w0
                 0.000000, 2.070171, -0.515526, -0.514496, // x1, y1, z1, w1
                 -1.448528, -0.745262, -0.515526, -0.514496, // x2, y2, z2, w2
                 0.000000, 0.000000, 5.642426, 5.830953);

int pid = 0;
bool show_menu = false;
bool show_console = false;
bool draw_bounding = false;
std::vector<Geometry> geometries;
std::string file;

static bool CapturedByGUI() {
  ImGuiIO &io = ImGui::GetIO();
  return (io.WantCaptureMouse);
}

static void ErrorCallback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE); // close window
  } else if (key == GLFW_KEY_G && action == GLFW_PRESS) {
    show_menu = !show_menu;
  }
}

static void CursorPositionCallback(GLFWwindow *window, double xpos, double ypos) {
  if (!CapturedByGUI()) {
    int left_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    int right_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    /* cursor projection */
    // TODO (optional)

    // right click
    if (right_state == GLFW_PRESS || right_state == GLFW_REPEAT) {
      // TODO (optional)
    }

    // left click
    if (left_state == GLFW_PRESS || left_state == GLFW_REPEAT) {
      // TODO (optional)
    }

    // TODO (optional)
  }
}

void WindowSizeCallback(GLFWwindow *window, int width, int height) {
  // TODO (optional)
}

int main(const int argc, const char **argv) {
  // Initialise GLFW
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    getchar();
    return -1;
  }

  // Open a window and create its OpenGL context
  const char *glsl_version = "#version 150"; // GL 3.3 + GLSL 150
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  auto window = glfwCreateWindow(1024, 768, "ECS 175 (P2) Press (G) to display GUI", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr,
            "Failed to open GLFW window. If you have a GPU that is "
            "not 3.3 compatible, try a lower OpenGL version.\n");
    getchar();
    glfwTerminate();
    return -1;
  }

  // Callback
  glfwSetKeyCallback(window, KeyCallback);
  glfwSetWindowSizeCallback(window, WindowSizeCallback);
  glfwSetCursorPosCallback(window, CursorPositionCallback);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // Load GLAD symbols
  int err = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress) == 0;
  if (err) {
    throw std::runtime_error("Failed to initialize OpenGL loader!");
  }

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  // ImGui
  {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // or ImGui::StyleColorsClassic();

    // Initialize Dear ImGui
    ImGui_ImplGlfw_InitForOpenGL(
        window, true /* 'true' -> allow imgui to capture keyboard inputs */);
    ImGui_ImplOpenGL3_Init(glsl_version);
  }

  // Dark blue background (avoid using black)
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  // TODO Create and compile our GLSL program from the shaders
  GLuint program_id =
      LoadProgram_FromEmbededTexts((char *) vshader, (char *) fshader);

  // TODO Create a shared vertex array
  GLuint vertex_array_id;
  glGenVertexArrays(1, &vertex_array_id);
  glBindVertexArray(vertex_array_id);

  // TODO Get handles for uniforms
  GLuint matrix_id = glGetUniformLocation(program_id, "Matrix");

  // TODO Load input geometries, compute bounding boxes
  if (argc == 2) {
    file = argv[1];
    ReadFile(file);
  }else
    std::cout << "Please specify the input file through GUI." << std::endl;

  // TODO Create vertex buffer objects
  CreateVBO();

  // Calculate Center mass
  for (auto &geometry : geometries)
    geometry.CalculateCenterMass();

  // Check if the ESC key was pressed or the window was closed
  while (!glfwWindowShouldClose(window)) {
    // Clear the screen and start rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
      // TODO Draw geometries correctly
      // Use our shader
      glUseProgram(program_id);
      if (draw_bounding) {
        GLfloat g_vertex_buffer_id_triangle[] = {
            -1, -1, -1,
            1, -1, -1,
            1, 1, -1,
            -1, 1, -1,
            -1, -1, 1,
            1, -1, 1,
            1, 1, 1,
            -1, 1, 1,
        };
        GLfloat g_vertex_buffer_id_lines[] = {
            -1, -1, -1, 1, -1, -1, // 1 2
            1, 1, -1, -1, 1, -1, // 3 4
            -1, -1, -1, -1, 1, -1, // 1 4
            1, -1, -1, 1, 1, -1, // 2 3
            -1, -1, 1, 1, -1, 1, // 5 6
            1, 1, 1, -1, 1, 1, // 7 8
            -1, -1, 1, -1, 1, 1,// 5 8
            1, 1, 1, 1, -1, 1,// 6 7
            -1, 1, 1, -1, 1, -1,// 8 4
            1, 1, 1, 1, 1, -1,// 7 3
            -1, -1, -1, -1, -1, 1,
            1, -1, -1, 1, -1, 1,
        };
        GLuint vertex_buffer_id_triangle;
        glGenBuffers(1, &vertex_buffer_id_triangle);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_triangle);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_id_triangle),
                     g_vertex_buffer_id_triangle, GL_STATIC_DRAW);

        GLuint vertex_buffer_id_lines;
        glGenBuffers(1, &vertex_buffer_id_lines);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_lines);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_id_lines),
                     g_vertex_buffer_id_lines, GL_STATIC_DRAW);

        glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &matrix[0][0]);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_lines);

        glVertexAttribPointer(0, // attribute. No particular reason for 0, but must
            // match the layout in the shader.
                              3, // size
                              GL_FLOAT, // type
                              GL_FALSE, // normalized?
                              sizeof(float) * 3, // stride
                              (void*)0 // array buffer offset
        );
        glDrawArrays(GL_LINES, 0, 24);
        glDisableVertexAttribArray(0);
      }

      for (auto &geometry : geometries) {
        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        auto temp = matrix * geometry.transformation_matrix_;
        glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &temp[0][0]);
        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, geometry.vertex_buffer_id_lines);
        glVertexAttribPointer(0,
                              3, // size
                              GL_FLOAT, // type
                              GL_FALSE, // normalized?
                              sizeof(float) * 3, // stride
                              (void *) 0 // array buffer offset
        );
        glDrawArrays(GL_LINES, 0, 2 * geometry.num_edge_);
        glDisableVertexAttribArray(0);
      }
    }

    // Initialization
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
      // TODO Draw GUI
      if (show_menu)
        ShowMenu(&show_menu, window, matrix_id);
    }
    // Render GUI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }


  // TODO Cleanup VBO, VAO and shader
  for (auto &geometry : geometries) {
    glDeleteBuffers(1, &geometry.vertex_buffer_id_triangles);
    glDeleteBuffers(1, &geometry.vertex_buffer_id_lines);
  }
  glDeleteProgram(program_id);
  glDeleteVertexArrays(1, &vertex_array_id);

  // Cleanup ImGui
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  // Output File
//  OutputFile(file);

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}
