//
//  NoteFilter.c
//  NoteTrainer Mac
//
//  Created by Mark on 28/06/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//
//  Update 20 JUL 2012: removed dependency on readnote.c
//	This is for tidiness but mainly so I can reuse this 
//	code with non-OpenAL audio capture. OAL seems to be
//	very broken on iPhone 4 but works OK on 3GS.
//

#include <stdio.h>
#include <sys/time.h>

#include "notetable.h"
#include "notefilter.h"

static const double GATE_THRESHOLD = 0.003;
static const double	PITCH_HOLD_TIME	= 62500.0; // .0625 of a second in microseconds
static struct timeval tv;

struct NF_NoteTracker
{
	int heldNoteIndex;
	int newIndex;
	long newIndexStartTimeS, newIndexStartTimeU; //seconds & microseconds
	
} tracker = {0, 0, 0.0};

void nf_init() // this isn't compulsory
{
	tracker.newIndex = 0;
	tracker.heldNoteIndex = 0;
}

void nf_startTimer()
{
	gettimeofday(&tv, NULL);
	tracker.newIndexStartTimeS = tv.tv_sec;
	tracker.newIndexStartTimeU = tv.tv_usec;
}

int nf_isNewNote(NR_NoteData *nd, double freq)
{	
	// get the nearest semitone index from C0
	int index = nt_indexAtFreq(freq);
	
	// get time elapsed: check for microseconds overflow
	gettimeofday(&tv, NULL);	
	long nowTimeS = tv.tv_sec; 
	long nowTimeU = tv.tv_usec;
	long elapsedMicroseconds;
	
	// see if we've ticked over a second and prevent wrapping microseconds
	if (nowTimeS > tracker.newIndexStartTimeS)
	{		
		// assume we haven't snuck more than a second into the time frame
		elapsedMicroseconds = (1000000-tracker.newIndexStartTimeU)+nowTimeU;
	}
	else if (nowTimeS == tracker.newIndexStartTimeS) 
	{
		elapsedMicroseconds = nowTimeU - tracker.newIndexStartTimeU;
	}
	else
	{
		printf("Underlap...WeiRd...");
		return 0;
	}
	
	if (index != tracker.newIndex) // -- new note since last pass
	{						
		tracker.newIndex = index;
		tracker.newIndexStartTimeS = nowTimeS;
		tracker.newIndexStartTimeU = nowTimeU;
		
//		printf("new note %d\n", index);
	}
	else if (elapsedMicroseconds >= PITCH_HOLD_TIME) // -- held note
	{		
		// don't keep registering as new note
		if (tracker.heldNoteIndex != tracker.newIndex)
		{
			tracker.heldNoteIndex = tracker.newIndex;
						
			// TODO: perhaps accumulate time duration for note sustain
						
			if (tracker.heldNoteIndex > 0)
				*nd = nt_noteDataForFreq(freq);
			else
				*nd = nt_noteDataEmpty();
			
			return 1; // zero is also "new note" signifying pause
		}
		
		// don't keep entering this section; set new timeout
		tracker.newIndexStartTimeS = nowTimeS;
		tracker.newIndexStartTimeU = nowTimeU;
	}
	return 0;
}
