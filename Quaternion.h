/*
  Copyright (C) 2008 Alex Diener

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Alex Diener adiener@sacredsoftware.net
*/

#ifndef __QUATERNION_H__
#define __QUATERNION_H__

//#include "vector3d.h"

typedef struct Quaternion Quaternion;

// struct Vector;
// struct Matrix;

struct Quaternion {
	float x;
	float y;
	float z;
	float w;
};

void Quaternion_loadIdentity(Quaternion * quaternion);
Quaternion Quaternion_identity();
Quaternion Quaternion_withValues(float x, float y, float z, float w);

Quaternion Quaternion_fromVector(struct vec3 vector);
struct vec3 Quaternion_toVector(Quaternion quaternion);
Quaternion Quaternion_fromAxisAngle(double Angle, vec3 Axis);
void Quaternion_toAxisAngle(Quaternion quaternion, struct vec3 * axis, float * angle);
// struct Matrix Quaternion_toMatrix(Quaternion quaternion);

void Quaternion_normalize(Quaternion * quaternion);
Quaternion Quaternion_normalized(Quaternion quaternion);

void Quaternion_multiply(Quaternion * quaternion1, Quaternion quaternion2);
Quaternion Quaternion_multiplied(Quaternion quaternion1, Quaternion quaternion2);
Quaternion Quaternion_slerp(Quaternion start, Quaternion end, float alpha);

// void Quaternion_rotate(Quaternion * quaternion, struct vec3 axis, float angle);
// Quaternion Quaternion_rotated(Quaternion quaternion, struct vec3 axis, float angle);

void Quaternion_invert(Quaternion * quaternion);
Quaternion Quaternion_inverted(Quaternion quaternion);

struct vec3 Quaternion_multiplyVector(Quaternion quaternion, struct vec3 vector);

Quaternion Quaternion_Mul_OrientationAxis(Quaternion AxisAngle, Quaternion m_Orientation);

#endif
