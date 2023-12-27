#include "ImageWindow.h"
#include <qlabel.h>
#include <qfiledialog.h>
#include <qlayout.h>

ImageWindow::ImageWindow(QPixmap pixmap, QWidget *parent)
	: QDialog(parent)
{
	mainMenu = new QMenuBar(this);
	fileMenu = mainMenu->addMenu("File");
	QAction* save = new QAction("Save", this);
	fileMenu->addAction(save);

	this->pixmap = pixmap;

	QLabel* label = new QLabel(this);
	label->setPixmap(pixmap);
	label->setFixedSize(pixmap.size());

	label->move(0, mainMenu->height());

	connect(save, &QAction::triggered, this, &ImageWindow::saveImage);
}

void ImageWindow::saveImage() {
	QString fileName = QFileDialog::getSaveFileName(this, "Save Image", "image.png", "Images (*.png, *.gif, *.jpg)");
	if (!fileName.isEmpty())
		pixmap.save(fileName);
}

ImageWindow::~ImageWindow()
{}
