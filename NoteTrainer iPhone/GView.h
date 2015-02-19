//
//  GView.h
//  NoteTrainer iPhone
//
//  Created by Mark Carolan on 21/04/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#import <GLKit/GLKit.h>

//@interface GView : GLKView <GLKViewDelegate>

@interface GView <GLKViewDelegate>
{
	BOOL isAutoPlaying;
	BOOL isMatching;
	int userNoteIndex;
	int autoNoteIndex;
	int autoNoteXPos;
	float autoNoteScale;
	float viewWidth, viewHeight;
}

// -- property setters
// -- this view doesn't need to know much about 
// -- what's happening, but it does need to know
// -- if auto play is on so it can draw the extra note
// -- actually it needs to know if it's matching too
// -- so it can do the different color or whatever

-(void)setUserNoteIndex:(int)index;
-(void)setIsAutoPlaying:(BOOL)flag;
-(void)setIsMatching:(BOOL)flag;
-(void)setAutoNoteIndex:(int)noteIndex;
-(void)setAutoNoteXPos:(int)xPos;
-(void)setAutoNoteScale:(float)scale;
-(void)setOctaveShift:(BOOL)flag;
-(void)draw;

@end
