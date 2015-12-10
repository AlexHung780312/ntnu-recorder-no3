#include "audioinput.h"
#include <QDebug>
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

  audio=new QAudioInput(settings,this);
  audio->start(&output);
  qInfo() << "state:" << audio->state();
  qInfo() << "error:" << audio->error();
  //QTimer::singleShot(3000, this, SLOT(terminateRecording()));
}
void AudioInput::terminateRecording(){
  audio->stop();
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
