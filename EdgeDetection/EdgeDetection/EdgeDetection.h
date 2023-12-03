#ifndef EDGEDETECTION_H
#define EDGEDETECTION_H

#include <QtWidgets/QWidget>
#include <qpushbutton.h>
#include <qlayout.h>
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

private:
    QVector<QPushButton*> original;
    QVector<QPushButton*> filteredCPU;
    QVector<QPushButton*> filteredGPU;
    QPushButton* startProccessButton;
    QVBoxLayout* mainLayout;
    QHBoxLayout* originalLayout;
    QHBoxLayout* filteredCPULayout;
    QHBoxLayout* filteredGPULayout;
};

#endif EDGEDETECTION_H