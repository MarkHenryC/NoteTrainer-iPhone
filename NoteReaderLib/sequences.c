//
//  sequences.c
//  NoteTrainer
//
//  Created by Mark Carolan on 10/04/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "sequences.h"

static NS_NoteSequence *noteSequences;

static int currentSequence = -1;
static int sequenceCount = 0;

// -- temp data; needs to be loaded from external source --

const int SEQ_SIZE = 12;
const int NUM_SEQ = 4;

static int seqArray[NUM_SEQ][SEQ_SIZE] = 
{
	{28, 33, 38, 43, 47, 52, 28, 43, 33, 47, 38, 52},
	{36, 40, 43, 41, 45, 48, 47, 43, 45, 38, 40, 38},
	{31, 38, 28, 35, 31, 40, 36, 38, 35, 31, 33, 28},
	{36, 37, 38, 39, 38, 37, 36, 35, 34, 33, 34, 35},	
};

static char *titles[NUM_SEQ] =
{
	"Open strings",
	"C notes",
	"Em b7",
	"C3 semitones",
};

void ns_initSequences()
{
	// this can easily be adapted to dynamic initialisation
	noteSequences = (NS_NoteSequence *) malloc(NUM_SEQ * sizeof(NS_NoteSequence));
	for (int i = 0; i < NUM_SEQ; i++)
	{
		noteSequences[i].title = titles[i];
		noteSequences[i].noteCount = SEQ_SIZE;
		noteSequences[i].notes = seqArray[i];
		
		sequenceCount++;
	}
	
	// important: if dynamic init and count is zero
	// put in dummy data at least in index zero
	if (sequenceCount > 0)
		currentSequence = 0;
	else
	{
		// important: if dynamic init and count is zero
		// put in dummy data at least in index zero
		assert(0);
	}
}

void ns_closeSequences()
{
	free(noteSequences);
}

// -- how many sequences do we have
int ns_getSequenceCount()
{
	return sequenceCount;
}

// -- set the current sequence for iteration
void ns_setSequence(int n)
{
	if (n < sequenceCount)
		currentSequence= n;
}

int ns_getCurrentSequenceSize()
{
	if (currentSequence >= 0)
		return noteSequences[currentSequence].noteCount;
	else
		return 0;
}

int ns_getNoteInCurrentSequence(int n)
{
	if (n < ns_getCurrentSequenceSize())
		return noteSequences[currentSequence].notes[n];
	else
		return 0;		
}

const char *ns_getTitleOfCurrentSequence()
{
	if (currentSequence >= 0)
		return noteSequences[currentSequence].title;
	else
		return "";	
}
