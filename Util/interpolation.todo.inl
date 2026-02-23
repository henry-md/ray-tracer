/*
Copyright (c) 2019, Michael Kazhdan
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer. Redistributions in binary form must reproduce
the above copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the distribution. 

Neither the name of the Johns Hopkins University nor the names of its contributors
may be used to endorse or promote products derived from this software without specific
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.
*/

#include <math.h>
#include <Util/exceptions.h>

namespace Util
{
	///////////////////
	// Interpolation //
	///////////////////
	template< typename SampleType >
	SampleType Interpolation::Sample( const std::vector< SampleType > &samples , double t , int interpolationType )
	{
		switch( interpolationType )
		{
		case NEAREST:
		{
			t *= samples.size();
			int it1 = (int)floor(t);
			int it2 = ( it1 + 1 ) % samples.size();
			t -= it1;
			if( t<0.5 ) return samples[it1];
			else        return samples[it2];
			break;
		}
		case LINEAR:
		{
			///////////////////////////////////////
			// Perform linear interpolation here //
			///////////////////////////////////////
			t *= samples.size();
			int it1 = (int)floor(t);
			int it2 = ( it1 + 1 ) % samples.size();
			t -= it1;
			return samples[it1] * (1 - t) + samples[it2] * t;
			break;
		}
		case CATMULL_ROM:
		{
			////////////////////////////////////////////
			// Perform Catmull-Rom interpolation here //
			////////////////////////////////////////////
			t *= samples.size();
				int it1 = (int)floor(t);
				int it0 = (it1 - 1 + samples.size()) % samples.size();
				int it2 = (it1 + 1) % samples.size();
				int it3 = (it1 + 2) % samples.size();
				t -= it1;
				double s = 0.5;
				double t2 = t * t;
				double t3 = t2 * t;
				double c0 = -s * t3 + 2 * s * t2 - s * t;
				double c1 = (2 - s) * t3 + (s - 3) * t2 + 1;
				double c2 = (s - 2) * t3 + (3 - 2 * s) * t2 + s * t;
				double c3 = s * t3 - s * t2;
				return samples[it0] * c0 + samples[it1] * c1 + samples[it2] * c2 + samples[it3] * c3;
				break;
		}
		case UNIFORM_CUBIC_B_SPLINE:
		{
			///////////////////////////////////////////////////////
			// Perform uniform cubic b-spline interpolation here //
			///////////////////////////////////////////////////////
			t *= samples.size();
			int it1 = (int)floor(t);
			int it0 = (it1 - 1 + samples.size()) % samples.size();
			int it2 = (it1 + 1) % samples.size();
			int it3 = (it1 + 2) % samples.size();
			t -= it1;
			double s = 0.5;
			double t2 = t * t;
			double t3 = t2 * t;
			double b_03 = (1/3.0 - s) * t3 + (-1/2.0 + 2 * s) * t2 - s * t;
			double b_13 = (1 - s) * t3 + (-3/2.0 + s) * t2 + 2/3.0;
			double b_23 = (-1 + s) * t3 + (3/2.0 - 2 * s) * t2 + s * t + 1/6.0;
			double b_33 = (-1/3.0 + s) * t3 + (1/2.0 - s) * t2;
			return samples[it0] * b_03 + samples[it1] * b_13 + samples[it2] * b_23 + samples[it3] * b_33;
			break;
		}
		default:
			ERROR_OUT( "unrecognized interpolation type" );
			return samples[0];
		}
	}
}