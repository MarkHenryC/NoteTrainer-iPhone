//
//  MainView.m
//  NoteTrainer iOS
//
//  Created by Mark Carolan on 20/04/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#import "MainView.h"


#import "readnote.h"
#import "notelookup.h"
#import "anims.h"

#import <objc/runtime.h>

const int MATCH_ANIM_LOOP_COUNT = 60;
const int SLIDE_ANIM_LOOP_COUNT = 60;
const double GATE_THRESHOLD = 0.01;

float scaleBuffer[MATCH_ANIM_LOOP_COUNT];
float slideInBuffer[SLIDE_ANIM_LOOP_COUNT];

@implementation MainView

//@synthesize sequencePicker = _sequenceButton;
@synthesize gView = _gView;
@synthesize playButton = _playButton;
@synthesize octaveSwitch = _octaveSelect;
@synthesize noteReadout = _noteDisplay;
@synthesize frequencyReadout = _frequencyDisplay;
@synthesize volumeBar = _volumeBar;

-(void)awakeFromNib
{		
	ReadNoteError rne = rn_initReadNote(); 
	if (rne != rnOK) {
		printf("%s\n", rn_getRnErrStr(rne));
		//infoLabel.text = [NSString stringWithCString:getRnErrStr(rne) encoding:NSASCIIStringEncoding];
	}   	
	initNoteArray();
	
	currentFrequency = previousFrequency = 0.0;
	currentNoteIndex = previousNoteIndex = 0;
	
	[_octaveSelect setOn:FALSE];
	
	notePlayer = [[NotePlayer alloc] init];
	//	[notePlayer populateMenu:_sequenceButton];			
	
	// -- used by both slide-in anim and match anim (separately of course)
	// -- actually now both anims can run simultaneously, so each would
	// -- be jumping by 2 increments; probably makes sense: if the user's
	// -- quick enough to match a note before it arrives they'll probably
	// -- get impatient. And they probably won't need this program...
	
	currentSlideAnimIndex = 0;	
	currentMatchAnimIndex = 0;
	currentMatchAnimScale = 1.0f;	
	isPlaying = NO;
	
	drawFlags = dfDefault;
	
	[_playButton setTitle:@"Pause" forState:UIControlStateSelected];
	[_playButton setTitle:@"Play" forState:UIControlStateNormal];
	
//	noteTextDisplay.text = @"hello";
	
//	[_sequenceButton setDelegate:self];
//	[_sequenceButton setDataSource:self];
//	[_sequenceButton reloadAllComponents]; 
	
	timer = [NSTimer timerWithTimeInterval:(NSTimeInterval)(1.0f/60.0f) 
									target:self selector:@selector(timerProc:) userInfo:nil repeats:YES];	
	
	gvd = [[GVDelegate alloc] init];
	
	_gView.delegate = gvd;
	
	_gView.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
}

// -- audio info display -------------------------

-(void)updateAudioDisplayInfo
{
	if (currentNoteIndex != previousNoteIndex) 
	{
		const char *noteDescription = getNoteDescription(currentNoteIndex);
		
//		_frequencyDisplay.text = [NSString stringWithFormat:@"%3.2f", currentFrequency];
		_noteDisplay.text = [NSString stringWithCString:noteDescription
												  encoding:NSASCIIStringEncoding];		
	}
}

-(void)switchAutoPlay:(BOOL)on
{
	// -- this is not doubling-up, because we can 
	// -- have a state where we're playing, but the 
	// -- mode is enMatching, where an animation
	// -- shows the note has matched before moving on
	
	isPlaying = on;
	
	[gvd setIsAutoPlaying:isPlaying];
	currentSlideAnimIndex = 0;
	
	if (isPlaying)
	{
		drawFlags |= dfSliding;
		
		[gvd setAutoNoteIndex:[notePlayer currentNote]];
		[gvd setAutoNoteXPos:slideInBuffer[currentSlideAnimIndex]];		
	}
	else
	{
		drawFlags &= ~dfSliding;
		drawFlags &= ~dfMatching;
		
		[gvd setAutoNoteXPos:0];
	}
	
}

-(void)calcSlideAnimFrame:(int)noteIndex
{
	if (currentSlideAnimIndex >= SLIDE_ANIM_LOOP_COUNT)
	{
		printf("slide animation ended\n");
		
		currentSlideAnimIndex = 0; // -- animation ended
		drawFlags &= ~dfSliding;
		
		[gvd setAutoNoteXPos:0];
	}
	else	
		[gvd setAutoNoteXPos:slideInBuffer[currentSlideAnimIndex++]];
}

-(void)calcMatchAnimFrame
{
	if (currentMatchAnimIndex >= MATCH_ANIM_LOOP_COUNT)
	{
		[self switchMatchAnim:NO];		
		
		[gvd setAutoNoteScale: 1.0];
		
	}	
	else
		[gvd setAutoNoteScale: 1.0+scaleBuffer[currentMatchAnimIndex++]];
}

-(void)switchMatchAnim:(BOOL)on
{
	[gvd setIsMatching:on];
	
	if (on)
	{
		drawFlags |= dfMatching;
		drawFlags &= ~dfSliding; // -- stop sliding
		currentMatchAnimIndex = 0;	
		
		printf("match animation on\n");
	}
	else
	{
		printf("match animation off\n");
		
		drawFlags &= ~dfMatching;
		
		if (isPlaying)
		{
			printf("getting new note\n");
			
			// -- animation ended after a match
			// -- fetch next note in list
			
			[notePlayer nextNote];
			
			drawFlags |= dfSliding;
			currentSlideAnimIndex = 0;
			currentMatchAnimIndex = 0;
			
			[gvd setAutoNoteIndex:[notePlayer currentNote]];
			[gvd setAutoNoteXPos:slideInBuffer[currentSlideAnimIndex]];
			
		}
		else
		{
			// -- this part's probably not necessary
			drawFlags &= ~dfSliding;
		}
	}
}

// -- audio routines ----------------------

- (NSDictionary *)userInfo {
    return [NSDictionary dictionaryWithObject:[NSDate date] forKey:@"StartDate"];
}

-(void)startAudio
{
	timer = [NSTimer timerWithTimeInterval:1
									target:self 
								  selector:@selector(timerProc:) 
								  userInfo:nil repeats:YES];
    
	[[NSRunLoop currentRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode];
}

-(void)stopAudio
{
    [timer invalidate];
	
	timer = nil;
}

-(void)close
{
    [self stopAudio];
    
    rn_closeReadNote();
}

-(SignalReturn)getSignal
{	
    if (rn_readBuffer())
    {
		// -- above noise gate
		currentFrequency = rn_getFrequency();
		currentNoteIndex = rn_getNoteIndex(currentFrequency, 0);	
		
		if (currentNoteIndex > 0)
			return signalGood;
		else 
			return signalBelowThreshold;
    }
	else
		return signalNotReady;
	
	// -- don't fail if buffer not read, as it may just be
	// -- that samples haven't filled buffer yet
}

-(SignalReturn)getAverageSignal
{	
    if (rn_readBuffer())
    {
		// -- get average magnitude		
        double avg = 0.0;
        double *dBuffer = rn_getDisplayBuffer();
        int sz = rn_getBufferSize();
		
        for (int i = 0; i < sz; i++)
            avg += fabs(dBuffer[i]);
		
        avg /= sz;       
		
		if (avg > GATE_THRESHOLD)        
		{
			// -- above noise gate
			currentFrequency = rn_getFrequency();
			currentNoteIndex = rn_getNoteIndex(currentFrequency, 0);	
			
			_volumeBar.progress = avg;
			
			return signalGood;
		}
		else
		{
			// -- below threshold, zero out
			currentFrequency = 0.0;
			currentNoteIndex = 0;
			
			return signalBelowThreshold;
		}
    }
	else
		return signalNotReady;
	
	// -- don't fail if buffer not read, as it may just be
	// -- that samples haven't filled buffer yet
}

-(void)timerProc:(NSTimer *)timer
{	
	SignalReturn ret = [self getAverageSignal];
	
	switch(ret)
	{	
		case signalGood:		
			// -- is it a new note?
			if (currentNoteIndex != previousNoteIndex)
			{
				[self updateAudioDisplayInfo];
				
				[gvd setUserNoteIndex:currentNoteIndex];
				
				previousFrequency = currentFrequency;
				previousNoteIndex = currentNoteIndex;
				
				if (isPlaying)
				{
					if (currentNoteIndex > 0)
					{
						if (currentNoteIndex == [notePlayer currentNote]) // -- matched
						{
							if (!(drawFlags & dfMatching))
								[self switchMatchAnim:YES];
						}
					}
				}
			}
		case signalBelowThreshold:
			// -- not loud enough; end of note presumed
			// -- let gfx know that note's finished
			[gvd setUserNoteIndex:currentNoteIndex];
			break;
		case signalNotReady:
			// -- don't do anything; just wait till buffer is filled
			break;
	}
	
	if (drawFlags & dfSliding)
		[self calcSlideAnimFrame:[notePlayer currentNote]];
	
	if (drawFlags & dfMatching)
		[self calcMatchAnimFrame];
	
	if ([gvd needsRedrawing])
		[_gView setNeedsDisplay];
}

-(void)drawRect:(CGRect)rect
{
	// -- Cocoa lessons for me:
	// -- This is called by whatever UI element needs drawing.
	// -- Since you can't tell what element is calling it like
	// -- the main view or a button, we need to explicitly call
	// -- for the frame rect of this view, otherwise we send
	// -- possibly just a button's dimensions to the animation
	// -- routine. It's OK to use the drawRect rect passed to
	// -- the GLView, as it doesn't contain subviews, and so
	// -- we get the entire area.
	
	CGRect r = [_gView frame];
	createSlideInAnimation(r.size.width, slideInBuffer, SLIDE_ANIM_LOOP_COUNT);
	//	createAccelSlideInAnimation(r.size.width, slideInBuffer, SLIDE_ANIM_LOOP_COUNT);
	createMatchAnimation(scaleBuffer, MATCH_ANIM_LOOP_COUNT);
	
	[_gView setNeedsDisplay];
}

// -- interface handlers ----------------------

//- (BOOL)performKeyEquivalent:(NSEvent *)theEvent
//{
//	NSEventType type = [theEvent type];
//	
//	if (theEvent.keyCode == 35 && type == NSKeyDown && ![theEvent isARepeat])
//	{
//		if (!isPlaying)
//			[_playButton setTitle:@"Pause"];
//		else
//			[_playButton setTitle:@"Play"];
//		
//		isPlaying = !isPlaying;
//		
//		[self switchAutoPlay:isPlaying];		
//		
//		return YES;
//	}
//	return NO;
//}

- (IBAction)octaveClicked:(id)sender {	
    UISwitch *btn = (UISwitch *)sender;
    
    [gvd setOctaveShift:[btn state]];
}

- (IBAction)playClicked:(id)sender {
	UIButton *button = (UIButton *)sender;
	
	isPlaying = !isPlaying;
	
	[self switchAutoPlay:isPlaying];
	
	if (isPlaying)
		[button setTitle:@"Pause" forState:UIControlStateSelected];
	else
		[button setTitle:@"Play" forState:UIControlStateNormal];	
	
}

- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component
{
	
}

- (CGFloat)pickerView:(UIPickerView *)pickerView rowHeightForComponent:(NSInteger)component
{
	return 40;
}

- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
	return @"YES!";
}

- (CGFloat)pickerView:(UIPickerView *)pickerView widthForComponent:(NSInteger)component
{
	return 200;
}

- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView
{
	return 1;
}

- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
	return 4;
}

//- (IBAction)sequenceSelected:(id)sender
//{
//	//	const char *str = class_getName([sender class]);
//	
//	NSPopUpButton *button = (NSPopUpButton *)sender;
//	int item = [button indexOfSelectedItem];
//	
//	isPlaying = NO;
//	[_playButton setTitle:@"Play"];
//	
//	[self switchAutoPlay:NO];
//	
//	[notePlayer setSequence:item];
//}

@end
