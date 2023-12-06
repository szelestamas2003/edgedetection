#include "EdgeDetection.h"
#include <qfiledialog.h>

EdgeDetection::EdgeDetection(QWidget *parent)
    : QWidget(parent)
{
    startProccessButton = new QPushButton("Start", this);
    title = new QLabel("EdgeDetection", this);
    title->setFont(QFont("Times New Roman", 40, QFont::Bold));
    title->setMargin(10);

    mainLayout = new QVBoxLayout(this);
    originalLayout = new QHBoxLayout();
    filteredCPUImageLayout = new QHBoxLayout();
    filteredGPUImageLayout = new QHBoxLayout();
    headerLayout = new QHBoxLayout();
    headerLayout->addWidget(title);
    headerLayout->addWidget(startProccessButton);
    
    for (int i = 0; i < 5; i++) {
        QPushButton* b = new QPushButton(this);
        b->setIcon(*imageIcon);
        b->setIconSize(QSize(100, 100));
        b->setFixedSize(QSize(100, 100));
        originalLayout->addWidget(b);
        original.push_back(b);
        connect(b, &QPushButton::clicked, this, &EdgeDetection::addImage);
    }

    for (int i = 0; i < 5; i++) {
        QPushButton* b = new QPushButton(this);
        b->setFixedSize(QSize(100, 100));
        b->setDisabled(true);
        b->setFlat(true);
        filteredCPUImageLayout->addWidget(b);
        filteredCPU.push_back(b);
    }

    for (int i = 0; i < 5; i++) {
        QPushButton* b = new QPushButton(this);
        b->setDisabled(true);
        b->setFlat(true);
        b->setFixedSize(QSize(100, 100));
        filteredGPUImageLayout->addWidget(b);
        filteredGPU.push_back(b);
    }

    filteredCPULayout = new QVBoxLayout();
    cpuTitle = new QLabel("Detected on CPU:", this);
    cpuTitle->setFont(QFont("Times New Roman", 25, QFont::DemiBold));
    filteredCPULayout->addWidget(cpuTitle);
    filteredCPULayout->addLayout(filteredCPUImageLayout);

    filteredGPULayout = new QVBoxLayout();
    gpuTitle = new QLabel("Detected on GPU:", this);
    gpuTitle->setFont(QFont("Times New Roman", 25, QFont::DemiBold));
    filteredGPULayout->addWidget(gpuTitle);
    filteredGPULayout->addLayout(filteredGPUImageLayout);

    mainLayout->addLayout(headerLayout);
    mainLayout->addLayout(originalLayout);
    mainLayout->addLayout(filteredCPULayout);
    mainLayout->addLayout(filteredGPULayout);

    setLayout(mainLayout);

    connect(startProccessButton, &QPushButton::clicked, this, &EdgeDetection::startProccess);
}

EdgeDetection::~EdgeDetection()
{}

void EdgeDetection::addImage()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QString fileName =  QFileDialog::getOpenFileName(this, "Select an image", "", "Images (*.png, *.gif, *.jpg)");
    if (!fileName.isEmpty()) {

    }
}

void EdgeDetection::startProccess()
{
    for (int i = 0; i < 5; i++) {

    }
}
