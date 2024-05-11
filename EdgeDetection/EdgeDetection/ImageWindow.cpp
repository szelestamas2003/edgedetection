#include "ImageWindow.h"
#include <qlabel.h>
#include <qfiledialog.h>
#include <qlayout.h>
#include <qscreen.h>
#include <qscrollarea.h>
#include <qwindow.h>

ImageWindow::ImageWindow(const QString& fileName, QWidget *parent)
	: QDialog(parent)
{
	mainMenu = new QMenuBar(this);
	fileMenu = mainMenu->addMenu("File");
	QAction* save = new QAction("Save", this);
	fileMenu->addAction(save);
	movie = nullptr;

	this->file = new QFile(fileName, this);

	QLabel* label = new QLabel(this);
	QScrollArea* scrollarea = new QScrollArea(this);
	scrollarea->setWidget(label);
	if (fileName.endsWith("gif")) {
		movie = new QMovie(fileName);
		connect(movie, &QMovie::frameChanged, [=] {
			label->setPixmap(movie->currentPixmap());
			});
		movie->start();
	} else
		label->setPixmap(QPixmap(fileName));
	label->setFixedSize(label->pixmap().size());
	QScreen* pActive = nullptr;
	QWidget* pWidget = this;
	while (pWidget)
	{
		auto w = pWidget->windowHandle();
		if (w != nullptr)
		{
			pActive = w->screen();
			break;
		}
		else
			pWidget = pWidget->parentWidget();
	}
	scrollarea->setMaximumSize(QSize(pActive->availableSize().width() - 10, pActive->availableSize().height() - 10));
	scrollarea->setFixedSize(label->pixmap().size().height() > pActive->availableSize().height() ? QSize(pActive->availableSize().width(), pActive->availableSize().height() - 60) : QSize(label->pixmap().size().width() + 2, label->pixmap().size().height() + 2));

	scrollarea->move(0, mainMenu->height());

	connect(save, &QAction::triggered, this, &ImageWindow::saveImage);
}

void ImageWindow::saveImage() {
	QString saveFileName = QFileInfo(file->fileName()).fileName();
	QString fileName = QFileDialog::getSaveFileName(this, "Save Image", saveFileName, "Images (*.png *.gif *.jpg)");
	if (!fileName.isEmpty())
		file->copy(fileName);
}

ImageWindow::~ImageWindow()
{
	if (movie) {
		movie->disconnect();
		delete movie;
	}
}
