//
//  GView.m
//  NoteTrainer iPhone
//
//  Created by Mark Carolan on 21/04/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#import "GVDelegate.h"

#import "drawnote.h"

@implementation GVDelegate

-(id)init
{
	self = [super init];
	if(self) 
	{
	}
	return self;
}

-(void)prepareOpenGL
{
	glClearColor(1, 1, 1, 1);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glMatrixMode(GL_MODELVIEW);
	
	glPointSize(2);
	
	isGLInited = YES;
	
    printf("prepareOpenGL\n"); 
}

-(void)glkView:(GLKView *)view drawInRect:(CGRect)r
{		
	if (!isGLInited)
		[self prepareOpenGL];
	
	float prevWidth = viewWidth;
	float prevHeight = viewHeight;
	
	viewWidth = r.size.width;
	viewHeight = r.size.height;
	
	if (viewWidth != prevWidth || viewHeight != prevHeight)
	{			
		printf("new coords in drawRect w: %f, h: %f\n", r.size.width, r.size.height);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		
		glOrthof(0, viewWidth, 0, viewHeight, -1, 1);
		glViewport(0, 0, viewWidth, viewHeight);
		
		glMatrixMode(GL_MODELVIEW);
		
		initDrawMusic(viewWidth, viewHeight);
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawStaff();
	
	if (isAutoPlaying)
	{
		if(isMatching) // goes green for a match
			glColor4f(0.0f, 0.7f, 0.0f, 1);
		else
			glColor4f(0.0f, 0.0f, 0.0f, 1);
		
		drawNoteAt(autoNoteIndex, autoNoteXPos, autoNoteScale); 
	}
	
	if (userNoteIndex > 0 && !isMatching)
	{
		glColor4f(0.0f, 0.0f, 1.0f, 1);
		drawNote(userNoteIndex);
	}	
	
	glFlush();
	
	needsRedrawing = NO;
	
}

-(void)setOctaveShift:(BOOL)flag
{
	setOctave(flag);
	
	needsRedrawing = YES;
}

-(void)setIsAutoPlaying:(BOOL)flag
{
	BOOL prevFlag = isAutoPlaying;
	isAutoPlaying = flag;	
	if (isAutoPlaying != prevFlag)
		needsRedrawing = YES;
}

-(void)setIsMatching:(BOOL)flag
{
	isMatching = flag;	
}

-(void)setUserNoteIndex:(int)i
{
	int prevNote = userNoteIndex;
	userNoteIndex = i;
	if (userNoteIndex != prevNote)
		needsRedrawing = YES;
}

-(void)setAutoNoteIndex:(int)i
{
	int prevNote = autoNoteIndex;
	autoNoteIndex = i;
	if (autoNoteIndex != prevNote)
		needsRedrawing = YES;
}

-(void)setAutoNoteXPos:(int)i
{
	int prevPos = autoNoteXPos;
	autoNoteXPos = i;
	if (autoNoteXPos != prevPos)
		needsRedrawing = YES;
}

-(void)setAutoNoteScale:(float)f
{
	float prevScale = autoNoteScale;
	autoNoteScale = f;
	if (autoNoteScale != prevScale)
		needsRedrawing = YES;
}

-(BOOL)needsRedrawing
{
	return needsRedrawing;
}

@end
