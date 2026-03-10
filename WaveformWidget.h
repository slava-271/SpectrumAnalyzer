#ifndef WAVEFORMWIDGET_H
#define WAVEFORMWIDGET_H

#include <QWidget>
#include <QVector>

class WaveformWidget : public QWidget {
    Q_OBJECT
public:
    explicit WaveformWidget(QWidget *parent = nullptr);

public slots:
    void updateWaveform(const QVector<double>& data);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<double> waveformData;
};

#endif // WAVEFORMWIDGET_H
