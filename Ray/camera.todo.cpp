#include <cmath>
#include <Util/exceptions.h>
#include "camera.h"
#include "shape.h"
#include <math.h>

using namespace Ray;
using namespace Util;
using namespace std;

////////////
// Camera //
////////////

// ./Assignment2 --in Static3D/_dog.ray --out Static3D/aaa.jpg


// Generate Util::Ray3Ds from the camera's position through (i,j)-th pixel of a widthxheight view plane.
Ray3D Camera::getRay( int i , int j , int width , int height ) const
{
	/////////////////////////////////////////////////
	// Get the ray through the (i,j)-th pixel here //
	/////////////////////////////////////////////////

	// WARN_ONCE( "method undefined" );
	// return Ray3D();

	// make ray
	Ray3D ray;
	ray.position = position;
	double widthAngle = ((double) width / height) * heightAngle;
	
	// do math
	double d = 1;
	double ar = height / width;
	Point3D p1_vert = position + d * forward - d * tan(heightAngle / 2.0) * up;
	Point3D p2_vert = position + d * forward + d * tan(heightAngle / 2.0) * up;
	Point3D pi_vert = p1_vert + (p2_vert - p1_vert) * ((j + 0.5) / height);
	Point3D p_ud = pi_vert - position;

	Point3D p1_hor = position + d * forward - d * tan(widthAngle / 2.0) * right;
	Point3D p2_hor = position + d * forward + d * tan(widthAngle / 2.0) * right;
	Point3D pi_hor = p1_hor + (p2_hor - p1_hor) * ((i + 0.5) / width);
	Point3D p_lr = pi_hor - position;

	Point3D p_ud_up = p_ud.dot(up) * up;
	Point3D dir = p_lr + p_ud_up;
	dir /= dir.length();

	ray.direction = dir;

	// if (i == width / 2 && j == height / 2) {
	// 	cout << "ray in camera.todo " << dir << endl;
	// }

	return ray;
}

/* call openGL commands for setting up a camera */
void Camera::drawOpenGL( void ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////

	glMatrixMode( GL_MODELVIEW );        
	glLoadIdentity();
	gluLookAt( position[0] , position[1] , position[2] , position[0]+forward[0] , position[1]+forward[1] , position[2]+forward[2] , up[0] , up[1] , up[2] );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}

void Camera::rotateUp( Point3D center , float angle )
{
	///////////////////////////////////////////////////
	// Rotate the camera about the up direction here //
	///////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
}

void Camera::rotateRight( Point3D center , float angle )
{
	//////////////////////////////////////////////////////
	// Rotate the camera about the right direction here //
	//////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
}

void Camera::moveForward( float dist )
{
	//////////////////////////////////
	// Move the camera forward here //
	//////////////////////////////////
	WARN_ONCE( "method undefined" );
}

void Camera::moveRight( float dist )
{
	///////////////////////////////////////
	// Move the camera to the right here //
	///////////////////////////////////////
	WARN_ONCE( "method undefined" );
}

void Camera::moveUp( float dist )
{
	/////////////////////////////
	// Move the camera up here //
	/////////////////////////////
	WARN_ONCE( "method undefined" );
}
