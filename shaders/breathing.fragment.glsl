#version 330 core
out vec4 FragColor;
  
uniform vec4 extColor;

void main()
{
    FragColor = extColor;
}