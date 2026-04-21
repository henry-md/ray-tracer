#include <cmath>
#include <Util/exceptions.h>
#include "pointLight.h"
#include "scene.h"

using namespace Ray;
using namespace Util;
using namespace std;

////////////////
// PointLight //
////////////////

Point3D PointLight::attenuate( Point3D I, double d) const {
	return I / (_constAtten + _linearAtten * d + _quadAtten * d * d);
}

Point3D PointLight::getAmbient( Ray3D ray , const RayShapeIntersectionInfo & iInfo , const Material &material ) const
{
	////////////////////////////////////////////////////
	// Get the ambient contribution of the light here //
	////////////////////////////////////////////////////

	return _ambient * material.ambient;
}

Point3D PointLight::getDiffuse( Ray3D ray , const RayShapeIntersectionInfo &iInfo , const Material &material ) const
{
	////////////////////////////////////////////////////
	// Get the diffuse contribution of the light here //
	////////////////////////////////////////////////////

	Point3D N = iInfo.normal;
	Point3D L = (_location - iInfo.position).unit();
	Point3D K_d = material.diffuse;
	Point3D I = attenuate(_diffuse, (_location - iInfo.position).length());
	return K_d * (N.dot(L)) * I;
}

Point3D PointLight::getSpecular( Ray3D ray , const RayShapeIntersectionInfo &iInfo , const Material &material ) const
{
	/////////////////////////////////////////////////////
	// Get the specular contribution of the light here //
	/////////////////////////////////////////////////////
	
	Point3D direction = (iInfo.position - _location).unit();
	Point3D V = -1 * ray.direction;
	Point3D R = 2 * abs(iInfo.normal.dot(direction)) * (iInfo.normal) + direction;
	Point3D K_s = material.specular;
	Point3D I = attenuate(_specular, (_location - iInfo.position).length());
	double n = material.specularFallOff;
	return K_s * pow(V.dot(R), n) * I;

}

bool PointLight::isInShadow( const RayShapeIntersectionInfo& iInfo , const Shape &shape , unsigned int tIdx ) const
{
	//////////////////////////////////////////////
	// Determine if the light is in shadow here //
	//////////////////////////////////////////////

	Point3D direction = _location - iInfo.position;
	Ray3D shadowRay = Ray3D(iInfo.position, -direction);
	Shape *shapePtr = (Shape *) &shape;
	RayShapeIntersectionInfo iInfo2;
	const BoundingBox1D range = BoundingBox1D(1e-10, 1000000);
	const AffineShape::RayIntersectionFilter rFilter = []( double ){ return true; };
	const AffineShape::RayIntersectionKernel rKernel = [&]( const AffineShape::ShapeProcessingInfo &spInfo , const RayShapeIntersectionInfo &_iInfo ) { return true; };
	const AffineShape::ShapeProcessingInfo spInfo = AffineShape::ShapeProcessingInfo();
	bool intersect = shapePtr->processFirstIntersection(shadowRay, range, rFilter, rKernel, spInfo, tIdx);
	return intersect;
}

Point3D PointLight::transparency( const RayShapeIntersectionInfo &iInfo , const Shape &shape , Point3D cLimit , unsigned int samples , unsigned int tIdx ) const
{
	//////////////////////////////////////////////////////////
	// Compute the transparency along the path to the light //
	//////////////////////////////////////////////////////////

	// Compute the transparency along the path to the light
	Point3D direction = _location - iInfo.position;
	Point3D transparency = Point3D(1.0, 1.0, 1.0);
	Ray3D shadowRay(iInfo.position, -direction);
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

void PointLight::drawOpenGL( int index , GLSLProgram * glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	WARN_ONCE( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}