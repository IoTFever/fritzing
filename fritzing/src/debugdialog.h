/*******************************************************************

Part of the Fritzing project - http://fritzing.org
Copyright (c) 2007-08 Fachhochschule Potsdam - http://fh-potsdam.de

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

$Revision$:
$Author$:
$Date$

********************************************************************/


#ifndef DEBUGDIALOG_H
#define DEBUGDIALOG_H

#include <QDialog>
#include <QEvent>
#include <QTextEdit>
#include <QFile>

class DebugDialog : public QDialog
{
	Q_OBJECT

private:
	DebugDialog(QWidget *parent = 0);
	~DebugDialog();

public:
	static void debug(QString, QObject * ancestor = 0);
	static void hideDebug();
	static void showDebug();
	static void closeDebug();
	static bool visible();
	static bool connectToBroadcast(QObject * receiver, const char* slot); 

protected:
	bool event ( QEvent * e ); 
	void resizeEvent ( QResizeEvent * event );

protected:
	static DebugDialog* singleton;
	static QFile m_file;
	
	QTextEdit* m_textEdit;
	
signals:
	void debugBroadcast(const QString & message, QObject * ancestor);
};

#endif 
