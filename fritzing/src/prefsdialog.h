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

$Revision$:
$Author$:
$Date$

********************************************************************/


#ifndef PREFSDIALOG_H
#define PREFSDIALOG_H

#include <QDialog>
#include <QFile>
#include <QAbstractListModel>
#include <QFileInfoList>
#include <QLocale>
#include <QHash>

class TranslatorListModel : public  QAbstractListModel
{
	Q_OBJECT

public:
	TranslatorListModel(QFileInfoList &, QObject* parent = 0);
	~TranslatorListModel();

	QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	int rowCount ( const QModelIndex & parent = QModelIndex() ) const ;

	const QLocale * locale(int index);
	int findIndex(const QString & language);

protected:
	QList<QLocale *> m_localeList;

	static QHash<QString, QString> m_languages;
};

class PrefsDialog : public QDialog
{
	Q_OBJECT

public:
	PrefsDialog(const QString & language, QFileInfoList & list, QWidget *parent = 0);
	~PrefsDialog();

	const QString & name();
	bool cleared();

protected:
	QWidget * createLanguageForm(QFileInfoList & list);
	QWidget* createOtherForm();

protected slots:
	void changeLanguage(int);
	void clear();

protected:
	QString m_name;
	TranslatorListModel * m_translatorListModel;
	bool m_cleared;
};

#endif 
