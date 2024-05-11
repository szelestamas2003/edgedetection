#include "EdgeDetection.h"
#include <qfiledialog.h>
#include "ImageWindow.h"
#include <qstringlist.h>
#include <QtConcurrent/qtconcurrentmap.h>
#include <qmovie.h>

EdgeDetection::EdgeDetection(QWidget *parent)
    : QWidget(parent)
{
    QDir().mkdir(dirPath);

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
    movies.resize(original.size());
    moviesCPU.resize(original.size());
    moviesGPU.resize(original.size());

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
    connect(&watcher, &QFutureWatcher<std::vector<QString>>::finished, this, &EdgeDetection::OnEdgeDetectionCompleted);
    connect(&watcher, &QFutureWatcher<QString>::resultReadyAt, this, &EdgeDetection::onItemProcessed);
    connect(resetButton, &QPushButton::clicked, this, &EdgeDetection::resetButtons);
}

EdgeDetection::~EdgeDetection()
{
    for (QMovie* movie : movies) {
        if (movie) {
            movie->disconnect();
            delete movie;
        }
    }
    for (QMovie* movie : moviesCPU) {
        if (movie) {
            movie->disconnect();
            delete movie;
        }
    }
    QDir(dirPath).removeRecursively();
}

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

    for (QMovie* movie : movies) {
        if (movie) {
            movie->disconnect();
            delete movie;
        }
    }
    for (QMovie* movie : moviesCPU) {
        if (movie) {
            movie->disconnect();
            delete movie;
        }
    }

    fileNames.clear();
    movies.clear();
    moviesCPU.clear();
    filteredImagesCPU.clear();
    QDir dir(dirPath);
    for (QString fileName : dir.entryList()) {
        dir.remove(fileName);
    }
}

void EdgeDetection::addImage()
{
    QPushButton* senderButton = qobject_cast<QPushButton*>(sender());
    QStringList fileNames =  QFileDialog::getOpenFileNames(this, "Select an image", "", "Images (*.png *.gif *.jpg)");
    int index = original.indexOf(senderButton);
    if (!fileNames.isEmpty()) {
        int imageCount = fileNames.count();
        int buttons = original.size();
        for (int i = 0; i < buttons && i < imageCount; i++) {
            QPushButton* button = original[(index + i) % buttons];
            if (button->iconSize() != QSize(35, 35) && movies[i] != nullptr) {
                movies[i]->disconnect();
                delete movies[i];
            }
            if (fileNames[i].endsWith("gif")) {
                QMovie* movie = new QMovie(fileNames[i]);
                movies[i] = movie;
                connect(movie, &QMovie::frameChanged, [=] {
                    button->setIcon(movie->currentPixmap());
                    });
                movie->start();
            }
            else
                button->setIcon(QIcon(fileNames[i]));
            this->fileNames[index + i % buttons] = fileNames[i];
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
    delete window;
}

void EdgeDetection::startProccess()
{
    startProccessButton->setDisabled(true);
    resetButton->setDisabled(true);
    watcher.setFuture(QtConcurrent::mapped(fileNames, [this](const QString& fileName) {
        return processItem(fileName);
        }));
}



void EdgeDetection::OnEdgeDetectionCompleted() {
    startProccessButton->setEnabled(true);
    resetButton->setEnabled(true);
}

void EdgeDetection::onItemProcessed(int value)
{
    QString fileName = watcher.future().resultAt(value);
    if (!fileName.isNull()) {
        if (filteredCPU[value]->isEnabled() && moviesCPU[value] != nullptr) {
            delete moviesCPU[value];
        }
        if (fileName.endsWith("gif")) {
            QMovie* movie = new QMovie(fileName);
            moviesCPU[value] = movie;
            connect(movie, &QMovie::frameChanged, [=] {
                filteredCPU[value]->setIcon(movie->currentPixmap());
                });
            movie->start();
        }
        else
            filteredCPU[value]->setIcon(QIcon(fileName));
        filteredImagesCPU[value] = fileName;
        filteredCPU[value]->setIconSize(filteredCPU[value]->size());
        filteredCPU[value]->setEnabled(true);
        filteredCPU[value]->setCursor(Qt::CursorShape::PointingHandCursor);
    }
}

QString EdgeDetection::processItem(const QString& fileName)
{
    QString newfileName;
    if (!fileName.isEmpty()) {
        QFileInfo fileInfo(fileName);
        newfileName = dirPath + fileInfo.baseName() + "_black." + fileInfo.completeSuffix();
        algorithm.RunEdgeDetectionOnCPU(fileName.toStdString(), newfileName.toStdString());
    }
    else
        newfileName = nullptr;
    return newfileName;
}

