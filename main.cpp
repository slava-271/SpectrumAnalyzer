#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "SpectrumWidget.h"
#include "WaveformWidget.h"
#include "SettingsDialog.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QMainWindow window;


    SpectrumWidget *spectrum = new SpectrumWidget(&window);
    WaveformWidget *waveform = new WaveformWidget(&window);


    QObject::connect(spectrum, &SpectrumWidget::rawDataAvailable,
                     waveform, &WaveformWidget::updateWaveform);


    QStackedWidget *stackedWidget = new QStackedWidget(&window);


    stackedWidget->addWidget(waveform);

    stackedWidget->addWidget(spectrum);


    stackedWidget->setCurrentIndex(1);

    window.setCentralWidget(stackedWidget);


    QMenuBar *menuBar = new QMenuBar(&window);


    QMenu *settingsMenu = menuBar->addMenu("Настройки");
    QAction *rangeAction = settingsMenu->addAction("Диапазоны...");


    QMenu *graphsMenu = menuBar->addMenu("Графики");
    QAction *showWaveAction = graphsMenu->addAction("Осциллограмма");
    QAction *showSpecAction = graphsMenu->addAction("SND Спектр");


    QActionGroup *graphGroup = new QActionGroup(&window);
    graphGroup->addAction(showWaveAction);
    graphGroup->addAction(showSpecAction);
    showWaveAction->setCheckable(true);
    showSpecAction->setCheckable(true);
    showSpecAction->setChecked(true);

    window.setMenuBar(menuBar);


    QObject::connect(showWaveAction, &QAction::triggered, [stackedWidget]() {
        stackedWidget->setCurrentIndex(0);
    });

    QObject::connect(showSpecAction, &QAction::triggered, [stackedWidget]() {
        stackedWidget->setCurrentIndex(1);
    });


    QObject::connect(rangeAction, &QAction::triggered, [&window, spectrum](){

        SettingsDialog d(spectrum->getMinDb(), spectrum->getMaxDb(), spectrum->getMaxFreq(), &window);
        if(d.exec() == QDialog::Accepted) {
            spectrum->setParameters(d.getNewMinDb(), d.getNewMaxDb(), d.getNewMaxFreq());
        }
    });

    window.resize(900, 500);
    window.setWindowTitle("Аудио Анализатор");
    window.show();

    return a.exec();
}
