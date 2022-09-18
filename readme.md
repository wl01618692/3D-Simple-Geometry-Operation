## General info
This program supports 3D translation, rotation, scaling, and displays three orthographic projections. The user can interact with the program through a GUI by pressing G after running the program.

The final scene will be written to a data file replacing the one being input. This part is handled at line 57 in gui.cpp by
```C++
void OutputFile(const std::string &input)
```
## Usage
To run the program, please specify the path of the input file as the first command line argument. The user can also specify the input file path through the GUI.
 The program will detect and inform the user to specify input file path through the GUI if the number of command line argument is less than 2. The program will throw a runtime error and ask the user to input again if the input file cannot be opened.
```bash
./run_p2 /projects/p1/input1.data
```
![Alt text](./input_file.png?raw=true "Title")

All VBOs of the input objects are created in one function and can be found at line 86 in gui.cpp.
```C++
void CreateVBO();
```
By default, the id of each object is specified by the order the program read in. The first object, a tetrahedron, will have id 0. The second object, a perfect square, will have id 1 and so on. Input1.data is already provided to the user and is correctly formatted. The readfile function can be found at line 20 in gui.cpp by
```C++
void ReadFile(const std::string &input) 
```

After running the program, the program implements shader rendering pipeline with a MVP matrix,a global variable. Each object, known as the variable geometry, also has its own transformation matrix. In each iteration of the while loop, each object's transformation matrix will be passed into the shader when its their turn.
```C++
struct Geometry {
  ...
  glm::mat4 transformation_matrix_ = {1, 0, 0, 0,
                                   0, 1, 0, 0,
                                   0, 0, 1, 0,
                                   0, 0, 0, 1};
};
```
The program also used a vector of Geometry as a global variable to store the objects.
```C++
std::vector<Geometry> geometries;
```


To interact with the program, press G to activate a GUI. In the GUI, there are three sections: input file, 3D transformation, and orthographic projection. The function for GUI can be found at line 110 at gui.cpp.

To exit the program, press ESC key.

##Input File Format
The coordinates of objects in the input file supports any floating point number from the domain [0,1] X [0,1] X [0,1].

The input file has the following format:\
number of objects

4 number of points of 1st object \
0 0 0 coordinates of 1st point \
0.5 0 0 coordinates of 2nd point \
0 0.5 0 coordinates of 3rd point \
0 0 0.5 coordinates of 4th point \
6 number of edges of 1st object \
1 2 first edge\
1 3 second edge\
1 4 third edge\
2 3 fourth edge\
2 4 fifth edge\
3 4 sixth edge

8 number of points of 2nd object \
0.5 -0.5 -0.5 coordinates of 1st point \
1 -0.5 -0.5 coordinates of 2nd point \
0.5 -1 -0.5 coordinates of 3rd point \
1 -1 -0.5 coordinates of 4th point \
0.5 -0.5 0\
1 -0.5 0\
0.5 -1 0\
1 -1 0\
12 number of edges of 2nd object \
1 2\
1 3\
2 4\
3 4\
1 5\
2 6\
3 7\
4 8\
6 8\
7 8\
5 6\
5 7
### 3D linear Transformation
The user can specify polygon individually through its polygon ID. After choosing the polygon ID, the user can specify the scaling factor, translation vector, and rotation angle by dragging each corresponding bar and click apply. 
![Alt text](./3D_Transformation_GUI.png?raw=true "Title")

Since the order of transformation is important if multiple transformation is applied, the user can only specify one transformation at a time, meaning that the user need to apply each transformation in order.

The implementation behind these transformation is that the original points stored in vertex is first modified according to the operation and then its transformation matrix is also modified. The transformation matrix will later get sent to the shader when its the objects turn.
```C++
  glm::mat4 translation(1, 0, 0, 0, // x0, y0, z0, w0
                        0, 1, 0, 0, // x1, y1, z1, w1
                        0, 0, 1, 0, // x2, y2, z2, w2
                        tx, ty, tz, 1); // x3, y3, z3, w3
```

Translation works properly as shown in the following picture. Translation function can be found at line 313 in gui.cpp.
![Alt text](./Translation1.png?raw=true "Title")

Rotation works properly as shown in the following picture. Rotation function can be found at line 337 in gui.cpp. Rotation is comprised of product of 5 matrices as discussed in lecture. Determinant of a given matrix is calculated at line 503 in gui.cpp by the function
```C++
float Det(glm::mat4 M_2)
```
Inverse matrix is calculated at line 517 in gui.cpp by analytic solution of Cramer's rule from linear algebra by
```C++
glm::mat4 Inverse_Matrix(glm::mat4 M_2, float det)
```
![Alt text](./Rotation1.png?raw=true "Title")

Scaling works properly as shown in the following picture. Scaling function can be found at line 414 in gui.cpp. Scaling is done through first translation to the origin, then scale and translate back to its original position. The scaling matrix is a product of three matrices, the first one being a translation by -c, the second one is normal scaling at origin, and the thrid one is tranlsation by c, where c is the center of mass in 3D. A center of mass function can be found at line 34 in gui.hpp.
![Alt text](./Scaling1.png?raw=true "Title")
### Orthorgraphic projections
The GUI interface of this section looks like the following. One downside of my implementation is that this program cannot display all three projections at once but one projection only. A bounding box will be drawn after switching from default to either one of the projection.
The actual function can be found at 196 in gui.cpp.

![Alt text](./Orthographic_GUI.png?raw=true "Title")
The implemetation of these projections are that I applied a rotaiton matrix to AxonYZ, AxonXZ before multiplying to object's transformation matrix. Then the program draw it immediately on the viewport. They look like the following.
```C++
        glm::mat4 rotYZ(1, 0, 0, 0, // x0, y0, z0, w0
                      0, 0, 1, 0, // x1, y1, z1, w1
                      0, 1, 0, 0, // x2, y2, z2, w2
                      0, 0, 0, 1); // x3, y3, z3, w3
```
```C++
        glm::mat4 rotXZ(0, 1, 0, 0, // x0, y0, z0, w0
                      0, 0, 1, 0, // x1, y1, z1, w1
                      1, 0, 0, 0, // x2, y2, z2, w2
                      0, 0, 0, 1); // x3, y3, z3, w3
```
![Alt text](./xy_projection1.png?raw=true "Title")
![Alt text](./yz_projection1.png?raw=true "Title")
![Alt text](./xz_projection1.png?raw=true "Title")

By changing the radio button to default, the port looks like the original one without any modification.
