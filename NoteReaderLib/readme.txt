
Setup info for notereader source files

Include the following:

#include <OpenGL/gl.h>
#import <OpenGL/glu.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#include "readnote.h"
#include "drawnote.h"

Create a buffer for the audio:

double calcBuffer[4096];

You could also set up some default values:

const int DISPLAY_BORDER_H = 40;
const int DISPLAY_BORDER_V = 40;

const int DISPLAY_WIDTH = 1024;
const int DISPLAY_HEIGHT = 660;

const int WINDOW_WIDTH = DISPLAY_WIDTH+DISPLAY_BORDER_H*2;
const int WINDOW_HEIGHT = DISPLAY_HEIGHT+DISPLAY_BORDER_V*2;
const int TIMEFRAME = 125;

You'll need a display function along these lines:

void drawDisplayBuffer()
{
    double frequency = getFrequency();
    
    if (first)
    {
        first = 0;
        frequency = 0.01; // force draw of staff
    }
    
    if (frequency > 0.0)
    {
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        drawMusic(frequency, DISPLAY_WIDTH, DISPLAY_HEIGHT);
                        
        glFlush();
    }
}

And a timer callback function:

void timerFunc(int value) 
{
    if (readBuffer())
    {
        double avg = 0.0;
        double *dBuffer = getDisplayBuffer();
        
        for (int i = 0; i < 4096; i++)
        {
            calcBuffer[i]=dBuffer[i];
            avg += fabs(calcBuffer[i]);
        }
        avg /= 4096;
        const double threshold = 0.01;
        if (avg > threshold)        
            drawDisplayBuffer();
    }   
    glutTimerFunc(TIMEFRAME, timerFunc, 1);
}

Set up a 2D OpenGL context:

void initGL(void)
{    
    glClearColor(0.95, 0.95, 0.95, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, DISPLAY_WIDTH, 0.0, DISPLAY_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
}

If using GLUT, something like this:

int main(int argc, char** argv)
{
    initReadNote();
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH,WINDOW_HEIGHT);
    glutInitWindowPosition(100, 150);
    glutCreateWindow("Guitar note display");    
        
    initGL();            
    
#if TEST
    glutDisplayFunc(testDrawDisplayBuffer);
#else
    glutDisplayFunc(drawDisplayBuffer);    
    glutTimerFunc(TIMEFRAME, timerFunc, 1);
#endif
    
    std::cout << "Entering main loop." << std::endl;
    
    glutMainLoop();
    
    closeReadNote();
}
