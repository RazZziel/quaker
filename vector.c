#include <math.h>
#include "vector.h"

void Normalize(VECTOR &V)
// Creates a VECTOR with a unit length of 1
{
    float M = Magnitude (V);      // Calculate The Length Of The VECTOR

    if (M != 0.0f)                // Make Sure We Don't Divide By 0
    {
        V.x /= M;                 // Normalize The 3 Components
        V.y /= M;
        V.z /= M;
    }
}

void RotateVector (MATRIX &M, VECTOR &V, VECTOR &D)                     // Rotate A Vector Using The Supplied Matrix
{
    D.x = (M.Data[0] * V.x) + (M.Data[4] * V.y) + (M.Data[8]  * V.z);   // Rotate Around The X Axis
    D.y = (M.Data[1] * V.x) + (M.Data[5] * V.y) + (M.Data[9]  * V.z);   // Rotate Around The y Axis
    D.z = (M.Data[2] * V.x) + (M.Data[6] * V.y) + (M.Data[10] * V.z);   // Rotate Around The Z Axis
}
