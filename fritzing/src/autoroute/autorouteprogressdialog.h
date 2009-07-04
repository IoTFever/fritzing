/*******************************************************************

Part of the Fritzing project - http://fritzing.org
Copyright (c) 2007-2009 Fachhochschule Potsdam - http://fh-potsdam.de

Fritzing is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Fritzing is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Fritzing.  If not, see <http://www.gnu.org/licenses/>.

********************************************************************

$Revision: 2597 $:
$Author: cohen@irascible.com $:
$Date: 2009-03-10 12:44:55 +0100 (Tue, 10 Mar 2009) $

********************************************************************/


#ifndef AUTOROUTEPROGRESSDIALOG_H
#define AUTOROUTEPROGRESSDIALOG_H

#include <QDialog>
#include <QProgressBar>
#include <QLabel>

class AutorouteProgressDialog : public QDialog
{
Q_OBJECT

public:
	AutorouteProgressDialog(class ZoomableGraphicsView * view, QWidget *parent = 0);
	~AutorouteProgressDialog();

protected:
	void closeEvent(QCloseEvent *);
	void wheelEvent(QWheelEvent *);
	void mouseMoveEvent(QMouseEvent *);

public slots:
	void setMinimum(int);
	void setMaximum(int);
	void setValue(int);
	void sendSkip();
	void sendCancel();
	void sendStop();

signals:
	void skip();
	void cancel();
	void stop();

protected:
	QProgressBar * m_progressBar;	
};

class ArrowButton : public QLabel {
	Q_OBJECT

public:
	ArrowButton(int scrollX, int scrollY, ZoomableGraphicsView * view, const QString & path);

protected:
	void mousePressEvent(QMouseEvent *event);

protected:
	int m_scrollX;
	int m_scrollY;
	ZoomableGraphicsView * m_view;
};


#endif 
