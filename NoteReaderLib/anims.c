//
//  anims.c
//  NoteTrainer Mac
//
//  Created by Mark Carolan on 16/04/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#include <stdio.h>
#include <math.h>

#include "anims.h"

static const float MYPI = (22.0/7.0);

void createAnim(AnimType type, AnimParams params,
				float *linearBuffer, int bufferLength)
{
}

void createWobble(float restPos, float range, float cycles, float *buffer, float size)
{
	float step = 1.0/size;
	float x = 0;
	
	for (int i = 0; i < size; i++)
	{
		buffer[i] = restPos + range * (-sinf(cycles*MYPI*x*x) / (MYPI*x+1));
		x += step;
		
//		printf("x: %f, y: %f\n", x, buffer[i]);
	}
}

void createSlideInAnimation(float w, float *buffer, float size)
{	
	int bufferSize = (int)(size*0.7);
	int rem = size-bufferSize;
	
	// -- initialise the xPos steps buffer
	// -- x : p4 .. 0 
	float p4 = MYPI/4.0;
	
	float decr = p4/bufferSize;	
	float dcntr = p4;
	for (int i = 0; i < bufferSize; i++)
	{
		buffer[i] = sinf(dcntr)*w*10; // why *10? Not sure. Just works.
		
		dcntr -= decr;
	}
	
	if (rem > 0)
		createWobble(0, w*.05, 8, buffer+bufferSize, rem);		
}

void createMatchAnimation(float *buffer, float bufferSize)
{		
	createWobble(0.2, 0.8, 16, buffer, bufferSize);
}

void createMatchAnimation0(float *buffer, float bufferSize)
{		
	// -- initialise the scale steps buffer
	// -- x : 0 .. PI/3
	float p3 = MYPI/3.0;
	
	float incr = p3/(float)bufferSize;
	float cntr = 0.0;
	for (int i = 0; i < bufferSize; i++)
	{
		buffer[i] = sinf(cntr*3.0);		
		
		cntr += incr;
	}		
}

void createAccelSlideInAnimation(float w, float *buffer, float bufferSize)
{			
	float xResolution = 2.0/bufferSize;
	float x = 1.0;
		
	// -- input range x: 1.0 --> -1.0
	// -- output range y: 1.0 --> -1.0
	
	for (int i = 0; i < bufferSize; i++)
	{
		buffer[i] = ((atanf(10.0*x)/1.5)+.98)*w/4; // -- uses half screen; scale is +2; thus /4
		x -= xResolution;
	}	
}

void createBouncySlideInAnimation(float w, float *buffer, float bufferSize)
{	
	// -- initialise the xPos steps buffer
	// -- x : p4 .. 0 
	
	float p4 = MYPI/4.0;
	
	float decr = p4/bufferSize;	
	float dcntr = p4;
	for (int i = 0; i < bufferSize; i++)
	{
		buffer[i] = sinf(dcntr)*w*10; // why *10? Not sure. Just works.
		
		dcntr -= decr;
	}
	
}

void createBouncyMatchAnimation(float *buffer, float bufferSize)
{		
	// -- initialise the scale steps buffer
	// -- x : 0 .. PI/3
	float p3 = MYPI/3.0;
	
	float incr = p3/(float)bufferSize;
	float cntr = 0.0;
	for (int i = 0; i < bufferSize; i++)
	{
		buffer[i] = sinf(cntr*3.0);		
		
		cntr += incr;
	}		
}