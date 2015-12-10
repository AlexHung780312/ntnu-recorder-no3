#ifndef AUDIOINPUT_H
#define AUDIOINPUT_H

#include <QtMultimedia>
#include <QtCore>

class AudioInput : public QObject
{
    Q_OBJECT
public:
    explicit AudioInput(QObject *parent = 0);
    QAudio::State state();
signals:

public slots:
    void startRecording(const QString &path);
    void terminateRecording();
private:
    QAudioInput *audio;
    QFile output;
    QBuffer buffer;
};

#endif // AUDIOINPUT_H
