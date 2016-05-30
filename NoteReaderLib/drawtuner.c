//
//  drawtuner.c
//  NoteTrainer Mac
//
//  Created by Mark Carolan on 4/05/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#include <stdio.h>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include "drawtuner.h"
#include "notelookup.h"

// -- index 23..55 in notelookup
// -- 61.74 .. 392 hz
// -- strings indices are - from low E:
// -- 28, 33, 38, 43, 47, 52
// -- draw strings spaced as according to frequency
// -- intervals. This may not look good, but it's
// -- good for initial development as it shows
// -- real frequency spacing

static const int STRING_COUNT = 6;
static const int VIRTUAL_STRING_COUNT = STRING_COUNT+2;
static const int START_STRING_INDEX = 1;
static const int END_STRING_INDEX = 6;
static int guitarStringFreqIndices[6] = { 28, 33, 38, 43, 47, 52 };
static float guitarVirtualStringFreqIndices[VIRTUAL_STRING_COUNT] = { 23, 28, 33, 38, 43, 47, 52, 57 };
										 
// -- drawing data
static const int GUITARSTRING_VERTEX_COUNT = 8;
static const float STRING_THICKNESS = 0.01;
static const float GUITAR_STRING_SPACING = 1.0/(VIRTUAL_STRING_COUNT+1);
static GLfloat guitarStringVertices[GUITARSTRING_VERTEX_COUNT];

static void createGuitarStringVertices()
{
	int i = 0;
	
	guitarStringVertices[i++] =  0;
	guitarStringVertices[i++] =  0;
	guitarStringVertices[i++] =  0;
	guitarStringVertices[i++] =  STRING_THICKNESS;		
	guitarStringVertices[i++] =  1;
	guitarStringVertices[i++] =  0;
	guitarStringVertices[i++] =  1;
	guitarStringVertices[i++] =  STRING_THICKNESS;
}

static void drawGuitarStringVertices()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glColor3f(0, 0, 0);
	for (int i = 0; i < 6; i++)
	{
		glPushMatrix();
		glTranslatef(0, GUITAR_STRING_SPACING + i * GUITAR_STRING_SPACING, 0);
		glVertexPointer(2, GL_FLOAT, 0, guitarStringVertices);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, GUITARSTRING_VERTEX_COUNT);
		glPopMatrix();
	}
    glDisableClientState(GL_VERTEX_ARRAY); 	
}

static float freqForVirtualString(n)
{
	if (n >= 0 && n < VIRTUAL_STRING_COUNT)
	{
		return getFreqAtIndex(guitarVirtualStringFreqIndices[n]);
	}
	else return 0;
}

// -------------- public ----------------------------

void initDrawTuner()
{
	createGuitarStringVertices();
	
}

void updateDrawTuner()
{
	drawGuitarStringVertices();
}

void drawTunerNote(float freq)
{
	double distance = 0;
	int ix = getNoteIndex(freq, &distance);
	float prevStringFreq = 0, nextStringFreq = 0;
	
	if (ix < guitarVirtualStringFreqIndices[0])
	{
		ix = guitarVirtualStringFreqIndices[0];
		nextStringFreq = freqForVirtualString(START_STRING_INDEX); // next is bottom E
		prevStringFreq = freqForVirtualString(0); // previous is self
	}
	else if (ix > guitarVirtualStringFreqIndices[END_STRING_INDEX+1])
	{
		ix = guitarVirtualStringFreqIndices[END_STRING_INDEX+1];
		nextStringFreq = freqForVirtualString(END_STRING_INDEX+1); // next is self
		prevStringFreq = freqForVirtualString(END_STRING_INDEX); // previous is top E
		
	}

	// -- assumptions:
	// -- freq >= first virtual string
	// -- freq <= last virtual string	
	
	float targetFreq = getFreqAtIndex(ix); // -- this is the freq we're closest to
	float distanceToTarget = targetFreq - freq;
	float offsetFromNote; // in freq
	
	for (int n = 0; n < VIRTUAL_STRING_COUNT-1; n++)
	{
		float curFreqSlot = freqForVirtualString(n);
		float nextFreqSlot = freqForVirtualString(n+1); 
		if (freq < nextFreqSlot)
		{
			float span = nextFreqSlot - curFreqSlot;
			float spanNormalised = span/GUITAR_STRING_SPACING;
			float targetOffset = targetFreq-curFreqSlot;
			float ourNoteOffset = freq - curFreqSlot;
//			if (ourNoteOffset < target)
//				offsetFromNote = (target-curFreqSlot)/GUITAR_STRING_SPACING;
		}
		
	}
}