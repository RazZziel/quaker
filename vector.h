#ifndef _VECTOR_H_
#define _VECTOR_H_

// User Defined Structures
typedef struct tagMATRIX        // A Structure To Hold An OpenGL Matrix
{
    float Data[16];             // We Use [16] Due To OpenGL's Matrix Format
} MATRIX;

typedef struct tagVECTOR                    // A Structure To Hold A Single VECTOR
{
    float x, y, z;              // The Components Of The VECTOR
    //VECTOR(_x,_y,_z) : x(_x), y(_y), z(_z) {};
} VECTOR;

#define unfoldVector(v) (v).x, (v).y, (v).z

typedef struct tagVERTEX        // A Structure To Hold A Single Vertex
{
    VECTOR Nor;                 // Vertex Normal
    VECTOR Pos;                 // Vertex Position
} VERTEX;

typedef struct tagPOLYGON       // A Structure To Hold A Single Polygon
{
    VERTEX Verts[3];            // Array Of 3 VERTEX Structures
} POLYGON;

inline float DotProduct(VECTOR &V1, VECTOR &V2) { return V1.x * V2.x + V1.y * V2.y + V1.z * V2.z; }
inline float Magnitude(VECTOR &V) { return sqrtf (V.x * V.x + V.y * V.y + V.z * V.z); }
void Normalize(VECTOR &V);
void RotateVector (MATRIX &M, VECTOR &V, VECTOR &D);
VECTOR RotateVectorIn (MATRIX &M, VECTOR &V);

#endif /* _VECTOR_H_ */
