#version 330 core

// layout(location = 0) in vec3 vertexPosition_modelspace;
// attribute highp vec3 vertex;
layout(location = 0) in vec4 vertex;
// layout(location = 1) in vec3 vertexColor;

uniform vec3 vertexcolor;
out vec3 color;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){
  // Output position of the vertex, in clip space : MVP * position
  gl_Position = MVP * vertex;
  //gl_Position = vec4(vertex, 1);

  color = vertexcolor;
  //fragmentColor = shaderColor; // vec3(.4, .4, .4);
  //fragmentColor = vec3(.4, .4, .4);
}
