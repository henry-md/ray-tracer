/*
Copyright (c) 2019, Michael Kazhdan
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer. Redistributions in binary form must reproduce
the above copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the distribution. 

Neither the name of the Johns Hopkins University nor the names of its contributors
may be used to endorse or promote products derived from this software without specific
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.
*/
#include <cmath>
#include <SVD/SVDFit.h>
#include <SVD/MatrixMNTC.h>
#include <Util/exceptions.h>
#include "geometry.h"

namespace Util
{
	////////////////////////////
	// EulerRotationParameter //
	////////////////////////////
	// transforms the triplet of Euler angles into a rotation matrix
	Matrix3D EulerRotationParameter::operator() ( void ) const
	{
		///////////////////////////////////////////////
		// Transform Euler angles to a rotation here //
		///////////////////////////////////////////////

		Point3D angles = parameter;
		double c1 = cos(angles[0]);
		double s1 = sin(angles[0]);
		double c2 = cos(angles[1]);
		double s2 = sin(angles[1]);
		double c3 = cos(angles[2]);
		double s3 = sin(angles[2]);

		Matrix3D rotationMatrixX;
		rotationMatrixX(0, 0) = 1;
		rotationMatrixX(1, 1) = c1;
		rotationMatrixX(1, 2) = -s1;
		rotationMatrixX(2, 1) = s1;
		rotationMatrixX(2, 2) = c1;

		Matrix3D rotationMatrixY;
		rotationMatrixY(0, 0) = c2;
		rotationMatrixY(0, 2) = s2;
		rotationMatrixY(1, 1) = 1;
		rotationMatrixY(2, 0) = -s2;
		rotationMatrixY(2, 2) = c2;

		Matrix3D rotationMatrixZ;
		rotationMatrixZ(0, 0) = c3;
		rotationMatrixZ(0, 1) = -s3;
		rotationMatrixZ(1, 0) = s3;
		rotationMatrixZ(1, 1) = c3;
		rotationMatrixZ(2, 2) = 1;

		Matrix3D rotationMatrix = rotationMatrixZ * rotationMatrixY * rotationMatrixX;
		return rotationMatrix;
	}

	/////////////////////////////////
	// QuaternionRotationParameter //
	/////////////////////////////////
	/* generates a 3x3 matrix corresponding to a quaternion. (Keep in mind that the matrix is only a rotation if the quaternion is a unit quaternion.) */
	Matrix3D QuaternionRotationParameter::operator()( void ) const
	{
		///////////////////////////////////////////////
		// Transform a quaternion to a rotation here //
		///////////////////////////////////////////////
		
		// get quaternion values
		Quaternion q = parameter;
		double a = q.real;
		double b = q.imag[0] / q.length();
		double c = q.imag[1] / q.length();
		double d = q.imag[2] / q.length();

		// precompute crossterms
		double aa = a * a;
		double bb = b * b;
		double cc = c * c;
		double dd = d * d;
		double ab = a * b;
		double ac = a * c;
		double ad = a * d;
		double bc = b * c;
		double bd = b * d;
		double cd = c * d;

		// do conversion
		Matrix3D rotationMatrix;
		rotationMatrix(0, 0) = aa + bb - cc - dd;
		rotationMatrix(0, 1) = 2 * (bc - ad);
		rotationMatrix(0, 2) = 2 * (ac + bd);
		rotationMatrix(1, 0) = 2 * (ad + bc);
		rotationMatrix(1, 1) = aa - bb + cc - dd;
		rotationMatrix(1, 2) = 2 * (cd - ab);
		rotationMatrix(2, 0) = 2 * (bd - ac);
		rotationMatrix(2, 1) = 2 * (ab + cd);
		rotationMatrix(2, 2) = aa - bb - cc + dd;
		return rotationMatrix;
	}
}
