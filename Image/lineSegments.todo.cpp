#include "lineSegments.h"
#include <math.h>
#include <Util/exceptions.h>

using namespace Util;
using namespace Image;
using namespace std;

////////////////////////////
// Image processing stuff //
////////////////////////////
double OrientedLineSegment::length( void ) const
{

	OrientedLineSegment ols = *this;
	Point2D p1 = ols.endPoints[0], p2 = ols.endPoints[1];
	double y_dist = p2[1] - p1[1], x_dist = p2[0] - p1[0];
	return sqrt(pow(x_dist, 2) + pow(y_dist, 2));

}
double OrientedLineSegment::distance( Point2D p ) const
{
	OrientedLineSegment ols = *this;
	Point2D start = ols.endPoints[0], end = ols.endPoints[1];

	// dist is || lp x l_v || / ||l_v||
	double x = end[0] - start[0], y = end[1] - start[1];
	double l_v[2] = {x, y};
	double l_v_norm = sqrt(pow(l_v[0], 2) + pow(l_v[1], 2));
	double lp[2] = {p[0] - start[0], p[1] - start[1]};
	double lp_norm = sqrt(pow(lp[0], 2) + pow(lp[1], 2));
	double lp_x_l_v = lp[0] * l_v[1] - lp[1] * l_v[0];

	return abs(lp_x_l_v / l_v_norm);

}
Point2D OrientedLineSegment::perpendicular( void ) const
{
	OrientedLineSegment ols = *this;
	Point2D start = ols.endPoints[0], end = ols.endPoints[1];
	double x = end[0] - start[0], y = end[1] - start[1];
	return Point2D(y / ols.length(), -x / ols.length());
}

// This method sets the value of the source pixel position (sourceX,sourceY), given the destination pixel position.
Point2D OrientedLineSegment::GetSourcePosition( const OrientedLineSegment& source , const OrientedLineSegment& destination , Point2D target )
{
	// approach: travel a units of a_unit in the parallel direction and b units of b_unit in the perp direction (which is a total of c units from beginning of ols arrow to target)

	// DESTINATION FRAME

	// get a and b unit vectors
	Point2D dest_start = destination.endPoints[0], dest_end = destination.endPoints[1];
	double a_unit_dest[2] = {(dest_end[0] - dest_start[0]) / destination.length(), (dest_end[1] - dest_start[1]) / destination.length()};
	Point2D b_unit_point_dest = destination.perpendicular();
	double b_unit_dest[2] = {b_unit_point_dest[0], b_unit_point_dest[1]};
	
	// get a and b
	double p_v[2] = {target[0] - dest_start[0], target[1] - dest_start[1]};
	double c_dest = sqrt(pow(p_v[0], 2) + pow(p_v[1], 2));
	double b_dest = destination.distance(target);
	double a_dest = sqrt(pow(c_dest, 2) - pow(b_dest, 2));

	// correct orientation of unit vectors
	// if the dot product between a and c is negative, reverse a
	if (a_unit_dest[0] * p_v[0] + a_unit_dest[1] * p_v[1] < 0) {
		a_dest *= -1;
	}
	// if the dot product between b and c is negative, reverse b
	if (b_unit_dest[0] * p_v[0] + b_unit_dest[1] * p_v[1] < 0) {
		b_dest *= -1;
	}

	// SOURCE FRAME

	// get a and b unit vectors
	Point2D source_start = source.endPoints[0], source_end = source.endPoints[1];
	double a_unit_source[2] = {(source_end[0] - source_start[0]) / source.length(), (source_end[1] - source_start[1]) / source.length()};
	Point2D b_unit_point_source = source.perpendicular();
	double b_unit_source[2] = {b_unit_point_source[0], b_unit_point_source[1]};

	// scale
	a_unit_source[0] *= source.length() / destination.length();
	a_unit_source[1] *= source.length() / destination.length();

	// compute final point
	double final_x = source_start[0] + a_dest * a_unit_source[0] + b_dest * b_unit_source[0];
	double final_y = source_start[1] + a_dest * a_unit_source[1] + b_dest * b_unit_source[1];

	// debugging
	// cout << "---" << endl;
	// cout << "source vec (" << source_start[0] << ", " << source_start[1] << ") (" << source_end[0] << ", " << source_end[1] << ")" << endl;
	// cout << "dest vec (" << dest_start[0] << ", " << dest_start[1] << ") (" << dest_end[0] << ", " << dest_end[1] << ")" << endl;
	// cout << "target pt (" << target[0] << ", " << target[1] << ")" << endl;
	// cout << "source pt (" << final_x << ", " << final_y << ")" << endl;
	// cout << "a_unit_dest " << a_unit_dest[0] << ", " << a_unit_dest[1] << endl;
	// cout << "b_unit_dest " << b_unit_dest[0] << ", " << b_unit_dest[1] << endl;
	// cout << "a " << a_dest << endl;
	// cout << "b " << b_dest << endl;

	return Point2D(final_x, final_y);
}