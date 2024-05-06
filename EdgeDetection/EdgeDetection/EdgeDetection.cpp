#include "EdgeDetection.h"
#include <qfiledialog.h>
#include <qstringlist.h>
#include "EdgeDetectionAlg.h"

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
        b->setIconSize(QSize(35, 35));
        b->setFixedSize(QSize(100, 100));
        b->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
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

    fileNames.resize(original.size());

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
    QPushButton* senderButton = qobject_cast<QPushButton*>(sender());
    QStringList fileNames =  QFileDialog::getOpenFileNames(this, "Select an image", "", "Images (*.png, *.gif, *.jpg)");
    int index = original.indexOf(senderButton);
    if (!fileNames.isEmpty()) {
        int imageCount = fileNames.count();
        int buttons = original.size();
        for (int i = 0; i < buttons && i < imageCount; i++) {
            QPushButton* button = original[(index + i) % buttons];
            button->setIcon(QIcon(fileNames[i]));
            this->fileNames[i] = fileNames[i];
            button->setIconSize(button->size());
            button->setFlat(true);
        }
    }
}

void EdgeDetection::startProccess()
{
    for (int i = 0; i < original.size(); i++) {
        if (!(original[i]->iconSize() == QSize(35, 35))) {
            cv::Mat image = EdgeDetectionAlg::EdgeDetectionOnCPU(fileNames[i].toStdString());
            filteredCPU[i]->setIcon(QIcon(QPixmap::fromImage(QImage(image.data, image.cols, image.rows, image.step, QImage::Format_Grayscale8))));
            filteredCPU[i]->setIconSize(filteredCPU[i]->size());
            filteredCPU[i]->setEnabled(true);
            filteredCPU[i]->setCursor(Qt::CursorShape::PointingHandCursor);
        }
    }
}
