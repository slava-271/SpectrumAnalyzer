#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSlider>
#include <QLabel>

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(double minDb, double maxDb, double maxFreq, QWidget *parent = nullptr);
    double getNewMinDb() const { return m_newMinDb; }
    double getNewMaxDb() const { return m_newMaxDb; }
    double getNewMaxFreq() const { return m_newMaxFreq; }

private:
    double m_newMinDb, m_newMaxDb, m_newMaxFreq;
    QSlider *minDbSlider, *maxDbSlider, *maxFreqSlider;
    QLabel *minDbLabel, *maxDbLabel, *maxFreqLabel;

private slots:
    void updateMinDb(int value);
    void updateMaxDb(int value);
    void updateMaxFreq(int value);
};

#endif
