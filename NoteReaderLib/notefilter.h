//
//  notefilter.h
//  NoteTrainer Mac
//
//  Created by Mark on 28/06/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#ifndef NoteTrainer_Mac_notefilter_h
#define NoteTrainer_Mac_notefilter_h

#include "notedata.h"

void nf_init();
void nf_startTimer();

int nf_isNewNote(NR_NoteData *nd, double freq);

#endif
