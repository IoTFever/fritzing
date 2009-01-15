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

$Revision: 1886 $:
$Author: merunga $:
$Date: 2008-12-18 19:17:13 +0100 (Thu, 18 Dec 2008) $

********************************************************************/


#ifndef TERMINALPOINTITEM_H_
#define TERMINALPOINTITEM_H_

#include <QGraphicsRectItem>
#include <QPen>
#include "../resizablemovablegraphicsrectitem.h"

class PartsEditorConnectorItem;

class TerminalPointItem : public ResizableMovableGraphicsRectItem {
public:
	TerminalPointItem(PartsEditorConnectorItem *parent, bool movable=true);
	QPointF point();
	void updatePoint();

	bool isOutSideConnector();

protected:
	void initPen();
	void drawCross();

	void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

	QPen m_linePen;
	QGraphicsLineItem *m_hLine;
	QGraphicsLineItem *m_vLine;

	PartsEditorConnectorItem *m_parent;

	static const qreal size;
};

#endif /* TERMINALPOINTITEM_H_ */
