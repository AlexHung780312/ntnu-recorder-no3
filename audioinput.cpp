#include "audioinput.h"
#include <QDebug>
#include <wavheader.h>
AudioInput::AudioInput(QObject *parent) : QObject(parent),
    audio(NULL)
{

}

void AudioInput::startRecording(const QString &path){
    output.setFileName(path);
    output.open(QIODevice::WriteOnly| QIODevice::Truncate);
    QAudioFormat settings;
    settings.setCodec("audio/PCM");
    settings.setSampleRate(16000);
    settings.setSampleSize(16);
    settings.setChannelCount(1);
    settings.setByteOrder(QAudioFormat::LittleEndian);
    settings.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(settings)) {
        qWarning() << "Default format not supported, trying to use the nearest.";
        settings = info.nearestFormat(settings);
    }
    qInfo() << settings;
    audio=new QAudioInput(settings,this);
    buffer.open(QIODevice::ReadWrite);
    audio->start(&buffer);
    qInfo() << "state:" << audio->state();
    qInfo() << "error:" << audio->error();
}
void AudioInput::terminateRecording(){
    audio->stop();
    unsigned int nbyte = std::max(0ll,buffer.pos() - 1280);
    qInfo() << "nbyte=" << nbyte;
    char *wave = new char[sizeof(Wav_header) + nbyte];
    fillWavHeader(reinterpret_cast<Wav_header*>(wave),audio->format(),nbyte);
    buffer.seek(640);
    buffer.read(wave+44,nbyte);
    output.write(wave,sizeof(Wav_header) + nbyte);
    buffer.seek(0);
    buffer.close();
    output.close();
    delete audio;
    audio=NULL;
}
QAudio::State AudioInput::state()
{
    if(audio != NULL)
        return audio->state();
    else
        return QAudio::IdleState;
}
