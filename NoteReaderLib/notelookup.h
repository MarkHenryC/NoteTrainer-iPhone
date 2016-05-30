//
//  drawnote.h
//
//  Created by Mark Carolan on 19/03/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#ifndef RN_NOTELOOKUP_H
#define RN_NOTELOOKUP_H

#define NOTELOOKUP_ARRAY_SIZE 100

#ifndef SHARP
#define SHARP 1
#endif

#ifndef NATURAL
#define NATURAL 0
#endif

struct NoteLookup
{
	float freqCenter;
	const char *note;
    
};

#ifdef __cplusplus
extern "C" {
#endif
    
void initNoteArray();
int getNoteIndex(double freq, double *pDistance);
int isSharp(int index);
const char *getNoteDescription(int index);
float getFreqAtIndex(int index);

#define MIDDLE_C 48
    
#ifdef __cplusplus
} // extern "C"
#endif

#endif