//
//  getPitch.c
//

#include "getPitch.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

static double sam[PITCHTRACKER_BUFFER_SIZE];
static int distances[PITCHTRACKER_BUFFER_SIZE];
static int mins[PITCHTRACKER_BUFFER_SIZE];
static int maxs[PITCHTRACKER_BUFFER_SIZE];

static double acceptedError = 0.2;
static double maximaThresholdRatio = 0.75;

typedef struct _dywapitchtracker 
{
	double	_prevPitch;
	int		_pitchConfidence;
	
} Dywapitchtracker;

static Dywapitchtracker pitchTracker;

/*
 
 Utils

 */

#ifndef max
#define max(x, y) ((x) > (y)) ? (x) : (y)
#endif
#ifndef min
#define min(x, y) ((x) < (y)) ? (x) : (y)
#endif

// returns 1 if power of 2
static inline int _power2p(int value) 
{
	if (value == 0) return 1;
	if (value == 2) return 1;
	if (value & 0x1) return 0;
	return (_power2p(value >> 1));
}

// count number of bits
static inline int _bitcount(int value) 
{
	if (value == 0) return 0;
	if (value == 1) return 1;
	if (value == 2) return 2;
	return _bitcount(value >> 1) + 1;
}

// closest power of 2 above or equal
static inline int _ceil_power2(int value) 
{
	if (_power2p(value)) return value;
	
	if (value == 1) return 2;
	int j, i = _bitcount(value);
	int res = 1;
	for (j = 0; j < i; j++) res <<= 1;
	return res;
}

// closest power of 2 below or equal
static inline int _floor_power2(int value) 
{
	if (_power2p(value)) return value;
	return _ceil_power2(value)/2;
}

// abs value
static inline int _iabs(int x) 
{
	if (x >= 0) return x;
	return -x;
}

// 2 power
static inline int _2power(int i) 
{
	int res = 1, j;
	for (j = 0; j < i; j++) res <<= 1;
	return res;
}

/*
 
 The Wavelet algorithm
 
*/

static double _computeWaveletPitch(double * samples) 
{
	double pitchF = 0.0;
	
	int i, j;
	double si, si1;
	
	memcpy(sam, samples, sizeof(double)*PITCHTRACKER_BUFFER_SIZE);
	int curSamNb = PITCHTRACKER_BUFFER_SIZE;
	
	int nbMins, nbMaxs;
	
	// algorithm parameters
	int maxFLWTlevels = 6;
	double maxF = 3000.;
	int differenceLevelsN = 3;
	
	double ampltitudeThreshold;  
	double theDC = 0.0;
	
	// compute ampltitudeThreshold and theDC
	//first compute the DC and maxAMplitude
	double maxValue = 0.0;
	double minValue = 0.0;
	for (i = 0; i < PITCHTRACKER_BUFFER_SIZE;i++) 
	{
		si = sam[i];
		theDC = theDC + si;
		if (si > maxValue) maxValue = si;
		if (si < minValue) minValue = si;
	}
	theDC = theDC/(double)PITCHTRACKER_BUFFER_SIZE;
	maxValue = maxValue - theDC;
	minValue = minValue - theDC;
	double amplitudeMax = (maxValue > -minValue ? maxValue : -minValue);
	
	ampltitudeThreshold = amplitudeMax*maximaThresholdRatio;			
	
	// levels, start without downsampling..
	int curLevel = 0;
	double curModeDistance = -1.;
	int delta;
	
	for(;;) 
	{
		
		// delta
		delta = 44100./(_2power(curLevel)*maxF);
		
		if (curSamNb < 2) 
			break;
		
		// compute the first maximums and minumums after zero-crossing
		// store if greater than the min threshold
		// and if at a greater distance than delta
		double dv, previousDV = -1000;
		nbMins = nbMaxs = 0;   
		int lastMinIndex = -1000000;
		int lastmaxIndex = -1000000;
		int findMax = 0;
		int findMin = 0;
		for (i = 2; i < curSamNb; i++) 
		{
			si = sam[i] - theDC;
			si1 = sam[i-1] - theDC;
			
			if (si1 <= 0 && si > 0) findMax = 1;
			if (si1 >= 0 && si < 0) findMin = 1;
			
			// min or max ?
			dv = si - si1;
			
			if (previousDV > -1000) 
			{				
				if (findMin && previousDV < 0 && dv >= 0) 
				{ 
					// minimum
					if (fabs(si) >= ampltitudeThreshold) 
					{
						if (i > lastMinIndex + delta) 
						{
							mins[nbMins++] = i;
							lastMinIndex = i;
							findMin = 0;
						}
					}
				}
				
				if (findMax && previousDV > 0 && dv <= 0) 
				{
					// maximum
					if (fabs(si) >= ampltitudeThreshold) 
					{
						if (i > lastmaxIndex + delta) 
						{
							maxs[nbMaxs++] = i;
							lastmaxIndex = i;
							findMax = 0;
						}
					}
				}
			}
			
			previousDV = dv;
		}
		
		if (nbMins == 0 && nbMaxs == 0) 
			break;

		// maxs = [5, 20, 100,...]
		// compute distances
		int d;
		memset(distances, 0, PITCHTRACKER_BUFFER_SIZE*sizeof(int));
		for (i = 0 ; i < nbMins ; i++) 
		{
			for (j = 1; j < differenceLevelsN; j++) 
			{
				if (i+j < nbMins) 
				{
					d = _iabs(mins[i] - mins[i+j]);
					distances[d] = distances[d] + 1;
				}
			}
		}
		for (i = 0 ; i < nbMaxs ; i++) 
		{
			for (j = 1; j < differenceLevelsN; j++) 
			{
				if (i+j < nbMaxs) 
				{
					d = _iabs(maxs[i] - maxs[i+j]);
					distances[d] = distances[d] + 1;
				}
			}
		}
		
		// find best summed distance
		int bestDistance = -1;
		int bestValue = -1;
		for (i = 0; i< curSamNb; i++) 
		{
			int summed = 0;
			for (j = -delta ; j <= delta ; j++) 
			{
				if (i+j >=0 && i+j < curSamNb)
					summed += distances[i+j];
			}

			if (summed == bestValue) 
			{
				if (i == 2*bestDistance)
					bestDistance = i;
				
			} else if (summed > bestValue) 
			{
				bestValue = summed;
				bestDistance = i;
			}
		}
		
		// averaging
		double distAvg = 0.0;
		double nbDists = 0;
		for (j = -delta ; j <= delta ; j++) 
		{
			if (bestDistance+j >=0 && bestDistance+j < PITCHTRACKER_BUFFER_SIZE) 
			{
				int nbDist = distances[bestDistance+j];
				if (nbDist > 0) 
				{
					nbDists += nbDist;
					distAvg += (bestDistance+j)*nbDist;
				}
			}
		}
		// this is our mode distance !
		distAvg /= nbDists;
		
		// continue the levels ?
		if (curModeDistance > -1.) 
		{
			double similarity = fabs(distAvg*2 - curModeDistance);
			if (similarity <= 2*delta) 
			{
				pitchF = 44100./(_2power(curLevel-1)*curModeDistance);
				break;
			}
		}
		
		// not similar, continue next level
		curModeDistance = distAvg;
		
		curLevel = curLevel + 1;
		if (curLevel >= maxFLWTlevels) 
			break;
		
		// downsample
		if (curSamNb < 2) 
			break;
		for (i = 0; i < curSamNb/2; i++) 
		{
			sam[i] = (sam[2*i] + sam[2*i + 1])/2.;
		}
		curSamNb /= 2;
	}
	
	return pitchF;
}

/*
 
 Post-processing.
 
 Assumptions: 
 
 * a rapid change in pitch (>20%) is assumed to be an error and will be smoothed

 * a rapid doubling or halving (a common side effect of pitch readers) 
   is assumed to be an error and will be smoothed
 
 * a discrete, short-burst pitch is assumed to be an error and will be ignored
 
 */

double _dynamicprocess(double pitch) 
{
	
	if (pitch == 0.0) 
		pitch = -1.0;
	
	double estimatedPitch = -1;
	int maxConfidence = 5;
	
	if (pitch != -1) 
	{
		// I have a pitch here
		
		if (pitchTracker._prevPitch == -1) 
		{
			// no previous
			estimatedPitch = pitch;
			pitchTracker._prevPitch = pitch;
			pitchTracker._pitchConfidence = 1;
			
		} 
		else if (abs(pitchTracker._prevPitch - pitch)/pitch < acceptedError) 
		{
			// similar : remember and increment pitch
			pitchTracker._prevPitch = pitch;
			estimatedPitch = pitch;
			pitchTracker._pitchConfidence = min(maxConfidence, pitchTracker._pitchConfidence + 1); 
			// maximum 3
			
		} 
		else if ((pitchTracker._pitchConfidence >= maxConfidence-2) && 
				   abs(pitchTracker._prevPitch - 2.*pitch)/(2.*pitch) < acceptedError) 
		{
			// close to half the last pitch, which is trusted
			estimatedPitch = 2.*pitch;
			pitchTracker._prevPitch = estimatedPitch;
			
		} 
		else if ((pitchTracker._pitchConfidence >= maxConfidence-2) && 
				   abs(pitchTracker._prevPitch - 0.5*pitch)/(0.5*pitch) < acceptedError) 
		{
			// close to twice the last pitch, which is trusted
			estimatedPitch = 0.5*pitch;
			pitchTracker._prevPitch = estimatedPitch;
			
		} 
		else 
		{
			// nothing like this : very different value
			if (pitchTracker._pitchConfidence >= 1) 
			{
				// previous trusted : keep previous
				estimatedPitch = pitchTracker._prevPitch;
				pitchTracker._pitchConfidence = max(0, pitchTracker._pitchConfidence - 1);
			} 
			else 
			{
				// previous not trusted : take current
				estimatedPitch = pitch;
				pitchTracker._prevPitch = pitch;
				pitchTracker._pitchConfidence = 1;
			}
		}
		
	} 
	else 
	{
		// no pitch now
		if (pitchTracker._prevPitch != -1) 
		{
			// was pitch before
			if (pitchTracker._pitchConfidence >= 1) 
			{
				// continue previous
				estimatedPitch = pitchTracker._prevPitch;
				pitchTracker._pitchConfidence = max(0, pitchTracker._pitchConfidence - 1);
			} 
			else 
			{
				pitchTracker._prevPitch = -1;
				estimatedPitch = -1.;
				pitchTracker._pitchConfidence = 0;
			}
		}
	}
	
	if (pitchTracker._pitchConfidence >= 1) 
	{
		// ok
		pitch = estimatedPitch;
	} 
	else 
	{
		pitch = -1;
	}
	
	// equivalence
	if (pitch == -1) 
		pitch = 0.0;
	
	return pitch;
}


// -- Public functions --

void getPitch_setAcceptedError(double e)
{
	acceptedError = e;
}

void getPitch_setThresholdRatio(double r)
{
	maximaThresholdRatio = r;
}

void getPitch_init() 
{
	pitchTracker._prevPitch = -1.;
	pitchTracker._pitchConfidence = -1;
}

double getPitch_raw(double *samples)
{
	return _computeWaveletPitch(samples);
}

double getPitch_smoothed(double *samples) 
{
	double raw_pitch = _computeWaveletPitch(samples);
	return _dynamicprocess(raw_pitch);
}



