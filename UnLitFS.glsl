#version 330 core

in vec3 fragmentPosition;
in vec3 fragmentNormal;
in vec3 fragmentColor;

out vec3 color;

void main(){
  color = fragmentColor;
}
