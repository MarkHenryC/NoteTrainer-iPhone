//
//  NotePlayer.h
//  NoteTrainer
//
//  Created by Mark Carolan on 8/04/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Foundation/NSArray.h>

@interface NotePlayer : NSObject
{
	int currentNoteIndex;
}

-(id)init;
-(int)currentNote;
-(int)nextNote;
//-(void)populateMenu:(NSPopUpButton *)menu;
-(void)setSequence:(int)seq;

@end
