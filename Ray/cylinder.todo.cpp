#include <cmath>
#include <Util/exceptions.h>
#include "scene.h"
#include "cylinder.h"

using namespace Ray;
using namespace Util;

//////////////
// Cylinder //
//////////////

void Cylinder::init( const LocalSceneData &data )
{
	// Set the material pointer
	if( _materialIndex<0 ) THROW( "negative material index: " , _materialIndex );
	else if( _materialIndex>=data.materials.size() ) THROW( "material index out of bounds: " , _materialIndex , " <= " , data.materials.size() );
	else _material = &data.materials[ _materialIndex ];
	_primitiveNum = 1;

	//////////////////////////////////
	// Do any necessary set-up here //
	//////////////////////////////////
	WARN_ONCE( "method undefined" );
}

void Cylinder::updateBoundingBox( void )
{
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	
	Point3D p( radius , height/2 , radius );
	_bBox = BoundingBox3D( center-p , center+p );
}

void Cylinder::initOpenGL( void )
{
	/////////////////////////////////////////
	// Do any necessary OpenGL set-up here //
	/////////////////////////////////////////
	WARN_ONCE( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}

bool Cylinder::processFirstIntersection( const Ray3D &ray , const BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const
{
	RayTracingStats::IncrementRayPrimitiveIntersectionNum();
	spInfo.material = _material;

	/////////////////////////////////////////////////////////////
	// Compute the intersection of the shape with the ray here //
	/////////////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return false;
}

int Cylinder::processAllIntersections( const Ray3D &ray , const BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const
{
	RayTracingStats::IncrementRayPrimitiveIntersectionNum();
	spInfo.material = _material;

	/////////////////////////////////////////////////////////////
	// Compute the intersection of the shape with the ray here //
	/////////////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return 0;
}
bool Cylinder::isInside( Point3D p ) const
{
	////////////////////////////////////////////////////////
	// Determine if the point is inside the cylinder here //
	////////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return false;
}

void Cylinder::drawOpenGL( GLSLProgram *glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////

	_material->drawOpenGL( glslProgram );

	GLUquadric *q = gluNewQuadric();

	glPushMatrix();

	glTranslatef( center[0] , center[1] - height/2 , center[2] );
	glRotatef( 90 , -1 , 0 , 0 );
	gluCylinder( q , radius , radius , height , Shape::OpenGLTessellationComplexity , Shape::OpenGLTessellationComplexity );

	glPushMatrix();
	glTranslatef( 0 , 0 , height );
	gluDisk( q , 0 , radius , Shape::OpenGLTessellationComplexity , Shape::OpenGLTessellationComplexity );
	glPopMatrix();

	glRotatef( 180 , 1 , 0 , 0 ); // Normals pointing out
	gluDisk( q , 0 , radius , Shape::OpenGLTessellationComplexity , Shape::OpenGLTessellationComplexity );

	glPopMatrix();

	gluDeleteQuadric( q );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}
