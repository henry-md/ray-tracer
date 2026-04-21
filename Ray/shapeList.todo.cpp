#include <Util/exceptions.h>
#include <Util/threads.h>
#include "shapeList.h"
#include "triangle.h"

using namespace Ray;
using namespace Util;
using namespace std;

////////////////
// Difference //
////////////////
void Difference::updateBoundingBox( void )
{
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	
	_shape0->updateBoundingBox();
	_bBox = _shape0->boundingBox();
}

bool Difference::processFirstIntersection( const Ray3D &ray , const BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const
{
	//////////////////////////////////////////////////////////////////
	// Compute the intersection of the difference with the ray here //
	//////////////////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return false;
}

int Difference::processAllIntersections( const Ray3D &ray , const BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const
{
	//////////////////////////////////////////////////////////////////
	// Compute the intersection of the difference with the ray here //
	//////////////////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return 0;
}
bool Difference::isInside( Util::Point3D p ) const
{
	//////////////////////////////////////////////////////////
	// Determine if the point is inside the difference here //
	//////////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return false;
}

///////////////
// ShapeList //
///////////////
bool ShapeList::processFirstIntersection( const Ray3D &ray , const BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const
{
	//////////////////////////////////////////////////////////////////
	// Compute the intersection of the shape list with the ray here //
	//////////////////////////////////////////////////////////////////
	// WARN_ONCE( "method undefined" );

	double left_bound = range[0][0], right_bound = range[1][0];
	RayShapeIntersectionInfo iInfo = RayShapeIntersectionInfo();
	bool hit = false;

	RayIntersectionKernel k = [&]( const ShapeProcessingInfo& _spInfo , const class RayShapeIntersectionInfo& _iInfo  ) {
		if (_iInfo.t < iInfo.t) {
			iInfo = _iInfo;
			spInfo = _spInfo;
			hit = true;
			return true;
		}
		return false;
	};

	for (const Shape *s : shapes) {
		s->processFirstIntersection(ray, range, rFilter, k, spInfo, tIdx);
	}

	if (hit) rKernel(spInfo, iInfo);
	return hit;
}

int ShapeList::processAllIntersections( const Ray3D &ray , const BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const
{
	int count = 0;
	for( unsigned int i=0 ; i<shapes.size() ; i++ ) count += shapes[i]->processAllIntersections( ray , range , rFilter , rKernel , spInfo , tIdx );
	return count;
}
bool ShapeList::isInside( Point3D p ) const
{
	//////////////////////////////////////////////////////////
	// Determine if the point is inside the shape list here //
	//////////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return false;
}

void ShapeList::init( const LocalSceneData &data )
{
	// Initialize the children
	for( int i=0 ; i<shapes.size() ; i++ ) shapes[i]->init( data );
	_primitiveNum = 0;
	for( int i=0 ; i<shapes.size() ; i++ ) _primitiveNum += shapes[i]->primitiveNum();

	///////////////////////////////////
	// Do any additional set-up here //
	///////////////////////////////////
	// WARN_ONCE( "method undefined" );

	// initialize bounding box
	// _bBox = BoundingBox3D();
	// for (const Shape *s : shapes) {
	// 	_bBox += s->boundingBox();
	// }



}

void ShapeList::updateBoundingBox( void )
{
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	Point3D *pList = new Point3D[shapes.size()*2];
	for( int i=0 ; i<shapes.size() ; i++ )
	{
		shapes[i]->updateBoundingBox();
		_bBox = shapes[i]->boundingBox();
		pList[2*i  ] = _bBox[0];
		pList[2*i+1] = _bBox[1];
	}
	_bBox = BoundingBox3D( pList , (int)shapes.size()*2 );
	delete[] pList;
}

void ShapeList::initOpenGL( void )
{
	// Initialize the children
	for( int i=0 ; i<shapes.size() ; i++ ) shapes[i]->initOpenGL();

	///////////////////////////
	// Do OpenGL set-up here //
	///////////////////////////
	WARN_ONCE( "remainder of method undefined" );
}

/* Calls the necessary OpenGL commands to render the primitive */
void ShapeList::drawOpenGL( GLSLProgram * glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	
	// Render each shape in the shape list
	for (const Shape* shape : shapes) {
		shape->drawOpenGL(glslProgram);
	}

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}

/////////////////
// AffineShape //
/////////////////

bool AffineShape::isInside( Point3D p ) const
{
	///////////////////////////////////////////////////////////////////////
	// Determine if the point is inside the affinely deformed shape here //
	///////////////////////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return _shape->isInside( p );
}

void AffineShape::updateBoundingBox( void )
{
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	_shape->updateBoundingBox();
	_bBox = getMatrix() * _shape->boundingBox();
}

void AffineShape::drawOpenGL( GLSLProgram * glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	
	Matrix4D matrix = getMatrix().transpose();
	glPushMatrix();
	glMultMatrixd( &matrix(0,0) );
	_shape->drawOpenGL( glslProgram );
	glPopMatrix();

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}

///////////////////////
// StaticAffineShape //
///////////////////////

/* Initialize the local-to-global position transform Ray::StaticAffineShape::_inverseTransform and the local-to-global normal transform */
void StaticAffineShape::init( const LocalSceneData &data )
{
	////////////////////////////////////////
	// Do/replace the initialization here //
	////////////////////////////////////////
	// _inverseTransform = Util::Matrix4D::Identity();
	// _normalTransform = Util::Matrix3D::Identity();
	// WARN_ONCE( "method undefined" );

	_inverseTransform = _localTransform.inverse();
	_normalTransform = _localTransform.transpose().inverse();

	// what to do here
	// cout << "inverse transform: " << _inverseTransform << endl;


	_shape->init( data );
	_primitiveNum = _shape->primitiveNum();
}

//////////////////
// TriangleList //
//////////////////

/* calls the necessary OpenGL commands to render the primitive */
void TriangleList::drawOpenGL(GLSLProgram *glslProgram) const
{
	// Do OpenGL rendering here

	for (const Shape* shape : shapes) {
		shape->drawOpenGL(glslProgram);
	}

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}

void TriangleList::init( const LocalSceneData &data )
{
	// Set the vertex and material pointers
	_vertices = &data.vertices[0];
	_vNum = (unsigned int)data.vertices.size();
	if( _materialIndex>=data.materials.size() ) THROW( "shape specifies a material that is out of bounds: " , _materialIndex , " <= " , data.materials.size() );
	else if( _materialIndex<0 ) THROW( "negative material index: " , _materialIndex );
	else _material = &data.materials[ _materialIndex ];

	ShapeList::init( data );

	///////////////////////////////////
	// Do any additional set-up here //
	///////////////////////////////////
	WARN_ONCE( "method undefined" );
}

void TriangleList::initOpenGL( void )
{
	ShapeList::initOpenGL();

	///////////////////////////
	// Do OpenGL set-up here //
	///////////////////////////
	WARN_ONCE( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}

///////////
// Union //
///////////
bool Union::processFirstIntersection( const Ray3D &ray , const BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const
{
	/////////////////////////////////////////////////////////////
	// Compute the intersection of the union with the ray here //
	/////////////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return false;
}

int Union::processAllIntersections( const Ray3D &ray , const BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const
{
	/////////////////////////////////////////////////////////////
	// Compute the intersection of the union with the ray here //
	/////////////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return 0;
}
void Union::init( const LocalSceneData &data )
{
	_shapeList.init( data );
	_primitiveNum = _shapeList.primitiveNum();

	///////////////////////////////////
	// Do any additional set-up here //
	///////////////////////////////////
	WARN_ONCE( "method undefined" );
}

void Union::updateBoundingBox( void )
{
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	WARN_ONCE( "method undefined" );
}

bool Union::isInside( Point3D p ) const
{
	/////////////////////////////////////////////////////
	// Determine if the point is inside the union here //
	/////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return false;
}

//////////////////
// Intersection //
//////////////////
bool Intersection::processFirstIntersection( const Ray3D &ray , const BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const
{
	/////////////////////////////////////////////////////////////////////////////////////
	// Compute the intersection of the difference with the intersection of shapes here //
	/////////////////////////////////////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return false;
}

int Intersection::processAllIntersections( const Ray3D &ray , const BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const
{
	/////////////////////////////////////////////////////////////////////////////////////
	// Compute the intersection of the difference with the intersection of shapes here //
	/////////////////////////////////////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return 0;
}
void Intersection::init( const LocalSceneData &data )
{
	_shapeList.init( data );
	_primitiveNum = _shapeList.primitiveNum();

	///////////////////////////////////
	// Do any additional set-up here //
	///////////////////////////////////
	WARN_ONCE( "method undefined" );
}

void Intersection::updateBoundingBox( void )
{
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	WARN_ONCE( "method undefined" );
}

bool Intersection::isInside( Point3D p ) const
{
	///////////////////////////////////////////////////////////////////////
	// Determine if the point is inside the instersection of shapes here //
	///////////////////////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return false;
}