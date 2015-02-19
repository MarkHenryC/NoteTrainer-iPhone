//
//  MyViewController.h
//  NoteTrainer iPhone
//
//  Created by Mark Carolan on 21/04/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MainView.h"

@interface MyViewController : UIViewController

@property (strong, nonatomic) IBOutlet MainView *mainView;

-(void)start;
-(void)stop;

@end
