/*
 * Transmission.h - Transmission Control Unit dialog
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

#ifndef TRANSMISSION_H
#define TRANSMISSION_H



#include <QtGui>
#include "ui_Transmission.h"
#include "SSMprotocol.h"
#include "FSSMdialogs.h"
#include "CUcontent_DCs_transmission.h"
#include "CUcontent_MBsSWs.h"
#include "CUcontent_Adjustments.h"
#include "ClearMemoryDlg.h"



class Transmission : public QMainWindow, private Ui::Transmission_Window
{
	Q_OBJECT

public:
	Transmission(SSMprotocol *ssmpdev, QString progversion = "");
	~Transmission();
 
private:
	enum mode_dt {DCs_mode=1, MBsSWs_mode=2, Adaptions_mode=3};

	SSMprotocol *_SSMPdev;
	QString _progversion;
	// Content backup parameters:
	MBSWmetadata_dt _lastMBSWmetaList[SSMP_MAX_MBSW];
	unsigned int _lastMBSWmetaList_len;
	bool _MBSWtimemode;
	// Pointer to content-widges:
	CUcontent_DCs_transmission *_content_DCs;
	CUcontent_MBsSWs *_content_MBsSWs;
	CUcontent_Adjustments *_content_Adjustments;
	// Current content/mode:
	mode_dt _mode;

	void setup();
	void runClearMemory(bool level);
	void setupUiFonts();
	void clearContent();
	void closeEvent(QCloseEvent *event);

private slots:
	void DTCs();
	void measuringblocks();
	void adjustments();
	void clearMemory();
	void clearMemory2();
	void communicationError(QString addstr = "");

};



#endif
