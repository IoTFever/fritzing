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


#include "itembase.h"
#include "../debugdialog.h"
#include "../modelpart.h"
#include "../connectoritem.h"
#include "../connectorshared.h"
#include "../infographicsview.h"
#include "../connector.h"
#include "../bus.h"
#include "../labels/partlabel.h"
#include "../layerattributes.h"
#include "../fsvgrenderer.h"
#include "../svg/svgfilesplitter.h"
#include "../utils/folderutils.h"

#include <QScrollBar>
#include <QTimer>
#include <QVector>
#include <QSet>
#include <QSettings>
#include <math.h>

/////////////////////////////////

class NameTriple {

public:
	NameTriple(const QString & _xmlName, const QString & _viewName, const QString & _naturalName) {
		m_xmlName = _xmlName;
		m_viewName = _viewName;
		m_naturalName = _naturalName;
	}

	QString & xmlName() {
		return m_xmlName;
	}

	QString & viewName() {
		return m_viewName;
	}

	QString & naturalName() {
		return m_naturalName;
	}

protected:
	QString m_xmlName;
	QString m_naturalName;
	QString m_viewName;
};


/////////////////////////////////

const QString ItemBase::rulerModuleIDName = "RulerModuleID";
const QString ItemBase::breadboardModuleIDName = "BreadboardModuleID";
const QString ItemBase::tinyBreadboardModuleIDName = "TinyBreadboardModuleID";
const QString ItemBase::groupModuleIDName = "GroupModuleID";
const QString ItemBase::rectangleModuleIDName = "RectanglePCBModuleID";
const QString ItemBase::noteModuleIDName = "NoteModuleID";
const QString ItemBase::wireModuleIDName = "WireModuleID";
const QString ItemBase::jumperModuleIDName = "JumperModuleID";
const QString ItemBase::groundPlaneModuleIDName = "GroundPlaneModuleID";
const QString ItemBase::groundModuleIDName = "GroundModuleID";
const QString ItemBase::powerModuleIDName = "PowerModuleID";

const QString ItemBase::ITEMBASE_FONT_PREFIX = "<font size='2'>";
const QString ItemBase::ITEMBASE_FONT_SUFFIX = "</font>";

QString ItemBase::partInstanceDefaultTitle;
QString ItemBase::moduleInstanceDefaultTitle;
QList<ItemBase *> ItemBase::emptyList;
QString ItemBase::SvgFilesDir = "svg";

const QColor ItemBase::hoverColor(0,0,0);
const qreal ItemBase::hoverOpacity = .20;
const QColor ItemBase::connectorHoverColor(0,0,255);
const qreal ItemBase::connectorHoverOpacity = .40;

const QColor StandardConnectedColor(0, 255, 0);
const QColor StandardUnconnectedColor(255, 0, 0);

QPen ItemBase::normalPen(QColor(255,0,0));
QPen ItemBase::hoverPen(QColor(0, 0, 255));
QPen ItemBase::connectedPen(StandardConnectedColor);
QPen ItemBase::unconnectedPen(StandardUnconnectedColor);
QPen ItemBase::chosenPen(QColor(255,0,0));
QPen ItemBase::equalPotentialPen(QColor(255,255,0));

QBrush ItemBase::normalBrush(QColor(255,0,0));
QBrush ItemBase::hoverBrush(QColor(0,0,255));
QBrush ItemBase::connectedBrush(StandardConnectedColor);
QBrush ItemBase::unconnectedBrush(StandardUnconnectedColor);
QBrush ItemBase::chosenBrush(QColor(255,0,0));
QBrush ItemBase::equalPotentialBrush(QColor(255,255,0));

const qreal ItemBase::normalConnectorOpacity = 0.4;

bool wireLessThan(ConnectorItem * c1, ConnectorItem * c2)
{
	if (c1->connectorType() == c2->connectorType()) {
		// if they're the same type return the topmost
		return c1->zValue() > c2->zValue();
	}
	if (c1->connectorType() == Connector::Female) {
		// choose the female first
		return true;
	}
	if (c2->connectorType() == Connector::Female) {
		// choose the female first
		return false;
	}
	if (c1->connectorType() == Connector::Male) {
		// choose the male first
		return true;
	}
	if (c2->connectorType() == Connector::Male) {
		// choose the male first
		return false;
	}

	return c1->zValue() > c2->zValue();

}

///////////////////////////////////////////////////

ItemBase::ItemBase( ModelPart* modelPart, ViewIdentifierClass::ViewIdentifier viewIdentifier, const ViewGeometry & viewGeometry, long id, QMenu * itemMenu )
	: GraphicsSvgLineItem()
{
	//DebugDialog::debug(QString("itembase %1 %2").arg(id).arg((long) static_cast<QGraphicsItem *>(this), 0, 16));
	m_hoverEnterSpaceBarWasPressed = m_spaceBarWasPressed = false;

	m_everVisible = true;

	m_rightClickedConnector = NULL;

	m_partLabel = NULL;
	m_itemMenu = itemMenu;
	m_hoverCount = m_connectorHoverCount = m_connectorHoverCount2 = 0;
	m_viewIdentifier = viewIdentifier;
	m_modelPart = modelPart;
	if (m_modelPart) {
		m_modelPart->addViewItem(this);
	}
	m_id = id;
	m_hidden = false;
	m_sticky = false;
	m_canFlipHorizontal = m_canFlipVertical = false;

	setCursor(Qt::ArrowCursor);

   	m_viewGeometry.set(viewGeometry);
	setAcceptHoverEvents ( true );
	m_zUninitialized = true;
}

ItemBase::~ItemBase() {
	//DebugDialog::debug(QString("deleting itembase %1 %2 %3").arg((long) this, 0, 16).arg(m_id).arg((long) m_modelPart, 0, 16));
	if (m_partLabel) {
		delete m_partLabel;
		m_partLabel = NULL;
	}

	foreach (QGraphicsItem * childItem, childItems()) {
		ConnectorItem * connectorItem = dynamic_cast<ConnectorItem *>(childItem);
		if (connectorItem == NULL) continue;

		foreach (ConnectorItem * toConnectorItem, connectorItem->connectedToItems()) {
			toConnectorItem->tempRemove(connectorItem, true);
		}
	}

	foreach (ItemBase * itemBase, m_stickyList) {
		itemBase->addSticky(this, false);
	}

	if (m_modelPart != NULL) {
		m_modelPart->removeViewItem(this);
	}

	clearBusConnectorItems();
}

void ItemBase::clearBusConnectorItems()
{
	foreach (QList<ConnectorItem *> * list, m_busConnectorItems) {
		delete list;
	}

	m_busConnectorItems.clear();
}

void ItemBase::setTooltip() {
	if(m_modelPart) {
		QString title = m_modelPart->instanceTitle();
		if(!title.isNull() && !title.isEmpty()) {
			setInstanceTitleTooltip(m_modelPart->instanceTitle());
		} else {
			setDefaultTooltip();
		}
	} else {
		setDefaultTooltip();
	}
}

void ItemBase::setConnectorTooltips() {
	foreach (QGraphicsItem * childItem, childItems()) {
		ConnectorItem * connectorItem = dynamic_cast<ConnectorItem *>(childItem);
		if (connectorItem == NULL) continue;

		QString tt = QString("<b>%1</b> %2<br />" + ITEMBASE_FONT_PREFIX + "%3" + ITEMBASE_FONT_SUFFIX)
			.arg(connectorItem->connector()->connectorShared()->description())
			.arg(connectorItem->connectorSharedName())
			.arg(toolTip());

		connectorItem->setBaseTooltip(tt);
	}
}

void ItemBase::removeTooltip() {
	this->setToolTip(___emptyString___);
}

bool ItemBase::zLessThan(ItemBase * & p1, ItemBase * & p2)
{
	return p1->z() < p2->z();
}

qint64 ItemBase::getNextID() {
	return ModelPart::nextIndex() * ModelPart::indexMultiplier;								// make sure we leave room for layerkin inbetween
}

qint64 ItemBase::getNextID(qint64 index) {

	qint64 temp = index * ModelPart::indexMultiplier;						// make sure we leave room for layerkin inbetween
	ModelPart::updateIndex(index);
	return temp;
}

QSizeF ItemBase::size() {
	return m_size;
}

qint64 ItemBase::id() {
 	return m_id;
}

void ItemBase::resetID() {
	m_id = m_modelPart->modelIndex() * ModelPart::indexMultiplier;
}

qreal ItemBase::z() {
	return getViewGeometry().z();
}

ModelPart * ItemBase::modelPart() {
	return m_modelPart;
}

void ItemBase::setModelPart(ModelPart * modelPart) {
	m_modelPart = modelPart;
}

ModelPartShared * ItemBase::modelPartShared() {
	if (m_modelPart == NULL) return NULL;

	return m_modelPart->modelPartShared();
}

void ItemBase::initNames() {
	partInstanceDefaultTitle = tr("Part");
	moduleInstanceDefaultTitle = tr("Module");

	QSettings settings;
	QString colorName = settings.value("ConnectedColor").toString();
	if (!colorName.isEmpty()) {
		QColor color;
		color.setNamedColor(colorName);
		setConnectedColor(color);
	}

	colorName = settings.value("UnconnectedColor").toString();
	if (!colorName.isEmpty()) {
		QColor color;
		color.setNamedColor(colorName);
		setUnconnectedColor(color);
	}
}

void ItemBase::saveInstance(QXmlStreamWriter & streamWriter) {
	streamWriter.writeStartElement(ViewIdentifierClass::viewIdentifierXmlName(m_viewIdentifier));
	streamWriter.writeAttribute("layer", ViewLayer::viewLayerXmlNameFromID(m_viewLayerID));
	this->saveGeometry();
	writeGeometry(streamWriter);
	if (m_partLabel) {
		m_partLabel->saveInstance(streamWriter);
	}

	bool saveConnectorItems = false;
	foreach (QGraphicsItem * childItem, childItems()) {
		ConnectorItem * connectorItem = dynamic_cast<ConnectorItem *>(childItem);
		if (connectorItem == NULL) continue;

		if (connectorItem->connectionsCount() > 0) {
			saveConnectorItems = true;
			break;
		}
	}

	if (saveConnectorItems) {
		streamWriter.writeStartElement("connectors");
		foreach (QGraphicsItem * childItem, childItems()) {
			ConnectorItem * connectorItem = dynamic_cast<ConnectorItem *>(childItem);
			if (connectorItem == NULL) continue;

			connectorItem->saveInstance(streamWriter);
		}
		streamWriter.writeEndElement();
	}


	streamWriter.writeEndElement();
}

void ItemBase::writeGeometry(QXmlStreamWriter & streamWriter) {
	streamWriter.writeStartElement("geometry");
	streamWriter.writeAttribute("z", QString::number(z()));
	this->saveInstanceLocation(streamWriter);
	// do not write attributes here
	streamWriter.writeEndElement();
}

ViewGeometry & ItemBase::getViewGeometry() {
	return m_viewGeometry;
}

ViewIdentifierClass::ViewIdentifier ItemBase::viewIdentifier() {
	return m_viewIdentifier;
}

QString & ItemBase::viewIdentifierName() {
	return ViewIdentifierClass::viewIdentifierName(m_viewIdentifier);
}

ViewLayer::ViewLayerID ItemBase::viewLayerID() {
	return m_viewLayerID;
}

void ItemBase::setViewLayerID(const QString & layerName, const LayerHash & viewLayers) {
	//DebugDialog::debug(QString("using z %1").arg(layerName));
	setViewLayerID(ViewLayer::viewLayerIDFromXmlString(layerName), viewLayers);
}

void ItemBase::setViewLayerID(ViewLayer::ViewLayerID viewLayerID, const LayerHash & viewLayers) {
	m_viewLayerID = viewLayerID;
	if (m_zUninitialized) {
   		ViewLayer * viewLayer = viewLayers.value(m_viewLayerID);
   		if (viewLayer != NULL) {
			m_zUninitialized = false;
			if (!viewLayer->alreadyInLayer(m_viewGeometry.z())) {
   				m_viewGeometry.setZ(viewLayer->nextZ());
			}
  		}
  	}

    //DebugDialog::debug(QString("using z: %1 z:%2 lid:%3").arg(modelPart()->modelPartShared()->title()).arg(m_viewGeometry.z()).arg(m_viewLayerID) );
}

void ItemBase::removeLayerKin() {
}

void ItemBase::hoverEnterConnectorItem(QGraphicsSceneHoverEvent * , ConnectorItem * ) {
	//DebugDialog::debug(QString("hover enter c %1").arg(instanceTitle()));
	hoverEnterConnectorItem();
}

void ItemBase::hoverEnterConnectorItem() {
	//DebugDialog::debug(QString("hover enter c %1").arg(instanceTitle()));
	m_connectorHoverCount++;
	if (itemType() != ModelPart::Breadboard) {
		this->update();
	}
}

void ItemBase::hoverLeaveConnectorItem(QGraphicsSceneHoverEvent * , ConnectorItem * ) {
	hoverLeaveConnectorItem();
}

void ItemBase::hoverMoveConnectorItem(QGraphicsSceneHoverEvent * , ConnectorItem * ) {
}

void ItemBase::hoverLeaveConnectorItem() {
	//DebugDialog::debug(QString("hover leave c %1").arg(instanceTitle()));
	m_connectorHoverCount--;
	if (itemType() != ModelPart::Breadboard) {
		this->update();
	}
}

void ItemBase::clearConnectorHover()
{
	m_connectorHoverCount2 = 0;
	if (itemType() != ModelPart::Breadboard) {
		update();
	}
}

void ItemBase::connectorHover(ConnectorItem *, ItemBase *, bool hovering) {
	//DebugDialog::debug(QString("hover c %1 %2").arg(hovering).arg(instanceTitle()));

	if (hovering) {
		m_connectorHoverCount2++;
	}
	else {
		m_connectorHoverCount2--;
	}
	// DebugDialog::debug(QString("m_connectorHoverCount2 %1 %2").arg(instanceTitle()).arg(m_connectorHoverCount2));
	if (itemType() != ModelPart::Breadboard) {
		this->update();
	}
}

void ItemBase::mousePressConnectorEvent(ConnectorItem *, QGraphicsSceneMouseEvent *) {
}

void ItemBase::mouseDoubleClickConnectorEvent(ConnectorItem *, QGraphicsSceneMouseEvent *) {
}

void ItemBase::mouseMoveConnectorEvent(ConnectorItem *, QGraphicsSceneMouseEvent *) {
}

void ItemBase::mouseReleaseConnectorEvent(ConnectorItem *, QGraphicsSceneMouseEvent *) {
}

bool ItemBase::filterMousePressConnectorEvent(ConnectorItem *, QGraphicsSceneMouseEvent *) {
	return false;
}

bool ItemBase::acceptsMousePressConnectorEvent(ConnectorItem *, QGraphicsSceneMouseEvent *) {
	return true;
}

bool ItemBase::acceptsMouseReleaseConnectorEvent(ConnectorItem *, QGraphicsSceneMouseEvent *) {
	return false;
}

bool ItemBase::acceptsMouseDoubleClickConnectorEvent(ConnectorItem *, QGraphicsSceneMouseEvent *) {
	return false;
}

bool ItemBase::acceptsMouseMoveConnectorEvent(ConnectorItem *, QGraphicsSceneMouseEvent *) {
	return false;
}

void ItemBase::connectionChange(ConnectorItem * onMe, ConnectorItem * onIt, bool connect) {
	Q_UNUSED(onMe);
	Q_UNUSED(onIt);
	Q_UNUSED(connect);
}

void ItemBase::connectedMoved(ConnectorItem * /* from */, ConnectorItem * /* to */) {
}

ItemBase * ItemBase::extractTopLevelItemBase(QGraphicsItem * item) {
	ItemBase * itemBase = dynamic_cast<ItemBase *>(item);
	if (itemBase == NULL) return NULL;

	if (itemBase->topLevel()) return itemBase;

	return NULL;
}

bool ItemBase::topLevel() {
	return (this == this->layerKinChief());
}

void ItemBase::setHidden(bool hide) {

	m_hidden = hide;
	setAcceptedMouseButtons(hide ? Qt::NoButton : ALLMOUSEBUTTONS);
	setAcceptHoverEvents(!hide);
	update();
	for (int i = 0; i < childItems().count(); i++) {
		ConnectorItem * connectorItem = dynamic_cast<ConnectorItem *>(childItems()[i]);
		if (connectorItem == NULL) continue;

		connectorItem->setHidden(hide);
	}
}

bool ItemBase::hidden() {
	return m_hidden;
}

void ItemBase::collectConnectors(ConnectorPairHash & connectorHash, QGraphicsScene * scene) {
	Q_UNUSED(scene);

	ModelPart * modelPart = this->modelPart();
	if (modelPart == NULL) return;

	// collect all the connectorItem pairs

	foreach (QGraphicsItem * childItem, childItems()) {
		ConnectorItem * fromConnectorItem = dynamic_cast<ConnectorItem *>( childItem );
		if (fromConnectorItem == NULL) continue;

		foreach (ConnectorItem * toConnectorItem, fromConnectorItem->connectedToItems()) {
			connectorHash.insert(fromConnectorItem, toConnectorItem);
		}
	}
}

void ItemBase::collectConnectors(QList<ConnectorItem *> & connectors) {
	foreach (QGraphicsItem * childItem, childItems()) {
		ConnectorItem * connectorItem = dynamic_cast<ConnectorItem *>( childItem );
		if (connectorItem != NULL) connectors.append(connectorItem);
	}
}

ConnectorItem * ItemBase::findConnectorItemNamed(const QString & connectorID)  {
	for (int i = 0; i < childItems().count(); i++) {
		ConnectorItem * connectorItem = dynamic_cast<ConnectorItem *>(childItems()[i]);
		if (connectorItem == NULL) continue;

		if (connectorID.compare(connectorItem->connectorSharedID()) == 0) {
			return connectorItem;
		}
	}

	return NULL;
}

void ItemBase::hoverEnterEvent ( QGraphicsSceneHoverEvent * event ) {
	//DebugDialog::debug(QString("hover enter %1").arg(instanceTitle()));
	InfoGraphicsView * infoGraphicsView = InfoGraphicsView::getInfoGraphicsView(this);
	if (infoGraphicsView != NULL && infoGraphicsView->spaceBarIsPressed()) {
		m_hoverEnterSpaceBarWasPressed = true;
		event->ignore();
		return;
	}

	m_hoverEnterSpaceBarWasPressed = false;
	m_hoverCount++;
	if (itemType() != ModelPart::Breadboard) {
		update();
	}
	if (infoGraphicsView != NULL) {
		infoGraphicsView->hoverEnterItem(event, this);
	}
}

void ItemBase::hoverLeaveEvent ( QGraphicsSceneHoverEvent * event ) {
	//DebugDialog::debug(QString("hover leave %1").arg(instanceTitle()));
	if (m_hoverEnterSpaceBarWasPressed) {
		event->ignore();
		return;
	}

	m_hoverCount--;
	if (itemType() != ModelPart::Breadboard) {
		update();
	}
	InfoGraphicsView * infoGraphicsView = InfoGraphicsView::getInfoGraphicsView(this);
	if (infoGraphicsView != NULL) {
		infoGraphicsView->hoverLeaveItem(event, this);
	}
}

void ItemBase::hoverMoveEvent ( QGraphicsSceneHoverEvent *  ) {
	//DebugDialog::debug(QString("hover move %1 %2").arg(instanceTitle()).arg(QTime::currentTime().msec()));
}

ConnectorItem * ItemBase::findConnectorUnder(ConnectorItem * connectorItemOver, ConnectorItem * lastUnderConnector, bool useTerminalPoint, bool allowAlready, const QList<ConnectorItem *> & exclude)
{
	QList<QGraphicsItem *> items = useTerminalPoint
		? this->scene()->items(connectorItemOver->sceneAdjustedTerminalPoint(NULL))
		: this->scene()->items(mapToScene(connectorItemOver->rect()));
	QList<ConnectorItem *> candidates;
	// for the moment, take the topmost ConnectorItem that doesn't belong to me
	foreach (QGraphicsItem * item, items) {
		ConnectorItem * connectorItemUnder = dynamic_cast<ConnectorItem *>(item);
		if (connectorItemUnder == NULL) continue;
		if (connectorItemUnder->connector() == NULL) continue;			// shouldn't happen
		if (childItems().contains(connectorItemUnder)) continue;		// don't use own connectors
		if (!connectorItemOver->connectionIsAllowed(connectorItemUnder)) {
			continue;
		}
		if (!allowAlready) {
			if (connectorItemUnder->connectedToItems().contains(connectorItemOver)) {
				continue;		// already connected
			}
		}
		if (exclude.contains(connectorItemUnder)) continue;

		candidates.append(connectorItemUnder);
	}

	ConnectorItem * candidate = NULL;
	if (candidates.count() == 1) {
		candidate = candidates[0];
	}
	else if (candidates.count() > 0) {
		qSort(candidates.begin(), candidates.end(), wireLessThan);
		candidate = candidates[0];
	}

	if (lastUnderConnector != NULL && candidate != lastUnderConnector) {
		lastUnderConnector->connectorHover(this, false);
	}
	if (candidate != NULL && candidate != lastUnderConnector) {
		candidate->connectorHover(this, true);
	}

	lastUnderConnector = candidate;

	if (candidate == NULL) {
		if (connectorItemOver->connectorHovering()) {
			connectorItemOver->connectorHover(NULL, false);
		}
	}
	else {
		if (!connectorItemOver->connectorHovering()) {
			connectorItemOver->connectorHover(NULL, true);
		}
	}

	return lastUnderConnector;
}

void ItemBase::updateConnections() {
}

void ItemBase::updateConnections(ConnectorItem * item) {
	item->attachedMoved();
}

const QString & ItemBase::title() {
	if (m_modelPart == NULL) return ___emptyString___;

	return m_modelPart->title();
}

bool ItemBase::getVirtual() {
	return m_viewGeometry.getVirtual();
}

const QHash<QString, Bus *> & ItemBase::buses() {
	if (m_modelPart != NULL) return m_modelPart->buses();

	return Bus::___emptyBusList___;
}

void ItemBase::addBusConnectorItem(Bus * bus, ConnectorItem * item) {
	QList <ConnectorItem *> * busConnectorItems = m_busConnectorItems.value(bus);
	if (busConnectorItems == NULL) {
		busConnectorItems = new QList<ConnectorItem *>;
		m_busConnectorItems.insert(bus, busConnectorItems);
	}
	busConnectorItems->append(item);
}

int ItemBase::itemType() const
{
	if (m_modelPart == NULL) return ModelPart::Unknown;

	return m_modelPart->itemType();
}

void ItemBase::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
	if (m_connectorHoverCount > 0 || m_hoverCount > 0 || m_connectorHoverCount2 > 0) {
		paintHover(painter, option, widget);
	}

	GraphicsSvgLineItem::paint(painter, option, widget);
}

void ItemBase::paintHover(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(widget);
	Q_UNUSED(option);
	painter->save();
	if (m_connectorHoverCount > 0 || m_connectorHoverCount2 > 0) {
		painter->setOpacity(connectorHoverOpacity);
		painter->fillPath(this->hoverShape(), QBrush(connectorHoverColor));
	}
	else {
		painter->setOpacity(hoverOpacity);
		painter->fillPath(this->hoverShape(), QBrush(hoverColor));
	}
	painter->restore();
}

void ItemBase::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	InfoGraphicsView *infoGraphicsView = InfoGraphicsView::getInfoGraphicsView(this);
	if (infoGraphicsView != NULL && infoGraphicsView->spaceBarIsPressed()) {
		event->ignore();
		return;
	}

	//scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
	//setCacheMode(QGraphicsItem::DeviceCoordinateCache);
	GraphicsSvgLineItem::mousePressEvent(event);
}

void ItemBase::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	m_rightClickedConnector = NULL;
	// calling parent class so that multiple selection will work
	// haven't yet discovered any nasty side-effect
	GraphicsSvgLineItem::mouseReleaseEvent(event);

	//scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
	// setCacheMode(QGraphicsItem::NoCache);

}

void ItemBase::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	Q_UNUSED(event);
	return;
}

void ItemBase::setItemPos(QPointF & loc) {
	setPos(loc);
}

bool ItemBase::stickyEnabled(ItemBase * stickTo) {
	Q_UNUSED(stickTo);

	return true;
}

bool ItemBase::sticky() {
	return m_sticky;
}

void ItemBase::setSticky(bool s)
{
	m_sticky = s;
}

void ItemBase::addSticky(ItemBase * sticky, bool stickem) {
	if (stickem) {
		if (!m_sticky) {
			foreach (ItemBase * oldStuckTo, m_stickyList) {
				if (oldStuckTo == sticky->layerKinChief()) continue;

				oldStuckTo->addSticky(this, false);
			}
			m_stickyList.clear();
		}
		m_stickyList.append(sticky->layerKinChief());
	}
	else {
		int result = m_stickyList.removeOne(sticky);
		if (result <= 0) {
			m_stickyList.removeOne(sticky->layerKinChief());
		}
	}
}


ItemBase * ItemBase::stuckTo() {
	if (m_sticky) return NULL;

	if (m_stickyList.count() < 1) return NULL;

	if (m_stickyList.count() > 1) {
		DebugDialog::debug(QString("error: sticky list > 1 %1").arg(title()));
	}

	return m_stickyList[0];
}

QList<ItemBase *> & ItemBase::stickyList() {
	return m_stickyList;
}

bool ItemBase::alreadySticking(ItemBase * itemBase) {
	return m_stickyList.contains(itemBase->layerKinChief());
}

ConnectorItem* ItemBase::newConnectorItem(Connector *connector) 
{
	return new ConnectorItem(connector, this);
}

ConnectorItem * ItemBase::anyConnectorItem() {
	foreach (QGraphicsItem * childItem, childItems()) {
		ConnectorItem * connectorItem = dynamic_cast<ConnectorItem *>(childItem);
		if (connectorItem != NULL) return connectorItem;
	}

	return NULL;
}


QString ItemBase::instanceTitle() {
	if(m_modelPart) {
		return m_modelPart->instanceTitle();
	}
	return ___emptyString___;
}

void ItemBase::setInstanceTitle(const QString &title) {
	setInstanceTitleAux(title);
	if (m_partLabel) {
		m_partLabel->setPlainText(title);
	}
}

void ItemBase::setInstanceTitleAux(const QString &title)
{
	if (m_modelPart) {
		m_modelPart->setInstanceTitle(title);
	}
	setInstanceTitleTooltip(title);

//	InfoGraphicsView *infographics = InfoGraphicsView::getInfoGraphicsView(this);
//	if (infographics != NULL) {
//		infographics->setItemTooltip(this, title);
//	}
}

QString ItemBase::label() {
	if(m_modelPart && m_modelPart->modelPartShared()) {
		return m_modelPart->modelPartShared()->label();
	}
	return ___emptyString___;
}

void ItemBase::updateTooltip() {
	setInstanceTitleTooltip(instanceTitle());
}

void ItemBase::setInstanceTitleTooltip(const QString &text) {
	setToolTip("<b>"+text+"</b><br></br>" + ITEMBASE_FONT_PREFIX + modelPartShared()->title()+ ITEMBASE_FONT_SUFFIX);
}

void ItemBase::setDefaultTooltip() {
	if (m_modelPart) {
		QString title = (m_modelPart->itemType() == ModelPart::Module) ? ItemBase::moduleInstanceDefaultTitle : ItemBase::partInstanceDefaultTitle;
		QString inst = instanceTitle();
		if(!inst.isNull() && !inst.isEmpty()) {
			title = inst;
		} else {
			QString defaultTitle = label();
			if(!defaultTitle.isNull() && !defaultTitle.isEmpty()) {
				title = defaultTitle;
			}
		}
		ensureUniqueTitle(title);
		setInstanceTitleTooltip(m_modelPart->instanceTitle());
	}
}

bool ItemBase::isConnectedTo(ItemBase * other) {
	foreach (QGraphicsItem * childItem, childItems()) {
		ConnectorItem * fromConnectorItem = dynamic_cast<ConnectorItem *>(childItem);
		if (fromConnectorItem == NULL) continue;

		foreach (ConnectorItem * toConnectorItem, fromConnectorItem->connectedToItems()) {
			if (toConnectorItem->attachedTo() == other) return true;
		}
	}

	return false;
}


void ItemBase::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	if ((acceptedMouseButtons() & Qt::RightButton) == 0) {
		event->ignore();
		return;
	}

	if (m_hidden) {
		event->ignore();
		return;
	}

    scene()->clearSelection();
    setSelected(true);

    if (m_itemMenu != NULL) {
		m_rightClickedConnector = NULL;
		foreach (QGraphicsItem * item, scene()->items(event->scenePos())) {
			ConnectorItem * connectorItem = dynamic_cast<ConnectorItem *>(item);
			if (connectorItem == NULL) continue;

			if (connectorItem->attachedTo() == this) {
				m_rightClickedConnector = connectorItem;
				break;
			}
		}

     	m_itemMenu->exec(event->screenPos());
	}
}



bool ItemBase::hasConnectors() {
	foreach (QGraphicsItem * childItem, childItems()) {
		if (dynamic_cast<ConnectorItem *>(childItem) != NULL) return true;
	}

	return false;
}


bool ItemBase::canFlipHorizontal() {
	return m_canFlipHorizontal;
}

void ItemBase::setCanFlipHorizontal(bool cf) {
	m_canFlipHorizontal = cf;
}

bool ItemBase::canFlipVertical() {
	return m_canFlipVertical;
}

void ItemBase::setCanFlipVertical(bool cf) {
	m_canFlipVertical = cf;
}

void ItemBase::busConnectorItems(class Bus * bus, QList<class ConnectorItem *> & items) {
	QList<ConnectorItem *> * busConnectorItems = m_busConnectorItems.value(bus);
	if (busConnectorItems != NULL) {
		foreach (ConnectorItem * connectorItem, *busConnectorItems) {
			items.append(connectorItem);
		}
	}
}

void ItemBase::clearModelPart() {
	m_modelPart = NULL;
}

void ItemBase::showPartLabel(bool showIt, ViewLayer* viewLayer) {
	if (m_partLabel) {
		m_partLabel->showLabel(showIt, viewLayer);
	}
}

void ItemBase::partLabelChanged(const QString & newText) {
	// sent from part label after inline edit
	InfoGraphicsView *infographics = InfoGraphicsView::getInfoGraphicsView(this);
	QString oldText = modelPart()->instanceTitle();
	setInstanceTitleAux(newText);
	if (infographics != NULL) {
		infographics->partLabelChanged(this, oldText, newText, QSizeF(), QSizeF(), false);
	}
}

bool ItemBase::isPartLabelVisible() {
	if (m_partLabel == NULL) return false;
	if (!m_partLabel->initialized()) return false;
	return m_partLabel->isVisible();
}


void ItemBase::clearPartLabel() {
	m_partLabel = NULL;
}

void ItemBase::restorePartLabel(QDomElement & labelGeometry, ViewLayer::ViewLayerID viewLayerID)
{
	if (m_partLabel) {
		if (!labelGeometry.isNull()) {
			m_partLabel->restoreLabel(labelGeometry, viewLayerID);
		}
		m_partLabel->setPlainText(m_modelPart->instanceTitle());
	}
}

void ItemBase::movePartLabel(QPointF newPos, QPointF newOffset) {
	if (m_partLabel) {
		m_partLabel->moveLabel(newPos, newOffset);
	}
}


void ItemBase::partLabelMoved(QPointF oldPos, QPointF oldOffset, QPointF newPos, QPointF newOffset) {
	InfoGraphicsView * infoGraphicsView = InfoGraphicsView::getInfoGraphicsView(this);
	if (infoGraphicsView != NULL) {
		infoGraphicsView->partLabelMoved(this, oldPos, oldOffset, newPos, newOffset);
	}
}

void ItemBase::rotateFlipPartLabel(qreal degrees, Qt::Orientations orientation)
{
	InfoGraphicsView * infoGraphicsView = InfoGraphicsView::getInfoGraphicsView(this);
	if (infoGraphicsView != NULL) {
		infoGraphicsView->rotateFlipPartLabel(this, degrees, orientation);
	}
}


void ItemBase::doRotateFlipPartLabel(qreal degrees, Qt::Orientations orientation)
{
	if (m_partLabel) {
		m_partLabel->rotateFlipLabel(degrees, orientation);
	}
}

bool ItemBase::isSwappable() {
	return true;
}

void ItemBase::ensureUniqueTitle(QString &title) {
	if(instanceTitle().isEmpty() || instanceTitle().isNull()) {
		int count;

		QList<QGraphicsItem*> items = scene()->items();
		// If someone ends up with 1000 parts in the sketch, this for sure is not the best solution
		count = getNextTitle(items, title);

		title = QString(title+"%1").arg(count);
		setInstanceTitle(title);
	}
}

int ItemBase::getNextTitle(QList<QGraphicsItem*> & items, const QString &title) {
	int max = 1;
	foreach(QGraphicsItem* gitem, items) {
		ItemBase* item = dynamic_cast<ItemBase*>(gitem);
		if(item) {
			QString currTitle = item->instanceTitle();
			if(currTitle.isEmpty() || currTitle.isNull()) {
				currTitle = item->label();
				if(currTitle.isEmpty() || currTitle.isNull()) {
					currTitle = title;
				}
			}

			if(currTitle.startsWith(title)) {
				QString helpStr = currTitle.remove(title);
				if(!helpStr.isEmpty()) {
					bool isInt;
					int helpInt = helpStr.toInt(&isInt);
					if(isInt && max <= helpInt) {
						max = ++helpInt;
					}
				}
			}
		}
	}
	return max;
}

QVariant ItemBase::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant & value)
{
	switch (change) {
		case QGraphicsItem::ItemSceneHasChanged:
			if (this->scene() && instanceTitle().isEmpty()) {
				setTooltip();
			}
			break;
		case QGraphicsItem::ItemSelectedChange:
			if (m_partLabel) {
				m_partLabel->ownerSelected(value.toBool());
			}
			break;
		default:
			break;
	}

	return GraphicsSvgLineItem::itemChange(change, value);
}

QString ItemBase::toolTip2() {
	// because QGraphicsItem::toolTip() isn't virtual
	QString tt = toolTip();
	if (!tt.isEmpty()) return tt;

	// because the tooltip may not have been initialized for this view
	setTooltip();
	return toolTip();
}

void ItemBase::cleanup() {
}

const QList<ItemBase *> & ItemBase::layerKin() {
	return emptyList;
}

ItemBase * ItemBase::layerKinChief() {
	return this;
}

void ItemBase::rotateItem(qreal degrees) {
	transformItem(QTransform().rotate(degrees));
}

void ItemBase::flipItem(Qt::Orientations orientation) {
	int xScale; int yScale;
	if(orientation == Qt::Vertical) {
		xScale = 1;
		yScale = -1;
	} else if(orientation == Qt::Horizontal) {
		xScale = -1;
		yScale = 1;
	}
	else {
		return;
	}

	transformItem(QTransform().scale(xScale,yScale));
}

void ItemBase::transformItem(const QTransform & currTransf) {
	QRectF rect = this->boundingRect();
	qreal x = rect.width() / 2.0;
	qreal y = rect.height() / 2.0;
	QTransform transf = QTransform().translate(-x, -y) * currTransf * QTransform().translate(x, y);
	getViewGeometry().setTransform(getViewGeometry().transform()*transf);
	this->setTransform(getViewGeometry().transform());
	updateConnections();
	update();
}

void ItemBase::transformItem2(const QMatrix & matrix) {
	QTransform transform(matrix);
	transformItem(transform);
}

void ItemBase::collectWireConnectees(QSet<class Wire *> & wires) {
	Q_UNUSED(wires);
}

void ItemBase::collectFemaleConnectees(QSet<ItemBase *> & items) {
	Q_UNUSED(items);
}

void ItemBase::prepareGeometryChange() {
	GraphicsSvgLineItem::prepareGeometryChange();
}

void ItemBase::saveLocAndTransform(QXmlStreamWriter & streamWriter)
{
	streamWriter.writeAttribute("x", QString::number(m_viewGeometry.loc().x()));
	streamWriter.writeAttribute("y", QString::number(m_viewGeometry.loc().y()));
	if (!m_viewGeometry.transform().isIdentity()) {
		streamWriter.writeStartElement("transform");
		streamWriter.writeAttribute("m11", QString::number(m_viewGeometry.transform().m11()));
		streamWriter.writeAttribute("m12", QString::number(m_viewGeometry.transform().m12()));
		streamWriter.writeAttribute("m13", QString::number(m_viewGeometry.transform().m13()));
		streamWriter.writeAttribute("m21", QString::number(m_viewGeometry.transform().m21()));
		streamWriter.writeAttribute("m22", QString::number(m_viewGeometry.transform().m22()));
		streamWriter.writeAttribute("m23", QString::number(m_viewGeometry.transform().m23()));
		streamWriter.writeAttribute("m31", QString::number(m_viewGeometry.transform().m31()));
		streamWriter.writeAttribute("m32", QString::number(m_viewGeometry.transform().m32()));
		streamWriter.writeAttribute("m33", QString::number(m_viewGeometry.transform().m33()));
		streamWriter.writeEndElement();
	}

}

FSvgRenderer * ItemBase::setUpImage(ModelPart * modelPart, ViewIdentifierClass::ViewIdentifier viewIdentifier, ViewLayer::ViewLayerID viewLayerID, LayerAttributes & layerAttributes)
{
#ifndef QT_NO_DEBUG
	QTime t;
	t.start();
#endif

    ModelPartShared * modelPartShared = modelPart->modelPartShared();

    if (modelPartShared == NULL) return NULL;
    if (modelPartShared->domDocument() == NULL) return NULL;

	bool result = layerAttributes.getSvgElementID(modelPartShared->domDocument(), viewIdentifier, viewLayerID);
	if (!result) return NULL;

	//DebugDialog::debug(QString("setting z %1 %2")
		//.arg(this->z())
		//.arg(ViewLayer::viewLayerNameFromID(viewLayerID))  );


	//DebugDialog::debug(QString("set up image elapsed (1) %1").arg(t.elapsed()) );
	FSvgRenderer * renderer = FSvgRenderer::getByModuleID(modelPartShared->moduleID(), viewLayerID);
	if (renderer == NULL) {
		QString tempPath1;
		QString tempPath2;
		QString postfix = +"/"+ ItemBase::SvgFilesDir +"/%1/"+ layerAttributes.filename();
		if(modelPartShared->path() != ___emptyString___) {
			QDir dir(modelPartShared->path());			// is a path to a filename
			dir.cdUp();									// lop off the filename
			dir.cdUp();									// parts root
			tempPath1 = dir.absolutePath() + "/" + ItemBase::SvgFilesDir +"/%1/" + layerAttributes.filename();
			tempPath2 = FolderUtils::getApplicationSubFolderPath("parts")+postfix;    // some svgs may still be in the fritzing parts folder, though the other svgs are in the user folder
		} else { // for fake models
			tempPath1 = FolderUtils::getApplicationSubFolderPath("parts")+postfix;
			tempPath2 = FolderUtils::getUserDataStorePath("parts")+postfix;
		}

		//DebugDialog::debug(QString("got tempPath %1").arg(tempPath));

    	QStringList possibleFolders;
    	possibleFolders << "core" << "contrib" << "user";
		bool gotOne = false;
		QString filename;
		foreach (QString possibleFolder, possibleFolders) {
			filename = tempPath1.arg(possibleFolder);
			if (QFileInfo(filename).exists()) {
				gotOne = true;
				break;
			} else {
				filename = tempPath2.arg(possibleFolder);
				if (QFileInfo(filename).exists()) {
					gotOne = true;
					break;
				}
			}
		}

//#ifndef QT_NO_DEBUG
		//DebugDialog::debug(QString("set up image elapsed (2) %1").arg(t.elapsed()) );
//#endif

		if (gotOne) {
			renderer = FSvgRenderer::getByFilename(filename, viewLayerID);
			if (renderer == NULL) {
				bool readConnectors = (viewLayerID == ViewLayer::Copper0) && (viewIdentifier == ViewIdentifierClass::PCBView);
				gotOne = false;
				renderer = new FSvgRenderer();
				if (layerAttributes.multiLayer()) {
					// need to treat create "virtual" svg file for each layer
					SvgFileSplitter svgFileSplitter;
					if (svgFileSplitter.split(filename, layerAttributes.layerName())) {
						if (renderer->load(svgFileSplitter.byteArray(), filename, readConnectors)) {
							gotOne = true;
						}
					}
				}
				else {
//#ifndef QT_NO_DEBUG
//					DebugDialog::debug(QString("set up image elapsed (2.3) %1").arg(t.elapsed()) );
//#endif
					// only one layer, just load it directly
					if (renderer->load(filename, readConnectors)) {
						gotOne = true;
					}
//#ifndef QT_NO_DEBUG
//					DebugDialog::debug(QString("set up image elapsed (2.4) %1").arg(t.elapsed()) );
//#endif
				}
				if (!gotOne) {
					delete renderer;
					renderer = NULL;
				}
			}
			//DebugDialog::debug(QString("set up image elapsed (3) %1").arg(t.elapsed()) );

			if (renderer) {
				FSvgRenderer::set(modelPartShared->moduleID(), viewLayerID, renderer);
			}
    	}
	}

	if (renderer) {
		layerAttributes.setFilename(renderer->filename());
	}

	return renderer;
}

void ItemBase::updateConnectionsAux() {
	//DebugDialog::debug("update connections");
	foreach (QGraphicsItem * childItem, childItems()) {
		ConnectorItem * connectorItem = dynamic_cast<ConnectorItem *>(childItem);
		if (connectorItem == NULL) continue;

		updateConnections(connectorItem);
	}
}

void ItemBase::updateExternalConnections() {
	foreach (QGraphicsItem * childItem, childItems()) {
		ConnectorItem * connectorItem = dynamic_cast<ConnectorItem *>(childItem);
		if (connectorItem != NULL && connectorItem->isExternal()) {
			updateConnections(connectorItem);
			continue;
		}

		ItemBase * itemBase = dynamic_cast<ItemBase *>(childItem);
		if (itemBase != NULL) {
			itemBase->updateExternalConnections();
		}
	}
}

void ItemBase::blockSyncKinMoved(bool block) {
	Q_UNUSED(block);
}

ItemBase * ItemBase::lowerConnectorLayerVisible(ItemBase * itemBase) {
	Q_UNUSED(itemBase);
	return NULL;
}

void ItemBase::figureHover() {
}

QString ItemBase::retrieveSvg(ViewLayer::ViewLayerID viewLayerID, QHash<QString, SvgFileSplitter *> & svgHash, bool blackOnly, qreal dpi)
{
	Q_UNUSED(viewLayerID);
	Q_UNUSED(svgHash);
	Q_UNUSED(blackOnly);
	Q_UNUSED(dpi);
	return ___emptyString___;
}

bool ItemBase::hasConnections()
{
	foreach (QGraphicsItem * item, childItems()) {
		ConnectorItem * fromConnectorItem = dynamic_cast<ConnectorItem *>(item);
		if (fromConnectorItem == NULL) continue;

		if (fromConnectorItem->connectionsCount() > 0) return true;
	}

	return false;
}

void ItemBase::getConnectedColor(ConnectorItem *, QBrush * &brush, QPen * &pen, qreal & opacity, int & negativePenWidth) {
	brush = &connectedBrush;
	pen = &connectedPen;
	opacity = 0.2;
	negativePenWidth = 0;
}

void ItemBase::getNormalColor(ConnectorItem *, QBrush * &brush, QPen * &pen, qreal & opacity, int & negativePenWidth) {
	brush = &normalBrush;
	pen = &normalPen;
	opacity = normalConnectorOpacity;
	negativePenWidth = 0;
}

void ItemBase::getUnconnectedColor(ConnectorItem *, QBrush * &brush, QPen * &pen, qreal & opacity, int & negativePenWidth) {
	brush = &unconnectedBrush;
	pen = &unconnectedPen;
	opacity = 0.6;
	negativePenWidth = 0;
}

void ItemBase::getChosenColor(ConnectorItem *, QBrush * &brush, QPen * &pen, qreal & opacity, int & negativePenWidth) {
	brush = &chosenBrush;
	pen = &chosenPen;
	opacity = normalConnectorOpacity;
	negativePenWidth = 0;
}

void ItemBase::getHoverColor(ConnectorItem *, QBrush * &brush, QPen * &pen, qreal & opacity, int & negativePenWidth) {
	brush = &hoverBrush;
	pen = &hoverPen;
	opacity = normalConnectorOpacity;
	negativePenWidth = 0;
}

void ItemBase::getEqualPotentialColor(ConnectorItem *, QBrush * &brush, QPen * &pen, qreal & opacity, int & negativePenWidth) {
	brush = &equalPotentialBrush;
	pen = &equalPotentialPen;
	opacity = 1.0;
	negativePenWidth = 0;
}

void ItemBase::slamZ(qreal newZ) {
	qreal z = floor(m_viewGeometry.z()) + newZ;
	m_viewGeometry.setZ(z);
	setZValue(z);
}

bool ItemBase::isEverVisible() {
	return m_everVisible;
}

void ItemBase::setEverVisible(bool v) {
	m_everVisible = v;
}

bool ItemBase::connectionIsAllowed(ConnectorItem *) {
	return true;
}

void ItemBase::collectExtraInfoValues(const QString & prop, QString & value, QStringList & extraValues, bool & ignoreValues) {
	Q_UNUSED(prop);
	Q_UNUSED(value);
	Q_UNUSED(extraValues);
	ignoreValues = false;
}

QString ItemBase::collectExtraInfoHtml(const QString & prop, const QString & value) {
	Q_UNUSED(prop);
	Q_UNUSED(value);
	return ___emptyString___;
}

ConnectorItem * ItemBase::rightClickedConnector() {
	return m_rightClickedConnector;
}

QColor ItemBase::connectedColor() {
	return connectedPen.color();
}

QColor ItemBase::unconnectedColor() {
	return unconnectedPen.color();
}

QColor ItemBase::standardConnectedColor() {
	return StandardConnectedColor;
}

QColor ItemBase::standardUnconnectedColor() {
	return StandardUnconnectedColor;
}

void ItemBase::setConnectedColor(QColor & c) {
	connectedPen.setColor(c);
	connectedBrush.setColor(c);
}

void ItemBase::setUnconnectedColor(QColor & c) {
	unconnectedPen.setColor(c);
	unconnectedBrush.setColor(c);
}

