//
//  notedata.h
//  NoteTrainer Mac
//
//  Created by Mark on 29/06/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#ifndef NoteTrainer_Mac_notedata_h
#define NoteTrainer_Mac_notedata_h

typedef struct
{
	float frequency;
	int octave;
	char noteName;
	int accidental;
	int index;			// -- semitones from C0
	int count;			// -- number of cycles note is held	
	int staffPos;		// -- C scale notes or staff positions from C0
	float drift;		// -- cycles off main note
	float MIDINote;		// -- fractional MIDI note
	int nearestMIDINote;// -- whole note for 12 tone instruments
	
} NR_NoteData;

#endif
