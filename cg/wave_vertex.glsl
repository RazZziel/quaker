uniform float wave;
uniform vec3 path;

void main()
{
    vec4 vertex = gl_Vertex; // gl_Vertex is read-only

    if (vertex.x < path.x || vertex.x > path.y)
        vertex.y = ( sin(wave + (vertex.x / 5.0) ) +
                     sin(wave + (vertex.z / 4.0) ) ) * 2.5;
    else
        vertex.y += path.z * cos(vertex.x/path.y);

    // Transform The Vertex Position Into Homogenous Clip-Space (Required)
    gl_Position = ftransform();

    gl_FrontColor.x = sin(vertex.x + vertex.y);
    gl_FrontColor.y = cos(vertex.x + vertex.y);
    gl_FrontColor.z = tan(vertex.x + vertex.y);
    gl_FrontColor.w = gl_Color.w;
}
