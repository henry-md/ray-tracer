#include <algorithm>
#include "image.h"
#include <stdlib.h>
#include <math.h>
#include <Util/exceptions.h>

using namespace Util;
using namespace Image;
using namespace std;

// clamp helper function
int clamp(int value) {
	return std::max(0, std::min(255, value));
}

double clamp(double value) {
	return std::max(0.0, std::min(255.0, value));
}

/////////////
// Image32 //
/////////////
Image32 Image32::addRandomNoise( double noise ) const
{
	// Create a copy of the image
	Image32 copy = *this;

	// Loop through each pixel
	for (int i = 0; i < copy.width(); i++) {
		for (int j = 0; j < copy.height(); j++) {
			// Get the pixel
			Pixel32 pixel = copy(i, j);

			// get rand number [-1, 1]
			double rand_r = (rand() % 2001 - 1000) / 1000.0;
			double rand_g = (rand() % 2001 - 1000) / 1000.0;
			double rand_b = (rand() % 2001 - 1000) / 1000.0;

			// Add noise to the pixel
			double r = pixel.r + rand_r * 255 * noise; // rand() % (int)noise;
			double g = pixel.g + rand_g * 255 * noise;
			double b = pixel.b + rand_b * 255 * noise;

			// Clamp the pixel
			pixel.r = clamp(r);
			pixel.g = clamp(g);
			pixel.b = clamp(b);

			// Set the pixel
			copy(i, j) = pixel;
		}
	}

	return copy;
}

Image32 Image32::brighten( double brightness ) const
{
	Image32 copy = *this;

	for (int i = 0; i < copy.width(); i++) {
		for (int j = 0; j < copy.height(); j++) {
			// Get the pixel
			Pixel32 pixel = copy(i, j);

			// Add brightness to the pixel
			double r = pixel.r * brightness;
			double g = pixel.g * brightness;
			double b = pixel.b * brightness;

			// Clamp and set pixel
			pixel.r = clamp(r), pixel.g = clamp(g), pixel.b = clamp(b);
			copy(i, j) = pixel;
		}
	}

	return copy;
}

// outputs the gray-scale image.
Image32 Image32::luminance( void ) const
{
	Image32 copy = *this;

	for (int i = 0; i < copy.width(); i++) {
		for (int j = 0; j < copy.height(); j++) {
			Pixel32 pixel = copy(i, j);
			double luminance = 0.3 * pixel.r + 0.59 * pixel.g + 0.11 * pixel.b;
			pixel.r = luminance, pixel.g = luminance, pixel.b = luminance;
			copy(i, j) = pixel;
		}
	}

	return copy;
}

// Q: not working
Image32 Image32::contrast( double contrast ) const
{
	Image32 copy = *this;

	// get average luminescence
	double avg_lum = 0;
	for (int i = 0; i < copy.width(); i++) {
		for (int j = 0; j < copy.height(); j++) {
			Pixel32 pixel = copy(i, j);
			avg_lum += 0.3 * pixel.r + 0.59 * pixel.g + 0.11 * pixel.b;
		}
	}
	avg_lum /= copy.width() * copy.height();

	// Loop through each pixel
	for (int i = 0; i < copy.width(); i++) {
		for (int j = 0; j < copy.height(); j++) {
			// Get the pixel
			Pixel32 pixel = copy(i, j);

			// Compute the luminance
			double luminance = 0.3 * pixel.r + 0.59 * pixel.g + 0.11 * pixel.b;

			// Set the pixel
			double r = (pixel.r - avg_lum) * contrast + avg_lum;
			double g = (pixel.g - avg_lum) * contrast + avg_lum;
			double b = (pixel.b - avg_lum) * contrast + avg_lum;

			// Clamp and set pixel
			pixel.r = clamp(r), pixel.g = clamp(g), pixel.b = clamp(b);
			copy(i, j) = pixel;
		}
	}

	return copy;
}

Image32 Image32::saturate( double saturation ) const
{
	Image32 copy = *this;

	for (int i = 0; i < copy.width(); i++) {
		for (int j = 0; j < copy.height(); j++) {
			// Get the pixel
			Pixel32 pixel = copy(i, j);

			// Compute the luminance
			double luminance = 0.3 * pixel.r + 0.59 * pixel.g + 0.11 * pixel.b;

			// Set the pixel
			double r = (pixel.r - luminance) * saturation + luminance;
			double g = (pixel.g - luminance) * saturation + luminance;
			double b = (pixel.b - luminance) * saturation + luminance;

			// Clamp and set pixel
			pixel.r = clamp(r), pixel.g = clamp(g), pixel.b = clamp(b);
			copy(i, j) = pixel;
		}
	}

	return copy;
}

Image32 Image32::quantize( int bits ) const
{
	Image32 copy = *this;

	// Loop through each pixel
	for (int i = 0; i < copy.width(); i++) {
		for (int j = 0; j < copy.height(); j++) {
			// Get the pixel
			Pixel32 pixel = copy(i, j);

			// Quantize the pixel
			double new_r = (int)(pixel.r / 255.0 * (pow(2, bits) - 1) + 0.5);
			double new_g = (int)(pixel.g / 255.0 * (pow(2, bits) - 1) + 0.5);
			double new_b = (int)(pixel.b / 255.0 * (pow(2, bits) - 1) + 0.5);

			double r = new_r * 255.0 / (pow(2, bits) - 1);
			double g = new_g * 255.0 / (pow(2, bits) - 1);
			double b = new_b * 255.0 / (pow(2, bits) - 1);

			// Clamp and set pixel
			pixel.r = clamp(r), pixel.g = clamp(g), pixel.b = clamp(b);
			copy(i, j) = pixel;
		}
	}
	return copy;
}

Image32 Image32::randomDither( int bits ) const
{
	Image32 copy = *this;
	Image32 noisy = copy.addRandomNoise(1 / pow(2, bits));
	Image32 quantized = noisy.quantize(bits);
	return quantized;
}

Image32 Image32::orderedDither2X2( int bits ) const
{
	Image32 copy = *this;
	int D[2][2] = { { 1, 3 }, { 4, 2 } };

	// loop through each pixel
	for (int i = 0; i < copy.width(); i++) {
		for (int j = 0; j < copy.height(); j++) {

			// Get the pixel
			Pixel32 pixel = copy(i, j);

			double I[3] = { pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0 };
			double b_bit_I[3] = { I[0] * (pow(2, bits) - 1), I[1] * (pow(2, bits) - 1), I[2] * (pow(2, bits) - 1) };
			double b_bits_frac[3] = { b_bit_I[0] - (int)b_bit_I[0], b_bit_I[1] - (int)b_bit_I[1], b_bit_I[2] - (int)b_bit_I[2] };
			double round_thresh = D[i % 2][j % 2] / 5.0;
			double b_bits_round[3] = { b_bits_frac[0] > round_thresh ? 1.0 : 0.0, b_bits_frac[1] > round_thresh ? 1.0 : 0.0, b_bits_frac[2] > round_thresh ? 1.0 : 0.0 };
			double b_bit_I_ordered_dither[3] = { (int)b_bit_I[0] + b_bits_round[0], (int)b_bit_I[1] + b_bits_round[1], (int)b_bit_I[2] + b_bits_round[2] };
			double I_8_bit[3] = { b_bit_I_ordered_dither[0] * 255.0 / (pow(2, bits) - 1), b_bit_I_ordered_dither[1] * 255.0 / (pow(2, bits) - 1), b_bit_I_ordered_dither[2] * 255.0 / (pow(2, bits) - 1) };

			// Clamp and set pixel
			pixel.r = clamp(I_8_bit[0]), pixel.g = clamp(I_8_bit[1]), pixel.b = clamp(I_8_bit[2]);
			copy(i, j) = pixel;
		}
	}
	
	return copy;
}

Image32 Image32::floydSteinbergDither( int bits ) const
{
	Image32 copy = *this;

	// loop through each pixel
	for (int j = 0; j < copy.height(); j++) {
		for (int i = 0; i < copy.width(); i++) {
		
			// quantize pixel and get errors of the quantization per channel
			Pixel32 pixel = copy(i, j);
			double b_bit_I[3] = { round(copy(i, j).r / 255.0 * (pow(2, bits) - 1)), round(copy(i, j).g / 255.0 * (pow(2, bits) - 1)), round(copy(i, j).b / 255.0 * (pow(2, bits) - 1)) };
			double I_quantized[3] = { b_bit_I[0] * 255.0 / (pow(2, bits) - 1), b_bit_I[1] * 255.0 / (pow(2, bits) - 1), b_bit_I[2] * 255.0 / (pow(2, bits) - 1) };
			double error[3] = { pixel.r - I_quantized[0], pixel.g - I_quantized[1], pixel.b - I_quantized[2] };
			
			// clamp and set pixel to quantized values
			pixel.r = clamp(I_quantized[0]), pixel.g = clamp(I_quantized[1]), pixel.b = clamp(I_quantized[2]);
			copy(i, j) = pixel;

			// propagate errors
			if (i != copy.width() - 1) {
				copy(i + 1, j).r = clamp(copy(i + 1, j).r + error[0] * 7.0 / 16.0);
				copy(i + 1, j).g = clamp(copy(i + 1, j).g + error[1] * 7.0 / 16.0);
				copy(i + 1, j).b = clamp(copy(i + 1, j).b + error[2] * 7.0 / 16.0);
			}
			if (i != 0 && j != copy.height() - 1) {
				copy(i - 1, j + 1).r = clamp(copy(i - 1, j + 1).r + error[0] * 3.0 / 16.0);
				copy(i - 1, j + 1).g = clamp(copy(i - 1, j + 1).g + error[1] * 3.0 / 16.0);
				copy(i - 1, j + 1).b = clamp(copy(i - 1, j + 1).b + error[2] * 3.0 / 16.0);
			}
			if (j != copy.height() - 1) {
				copy(i, j + 1).r = clamp(copy(i, j + 1).r + error[0] * 5.0 / 16.0);
				copy(i, j + 1).g = clamp(copy(i, j + 1).g + error[1] * 5.0 / 16.0);
				copy(i, j + 1).b = clamp(copy(i, j + 1).b + error[2] * 5.0 / 16.0);
			}
			if (i != copy.width() - 1 && j != copy.height() - 1) {
				copy(i + 1, j + 1).r = clamp(copy(i + 1, j + 1).r + error[0] * 1.0 / 16.0);
				copy(i + 1, j + 1).g = clamp(copy(i + 1, j + 1).g + error[1] * 1.0 / 16.0);
				copy(i + 1, j + 1).b = clamp(copy(i + 1, j + 1).b + error[2] * 1.0 / 16.0);
			}
		}
	}

	return copy;
}

Image32 Image32::blur3X3( void ) const
{
	Image32 copy = *this;
	Image32 blurred = Image32();
	blurred.setSize(copy.width(), copy.height());

	// loop through each pixel
	for (int i = 0; i < copy.width(); i++) {
		for (int j = 0; j < copy.height(); j++) {
			Pixel32 pixel = copy(i, j);
			double r = 0, g = 0, b = 0;
			// int count = 0;

			int filter[3][3] = { { 1, 2, 1 }, { 2, 4, 2 }, { 1, 2, 1 } };

			// loop through each neighbor
			for (int k = -1; k <= 1; k++) {
				for (int l = -1; l <= 1; l++) {
					if (j + k < 0 || j + k >= copy.height()) continue;
					if (i + l < 0 || i + l >= copy.width()) continue;
					r += copy(i + l, j + k).r * filter[k + 1][l + 1];
					g += copy(i + l, j + k).g * filter[k + 1][l + 1];
					b += copy(i + l, j + k).b * filter[k + 1][l + 1];
				}
			}

			Pixel32 new_pixel;
			new_pixel.r = r / 16.0, new_pixel.g = g / 16.0, new_pixel.b = b / 16.0;
			blurred(i, j) = new_pixel;
		}
	}
	return blurred;
}

Image32 Image32::edgeDetect3X3( void ) const
{
	Image32 copy = *this;
	Image32 blurred = Image32();
	blurred.setSize(copy.width(), copy.height());

	int filter[3][3] = { { -1, -1, -1 }, { -1, 8, -1 }, { -1, -1, -1 } };
	double r = 0, g = 0, b = 0;

	// loop through each pixel
	for (int i = 0; i < copy.width(); i++) {
		for (int j = 0; j < copy.height(); j++) {
			
			// loop through each neighbor
			r = 0, g = 0, b = 0;
			for (int l = -1; l <= 1; l++) {
				for (int k = -1; k <= 1; k++) {
					if (i + l < 0 || i + l >= copy.width()) continue;
					if (j + k < 0 || j + k >= copy.height()) continue;
					
					r += copy(i + l, j + k).r * filter[k + 1][l + 1];
					g += copy(i + l, j + k).g * filter[k + 1][l + 1];
					b += copy(i + l, j + k).b * filter[k + 1][l + 1];
				}
			}

			Pixel32 new_pixel;
			new_pixel.r = clamp(r), new_pixel.g = clamp(g), new_pixel.b = clamp(b);
			blurred(i, j) = new_pixel;
		}
	}
	return blurred;
}

Image32 Image32::scaleNearest( double scaleFactor ) const
{
	Image32 copy = *this;
	Image32 scaled = Image32();
	scaled.setSize((int)(copy.width() * scaleFactor), (int)(copy.height() * scaleFactor));

	for (int i = 0; i < scaled.width(); i++) {
		for (int j = 0; j < scaled.height(); j++) {
			double x = i / scaleFactor, y = j / scaleFactor;
			Pixel32 pixel = nearestSample(Point2D(x, y));
			scaled(i, j) = pixel;
		}
	}

	return scaled;
}

Image32 Image32::scaleBilinear( double scaleFactor ) const
{
	Image32 copy = *this;
	Image32 scaled = Image32();
	scaled.setSize((int)(copy.width() * scaleFactor), (int)(copy.height() * scaleFactor));

	for (int i = 0; i < scaled.width(); i++) {
		for (int j = 0; j < scaled.height(); j++) {
			double x = i / scaleFactor, y = j / scaleFactor;

			// call sample bilinear function
			Pixel32 pixel = bilinearSample(Point2D(x, y));
			scaled(i, j) = pixel;
		}
	}

	return scaled;
}

// TODO
Image32 Image32::scaleGaussian( double scaleFactor ) const
{
	Image32 copy = *this;
	Image32 scaled = Image32();
	scaled.setSize((int)(copy.width() * scaleFactor), (int)(copy.height() * scaleFactor));
	double sigma = 1 / scaleFactor;
	double radius = 2 * sigma;

	// seems to work better for scaling factors < 1
	sigma = 0.5;
	radius = 1 / sigma;

	// loop through all pixels
	for (int i = 0; i < scaled.width(); i++) {
		for (int j = 0; j < scaled.height(); j++) {
			double x = i / scaleFactor, y = j / scaleFactor;
			Pixel32 pixel = gaussianSample(Point2D(x, y), sigma, radius);
			scaled(i, j) = pixel;
		}
	}

	return scaled;
}

Image32 Image32::rotateNearest( double angle ) const
{
	double pi = M_PI;
	double radian = angle * pi/180;

	Image32 copy = *this;
	int w = copy.width(), h = copy.height();
	int new_w = h * sin(radian) + w * cos(radian), new_h = w * sin(radian) + h * cos(radian);
	cout << new_w << " " << new_h << endl;
	int w_offset = (new_w - w) / 2, h_offset = (new_h - h) / 2;

	Image32 rotated = Image32();
	rotated.setSize(new_w, new_h);

	for (int i = 0; i < new_w; i++) {
		for (int j = 0; j < new_h; j++) {

			// center at origin
			double x_b = i - new_w/2, y_b = j - new_h/2;

			// pass through rotation matrix (rotating counter clockwise)
			double x_a = x_b * cos(radian) - y_b * sin(radian), y_a = x_b * sin(radian) + y_b * cos(radian);

			// center to original image
			x_a += w/2, y_a += h/2;
			int closest_x = (int)(x_a + 0.5), closest_y = (int)(y_a + 0.5);

			// read the pixel
			Pixel32 p;
			if (closest_x >= 0 && closest_x < w && closest_y >= 0 && closest_y < h) {
				p = copy(closest_x, closest_y);
			}
			else {
				p.r = 0, p.g = 0, p.b = 0;
			}

			rotated(i, j) = p;
		}
	}
	return rotated;
}

Image32 Image32::rotateBilinear( double angle ) const
{
	double pi = M_PI;
	double radian = angle * pi/180;

	Image32 copy = *this;
	int w = copy.width(), h = copy.height();
	int new_w = h * abs(sin(radian)) + w * abs(cos(radian)), new_h = w * abs(sin(radian)) + h * abs(cos(radian));
	// cout << new_w << " " << new_h << endl;
	int w_offset = (new_w - w) / 2, h_offset = (new_h - h) / 2;

	Image32 rotated = Image32();
	rotated.setSize(new_w, new_h);

	for (int i = 0; i < new_w; i++) {
		for (int j = 0; j < new_h; j++) {

			// center at origin
			double x_b = i - new_w/2, y_b = j - new_h/2;

			// pass through rotation matrix (rotating counter clockwise)
			double x_a = x_b * cos(radian) - y_b * sin(radian), y_a = x_b * sin(radian) + y_b * cos(radian);

			// center to original image
			x_a += w/2, y_a += h/2;
			
			// how would you even use this function if you can only pass a float?
			// Pixel32 pixel = bilinearSample(Point2D(x_a, y_a));

			// read 4 pixels around the sampling point
			double x = x_a, y = y_a;
			int x1 = (int)x, y1 = (int)y;
			int x2 = x1 + 1, y2 = y1 + 1;
			double dx = x - x1, dy = y - y1;
			double top[3], bottom[3], mid[3];
			int tl[3] = {0, 0, 0}, tr[3] = {0, 0, 0}, bl[3] = {0, 0, 0}, br[3] = {0, 0, 0};
			if (x1 >= 0 && y1 >= 0 && x1 < copy.width() && y1 < copy.height()) {
				tl[0] = copy(x1, y1).r, tl[1] = copy(x1, y1).g, tl[2] = copy(x1, y1).b;
			}
			if (x2 >= 0 && y1 >= 0 && x2 < copy.width() && y1 < copy.height()) {
				tr[0] = copy(x2, y1).r, tr[1] = copy(x2, y1).g, tr[2] = copy(x2, y1).b;
			}
			if (x1 >= 0 && y2 >= 0 && x1 < copy.width() && y2 < copy.height()) {
				bl[0] = copy(x1, y2).r, bl[1] = copy(x1, y2).g, bl[2] = copy(x1, y2).b;
			}
			if (x2 >=0 && y2 >=0 && x2 < copy.width() && y2 < copy.height()) {
				br[0] = copy(x2, y2).r, br[1] = copy(x2, y2).g, br[2] = copy(x2, y2).b;
			}

			// linear sampling horizontally
			top[0] = tl[0] * (1 - dx) + tr[0] * dx;
			top[1] = tl[1] * (1 - dx) + tr[1] * dx;
			top[2] = tl[2] * (1 - dx) + tr[2] * dx;
			bottom[0] = bl[0] * (1 - dx) + br[0] * dx;
			bottom[1] = bl[1] * (1 - dx) + br[1] * dx;
			bottom[2] = bl[2] * (1 - dx) + br[2] * dx;

			// linear sample from top and bottom
			mid[0] = top[0] * (1 - dy) + bottom[0] * dy;
			mid[1] = top[1] * (1 - dy) + bottom[1] * dy;
			mid[2] = top[2] * (1 - dy) + bottom[2] * dy;

			Pixel32 pixel;
			pixel.r = mid[0], pixel.g = mid[1], pixel.b = mid[2];
			rotated(i, j) = pixel;
		}
	}
	return rotated;
}

Image32 Image32::rotateGaussian( double angle ) const
{
	double pi = M_PI;
	double radian = angle * pi/180;

	double sigma = 0.5;
	double radius = 1 / sigma;

	Image32 copy = *this;
	int w = copy.width(), h = copy.height();
	int new_w = h * abs(sin(radian)) + w * abs(cos(radian)), new_h = w * abs(sin(radian)) + h * abs(cos(radian));
	// cout << new_w << " " << new_h << endl;
	int w_offset = (new_w - w) / 2, h_offset = (new_h - h) / 2;

	Image32 rotated = Image32();
	rotated.setSize(new_w, new_h);

	for (int i = 0; i < new_w; i++) {
		for (int j = 0; j < new_h; j++) {

			// center at origin
			double x_b = i - new_w/2, y_b = j - new_h/2;

			// pass through rotation matrix (rotating counter clockwise)
			double x_a = x_b * cos(radian) - y_b * sin(radian), y_a = x_b * sin(radian) + y_b * cos(radian);

			// center to original image
			x_a += w/2, y_a += h/2;
			
			Pixel32 pixel = gaussianSample(Point2D(x_a, y_a), sigma, radius);

			rotated(i, j) = pixel;
		}
	}
	return rotated;
}


/* don't have to do */
void Image32::setAlpha( const Image32& matte )
{
	WARN( "method undefined" );
}

Image32 Image32::composite( const Image32& overlay ) const
{
	WARN( "method undefined" );
	return Image32();
}

Image32 Image32::CrossDissolve( const Image32& source , const Image32& destination , double blendWeight )
{
	
	Image32 blended = Image32();
	blended.setSize(source.width(), source.height());

	for (int i = 0; i < source.width(); i++) {
		for (int j = 0; j < source.height(); j++) {
			Pixel32 pixel;
			pixel.r = source(i, j).r * (1 - blendWeight) + destination(i, j).r * blendWeight;
			pixel.g = source(i, j).g * (1 - blendWeight) + destination(i, j).g * blendWeight;
			pixel.b = source(i, j).b * (1 - blendWeight) + destination(i, j).b * blendWeight;
			blended(i, j) = pixel;
		}
	}

	return blended;
}

Image32 Image32::warp( const OrientedLineSegmentPairs& olsp ) const
{
	Image32 copy = *this;
	Image32 warped = Image32();
	warped.setSize(copy.width(), copy.height());
	int pairs_cnt = olsp.size();

	Pixel32 dis = copy(100, 100);
	#pragma message ("[QUESTION] Why can't I read a pixel here?")
	// cout << "dis " << dis.r << " " << dis.g << " " << dis.b << endl;

	for (int i = 0; i < copy.width(); i++) {
		for (int j = 0; j < copy.height(); j++) {
			Point2D target = Point2D(i, j);
			Point2D sample_rgn = olsp.getSourcePosition(target);

			if (sample_rgn[0] < 0 || sample_rgn[1] < 0 || sample_rgn[0] >= copy.width() || sample_rgn[1] >= copy.height()) {
				warped(i, j) = Pixel32();
				continue;
			}

			// clamp point
			if (sample_rgn[0] < 0) sample_rgn[0] = 0;
			if (sample_rgn[1] < 0) sample_rgn[1] = 0;
			if (sample_rgn[0] >= copy.width()) sample_rgn[0] = copy.width() - 1;
			if (sample_rgn[1] >= copy.height()) sample_rgn[1] = copy.height() - 1;

			warped(i, j) = copy(sample_rgn[0], sample_rgn[1]);
		}
	}
	
	return warped;
}

// inverse sin mapping from source to destination
Image32 Image32::funFilter( void ) const
{
	Image32 copy = *this;
	Image32 fun = Image32();
	fun.setSize(copy.width(), copy.height());

	for (int i = 0; i < copy.width(); i++) {
		for (int j = 0; j < copy.height(); j++) {
			double theta_x = sin(i * M_PI / 2 / copy.width());
			double theta_y = sin(j * M_PI / 2 / copy.height());
			double scaled_back_x = theta_x * copy.width();
			double scaled_back_y = theta_y * copy.height();
			fun(i, j) = nearestSample(Point2D(scaled_back_x, scaled_back_y));
		}
	}

	return fun;
}

Image32 Image32::crop( int x1 , int y1 , int x2 , int y2 ) const
{
	Image32 copy = *this;
	Image32 cropped;
	cropped.setSize(x2 - x1 + 1, y2 - y1 + 1);

	for (int i = x1; i <= x2; i++) {
		for (int j = y1; j <= y2; j++) {
			cropped(i - x1, j - y1) = copy(i, j);
		}
	}
	return cropped;
}

Pixel32 Image32::nearestSample( Point2D p ) const
{
	Image32 copy = *this;
	double x = p[0], y = p[1];
	int closest_x = (int)(x + 0.5);
	int closest_y = (int)(y + 0.5);
	if (closest_x < 0 || closest_x >= copy.width()) return Pixel32();
	if (closest_y < 0 || closest_y >= copy.height()) return Pixel32();
	return copy(closest_x, closest_y);
}

Pixel32 Image32::bilinearSample( Point2D p ) const
{
	///////////////////////////////
	// Do bilinear sampling here //
	///////////////////////////////

	Image32 copy = *this;

	double x = p[0], y = p[1];
	int x1 = (int)x, y1 = (int)y;
	int x2 = x1 + 1, y2 = y1 + 1;
	double dx = x - x1, dy = y - y1;
	double top[3], bottom[3], mid[3];
	int tl[3] = {0, 0, 0}, tr[3] = {0, 0, 0}, bl[3] = {0, 0, 0}, br[3] = {0, 0, 0};
	if (x1 < copy.width() && y1 < copy.height()) {
		tl[0] = copy(x1, y1).r, tl[1] = copy(x1, y1).g, tl[2] = copy(x1, y1).b;
	}
	if (x2 < copy.width() && y1 < copy.height()) {
		tr[0] = copy(x2, y1).r, tr[1] = copy(x2, y1).g, tr[2] = copy(x2, y1).b;
	}
	if (x1 < copy.width() && y2 < copy.height()) {
		bl[0] = copy(x1, y2).r, bl[1] = copy(x1, y2).g, bl[2] = copy(x1, y2).b;
	}
	if (x2 < copy.width() && y2 < copy.height()) {
		br[0] = copy(x2, y2).r, br[1] = copy(x2, y2).g, br[2] = copy(x2, y2).b;
	}

	// linear sampling horizontally
	top[0] = tl[0] * (1 - dx) + tr[0] * dx;
	top[1] = tl[1] * (1 - dx) + tr[1] * dx;
	top[2] = tl[2] * (1 - dx) + tr[2] * dx;
	bottom[0] = bl[0] * (1 - dx) + br[0] * dx;
	bottom[1] = bl[1] * (1 - dx) + br[1] * dx;
	bottom[2] = bl[2] * (1 - dx) + br[2] * dx;

	// linear sample from top and bottom
	mid[0] = top[0] * (1 - dy) + bottom[0] * dy;
	mid[1] = top[1] * (1 - dy) + bottom[1] * dy;
	mid[2] = top[2] * (1 - dy) + bottom[2] * dy;

	Pixel32 pixel;
	pixel.r = mid[0], pixel.g = mid[1], pixel.b = mid[2];
	return pixel;
}

Pixel32 Image32::gaussianSample( Point2D p , double variance , double radius ) const
{
	Image32 copy = *this;
	double x = p[0], y = p[1];

	double x_err = x - (int)x, y_err = y - (int)y;

	// store sums of rgb
	double r = 0, g = 0, b = 0;
	double gaussian_weight_sum = 0;

	// gaussian sampling
	int x_l = (int)(x - radius), x_r = (int)(x + 0.5 + radius);
	int y_l = (int)(y - radius), y_r = (int)(y + 0.5 + radius);

	for (int l = x_l; l <= x_r; l++) {
		for (int k = y_l; k <= y_r; k++) {
			
			// compute gaussian weight for the given point in gaussian mask
			double dist_to_sampling_rgn = sqrt(pow(x - l , 2) + pow(y - k , 2));
			double squared_dist_to_sampling_rgn = pow(dist_to_sampling_rgn, 2);
			// if (dist_to_sampling_rgn > radius) continue;
			double gaussian_weight = exp(-(squared_dist_to_sampling_rgn) / (2 * pow(variance, 2)));

			// add one more value in gaussian-weighted mask
			Pixel32 pixel;
			if (l >= 0 && l < copy.width() && k >= 0 && k < copy.height()) {
				pixel = copy(l, k);
			}
			r += pixel.r * gaussian_weight;
			g += pixel.g * gaussian_weight;
			b += pixel.b * gaussian_weight;
			gaussian_weight_sum += gaussian_weight;
		}
	}
	Pixel32 pixel;
	pixel.r = r / gaussian_weight_sum, pixel.g = g / gaussian_weight_sum, pixel.b = b / gaussian_weight_sum;
	return pixel;
}
