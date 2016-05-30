//
//  drawtuner.h
//  NoteTrainer Mac
//
//  Created by Mark Carolan on 4/05/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#ifndef NoteTrainer_Mac_drawtuner_h
#define NoteTrainer_Mac_drawtuner_h

#ifdef __cplusplus
extern "C" {
#endif
	
void initDrawTuner();
void updateDrawTuner();
void drawTunerNote(float freq);
	
#ifdef __cplusplus
} // extern "C"
#endif

#endif
