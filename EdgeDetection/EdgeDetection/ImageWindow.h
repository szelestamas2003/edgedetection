#pragma once

#include <QDialog>
#include <qmenubar.h>
#include <qpixmap.h>

class ImageWindow : public QDialog
{
	Q_OBJECT

public:
	ImageWindow(QPixmap pixmap, QWidget *parent = nullptr);
	~ImageWindow();

private slots:
	void saveImage();

private:
	QMenuBar* mainMenu;
	QMenu* fileMenu;
	QPixmap pixmap;
};
