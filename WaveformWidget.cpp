#include "WaveformWidget.h"
#include <QPainter>
#include <QColor>
#include <QPen>
#include <QPointF>
#include <algorithm>

WaveformWidget::WaveformWidget(QWidget *parent) : QWidget(parent) {
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(10, 10, 10));
    setAutoFillBackground(true);
    setPalette(pal);


    setMinimumHeight(120);
}

void WaveformWidget::updateWaveform(const QVector<double>& data) {
    waveformData = data;
    update();
}

void WaveformWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();
    int centerY = h / 2;


    painter.setPen(QPen(QColor(50, 50, 50), 1, Qt::DotLine));
    painter.drawLine(0, centerY, w, centerY);
    painter.drawLine(0, h/4, w, h/4);
    painter.drawLine(0, h*3/4, w, h*3/4);
    if (waveformData.isEmpty()) return;



    const int dataSize = waveformData.size();
    const double halfHeight = (h / 2.0) * 0.9;

\
    QPolygonF polylineUpper;
    QPolygonF polylineLower;

\
    double samplesPerPixel = (double)dataSize / w;

 \
    for (int x = 0; x < w; ++x) {

     \
        int sampleStart = (int)(x * samplesPerPixel);
        int sampleEnd = (int)((x + 1) * samplesPerPixel);

        if (sampleEnd > dataSize) {
            sampleEnd = dataSize;
        }
\
        if (sampleStart >= sampleEnd) {

            if (x > 0) {
                polylineUpper.append(polylineUpper.last());
                polylineLower.append(polylineLower.last());
            }
            continue;
        }


        double maxVal = -1.0;
        double minVal = 1.0;

        for (int i = sampleStart; i < sampleEnd; ++i) {
            double currentSample = waveformData[i];

            if (currentSample > maxVal) maxVal = currentSample;
            if (currentSample < minVal) minVal = currentSample;
        }

        double yMax = centerY - (maxVal * halfHeight);
        polylineUpper.append(QPointF(x, yMax));


        double yMin = centerY - (minVal * halfHeight);
        polylineLower.append(QPointF(x, yMin));
    }


    if (!polylineUpper.isEmpty()) {


        QPolygonF fullWaveform = polylineUpper;


        for (int i = polylineLower.size() - 1; i >= 0; --i) {
            fullWaveform.append(polylineLower[i]);
        }


        QColor fillColor(0, 255, 128, 120);
        painter.setBrush(fillColor);
        painter.setPen(Qt::NoPen);
        painter.drawPolygon(fullWaveform);

        painter.setPen(QPen(QColor(0, 255, 128), 1));
        painter.drawPolyline(polylineUpper);
        painter.drawPolyline(polylineLower);
    }

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 8));
    painter.drawText(5, 15, "Осциллограмма (Min/Max)");
}
