
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

#define PROGRAM_NAME    "ee469_lab01_dtmf_wav_gen"
#define FILENAME        "/home/mark/src/tmp/blob.wav"

#define CHANNELS         1      /* 1 for mono 2 for stereo              */
#define SAMPLE_RATE      8000   /* Samples per second                   */
#define BYTES_PER_SAMPLE 1      /* (BitsPerSample * Channels) / 8       *
                                 * 1 - 8 bit mono                       *
                                 * 2 - 8 bit stereo/16 bit mono         *
                                 * 4 - 16 bit stereo                    */

#define BITS_PER_SAMPLE 8      /* 256 possible values                  */
#define DURATION_IN_MS  2000    /* Duration in milliseconds             */
#define SILENCE_IN_MS   150     /* The pause between each DTMF tone     */
#define AMPLITUDE       0.1     /* Max amplitude of signal, relative    *
                                 * to the maximum scale                 */
#define DTMF_ROW_1 697
#define DTMF_ROW_2 770
#define DTMF_ROW_3 852
#define DTMF_ROW_4 941

#define DTMF_COL_1 1209
#define DTMF_COL_2 1336
#define DTMF_COL_3 1477
#define DTMF_COL_4 1633


using namespace std;

const int sampleRate = 8000;
const int bitDepth = 8;        //It is really 8 bit I dont know why I need to put 16 for it to work


void writeToFile(ofstream &file, int value, int size) {
    file.write(reinterpret_cast<const char*> (&value), size);
}


// writes to file a sin wave with frequency in hertz and duration in seconds
void sinFunc( int hertz, int duration ) {
    float coef = ( pow(2, bitDepth) / 2 ) - 1;
    
    // create sin pattern from 0 to 255
    float angle = 0;
    for ( int i = 0; i < sampleRate * duration; i++ ) {
        writeToFile(sinFile, (coef*sin(angle) + coef), bitdepth/8);
        angle+= 2 * M_PI / (sampleRate / hertz);
    }
}

int main() {

    ////////////////////  Silence  ////////////////////

    ofstream myAudioFile;
    myAudioFile.open("silence.wav", ios::binary);

    // Header
    myAudioFile << "RIFF";            // File type
    myAudioFile << "----";            // Size of file (to be filled later)
    myAudioFile << "WAVE";
    myAudioFile << "fmt ";
    writeToFile(myAudioFile, 16, 4);                          // Size of format
    writeToFile(myAudioFile, 1, 2);                           // Compression code
    writeToFile(myAudioFile, 1, 2);                           // Number of channels
    writeToFile(myAudioFile, sampleRate, 4);                  // Sample rate
    writeToFile(myAudioFile, sampleRate * bitDepth / 8, 4 );  // Byte rate
    writeToFile(myAudioFile, bitDepth / 8, 2);                // Block align
    writeToFile(myAudioFile, bitDepth, 2);                    // Bit depth
    myAudioFile << "data";
    myAudioFile << "----";                    // Size of data (to be filled later)

    int preAudioPosition = myAudioFile.tellp();

    // fill data with silence
    for ( int i = 0; i < sampleRate; i++ ) {
        int sample = 128;
        writeToFile(myAudioFile, sample, 1);
    }

    int postAudioPosition = myAudioFile.tellp();

    // fill in the size of the data
    myAudioFile.seekp(preAudioPosition - 4);
    writeToFile(myAudioFile, postAudioPosition - preAudioPosition, 4);

    // fill in the size of the file
    myAudioFile.seekp(4, ios::beg);
    writeToFile(myAudioFile, postAudioPosition - 8, 4);

    myAudioFile.close();



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
    writeToFile(sawtoothFile, sampleRate, 4); // Sample rate
    writeToFile(sawtoothFile, sampleRate * bitDepth / 8, 4 ); // Byte rate
    writeToFile(sawtoothFile, bitDepth / 8, 2); // Block align
    writeToFile(sawtoothFile, bitDepth, 2); // Bit depth
    sawtoothFile << "data";
    sawtoothFile << "----";  // size of data (to be filled later)

    int sawtoothPreAudioPosition = sawtoothFile.tellp();

    // create sawtooth pattern
    int sawtooth;
    for ( int i = 0; i < sampleRate; i++ ) {
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
    writeToFile(sinFile, sampleRate, 4); // Sample rate
    writeToFile(sinFile, sampleRate * bitDepth / 8, 4 ); // Byte rate
    writeToFile(sinFile, bitDepth / 8, 2); // Block align
    writeToFile(sinFile, bitDepth, 2); // Bit depth
    sinFile << "data";
    sinFile << "----";  // size of data (to be filled later)

    int sinPreAudioPosition = sinFile.tellp();

    // create sin pattern from 1 to 255
    float angle = 0;
    for ( int i = 0; i < sampleRate; i++ ) {
        writeToFile(sinFile, (127*sin(angle) + 128), 1);
        angle+= M_PI / 4;
    }

    int sinPostAudioPosition = sinFile.tellp();

    // fill in the size of the data
    sinFile.seekp(sinPreAudioPosition - 4);
    writeToFile(sinFile, sinPostAudioPosition - sinPreAudioPosition, 4);

    // fill in the size of the file
    sinFile.seekp(4, ios::beg);
    writeToFile(sinFile, sinPostAudioPosition - 8, 4);

    sinFile.close();


    ////////////////////  DTMF Tone  ////////////////////
    
    ofstream File;
    sinFile.open("sine.wav", ios::binary);

    // Header
    sinFile << "RIFF";
    sinFile << "----";  // size of file (to be filled later)
    sinFile << "WAVE";
    sinFile << "fmt ";
    writeToFile(sinFile, 16, 4); // Size
    writeToFile(sinFile, 1, 2); // Compression code
    writeToFile(sinFile, 1, 2); // Number of channels
    writeToFile(sinFile, sampleRate, 4); // Sample rate
    writeToFile(sinFile, sampleRate * bitDepth / 8, 4 ); // Byte rate
    writeToFile(sinFile, bitDepth / 8, 2); // Block align
    writeToFile(sinFile, bitDepth, 2); // Bit depth
    sinFile << "data";
    sinFile << "----";  // size of data (to be filled later)

    int sinPreAudioPosition = sinFile.tellp();

    // create sin pattern from 1 to 255
    float angle = 0;
    for ( int i = 0; i < sampleRate; i++ ) {
        writeToFile(sinFile, (127*sin(angle) + 128), 1);
        angle+= M_PI / 4;
    }

    int sinPostAudioPosition = sinFile.tellp();

    // fill in the size of the data
    sinFile.seekp(sinPreAudioPosition - 4);
    writeToFile(sinFile, sinPostAudioPosition - sinPreAudioPosition, 4);

    // fill in the size of the file
    sinFile.seekp(4, ios::beg);
    writeToFile(sinFile, sinPostAudioPosition - 8, 4);

    sinFile.close();
  
    return 0;
}
