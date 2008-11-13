/*******************************************************************

Part of the Fritzing project - http://fritzing.org
Copyright (c) 2007-08 Fachhochschule Potsdam - http://fh-potsdam.de

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

#ifndef MODELBASE_H
#define MODELBASE_H

#include <QObject>
#include "modelpart.h"

class ModelBase : public QObject
{

public:
	ModelBase(bool makeRoot);
	virtual ~ModelBase();

	ModelPart * root();
	virtual ModelPart* retrieveModelPart(const QString & moduleID);
	virtual ModelPart * addModelPart(ModelPart * parent, ModelPart * copyChild);
	virtual bool load(const QString & fileName, ModelBase* refModel, bool doConnections);
	void save(const QString & fileName, bool asPart=false);
	virtual ModelPart * addPart(QString newPartPath, bool addToReference);
	virtual bool addPart(ModelPart * modelPart, bool update);


protected:
	ModelPart * m_root;
	ModelBase * m_referenceModel;

};

#endif
