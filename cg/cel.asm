!!ARBvp1.0
# Vertex Program for Cel shading

# Constant Parameters
PARAM mvp[4]         = { state.matrix.mvp };    # modelview projection matrix
PARAM lightPosition  = program.local[0];        # Object space light position

# Per-vertex inputs
ATTRIB inPosition    = vertex.position;         # Position
ATTRIB inColor       = vertex.color;            # Color
ATTRIB inNormal      = vertex.normal;           # Normal

# Temporaries
TEMP lightVector;                               # Light vector
TEMP normLightVector;                           # Normalized light vector

# Outputs
OUTPUT outPosition   = result.position;         # Position
OUTPUT outColor      = result.color;            # Primary color
OUTPUT outTexture    = result.texcoord[0];      # Texture coordinate set 0

# Output position
DP4 outPosition.x, mvp[0], inPosition;
DP4 outPosition.y, mvp[1], inPosition;
DP4 outPosition.z, mvp[2], inPosition;
DP4 outPosition.w, mvp[3], inPosition;

MOV outColor, inColor;                         # Output color

SUB lightVector, lightPosition, inPosition;    # Calculate light vector

# Normalize light vector
DP3 normLightVector.w, lightVector, lightVector;
RSQ normLightVector.w, normLightVector.w;
MUL normLightVector.xyz, normLightVector.w, lightVector;

DP3 outTexture.x, normLightVector, inNormal;   # Dot light vector with normal

END
