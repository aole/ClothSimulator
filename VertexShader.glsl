#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
//layout(location = 1) in vec3 vertexNormal;
// layout(location = 1) in vec3 vertexColor;

uniform vec3 shaderColor;

out vec3 fragmentColor;

// Values that stay constant for the whole mesh.
//uniform mat4 MVP;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){
  // Output position of the vertex, in clip space : MVP * position
  // gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
  gl_Position = projection * view * model * vec4(vertexPosition_modelspace, 1);
  
  fragmentColor = shaderColor; // vec3(.4, .4, .4);
}
