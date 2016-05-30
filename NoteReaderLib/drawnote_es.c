#include <OpenGLES/ES1/gl.h>

#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "drawnote.h"
#include "notelookup.h"

#include "bass.h"
#include "treble.h"
#include "sharp.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

extern double notesInited;

static const int B1 = 23;
static const int E2 = 28;
static const int E4 = 52;
static const int E5 = 64;
static const int D6 = 74;
static const int F6 = 77;

static const int BEGIN_NOTE = B1;
static const int END_NOTE = F6;

static const int STAFFLINECOUNT = 16;
static const int STAFFNOTEPOSITIONS = STAFFLINECOUNT*2+1;
static const int TOTALNOTES = 1+END_NOTE-BEGIN_NOTE;

// -- for gl vertex array data --

static const int VERTEX_SIZE = 2; // 2D vertices

// -- Ellipse consists of: (1) centre point (2) top point then 4 * points-per-quadrant (CCW)
static const int VERTICES_PER_QUADRANT = 5;
static const int NOTEDATAVERTEXCOUNT = 2 + 4 * VERTICES_PER_QUADRANT;
static const int NOTEDATASIZE = NOTEDATAVERTEXCOUNT*VERTEX_SIZE; // 2 coords x,y per vertex

static const int STAFFLINEVERTEXCOUNT = STAFFLINECOUNT*2; // 2 vertices each line
static const int STAFFLINEDATASIZE = STAFFLINEVERTEXCOUNT*VERTEX_SIZE;  // 2 coords x,y per vertex

static const int STAFFQUADVERTEXCOUNT = STAFFLINECOUNT*4; // 4 vertices each quad
static const int STAFFQUADDATASIZE = STAFFQUADVERTEXCOUNT*VERTEX_SIZE; // 2 coords x,y per vertex
static const int LINETHICKNESS = 4;
static const int LINEOFFSET = LINETHICKNESS/2;

// -- messy stuff for positioning music symbols --
static const float SYMBOL_OFFSET_X = 0.1;
static const float TREBLE_SYMBOL_OFFSET_Y = 0.6;
static const float BASS_SYMBOL_OFFSET_Y = 0.28;
static const float DISPLAY_SCALE_FACTOR = 4.0;
static const float ACCIDENTAL_DISPLAY_SCALE_FACTOR = 10.0;

static int staffNotes = 0;

static int octave = 1; // default to 1 octave up for display of guitar notes

static StaffMetrics staffMetrics;

// -- remember staff line Y coordinates, lines and spaces --
static int staffPosY[STAFFNOTEPOSITIONS];
// -- remember possible note positions on staff, including sharps
static int notePosY[TOTALNOTES];

static int staffLineWidth[STAFFLINECOUNT] = // short == 1, long == 2
{
    1, 1, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1, 1, 1
};

// --  this doesn't move, so can be set with static coords --
static GLshort staffLineArray[STAFFLINEDATASIZE];
static GLshort staffQuadArray[STAFFQUADDATASIZE];

// -- this to be positioned using transform calls --
static GLshort noteDataArray[NOTEDATASIZE];

///////////////////////////////////////////////////
#pragma mark -- window initialisation functions --
///////////////////////////////////////////////////

static void setNoteDataArray()
{
    // -- set start (center) point for GL_TRIANGLE_FAN --
    noteDataArray[0] = 0;
    noteDataArray[1] = 0;
	
    // -- exclude the center point of fan but add the extra overlapping point --
    float slice = (M_PI*2.0)/(float)(NOTEDATAVERTEXCOUNT-2); 
	
    int curVertexElement = 2; // may be x or y
	
    for (int n = 0; n < NOTEDATAVERTEXCOUNT-1; n++)
    {
        float fx = -sin((float)n * slice); // 0 1 0 -1 0
        float fy = cos((float)n * slice); // 1 0 -1 0 1
        
        int x = fx * staffMetrics.noteWidth;
        int y = fy * staffMetrics.noteHeight;
		
        noteDataArray[curVertexElement++] = x;
        noteDataArray[curVertexElement++] = y;
    }
}

static void getStaffDataCentered()
{
    // -- This staff should scale correctly & can be
    // -- scrolled etc. as its vertices are offsets from 0,0
	
    int hCenter = staffMetrics.displayWidth/2;    
    int hStartLong = -hCenter;
    int hEndLong = hCenter;
    int hStartShort = -hCenter/4;
    int hEndShort = hCenter/4;                               
    
    int vCenter = staffMetrics.displayHeight/2;
    int vDiv = staffMetrics.displayHeight/(STAFFLINECOUNT+1);
    
    // -- make note height of space between lines
    staffMetrics.noteHeight = vDiv/2;
    staffMetrics.noteWidth = staffMetrics.noteHeight*2.0;
    
    staffMetrics.lineOffset = MAX(staffMetrics.noteHeight/6.0, 1);
    
    // -- start tracking line Y positions
    int vPos = vDiv-vCenter;
    // -- position below current line
    int vPrevNoteOffset = vDiv/2;
    
    int noteCoordIndex = 0;
    int staffLineQuadIndex = 0;
    int staffQuadGroupIndex = 0;
    
    // -- This staff starts at BEGIN_NOTE, the note below the first line --    
    for (int y = 0; y < STAFFLINECOUNT; y++)
    {
        int hStart = hStartLong; // default
        int hEnd = hEndLong;
        
        int width = staffLineWidth[y];
        if (width == 1) // override for short line
        {
            hStart = hStartShort;
            hEnd = hEndShort;
        }
        
        staffLineArray[staffLineQuadIndex++] = hStart;
        staffLineArray[staffLineQuadIndex++] = vPos;
        staffLineArray[staffLineQuadIndex++] = hEnd;
        staffLineArray[staffLineQuadIndex++] = vPos;
        
		// -- these were quad coords for GL but same points and order
		// -- will work for ES tri-fans
        staffQuadArray[staffQuadGroupIndex++] = hStart;
        staffQuadArray[staffQuadGroupIndex++] = vPos+staffMetrics.lineOffset;
        staffQuadArray[staffQuadGroupIndex++] = hStart;
        staffQuadArray[staffQuadGroupIndex++] = vPos-staffMetrics.lineOffset;
        staffQuadArray[staffQuadGroupIndex++] = hEnd;
        staffQuadArray[staffQuadGroupIndex++] = vPos-staffMetrics.lineOffset;
        staffQuadArray[staffQuadGroupIndex++] = hEnd;
        staffQuadArray[staffQuadGroupIndex++] = vPos+staffMetrics.lineOffset;
        
        // -- note below current line --
        staffPosY[noteCoordIndex++]=vPos-vPrevNoteOffset; 
        staffPosY[noteCoordIndex++]=vPos;
        
        vPos += vDiv;
    }
    
    // -- END_NOTE above third extra line above treble clef --
    staffPosY[noteCoordIndex++]=vPos-vPrevNoteOffset; 
    
    staffNotes = noteCoordIndex;
}

// -- put into notePosY all possible note positions, including sharps/flats --
// -- info comes from staffPosY, which has all note positions excluding sharps/flats --
static void setupNoteStaffPos(int startNoteIndex, int endNoteIndex)
{
    int staffYIndex = 0, i = startNoteIndex;
    while (i <= endNoteIndex)
    {        
        if (isSharp(i))
            staffYIndex--;
		
        notePosY[i-startNoteIndex] = staffPosY[staffYIndex++];
        
        i++;
    }
}

///////////////////////////////////////////////////
#pragma mark -- staff and note drawing functions -- 
///////////////////////////////////////////////////

static void drawMusicSymbols()
{
    
	
    glPushMatrix();
    glTranslatef(staffMetrics.symbolXPos, staffMetrics.symbolYPosTreble, 0);
    glScalef(staffMetrics.symbolScaleX, staffMetrics.symbolScaleY, 1.0);    

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(VERTEX_SIZE, GL_FLOAT, 8, &Treble_vertex[5]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, Treble_vertexcount);	
    
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(staffMetrics.symbolXPos, staffMetrics.symbolYPosBass, 0);
    glScalef(staffMetrics.symbolScaleX, staffMetrics.symbolScaleY, 1.0);    
    
    glPopMatrix();
}

static void drawStaffDataQuads(float x, float y)
{
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    
    glPushMatrix();
    glTranslatef(x, y, 0);    
    glEnableClientState(GL_VERTEX_ARRAY);    
    glVertexPointer(VERTEX_SIZE, GL_SHORT, 0, staffQuadArray);
    glDrawArrays(GL_TRIANGLE_FAN, 0, STAFFQUADVERTEXCOUNT);
    glDisableClientState(GL_VERTEX_ARRAY);    
    glPopMatrix();
}

static void drawEllipse(int x, int y, float scale)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
	glScalef(scale, scale, 1.0);
    glEnableClientState(GL_VERTEX_ARRAY);    
    glVertexPointer(2, GL_SHORT, 0, noteDataArray);
    glDrawArrays(GL_TRIANGLE_FAN, 0, NOTEDATAVERTEXCOUNT);
    glDisableClientState(GL_VERTEX_ARRAY);    
    glPopMatrix();
}

static void drawSharp(int x, int y, float scale)
{
    glPushMatrix();
    glTranslatef(x + staffMetrics.noteWidth*2.4, y, 0);	
    glScalef(scale*staffMetrics.accidentalScaleX, scale*staffMetrics.accidentalScaleY, 1.0);    
    
	glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(VERTEX_SIZE, GL_SHORT, 8, &Sharp_vertex[5]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, Sharp_vertexcount);
    glDisableClientState(GL_VERTEX_ARRAY);    
    
    glPopMatrix();
}

static void drawNoteData(int x, int note, float scale)
{
    int noteVPos = notePosY[note-BEGIN_NOTE];
    
    if (isSharp(note))
    {
        //noteVPos -= 1; // previous note sharpened        
        drawSharp(x, staffMetrics.displayHeight/2+noteVPos, scale);
    }
    
    drawEllipse(x, staffMetrics.displayHeight/2+noteVPos, scale);
	
}

// -- public functions -- //

// -- guitar notes are usually displayed an octave higher on the staff
void setOctave(int oct)
{
    octave = oct;
}

void initDrawMusic(int width, int height)
{
	staffMetrics.displayWidth = width;
	staffMetrics.displayHeight = height;
	
    // --  not drawing code; only needs one init
    if (!notesInited)
        initNoteArray();
    
    // -- set up the vertex arrays and save positions of notes --
	
    getStaffDataCentered();
    
    // -- get positions for all notes, including sharp/flat --
    setupNoteStaffPos(BEGIN_NOTE, END_NOTE);
    
    // -- create triangle fan vertex array for note draw
    setNoteDataArray();
	
	staffMetrics.symbolXPos = (float)staffMetrics.displayWidth * SYMBOL_OFFSET_X;
	staffMetrics.symbolYPosTreble = (float)staffMetrics.displayHeight * TREBLE_SYMBOL_OFFSET_Y;
	staffMetrics.symbolYPosBass = (float)staffMetrics.displayHeight * BASS_SYMBOL_OFFSET_Y;
	staffMetrics.symbolScaleX = (float)staffMetrics.displayWidth / DISPLAY_SCALE_FACTOR;
	staffMetrics.symbolScaleY = (float)staffMetrics.displayHeight / DISPLAY_SCALE_FACTOR;	
	staffMetrics.accidentalScaleX = (float)staffMetrics.displayWidth / ACCIDENTAL_DISPLAY_SCALE_FACTOR;
	staffMetrics.accidentalScaleY = (float)staffMetrics.displayHeight / ACCIDENTAL_DISPLAY_SCALE_FACTOR;	    
}

void drawStaff()
{    
    
	// -- just means draw staff lines in quad format (iOS compatible, I think...?) --
    drawStaffDataQuads(staffMetrics.displayWidth/2.0, staffMetrics.displayHeight/2.0);	
	
    // -- add the decoration
    drawMusicSymbols();	
}

void drawNote(int note)
{
    note += (12 * octave); // *** up 1 octave if set ***
    
	int hCenter = staffMetrics.displayWidth/2;
	
    if (note >= BEGIN_NOTE && note <= END_NOTE)		
        drawNoteData(hCenter, note, 1.0);			
}

void drawNoteAt(int note, int hOffset, float scale)
{
    note += (12 * octave); // *** up 1 octave if set ***
    
	int hCenter = staffMetrics.displayWidth/2;
	
    if (note >= BEGIN_NOTE && note <= END_NOTE)		
        drawNoteData(hCenter + hOffset, note, scale);			
}

