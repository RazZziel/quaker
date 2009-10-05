uniform float wave;
uniform vec3 path;

void main()
{
    if (gl_Vertex.x < path.x || gl_Vertex.x > path.y)
        gl_Vertex.y = ( sin(wave + (gl_Vertex.x / 5.0) ) +
                        sin(wave + (gl_Vertex.z / 4.0) ) ) * 2.5;
    else
        gl_Vertex.y += path.z * cos(gl_Vertex.x/path.y);

    // Transform The Vertex Position Into Homogenous Clip-Space (Required)
    gl_Position = ftransform();

    gl_FrontColor.x = sin(gl_Vertex.x + gl_Vertex.y);
    gl_FrontColor.y = cos(gl_Vertex.x + gl_Vertex.y);
    gl_FrontColor.z = tan(gl_Vertex.x + gl_Vertex.y);
    gl_FrontColor.w = gl_Color.w;
}
