varying vec3 normal;

void main()
{
   normal = gl_NormalMatrix * gl_Normal;
   gl_Position = ftransform(); // gl_ModelViewProjectionMatrix * gl_Vertex;
   gl_FrontColor = gl_Color;
}
