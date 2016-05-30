//
//  drawnote.h
//
//  Created by Mark Carolan on 19/03/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#ifndef RN_DRAWNOTE_H
#define RN_DRAWNOTE_H

typedef struct {
	float r, g, b, a;
} Rgba;

typedef enum { en_isFlat = -1, en_isNatural = 0, en_isSharp = 1} NoteStatus;

// -- cache dynamic values, calculated each time window size changes
typedef struct {
	int noteWidth, noteHeight; // calculated note dimensions for current window size
	int lineOffset; // thickness of staff line / 2
	int displayWidth, displayHeight; // updated drawing area
	float accidentalScaleX, accidentalScaleY; // scale of sharps & flats
	float symbolXPos, symbolYPosTreble, symbolYPosBass; // for clef symbols
	float symbolScaleX, symbolScaleY;
} StaffMetrics;

#ifdef __cplusplus
extern "C" {
#endif

// -- the init function is called each time the window is resized
// -- which of course includes when the window first appears
void initDrawMusic(int width, int height);
// -- draw the staff with the clef symbols
void drawStaff();
// draw the note being played or the auto-generated note
void drawNote(int index);
// draw the note being played or the auto-generated note
void drawNoteAt(int index, int x, float scale);
// -- display the note in its real position or in guitar format 1 oct high
void setOctave(int octave);
// -- for setting up special notes, such as auto-playing ones
// -- this data is cached so the standard drawNote call is made
void setNoteParams(float xOffset, Rgba rgba, NoteStatus status);
    
#ifdef __cplusplus
} // extern "C"
#endif

#endif
