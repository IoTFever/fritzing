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

$Revision: 1794 $:
$Author: merunga $:
$Date: 2008-12-11 14:50:11 +0100 (Thu, 11 Dec 2008) $

********************************************************************/


#ifndef DOCKMANAGER_H_
#define DOCKMANAGER_H_

#include <QObject>

#include "mainwindow.h"

class DockManager : public QObject {
	Q_OBJECT
	public:
		DockManager(MainWindow *mainWindow);
		void createBinAndInfoViewDocks();
		void createDockWindows();

	protected slots:
		void dockChangeActivation(bool activate);
		void ensureSpaceForSizeGrip(Qt::DockWidgetArea area);

	protected:
		class FDockWidget * makeDock(const QString & title, QWidget * widget, int dockMinHeight, int dockDefaultHeight, Qt::DockWidgetArea area = Qt::RightDockWidgetArea);
		class FDockWidget * dockIt(FDockWidget* dock, int dockMinHeight, int dockDefaultHeight, Qt::DockWidgetArea area = Qt::RightDockWidgetArea);
		//class FDockWidget * createSizeGripDock();

	protected:
		MainWindow *m_mainWindow;

	public:
		static const int PartsBinDefaultHeight = 220;
		static const int PartsBinMinHeight = 122;
		static const int NavigatorDefaultHeight = 70;
		static const int NavigatorMinHeight = NavigatorDefaultHeight;
		static const int InfoViewDefaultHeight = 150;
		static const int InfoViewMinHeight = 50;
		static const int UndoHistoryDefaultHeight = 70;
		static const int UndoHistoryMinHeight = UndoHistoryDefaultHeight;
	public:
		static const int DockDefaultWidth = 185;
		static const int DockMinWidth = 130;
		static const int DockDefaultHeight = 50;
		static const int DockMinHeight = 30;
};

#endif /* DOCKMANAGER_H_ */
