/***************************************************************************
 *   Copyright (C) 2006 by Ismael Barros (Raziel)                          *
 *   razielmine@gmail.com                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef _CVECTOR_H_
#define _CVECTOR_H_

#include <math.h>

class Vec {

public:
    float x, y, z;

    Vec(void) : x(0), y(0), z(0) {}
    Vec(float x, float y, float z = 0) : x(x), y(y), z(z) {}

    inline float module(void) { return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)); }
    void normalize(void);
    void reverse(void);

    Vec& operator+=(Vec u);
    Vec& operator-=(Vec u);
    Vec& operator*=(float s);
    Vec& operator/=(float s);

    inline Vec operator+(Vec u)    { return Vec(x + u.x, y + u.y, z + u.z); }
    inline Vec operator-(Vec u)    { return Vec(x - u.x, y - u.y, z - u.z); }
    inline float operator*(Vec u)  { return (x * u.x) + (y * u.y) + (z * u.z); }
    inline Vec operator*(float s)  { return Vec(x*s, y*s, z*s); }
    inline Vec operator/(float s)  { return Vec(x/s, y/s, z/s); }
    inline Vec operator-(void)     { return Vec(-x, -y, -z); }
};

#include <iostream>
std::ostream& operator<< (std::ostream& out, const Vec& p );

#endif /* _CVECTOR_H_ */
