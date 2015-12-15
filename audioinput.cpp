#include "audioinput.h"
#include <QDebug>
#include <wavheader.h>
#include <QMessageBox>
AudioInput::AudioInput(QObject *parent) : QObject(parent),
    audio(NULL),output(NULL)
{

}

void AudioInput::startRecording(const QString &path){
    output = new QFile();
    output->setFileName(path);
    QAudioFormat settings;
    settings.setCodec("audio/pcm");
    settings.setSampleRate(16000);
    settings.setSampleSize(16);
    settings.setChannelCount(1);
    settings.setByteOrder(QAudioFormat::LittleEndian);
    settings.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(settings)) {
        QMessageBox::information(NULL,"info","Default format not supported, trying to use the nearest.");
        settings = info.nearestFormat(settings);
        //QMessageBox::information(NULL,"info",info.deviceName());
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
    output->open(QIODevice::WriteOnly| QIODevice::Truncate);
    output->write(wave,sizeof(Wav_header) + nbyte);
    buffer.seek(0);
    buffer.close();
    output->close();
    delete output;
    delete audio;
    audio=NULL;
    output=NULL;
}
QAudio::State AudioInput::state()
{
    if(audio != NULL)
        return audio->state();
    else
        return QAudio::IdleState;
}
