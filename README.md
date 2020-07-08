# NoteTrainer-iPhone
Plays notes from any monophonic instrument and it will show on a staff.
This is a gamified sight-reading tool. Notes slide in and you make them explode by hitting the right note on your guitar or with your voice.

This was built for the iPhone 3GS and was released into the App Store way back.

I found that the FFT-based note trackers were not very good at picking the exact pitch, just the note: in other words, a multiple of the base frequency. 
Wavelets, however, gave me a much better result, as least on minimal hardware.

The meat of it is in getPitch.c. See the corresponding header file for info on the origin of the wavelet algorithm.
