//
// Created by ChiaYuan Chang on 10/29/20.
//

#ifndef ECS175_TEMPLATE_PROJECTS_SKELETON_P2_GUI_HPP_
#define ECS175_TEMPLATE_PROJECTS_SKELETON_P2_GUI_HPP_
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

struct Geometry {
  std::vector<float> g_vertex_buffer_triangles;
  std::vector<int> edge_index;
  std::vector<float> g_vertex_buffer_lines;
  GLuint vertex_buffer_id_lines, vertex_buffer_id_triangles;
  unsigned int num_vertex_;
  unsigned int num_edge_;
  glm::mat4 transformation_matrix_ = {1, 0, 0, 0,
                                      0, 1, 0, 0,
                                      0, 0, 1, 0,
                                      0, 0, 0, 1};
  float cx, cy, cz;
  void CalculateCenterMass() {
    cx = 0, cy = 0, cz = 0;
    int k = 0;
    for (auto &item : g_vertex_buffer_triangles) {
      if ((k % 3) == 0)
        cx += item;
      if ((k % 3) == 1)
        cy += item;
      if ((k % 3) == 2)
        cz += item;
      ++k;
    }
    cx /= (k / 3);
    cy /= (k / 3);
    cz /= (k / 3);
  }
};


// GUI function
extern glm::mat4 matrix;
extern std::vector<Geometry> geometries;
void ShowMenu(bool *p_open, GLFWwindow *window, GLuint i);
void ShowInputFile(GLFWwindow *window);
void Show3DTransformation(GLFWwindow *window);
void ShowOrthoProjection(GLFWwindow *window, GLuint matrix_id);
extern bool draw_bounding;

// Read file function
extern std::string file;
void ReadFile(const std::string &input);
void CreateVBO();
void OutputFile(const std::string &input);

// Transformation
void ApplyTranslation(int local_id, float tx, float ty, float tz);
void ApplyRotation(int id, float angle, float ax, float ay, float az, float bx, float by, float bz);
void ApplyScaling(int local_id, float sx, float sy, float sz);
void ShowVertexCoordinate(int local_id);

// Matrix method
glm::mat4 Inverse_Matrix(glm::mat4 M_2, float det);
float Det(glm::mat4 M_2);

#endif //ECS175_TEMPLATE_PROJECTS_SKELETON_P2_GUI_HPP_
