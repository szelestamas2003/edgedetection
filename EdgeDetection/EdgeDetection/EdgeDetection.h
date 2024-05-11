#ifndef EDGEDETECTION_H
#define EDGEDETECTION_H

#include <QtWidgets/QWidget>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qvector.h>
#include <qfuturewatcher.h>
#include "EdgeDetectionAlg.h"

class EdgeDetection : public QWidget
{
    Q_OBJECT

public:
    EdgeDetection(QWidget *parent = nullptr);
    ~EdgeDetection();

private slots:
    void startProccess();
    void resetButtons();
    void addImage();
    void showImage();
    void OnEdgeDetectionCompleted();
    void onItemProcessed(int value);

private:
    QFutureWatcher<QString> watcher;
    EdgeDetectionAlg algorithm;
    QIcon* imageIcon = new QIcon(QPixmap(":/EdgeDetection/plus_png.png"));
    QLabel* title, *cpuTitle, *gpuTitle;
    QVector<QPushButton*> original, filteredCPU, filteredGPU;
    QVector<QMovie*> movies, moviesCPU, moviesGPU;
    QStringList fileNames, filteredImagesCPU, filteredImagesGPU;
    QPushButton* startProccessButton, *resetButton;
    QVBoxLayout* mainLayout, * filteredCPULayout, * filteredGPULayout, *buttonLayout;
    QHBoxLayout* originalLayout, *filteredCPUImageLayout, *filteredGPUImageLayout, *headerLayout;

    const QString dirPath = "temp/";

    QString processItem(const QString& fileName);
};

#endif EDGEDETECTION_H