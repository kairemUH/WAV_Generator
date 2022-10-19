
///////////////////////////////////////////////////////////////////////////////
// University of Hawaii, College of Engineering
// ee469_lab01_dtmf_wav_gen - EE 469 - Fall 2022
//
/// Generate a .wav file containing DTMF dialing tones
///
/// @see https://docs.fileformat.com/audio/wav/
///
/// @file main.cpp
/// @version 1.0
///
/// @author Kai <kairem@hawaii.edu>
/// @date 04_Oct_2022
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <cmath>

#define CHANNELS         1      /* 1 for mono 2 for stereo              */
#define SAMPLE_RATE      8000   /* Samples per second                   */
#define BYTES_PER_SAMPLE 1      /* (BitsPerSample * Channels) / 8       *
                                 * 1 - 8 bit mono                       *
                                 * 2 - 8 bit stereo/16 bit mono         *
                                 * 4 - 16 bit stereo                    */

#define BIT_DEPTH 8             /* 256 possible values                  */
#define AMPLITUDE       0.1     /* Max amplitude of signal, relative to *
                                 * the maximum scale (Acts as volume)   */
#define DTMF_ROW_1 697
#define DTMF_ROW_2 770
#define DTMF_ROW_3 852
#define DTMF_ROW_4 941

#define DTMF_COL_1 1209
#define DTMF_COL_2 1336
#define DTMF_COL_3 1477
#define DTMF_COL_4 1633

using namespace std;

// @todo implement amplitude define into functions
// @todo implement channels into wav header

// writes value to file with "size" bytes
void writeToFile(ofstream &file, unsigned int value, int size) {
    file.write(reinterpret_cast<const char*> (&value), size);
}


//@todo refactor silenceFunc, sinFunc, and dtmfMixer to include less conversions and floats to minimize drift

// writes "durationMS" milliseconds of silence to file
void silenceFunc( ofstream &file, int durationMS ) {

    int silence = pow(2, BIT_DEPTH) / 2;

    for ( int i = 0; i < SAMPLE_RATE * durationMS / 1000; i++ ) {
        writeToFile(file, silence, BIT_DEPTH/8);
    }

}



// writes to file a sin wave with frequency in hertz and duration in milliseconds
void sinFunc( ofstream &file, int hertz, int durationMS ) {

    float amplitude = ( pow(2, BIT_DEPTH) / 2 ) - 1;

    float angle = 0;
    for ( int i = 0; i < SAMPLE_RATE * durationMS / 1000; i++ ) {
        writeToFile(file, (unsigned int) ( amplitude * sin(angle) + amplitude ), BIT_DEPTH/8);
        angle+= 2 * M_PI / ( (float) SAMPLE_RATE / hertz);
    }

}



// mixes together two tones with frequencies hertz1 and hertz2 and writes to file for duration in milliseconds
void dtmfMixer( ofstream &file, int hertz1, int hertz2, int durationMS ) {

    float amplitude = ( pow(2, BIT_DEPTH) / 2 ) - 1;

    float angle1 = 0;
    float angle2 = 0;
    for ( int i = 0; i < SAMPLE_RATE * durationMS / 1000; i++ ) {

        unsigned int sin1 = (unsigned int) ( amplitude * sin(angle1) + amplitude );
        unsigned int sin2 = (unsigned int) ( amplitude * sin(angle2) + amplitude );

        writeToFile(file, (sin1 + sin2) / 2, BIT_DEPTH/8);

        angle1+= 2 * M_PI / ( (float) SAMPLE_RATE / hertz1);
        angle2+= 2 * M_PI / ( (float) SAMPLE_RATE / hertz2);

    }

}



int main() {

    ////////////////////  Silence  ////////////////////

    ofstream silenceFile;
    silenceFile.open("silence.wav", ios::binary);   // write to file in binary

    // Header
    silenceFile << "RIFF";            // File type
    silenceFile << "----";            // Size of file (to be filled later)
    silenceFile << "WAVE";
    silenceFile << "fmt ";
    writeToFile(silenceFile, 16, 4);                          // Size of format
    writeToFile(silenceFile, 1, 2);                           // Compression code
    writeToFile(silenceFile, 1, 2);                           // Number of channels
    writeToFile(silenceFile, SAMPLE_RATE, 4);                  // Sample rate
    writeToFile(silenceFile, SAMPLE_RATE * BIT_DEPTH / 8, 4 );  // Byte rate
    writeToFile(silenceFile, BIT_DEPTH / 8, 2);                // Block align
    writeToFile(silenceFile, BIT_DEPTH, 2);                    // Bit depth
    silenceFile << "data";
    silenceFile << "----";                    // Size of data (to be filled later)

    int preAudioPosition = silenceFile.tellp();

    // fill data with 1 second of silence
    silenceFunc( silenceFile, 1000 );

    int postAudioPosition = silenceFile.tellp();

    // fill in the size of the data
    silenceFile.seekp(preAudioPosition - 4);
    writeToFile(silenceFile, postAudioPosition - preAudioPosition, 4);

    // fill in the size of the file
    silenceFile.seekp(4, ios::beg);     // seek 4 bytes after the beginning of file
    writeToFile(silenceFile, postAudioPosition - 8, 4);

    silenceFile.close();



    ////////////////////  Sawtooth Pattern  ////////////////////

    ofstream sawtoothFile;
    sawtoothFile.open("sawtooth.wav", ios::binary);

    // Header
    sawtoothFile << "RIFF";
    sawtoothFile << "----";  // size of file (to be filled later)
    sawtoothFile << "WAVE";
    sawtoothFile << "fmt ";
    writeToFile(sawtoothFile, 16, 4); // Size
    writeToFile(sawtoothFile, 1, 2); // Compression code
    writeToFile(sawtoothFile, 1, 2); // Number of channels
    writeToFile(sawtoothFile, SAMPLE_RATE, 4); // Sample rate
    writeToFile(sawtoothFile, SAMPLE_RATE * BIT_DEPTH / 8, 4 ); // Byte rate
    writeToFile(sawtoothFile, BIT_DEPTH / 8, 2); // Block align
    writeToFile(sawtoothFile, BIT_DEPTH, 2); // Bit depth
    sawtoothFile << "data";
    sawtoothFile << "----";  // size of data (to be filled later)

    int sawtoothPreAudioPosition = sawtoothFile.tellp();

    // create sawtooth pattern
    int sawtooth;
    for ( int i = 0; i < SAMPLE_RATE; i++ ) {
        if ( sawtooth == 255 ) {sawtooth = 0;}
        writeToFile(sawtoothFile, sawtooth, 1);
        sawtooth++;
    }

    int sawtoothPostAudioPosition = sawtoothFile.tellp();

    // fill in the size of the data
    sawtoothFile.seekp(sawtoothPreAudioPosition - 4);
    writeToFile(sawtoothFile, sawtoothPostAudioPosition - sawtoothPreAudioPosition, 4);

    // fill in the size of the file
    sawtoothFile.seekp(4, ios::beg);
    writeToFile(sawtoothFile, sawtoothPostAudioPosition - 8, 4);

    sawtoothFile.close();


    ////////////////////  Sine Wave  ////////////////////

    ofstream sinFile;
    sinFile.open("sine.wav", ios::binary);

    // Header
    sinFile << "RIFF";
    sinFile << "----";  // size of file (to be filled later)
    sinFile << "WAVE";
    sinFile << "fmt ";
    writeToFile(sinFile, 16, 4); // Size
    writeToFile(sinFile, 1, 2); // Compression code
    writeToFile(sinFile, 1, 2); // Number of channels
    writeToFile(sinFile, SAMPLE_RATE, 4); // Sample rate
    writeToFile(sinFile, SAMPLE_RATE * BIT_DEPTH / 8, 4 ); // Byte rate
    writeToFile(sinFile, BIT_DEPTH / 8, 2); // Block align
    writeToFile(sinFile, BIT_DEPTH, 2); // Bit depth
    sinFile << "data";
    sinFile << "----";  // size of data (to be filled later)

    int sinPreAudioPosition = sinFile.tellp();

    // write to sinFile a 900hz sin pattern for 1 second
    sinFunc( sinFile, 900, 1000 );

    int sinPostAudioPosition = sinFile.tellp();

    // fill in the size of the data
    sinFile.seekp(sinPreAudioPosition - 4);
    writeToFile(sinFile, sinPostAudioPosition - sinPreAudioPosition, 4);

    // fill in the size of the file
    sinFile.seekp(4, ios::beg);
    writeToFile(sinFile, sinPostAudioPosition - 8, 4);

    sinFile.close();


    ////////////////////  DTMF Tone  ////////////////////
    
    ofstream dtmfFile;
    dtmfFile.open("dtmf.wav", ios::binary);

    // Header
    dtmfFile << "RIFF";
    dtmfFile << "----";  // size of file (to be filled later)
    dtmfFile << "WAVE";
    dtmfFile << "fmt ";
    writeToFile(dtmfFile, 16, 4); // Size
    writeToFile(dtmfFile, 1, 2); // Compression code
    writeToFile(dtmfFile, 1, 2); // Number of channels
    writeToFile(dtmfFile, SAMPLE_RATE, 4); // Sample rate
    writeToFile(dtmfFile, SAMPLE_RATE * BIT_DEPTH / 8, 4 ); // Byte rate
    writeToFile(dtmfFile, BIT_DEPTH / 8, 2); // Block align
    writeToFile(dtmfFile, BIT_DEPTH, 2); // Bit depth
    dtmfFile << "data";
    dtmfFile << "----";  // size of data (to be filled later)

    int dtmfPreAudioPosition = dtmfFile.tellp();

    // write a dtmf 1 to dtmf file for 1 second
    dtmfMixer( dtmfFile, DTMF_ROW_1, DTMF_COL_1, 1000 );

    int dtmfPostAudioPosition = dtmfFile.tellp();

    // fill in the size of the data
    dtmfFile.seekp(dtmfPreAudioPosition - 4);
    writeToFile(dtmfFile, dtmfPostAudioPosition - dtmfPreAudioPosition, 4);

    // fill in the size of the file
    dtmfFile.seekp(4, ios::beg);
    writeToFile(dtmfFile, dtmfPostAudioPosition - 8, 4);

    dtmfFile.close();



    ////////////////////  DTMF Dial Tone  ////////////////////

    ofstream dialFile;
    dialFile.open("dial.wav", ios::binary);

    // Header
    dialFile << "RIFF";
    dialFile << "----";  // size of file (to be filled later)
    dialFile << "WAVE";
    dialFile << "fmt ";
    writeToFile(dialFile, 16, 4); // Size
    writeToFile(dialFile, 1, 2); // Compression code
    writeToFile(dialFile, 1, 2); // Number of channels
    writeToFile(dialFile, SAMPLE_RATE, 4); // Sample rate
    writeToFile(dialFile, SAMPLE_RATE * BIT_DEPTH / 8, 4 ); // Byte rate
    writeToFile(dialFile, BIT_DEPTH / 8, 2); // Block align
    writeToFile(dialFile, BIT_DEPTH, 2); // Bit depth
    dialFile << "data";
    dialFile << "----";  // size of data (to be filled later)

    int dialPreAudioPosition = dialFile.tellp();

    // write dtmf tone "0123456789*#abcd"
    silenceFunc( dialFile, 100 );
    dtmfMixer( dialFile, DTMF_ROW_4, DTMF_COL_2, 200 );     // 0
    silenceFunc( dialFile, 100 );
    dtmfMixer( dialFile, DTMF_ROW_1, DTMF_COL_1, 200 );     // 1
    silenceFunc( dialFile, 100 );
    dtmfMixer( dialFile, DTMF_ROW_1, DTMF_COL_2, 200 );     // 2
    silenceFunc( dialFile, 100 );
    dtmfMixer( dialFile, DTMF_ROW_1, DTMF_COL_3, 200 );     // 3
    silenceFunc( dialFile, 100 );
    dtmfMixer( dialFile, DTMF_ROW_2, DTMF_COL_1, 200 );     // 4
    silenceFunc( dialFile, 100 );
    dtmfMixer( dialFile, DTMF_ROW_2, DTMF_COL_2, 200 );     // 5
    silenceFunc( dialFile, 100 );
    dtmfMixer( dialFile, DTMF_ROW_2, DTMF_COL_3, 200 );     // 6
    silenceFunc( dialFile, 100 );
    dtmfMixer( dialFile, DTMF_ROW_3, DTMF_COL_1, 200 );     // 7
    silenceFunc( dialFile, 100 );
    dtmfMixer( dialFile, DTMF_ROW_3, DTMF_COL_2, 200 );     // 8
    silenceFunc( dialFile, 100 );
    dtmfMixer( dialFile, DTMF_ROW_3, DTMF_COL_3, 200 );     // 9
    silenceFunc( dialFile, 100 );
    dtmfMixer( dialFile, DTMF_ROW_4, DTMF_COL_1, 200 );     // *
    silenceFunc( dialFile, 100 );
    dtmfMixer( dialFile, DTMF_ROW_4, DTMF_COL_3, 200 );     // #
    silenceFunc( dialFile, 100 );
    dtmfMixer( dialFile, DTMF_ROW_1, DTMF_COL_4, 200 );     // a
    silenceFunc( dialFile, 100 );
    dtmfMixer( dialFile, DTMF_ROW_2, DTMF_COL_4, 200 );     // b
    silenceFunc( dialFile, 100 );
    dtmfMixer( dialFile, DTMF_ROW_3, DTMF_COL_4, 200 );     // c
    silenceFunc( dialFile, 100 );
    dtmfMixer( dialFile, DTMF_ROW_4, DTMF_COL_4, 200 );     // d
    silenceFunc( dialFile, 100 );


    int dialPostAudioPosition = dialFile.tellp();

    // fill in the size of the data
    dialFile.seekp(dialPreAudioPosition - 4);
    writeToFile(dialFile, dialPostAudioPosition - dialPreAudioPosition, 4);

    // fill in the size of the file
    dialFile.seekp(4, ios::beg);
    writeToFile(dialFile, dialPostAudioPosition - 8, 4);

    dialFile.close();

    return 0;

}
