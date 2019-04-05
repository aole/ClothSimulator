#version 330 core

in vec3 color;
out vec4 fragmentColor;

void main(){
  fragmentColor = vec4(color, 1); //fragmentColor;
  //gl_FragColor = vec3(.1f, .3f, .5f); //fragmentColor;
}
