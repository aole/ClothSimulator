#version 330 core

layout(location = 0) in vec3 vertexPosition;

uniform vec3 shaderColor;

out vec3 fragmentPosition;
out vec3 fragmentColor;

// Values that stay constant for the whole mesh.
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){
  gl_Position = projection * view * model * vec4(vertexPosition, 1);
  
  fragmentPosition = vec3(model * vec4(vertexPosition, 1.0));
  fragmentColor = shaderColor; // vec3(.4, .4, .4);
}
