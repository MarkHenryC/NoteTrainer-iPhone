//
//  NotePlayer.m
//  NoteTrainer
//
//  Created by Mark Carolan on 8/04/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#import "NotePlayer_iOS.h"

#import <objc/runtime.h>

#include <stdlib.h>

#include "sequences.h"

const int range = 64-28;
const int start = 28;

const int MAX_ARRAYS = 2;

@implementation NotePlayer

-(id)init
{
	self = [super init];
	if(self) 
	{
//		sequences = [NSMutableArray array];
//		
//		for (int arrays = 0; arrays < MAX_ARRAYS; arrays++)
//		{
//			NSMutableArray *notes = [NSMutableArray array];
//			
//			for (int i = 0; i < 30; i++)
//			{
//				int r = (random() % range) + start;
//				[notes addObject:[NSNumber numberWithInt:r]];
//			}
//			
//			[sequences addObject:notes];
//		}		
		currentNoteIndex = 0;
		
		initSequences();
	}
	return self;
}

- (void)dealloc
{
	closeSequences();	
}

-(int)currentNote
{
	return getNoteInCurrentSequence(currentNoteIndex);
}

-(int)nextNote
{
	if(++currentNoteIndex >= getCurrentSequenceSize())
		currentNoteIndex = 0;
	
	return [self currentNote];
}

//-(void)populateMenu:(NSPopUpButton *)menu
//{
////	const char *str = class_getName([menu class]);
//	
//	int cnt = getSequenceCount();
//	
//	for (int i = 0; i < cnt; i++)
//	{
//		setSequence(i);
//		
//		[menu insertItemWithTitle:[NSString stringWithCString:getTitleOfCurrentSequence() encoding:NSASCIIStringEncoding] atIndex:i];
//	}	
//	
//	setSequence(0); // -- point to first sequence
//}

-(void)setSequence:(int)seq
{
	setSequence(seq);
		
	// -- reset note index
	currentNoteIndex = 0;
}
@end
