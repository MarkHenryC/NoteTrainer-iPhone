//
//  notetable.h
//  NoteTrainer Shared
//
//  Created by Mark Carolan on 15/05/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

/*
	This module handles converting from frequency to 
	music notes (based on Western 12 tone scale, but 
	it could easily support alternatives). There are
	3 main types of information returned in a
	NR_NoteData structure:
 
	1. nearest note to the supplied frequency, along with deviation info
	2. returning textual data about that note for display
	3. getting note information based on an index of a note
 
	In 1, a frequency is passed in and the structure returned
	stores nearest note as well as deviation frequency.
	As in 2, textual data is always returned. In case 3,
	an index into the 12 tone system, starting with C0, is
	passed in and the corresponding note data is returned. The 
	purpose of this is to generate notes for displaying, based
	on sequence data stored in integers, such as is used in the 
	sequences module.
 
 */

#ifndef NoteTrainer_Shared_notetable_h
#define NoteTrainer_Shared_notetable_h

#include "notedata.h"

#define NOTETABLE_C0 16.35

extern int C_indices[];

void nt_init();
int nt_initialized();
char nt_NoteName(int index);
int nt_isSharp(int index);	
int nt_size();

NR_NoteData nt_noteDataForFreq(float freq);
NR_NoteData nt_noteDataForIndex(int index);
NR_NoteData nt_noteDataEmpty();

int nt_indexAtFreq(float freq);

float nt_freqAtIndex(int index);
float nt_normalizedFrequencyIndex(float freq);	
float nt_getMaxFrequency();
float nt_getMinFrequency();
int nt_getCIndex(int C_number);
int nt_getNearestMIDINote(float freq);
float nt_getFractionalMIDINote(float freq);

#endif
