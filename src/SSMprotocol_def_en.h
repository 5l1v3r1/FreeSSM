/*
 * SSMprotocol_def_en.h - SSM-protocol-definitions
 *
 * Copyright © 2008 Comer352l
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


#ifndef SSMPROTOCOL_DEF_EN_H
#define SSMPROTOCOL_DEF_EN_H


#include <QStringList>


class SSMprotocol_def_en
{

private:
	QStringList MB_defs_en;
	QStringList SW_defs_en;
	QStringList DTC_SUBARU_defs_en;
	QStringList DTC_OBD_defs_en;
	QStringList CC_defs_en;
	QStringList actuator_defs_en;
	QStringList adjustment_defs_en;

public:
	SSMprotocol_def_en();
	const QStringList MBrawDefs();
	const QStringList SWrawDefs();
	const QStringList SUBDTCrawDefs();
	const QStringList OBDDTCrawDefs();
	const QStringList CCCCrawDefs();
	const QStringList ActuatorRawDefs();
	const QStringList AdjustmentRawDefs();

};


#endif
