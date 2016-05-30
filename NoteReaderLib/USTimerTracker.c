//
//  USTimerTracker.c
//  NoteTrainer-iOS
//
//  Created by Mark on 6/07/12.
//  Copyright (c) 2012 DDD. All rights reserved.
//

#include <stdio.h>
#include "USTimeTracker.h"

// Formula:
// ((B_sec - A_sec) * 1000000) - (1000000 - A_uSec) + B_uSec

// Some of the assumptions here may not be an issue. I've done the diff
// function so there's no chance of overflow in the microsecond field.
// But if we use as a base, say, when the timer is first started, then
// the microsecond field - even on a 32 bit system - will hold
// 2,147 (K micro) seconds; over half an hour. So we can safely simplify things
// by doing all maths in the microseconds (uSec) domain.

// This one's pretty good for avoiding overflow, but unnecessary
// if using time differences less than half an hour.
static inline long getDiff(struct timeval B, struct timeval A)
{
	long B_sec = B.tv_sec;
	long B_uSec = B.tv_usec;
	long A_sec = A.tv_sec;
	long A_uSec = A.tv_usec;
	
	return ((B_sec - A_sec) * 1000000) - (1000000 - A_uSec) + B_uSec;
}

//
// A little search revealed this handy routine for returning the result
// in a timeval struct.
// http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
//
//
// Subtract the `struct timeval' values X and Y,
// storing the result in RESULT.
// Return 1 if the difference is negative, otherwise 0.
//

static int timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y)
{
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_usec < y->tv_usec) {
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000) {
		int nsec = (x->tv_usec - y->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}
	
	/* Compute the time remaining to wait.
	 tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;
	
	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}

void ustt_startTimer(USTT_Data *pData)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	
	pData->tvStart = tv;
	pData->tvCurrent = tv;
}

long ustt_elapsedSinceLastCall(USTT_Data *pData)
{
	struct timeval tvNow;
	gettimeofday(&tvNow, NULL);
	
	long diff = getDiff(tvNow, pData->tvCurrent);
	pData->tvCurrent = tvNow;
	
	return diff;
}

long ustt_elapsedSinceStart(USTT_Data *pData)
{
	struct timeval tvNow;
	gettimeofday(&tvNow, NULL);	
	
	long diff = getDiff(tvNow, pData->tvStart);
	
	return diff;
}

long ustt_hasElapsed(USTT_Data *pData, long uSecsPeriod)
{
	struct timeval tvNow;
	gettimeofday(&tvNow, NULL);	
	
	long elapsed = getDiff(tvNow, pData->tvCurrent);
	
	if (elapsed >= uSecsPeriod)
	{
		// set current timeval to time when
		// last period elapsed
		
		long diff = elapsed - uSecsPeriod;
		
		pData->tvCurrent = tvNow;
			
		return diff;
	}
	
	return 0;
}
