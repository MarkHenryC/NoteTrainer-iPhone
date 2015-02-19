//
//  MainView.h
//  NoteTrainer iOS
//
//  Created by Mark Carolan on 20/04/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKView.h>

#import "GVDelegate.h"
#import "NotePlayer_iOS.h"

typedef enum { dfDefault = 0, dfSliding = 1, dfMatching = 2 } DrawFlags;
typedef enum { signalNotReady, signalBelowThreshold, signalGood } SignalReturn;

@interface MainView : UIView <UIPickerViewDelegate, UIPickerViewDataSource>
{
	NSTimer *timer;
    
    int display_width, display_height;
	BOOL isPlaying;
	NotePlayer *notePlayer;
	
	double currentFrequency, previousFrequency;
	int currentNoteIndex, previousNoteIndex;
	
	float currentMatchAnimScale;
	int currentSlideAnimIndex;
	int currentMatchAnimIndex;
	
	DrawFlags drawFlags;
	
	GVDelegate *gvd;
}


@property (strong, nonatomic) IBOutlet GLKView *gView;
//@property (strong, nonatomic) IBOutlet UIPickerView *sequencePicker;
@property (strong, nonatomic) IBOutlet UIButton *playButton;
@property (strong, nonatomic) IBOutlet UISwitch *octaveSwitch;
@property (strong, nonatomic) IBOutlet UIProgressView *volumeBar;
@property (strong, nonatomic) IBOutlet UILabel *noteReadout;
@property (strong, nonatomic) IBOutlet UILabel *frequencyReadout;

- (IBAction)octaveClicked:(id)sender;
- (IBAction)playClicked:(id)sender;

// -- audio routines
-(void)startAudio;
-(void)stopAudio;
-(void)close; 
-(SignalReturn)getSignal;
-(SignalReturn)getAverageSignal;

// -- audio info display
-(void)updateAudioDisplayInfo;

// -- main time callback
-(void)timerProc:(NSTimer *)timer;

// -- managing display info
-(void)switchAutoPlay:(BOOL)on;
-(void)calcSlideAnimFrame:(int)noteIndex;
-(void)switchMatchAnim:(BOOL)on;
-(void)calcMatchAnimFrame;

// -- overrides
-(void)awakeFromNib;

@end
