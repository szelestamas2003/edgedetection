#include "EdgeDetection.h"

EdgeDetection::EdgeDetection(QWidget *parent)
    : QWidget(parent)
{
    startProccessButton = new QPushButton("Start", this);

    mainLayout = new QVBoxLayout(this);
    originalLayout = new QHBoxLayout();
    filteredCPULayout = new QHBoxLayout();
    filteredGPULayout = new QHBoxLayout();
    
    for (int i = 0; i < 5; i++) {
        QPushButton* b = new QPushButton(this);
        b->setFixedSize(QSize(100, 100));
        originalLayout->addWidget(b);
        original.push_back(b);
    }

    for (int i = 0; i < 5; i++) {
        QPushButton* b = new QPushButton(this);
        b->setFixedSize(QSize(100, 100));
        filteredCPULayout->addWidget(b);
        filteredCPU.push_back(b);
    }

    for (int i = 0; i < 5; i++) {
        QPushButton* b = new QPushButton(this);
        b->setFixedSize(QSize(100, 100));
        filteredGPULayout->addWidget(b);
        filteredGPU.push_back(b);
    }

    mainLayout->addWidget(startProccessButton);
    mainLayout->addLayout(originalLayout);
    mainLayout->addLayout(filteredCPULayout);
    mainLayout->addLayout(filteredGPULayout);

    setLayout(mainLayout);

    connect(startProccessButton, &QPushButton::clicked, this, &EdgeDetection::startProccess);
}

EdgeDetection::~EdgeDetection()
{}

void EdgeDetection::startProccess()
{
    for (QPushButton* b : filteredCPU) {
        b->setText("Done");
    }
}
