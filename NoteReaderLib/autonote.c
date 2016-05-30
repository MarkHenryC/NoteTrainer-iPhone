//
//  autonote.c
//  NoteTrainer-iOS
//
//  Created by Mark on 13/07/12.
//  Copyright (c) 2012 DDD. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "autonote.h"
#include "notetable.h"
#include "notedata.h"

static inline int anMax(int i, int j) 
{ 
	return i > j ? i : j; 
}

static inline int anMin(int i, int j) 
{ 
	return i < j ? i : j; 
}

static const int DEFAULT_JUMP_RANGE = 5; // max 5 semitones from current note
static int jumpRange = DEFAULT_JUMP_RANGE;

typedef struct {int low, high;} Range;
static Range noteRanges[rangeCOUNT] = {{28,57},{28,37},{38,46},{47,57}}; // RangeType

// Not using separate table for C notes
//static Range cNoteRanges[rangeCOUNT] = {{16,33},{16,21},{22,26},{27,33}}; // RangeType

typedef enum { dirDown = -1, dirUp = 1 } Direction;
static Direction dirOptions[] = { dirDown, dirUp };

struct _NoteInfo;

typedef void (*NEXTNOTE)(struct _NoteInfo *);

typedef  struct _NoteInfo {
	Direction direction; // determined partly by progType
	ProgType progType; // random, up/down, down/up do both directions
	RangeType rangeType; // all, low, mid, high; index into noteRanges
	NotesType notesType; // 12 semitones or round up/down to chromatic note

	int currentNote;
	
	NEXTNOTE nextNote;
	
} NoteInfo;

// optional runtime reset function
static void noteInfo_init(NoteInfo *inf, Direction dir, 
						  ProgType prog, RangeType range, 
						  NotesType nt, int note)
{
	inf->direction = dir;
	inf->progType = prog;
	inf->rangeType = range;
	inf->notesType = nt;
	inf->currentNote = note;	
}

static void noteInfo_default_init(NoteInfo *inf)
{
	noteInfo_init(inf, dirUp, progRandom, rangeAll, notesCScale, 36);
	
}

static int randomInclusive(int low, int high)
{
	// Get a random number from low..high inclusive
	
	return  low + random() % (1 + high - low);
}

static int randomInclusiveRange(Range range)
{
	// Get a random number from low..high inclusive
	
	return  randomInclusive(range.low, range.high);
}

static Direction randomDirection()
{
	int sz = sizeof(dirOptions)/sizeof(Direction);
	int index = random() % sz;
		
	Direction dir = dirOptions[index];
	printf("Direction %d\n", (int)dir);
	
	return dir;
}

#pragma mark -
#pragma mark NoteInfo routines

// For setting the starting note for a new sequence after
// user has selected a new range or new sequence type,
// or when an oscillating sequence loops.
// Bias is based on direction: e.g. upward direction  
// results in starting point biased toward bottom of range.

static void ni_setBiasedRandomNote(NoteInfo *inf)
{
	Range range = noteRanges[inf->rangeType];
	
	if (inf->direction == dirUp) // bias to lower end by clamping upper bound
		range.high = range.low + jumpRange;
	else if (inf->direction == dirDown) // ditto with lower bound
		range.low = range.high - jumpRange;
	
	inf->currentNote = randomInclusiveRange(range);
}

static void ni_defaultNextNote(NoteInfo *inf)
{
	noteInfo_default_init(inf);
}

static void ni_nextNote(NoteInfo *inf)
{
	// Get the current range we're working in
	Range range = noteRanges[inf->rangeType];
	Range nextNoteRange = range; // random uses full range
	
	// Need to check for repeats when using random
	// notes. I think...
	
	int previousNote = inf->currentNote;
	
	if (inf->direction == dirUp)
	{	
		nextNoteRange.low = inf->currentNote + 1;
		
		//inf->currentNote++; // make sure we dont' repeat note
		
		printf("Going up: range start %d\n", nextNoteRange.low);
		
		// See if we're already at the upper limit
		if (nextNoteRange.low >= range.high) // can't have same start and end range
		{
			// If oscillating sequence, reverse direction
			if (inf->progType == progDownUp || inf->progType == progUpDown)
			{
				inf->direction = dirDown;
				
				// Get range working from interval 
				// backwards from end position.
				// Start from top.
				nextNoteRange.low = range.high - jumpRange;
				nextNoteRange.high = inf->currentNote - 1;
			}
			else if (inf->progType == progUp)
			{
				// Start again at bottom
				nextNoteRange.low = range.low;
				nextNoteRange.high = range.low + jumpRange;
			}
			else if (inf->progType == progRandom)
			{
				// Pick any note from mid
				inf->direction = randomDirection();
				int midRange = (int)((range.high-range.low)/2);
				nextNoteRange.low = range.low + midRange - jumpRange;
				nextNoteRange.high = range.low + midRange + jumpRange;
			}
			else
				assert(0);
		}
		else
		{
			// Just get next note in remaining range space
			nextNoteRange.high = anMin(inf->currentNote + jumpRange, range.high);
		}
	}
	else if (inf->direction == dirDown)
	{				
		//inf->currentNote--;
		
		nextNoteRange.high = inf->currentNote - 1;
		
		printf("Going down: range end %d\n", nextNoteRange.high);
		
		// See if we're already at the lower limit
		if (nextNoteRange.high <= range.low)
		{
			// If oscillating sequence, reverse direction
			if (inf->progType == progDownUp || inf->progType == progUpDown)
			{
				inf->direction = dirUp;
				nextNoteRange.low = inf->currentNote + 1;
				nextNoteRange.high = range.low + jumpRange;
			}
			else if (inf->progType == progDown)
			{
				// Restart at top
				nextNoteRange.low = range.high - jumpRange;
			}
			else if (inf->progType == progRandom)
			{
				// Pick any note from mid
				inf->direction = randomDirection();
				int midRange = (int)((range.high-range.low)/2);
				nextNoteRange.low = range.low + midRange - jumpRange;
				nextNoteRange.high = range.low + midRange + jumpRange;

			}
			else
				assert(0);
		}
		else
		{
			// Just get next note in remaining range space
			nextNoteRange.low = anMax(inf->currentNote - jumpRange, range.low);
			nextNoteRange.high = inf->currentNote;
		}
		
	}
	else
		assert(0);	
	
	printf("nextNoteRange: %d - %d\n", nextNoteRange.low, nextNoteRange.high);
	
	inf->currentNote = randomInclusiveRange(nextNoteRange);

	// This was to prevent note repetition, but I'd forgotten
	// that the current note has already been set +- its 
	// original, and that new figure forms the boundary 
	// of the new range. But this still may apply when
	// random direction is set, although probably not, since
	// now when random is set and note overflows, note
	// starts within jumpRange*2 at centre of overall range.
	
	while(inf->currentNote == previousNote) {
		printf("Preventing duplicates\n");
		inf->currentNote = randomInclusiveRange(nextNoteRange);
	}
	
	if (inf->notesType == notesCScale)
	{
		NR_NoteData nd = nt_noteDataForIndex(inf->currentNote);
		if (nd.accidental != 0) {

			printf("Recursing into nextNote()\n");
			
			inf->currentNote = previousNote; // reset
			inf->nextNote(inf); // recursively try again
		}
	}
	
	if (inf->progType == progRandom)
	{
		inf->direction = randomDirection();
	}
}

static int ni_getCurrentNote(NoteInfo *inf)
{
	return inf->currentNote;
}

static int ni_getNextNote(NoteInfo *inf)
{
	inf->nextNote(inf);

	return ni_getCurrentNote(inf);
}


static void ni_updateDirection(NoteInfo *inf)
{
	switch (inf->progType) {
		case progDown:
		case progDownUp:
			inf->direction = dirDown;	
			break;
		case progUp:
		case progUpDown:
			inf->direction = dirUp;
			break;
		case progRandom:
		default:
			inf->direction = randomDirection();
			break;
	}
}

// use dynamic if multi-instance required
static NoteInfo noteInfo = 
{
	dirUp,
	progRandom,
	rangeAll,
	notesCScale,
	36,
	ni_nextNote // pointer to appropriate function
};

#pragma mark -
#pragma mark Public functions

int an_getCurrentNote()
{
	return ni_getCurrentNote(&noteInfo);
}

int an_getNextNote()
{
	return ni_getNextNote(&noteInfo);
}

void an_setProgType(ProgType type)
{
	noteInfo.progType = type;
	
	ni_updateDirection(&noteInfo);
	
	ni_setBiasedRandomNote(&noteInfo);
}

void an_setRangeType(RangeType type)
{
	noteInfo.rangeType = type;
	
	ni_updateDirection(&noteInfo); // reset to defaults based on current program
	
	ni_setBiasedRandomNote(&noteInfo);
}

void an_setNotesType(NotesType type)
{			
	noteInfo.notesType = type;
}
