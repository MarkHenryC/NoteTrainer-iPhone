//
//  notetable.c
//  NoteTrainer Shared
//
//  Created by Mark Carolan on 15/05/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "notetable.h"

// -----------------------------------------------------------------------
// Dynamic creation of frequency table. Is there any point, as opposed
// to a lookup table? Maybe not.
// Let's say 7 octaves will do
// -----------------------------------------------------------------------

static int noteDataInited = 0;

static const char noteNames[] = { 'C', 'C', 'D', 'D', 'E', 'F', 'F', 'G', 'G', 'A', 'A', 'B' };
static const int sharps[] = { 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0 };

static const float FREQSTART = NOTETABLE_C0;
static const int OCTAVES = 7; // -- Range C0 .. B6 inclusive
static const int SEMITONES = 12; // -- der
static const int CNOTES = 7; // -- double der
static const int NOTEDATA_SIZE = OCTAVES*SEMITONES;
static const int CNOTEDATA_SIZE = OCTAVES*CNOTES;

static double TWELVEROOT2;

// -----------------------------------------------------------------------
// -- The main lookup table with frequency, note name, accidental and 
// -- C0 offset indices.

static NR_NoteData noteData[NOTEDATA_SIZE]; // data on all semitones from c0
static int cNoteLookupTable[CNOTEDATA_SIZE]; // index of all naturals in noteData

static NR_NoteData createNoteData(float frequency, int octave, char noteName, 
							   int accidental, int index, int staffPos, 
								  float drift, int nearestMIDI)
{
	NR_NoteData nd;
	
	nd.frequency = frequency;
	nd.octave = octave;
	nd.noteName = noteName;
	nd.accidental = accidental;
	
	nd.index = index;
	nd.count = 0;
	nd.staffPos = staffPos;
	nd.drift = drift;
	nd.MIDINote = 0.0;
	nd.nearestMIDINote = nearestMIDI;
	
	return nd;
}

static NR_NoteData createVoidNoteData()
{
	return createNoteData(0, 0, ' ', 0, 0, 0, 0, 0);
}

static NR_NoteData copyNoteData(NR_NoteData src)
{
	return createNoteData(src.frequency, src.octave, src.noteName, 
						  src.accidental, src.index, src.staffPos, src.drift,
						  src.nearestMIDINote);
}

static NR_NoteData copyNoteDataCreateDrift(NR_NoteData src, float drift)
{
	NR_NoteData nd = copyNoteData(src);
	nd.drift = drift;
	
	return nd;
}

// ----------------
// public functions
// ----------------

void nt_init()
{	
	TWELVEROOT2 = pow(2.0, 1.0/12.0); // -- 12th root of 2
	
	int staffPosIndex = -1; // -- draw pos from C0. Must start below zero unless starting with sharp
	int cNoteDataIndex = 0;
	
	for (int i = 0; i < OCTAVES; i++)
	{
		for (int j = 0; j < SEMITONES; j++)
		{
			int semitoneIndex = i * SEMITONES + j;
			int noteNameIndex = j % SEMITONES; // -- 0..11
			
			char noteName = noteNames[noteNameIndex];
			int sharp = sharps[noteNameIndex];
			
			if (!sharp) {				
				staffPosIndex++;
				cNoteLookupTable[cNoteDataIndex++] = semitoneIndex;
			}
			
			// -- frequency of each semitone from FREQSTART
			float frequency = FREQSTART * pow(TWELVEROOT2, semitoneIndex);
			int MIDINote = nt_getNearestMIDINote(frequency);
			
			noteData[semitoneIndex] = createNoteData( 													 
				frequency,
				i,				// -- octave number
				noteName,		// -- C..B
				sharp,			// -- 1 if sharp, 0 otherwise
				semitoneIndex,	// -- handy for quick retrieval
				staffPosIndex,	// -- where to draw it
				0.0,			// -- calculated at runtime based on detected freq
				MIDINote		// -- static value based on freq via (integer) semitone index
			);
		}		
		
	}
	
	noteDataInited = 1;
}

int nt_initialized()
{
	return noteDataInited;
}

char nt_NoteName(int index)
{
	assert(index >= 0 && index < (NOTEDATA_SIZE-1));
	if (!(index >= 0 && index < (NOTEDATA_SIZE-1)))
		return ' ';
	
	return noteData[index].noteName;
}

int nt_isSharp(int index)
{
	assert(index >= 0 && index < (NOTEDATA_SIZE-1));
	if (!(index >= 0 && index < (NOTEDATA_SIZE-1)))
		return -1;
	
    return noteData[index].accidental == 1;
}

float nt_freqAtIndex(int index)
{
	assert(index >= 0 && index < (NOTEDATA_SIZE-1));
	if (!(index >= 0 && index < (NOTEDATA_SIZE-1)))
		return 0.0;
	
	return noteData[index].frequency;
}

float nt_normalizedFrequencyIndex(float freq)
{
	if (freq >= FREQSTART && freq <= noteData[NOTEDATA_SIZE-1].frequency)
		return log(freq/FREQSTART) / log(TWELVEROOT2);	
	else
		return 0.0;
}

int nt_size() // -- number of notes in this array
{
	return NOTEDATA_SIZE;
}

int nt_getCIndex(int C_number) // -- convenience function to grab appropriate C offset
{
	assert(C_number < OCTAVES); // -- 0 .. 6
	
	return C_number*SEMITONES;
}

// A new addition: MIDI note
// MidiNote = 69 + 12 * log2 (freq/440)
// -------------------------------------

float nt_getFractionalMIDINote(float freq)
{
	return (float) (69.0 + 12.0 * log2(freq/440.0));
}

int nt_getNearestMIDINote(float freq)
{
	return (int) ((69.0 + 12.0 * log2(freq/440.0) + 0.5));
}

int nt_indexAtFreq(float freq)
{	
	if (freq == 0.0) return 0;
	
	float lowestFreq = noteData[0].frequency;
	float highestFreq = noteData[NOTEDATA_SIZE-1].frequency;
	
    if (freq < lowestFreq) // -- return lowest note
    {
		return 0;
	}
	else if (freq > highestFreq) // -- return highest note
	{
		return 0;
	}
	else
	{		
		for (int i = 1; i < NOTEDATA_SIZE-1; i++)
		{
			float prevNote = noteData[i-1].frequency;
			float thisNote = noteData[i].frequency;
			float nextNote = noteData[i+1].frequency;
			
			if (freq > prevNote && freq < nextNote)
			{
				// -----------------------------------------------------
				// TODO: this needs to be improved to account for
				// drift in frequency spacing, as it's not linear
				// OR: maybe it's not necessary here, as caller
				// can do a rescaled lookup of source and target 
				// frequency to place on screen for tuner etc.
				// ------------------------------------------------------
				
				float halfDistanceUp = (nextNote-thisNote)/2.0;
				float halfDistanceDown = (thisNote-prevNote)/2.0;
				
				int targetIndex;
				
				if (freq >= prevNote+halfDistanceDown)
				{
					if (freq < nextNote-halfDistanceUp)
						targetIndex = i;
					else
						targetIndex = i+1;
				}
				else
					targetIndex = i-1;
				
				return targetIndex;
			}
		}
		assert(0); // shouldn't get here; if freq is outside array it should have been caught at top
	}
}

NR_NoteData nt_noteDataEmpty()
{
	return createVoidNoteData();
}

NR_NoteData nt_noteDataForIndex(int index)
{
	return noteData[index];
}

NR_NoteData nt_noteDataForFreq(float freq)
{	
	if (freq == 0) return createVoidNoteData();
	
	float lowestFreq = noteData[0].frequency;
	float highestFreq = noteData[NOTEDATA_SIZE-1].frequency;
	
    if (freq < lowestFreq) // -- return lowest note
    {
		// -- let caller know how far below minimum note frequency is
		
		return copyNoteDataCreateDrift(noteData[0], lowestFreq-freq); // -- negative offset		
	}
	else if (freq > highestFreq) // -- return highest note
	{
		// -- let caller know how far above maximum note frequency is
		
		return copyNoteDataCreateDrift(noteData[NOTEDATA_SIZE-1], freq-highestFreq); // -- positive offset
	}
	else
	{		
		for (int i = 1; i < NOTEDATA_SIZE-1; i++)
		{
			float prevNote = noteData[i-1].frequency;
			float thisNote = noteData[i].frequency;
			float nextNote = noteData[i+1].frequency;
			
			if (freq > prevNote && freq < nextNote)
			{
				// -- TODO: ditto as for previous TODO
				
				float halfDistanceUp = (nextNote-thisNote)/2.0;
				float halfDistanceDown = (thisNote-prevNote)/2.0;
				
				int targetIndex;
				
				if (freq >= prevNote+halfDistanceDown)
				{
					if (freq < nextNote-halfDistanceUp)
						targetIndex = i;
					else
						targetIndex = i+1;
				}
				else
					targetIndex = i-1;
				
				NR_NoteData nd = copyNoteDataCreateDrift(noteData[targetIndex], freq-thisNote);
				nd.frequency = freq; // -- keep original frequency
				nd.MIDINote = nt_getFractionalMIDINote(freq);
				return nd;
			}
		}
		assert(0); // shouldn't get here; if freq is outside array it should have been caught at top
	}
}
