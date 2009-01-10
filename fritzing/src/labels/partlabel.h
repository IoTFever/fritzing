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

#ifndef PARTLABEL_H
#define PARTLABEL_H

#include <QGraphicsTextItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

class PartLabel : public QGraphicsTextItem
{
 Q_OBJECT

public:
	PartLabel(class ItemBase * owner, const QString & text, QGraphicsItem * parent = 0 );   // itembase is not the parent	

	void showLabel(bool showIt);
	QRectF boundingRect() const;
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	QPainterPath PartLabel::shape() const;
	void setPlainText(const QString & text);
	bool initialized();
	void ownerMoved(QPointF newPos);

protected slots:
	void contentsChangedSlot();

protected:
	class ItemBase * m_owner;
	bool m_initialized;
	bool  m_doDrag;
	QPointF m_initialPosition;
	QPointF m_offset;
};

#endif
