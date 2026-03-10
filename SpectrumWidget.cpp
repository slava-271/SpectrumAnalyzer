#include "SpectrumWidget.h"
#include <QPainter>
#include <cmath>
#include <algorithm>
#include <QDebug>

const double PI = 3.14159265358979323846;
typedef std::complex<double> Complex;

unsigned int bitReverse(unsigned int x, int log2n) {
    int n = 0;
    for (int i = 0; i < log2n; i++) {
        n <<= 1; n |= (x & 1); x >>= 1;
    }
    return n;
}

void fft(QVector<Complex>& a) {
    int n = a.size();
    int log2n = log2(n);
    for (int i = 0; i < n; i++) {
        unsigned int rev = bitReverse(i, log2n);
        if (i < rev) std::swap(a[i], a[rev]);
    }
    for (int len = 2; len <= n; len <<= 1) {
        double ang = 2 * PI / len;
        Complex wlen(cos(ang), sin(ang));
        for (int i = 0; i < n; i += len) {
            Complex w(1);
            for (int j = 0; j < len / 2; j++) {
                Complex u = a[i + j];
                Complex v = a[i + j + len / 2] * w;
                a[i + j] = u + v;
                a[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }
}


AudioCapture::AudioCapture() { processingBuffer.resize(N_SAMPLES); }
AudioCapture::~AudioCapture() { stop(); }

void AudioCapture::stop() {
    if(hWaveIn) {
        waveInStop(hWaveIn);
        waveInReset(hWaveIn);
        waveInUnprepareHeader(hWaveIn, &waveHdr[0], sizeof(WAVEHDR));
        waveInUnprepareHeader(hWaveIn, &waveHdr[1], sizeof(WAVEHDR));
        waveInClose(hWaveIn);
        hWaveIn = nullptr;
    }
}

bool AudioCapture::start() {
    stop();
    WAVEFORMATEX wfx;
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = SAMPLE_RATE;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = 2;
    wfx.nAvgBytesPerSec = SAMPLE_RATE * 2;
    wfx.cbSize = 0;

    if (waveInOpen(&hWaveIn, WAVE_MAPPER, &wfx, (DWORD_PTR)waveInProc, (DWORD_PTR)this, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) return false;

    for (int i = 0; i < 2; i++) {
        waveHdr[i].lpData = (LPSTR)buffer[i];
        waveHdr[i].dwBufferLength = N_SAMPLES * 2;
        waveHdr[i].dwUser = i;
        waveInPrepareHeader(hWaveIn, &waveHdr[i], sizeof(WAVEHDR));
        waveInAddBuffer(hWaveIn, &waveHdr[i], sizeof(WAVEHDR));
    }
    waveInStart(hWaveIn);
    return true;
}

void CALLBACK AudioCapture::waveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
    if (uMsg == WIM_DATA) {
        AudioCapture* self = (AudioCapture*)dwInstance;
        WAVEHDR* hdr = (WAVEHDR*)dwParam1;
        int bufIndex = hdr->dwUser;
        for(int i=0; i<N_SAMPLES; i++) {
            self->processingBuffer[i] = self->buffer[bufIndex][i] / 32768.0;
        }
        waveInAddBuffer(hwi, hdr, sizeof(WAVEHDR));
    }
}

QVector<double> AudioCapture::getData() { return processingBuffer; }



SpectrumWidget::SpectrumWidget(QWidget *parent) : QWidget(parent) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SpectrumWidget::updateSpectrum);
    timer->start(16); // ~60 FPS

    capturer.start();

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(20, 20, 20));
    setAutoFillBackground(true);
    setPalette(pal);
    setMinimumHeight(200);
}

void SpectrumWidget::updateSpectrum() {
    QVector<double> rawData = capturer.getData();


    emit rawDataAvailable(rawData);


    performFFT(rawData);
}

void SpectrumWidget::performFFT(QVector<double>& inputData) {
    QVector<Complex> fftData(N_SAMPLES);
    for (int i = 0; i < N_SAMPLES; i++) {
        double window = 0.5 * (1 - cos(2 * PI * i / (N_SAMPLES - 1)));
        fftData[i] = Complex(inputData[i] * window, 0);
    }
    fft(fftData);
    spectrumData.resize(N_SAMPLES);
    for (int i = 0; i < N_SAMPLES; i++) {
        spectrumData[i] = std::abs(fftData[i]);
    }
    update();
}

double dbToY(double db, int height, int top, int bottom, double minDb, double maxDb) {
    if (db < minDb) db = minDb;
    if (db > maxDb) db = maxDb;
    return top + (height - top - bottom) * (1.0 - (db - minDb) / (maxDb - minDb));
}

double xToFreq(double x, int width, int left, int right, double maxFreq) {
    double minFreq = 20.0;
    return pow(10.0, (x - left) / (width - left - right) * (log10(maxFreq) - log10(minFreq)) + log10(minFreq));
}

double freqToX(double freq, int width, int left, int right, double maxFreq) {
    double minFreq = 20.0;
    if (freq < minFreq) freq = minFreq;
    if (freq > maxFreq) freq = maxFreq;
    return left + (log10(freq) - log10(minFreq)) / (log10(maxFreq) - log10(minFreq)) * (width - left - right);
}

void SpectrumWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int w = width(), h = height();
    const int M_LEFT=50, M_RIGHT=20, M_TOP=20, M_BOTTOM=30;


    painter.setFont(QFont("Arial", 8));
    QColor gridColor(60, 60, 60);
    QColor textColor(180, 180, 180);


    for (int db = (int)m_maxDb; db >= m_minDb; db -= 20) {
        int y = (int)dbToY(db, h, M_TOP, M_BOTTOM, m_minDb, m_maxDb);
        painter.setPen(QPen(gridColor, 1, Qt::DotLine));
        painter.drawLine(M_LEFT, y, w - M_RIGHT, y);
        painter.setPen(textColor);
        painter.drawText(5, y + 4, QString::number(db) + " dB");
    }


    int freqs[] = { 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000 };
    for (int f : freqs) {
        if (f > m_maxFreq) continue;
        int x = (int)freqToX(f, w, M_LEFT, M_RIGHT, m_maxFreq);
        painter.setPen(QPen(gridColor, 1, Qt::DotLine));
        painter.drawLine(x, M_TOP, x, h - M_BOTTOM);
        painter.setPen(textColor);
        painter.drawText(x - 10, h - 5, QString::number(f < 1000 ? f : f/1000) + (f < 1000 ? "" : "k"));
    }


    painter.setPen(Qt::white);
    painter.drawRect(M_LEFT, M_TOP, w - M_LEFT - M_RIGHT, h - M_TOP - M_BOTTOM);


    if (spectrumData.isEmpty()) return;
    double freqPerBin = (double)SAMPLE_RATE / N_SAMPLES;
    int plotWidth = w - M_LEFT - M_RIGHT;

    for (int x = 0; x < plotWidth; x++) {
        double f1 = xToFreq(x + M_LEFT, w, M_LEFT, M_RIGHT, m_maxFreq);
        double f2 = xToFreq(x + 1 + M_LEFT, w, M_LEFT, M_RIGHT, m_maxFreq);

        int idx1 = (int)(f1 / freqPerBin);
        int idx2 = (int)(f2 / freqPerBin);
        if (idx2 >= spectrumData.size()/2) idx2 = spectrumData.size()/2 - 1;
        if (idx1 > idx2) idx1 = idx2;

        double maxVal = 0;
        for (int k = idx1; k <= idx2; k++) if(spectrumData[k] > maxVal) maxVal = spectrumData[k];

        double db = 20.0 * log10(maxVal + 1e-9);
        double yPos = dbToY(db, h, M_TOP, M_BOTTOM, m_minDb, m_maxDb);
        double zeroY = dbToY(m_minDb, h, M_TOP, M_BOTTOM, m_minDb, m_maxDb);

        double hue = 240.0 - (240.0 * ((double)x / plotWidth));
        painter.setPen(QColor::fromHsv((int)hue, 200, 255));
        painter.drawLine(M_LEFT + x, (int)zeroY, M_LEFT + x, (int)yPos);
    }
}

void SpectrumWidget::setParameters(double minDb, double maxDb, double maxFreq) {
    m_minDb = minDb; m_maxDb = maxDb; m_maxFreq = maxFreq; update();
}
