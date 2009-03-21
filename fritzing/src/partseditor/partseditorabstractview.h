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


#ifndef PARTSEDITORABSTRACTVIEW_H_
#define PARTSEDITORABSTRACTVIEW_H_

#include "../sketchwidget.h"
#include "../connectorshared.h"
#include "partseditorpaletteitem.h"

class PartsEditorAbstractView : public SketchWidget {
	Q_OBJECT

	public:
		PartsEditorAbstractView(ViewIdentifierClass::ViewIdentifier, QDir tempDir, bool deleteModelPartOnSceneClear, QWidget *parent=0, int size=150);
		~PartsEditorAbstractView();
		QDir tempFolder();
		bool isEmpty();
		ViewLayer::ViewLayerID connectorLayerId();
		QString terminalIdForConnector(const QString &connId);

	public slots:
		virtual void loadFromModel(PaletteModel *paletteModel, ModelPart * modelPart);
		virtual void addItemInPartsEditor(ModelPart * modelPart, SvgAndPartFilePath * svgFilePath);

	protected:
		virtual PartsEditorPaletteItem *newPartsEditorPaletteItem(ModelPart * modelPart);
		virtual PartsEditorPaletteItem *newPartsEditorPaletteItem(ModelPart * modelPart, SvgAndPartFilePath *path);

		void setDefaultBackground();
		void clearScene();
		virtual void fitCenterAndDeselect();
		void removeConnectors();
		void addDefaultLayers();

		void wheelEvent(QWheelEvent* event);

		ItemBase * addItemAux(ModelPart * modelPart, const ViewGeometry & viewGeometry, long id, long originalModelIndex, AddDeleteItemCommand * originatingCommand, PaletteItem* paletteItem, bool doConnectors);

		ModelPart *createFakeModelPart(SvgAndPartFilePath *svgpath);
		ModelPart *createFakeModelPart(const QHash<QString,StringPair*> &connIds, const QStringList &layers, const QString &svgFilePath);

		const QHash<QString,StringPair*> getConnectorIds(const QString &path);
		void getConnectorIdsAux(QHash<QString,StringPair*> &retval, QDomElement &docElem);
		const QStringList getLayers(const QString &path);

		QString getOrCreateViewFolderInTemp();
		bool ensureFilePath(const QString &filePath);

		QString findConnectorLayerId(QDomDocument *svgDom);
		bool findConnectorLayerIdAux(QString &result, QDomElement &docElem, QStringList &prevLayers);
		bool terminalIdForConnectorIdAux(QString &result, const QString &connId, QDomElement &docElem);
		QString getLayerFileName(ModelPart * modelPart);

		PartsEditorPaletteItem *m_item; // just one item per view
		QDir m_tempFolder;
		bool m_deleteModelPartOnSceneClear;
};

#endif /* PARTSEDITORABSTRACTVIEW_H_ */
