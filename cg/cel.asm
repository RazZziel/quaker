!!ARBvp1.0
# Vertex Program for cel shading

# Parameters
PARAM mvp[4]		= { state.matrix.mvp };		#modelview projection matrix
PARAM lightPosition = program.env[0];			#object space light position

# Per vertex inputs
ATTRIB iPos			= vertex.position;			#position
ATTRIB iCol0		= vertex.color;				#color
ATTRIB iNorm		= vertex.normal;			#normal

# Temporaries
TEMP lightVector;		#light vector
TEMP normLightVector;	#normalized light vector

# Outputs
OUTPUT oPos			= result.position;			#position
OUTPUT oCol0		= result.color;				#primary color
OUTPUT oTex0		= result.texcoord[0];		#texture coordinate set 0




#Output position
DP4 oPos.x, mvp[0], iPos;
DP4 oPos.y, mvp[1], iPos;
DP4 oPos.z, mvp[2], iPos;
DP4 oPos.w, mvp[3], iPos;

#Output color
MOV oCol0, iCol0;

#Calculate light vector
SUB lightVector, lightPosition, iPos;

#Normalize light vector
DP3 normLightVector.w, lightVector, lightVector;
RSQ normLightVector.w, normLightVector.w;
MUL normLightVector.xyz, normLightVector.w, lightVector;

#Dot light vector with normal
DP3 oTex0.x, normLightVector, iNorm;


END
