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



//#include <QXmlQuery>
#include <QInputDialog>

#include "partseditorconnectorviewimagewidget.h"
#include "../fsvgrenderer.h"
#include "../fritzingwindow.h"
#include "../debugdialog.h"

int PartsEditorConnectorViewImageWidget::ConnDefaultWidth = 5;
int PartsEditorConnectorViewImageWidget::ConnDefaultHeight = ConnDefaultWidth;

PartsEditorConnectorViewImageWidget::PartsEditorConnectorViewImageWidget(ItemBase::ViewIdentifier viewId, QWidget *parent, int size)
	: PartsEditorAbstractViewImage(viewId, true /*show terminal points*/, parent, size)
{
	m_connRubberBandOrigin = QPoint(-1,-1);
	m_connRubberBand = NULL;

	m_zoomControls = new ZoomControls(this);
	addFixedToBottomRightItem(m_zoomControls);

	//m_connFreeDrawingEnabled = false;
	//if(m_connFreeDrawingEnabled) {
		//setDragMode(QGraphicsView::RubberBandDrag);
	//} else {
		setDragMode(QGraphicsView::ScrollHandDrag);
	//}
}

void PartsEditorConnectorViewImageWidget::wheelEvent(QWheelEvent* event) {
	SketchWidget::wheelEvent(event);
}

void PartsEditorConnectorViewImageWidget::mousePressEvent(QMouseEvent *event) {
	PartsEditorAbstractViewImage::mousePressEvent(event);
	/*if(m_connFreeDrawingEnabled && m_item) {
		m_connRubberBandOrigin = event->pos();
		if (!m_connRubberBand) {
			m_connRubberBand = new QRubberBand(QRubberBand::Rectangle, this);
		}

		m_connRubberBand->setGeometry(QRect(m_connRubberBandOrigin, QSize()));
		m_connRubberBand->show();
	}*/
}

void PartsEditorConnectorViewImageWidget::mouseMoveEvent(QMouseEvent *event) {
	QGraphicsView::mouseMoveEvent(event);
	/*if(m_connFreeDrawingEnabled && m_item && m_connRubberBandOrigin != QPoint(-1,-1)) {
		m_connRubberBand->setGeometry(QRect(m_connRubberBandOrigin, event->pos()).normalized());
	}*/
}

void PartsEditorConnectorViewImageWidget::mouseReleaseEvent(QMouseEvent *event) {
	/*if(m_connFreeDrawingEnabled && m_item) {
		m_connRubberBand->hide();
		createConnector(m_connRubberBand->geometry());
		m_connRubberBandOrigin = QPoint(-1,-1);
	}*/
	PartsEditorAbstractViewImage::mouseReleaseEvent(event);
}

void PartsEditorConnectorViewImageWidget::drawConector(Connector *conn, bool showTerminalPoint) {
	QSize size(ConnDefaultWidth,ConnDefaultHeight);
	createConnector(conn,size,showTerminalPoint);
}

void PartsEditorConnectorViewImageWidget::createConnector(Connector *conn, const QSize &connSize, bool showTerminalPoint) {
	Q_ASSERT(m_item);
	QString connId = conn->connectorStuffID();

	QRectF bounds = QRectF(m_item->boundingRect().center(),connSize);
	PartsEditorConnectorItem *connItem = new PartsEditorConnectorItem(conn, m_item, m_showsTerminalPoints, bounds);
	m_drawnConns << connItem;
	connItem->setShowTerminalPoint(showTerminalPoint);

	m_undoStack->push(new QUndoCommand(
		QString("connector '%1' added to %2 view")
		.arg(connId).arg(ItemBase::viewIdentifierName(m_viewIdentifier))
	));
}

void PartsEditorConnectorViewImageWidget::removeConnector(const QString &connId) {
	PartsEditorConnectorItem *connToRemove = NULL;
	foreach(QGraphicsItem *item, items()) {
		PartsEditorConnectorItem *connItem = dynamic_cast<PartsEditorConnectorItem*>(item);
		if(connItem && connItem->connector()->connectorStuffID() == connId) {
			connToRemove = connItem;
			break;
		}
	}

	if(connToRemove) {
		scene()->removeItem(connToRemove);
		scene()->update();
		m_undoStack->push(new QUndoCommand(
			QString("connector '%1' removed from %2 view")
			.arg(connId).arg(ItemBase::viewIdentifierName(m_viewIdentifier))
		));
		m_removedConnIds << connId;
	}
}

void PartsEditorConnectorViewImageWidget::loadFromModel(PaletteModel *paletteModel, ModelPart * modelPart) {
	PartsEditorAbstractViewImage::loadFromModel(paletteModel, modelPart);
	setItemProperties();
}

void PartsEditorConnectorViewImageWidget::addItemInPartsEditor(ModelPart * modelPart, StringPair * svgFilePath) {
	QString imagePath = svgFilePath->first+(svgFilePath->second != ___emptyString___ ? "/" : "")+svgFilePath->second;
	if(!modelPart) {
		modelPart = createFakeModelPart(imagePath, svgFilePath->second);
	}

	PartsEditorAbstractViewImage::addItemInPartsEditor(modelPart,svgFilePath);
	setItemProperties();
	//m_item->removeFromModel();

	emit connectorsFound(this->m_viewIdentifier,m_item->connectors());

	//m_item->highlightConnectors("");
}

void PartsEditorConnectorViewImageWidget::setItemProperties() {
	if(m_item) {
		m_item->setFlag(QGraphicsItem::ItemIsSelectable, false);
		m_item->setFlag(QGraphicsItem::ItemIsMovable, false);
		m_item->setFlag(QGraphicsItem::ItemClipsToShape, true);
		//m_item->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
		m_item->removeFromModel();
		m_item->highlightConnectors("");

		//qreal size = 500; // just make sure the user get enough place to play
		//setSceneRect(0,0,size,size);

		//m_item->setPos((size-m_item->size().width())/2,(size-m_item->size().height())/2);
		centerOn(m_item);
		m_item->setWithBorder(true);
	}
	ensureFixedToBottomRight(m_zoomControls);
}

void PartsEditorConnectorViewImageWidget::informConnectorSelection(const QString &connId) {
	if(m_item) {
		m_item->highlightConnectors(connId);
	}
}

void PartsEditorConnectorViewImageWidget::setMismatching(ItemBase::ViewIdentifier viewId, const QString &id, bool mismatching) {
	if(m_item && viewId == m_viewIdentifier) {
		for (int i = 0; i < m_item->childItems().count(); i++) {
			PartsEditorConnectorItem * connectorItem = dynamic_cast<PartsEditorConnectorItem *>(m_item->childItems()[i]);
			if(connectorItem == NULL) continue;

			if(connectorItem->connector()->connectorStuffID() == id) {
				connectorItem->setMismatching(mismatching);
			}
		}
	}
}

void PartsEditorConnectorViewImageWidget::updateDomIfNeeded() {
	if(m_item) {
		FSvgRenderer *renderer = new FSvgRenderer();
		if(renderer->load(m_item->flatSvgFilePath())) {
			QRectF viewBox = renderer->viewBoxF();
			QSizeF defaultSize = renderer->defaultSizeF();
			QDomDocument *svgDom = m_item->svgDom();

			if(!m_drawnConns.isEmpty()) {
				DebugDialog::debug(QString("<<<< dsW %1  dsH %2  vbW %3  vbH %4")
						.arg(defaultSize.width()).arg(defaultSize.height())
						.arg(viewBox.width()).arg(viewBox.height()));

				QRectF bounds;
				QString connId;

				foreach(PartsEditorConnectorItem* drawnConn, m_drawnConns) {
					QRectF rectAux = drawnConn->boundingRect();
					QPointF posAux = QPointF(rectAux.x(),rectAux.y());
					DebugDialog::debug(QString("<<<< rect pos x=%1  y=%2 w=%3 h=%4")
							.arg(rectAux.x()).arg(rectAux.y()).arg(rectAux.width()).arg(rectAux.height()));
					qreal xAux = posAux.x();
					qreal yAux = posAux.y();
					bounds = QRectF(xAux, yAux, rectAux.width(), rectAux.height());
					connId = drawnConn->connectorStuffID();

					QRectF svgRect = mapFromSceneToSvg(bounds,defaultSize,viewBox);
					addRectToSvg(svgDom,connId/*+"pin"*/,svgRect);

					TerminalPointItem *tp = drawnConn->terminalPointItem();
					if(tp && tp->hasBeenMoved()) {
						QRectF rectTPAux = tp->boundingRect();
						QPointF posTPAux = QPointF(
							rectTPAux.x()+rectTPAux.width()/2,
							rectTPAux.y()+rectTPAux.height()/2
						);
						qreal halfTPSize = 0.001; // a tiny rectangle
						QRectF tpointRect(
							tp->mapToParent(posTPAux).x()-halfTPSize,
							tp->mapToParent(posTPAux).y()-halfTPSize,
							halfTPSize*2, halfTPSize*2
						);
						QRectF svgTpRect = mapFromSceneToSvg(tpointRect,defaultSize,viewBox);
						addRectToSvg(svgDom,connId+"terminal",svgTpRect);
					}
				}
			}

			if(!m_removedConnIds.isEmpty()) {
				QDomElement docEle = svgDom->documentElement();
				if (docEle.tagName() != "svg") return;
				QString result;
				QList<QDomNode> nodesToRemove;
				for (int i = 0; i < docEle.childNodes().count(); ++i) {
					QDomNode n = docEle.childNodes().at(i);
					if (n.nodeType() == QDomNode::ElementNode) {
						if (isSupposedToBeRemoved(n.toElement().attribute("id"))) {
							docEle.removeChild(n);
							continue;
						}

						QDomNodeList children = n.toElement().childNodes();
						for (int c = 0; c < children.count(); ++c) {
							QDomNode child = children.at(c);
							if (child.nodeType() == QDomNode::ElementNode
								&& isSupposedToBeRemoved(child.toElement().attribute("id"))) {
								n.removeChild(child);
								continue;
							}
						}
					}
				}
			}

			QString tempFile = QDir::tempPath()+"/"+FritzingWindow::getRandText()+".svg";
			QFile file(tempFile);
			Q_ASSERT(file.open(QFile::WriteOnly));
			QTextStream out(&file);
			out << svgDom->toString();
			file.close();

			emit svgFileLoadNeeded(tempFile);
			QFile::remove(tempFile);
		} else {
			DebugDialog::debug("<<< <ould not load file "+m_item->flatSvgFilePath());
		}
	}
}

QRectF PartsEditorConnectorViewImageWidget::mapFromSceneToSvg(const QRectF &sceneRect, const QSizeF &defaultSize, const QRectF &viewBox) {
	qreal relationW = defaultSize.width() / viewBox.width();
	qreal relationH = defaultSize.height() / viewBox.height();
	DebugDialog::debug(QString("<<< relation w=%1 h=%2").arg(relationW).arg(relationH));
	qreal x = sceneRect.x() * relationW;
	qreal y = sceneRect.y() * relationH;
	qreal width = sceneRect.width() * relationW;
	qreal height = sceneRect.height() * relationH;

	return QRectF(x,y,width,height);
}

void PartsEditorConnectorViewImageWidget::addRectToSvg(QDomDocument* svgDom, const QString &id, const QRectF &rect) {
	QDomElement connElem = svgDom->createElement("rect");
	connElem.setAttribute("id",id);
	connElem.setAttribute("x",rect.x());
	connElem.setAttribute("y",rect.y());
	connElem.setAttribute("width",rect.width());
	connElem.setAttribute("height",rect.height());
	connElem.setAttribute("fill","none");
	Q_ASSERT(!svgDom->firstChildElement("svg").isNull());
	svgDom->firstChildElement("svg").appendChild(connElem);
}


bool PartsEditorConnectorViewImageWidget::isSupposedToBeRemoved(const QString& id) {
	foreach(QString toBeRemoved, m_removedConnIds) {
		if(id.startsWith(toBeRemoved)) {
			return true;
		}
	}
	return false;
}

void PartsEditorConnectorViewImageWidget::showTerminalPoints(bool show) {
	foreach(QGraphicsItem *item, items()) {
		PartsEditorConnectorItem *connItem = dynamic_cast<PartsEditorConnectorItem*>(item);
		if(connItem) {
			connItem->setShowTerminalPoint(show);
		}
	}
}
