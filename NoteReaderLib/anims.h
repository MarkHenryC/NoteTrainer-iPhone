//
//  anims.h
//  NoteTrainer Mac
//
//  Created by Mark Carolan on 16/04/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#ifndef NoteTrainer_Mac_anims_h
#define NoteTrainer_Mac_anims_h

typedef enum { atSpringOut, atSpringIn } AnimType;

typedef struct {
	float startPoint, endPoint, minMag, maxMag;
} AnimParams;

void createSlideInAnimation(float w, float *buffer, float bufferSize);
void createMatchAnimation(float *buffer, float bufferSize);
void createAccelSlideInAnimation(float w, float *buffer, float bufferSize);

#endif
