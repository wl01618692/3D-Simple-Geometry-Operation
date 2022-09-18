//
// Created by ChiaYuan Chang on 10/29/20.
//
#include "util.hpp"
#include "gui.hpp"
template<typename T>
T SafeRead(std::ifstream &is) {
  T ret;
  is >> ret;
  auto rdstate = is.rdstate();
  if ((rdstate & std::ifstream::eofbit) != 0)
    throw std::runtime_error("End-of-File reached on input operation");
  if ((rdstate & std::ifstream::failbit) != 0)
    throw std::runtime_error("Logical error on i/o operation");
  if ((rdstate & std::ifstream::badbit) != 0)
    throw std::runtime_error("Read/writing error on i/o operation");
  return ret;
}

void ReadFile(const std::string &input) {
  std::ifstream file(input);
  if (!file.is_open())
    throw std::runtime_error("cannot open input file: " + input);

  int n = SafeRead<int>(file);
  std::cout << "there are " << n << " objects" << std::endl;
  geometries.resize(n);

  for (int i = 0; i < n; ++i) {
    int m = SafeRead<int>(file);
    geometries.at(i).num_vertex_ = m;
    for (auto j = 0; j < m; ++j) {
      auto x = SafeRead<float>(file), y = SafeRead<float>(file), z = SafeRead<float>(file);
      geometries.at(i).g_vertex_buffer_triangles.push_back(x);
      geometries.at(i).g_vertex_buffer_triangles.push_back(y);
      geometries.at(i).g_vertex_buffer_triangles.push_back(z);
    }

    int n = SafeRead<int>(file);
    geometries.at(i).num_edge_ = n;
    for (auto j = 0; j < n; ++j) {
      int x = SafeRead<int>(file), y = SafeRead<int>(file);
      for (int k = (x - 1) * 3; k <= x * 3 - 1; ++k)
        geometries.at(i).g_vertex_buffer_lines.push_back(geometries.at(i).g_vertex_buffer_triangles.at(k));

      for (int k = (y - 1) * 3; k <= y * 3 - 1; ++k)
        geometries.at(i).g_vertex_buffer_lines.push_back(geometries.at(i).g_vertex_buffer_triangles.at(k));
      geometries.at(i).edge_index.push_back(x);
      geometries.at(i).edge_index.push_back(y);
    }
    geometries.at(i).vertex_buffer_id_lines = i;
    geometries.at(i).vertex_buffer_id_triangles = i;
  }
  file.close();
}

void OutputFile(const std::string &input) {
  std::ofstream file(input, std::ofstream::trunc);
  // Number of polygon
  file << geometries.size() << std::endl;
  file << std::endl;

  for (int i = 0; i < geometries.size(); ++i) {
    file << geometries.at(i).num_vertex_;
    int k = 0;
    for (auto &item : geometries.at(i).g_vertex_buffer_triangles) {
      if ((k % 3) == 0)
        file << std::endl;
      file << item << " ";
      ++k;
    }
    file << std::endl;

    file << geometries.at(i).num_edge_;
    k = 0;
    for (auto &item : geometries.at(i).edge_index) {
      if ((k % 2) == 0)
        file << std::endl;
      file << item << " ";
      ++k;
    }
    file << std::endl << std::endl;
  }
}

void CreateVBO() {
  for (auto &geometry : geometries) {
    int n = geometry.g_vertex_buffer_triangles.size();
    GLfloat triangles[n];
    for (int i = 0; i < geometry.g_vertex_buffer_triangles.size(); ++i)
      triangles[i] = geometry.g_vertex_buffer_triangles.at(i);

    n = geometry.g_vertex_buffer_lines.size();
    GLfloat lines[n];
    for (int i = 0; i < geometry.g_vertex_buffer_lines.size(); ++i)
      lines[i] = geometry.g_vertex_buffer_lines.at(i);

    glGenBuffers(1, &geometry.vertex_buffer_id_triangles);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.vertex_buffer_id_triangles);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * geometry.g_vertex_buffer_triangles.size() * 3,
                 triangles, GL_STATIC_DRAW);

    glGenBuffers(1, &geometry.vertex_buffer_id_lines);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.vertex_buffer_id_lines);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * geometry.g_vertex_buffer_lines.size() * 3,
                 lines, GL_STATIC_DRAW);
  }
}

void ShowMenu(bool *p_open, GLFWwindow *window, GLuint i) {
  ImGui::Text("ECS175 Fall 2020 P2");
  ImGui::Text("Name: Chia-Yuan Chang");
  ImGui::Text("SID: 916420745");
  ImGui::Spacing();
  ShowInputFile(window);
  Show3DTransformation(window);
  ShowOrthoProjection(window, i);
}

void ShowInputFile(GLFWwindow *window) {
  if (ImGui::CollapsingHeader("Choose Input File")) {
    ImGui::Text("Choose the path of the input file");
    static char buf1[64] = "";
    ImGui::InputText("", buf1, 64);
    if (ImGui::Button("Load file")) {
      ReadFile(buf1);
      file = buf1;
      CreateVBO();
    }
  }
}

void Show3DTransformation(GLFWwindow *window) {
  if (ImGui::CollapsingHeader("3D Linear Transformation")) {
    static float tx, ty, tz = 0.0f;
    static float angle = 0.0f;
    static float sx, sy, sz = 0.0f;
    static float ax, ay, az = 0.0f;
    static float bx, by, bz = 0.0f;

    // Polygon ID specification
    ImGui::Text("Polygon ID");
    static int local_id = 0;
    ImGui::InputInt("polygon ID", &local_id);

    // Show vertex coordinates of the polygon
    if (ImGui::Button("Show Vertex Coordinates"))
      ShowVertexCoordinate(local_id);

    ImGui::NewLine();

    // Translation
    {
      ImGui::Text("Translation");
      ImGui::DragFloat("translate in x##1", &tx, 0.1);
      ImGui::DragFloat("translate in y##2", &ty, 0.1);
      ImGui::DragFloat("translate in z##3", &tz, 0.1);

      if (ImGui::Button("Apply Translation"))
        ApplyTranslation(local_id, tx, ty, tz);

    }
    ImGui::NewLine();

    // Rotation
    {
      ImGui::Text("Rotation about an arbitrary axis in 3D space");
      ImGui::DragFloat("Rotation angle##4", &angle, 0.1);
      ImGui::Text("Point 1 of the axis");
      ImGui::DragFloat("translate in x##5", &ax, 0.1);
      ImGui::DragFloat("translate in y##6", &ay, 0.1);
      ImGui::DragFloat("translate in z##7", &az, 0.1);
      ImGui::Text("Point 2 of the axis");
      ImGui::DragFloat("translate in x##8", &bx, 0.1);
      ImGui::DragFloat("translate in y##9", &by, 0.1);
      ImGui::DragFloat("translate in z##10", &bz, 0.1);
      if (ImGui::Button("Apply Rotation"))
        ApplyRotation(local_id, angle, ax, ay, az, bx, by, bz);
    }
    ImGui::NewLine();

    // Scaling
    {
      ImGui::Text("Scaling with respect to centroid");
      ImGui::DragFloat("scale in x##11", &sx, 0.1);
      ImGui::DragFloat("scale in y##12", &sy, 0.1);
      ImGui::DragFloat("scale in z##13", &sz, 0.1);
      if (ImGui::Button("Apply Scaling"))
        ApplyScaling(local_id, sx, sy, sz);

    }
  }
}

void ShowOrthoProjection(GLFWwindow *window, GLuint matrix_id) {
  if (ImGui::CollapsingHeader("Orthographic Projections")) {
    // Polygon ID specification
    ImGui::Text("Polygon ID");
    static int local_id = 0;
    ImGui::InputInt("polygon ID", &local_id);

    static int e = 0;
    ImGui::RadioButton("Default", &e, 0);
    ImGui::RadioButton("xy projection", &e, 3);
    ImGui::RadioButton("yz projection", &e, 1);
    ImGui::RadioButton("xz projection", &e, 2);

    // Show vertex coordinates of the polygon
    if (ImGui::Button("Show Vertex Coordinates"))
      ShowVertexCoordinate(local_id);

    int tx = 0, ty = 0, tz = 0;
    int k = 0;
    for (auto &element:geometries.at(local_id).g_vertex_buffer_triangles) {
      if ((k % 3) == 0) {
        if (element > tx)
          tx = element;
      } else if ((k % 3) == 1) {
        if (element > ty)
          ty = element;
      } else if ((k % 3) == 2) {
        if (element > tz)
          tz = element;
      }
    }

    if (e == 3) {
      draw_bounding = true;
      glm::mat4 xy(1, 0, 0, 0, // x0, y0, z0, w0
                   0, 1, 0, 0, // x1, y1, z1, w1
                   0, 0, 0, 0, // x2, y2, z2, w2
                   0, 0, -1 + tz, 1); // x3, y3, z3, w3

      auto temp = matrix * geometries.at(local_id).transformation_matrix_ * xy;
      glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &temp[0][0]);
      // 1st attribute buffer : vertices
      glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, geometries.at(local_id).vertex_buffer_id_lines);
      glVertexAttribPointer(0,
                            3, // size
                            GL_FLOAT, // type
                            GL_FALSE, // normalized?
                            sizeof(float) * 3, // stride
                            (void *) 0 // array buffer offset
      );
      glDrawArrays(GL_LINES, 0, 2 * geometries.at(local_id).num_edge_);
      glDisableVertexAttribArray(0);

    } else if (e == 2) {
      draw_bounding = true;
      auto copy = geometries.at(local_id).transformation_matrix_;
      {
        glm::mat4 rot(0, 1, 0, 0, // x0, y0, z0, w0
                      0, 0, 1, 0, // x1, y1, z1, w1
                      1, 0, 0, 0, // x2, y2, z2, w2
                      0, 0, 0, 1); // x3, y3, z3, w3

        glm::mat4 xz(1, 0, 0, 0, // x0, y0, z0, w0
                     0, 0, 0, 0, // x1, y1, z1, w1
                     0, 0, 1, 0, // x2, y2, z2, w2
                     0, -1 + ty, 0, 1); // x3, y3, z3, w3

        auto temp = matrix * geometries.at(local_id).transformation_matrix_ * xz * rot;
        glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &temp[0][0]);
        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, geometries.at(local_id).vertex_buffer_id_lines);
        glVertexAttribPointer(0,
                              3, // size
                              GL_FLOAT, // type
                              GL_FALSE, // normalized?
                              sizeof(float) * 3, // stride
                              (void *) 0 // array buffer offset
        );
        glDrawArrays(GL_LINES, 0, 2 * geometries.at(local_id).num_edge_);
        glDisableVertexAttribArray(0);
      }

    } else if (e == 1) {
      draw_bounding = true;
      {
        glm::mat4 rot(1, 0, 0, 0, // x0, y0, z0, w0
                      0, 0, 1, 0, // x1, y1, z1, w1
                      0, 1, 0, 0, // x2, y2, z2, w2
                      0, 0, 0, 1); // x3, y3, z3, w3

        glm::mat4 yz(0, 0, 0, 0, // x0, y0, z0, w0
                     0, 1, 0, 0, // x1, y1, z1, w1
                     0, 0, 1, 0, // x2, y2, z2, w2
                     -1 + tx, 0, 0, 1); // x3, y3, z3, w3
        auto temp = matrix * geometries.at(local_id).transformation_matrix_ * yz * rot;
        glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &temp[0][0]);
        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, geometries.at(local_id).vertex_buffer_id_lines);
        glVertexAttribPointer(0,
                              3, // size
                              GL_FLOAT, // type
                              GL_FALSE, // normalized?
                              sizeof(float) * 3, // stride
                              (void *) 0 // array buffer offset
        );
        glDrawArrays(GL_LINES, 0, 2 * geometries.at(local_id).num_edge_);
        glDisableVertexAttribArray(0);
      }
    } else {
      draw_bounding = false;
    }
  }
}

void ApplyTranslation(int local_id, float tx, float ty, float tz) {
  // Modify original data
  int k = 0;
  for (auto &item : geometries.at(local_id).g_vertex_buffer_triangles) {
    if ((k % 3) == 0)
      item += tx;
    if ((k % 3) == 1)
      item += ty;
    if ((k % 3) == 2)
      item += tz;
    ++k;
  }

  glm::mat4 translation(1, 0, 0, 0, // x0, y0, z0, w0
                        0, 1, 0, 0, // x1, y1, z1, w1
                        0, 0, 1, 0, // x2, y2, z2, w2
                        0, 0, 0, 1); // x3, y3, z3, w3
  translation[3][0] = tx;
  translation[3][1] = ty;
  translation[3][2] = tz;
  geometries.at(local_id).transformation_matrix_ = geometries.at(local_id).transformation_matrix_ * translation;
  geometries.at(local_id).CalculateCenterMass();
}

void ApplyRotation(int local_id, float angle, float ax, float ay, float az, float bx, float by, float bz) {
  float dx = bx - ax, dy = by - ay, dz = bz - az;
  // Euclidean norm
  float L = sqrt(static_cast<float>(pow(dx, 2) + pow(dy, 2) + pow(dz, 2)));
  if (L != 0) {

    // M1
    int k = 0;
    for (auto &item : geometries.at(local_id).g_vertex_buffer_triangles) {
      if ((k % 3) == 0)
        item -= ax;
      if ((k % 3) == 1)
        item -= ay;
      if ((k % 3) == 2)
        item -= az;
      ++k;
    }

    glm::mat4 translation_negative_a(1, 0, 0, 0, // x0, y0, z0, w0
                                     0, 1, 0, 0, // x1, y1, z1, w1
                                     0, 0, 1, 0, // x2, y2, z2, w2
                                     0, 0, 0, 1); // x3, y3, z3, w3
    translation_negative_a[3][0] = -ax;
    translation_negative_a[3][1] = -ay;
    translation_negative_a[3][2] = -az;

    // M2 = M_22 * M_21
    auto l = sqrt(pow(dx, 2) + pow(dz, 2));
    glm::mat4 M_22(l, 0, -dx, 0, // x0, y0, z0, w0
                   0, 1, 0, 0, // x1, y1, z1, w1
                   dx, 0, l, 0, // x2, y2, z2, w2
                   0, 0, 0, 1); // x3, y3, z3, w3

    glm::mat4 M_21(1, 0, 0, 0, // x0, y0, z0, w0
                   0, dz / L, -dy / L, 0, // x1, y1, z1, w1
                   0, dy / L, dz / L, 0, // x2, y2, z2, w2
                   0, 0, 0, 1); // x3, y3, z3, w3

    glm::mat4 M_2 = M_22 * M_21;

    // M3
    angle = angle * M_PI / 180;
    glm::mat4 M3(cos(angle), -sin(angle), 0, 0, // x0, y0, z0, w0
                 sin(angle), cos(angle), 0, 0, // x1, y1, z1, w1
                 0, 0, 1, 0, // x2, y2, z2, w2
                 0, 0, 0, 1); // x3, y3, z3, w3

    // M4  = inverse of M2
    float det = Det(M_2);
    glm::mat4 inverse_M2;
    if (det != 0)
      inverse_M2 = Inverse_Matrix(M_2, det);
    else
      throw std::runtime_error("Matrix M2 is not invertible");

    // M5
    glm::mat4 translation_a(1, 0, 0, 0, // x0, y0, z0, w0
                            0, 1, 0, 0, // x1, y1, z1, w1
                            0, 0, 1, 0, // x2, y2, z2, w2
                            0, 0, 0, 1); // x3, y3, z3, w3
    translation_negative_a[3][0] = ax;
    translation_negative_a[3][1] = ay;
    translation_negative_a[3][2] = az;


    auto M = translation_a * inverse_M2 * M3 * M_2 * translation_negative_a;
    geometries.at(local_id).transformation_matrix_ *= M;
    ShowVertexCoordinate(local_id);
  }
}

void ApplyScaling(int local_id, float sx, float sy, float sz) {
  // Compute C
  int k = 0;
  float cx = 0.f, cy = 0.f, cz = 0.f;
  for (auto &item : geometries.at(local_id).g_vertex_buffer_triangles) {
    if ((k % 3) == 0)
      cx += item;
    if ((k % 3) == 1)
      cy += item;
    if ((k % 3) == 2)
      cz += item;
    ++k;
  }
  cx /= geometries.at(local_id).g_vertex_buffer_triangles.size();
  cy /= geometries.at(local_id).g_vertex_buffer_triangles.size();
  cz /= geometries.at(local_id).g_vertex_buffer_triangles.size();

  // Translate by -C
  k = 0;
  for (auto &item : geometries.at(local_id).g_vertex_buffer_triangles) {
    if ((k % 3) == 0) {
      item -= cx;
      item *= sx;
      item += cx;
    }

    if ((k % 3) == 1) {
      item -= cy;
      item *= sy;
      item += cy;
    }

    if ((k % 3) == 2) {
      item -= cz;
      item *= sz;
      item += cz;
    }
    ++k;
  }

  // Displaying
  glm::mat4 translation_negative_C(1, 0, 0, 0, // x0, y0, z0, w0
                                   0, 1, 0, 0, // x1, y1, z1, w1
                                   0, 0, 1, 0, // x2, y2, z2, w2
                                   0, 0, 0, 1); // x3, y3, z3, w3
  translation_negative_C[3][0] = -cx;
  translation_negative_C[3][1] = -cy;
  translation_negative_C[3][2] = -cz;

  glm::mat4 scaling(1, 0, 0, 0, // x0, y0, z0, w0
                    0, 1, 0, 0, // x1, y1, z1, w1
                    0, 0, 1, 0, // x2, y2, z2, w2
                    0, 0, 0, 1); // x3, y3, z3, w3
  scaling[0][0] = sx;
  scaling[1][1] = sy;
  scaling[2][2] = sz;

  glm::mat4 translation_C(1, 0, 0, 0, // x0, y0, z0, w0
                          0, 1, 0, 0, // x1, y1, z1, w1
                          0, 0, 1, 0, // x2, y2, z2, w2
                          0, 0, 0, 1); // x3, y3, z3, w3
  translation_C[3][0] = cx;
  translation_C[3][1] = cy;
  translation_C[3][2] = cz;

  geometries.at(local_id).transformation_matrix_ *= translation_C * scaling * translation_negative_C;
}

// For Debugging purpose
void ShowVertexCoordinate(int local_id) {
  int i = 0;
  int k = 0;
  for (auto &item : geometries.at(local_id).g_vertex_buffer_triangles) {
    if ((k % 3) == 0) {
      std::cout << "v" << i << " : ";
      ++i;
      std::cout << "x: " << item;
    }

    if ((k % 3) == 1)
      std::cout << " y: " << item;

    if ((k % 3) == 2)
      std::cout << " z: " << item << std::endl;
    ++k;
  }
  std::cout << std::endl;
}

float Det(glm::mat4 M_2) {
  auto a = M_2[0][0], b = M_2[0][1], c = M_2[0][2], d = M_2[0][3],
      e = M_2[1][0], f = M_2[1][1], g = M_2[1][2], h = M_2[1][3],
      i = M_2[2][0], j = M_2[2][1], k_0 = M_2[2][2], l_0 = M_2[2][3],
      m = M_2[3][0], n = M_2[3][1], o = M_2[3][2], p = M_2[3][3];

  // Find Det(A)
  float det = a * (f * k_0 * p + j * o * h + n * g * l_0 - n * k_0 * h - f * l_0 * o - j * g * p)
      - b * (e * k_0 * p + i * o * h + g * l_0 * m - m * k_0 * h - e * l_0 * o - i * g * p)
      + c * (e * j * p + i * n * h + f * l_0 * m - m * j * h - e * l_0 * n - i * f * p)
      - d * (e * j * o + i * n * g + m * k_0 * f - m * j * g - e * k_0 * n - i * f * o);
  return det;
}

glm::mat4 Inverse_Matrix(glm::mat4 M_2, float det) {
  glm::mat4 inverse;
  // Hard coded
  // Row 0
  inverse[0][0] =
      M_2[1][1] * M_2[2][2] * M_2[3][3] + M_2[1][2] * M_2[2][3] * M_2[3][1] + M_2[1][3] * M_2[2][1] * M_2[3][2]
          - M_2[1][3] * M_2[2][2] * M_2[3][1] - M_2[1][2] * M_2[2][1] * M_2[3][3] - M_2[1][1] * M_2[2][3] * M_2[3][2];

  inverse[0][1] =
      M_2[1][0] * M_2[2][2] * M_2[3][3] + M_2[1][2] * M_2[2][3] * M_2[3][0] + M_2[1][3] * M_2[2][0] * M_2[3][2]
          - M_2[1][3] * M_2[2][2] * M_2[3][0] - M_2[1][2] * M_2[2][0] * M_2[3][3] - M_2[1][0] * M_2[2][3] * M_2[3][2];
  inverse[0][1] *= -1;

  inverse[0][2] =
      M_2[1][0] * M_2[2][1] * M_2[3][3] + M_2[1][1] * M_2[2][3] * M_2[3][0] + M_2[1][3] * M_2[2][0] * M_2[3][1]
          - M_2[1][3] * M_2[2][1] * M_2[3][0] - M_2[1][1] * M_2[2][0] * M_2[3][3] - M_2[1][0] * M_2[2][3] * M_2[3][1];

  inverse[0][3] =
      M_2[1][0] * M_2[2][1] * M_2[3][2] + M_2[1][1] * M_2[2][2] * M_2[3][0] + M_2[1][2] * M_2[2][0] * M_2[3][1]
          - M_2[1][2] * M_2[2][1] * M_2[3][0] - M_2[1][1] * M_2[2][0] * M_2[3][2] - M_2[1][0] * M_2[2][2] * M_2[3][1];
  inverse[0][3] *= -1;

  // Row 1
  inverse[1][0] =
      M_2[0][1] * M_2[2][2] * M_2[3][3] + M_2[0][2] * M_2[2][3] * M_2[3][1] + M_2[0][3] * M_2[2][1] * M_2[3][2]
          - M_2[0][3] * M_2[2][2] * M_2[3][1] - M_2[0][2] * M_2[2][1] * M_2[3][3] - M_2[0][1] * M_2[2][3] * M_2[3][2];
  inverse[1][0] *= -1;

  inverse[1][1] =
      M_2[0][0] * M_2[2][2] * M_2[3][3] + M_2[0][2] * M_2[2][3] * M_2[3][0] + M_2[0][3] * M_2[2][0] * M_2[3][2]
          - M_2[0][3] * M_2[2][2] * M_2[3][0] - M_2[0][2] * M_2[2][0] * M_2[3][3] - M_2[0][0] * M_2[2][3] * M_2[3][2];

  inverse[1][2] =
      M_2[0][0] * M_2[2][1] * M_2[3][3] + M_2[0][1] * M_2[2][3] * M_2[3][0] + M_2[0][3] * M_2[2][0] * M_2[3][1]
          - M_2[0][3] * M_2[2][1] * M_2[3][0] - M_2[0][1] * M_2[2][0] * M_2[3][3] - M_2[0][0] * M_2[2][3] * M_2[3][1];
  inverse[1][2] *= -1;

  inverse[1][3] =
      M_2[0][0] * M_2[2][1] * M_2[3][2] + M_2[0][1] * M_2[2][2] * M_2[3][0] + M_2[0][2] * M_2[2][0] * M_2[3][1]
          - M_2[0][2] * M_2[2][1] * M_2[3][0] - M_2[0][1] * M_2[2][0] * M_2[3][2] - M_2[0][0] * M_2[2][2] * M_2[3][1];

  // Row 2
  inverse[2][0] =
      M_2[0][1] * M_2[1][2] * M_2[3][3] + M_2[0][2] * M_2[1][3] * M_2[3][1] + M_2[0][3] * M_2[1][1] * M_2[3][2]
          - M_2[0][3] * M_2[1][2] * M_2[3][1] - M_2[0][2] * M_2[1][1] * M_2[3][3] - M_2[0][1] * M_2[1][3] * M_2[3][2];
  inverse[2][1] =
      M_2[0][0] * M_2[1][2] * M_2[3][3] + M_2[0][2] * M_2[1][3] * M_2[3][0] + M_2[0][3] * M_2[1][0] * M_2[3][2]
          - M_2[0][3] * M_2[1][2] * M_2[3][0] - M_2[0][2] * M_2[1][0] * M_2[3][3] - M_2[0][0] * M_2[1][3] * M_2[3][2];
  inverse[2][1] *= -1;

  inverse[2][2] =
      M_2[0][0] * M_2[1][1] * M_2[3][3] + M_2[0][1] * M_2[1][3] * M_2[3][0] + M_2[0][3] * M_2[1][0] * M_2[3][1]
          - M_2[0][3] * M_2[1][1] * M_2[3][0] - M_2[0][1] * M_2[1][0] * M_2[3][3] - M_2[0][0] * M_2[1][3] * M_2[3][1];
  inverse[2][3] =
      M_2[0][0] * M_2[1][1] * M_2[3][2] + M_2[0][1] * M_2[1][2] * M_2[3][0] + M_2[0][2] * M_2[1][0] * M_2[3][1]
          - M_2[0][2] * M_2[1][1] * M_2[3][0] - M_2[0][1] * M_2[1][0] * M_2[3][2] - M_2[0][0] * M_2[1][2] * M_2[3][1];
  inverse[2][3] *= -1;

  // Row 3
  inverse[3][0] =
      M_2[0][1] * M_2[1][2] * M_2[2][3] + M_2[0][2] * M_2[1][3] * M_2[2][1] + M_2[0][3] * M_2[1][1] * M_2[2][2]
          - M_2[0][3] * M_2[1][2] * M_2[2][1] - M_2[0][2] * M_2[1][1] * M_2[2][3] - M_2[0][1] * M_2[1][3] * M_2[2][2];
  inverse[3][0] *= -1;

  inverse[3][1] =
      M_2[0][0] * M_2[1][2] * M_2[2][3] + M_2[0][2] * M_2[1][3] * M_2[2][0] + M_2[0][3] * M_2[1][0] * M_2[2][2]
          - M_2[0][3] * M_2[1][2] * M_2[2][0] - M_2[0][2] * M_2[1][0] * M_2[2][3] - M_2[0][0] * M_2[1][3] * M_2[2][2];

  inverse[3][2] =
      M_2[0][0] * M_2[1][1] * M_2[2][3] + M_2[0][1] * M_2[1][3] * M_2[2][0] + M_2[0][3] * M_2[1][0] * M_2[2][1]
          - M_2[0][3] * M_2[1][1] * M_2[2][0] - M_2[0][1] * M_2[1][0] * M_2[2][3] - M_2[0][0] * M_2[1][3] * M_2[2][1];
  inverse[3][2] *= -1;

  inverse[3][3] =
      M_2[0][0] * M_2[1][1] * M_2[2][2] + M_2[0][1] * M_2[1][2] * M_2[2][0] + M_2[0][2] * M_2[1][0] * M_2[2][1]
          - M_2[0][2] * M_2[1][1] * M_2[2][0] - M_2[0][1] * M_2[1][0] * M_2[2][2] - M_2[0][0] * M_2[1][2] * M_2[2][1];

  for (int kI = 0; kI < 4; ++kI)
    for (int kJ = 0; kJ < 4; ++kJ)
      inverse[kI][kJ] /= det;
  return inverse;
}
