#include <assert.h>

#include "NoteLookup.h"

// -- accessed as flag from client code
int notesInited = 0;

static const int NOTEARRAYSIZE = NOTELOOKUP_ARRAY_SIZE;
static double noteArray[NOTEARRAYSIZE];
static int noteSharpTable[NOTEARRAYSIZE];

static struct NoteLookup noteLookup[NOTEARRAYSIZE] = 
{
{16.35,"C0"},
{17.32,"C#0/Db0"},
{18.35,"D0"},
{19.45,"D#0/Eb0"},
{20.60,"E0"},
{21.83,"F0"},
{23.12,"F#0/Gb0"},
{24.50,"G0"},
{25.96,"G#0/Ab0"},
{27.50,"A0"},
{29.14,"A#0/Bb0"},
{30.87,"B0"},
{32.70,"C1"},
{34.65,"C#1/Db1"},
{36.71,"D1"},
{38.89,"D#1/Eb1"},
{41.20,"E1"},
{43.65,"F1"},
{46.25,"F#1/Gb1"},
{49.00,"G1"},
{51.91,"G#1/Ab1"},
{55.00,"A1"},
{58.27,"A#1/Bb1"},
{61.74,"B1"},           // [23] B start of staff
{65.41,"C2"},
{69.30,"C#2/Db2"},
{73.42,"D2"},			// [26]
{77.78,"D#2/Eb2"},
{82.41,"E2"},           // [28] E open low
{87.31,"F2"},			// [29] F on e
{92.50,"F#2/Gb2"},
{98.00,"G2"},			// [31] G on e
{103.83,"G#2/Ab2"},
{110.00,"A2"},          // [33] A open
{116.54,"A#2/Bb2"},
{123.47,"B2"},			// [35] B on a
{130.81,"C3"},			// [36] C on a
{138.59,"C#3/Db3"},
{146.83,"D3"}, 			// [38] D open
{155.56,"D#3/Eb3"},
{164.81,"E3"},			// [40] E on d
{174.61,"F3"},			// [41] F on d
{185.00,"F#3/Gb3"},
{196.00,"G3"}, 			// [43] G open
{207.65,"G#3/Ab3"},	
{220.00,"A3"},			// [45] A on g
{233.08,"A#3/Bb3"},	
{246.94,"B3"}, 			// [47] B open
{261.63,"C4"},          // [48] C on b
{277.18,"C#4/Db4"},
{293.66,"D4"},			// [50] D on b
{311.13,"D#4/Eb4"},
{329.63,"E4"}, 			// [52] E open high
{349.23,"F4"},			// [53] F on e
{369.99,"F#4/Gb4"},
{392.00,"G4"},			// [55] G on e
{415.30,"G#4/Ab4"},
{440.00,"A4"},			// [57] A on e
{466.16,"A#4/Bb4"},
{493.88,"B4"},
{523.25,"C5"},
{554.37,"C#5/Db5"},
{587.33,"D5"},
{622.25,"D#5/Eb5"},
{659.26,"E5"}, 			// [64] E on e 12th fret
{698.46,"F5"},
{739.99,"F#5/Gb5"},
{783.99,"G5"},
{830.61,"G#5/Ab5"},
{880.00,"A5"},
{932.33,"A#5/Bb5"},
{987.77,"B5"},
{1046.50,"C6"},
{1108.73,"C#6/Db6"},
{1174.66,"D6"},         // [index: 74]
{1244.51,"D#6/Eb6"},
{1318.51,"E6"},
{1396.91,"F6"},         // [index: 77]
{1479.98,"F#6/Gb6"},
{1567.98,"G6"},
{1661.22,"G#6/Ab6"},
{1760.00,"A6"},
{1864.66,"A#6/Bb6"},
{1975.53,"B6"},			// [83]
{2093.00,"C7"},
{2217.46,"C#7/Db7"},
{2349.32,"D7"},
{2489.02,"D#7/Eb7"},
{2637.02,"E7"},
{2793.83,"F7"},
{2959.96,"F#7/Gb7"},
{3135.96,"G7"},
{3322.44,"G#7/Ab7"},
{3520.00,"A7"},
{3729.31,"A#7/Bb7"},
{3951.07,"B7"},			// [95]
{4186.01,"C8"},
{4434.92,"C#8/Db8"},
{4698.64,"D8"},
{4978.03,"D#8/Eb8"}
};

// -- forward declare alternative
void initDynamicNoteArray();

// -- public functions --

void initNoteArray()
{
    if (!notesInited)
    {
        for (int i = 0; i < NOTEARRAYSIZE; i++)
        {
            noteArray[i] =  noteLookup[i].freqCenter;         
            noteSharpTable[i] = noteLookup[i].note[1] == '#' ? SHARP : NATURAL;
        }
        notesInited = 1;
    }
}

int getNoteIndex(double freq, double *pDistance)
{
    if (freq < noteArray[0]) // -- return lowest note
    {
        if (pDistance) 
            *pDistance = freq-noteArray[0];      
        
        return 0;
    }
    else if (freq > noteArray[NOTEARRAYSIZE-1]) // -- return highest note
    {
        if (pDistance) 
            *pDistance = freq-noteArray[NOTEARRAYSIZE-1];      
        
        return NOTEARRAYSIZE-1;
    }
    
    for (int i = 1; i < NOTEARRAYSIZE-1; i++)
    {
        double prevNote = noteArray[i-1];
        double thisNote = noteArray[i];
        double nextNote = noteArray[i+1];
        
        if (freq > prevNote && freq < nextNote)
        {
            double halfDistanceUp = (nextNote-thisNote)/2.0;
            double halfDistanceDown = (thisNote-prevNote)/2.0;
                        
            if (pDistance)
                *pDistance = freq-thisNote;
            
            if (freq >= prevNote+halfDistanceDown)
            {
                if (freq < nextNote-halfDistanceUp)
                    return i;
                else
                    return i+1;
            }
            else
                return i-1;
                
            return i;
        }
    }
    assert(0); // shouldn't get here >> but we do now!! Both audio and gfx now fucked
    return 0;
}

const char *getNoteDescription(int index)
{
    return noteLookup[index].note;
}

int isSharp(int index)
{
    return noteSharpTable[index];
}

float getFreqAtIndex(int index)
{
	return noteLookup[index].freqCenter;
}

