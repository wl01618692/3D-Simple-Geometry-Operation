add_executable(run_p2 main.cpp gui.cpp gui.hpp)
target_link_libraries(run_p2 PUBLIC util-core-3.3 glm)
set_target_properties(run_p2 PROPERTIES CXX_STANDARD 11) 
target_add_embeded_shaders(run_p2
  OUTPUT_NAME shaders.h SHADERS 
  vshader=TransformVertexShader.glsl 
  fshader=ColorFragmentShader.glsl)
