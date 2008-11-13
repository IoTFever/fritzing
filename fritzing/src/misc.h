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

#ifndef MISC_H
#define MISC_H

#include <QPair>
#include <QString>
#include <QDir>
#include <QDomElement>
#include <QStringList>
#include <QHash>

template <class T1, class T2, class T3>
struct Triple {
public:
	Triple() {}

	Triple(T1 _first, T2 _second, T3 _third) {
		first = _first;
		second = _second;
		third = _third;
	}

	T1 first;
	T2 second;
	T3 third;
};

typedef QPair<qreal, qreal> RealPair;
typedef QPair<QString, QString> StringPair;
typedef Triple<QString, QString, QString> StringTriple;

QString getUserPartsFolder();
QDir *getApplicationSubFolder(QString);
QString getApplicationSubFolderPath(QString);

static QString ___emptyString___;
static QDomElement ___emptyElement___;
static QStringList ___emptyStringList___;
static QHash<QString, QString> ___emptyStringHash___;
static QDir ___emptyDir___;

#ifdef Q_WS_MAC
static const QString ___MacStyle___ = " QTabBar::tab {margin-bottom: 10px; min-width: 15ex;} ";
#else
static const QString ___MacStyle___ = "";
#endif

#endif
