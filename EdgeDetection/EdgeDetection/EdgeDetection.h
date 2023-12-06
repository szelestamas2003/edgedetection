#ifndef EDGEDETECTION_H
#define EDGEDETECTION_H

#include <QtWidgets/QWidget>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qvector.h>
#include "EdgeDetectionAlg.h"

class EdgeDetection : public QWidget
{
    Q_OBJECT

public:
    EdgeDetection(QWidget *parent = nullptr);
    ~EdgeDetection();

private slots:
    void startProccess();
    void addImage();

private:
    QIcon* imageIcon = new QIcon(QPixmap(":/EdgeDetection/plus.jpg"));
    QLabel* title, *cpuTitle, *gpuTitle;
    QVector<QPushButton*> original, filteredCPU, filteredGPU;
    QPushButton* startProccessButton, *resetButton;
    QVBoxLayout* mainLayout, * filteredCPULayout, * filteredGPULayout;
    QHBoxLayout* originalLayout, *filteredCPUImageLayout, *filteredGPUImageLayout, *headerLayout;
};

#endif EDGEDETECTION_H