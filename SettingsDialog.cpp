#include "SettingsDialog.h"
#include <QGridLayout>
#include <QPushButton>

SettingsDialog::SettingsDialog(double minDb, double maxDb, double maxFreq, QWidget *parent)
    : QDialog(parent), m_newMinDb(minDb), m_newMaxDb(maxDb), m_newMaxFreq(maxFreq) {

    QGridLayout *layout = new QGridLayout(this);
    setWindowTitle("Настройки Спектра");

    minDbLabel = new QLabel(QString("Мин. dB: %1").arg(minDb));
    minDbSlider = new QSlider(Qt::Horizontal);
    minDbSlider->setRange(-160, (int)maxDb);
    minDbSlider->setValue((int)minDb);
    connect(minDbSlider, &QSlider::valueChanged, this, &SettingsDialog::updateMinDb);
    layout->addWidget(minDbLabel, 0, 0);
    layout->addWidget(minDbSlider, 0, 1);


    maxDbLabel = new QLabel(QString("Макс. dB: %1").arg(maxDb));
    maxDbSlider = new QSlider(Qt::Horizontal);
    maxDbSlider->setRange(-160, 10);
    maxDbSlider->setValue((int)maxDb);
    connect(maxDbSlider, &QSlider::valueChanged, this, &SettingsDialog::updateMaxDb);
    layout->addWidget(maxDbLabel, 1, 0);
    layout->addWidget(maxDbSlider, 1, 1);


    maxFreqLabel = new QLabel(QString("Макс. Частота: %1 Hz").arg(maxFreq));
    maxFreqSlider = new QSlider(Qt::Horizontal);
    maxFreqSlider->setRange(5000, 22000);
    maxFreqSlider->setValue((int)maxFreq);
    connect(maxFreqSlider, &QSlider::valueChanged, this, &SettingsDialog::updateMaxFreq);
    layout->addWidget(maxFreqLabel, 2, 0);
    layout->addWidget(maxFreqSlider, 2, 1);

    QPushButton *okBtn = new QPushButton("OK");
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(okBtn, 3, 0, 1, 2);
}

void SettingsDialog::updateMinDb(int value) {
    m_newMinDb = value; minDbLabel->setText(QString("Мин. dB: %1").arg(m_newMinDb));
}
void SettingsDialog::updateMaxDb(int value) {
    m_newMaxDb = value; maxDbLabel->setText(QString("Макс. dB: %1").arg(m_newMaxDb));
    minDbSlider->setMaximum(value);
}
void SettingsDialog::updateMaxFreq(int value) {
    m_newMaxFreq = value; maxFreqLabel->setText(QString("Макс. Частота: %1 Hz").arg(m_newMaxFreq));
}
