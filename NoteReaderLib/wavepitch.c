//
//  wavepitch.c
//  NoteTrainer Mac
//
//  Created by Mark on 28/06/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#include <stdio.h>
#include <math.h>

static const double fs = 44100.0;
static const int dataLen = 1024; // size of data
static const int LEVELS = 6;
static double maxCount[LEVELS+1];
static double minCount[LEVELS+1];

extern double mean(double *data, int size);

double wavePitch(double *data, double oldFreq)
{
	double oldMode = 0;
	if (oldFreq)
		oldMode = fs/oldFreq;
	
	double freq = 0;
	int lev = 6;
	double globalMaxThresh = 0.75;
	double maxFreq = 3000;
	int diffLevs = LEVELS;

	maxCount[0] = 0;
	minCount[0] = 0;
	
	double *a1 = data; // taken from Matlab a(1,:) vector
	double aver = mean(a1, dataLen);
	double globalMax = max(a1, dataLen);
	double globalMin = min(a1, dataLen);
	double maxThresh = globalMaxThresh*(globalMax-aver) + aver;
	double minThresh = globalMaxThresh*(globalMin-aver) + aver;
	
	for (int i = 2; i <= lev; i++)
	{
		int newWidth = dataLen/ pow(2.0, i-1);
		double j[newWidth]; // j=1:newWidth in Maatlab
		
		for (int n = 0; n < newWidth; n++)
		{
			// aw hell this'll be some work...
		}
	}
}