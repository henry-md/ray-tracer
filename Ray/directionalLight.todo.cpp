#include <cmath>
#include <Util/exceptions.h>
#include "directionalLight.h"
#include "scene.h"

using namespace Ray;
using namespace Util;
using namespace std;

//////////////////////
// DirectionalLight //
//////////////////////

Point3D DirectionalLight::getAmbient( Ray3D ray , const RayShapeIntersectionInfo &iInfo , const Material &material ) const
{
	////////////////////////////////////////////////////
	// Get the ambient contribution of the light here //
	////////////////////////////////////////////////////
	// WARN_ONCE( "method undefined" );
	// return Point3D();
	return _ambient * material.ambient;
}

Point3D DirectionalLight::getDiffuse( Ray3D ray , const RayShapeIntersectionInfo &iInfo , const Material &material ) const
{
	////////////////////////////////////////////////////
	// Get the diffuse contribution of the light here //
	////////////////////////////////////////////////////

	// check that ray is pointing into surface
	// if (ray.direction.dot(iInfo.normal) > 0) {
	// 	return Point3D(0, 0, 0);
	// }

	Point3D N = iInfo.normal;
	Point3D L = -1 * _direction;
	Point3D K_d = material.diffuse;
	Point3D I = _diffuse; // intensity of light source after attenuation (should I be attenuating?)
	Point3D ans = K_d * (N.dot(L)) * I;

	if (rand() % 1000 == 0) {
		cout << "----" << endl;
		cout << "K_d " << K_d << endl;
		cout << "dot prod " << (N.dot(L)) << endl;
		cout << "I " << I << endl;
		cout << K_d * (N.dot(L)) * I << endl;
	}
	// return Point3D(0, 1, 0);
	return K_d * (N.dot(L)) * I;
}

Point3D DirectionalLight::getSpecular( Ray3D ray , const RayShapeIntersectionInfo &iInfo , const Material &material ) const
{
	/////////////////////////////////////////////////////
	// Get the specular contribution of the light here //
	/////////////////////////////////////////////////////

	Point3D V = -1 * ray.direction;
	Point3D R = 2 * abs(iInfo.normal.dot(_direction)) * (iInfo.normal) + _direction;
	Point3D K_s = material.specular;
	Point3D I = _specular;
	double n = material.specularFallOff;
	return K_s * pow(V.dot(R), n) * I;
}

bool DirectionalLight::isInShadow( const RayShapeIntersectionInfo& iInfo , const Shape &shape , unsigned int tIdx ) const
{
	//////////////////////////////////////////////
	// Determine if the light is in shadow here //
	//////////////////////////////////////////////

	Ray3D shadowRay = Ray3D(iInfo.position, -_direction);
	Shape *shapePtr = (Shape *) &shape;
	RayShapeIntersectionInfo iInfo2;
	const BoundingBox1D range = BoundingBox1D(1e-10, 1000000);
	const AffineShape::RayIntersectionFilter rFilter = []( double ){ return true; };
	const AffineShape::RayIntersectionKernel rKernel = [&]( const AffineShape::ShapeProcessingInfo &spInfo , const RayShapeIntersectionInfo &_iInfo ) { return true; };
	const AffineShape::ShapeProcessingInfo spInfo = AffineShape::ShapeProcessingInfo();
	bool intersect = shapePtr->processFirstIntersection(shadowRay, range, rFilter, rKernel, spInfo, tIdx);
	return intersect;
}

Point3D DirectionalLight::transparency( const RayShapeIntersectionInfo &iInfo , const Shape &shape , Point3D cLimit , unsigned int samples , unsigned int tIdx ) const
{
	// Compute the transparency along the path to the light
	Point3D transparency = Point3D(1.0, 1.0, 1.0);
	Ray3D shadowRay(iInfo.position, -_direction);
	Shape *shapePtr = (Shape *) &shape;
	const BoundingBox1D range = BoundingBox1D(1e-10, 1000000);
	const AffineShape::RayIntersectionFilter rFilter = []( double ){ return true; };
	const AffineShape::RayIntersectionKernel rKernel = [&]( const AffineShape::ShapeProcessingInfo &spInfo , const RayShapeIntersectionInfo &_iInfo ) -> bool {
		transparency *= spInfo.material->transparent;
		return true;
	};
	const AffineShape::ShapeProcessingInfo spInfo = AffineShape::ShapeProcessingInfo();
	shapePtr->processAllIntersections(shadowRay, range, rFilter, rKernel, spInfo, tIdx);
	return transparency;
}

void DirectionalLight::drawOpenGL( int index , GLSLProgram * glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	WARN_ONCE( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}