#include "EdgeDetection.h"
#include <qfiledialog.h>
#include "ImageWindow.h"
#include <qstringlist.h>

EdgeDetection::EdgeDetection(QWidget *parent)
    : QWidget(parent)
{
    startProccessButton = new QPushButton("Start", this);
    resetButton = new QPushButton("Reset", this);
    title = new QLabel("EdgeDetection", this);
    title->setFont(QFont("Times New Roman", 40, QFont::Bold));
    title->setMargin(10);

    mainLayout = new QVBoxLayout(this);
    originalLayout = new QHBoxLayout();
    filteredCPUImageLayout = new QHBoxLayout();
    filteredGPUImageLayout = new QHBoxLayout();
    headerLayout = new QHBoxLayout();
    buttonLayout = new QVBoxLayout();
    buttonLayout->addWidget(startProccessButton);
    buttonLayout->addWidget(resetButton);
    headerLayout->addWidget(title);
    headerLayout->addLayout(buttonLayout);
    
    for (int i = 0; i < 5; i++) {
        QPushButton* b = new QPushButton(this);
        b->setIcon(*imageIcon);
        b->setIconSize(QSize(35, 35));
        b->setFixedSize(QSize(100, 100));
        b->setCursor(Qt::CursorShape::PointingHandCursor);
        originalLayout->addWidget(b);
        original.push_back(b);
        connect(b, &QPushButton::clicked, this, &EdgeDetection::addImage);
    }

    for (int i = 0; i < 5; i++) {
        QPushButton* b = new QPushButton(this);
        b->setFixedSize(QSize(100, 100));
        b->setDisabled(true);
        b->setFlat(true);
        b->setCursor(Qt::CursorShape::PointingHandCursor);
        filteredCPUImageLayout->addWidget(b);
        filteredCPU.push_back(b);
        connect(b, &QPushButton::clicked, this, &EdgeDetection::showImage);
    }

    for (int i = 0; i < 5; i++) {
        QPushButton* b = new QPushButton(this);
        b->setDisabled(true);
        b->setFlat(true);
        b->setFixedSize(QSize(100, 100));
        b->setCursor(Qt::CursorShape::PointingHandCursor);
        filteredGPUImageLayout->addWidget(b);
        filteredGPU.push_back(b);
        connect(b, &QPushButton::clicked, this, &EdgeDetection::showImage);
    }

    fileNames.resize(original.size());
    filteredImagesCPU.resize(original.size());
    filteredImagesGPU.resize(original.size());

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
    connect(resetButton, &QPushButton::clicked, this, &EdgeDetection::resetButtons);
}

EdgeDetection::~EdgeDetection()
{}

void EdgeDetection::resetButtons()
{
    for (QPushButton* b : original) {
        b->setIcon(*imageIcon);
        b->setFlat(false);
        b->setIconSize(QSize(35, 35));
    }

    for (QPushButton* b : filteredCPU) {
        b->setIcon(QIcon());
        b->setDisabled(true);
    }

    for (QPushButton* b : filteredGPU) {
        b->setIcon(QIcon());
        b->setDisabled(true);
    }

    fileNames.clear();
}

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

void EdgeDetection::showImage()
{
    QPushButton* senderButton = qobject_cast<QPushButton*>(sender());
    int index;
    ImageWindow* window;
    if ((index = filteredCPU.indexOf(senderButton)) != -1) {
        window = new ImageWindow(filteredImagesCPU[index], this);
    }
    else if ((index = filteredGPU.indexOf(senderButton)) != -1) {
        window = new ImageWindow(filteredImagesGPU[index], this);
    }
    window->exec();
}

void EdgeDetection::startProccess()
{
    for (int i = 0; i < original.size(); i++) {
        if (!(original[i]->iconSize() == QSize(35, 35))) {
            cv::Mat image = EdgeDetectionAlg::EdgeDetectionOnCPU(fileNames[i].toStdString());
            QPixmap imagePixmap = QPixmap::fromImage(QImage(image.data, image.cols, image.rows, image.step, QImage::Format_Grayscale8));
            filteredImagesCPU[i] = imagePixmap;
            filteredCPU[i]->setIcon(QIcon(imagePixmap));
            filteredCPU[i]->setIconSize(filteredCPU[i]->size());
            filteredCPU[i]->setEnabled(true);
        }
    }
}
