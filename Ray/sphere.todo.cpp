#include <cmath>
#include <Util/exceptions.h>
#include "scene.h"
#include "sphere.h"

using namespace Ray;
using namespace Util;
using namespace std;

/* camera.h 
namespace Ray
{
	class Camera
	{
	public:
		double heightAngle;

		Util::Point3D position;

		Util::Point3D forward;

		Util::Point3D up;

		Util::Point3D right;

		void drawOpenGL( void ) const;

		void rotateUp( Util::Point3D center , float angle );

		void rotateRight( Util::Point3D center , float angle );

		void moveForward( float dist );

		void moveRight( float dist );

		void moveUp( float dist );

		Util::Ray3D getRay( int i , int j , int width , int height ) const;
	};

	std::ostream &operator << ( std::ostream &stream , const Camera &camera );

	std::istream &operator >> ( std::istream &stream ,       Camera &camera );
}
#endif // CAMERA_INCLUDED
*/



 /* shapeList.h
 
 class ShapeList : public Shape
	{
		friend class Union;
		friend class Intersection;

		static std::string _DirectiveHeader( void ){ return "shape_list"; }
	public:

		static std::unordered_map< std::string , Util::BaseFactory< Shape > * > ShapeFactories;

		static std::string Directive( void ){ return _DirectiveHeader() + std::string( "_begin" ); }

		std::vector< Shape* > shapes;

		///////////////////
		// Shape methods //
		///////////////////
	protected:
		void _write( std::ostream &stream ) const;
		void _read( std::istream &stream );
	public:
		std::string name( void ) const { return "shape list"; }
		void init( const class LocalSceneData &data );
		void initOpenGL( void );
		void updateBoundingBox( void );
		bool processFirstIntersection( const Util::Ray3D &ray , const Util::BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const;
		int processAllIntersections( const Util::Ray3D &ray , const Util::BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const;
		void processOverlapping( const Filter &filter , const Kernel &kernel , ShapeProcessingInfo tInfo ) const;
		bool isInside( Util::Point3D p ) const;
		void drawOpenGL( GLSLProgram * glslProgram ) const;
		void addTrianglesOpenGL( std::vector< class TriangleIndex >& triangles );
	};
	*/

////////////
// Sphere //
////////////

void Sphere::init( const LocalSceneData &data )
{
	// Set the material pointer
	if( _materialIndex<0 ) THROW( "negative material index: " , _materialIndex );
	else if( _materialIndex>=data.materials.size() ) THROW( "material index out of bounds: " , _materialIndex , " <= " , data.materials.size() );
	else _material = &data.materials[ _materialIndex ];
	_primitiveNum = 1;

	///////////////////////////////////
	// Do any additional set-up here //
	///////////////////////////////////

	// get equation coefficients of sphere
	// P(x, y, z) = (x - a)^2 + (y - b)^2 + (z - c)^2 - r^2
	// P(x, y, z) = x^2 - 2ax + a^2 + y^2 - 2by + b^2 + z^2 - 2cz + c^2 - r^2
	// P(x, y, z) = x^2 - 2ax + y^2 - 2by + z^2 - 2cz + a^2 + b^2 + c^2 - r^2
	int a = center[0], b = center[1], c = center[2], r = radius;

	poly.coefficient(2u, 0u, 0u) = 1;
	poly.coefficient(1u, 0u, 0u) = -2 * a;
	poly.coefficient(0u, 2u, 0u) = 1;
	poly.coefficient(0u, 1u, 0u) = -2 * b;
	poly.coefficient(0u, 0u, 2u) = 1;
	poly.coefficient(0u, 0u, 1u) = -2 * c;
	poly.coefficient(0u, 0u, 0u) = pow(a, 2) + pow(b, 2) + pow(c, 2) - pow(r, 2);

	updateBoundingBox();
}
void Sphere::updateBoundingBox( void )
{
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	
	Point3D p( radius , radius , radius );
	_bBox = BoundingBox3D( center-p , center+p );
}
void Sphere::initOpenGL( void )
{
	///////////////////////////
	// Do OpenGL set-up here //
	///////////////////////////
	WARN_ONCE( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}

// BoundingBox1D range is a bounding range of time — can't be too low for reflections, don't want to intersect from your starting point.
// RayIntersectionFilter is function: double -> bool
// RayIntersectionKernel is function: const ShapeProcessingInfo & , const RayShapeIntersectionInfo & -> void

// use a polynomial 3D, which you will make the equation for the sphere. You can run poly[ray] to get Polynomial1D intersection, which is the time of intersection I think.
bool Sphere::processFirstIntersection( const Ray3D &ray , const BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const
{
	RayTracingStats::IncrementRayPrimitiveIntersectionNum();
	spInfo.material = _material;

	//////////////////////////////////////////////////////////////
	// Compute the intersection of the sphere with the ray here //
	//////////////////////////////////////////////////////////////

	// get intersection with this sphere & given ray
	Util::Polynomial1D< 2 > intersection = poly(ray);
	double roots[2];
	unsigned int rootNum = intersection.roots(roots);

	// find closest intersection
	double left_bound = range[0][0], right_bound = range[1][0]; // first idx takes point<1> to double
	double closest_time = Infinity;
	for (int i = 0; i < rootNum; i++) {
		if (left_bound <= roots[i] && roots[i] <= right_bound && roots[i] < closest_time && rFilter(roots[i])) {
			closest_time = roots[i];
		}
	}

	if (closest_time == Infinity) return false;

	// make intersection info and invoke rKernel
	RayShapeIntersectionInfo rsii = RayShapeIntersectionInfo();
	rsii.t = closest_time;
	rsii.position = ray(closest_time);
	Point3D normal = rsii.position - center;
	rsii.normal = normal / normal.length();
	rsii.texture = Point2D(0, 0); // TODO: figure out how to get texture coords

	// edit everything in spInfo
	
	rKernel(spInfo, rsii);
	return true;
}

/* processes all shapes which intersect the ray within the prescribed range and passing the rFilter test, invoking the rKernel kernel with the intersection information. The processing terminates early if the kernel returns false. The function returns the number of valid intersections.*/
int Sphere::processAllIntersections( const Ray3D &ray , const BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const
{
	RayTracingStats::IncrementRayPrimitiveIntersectionNum();
	spInfo.material = _material;

	//////////////////////////////////////////////////////////////
	// Compute the intersection of the sphere with the ray here //
	//////////////////////////////////////////////////////////////

	// get intersection with this sphere & given ray
	Util::Polynomial1D< 2 > intersection = poly(ray);
	double roots[2];
	unsigned int rootNum = intersection.roots(roots);

	// find ALL intersections
	int cnt = 0;
	double left_bound = range[0][0], right_bound = range[1][0]; // first idx takes point<1> to double
	for (int i = 0; i < rootNum; i++) {
		if (left_bound <= roots[i] && roots[i] <= right_bound && rFilter(roots[i])) {
			// make intersection info and invoke rKernel
			RayShapeIntersectionInfo rsii = RayShapeIntersectionInfo();
			rsii.t = roots[i];
			rsii.position = ray(roots[i]);
			Point3D normal = rsii.position - center;
			rsii.normal = normal / normal.length();
			rsii.texture = Point2D(0, 0); // TODO: figure out how to get texture coords
			rKernel(spInfo, rsii);
		}
	}

	return cnt;
}

bool Sphere::isInside( Point3D p ) const
{
	//////////////////////////////////////////////////////
	// Determine if the point is inside the sphere here //
	//////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return false;
}

/* void GLSLProgram::setUniform( const std::string &name , const double* v , bool showWarning ) */

/* calls the necessary OpenGL commands to render the primitive */
void Sphere::drawOpenGL(GLSLProgram* glslProgram) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	
	_material->drawOpenGL( glslProgram );
	glPushMatrix();
	glTranslatef( center[0] , center[1] , center[2] );
	GLUquadric *q = gluNewQuadric();
	gluSphere( q , radius , 2*Shape::OpenGLTessellationComplexity , Shape::OpenGLTessellationComplexity );
	gluDeleteQuadric( q );
	glPopMatrix();

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}
