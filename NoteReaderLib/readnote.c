//
//  readnote.c
//
//  Created by Mark Carolan on 22/03/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#include <stdio.h>

#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <math.h>

#include "readnote.h"
#include "getPitch.h"

// -- these all interrelate --
static const int SAMPLERATE = READNOTE_SAMPLERATE;

static ALCdevice *capture;
static ALCdevice *alcDevice;
static ALCcontext *alcContext;

static short captureBuffer[PITCHTRACKER_BUFFER_SIZE];
static double pitchComputeBuffer[PITCHTRACKER_BUFFER_SIZE];

static ALCenum err = 0;

static double frequency = 0.0;
static double averageVolume = 0.0;
static double maxVolume = 0.0;
static double peakVolume = 0.0;

static const char *RNERRSTRINGS[] = 
{
	"No error", 
	"openDevice error", 
	"getString error", 
	"Null device", 
	"captureStart error"
};

static inline double s2d(short s)
{
    double d = (double)s/32767.0;
    if (d < -1.0) d = -1.0;
    return d;
}

static inline double uc2d(unsigned char uc)
{
    return (((double)uc)/255.0)*2.0-1.0;    
}

static inline void zeroPitchComputeBuffer()
{
    for (int i = 0; i < PITCHTRACKER_BUFFER_SIZE; i++)
        pitchComputeBuffer[i] = 0.0;    
}

static inline void updatePitchComputeBuffer()
{
    for (int i = 0; i < PITCHTRACKER_BUFFER_SIZE; i++)
        pitchComputeBuffer[i] = s2d(captureBuffer[i]);
}

static int isAboveThreshold(float threshold)
{
	// -----------------------------------
	// A bit of experimentation here.
	// The averaging and threshold logic is
	// probably wrong because it's linear,
	// which isn't how db works.
	// ------------------------------------
	
	averageVolume = 0.0;
	peakVolume = 0.0;
	
	for (int i = 0; i < PITCHTRACKER_BUFFER_SIZE; i++)
	{
		float dVal = (float) fabs(pitchComputeBuffer[i]);
		
		if (dVal > peakVolume) 
		{
			peakVolume = dVal;
		}
		if (dVal > maxVolume) 
		{
			maxVolume = dVal;				
		}
		averageVolume += dVal;
	}
	averageVolume /= PITCHTRACKER_BUFFER_SIZE;
	
	if (averageVolume >= threshold)
	{
		//		double ratio = 1.0/peakVolume;
		//		for (int i = 0; i < PITCHTRACKER_BUFFER_SIZE; i++)
		//		{
		//			// --  normalise
		//			
		//			pitchComputeBuffer[i] *= ratio;
		//		}
		//		
		//		// -- and normalise average for readout
		//		
		//		averageVolume *= ratio;
		//		
		//		if (averageVolume > maxAverageVolume) 
		//		{
		//			printf("Max average: %f\n", maxAverageVolume);		
		//			maxAverageVolume = averageVolume;
		//		}
		
		return 1;
	}
	else
		return 0;
}

#pragma mark -
#pragma mark public functions

ReadNoteError rn_initReadNote()
{
	// -----------------------------------------------------
	// Firstly open non-capture device.
	// this seems necessary because of a wierd bug in iOS's
	// implementation of OpenAL. We must open a dummy device
	// and make it current, otherwise the call to captureStart
	// (using a separate capture context) will result in an 
	// error of invalid parameter, and - while no further
	// errors are reported - the buffer will not be filled.
	// this applies for the simulator as well, but not OSX
	// UPDATE: as of 5 JUL 12 this no longer seems to apply to
	// the simulator.
	// --------------------------------------------------------
	
	alcDevice = alcOpenDevice(NULL);
	err = alcGetError(alcDevice);
    if (err) {
		printf("alcOpenDevice error: %X\n", err);
		rn_closeReadNote();
		return rnErrOpenDevice;
	}
	
	if (alcDevice)
	{
		alcContext = alcCreateContext(alcDevice, NULL);
		err = alcGetError(alcDevice);
		if (err) 
		{
			printf("alcCreateContext error: %X\n", err);
			rn_closeReadNote();
			return rnErrCreateContext;
		}

		if (alcContext)
		{
			alcMakeContextCurrent(alcContext);
			
			capture = alcCaptureOpenDevice(NULL, SAMPLERATE, AL_FORMAT_MONO16, PITCHTRACKER_BUFFER_SIZE);
			
			err = alcGetError(capture);
			if (err) 
			{
				printf("alcCaptureOpenDevice error: %X\n", err);
				rn_closeReadNote();
				return rnErrOpenDevice;
			}
			
			if (capture)
			{
				const char *dev1 = alcGetString(capture, ALC_DEFAULT_DEVICE_SPECIFIER);
				if (dev1 && *dev1)
					printf("Capture device: %s\n", dev1);
				
				alcCaptureStart(capture);
				
				err = alcGetError(capture);
				if (err) 
				{
					printf("alcCaptureStart error: %X\n", err);
					rn_closeReadNote();
					return rnErrCaptureStart;
				}
				
				// Got this far, should all be OK
				
				zeroPitchComputeBuffer();  
				
				getPitch_init();
				
				return rnOK;

			}
			else
			{		
				printf("alcCaptureOpenDevice returned NULL but no error\n");
				rn_closeReadNote();
				return rnErrNullDevice;
			}			

		}
		else
		{
			printf("alcCreateContext returned NULL but no error\n");
			rn_closeReadNote();
			return rnErrCreateContext;
		}
	}
	else
	{	
		printf("alcOpenDevice returned NULL but no error code.\n");
		return rnErrOpenDevice;
	}		
}

void rn_closeReadNote()
{
    if (capture) {
        alcCaptureStop(capture);
        alcCaptureCloseDevice(capture);
        capture = 0;
    }
	
	// ----------------------------------------------------
	// We only need this for the bug in iOS where capture
	// doesn't work without a regular context open
	// ----------------------------------------------------
	
	if (alcContext) {
		alcDestroyContext(alcContext);
		alcContext = 0;
	}
	
	if (alcDevice) {
		alcCloseDevice(alcDevice);
		alcDevice = 0;
	}
}

int rn_isBufferReady()
{
	ALCint samplesReceived = 0;
	
	alcGetIntegerv(capture, ALC_CAPTURE_SAMPLES, sizeof(ALCint), &samplesReceived);
	
	return (samplesReceived >= PITCHTRACKER_BUFFER_SIZE);
}

double rn_readBufferThreshold(float threshold)
{    
	
#if DEBUG	
	// -- Clear error
    err = alcGetError(capture);    
#endif
	
	alcCaptureSamples(capture, captureBuffer, PITCHTRACKER_BUFFER_SIZE);
	
	// -- convert to doubles, transferring to pitchComputeBuffer			
	updatePitchComputeBuffer();  
	
#if DEBUG						
	ALCenum result = alGetError();
	if (result != ALC_NO_ERROR)
		printf("ALC error: %X", err);			
#endif			
	
	if (isAboveThreshold(threshold))
		frequency = getPitch_raw(pitchComputeBuffer);
	else
		frequency = 0.0;
		
    return frequency;
}

void rn_setErrorFactor(double e)
{
	getPitch_setAcceptedError(e);
}

void rn_setThresholdRatio(double r)
{
	getPitch_setThresholdRatio(r);
}

double rn_getFrequency()
{
    return frequency;
}

double *rn_getDisplayBuffer()
{
    return pitchComputeBuffer;
}

int rn_getBufferSize()
{
	// this can be accessed globally as well, since
	// it's in the top level header readPitch.h. The 
	// reason for this is so static memory can be used
	// and the size is known to clients
	
	return PITCHTRACKER_BUFFER_SIZE;
}

double rn_getAverageVolume()
{
	return averageVolume;
}

double rn_getPeakVolume()
{
	return peakVolume;
}

const char *rn_getRnErrStr(int rne) {
	if (rne >= 0 && rne < RNARRAYSIZE) // make sure not junk data
		return RNERRSTRINGS[rne];
	else
		return "Invalid parameter in getRnErrStr";
}