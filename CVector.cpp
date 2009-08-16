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


#include "CVector.hpp"


void Vec::normalize(void)
{
    float mod = module();

    x /= mod;
    y /= mod;
    z /= mod;
}

void Vec::reverse(void)
{
    x = -x;
    y = -y;
    z = -z;
}

Vec& Vec::operator+=(Vec u)
{
    x += u.x;
    y += u.y;
    y += u.z;

    return *this;
}

Vec& Vec::operator-=(Vec u)
{
    x -= u.x;
    y -= u.y;
    y -= u.z;

    return *this;
}

Vec& Vec::operator*=(float s)
{
    x *= s;
    y *= s;
    y *= s;

    return *this;
}

Vec& Vec::operator/=(float s)
{
    x /= s;
    y /= s;
    y /= s;

    return *this;
}

#include <iostream>
std::ostream& operator<< (std::ostream& out, const Vec& p )
{
    return out << '(' << p.x << ' ' << p.y << ' ' << p.z << ')';
}
