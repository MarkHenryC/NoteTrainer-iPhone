//
//  GView.m
//  NoteTrainer iPhone
//
//  Created by Mark Carolan on 21/04/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#import "GView.h"

#import "drawnote.h"

//#include <OpenGLES/ES1/gl.h>

@implementation GView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

-(void)awakeFromNib
{
	autoNoteScale = 1.0;
}

-(void)prepareOpenGL
{
	glClearColor(1, 0, 1, 1);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glMatrixMode(GL_MODELVIEW);
	
	glPointSize(2);
	
    printf("prepareOpenGL\n"); 
}

-(void)draw
{
	float prevWidth = viewWidth;
	float prevHeight = viewHeight;
	
	CGRect r = [self frame];
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
	
}

-(void)drawRect:(CGRect)r
{		
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
	
}

-(void)setOctaveShift:(BOOL)flag
{
	setOctave(flag);
	
	[self display];
}

-(void)setIsAutoPlaying:(BOOL)flag
{
	BOOL prevFlag = isAutoPlaying;
	isAutoPlaying = flag;	
	if (isAutoPlaying != prevFlag)
		[self display];
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
		[self display];	
}

-(void)setAutoNoteIndex:(int)i
{
	int prevNote = autoNoteIndex;
	autoNoteIndex = i;
	if (autoNoteIndex != prevNote)
		[self display];
}

-(void)setAutoNoteXPos:(int)i
{
	int prevPos = autoNoteXPos;
	autoNoteXPos = i;
	if (autoNoteXPos != prevPos)
		[self display];
}

-(void)setAutoNoteScale:(float)f
{
	float prevScale = autoNoteScale;
	autoNoteScale = f;
	if (autoNoteScale != prevScale)
		[self display];
}

@end
