#version 330 core

uniform vec3 lightPosition;

in vec3 fragmentPosition;
in vec3 fragmentNormal;
in vec3 fragmentColor;

out vec3 color;

void main(){
  vec3 norm = normalize(fragmentNormal);
  vec3 lightDir = normalize(lightPosition - fragmentPosition);
  float diffuse = max(dot(norm, lightDir), 0.0);

  color = diffuse * fragmentColor;
}
