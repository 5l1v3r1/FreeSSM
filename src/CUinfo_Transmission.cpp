/*
 * CUinfo_Transmission.cpp - Widget for displaying Transmission Control Unit information
 *
 * Copyright (C) 2008-2019 Comer352L
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "CUinfo_Transmission.h"
#include <QPixmap>


CUinfo_Transmission::CUinfo_Transmission(QWidget * parent) : CUinfo_abstract(parent)
{
	setupUi(this);
}


CUinfo_Transmission::~CUinfo_Transmission()
{
}


void CUinfo_Transmission::setSystemTypeText(QString Type)
{
	systemType_label->setText(Type);
}


void CUinfo_Transmission::setRomIDText(QString RomID)
{
	romID_label->setText(RomID);
}


void CUinfo_Transmission::setNrOfSupportedMBsSWs(unsigned int MBs, unsigned int SWs)
{
	nrofdatambs_label->setText( QString::number(MBs, 10) );
	nrofswitches_label->setText( QString::number(SWs, 10) );
}


void CUinfo_Transmission::setOBD2Supported(bool sup)
{
	if (sup)
		obd2system_label->setPixmap(QPixmap(QString::fromUtf8(":/icons/chrystal/22x22/ok.png")));
	else
		obd2system_label->setPixmap(QPixmap(QString::fromUtf8(":/icons/chrystal/22x22/editdelete.png")));
}

