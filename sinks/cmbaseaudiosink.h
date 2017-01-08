#ifndef CMBASEAUDIOSINK_H
#define CMBASEAUDIOSINK_H

#include <QObject>
#include <QAudio>

#include "cmbaseaudiosource.h"

class CMBaseAudioSink : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAudio::State state READ state NOTIFY stateChanged)
public:
    explicit CMBaseAudioSink(QObject *parent = 0);

    virtual bool play()=0;
    virtual bool stop()=0;
    virtual bool pause()=0;

    virtual bool isValid();

    virtual void setAudioSource(CMBaseAudioSource *source);

    QAudio::State state() const;

signals:
    void stateChanged(QAudio::State state);
    void position(qint64 pos);

public slots:

protected:
    void setState(QAudio::State state);
    CMBaseAudioSource *m_source;
    QAudio::State m_state;
    bool m_valid;
};

#endif // CMBASEAUDIOSINK_H
