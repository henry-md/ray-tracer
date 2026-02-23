#include <cmath>
#include <Util/exceptions.h>
#include "triangle.h"

using namespace Ray;
using namespace Util;
using namespace std;

//////////////
// Triangle //
//////////////


Point3D cross(Point3D a, Point3D b) {
	double i = a[1] * b[2] - a[2] * b[1];
	double j = -1 * (a[0] * b[2] - a[2] * b[0]);
	double k = a[0] * b[1] - a[1] * b[0];
	return Point3D(i, j, k);
}

void Triangle::init( const LocalSceneData &data )
{
	_primitiveNum = 1;
	// Set the vertex pointers
	for( int i=0 ; i<3 ; i++ )
	{
		if( _vIndices[i]==-1 ) THROW( "negative vertex index:" , _vIndices[i] );
		else if( _vIndices[i]>=data.vertices.size() ) THROW( "vertex index out of bounds: " , _vIndices[i] , " <= " , data.vertices.size() );
		else _v[i] = &data.vertices[ _vIndices[i] ];
	}

	///////////////////////////////////
	// Do any additional set-up here //
	///////////////////////////////////
	WARN_ONCE( "method undefined" );

	// get normal
	Point3D p0 = _v[0]->position;
	Point3D p1 = _v[1]->position;
	Point3D p2 = _v[2]->position;
	Point3D v1 = p1 - p0;
	Point3D v2 = p2 - p0;
	n = cross(v1, v2);
	n /= n.length();

	// // compute center
	// p = (p0 + p1 + p2) / 3.0;
}

void Triangle::updateBoundingBox( void )
{
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	
	Point3D pList[3];
	for( int i=0 ; i<3 ; i++ ) pList[i] = _v[i]->position;
	_bBox = BoundingBox3D( pList , 3 );
	for( int i=0 ; i<3 ; i++ ) _bBox[0][i] -= Epsilon , _bBox[1][i] += Epsilon;
}

void Triangle::initOpenGL( void )
{
	///////////////////////////
	// Do OpenGL set-up here //
	///////////////////////////
	WARN_ONCE( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}

bool Triangle::processFirstIntersection( const Util::Ray3D &ray , const Util::BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const
{
	RayTracingStats::IncrementRayPrimitiveIntersectionNum();

	/////////////////////////////////////////////////////////////
	// Compute the intersection of the shape with the ray here //
	/////////////////////////////////////////////////////////////

	// compute time to intersection
	Point3D p0 = _v[0]->position;
	Point3D v0 = ray.position;
	Point3D v = ray.direction;
	double t = n.dot(p0 - v0) / (n.dot(v)); // derived in notes

	// check if time of intersectin is valid
	if (t < range[0][0] || t > range[1][0]) return false;

	// check alpha, beta, gamma Barycentric coordinates
	Point3D p = ray(t);
	Point3D v1 = _v[0]->position;
	Point3D v2 = _v[1]->position;
	Point3D v3 = _v[2]->position;
	double alpha = (cross(v2 - p, v3 - p).dot(n) / 2.0) / (cross(v2 - v1, v3 - v1).dot(n) / 2.0);
	double beta = (cross(v3 - p, v1 - p).dot(n) / 2.0) / (cross(v3 - v2, v1 - v2).dot(n) / 2.0);
	double gamma = (cross(v1 - p, v2 - p).dot(n) / 2.0) / (cross(v1 - v3, v2 - v3).dot(n) / 2.0);

	// check if barycentric coordinates are positive
	if (alpha < 0 || beta < 0 || gamma < 0) return false;

	// make intersection info and invoke rKernel
	RayShapeIntersectionInfo rsii = RayShapeIntersectionInfo();
	rsii.t = t;
	rsii.position = p;
	rsii.normal = n;

	// texture stuff
	Point2D texAlpha = _v[0]->texCoordinate * alpha;
	Point2D texBeta = _v[1]->texCoordinate * beta;
	Point2D texGamma = _v[2]->texCoordinate * gamma;
	rsii.texture = (texAlpha + texBeta + texGamma);
	rKernel(spInfo, rsii);
	return true;
}

/* calls the necessary OpenGL commands to render the primitive. */
void Triangle::drawOpenGL( GLSLProgram * glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	
	// Enable vertex and normal arrays
	// glEnableClientState(GL_VERTEX_ARRAY);
	// glEnableClientState(GL_NORMAL_ARRAY);
	
	// // Set vertex and normal data
	// glVertexPointer(3, GL_FLOAT, 0, _vertices);
	// glNormalPointer(GL_FLOAT, 0, _normals);
	
	// // Draw the triangle
	// glDrawArrays(GL_TRIANGLES, 0, 3);
	
	// // Disable vertex and normal arrays
	// glDisableClientState(GL_VERTEX_ARRAY);
	// glDisableClientState(GL_NORMAL_ARRAY);

	WARN_ONCE( "method undefined" );
	
	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}
