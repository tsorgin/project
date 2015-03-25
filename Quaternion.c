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

#include "Quaternion.h"

#include "vector3d.h"

#include <math.h>
#include <stdlib.h>

// #include "Matrix.h"
// #include "Vector.h"

void Quaternion_loadIdentity(Quaternion * quaternion) {
	quaternion->x = 0.0f;
	quaternion->y = 0.0f;
	quaternion->z = 0.0f;
	quaternion->w = 1.0f;
}

Quaternion Quaternion_identity() {
	Quaternion quaternion;
	
	Quaternion_loadIdentity(&quaternion);
	return quaternion;
}

Quaternion Quaternion_withValues(float x, float y, float z, float w) {
	Quaternion quaternion;
	
	quaternion.x = x;
	quaternion.y = y;
	quaternion.z = z;
	quaternion.w = w;
	return quaternion;
}

Quaternion Quaternion_fromVector(vec3 vector) {
	Quaternion quaternion;
	
	quaternion.x = vector.x;
	quaternion.y = vector.y;
	quaternion.z = vector.z;
	quaternion.w = 0.0f;
	return quaternion;
}

vec3 Quaternion_toVector(Quaternion quaternion) {
	vec3 vector;
	
	vector.x = quaternion.x;
	vector.y = quaternion.y;
	vector.z = quaternion.z;
	
	return vector;
}

Quaternion Quaternion_fromAxisAngle(double Angle, vec3 Axis) {
    //Create a new quaternion
    Quaternion AxisAngleQuat = Quaternion();
    //Angle should be in radians
    AxisAngleQuat.w = cos(Angle/2);

    //Axes
    AxisAngleQuat.x = Axis.x * sin(Angle/2);
    AxisAngleQuat.y = Axis.y * sin(Angle/2);
    AxisAngleQuat.z = Axis.z * sin(Angle/2);

    //Normalize
    //Get length
    double Length = sqrt(AxisAngleQuat.w*AxisAngleQuat.w + AxisAngleQuat.x*AxisAngleQuat.x + AxisAngleQuat.y*AxisAngleQuat.y + AxisAngleQuat.z*AxisAngleQuat.z);
    //Normalize
    AxisAngleQuat.w /= Length;
    AxisAngleQuat.x /= Length;
    AxisAngleQuat.y /= Length;
    AxisAngleQuat.z /= Length;
    //AxisAngleQuat.Normalize();

    return AxisAngleQuat;
}

void Quaternion_toAxisAngle(Quaternion quaternion, vec3 * axis, float * angle) {
	float sinAngle;
	
	Quaternion_normalize(&quaternion);
	sinAngle = sqrt(1.0f - (quaternion.w * quaternion.w));
	if (fabs(sinAngle) < 0.0005f) sinAngle = 1.0f;
	
	if (axis != NULL) {
		axis->x = (quaternion.x / sinAngle);
		axis->y = (quaternion.y / sinAngle);
		axis->z = (quaternion.z / sinAngle);
	}
	
	if (angle != NULL) {
		*angle = (acos(quaternion.w) * 2.0f);
	}
}

// Matrix Quaternion_toMatrix(Quaternion quaternion) {
// 	Matrix matrix;
	
// 	matrix.m[0]  = (1.0f - (2.0f * ((quaternion.y * quaternion.y) + (quaternion.z * quaternion.z))));
// 	matrix.m[1]  =         (2.0f * ((quaternion.x * quaternion.y) + (quaternion.z * quaternion.w)));
// 	matrix.m[2]  =         (2.0f * ((quaternion.x * quaternion.z) - (quaternion.y * quaternion.w)));
// 	matrix.m[3]  = 0.0f;
// 	matrix.m[4]  =         (2.0f * ((quaternion.x * quaternion.y) - (quaternion.z * quaternion.w)));
// 	matrix.m[5]  = (1.0f - (2.0f * ((quaternion.x * quaternion.x) + (quaternion.z * quaternion.z))));
// 	matrix.m[6]  =         (2.0f * ((quaternion.y * quaternion.z) + (quaternion.x * quaternion.w)));
// 	matrix.m[7]  = 0.0f;
// 	matrix.m[8]  =         (2.0f * ((quaternion.x * quaternion.z) + (quaternion.y * quaternion.w)));
// 	matrix.m[9]  =         (2.0f * ((quaternion.y * quaternion.z) - (quaternion.x * quaternion.w)));
// 	matrix.m[10] = (1.0f - (2.0f * ((quaternion.x * quaternion.x) + (quaternion.y * quaternion.y))));
// 	matrix.m[11] = 0.0f;
// 	matrix.m[12] = 0.0f;
// 	matrix.m[13] = 0.0f;
// 	matrix.m[14] = 0.0f;
// 	matrix.m[15] = 1.0f;
	
// 	return matrix;
// }

void Quaternion_normalize(Quaternion * quaternion) {
	float magnitude;
	
	magnitude = sqrt((quaternion->x * quaternion->x) +
	                 (quaternion->y * quaternion->y) +
	                 (quaternion->z * quaternion->z) +
	                 (quaternion->w * quaternion->w));
	quaternion->x /= magnitude;
	quaternion->y /= magnitude;
	quaternion->z /= magnitude;
	quaternion->w /= magnitude;
}

Quaternion Quaternion_normalized(Quaternion quaternion) {
	Quaternion_normalize(&quaternion);
	return quaternion;
}

void Quaternion_multiply(Quaternion * quaternion1, Quaternion quaternion2) {
	vec3 vector1, vector2, cross;
	float angle;
	
	vector1 = Quaternion_toVector(*quaternion1);
	vector2 = Quaternion_toVector(quaternion2);
	angle = (quaternion1->w * quaternion2.w) - (vector1.x*vector2.x + vector1.y*vector2.y + vector1.z*vector2.z);

	cross = vec3(vector1.y*vector2.z-vector2.y*vector1.z,vector1.z*vector2.x-vector2.z*vector1.x,vector1.x*vector2.y-vector2.x*vector1.y);

	vector1.x *= quaternion2.w;
	vector1.y *= quaternion2.w;
	vector1.z *= quaternion2.w;
	vector2.x *= quaternion1->w;
	vector2.y *= quaternion1->w;
	vector2.z *= quaternion1->w;
	
	quaternion1->x = vector1.x + vector2.x + cross.x;
	quaternion1->y = vector1.y + vector2.y + cross.y;
	quaternion1->z = vector1.z + vector2.z + cross.z;
	quaternion1->w = angle;
}

Quaternion Quaternion_multiplied(Quaternion quaternion1, Quaternion quaternion2) {
	Quaternion_multiply(&quaternion1, quaternion2);
	return quaternion1;
}

#define SLERP_TO_LERP_SWITCH_THRESHOLD 0.01f

Quaternion Quaternion_slerp(Quaternion start, Quaternion end, float alpha) {
	float startWeight, endWeight, difference;
	Quaternion result;
	
	difference = (start.x * end.x) + (start.y * end.y) + (start.z * end.z) + (start.w * end.w);
	if ((1.0f - fabs(difference)) > SLERP_TO_LERP_SWITCH_THRESHOLD) {
		float theta, oneOverSinTheta;
		
		theta = acos(fabs(difference));
		oneOverSinTheta = 1.0f / sin(theta);
		startWeight = sin(theta * (1.0f - alpha)) * oneOverSinTheta;
		endWeight = sin(theta * alpha) * oneOverSinTheta;
		if (difference < 0.0f) {
			startWeight = -startWeight;
		}
	} else {
		startWeight = 1.0f - alpha;
		endWeight = alpha;
	}
	result.x = (start.x * startWeight) + (end.x * endWeight);
	result.y = (start.y * startWeight) + (end.y * endWeight);
	result.z = (start.z * startWeight) + (end.z * endWeight);
	result.w = (start.w * startWeight) + (end.w * endWeight);
	Quaternion_normalize(&result);
	
	return result;
}

// void Quaternion_rotate(Quaternion * quaternion, vec3 axis, float angle) {
// 	Quaternion rotationQuaternion;
	
// 	rotationQuaternion = Quaternion_fromAxisAngle(axis, angle);
// 	Quaternion_multiply(quaternion, rotationQuaternion);
// }

// Quaternion Quaternion_rotated(Quaternion quaternion, vec3 axis, float angle) {
// 	Quaternion_rotate(&quaternion, axis, angle);
// 	return quaternion;
// }

void Quaternion_invert(Quaternion * quaternion) {
	float length;
	
	length = 1.0f / ((quaternion->x * quaternion->x) +
	                 (quaternion->y * quaternion->y) +
	                 (quaternion->z * quaternion->z) +
	                 (quaternion->w * quaternion->w));
	quaternion->x *= -length;
	quaternion->y *= -length;
	quaternion->z *= -length;
	quaternion->w *= length;
}

Quaternion Quaternion_inverted(Quaternion quaternion) {
	Quaternion_invert(&quaternion);
	return quaternion;
}

vec3 Quaternion_multiplyVector(Quaternion quaternion, vec3 vector) {
	Quaternion vectorQuaternion, inverseQuaternion, result;
	
	vectorQuaternion = Quaternion_fromVector(vector);
	inverseQuaternion = Quaternion_inverted(quaternion);
	result = Quaternion_multiplied(quaternion, Quaternion_multiplied(vectorQuaternion, inverseQuaternion));
	return Quaternion_toVector(result);
}




Quaternion Quaternion_Mul_OrientationAxis(Quaternion AxisAngle, Quaternion m_Orientation) {
	Quaternion Result = Quaternion();
	double W = (m_Orientation.w * AxisAngle.w) - (m_Orientation.x * AxisAngle.x) - (m_Orientation.y * AxisAngle.y) - (m_Orientation.z * AxisAngle.z);
	double X = (m_Orientation.w * AxisAngle.x) + (m_Orientation.x * AxisAngle.w) + (m_Orientation.y * AxisAngle.z) - (m_Orientation.z * AxisAngle.y);
	double Y = (m_Orientation.w * AxisAngle.y) + (m_Orientation.y * AxisAngle.w) + (m_Orientation.z * AxisAngle.x) - (m_Orientation.x * AxisAngle.z);
	double Z = (m_Orientation.w * AxisAngle.z) + (m_Orientation.z * AxisAngle.w) + (m_Orientation.x * AxisAngle.y) - (m_Orientation.y * AxisAngle.x);

	Result.x = X;
	Result.y = Y;
	Result.z = Z;
	Result.w = W;
	return Result;
}