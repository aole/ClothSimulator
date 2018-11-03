#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
// layout(location = 1) in vec3 vertexColor;

out vec3 fragmentColor;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){
  // Output position of the vertex, in clip space : MVP * position
  gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
  
  fragmentColor = vec3(.4, .4, .4);
}