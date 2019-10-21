#version 330 core

in vec3 fragmentColor;
out vec3 color;
//out vec3 Normal;

void main(){
  color = fragmentColor;
  //Normal = vertexNormal;
}