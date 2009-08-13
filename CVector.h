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



#ifndef __CVECTOR_H__
#define __CVECTOR_H__


#define INT_VECTOR   0
#define FLOAT_VECTOR 0


class Vector {
	
public:
    float x, y, z;
		
    Vector(void) : x(0), y(0), z(0) {}
    Vector(float x, float y, float z = 0) : x(x), y(y), z(z) {}
		
    float module(void);
    void normalize(void);
    void reverse(void);
		
    Vector& operator+=(Vector u);
    Vector& operator-=(Vector u);
    Vector& operator*=(float s);
    Vector& operator/=(float s);
		
    inline Vector operator+(Vector u)	{ return Vector(x + u.x, y + u.y, z + u.z); }
    inline Vector operator-(Vector u)	{ return Vector(x - u.x, y - u.y, z - u.z); }
    inline float operator*(Vector u)	{ return (x * u.x) + (y * u.y) + (z * u.z); }
    inline Vector operator*(float s)	{ return Vector(x*s, y*s, z*s); }
    inline Vector operator/(float s)	{ return Vector(x/s, y/s, z/s); }
    inline Vector operator-(void)		{ return Vector(-x, -y, -z); }
		
};



#endif



















