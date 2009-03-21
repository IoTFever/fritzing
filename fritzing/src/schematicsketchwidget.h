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

$Revision: 1617 $:
$Author: cohen@irascible.com $:
$Date: 2008-11-22 20:32:44 +0100 (Sat, 22 Nov 2008) $

********************************************************************/



#ifndef SCHEMATICSKETCHWIDGET_H
#define SCHEMATICSKETCHWIDGET_H

#include "pcbschematicsketchwidget.h"

struct ConnectorPair {
	ConnectorItem* connectorItem0;
	ConnectorItem * connectorItem1;
};

struct DistanceThing {
	int distance;
	bool fromConnector0;
};

class SchematicSketchWidget : public PCBSchematicSketchWidget
{
	Q_OBJECT

public:
    SchematicSketchWidget(ViewIdentifierClass::ViewIdentifier, QWidget *parent=0);

	void addViewLayers();
	bool canDeleteItem(QGraphicsItem * item);
	bool canCopyItem(QGraphicsItem * item);
	void addRatnestTarget(SketchWidget * target);

signals:
	void schematicDisconnectWireSignal(	ConnectorPairHash & moveItems,  QSet<ItemBase *> & deletedItems, QHash<ItemBase *, ConnectorPairHash *> & deletedConnections, QUndoCommand * parentCommand);

protected:
	void setWireVisible(Wire * wire);
	void makeWires(QList<ConnectorItem *> & partsConnectorItems, QList <Wire *> & ratsnestWires, Wire * & modelWire, RatsnestCommand *);
	void updateRatsnestStatus(CleanUpWiresCommand*, QUndoCommand *);
	void dealWithRatsnest(long fromID, const QString & fromConnectorID, 
								  long toID, const QString & toConnectorID,
								  bool connect, class RatsnestCommand *, bool doEmit);
	ConnectorItem * tryWire(ConnectorItem * wireConnectorItem, ConnectorItem * otherConnectorItem);
	ConnectorItem * tryParts(ConnectorItem * otherConnectorItem, ConnectorItem * wireConnectorItem, QList<ConnectorItem *> partsConnectorItems);
	bool reviewDeletedConnections(QSet<ItemBase *> & deletedItems, QHash<ItemBase *, ConnectorPairHash * > & deletedConnections, QUndoCommand * parentCommand);
	bool canChainMultiple();
	bool alreadyOnBus(ConnectorItem * busCandidate, ConnectorItem * otherCandidate);
	bool modifyNewWireConnections(Wire * dragWire, ConnectorItem * fromOnWire, ConnectorItem * from, ConnectorItem * to, QUndoCommand * parentCommand);
	void modifyNewWireConnectionsAux(Wire * dragWire, ConnectorItem * fromDragWire, 
									 ConnectorItem * fromConnectorItem,
									 ConnectorItem * toConnectorItem, QUndoCommand * parentCommand);
	void makeTwoWires(Wire * dragWire, ConnectorItem * fromDragWire, 
						 ConnectorItem * originalFromConnectorItem, ConnectorItem * fromConnectorItem,
						 ConnectorItem * originalToConnectorItem, ConnectorItem * toConnectorItem, 
						 QUndoCommand * parentCommand);
	ConnectorItem * lookForBreadboardConnection(ConnectorItem * connectorItem);
	void removeRatsnestWires(QList< QList<ConnectorItem *>* > & allPartConnectorItems, CleanUpWiresCommand *);
	ConnectorItem * findEmptyBusConnectorItem(ConnectorItem * busConnectorItem);
	void chainVisible(ConnectorItem * fromConnectorItem, ConnectorItem * toConnectorItem, bool connect);
	ConnectorItem * lookForNewBreadboardConnection(ConnectorItem * connectorItem, ItemBase * & newBreadboard);
	ConnectorItem * findEmptyBus(ItemBase * breadboard);
	void makeModifiedWire(Wire * wire, ConnectorItem * fromOnWire, 
		ConnectorItem * originalFromConnectorItem, ConnectorItem * newFromConnectorItem, 
		ConnectorItem * originalToConnectorItem, ConnectorItem * newToConnectorItem, QUndoCommand * parentCommand);
	ConnectorItem * findNearestPartConnectorItem(ConnectorItem * fromConnectorItem);
	const QString & hoverEnterWireConnectorMessage(QGraphicsSceneHoverEvent * event, ConnectorItem * item);
	void obsolete(QSet<ItemBase *> & deletedItems, QHash<ItemBase *, ConnectorPairHash *> & deletedConnections, QUndoCommand * parentCommand); 
	void reviewNet(QList<ConnectorItem *> & net, bool leaveAlone, ConnectorPairHash & tempDisconnectItems, 
				   ConnectorPairHash & moveItems, QSet<Wire *> & possibleOrphans, QSet<ItemBase *> & deletedItems,
				   QUndoCommand * parentCommand);
	void restoreNet(QList<ConnectorItem *> & net, QUndoCommand * parentCommand);
	void checkInNet(ConnectorItem * connectorItem, QList< QList<ConnectorItem *> * >  & allEnds, QList< QList<ConnectorItem *> * >  & newNets);
	void reviewDeletedConnectionsAux(QSet<ItemBase *> & deletedItems, QHash<ItemBase *, ConnectorPairHash *> & deletedConnections, 
								QList<ConnectorItem *> & affectedEnds, QUndoCommand * parentCommand);


protected:
	static void calcDistances(Wire * wire, QList<ConnectorItem *> & ends);
	static void clearDistances();
	static int calcDistance(Wire * wire, ConnectorItem * end, int distance, QList<Wire *> & distanceWires, bool & fromConnector0);
	static int calcDistanceAux(ConnectorItem * from, ConnectorItem * to, int distance, QList<Wire *> & distanceWires);

protected:
	QList<Wire *> m_deleteStash;
	QList<SketchWidget *> m_ratsnestTargets;

};

#endif
