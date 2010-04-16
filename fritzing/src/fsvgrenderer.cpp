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

#include "fsvgrenderer.h"
#include "debugdialog.h"
#include "svg/svgfilesplitter.h"
#include "utils/textutils.h"

#include <QRegExp>
#include <QTextStream>
#include <QPainter>
#include <QCoreApplication>
#include <QGraphicsSvgItem>

QHash<QString, RendererHash *> FSvgRenderer::m_moduleIDRendererHash;
QHash<QString, RendererHash * > FSvgRenderer::m_filenameRendererHash;
QSet<RendererHash * > FSvgRenderer::m_deleted;

qreal FSvgRenderer::m_printerScale = 90.0;

static ConnectorInfo VanillaConnectorInfo;

FSvgRenderer::FSvgRenderer(QObject * parent) : QSvgRenderer(parent)
{
	m_defaultSizeF = QSizeF(0,0);
}

FSvgRenderer::~FSvgRenderer()
{
	foreach (ConnectorInfo * connectorInfo, m_connectorInfoHash.values()) {
		delete connectorInfo;
	}
	m_connectorInfoHash.clear();
}

void FSvgRenderer::cleanup() {
	foreach (RendererHash * rendererHash, m_filenameRendererHash.values()) {
		foreach (FSvgRenderer * renderer, rendererHash->values()) {
			delete renderer;
		}
		delete rendererHash;
	}
	m_filenameRendererHash.clear();
	foreach (RendererHash * rendererHash, m_moduleIDRendererHash.values()) {
		delete rendererHash;
	}
	m_moduleIDRendererHash.clear();

	foreach (RendererHash * rendererHash, m_deleted) {
		delete rendererHash;
	}
	m_deleted.clear();
}

bool FSvgRenderer::loadSvg(const QString & filename) {
	QStringList strings;
	QString string;
	return loadSvg(filename, strings, strings, string, string);
}

bool FSvgRenderer::loadSvg(const QString & filename, const QStringList & connectorIDs, const QStringList & terminalIDs, const QString & setColor, const QString & colorElementID) {
	if (!QFileInfo(filename).exists() || !QFileInfo(filename).isFile()) {
		return false;
	}

    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
		return false;
	}

	QByteArray contents = file.readAll();
	file.close();

	if (contents.length() <= 0) return false;

	return loadAux(contents, filename, connectorIDs, terminalIDs, setColor, colorElementID);

	/*

	QXmlStreamReader xml(contents);
	determineDefaultSize(xml);

	if (readConnectors) {
		file.seek(0);
		m_svgXml = file.readAll();
	}
	file.close();

	bool result = QSvgRenderer::load(filename);
	if (result) {
		m_filename = filename;
	}
	return result;
	*/

}

bool FSvgRenderer::loadSvg(const QByteArray & contents, const QString & filename) {
	QStringList strings;
	QString string;
	return loadSvg(contents, filename, strings, strings, string, string);
}

bool FSvgRenderer::loadSvg(const QByteArray & contents, const QString & filename, const QStringList & connectorIDs, const QStringList & terminalIDs, const QString & setColor, const QString & colorElementID) {
	return loadAux(contents, filename, connectorIDs, terminalIDs, setColor, colorElementID);
}

bool FSvgRenderer::loadAux(const QByteArray & contents, const QString & filename, const QStringList & connectorIDs, const QStringList & terminalIDs, const QString & setColor, const QString & colorElementID) {

	QByteArray cleanContents;
	bool cleaned = false;
	if (contents.contains("Illustrator")) {
		QString string(contents);
		if (TextUtils::fixPixelDimensionsIn(string)) {
			cleaned = true;
			cleanContents = string.toUtf8();
			//DebugDialog::debug("Illustrator " + filename);
		}
	}
	if (!cleaned) {
		cleanContents = contents; 
	}

	// no it isn't

	if (connectorIDs.count() > 0 || !setColor.isEmpty()) {
		QString errorStr;
		int errorLine;
		int errorColumn;
		QDomDocument doc;
		doc.setContent(cleanContents, &errorStr, &errorLine, &errorColumn);
		QDomElement root = doc.documentElement();
		if (!setColor.isEmpty()) {
			QDomElement element = TextUtils::findElementWithAttribute(root, "id", colorElementID);
			if (!element.isNull()) {
				SvgFileSplitter::fixColorRecurse(element, setColor);
				cleanContents = TextUtils::removeXMLEntities(doc.toString()).toUtf8();
			}
		}
		if (connectorIDs.count() > 0) {
			initConnectorInfo(doc, connectorIDs, terminalIDs);
		}
	}


	/*
	cleanContents = doc.toByteArray();

	//QFile file("all.txt");
	//if (file.open(QIODevice::Append)) {
		//QTextStream t(&file);
		//t << cleanContents;
		//file.close();
	//}

	*/

	//DebugDialog::debug(cleanContents.data());

	QXmlStreamReader xml(cleanContents);
	determineDefaultSize(xml);

	/*
	QString path = QCoreApplication::applicationDirPath();
	path += "/../boom.svg";
	QFile file(path);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream out(&file);
		out << QString(cleanContents);
		file.close();
	}
	*/


	bool result = QSvgRenderer::load(cleanContents);
	if (result) {
		m_filename = filename;
	}
	return result;
}

bool FSvgRenderer::fastLoad(const QByteArray & contents) {
	return QSvgRenderer::load(contents);
}

const QString & FSvgRenderer::filename() {
	return m_filename;
}

FSvgRenderer * FSvgRenderer::getByFilename(const QString & filename, ViewLayer::ViewLayerID viewLayerID) {
	RendererHash * rendererHash = m_filenameRendererHash.value(filename);
	if (rendererHash == NULL) return NULL;

	return rendererHash->value(viewLayerID, NULL);
}

FSvgRenderer * FSvgRenderer::getByModuleID(const QString & moduleID, ViewLayer::ViewLayerID viewLayerID) {
	RendererHash * rendererHash = m_moduleIDRendererHash.value(moduleID);
	if (rendererHash == NULL) return NULL;

	return rendererHash->value(viewLayerID, NULL);
}

QPixmap * FSvgRenderer::getPixmap(const QString & moduleID, ViewLayer::ViewLayerID viewLayerId, QSize size) {
	// TODO: cache pixmap by size?

	QPixmap *pixmap = NULL;
	FSvgRenderer * renderer = getByModuleID(moduleID, viewLayerId);
	if (renderer) {
		pixmap = new QPixmap(size);
		pixmap->fill(Qt::transparent);
		QPainter painter(pixmap);
		// preserve aspect ratio
		QSizeF def = renderer->defaultSizeF();
		qreal newW = size.width();
		qreal newH = newW * def.height() / def.width();
		if (newH > size.height()) {
			newH = size.height();
			newW = newH * def.width() / def.height();
		}
		QRectF bounds((size.width() - newW) / 2.0, (size.height() - newH) / 2.0, newW, newH);
		renderer->render(&painter, bounds);
		painter.end();
	}
	return pixmap;
}

void FSvgRenderer::set(const QString & moduleID, ViewLayer::ViewLayerID viewLayerID, FSvgRenderer * renderer) {
	RendererHash * rendererHash = m_filenameRendererHash.value(renderer->filename());
	if (rendererHash == NULL) {
		rendererHash = new RendererHash();
		m_filenameRendererHash.insert(renderer->filename(), rendererHash);
	}
	rendererHash->insert(viewLayerID, renderer);
	rendererHash = m_moduleIDRendererHash.value(moduleID);
	if (rendererHash == NULL) {
		rendererHash = new RendererHash();
		m_moduleIDRendererHash.insert(moduleID, rendererHash);
	}
	rendererHash->insert(viewLayerID, renderer);
}

void FSvgRenderer::determineDefaultSize(QXmlStreamReader & xml)
{
	QSizeF size = parseForWidthAndHeight(xml);
	m_defaultSizeF = QSizeF(size.width() * m_printerScale, size.height() * m_printerScale);
}

QSizeF FSvgRenderer::parseForWidthAndHeight(QXmlStreamReader & xml)
{
    xml.setNamespaceProcessing(false);

	QSizeF size(0,0);

	while (!xml.atEnd()) {
        switch (xml.readNext()) {
        case QXmlStreamReader::StartElement:
			if (xml.name().toString().compare("svg") == 0) {
				QString ws = xml.attributes().value("width").toString();
				QString hs = xml.attributes().value("height").toString();
				bool ok;
				qreal w = TextUtils::convertToInches(ws, &ok);
				if (!ok) return size;

				qreal h = TextUtils::convertToInches(hs, &ok);
				if (!ok) return size;

				size.setWidth(w);
				size.setHeight(h);
				return size;
			}
			return size;
		default:
			break;
		}
	}

	return size;
}

QSizeF FSvgRenderer::defaultSizeF() {
	if (m_defaultSizeF.width() == 0 && m_defaultSizeF.height() == 0) {
		return defaultSize();
	}

	return m_defaultSizeF;
}

void FSvgRenderer::calcPrinterScale() {

	// note: I think that printerScale is probably just 90 dpi, since the calculation
	// result is 89.8407 for the breadboard svg across all three platforms 
	// note: calculation result depends on the svg used; if the svg size is a float, the scale will vary a little
	// using an svg file with exactly a 1-inch width (like 'wire.svg') gives exactly a 90.0 printerscale value.

	VanillaConnectorInfo.gotCircle = false;				// TODO: only needs to happen once
	m_printerScale = 90.0;

	/*

	QString filename(":/resources/parts/svg/core/breadboard/wire.svg");
	
	QGraphicsSvgItem item(filename);
	QRectF b = item.boundingRect();
	QFile file(filename);
	file.open(QFile::ReadOnly);
	QXmlStreamReader xml(&file);
	QSizeF size = parseForWidthAndHeight(xml);
	if (size.width() <= 0) return;

	qreal pscale = b.width() / size.width();
	DebugDialog::debug(QString("printerscale %1").arg(pscale));

	*/
}

qreal FSvgRenderer::printerScale() {
	return m_printerScale;
}

void FSvgRenderer::initConnectorInfo(QDomDocument & domDocument, const QStringList & connectorIDs, const QStringList & terminalIDs)
{
	QDomElement root = domDocument.documentElement();
	QDomElement cachedElement = root;

	for (int i = 0; i < connectorIDs.count(); i++) {
		QString connectorID = connectorIDs[i];
		QString terminalID = terminalIDs[i];

		ConnectorInfo * connectorInfo = new ConnectorInfo();
		connectorInfo->radius = connectorInfo->strokeWidth = 0;
		connectorInfo->gotCircle = false;
		m_connectorInfoHash.insert(connectorID, connectorInfo);

		QDomElement connectorElement = TextUtils::findElementWithAttribute(cachedElement, "id", connectorID);
		if (connectorElement.isNull()) {
			connectorElement = TextUtils::findElementWithAttribute(root, "id", connectorID);
		}
		if (connectorElement.isNull()) continue;
	
		cachedElement = connectorElement.parentNode().toElement();
		QDomElement terminalElement;
		if (!terminalID.isEmpty()) {
			terminalElement = TextUtils::findElementWithAttribute(cachedElement, "id", terminalID);
			if (terminalElement.isNull()) {
				terminalElement = TextUtils::findElementWithAttribute(root, "id", terminalID);
			}
		}

		if (!terminalElement.isNull()) {
			connectorInfo->terminalMatrix = SvgFileSplitter::elementToMatrix(terminalElement);
		}

		if (connectorElement.isNull()) continue;

		connectorInfo->matrix = SvgFileSplitter::elementToMatrix(connectorElement);

		// right now we only handle circles
		if (connectorElement.nodeName().compare("circle") != 0) continue;

		bool ok;
		qreal cx = connectorElement.attribute("cx").toDouble(&ok);
		if (!ok) continue;

		qreal cy = connectorElement.attribute("cy").toDouble(&ok);
		if (!ok) continue;

		qreal r = connectorElement.attribute("r").toDouble(&ok);
		if (!ok) continue;

		qreal sw = connectorElement.attribute("stroke-width").toDouble(&ok);	
		if (!ok) {
			//QString strokewidth("stroke-width");
			//QString s = element.attribute("style");
			//SvgFileSplitter::fixStyleAttribute(connectorElement, s, strokewidth);
			sw = connectorElement.attribute("stroke-width").toDouble(&ok);
			if (!ok) {
				continue;
			}
		}

		connectorInfo->gotCircle = true;
		connectorInfo->bounds.setRect(cx - r - (sw / 2.0), cy - r - (sw / 2.0), (r * 2) + sw, (r * 2) + sw);
		connectorInfo->radius = r;
		connectorInfo->strokeWidth = sw;
	}
}

void FSvgRenderer::removeFromHash(const QString &moduleId, const QString filename) {
	//DebugDialog::debug(QString("length before %1").arg(m_moduleIDRendererHash.size()));
	RendererHash * r = m_moduleIDRendererHash.take(moduleId);
	if (r != NULL) {
		m_deleted.insert(r);
	}
	//DebugDialog::debug(QString("length after %1").arg(m_moduleIDRendererHash.size()));
	r = m_filenameRendererHash.take(filename);
	if (r != NULL) {
		m_deleted.insert(r);
	}
}

ConnectorInfo * FSvgRenderer::getConnectorInfo(const QString & connectorID) {
	return m_connectorInfoHash.value(connectorID, &VanillaConnectorInfo);
}
