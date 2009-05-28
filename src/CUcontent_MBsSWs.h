/*
 * CUcontent_MBsSWs.h - Widget for Reading of Measuring Blocks and Switches
 *
 * Copyright (C) 2008-2009 Comer352l
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

#ifndef CUCONTENT_MBSSWS_H
#define CUCONTENT_MBSSWS_H



#include <QtGui>
 #include <vector>
#include "ui_CUcontent_MBsSWs.h"
#include "AddMBsSWsDlg.h"
#include "SSMprotocol2.h"
#include "libFSSM.h"



class CUcontent_MBsSWs : public QWidget, private Ui::MBSWcontent_Form
{
	Q_OBJECT

public:
	CUcontent_MBsSWs(QWidget *parent, SSMprotocol2 *SSMP2dev, bool timemode = 0);
	~CUcontent_MBsSWs();
	bool setup();
	bool startMBSWreading();
	bool stopMBSWreading();
	bool setMBSWselection(std::vector<MBSWmetadata_dt> MBSWmetaList);
	void getCurrentMBSWselection(std::vector<MBSWmetadata_dt> *MBSWmetaList);
	void getCurrentTimeMode(bool *timemode);

private:
	SSMprotocol2 *_SSMP2dev;
	std::vector<mbsw_dt> _supportedMBs;
	std::vector<mbsw_dt> _supportedSWs;
	std::vector<MBSWmetadata_dt> _MBSWmetaList;
	bool _timemode;
	int _lastrefreshduration_ms;
	unsigned int _maxrowsvisible;
	QStringList _lastvalues;

	void setupUiFonts();
	void updateMWSWqueryListContent();
	void updateMBSWvalues(QStringList MBvalueStrList, QStringList MBunitStrList, int refreshduration_ms);
	void getSelectedTableWidgetRows(QTableWidget *tablewidget, unsigned int *selectedRowsIndexes, unsigned int *nrofselectedRows);
	void resizeEvent(QResizeEvent *event);
	bool eventFilter(QObject *obj, QEvent *event);
	void communicationError(QString adstr);

private slots:
	void startstopMBsSWsButtonPressed();
	void callStart();
	void callStop();
	void processMBSWRawValues(std::vector<unsigned int> rawValues, int refreshduration_ms);
	void addMBsSWs();
	void deleteMBsSWs();
	void moveupMBsSWs();
	void movedownMBsSWs();
	void setManipulateMBSWItemsButtonsStatus();
	void switchTimeMode();

signals:
	void error();

};



#endif
