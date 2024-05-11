#ifndef IMAGEWINDOW_H
#define IMAGEWINDOW_H

#include <QDialog>
#include <qmenubar.h>
#include <qpixmap.h>
#include <qmovie.h>
#include <qfile.h>

class ImageWindow : public QDialog
{
	Q_OBJECT

public:
	ImageWindow(const QString &fileName, QWidget *parent = nullptr);
	~ImageWindow();

private slots:
	void saveImage();

private:
	QMenuBar* mainMenu;
	QMenu* fileMenu;
	QFile* file;
	QMovie* movie;
};

#endif IMAGEWINDOW_H