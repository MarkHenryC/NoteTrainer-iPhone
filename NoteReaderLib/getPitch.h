#ifndef NoteTrainer_getPitch_h
#define NoteTrainer_getPitch_h

/* 
 
 Dynamic Wavelet Algorithm Pitch Tracking library
 Released under the MIT open source licence
 
 Original C code copyright (c) 2010 Antoine Schmitt
 
 Based on the paper:
 
 Eric Larson and Ross Maddox : "Real-Time Time-Domain Pitch Tracking Using Wavelets"
 http://online.physics.uiuc.edu/courses/phys498pom/NSF_REU_Reports/2005_reu/Real-Time_Time-Domain_Pitch_Tracking_Using_Wavelets.pdf
 
 Modified 2012 by Mark Carolan for use in the NoteTrainer project.
 
 1. Dynamic memory allocations made static
 2. Not using smoothing. This is done in new C lib notefilter.c
 3. Capturing using OpenAL in companion lib readnote.c
 3. Frequencies converted to music notes in notetable.c
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

 Usage:
 
 getPitch_init();
 double frequency = getPitch(sample_buffer);
 
 sample_buffer must be of size PITCHTRACKER_BUFFER_SIZE
 which must be a power of 2
 
 Frequency error tolerance defaults to 0.20
 Volume threshold ratio defaults to 0.75
 
 --MC
 
 */

#define PITCHTRACKER_BUFFER_SIZE 1024

void getPitch_init();
double getPitch_raw(double * samples);
double getPitch_smoothed(double * samples);

void getPitch_setAcceptedError(double e);
void getPitch_setThresholdRatio(double r);

#endif

