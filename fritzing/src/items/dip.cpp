/*******************************************************************

Part of the Fritzing project - http://fritzing.org
Copyright (c) 2007-2010 Fachhochschule Potsdam - http://fh-potsdam.de

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

#include "dip.h"
#include "../utils/textutils.h"

static QStringList Spacings;

static int MinSipPins = 2;
static int MaxSipPins = 64;
static int MinDipPins = 4;
static int MaxDipPins = 64;

Dip::Dip( ModelPart * modelPart, ViewIdentifierClass::ViewIdentifier viewIdentifier, const ViewGeometry & viewGeometry, long id, QMenu * itemMenu, bool doLabel)
	: MysteryPart(modelPart, viewIdentifier, viewGeometry, id, itemMenu, doLabel)
{
}

Dip::~Dip() {
}

bool Dip::collectExtraInfo(QWidget * parent, const QString & family, const QString & prop, const QString & value, bool swappingEnabled, QString & returnProp, QString & returnValue, QWidget * & returnWidget)
{
	bool result = MysteryPart::collectExtraInfo(parent, family, prop, value, swappingEnabled, returnProp, returnValue, returnWidget);
	if (prop.compare("chip label", Qt::CaseInsensitive) == 0) {
		returnProp = tr("chip label");
	}
	return result;
}

bool Dip::isDIP() {
	QString package = modelPart()->properties().value("package", "");
	return (package.indexOf("DIP", 0, Qt::CaseInsensitive) >= 0);
}

bool Dip::otherPropsChange(const QMap<QString, QString> & propsMap) {
	QString layout = modelPart()->properties().value("package", "");
	return (layout.compare(propsMap.value("package", "")) != 0);
}

const QStringList & Dip::spacings() {
	if (Spacings.count() == 0) {
		Spacings << "300mil" << "400mil" << "600mil" << "900mil";
	}

	return Spacings;
}

QString Dip::genSipFZP(const QString & moduleid)
{
	return PaletteItem::genFZP(moduleid, "generic_sip_fzpTemplate", MinSipPins, MaxSipPins, 1);
}

QString Dip::genDipFZP(const QString & moduleid)
{
	return PaletteItem::genFZP(moduleid, "generic_dip_fzpTemplate", MinDipPins, MaxDipPins, 2);
}

QStringList Dip::collectValues(const QString & family, const QString & prop, QString & value) {
	if (prop.compare("pins", Qt::CaseInsensitive) == 0) {
		QStringList values;
		value = modelPart()->properties().value("pins");

		if (isDIP()) {
			for (int i = MinDipPins; i <= MaxDipPins; i += 2) {
				values << QString::number(i);
			}
		}
		else {
			for (int i = MinSipPins; i <= MaxSipPins; i++) {
				values << QString::number(i);
			}
		}
		
		return values;
	}

	return MysteryPart::collectValues(family, prop, value);
}


QString Dip::genModuleID(QMap<QString, QString> & currPropsMap)
{
	QString value = currPropsMap.value("package");
	QString pins = currPropsMap.value("pins");
	if (pins.isEmpty()) pins = "16";		// pick something safe
	QString moduleID;
	if (value.contains("sip", Qt::CaseInsensitive)) {
		return QString("generic_sip_%1_300mil").arg(pins);
	}
	else {
		int p = pins.toInt();
		if (p < 4) p = 4;
		if (p % 2 == 1) p--;
		return QString("generic_ic_dip_%1_300mil").arg(p);
	}
}

QString Dip::makePcbSvg(const QString & expectedFileName) 
{
	QStringList pieces = expectedFileName.split("_");
	if (pieces.count() != 4) return "";

	int pins = pieces.at(1).toInt();
	QString spacingString = pieces.at(2);

	QString header("<?xml version='1.0' encoding='UTF-8'?>\n"
				    "<svg baseProfile='tiny' version='1.2' width='%1in' height='%2in' viewBox='0 0 %3 %4' xmlns='http://www.w3.org/2000/svg'>\n"
				    "<desc>Fritzing footprint SVG</desc>\n"
					"<g id='silkscreen'>\n"
					"<line stroke='white' stroke-width='10' x1='10' x2='10' y1='10' y2='%5'/>\n"
					"<line stroke='white' stroke-width='10' x1='10' x2='%6' y1='%5' y2='%5'/>\n"
					"<line stroke='white' stroke-width='10' x1='%6' x2='%6' y1='%5' y2='10'/>\n"
					"<line stroke='white' stroke-width='10' x1='10' x2='%7' y1='10' y2='10'/>\n"
					"<line stroke='white' stroke-width='10' x1='%8' x2='%6' y1='10' y2='10'/>\n"
					"</g>\n"
					"<g id='copper1'><g id='copper0'>\n"
					"<rect fill='none' height='55' stroke='rgb(255, 191, 0)' stroke-width='20' width='55' x='32.5' y='32.5'/>\n");

	qreal outerBorder = 10;
	qreal silkSplitTop = 100;
	qreal offsetX = 60;
	qreal offsetY = 60;
	qreal spacing = TextUtils::convertToInches(spacingString) * 1000; 
	qreal totalWidth = 120 + spacing;
	qreal totalHeight = (100 * pins / 2) + (outerBorder * 2);
	qreal center = totalWidth / 2;

	QString svg = header.arg(totalWidth / 1000).arg(totalHeight / 1000).arg(totalWidth).arg(totalHeight)
							.arg(totalHeight - outerBorder).arg(totalWidth - outerBorder)
							.arg(center - (silkSplitTop / 2)).arg(center + (silkSplitTop / 2));

	QString circle("<circle cx='%1' cy='%2' fill='none' id='connector%3pin' r='27.5' stroke='rgb(255, 191, 0)' stroke-width='20'/>\n");

	int y = offsetY;
	for (int i = 0; i < pins / 2; i++) {
		svg += circle.arg(offsetX).arg(y).arg(i);
		svg += circle.arg(totalWidth - offsetX).arg(y).arg(pins - 1 - i);
		y += 100;
	}

	svg += "</g></g>\n";
	svg += "</svg>\n";

	return svg;
}
