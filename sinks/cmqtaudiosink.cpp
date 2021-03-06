#include <QDebug>
#include <QFile>
#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QtCore/qmath.h>
#include <QtCore/qendian.h>

#include "cmqtaudiosink.h"

const int BufferSize      = 65535;

CMQtAudioSink::CMQtAudioSink(QObject *parent)
    :   CMBaseAudioSink(parent)
    ,   m_device(QAudioDeviceInfo::defaultOutputDevice())
    ,   m_audioOutput(0)
    ,   m_output(0)
    ,   m_buffer(BufferSize, 0)
{
    initializeAudio();
}

void CMQtAudioSink::initializeAudio()
{
#if QT_VERSION < 0x050000
    m_format.setFrequency(m_rate);
    m_format.setChannels(m_channels);
#else
    m_format.setSampleRate(m_rate);
    m_format.setChannelCount(m_channels);
#endif
    m_format.setSampleSize(16);
    m_format.setCodec("audio/pcm");
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setSampleType(QAudioFormat::SignedInt);

    qDebug() << m_device.deviceName();
    if (!m_device.isFormatSupported(m_format)) {
        qWarning() << "Default format not supported - trying to use nearest";
        m_format = m_device.nearestFormat(m_format);
    }

    createAudioOutput();
}

void CMQtAudioSink::createAudioOutput()
{
    m_audioOutput = new QAudioOutput(m_device, m_format, this);
    connect(m_audioOutput, SIGNAL(notify()), SLOT(notified()));
    connect(m_audioOutput, SIGNAL(stateChanged(QAudio::State)), SLOT(outputStateChanged(QAudio::State)));
    m_audioOutput->setBufferSize(65535);
}

CMQtAudioSink::~CMQtAudioSink()
{

}

bool CMQtAudioSink::play()
{    
    if (!m_audioOutput) {
        qWarning("AudioOutput not set!");
        return false;
    }

    if (!m_source) {
        qWarning("Source not set!");
        return false;
    }

    QAudio::State s=m_audioOutput->state();

    qDebug() << "Play: State is " << s;

    switch (s) {
    case QAudio::ActiveState:
        //
        break;
    case QAudio::SuspendedState:
        m_audioOutput->resume();
        break;
    case QAudio::StoppedState:
        if (!m_source->isOpen()) {
            qDebug("Opening source");
            bool r=m_source->open(QIODevice::ReadOnly);
            if (!r) {
                qWarning("Open for playback failed");
                return false;
            }
        }

        qDebug("Starting playback");
        m_audioOutput->start(m_source);
        break;
    case QAudio::IdleState:
        qDebug("Idle Audio state");
        //m_audioOutput->resume(m_generator);
        break;
    default:
        qWarning("Unknown Audio state");
        return false;
        break;
    }

    qDebug() << "Buffer size used is: " << m_audioOutput->bufferSize();

    return true;
}

bool CMQtAudioSink::stop()
{
    m_audioOutput->stop();
    // m_generator->close(); XXX Should we close the source too ?

    return true;
}

bool CMQtAudioSink::pause()
{
    m_audioOutput->suspend();

    return true;
}

bool CMQtAudioSink::resume()
{
    m_audioOutput->resume();

    return true;
}

void CMQtAudioSink::deviceChanged(int index)
{
    m_source->close();
    m_audioOutput->stop();
    m_audioOutput->disconnect(this);
    createAudioOutput();
}

void CMQtAudioSink::notified()
{
    //qWarning() << "bytesFree = " << m_audioOutput->bytesFree() << ", " << "elapsedUSecs = " << m_audioOutput->elapsedUSecs() << ", " << "processedUSecs = " << m_audioOutput->processedUSecs();
    emit position(m_audioOutput->processedUSecs());
}

void CMQtAudioSink::toggleSuspendResume()
{
    if (m_audioOutput->state() == QAudio::SuspendedState) {
        qDebug() << "status: Suspended, resume()";
        m_audioOutput->resume();
    } else if (m_audioOutput->state() == QAudio::ActiveState) {
        qDebug() << "status: Active, suspend()";
        m_audioOutput->suspend();
    } else if (m_audioOutput->state() == QAudio::StoppedState) {
        qDebug() << "status: Stopped, resume()";
        m_audioOutput->resume();
    } else if (m_audioOutput->state() == QAudio::IdleState) {
        qDebug() << "status: IdleState";
    }
}

void CMQtAudioSink::outputStateChanged(QAudio::State state)
{
    qDebug() << "QtAudioOuputState: " << state;
    setState(state);
}
