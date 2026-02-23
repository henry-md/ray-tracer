#include <cmath>
#include <Util/exceptions.h>
#include "scene.h"

using namespace Ray;
using namespace Util;
using namespace std;
using namespace Image;

///////////
// Scene //
///////////
Point3D Scene::Reflect( Point3D v , Point3D n )
{
	//////////////////
	// Reflect here //
	//////////////////

	return 2 * abs(v.dot(n)) * n + v;
}

bool Scene::Refract( Point3D v , Point3D n , double ir , Point3D& refract )
{
	double cos_theta_i = v.dot(n);
	double eta_i = 1.0;
	double eta_r = ir;
	Point3D N = n;

	// check inside/outside direction
	if (cos_theta_i < 0) {
		// outside -> inside
		cos_theta_i = -cos_theta_i;
	} else {
		// inside -> outside
		swap(eta_i, eta_r);
		N = -n; // point normal to the inside
	}

	// if no real values of negative sqrt => total internal reflection
	double eta = eta_i / eta_r;
	double k = 1 - eta * eta * (1 - cos_theta_i * cos_theta_i);
	if (k < 0) return false;

	// compute refracted ray
	refract = eta * v + (eta * cos_theta_i - sqrt(k)) * N;
	return true;
}

// spInfo attributes
// Util::Matrix4D localToGlobal , globalToLocal;
// Util::Matrix3D directionGlobalToLocal , normalLocalToGlobal;

/* modify the computation kernel getColor to support texture mapping (with bilinear interpolation of texture samples). That is, modulate the contribution of the emissive, ambient, diffuse, and specular terms by multiplying by the color sampled from the texture map */

// called directly by the scene, which calls processFirstIntersection's
Point3D Scene::getColor( Ray3D ray , int rDepth , Point3D cLimit , unsigned int lightSamples , unsigned int tIdx )
{
	Point3D color;
	RayTracingStats::IncrementRayNum();
	ShapeProcessingInfo spInfo;
	RayIntersectionFilter rFilter = []( double ){ return true; };

	if (rDepth == 0) return color;

	// sets color of intersected ray
	RayIntersectionKernel rKernel = [&]( const ShapeProcessingInfo &_spInfo , const RayShapeIntersectionInfo &_iInfo ) {
		/////////////////////////////////////////////////////////
		// Create the computational kernel that gets the color //
		/////////////////////////////////////////////////////////

		// transform ray back to global coordinates, M and M^T^-1
		RayShapeIntersectionInfo _iInfo2 = _iInfo;
		_iInfo2.position = _spInfo.localToGlobal * _iInfo.position;
		_iInfo2.normal = _spInfo.normalLocalToGlobal * _iInfo.normal;
		_iInfo2.normal /= _iInfo2.normal.length();

		// const Texture* objTexture = _spInfo.material->tex;
		// Point2D textureCoords = _iInfo.texture;
		// Pixel32 textureColor;
		// textureColor = objTexture->_image(textureCoords[0], textureCoords[1]);
		Point3D textureColor = Point3D(1, 1, 1);
		if (_spInfo.material->tex != NULL) {
			double texCoordX = _iInfo.texture[0] * (_spInfo.material->tex->_image.width() - 1);
			double texCoordY = _iInfo.texture[1] * (_spInfo.material->tex->_image.height() - 1);
			Point2D texCoord = Point2D(texCoordX, texCoordY);
			Pixel32 texColor = _spInfo.material->tex->_image.bilinearSample(texCoord);
			textureColor = Point3D(texColor.r, texColor.g, texColor.b) / 255.0;
		}
		

		color += _spInfo.material->emissive * textureColor;

		// loop over light sources in scene
		std::vector< Light * > lights = _globalData.lights;
		for (int i = 0; i < lights.size(); i++) {
			Light *light = lights[i];
			color += light->getAmbient(ray, _iInfo2, *_spInfo.material) * textureColor;
			Point3D transparency = light->transparency(_iInfo2, *this, Point3D(1, 1, 1), lightSamples, tIdx);
			Point3D diffuse = light->getDiffuse(ray, _iInfo2, *_spInfo.material) * textureColor;
			if (diffuse[0] < 0 || diffuse[1] < 0 || diffuse[2] < 0) {
				// cout << "diffuse: " << diffuse << endl;
				continue;
			}
			color += diffuse * transparency;
			// if (rand() % 1000 == 0) {
			// 	cout << "diffuse component I got is " << diffuse << endl;
			// 	cout << "transparency is " << transparency;
			// }
			color += light->getSpecular(ray, _iInfo2, *_spInfo.material) * transparency * textureColor;
		}

		// reflection
		if (ray.direction.dot(_iInfo2.normal) < 0) {
			Ray3D reflectedRay = Ray3D(_iInfo2.position, Reflect(ray.direction, _iInfo2.normal));
			color += getColor(reflectedRay, rDepth - 1, cLimit, lightSamples, tIdx) * _spInfo.material->specular * textureColor;
		}

		// refraction
		Point3D refracted = ray.direction;
		bool valid = Refract(ray.direction, _iInfo2.normal, _spInfo.material->ir, refracted);
		if (valid) color += getColor(Ray3D(_iInfo2.position, refracted), rDepth - 1, cLimit, lightSamples, tIdx) * _spInfo.material->transparent * textureColor;

		return true;
	};

	// to go local coordinates, M^-1
	ray.position = spInfo.globalToLocal * ray.position;
	ray.direction = spInfo.directionGlobalToLocal * ray.direction;

	processFirstIntersection( ray , BoundingBox1D( Epsilon , Infinity ) , rFilter , rKernel , spInfo , tIdx );

}

//////////////
// Material //
//////////////
void Material::drawOpenGL( GLSLProgram * glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	WARN_ONCE( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}

/////////////
// Texture //
/////////////
void Texture::initOpenGL( void )
{
	///////////////////////////////////
	// Do OpenGL texture set-up here //
	///////////////////////////////////
	WARN_ONCE( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}