//
//  ViewController.m
//  NoteTrainer iPhone
//
//  Created by Mark Carolan on 21/04/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#import "ViewController.h"

@interface ViewController ()

@end

@implementation ViewController
@synthesize  mainView;

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
	
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
	
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
}

-(void)start
{
	[mainView startAudio];
}

-(void)stop
{
	[mainView stopAudio];
}

@end
