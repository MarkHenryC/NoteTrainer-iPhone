//
//  USTimeTracker.h
//  NoteTrainer-iOS
//
//  Created by Mark on 6/07/12.
//  Copyright (c) 2012 DDD. All rights reserved.
//

#ifndef NoteTrainer_iOS_USTimeTracker_h
#define NoteTrainer_iOS_USTimeTracker_h

#include <sys/time.h>

typedef struct
{
	struct timeval tvStart;
	struct timeval tvCurrent;
	
} USTT_Data;

void ustt_startTimer(USTT_Data *pData);
long ustt_hasElapsed(USTT_Data *pData, long uSecs);
long ustt_elapsedSinceLastCall(USTT_Data *pData);
long ustt_elapsedSinceStart(USTT_Data *pData);
long ustt_hasElapsed(USTT_Data *pData, long uSecsPeriod);

#endif
