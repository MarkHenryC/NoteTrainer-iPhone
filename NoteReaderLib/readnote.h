//
//  readnote.h
//
//  Created by Mark Carolan on 22/03/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

/*
	This module sets up OpenAL then handles periodic
	requests for pitch data. It also converts OpenAL's
	short integer audio data to doubles
 */

#ifndef RN_READNOTE_H
#define RN_READNOTE_H

#define READNOTE_SAMPLERATE 44100

typedef enum 
{ 
	rnOK = 0, rnErrOpenDevice, 
	rnErrGetString, rnErrNullDevice, 
	rnErrCaptureStart, rnErrCreateContext,
	RNARRAYSIZE
	
} ReadNoteError;

// -- Initialise
ReadNoteError rn_initReadNote();
// -- Shut-down
void rn_closeReadNote();

// -- get frequency (called in main loop or timer callback)
int rn_isBufferReady();
double rn_readBufferThreshold(float threshold);

// -- Query internal variables
double *rn_getDisplayBuffer();
double rn_getFrequency();
int rn_getBufferSize();
const char *rn_getRnErrStr(int rne);
double rn_getAverageVolume();
double rn_getPeakVolume();
void rn_setErrorFactor(double e);
void rn_setThresholdRatio(double r);

#endif
