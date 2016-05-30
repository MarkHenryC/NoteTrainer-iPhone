//
//  autonote.h
//  NoteTrainer-iOS
//
//  Created by Mark on 13/07/12.
//  Copyright (c) 2012 DDD. All rights reserved.
//

#ifndef NoteTrainer_iOS_autonote_h
#define NoteTrainer_iOS_autonote_h

typedef enum { 
	progRandom = 0, progUp, progDown, progUpDown, progDownUp, progCOUNT } 
ProgType;

typedef enum { 
	rangeAll = 0, rangeLow, rangeMid, rangeHigh, rangeCOUNT } 
RangeType;

typedef enum { 
	notesCScale = 0, notes12Tone, notesCOUNT } 
NotesType;

int an_getCurrentNote();
int an_getNextNote();

void an_setProgType(ProgType type);
void an_setRangeType(RangeType type);
void an_setNotesType(NotesType type);

#endif
