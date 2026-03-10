#ifndef SPECTRUMWIDGET_H
#define SPECTRUMWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QVector>
#include <complex>
#include <windows.h>
#include <mmsystem.h>

const int SAMPLE_RATE = 44100;
const int N_SAMPLES = 2048;


class AudioCapture {
public:
    HWAVEIN hWaveIn = nullptr;
    WAVEHDR waveHdr[2];
    short buffer[2][N_SAMPLES];
    QVector<double> processingBuffer;

    AudioCapture();
    ~AudioCapture();
    bool start();
    void stop();
    QVector<double> getData();
    static void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
};

class SpectrumWidget : public QWidget {
    Q_OBJECT
public:
    explicit SpectrumWidget(QWidget *parent = nullptr);

    double getMinDb() const { return m_minDb; }
    double getMaxDb() const { return m_maxDb; }
    double getMaxFreq() const { return m_maxFreq; }

signals:
    void rawDataAvailable(const QVector<double>& data);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateSpectrum();

public slots:
    void setParameters(double minDb, double maxDb, double maxFreq);

private:
    void performFFT(QVector<double>& inputData);

    AudioCapture capturer;
    QTimer *timer;
    QVector<double> spectrumData;

    double m_minDb = -100.0;
    double m_maxDb = 0.0;
    double m_maxFreq = 20000.0;
};

#endif // SPECTRUMWIDGET_H
