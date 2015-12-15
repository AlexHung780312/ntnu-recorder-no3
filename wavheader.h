#ifndef WAVHEADER
#define WAVHEADER
#include <cstdio>
#include <QtMultimedia>
#include <QDebug>
struct Wav_header {
    char ChunkID[4];
    unsigned int ChunkSize;
    char Format[4];
    char Subchunk1ID[4];
    unsigned int Subchunk1Size;
    unsigned short AudioFormat;
    unsigned short NumChannels;
    unsigned int SampleRate;
    unsigned int ByteRate;
    unsigned short BlockAlign;
    unsigned short BitsPerSample;
    char Subchunk2ID[4];
    unsigned int Subchunk2Size;
    signed short data[];
};

inline
void fillWavHeader(Wav_header *ptr, const QAudioFormat &fmt, unsigned int nbyte) {
    Q_ASSERT(fmt.sampleType()==QAudioFormat::SignedInt);
    sprintf(ptr->ChunkID,"RIFF");
    ptr->ChunkSize = 36 + nbyte;
    sprintf(ptr->Format,"WAVE");
    sprintf(ptr->Subchunk1ID,"fmt ");
    ptr->Subchunk1Size = 16;//PCM
    ptr->AudioFormat = 1;//PCM
    ptr->NumChannels = fmt.channelCount();//mono
    ptr->SampleRate = fmt.sampleRate();
    ptr->BitsPerSample = fmt.sampleSize();//short
    ptr->ByteRate = ptr->SampleRate * ptr->NumChannels * ptr->BitsPerSample/8;
    ptr->BlockAlign = ptr->NumChannels * ptr->BitsPerSample/8;
    sprintf(ptr->Subchunk2ID, "data");
    ptr->Subchunk2Size = (nbyte/(ptr->BitsPerSample/8)) * ptr->NumChannels * ptr->BitsPerSample/8;
};

#endif // WAVHEADER

