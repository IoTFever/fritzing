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

$Revision: 2189 $:
$Author: merunga $:
$Date: 2009-01-16 17:12:45 +0100 (Fri, 16 Jan 2009) $

********************************************************************/

#ifndef PARTSEDITORCONNECTORSLAYERKINPALETTEITEM_H_
#define PARTSEDITORCONNECTORSLAYERKINPALETTEITEM_H_

#include "partseditorlayerkinpaletteitem.h"
#include "partseditorconnectorsconnectoritem.h"

class PartsEditorConnectorsLayerKinPaletteItem : public PartsEditorLayerKinPaletteItem {
public:
	PartsEditorConnectorsLayerKinPaletteItem(
			PaletteItemBase * chief, ModelPart * modelPart, ViewIdentifierClass::ViewIdentifier viewIdentifier,
			const ViewGeometry & viewGeometry, long id, QMenu* itemMenu, bool showingTerminalPoints)
                : PartsEditorLayerKinPaletteItem(chief, modelPart, viewIdentifier, viewGeometry, id, itemMenu)
	{
		m_showingTerminalPoints = showingTerminalPoints;
	}
protected:
	ConnectorItem* newConnectorItem(Connector *connector) {
		return new PartsEditorConnectorsConnectorItem(connector,this,m_showingTerminalPoints);
	}

	bool m_showingTerminalPoints;
};

#endif /* PARTSEDITORCONNECTORSLAYERKINPALETTEITEM_H_ */
