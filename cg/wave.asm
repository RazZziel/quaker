!!ARBvp1.0
# cgc version 2.2.0006, build date Mar 31 2009
# command line args: -profile arbvp1
# source file: wave.cg
#vendor NVIDIA Corporation
#version 2.2.0.6
#profile arbvp1
#program main
#semantic main.ModelViewProj
#var float4 IN.position : $vin.POSITION : POSITION : 0 : 1
#var float4 IN.color : $vin.COLOR0 : COLOR0 : 0 : 1
#var float IN.wave : $vin.ATTR1 : ATTR1 : 0 : 1
#var float3 IN.path : $vin.ATTR2 : ATTR2 : 0 : 1
#var float4x4 ModelViewProj :  : c[1], 4 : 1 : 1
#var float4 main.HPos : $vout.POSITION : HPOS : -1 : 1
#var float4 main.Col0 : $vout.COLOR0 : COL0 : -1 : 1
#const c[0] = 0.2 0.25 0 0.15915491
#const c[5] = 24.980801 -24.980801 -60.145809 60.145809
#const c[6] = 0 0.5 1 -1
#const c[7] = 85.453789 -85.453789 -64.939346 64.939346
#const c[8] = 19.73921 -19.73921 -9 0.75
#const c[9] = 2.5 0.25 0.75
#const c[10] = 0.25 0.5 0.75 1
#const c[11] = -19.73921 1
PARAM c[12] = { { 0.2, 0.25, 0, 0.15915491 },
		program.local[1..4],
		{ 24.980801, -24.980801, -60.145809, 60.145809 },
		{ 0, 0.5, 1, -1 },
		{ 85.453789, -85.453789, -64.939346, 64.939346 },
		{ 19.73921, -19.73921, -9, 0.75 },
		{ 2.5, 0.25, 0.75 },
		{ 0.25, 0.5, 0.75, 1 },
		{ -19.73921, 1 } };
TEMP R0;
TEMP R1;
TEMP R2;
TEMP R3;
TEMP R4;
MUL R0.x, vertex.position, c[0];
ADD R0.x, R0, vertex.attrib[1];
MAD R0.x, R0, c[0].w, -c[0].y;
FRC R0.w, R0.x;
ADD R0.xyz, -R0.w, c[6];
MUL R0.xyz, R0, R0;
MAD R1.xyz, R0, c[5].xyxw, c[5].zwzw;
MAD R1.xyz, R1, R0, c[7].xyxw;
MAD R1.xyz, R1, R0, c[7].zwzw;
MAD R1.xyz, R1, R0, c[8].xyxw;
MAD R0.xyz, R1, R0, c[6].wzww;
SGE R1.yz, R0.w, c[8].xzww;
SLT R1.x, R0.w, c[0].y;
MUL R1.w, vertex.position.z, c[0].y;
ADD R0.w, vertex.attrib[1].x, R1;
MAD R0.w, R0, c[0], -c[0].y;
FRC R2.w, R0;
DP3 R1.y, R1, c[6].wzww;
DP3 R0.w, R0, -R1;
ADD R0.xyz, -R2.w, c[6];
RCP R1.x, vertex.attrib[2].y;
MUL R1.w, vertex.position.x, R1.x;
MUL R0.xyz, R0, R0;
MAD R1.xyz, R0, c[5].xyxw, c[5].zwzw;
MAD R1.xyz, R1, R0, c[7].xyxw;
MAD R1.xyz, R1, R0, c[7].zwzw;
MAD R1.xyz, R1, R0, c[8].xyxw;
MUL R1.w, R1, c[0];
FRC R1.w, R1;
ADD R2.xyz, -R1.w, c[6];
MUL R2.xyz, R2, R2;
MAD R1.xyz, R1, R0, c[6].wzww;
MAD R3.xyz, R2, c[5].xyxw, c[5].zwzw;
SLT R4.x, R2.w, c[0].y;
SGE R4.yz, R2.w, c[8].xzww;
MOV R0.xz, R4;
DP3 R0.y, R4, c[6].wzww;
DP3 R1.x, R1, -R0;
ADD R0.w, R0, R1.x;
MAD R0.xyz, R3, R2, c[7].xyxw;
MAD R0.xyz, R0, R2, c[7].zwzw;
MAD R1.z, R0.w, c[9].x, -vertex.position.y;
MOV R0.w, vertex.position.x;
MAD R0.xyz, R0, R2, c[8].xyxw;
SLT R1.y, vertex.attrib[2], R0.w;
MOV R1.x, vertex.attrib[2];
SLT R0.w, vertex.position.x, R1.x;
ADD R0.w, R0, R1.y;
MAD R2.w, R0, R1.z, vertex.position.y;
MAD R1.xyz, R0, R2, c[6].wzww;
SGE R0.yz, R1.w, c[8].xzww;
SLT R0.x, R1.w, c[0].y;
DP3 R2.y, R0, c[6].wzww;
MOV R2.xz, R0;
DP3 R0.x, R1, -R2;
ABS R0.y, R0.w;
MUL R0.x, vertex.attrib[2].z, R0;
SGE R0.y, c[0].z, R0;
MAD R0.w, R0.x, R0.y, R2;
ADD R0.z, R0.w, vertex.position.x;
MUL R0.x, R0.z, c[0].w;
FRC R2.w, R0.x;
SLT R1, R2.w, c[10];
ADD R2.xyz, R1.yzww, -R1;
MOV R1.yzw, R2.xxyz;
DP4 R0.y, R1, c[9].yyzz;
DP3 R0.x, R2, c[6].yyzw;
ADD R0.xy, R2.w, -R0;
MUL R0.xy, R0, R0;
MUL R2.xy, R0, R0;
MUL R3, R2.xxyy, c[5].ywyw;
ADD R3, R3, c[7].ywyw;
MAD R3, R3, R2.xxyy, c[11].xyxy;
DP4 R2.y, R1, c[6].zzww;
DP4 R2.x, R1, c[6].zwwz;
MAD R0.xy, R3.xzzw, R0, R3.ywzw;
MUL R0.xy, R2, R0;
RCP R1.x, R0.x;
MAD R0.x, R0.z, c[0].w, -c[0].y;
FRC R1.w, R0.x;
MUL result.color.z, R0.y, R1.x;
ADD R1.xyz, -R1.w, c[6];
MUL R1.xyz, R1, R1;
MAD R2.xyz, R1, c[5].xyxw, c[5].zwzw;
MAD R2.xyz, R2, R1, c[7].xyxw;
MAD R2.xyz, R2, R1, c[7].zwzw;
MAD R2.xyz, R2, R1, c[8].xyxw;
ADD R0.xyz, -R2.w, c[6];
MUL R0.xyz, R0, R0;
MAD R3.xyz, R0, c[5].xyxw, c[5].zwzw;
MAD R3.xyz, R3, R0, c[7].xyxw;
MAD R3.xyz, R3, R0, c[7].zwzw;
MAD R3.xyz, R3, R0, c[8].xyxw;
MAD R1.xyz, R2, R1, c[6].wzww;
MAD R3.xyz, R3, R0, c[6].wzww;
SLT R4.x, R2.w, c[0].y;
SGE R4.yz, R2.w, c[8].xzww;
MOV R0.xz, R4;
DP3 R0.y, R4, c[6].wzww;
DP3 result.color.y, R3, -R0;
SLT R0.x, R1.w, c[0].y;
SGE R0.yz, R1.w, c[8].xzww;
MOV R2.xz, R0;
DP3 R2.y, R0, c[6].wzww;
DP3 result.color.x, R1, -R2;
MOV R1.xzw, vertex.position;
MOV R1.y, R0.w;
DP4 result.position.w, R1, c[4];
DP4 result.position.z, R1, c[3];
DP4 result.position.y, R1, c[2];
DP4 result.position.x, R1, c[1];
MOV result.color.w, vertex.color;
END
# 112 instructions, 5 R-regs
