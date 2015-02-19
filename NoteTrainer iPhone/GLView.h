//
//  GLView.h
//  NoteTrainer iPhone
//
//  Created by Mark Carolan on 21/04/12.
//  Copyright (c) 2012 DDD Pty Ltd. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>

@interface GLView : UIView
{
	CAEAGLLayer* _eaglLayer;
    EAGLContext* _context;
    GLuint _colorRenderBuffer;
}

-(void)render;

@end
