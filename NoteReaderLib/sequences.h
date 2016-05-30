//
//  sequences.h
//  NoteTrainer
//
//  Created by Mark Carolan on 10/04/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#ifndef NoteTrainer_sequences_h
#define NoteTrainer_sequences_h

typedef struct {
	const char *title;	
	int noteCount;
	int *notes;
} NS_NoteSequence;

void ns_initSequences();
void ns_closeSequences();

// -- how many sequences do we have
int ns_getSequenceCount();

// -- set the current sequence for iteration
void ns_setSequence(int n);

// -- get data from current sequence
int ns_getCurrentSequenceSize();
int ns_getNoteInCurrentSequence(int n);
const char *ns_getTitleOfCurrentSequence();

#endif
