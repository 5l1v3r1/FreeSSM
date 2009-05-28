/*
 * CUcontent_MBsSWs.cpp - Widget for Reading of Measuring Blocks and Switches
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

#include "CUcontent_MBsSWs.h"




CUcontent_MBsSWs::CUcontent_MBsSWs(QWidget *parent, SSMprotocol2 *SSMP2dev, bool timemode) : QWidget(parent)
{
	QHeaderView *headerview;
	_SSMP2dev = SSMP2dev;
	_supportedMBs.clear();
	_supportedSWs.clear();
	_MBSWmetaList.clear();
	_timemode = timemode;
	_lastrefreshduration_ms = 0;
	_maxrowsvisible = 0;
	_lastvalues.clear();

	// Setup GUI:
	setupUi(this);
	setupUiFonts();
	// Disable all GUI-elements:
	MBSWtitle_label->setEnabled( false );
	MBSWrefreshTimeTitle_label->setEnabled( false );
	MBSWrefreshTimeValue_label->setEnabled( false );
	selectedMBsSWs_tableWidget->setEnabled( false );
	startstopmbreading_pushButton->setEnabled( false );
	mbswadd_pushButton->setEnabled( false );
	mbswdelete_pushButton->setEnabled( false );
	mbswmoveup_pushButton->setEnabled( false );
	mbswmovedown_pushButton->setEnabled( false );
	timemode_pushButton->setEnabled( false );
	// Set content of time refresh-time labels:
	if (_timemode)
		MBSWrefreshTimeTitle_label->setText(tr("Block transfer rate:   "));
	else
		MBSWrefreshTimeTitle_label->setText(tr("Refresh duration:"));
	MBSWrefreshTimeValue_label->setText("---      ");
	// Set table column resize behavior:
	headerview = selectedMBsSWs_tableWidget->horizontalHeader();
	headerview->setResizeMode(0, QHeaderView::Stretch);
	headerview->setResizeMode(1, QHeaderView::Fixed); // resize doesn't work correctly in this constellation (Qt-bug)
	headerview->setResizeMode(2, QHeaderView::Fixed); // resize doesn't work correctly in this constellation (Qt-bug)
	// Set column widths (columns 2+3):
	selectedMBsSWs_tableWidget->setColumnWidth(1, 95);
	selectedMBsSWs_tableWidget->setColumnWidth(2, 58);
	// Install event-filter for MB/SW-table:
	selectedMBsSWs_tableWidget->viewport()->installEventFilter(this);
	// Connect signals and slots:
	connect(startstopmbreading_pushButton , SIGNAL( released() ), this, SLOT( startstopMBsSWsButtonPressed() ) ); 
	connect(mbswadd_pushButton , SIGNAL( released() ), this, SLOT( addMBsSWs() ) );
	connect(mbswdelete_pushButton , SIGNAL( released() ), this, SLOT( deleteMBsSWs() ) );
	connect(mbswmoveup_pushButton , SIGNAL( released() ), this, SLOT( moveupMBsSWs() ) );
	connect(mbswmovedown_pushButton , SIGNAL( released() ), this, SLOT( movedownMBsSWs() ) );
	connect(timemode_pushButton , SIGNAL( released() ), this, SLOT( switchTimeMode() ) );
	// NOTE: using released() instead of pressed() as workaround for a Qt-Bug occuring under MS Windows
	connect(selectedMBsSWs_tableWidget , SIGNAL( itemSelectionChanged() ), this, SLOT( setManipulateMBSWItemsButtonsStatus() ) );
}


CUcontent_MBsSWs::~CUcontent_MBsSWs()
{
	_SSMP2dev->stopMBSWreading();
	disconnect( _SSMP2dev, SIGNAL( newMBSWrawValues(std::vector<unsigned int>, int) ), this, SLOT( processMBSWRawValues(std::vector<unsigned int>, int) ) );
	disconnect( startstopmbreading_pushButton , SIGNAL( released() ), this, SLOT( startstopMBsSWsButtonPressed() ) ); 
	disconnect( mbswadd_pushButton , SIGNAL( released() ), this, SLOT( addMBsSWs() ) );
	disconnect( mbswdelete_pushButton , SIGNAL( released() ), this, SLOT( deleteMBsSWs() ) );
	disconnect( mbswmoveup_pushButton , SIGNAL( released() ), this, SLOT( moveupMBsSWs() ) );
	disconnect( mbswmovedown_pushButton , SIGNAL( released() ), this, SLOT( movedownMBsSWs() ) );
	disconnect( timemode_pushButton , SIGNAL(released() ), this, SLOT( switchTimeMode() ) );
	disconnect( selectedMBsSWs_tableWidget , SIGNAL( itemSelectionChanged() ), this, SLOT( setManipulateMBSWItemsButtonsStatus() ) );
	disconnect( _SSMP2dev , SIGNAL( stoppedMBSWreading() ), this, SLOT( callStop() ) );
	disconnect( _SSMP2dev , SIGNAL( startedMBSWreading() ), this, SLOT( callStart() ) );
}


bool CUcontent_MBsSWs::setup()
{
	bool ok;
	// Get supported MBs/SWs:
	ok = _SSMP2dev->getSupportedMBs(&_supportedMBs);
	if (ok)
		ok = _SSMP2dev->getSupportedSWs(&_supportedSWs);
	if (!ok)
	{
		// Reset CU-data:
		_supportedMBs.clear();
		_supportedSWs.clear();
	}
	_MBSWmetaList.clear();
	// Reset refresh time:
	_lastrefreshduration_ms = 0;
	MBSWrefreshTimeValue_label->setText("---      ");
	// Clear last recieved values:
	_lastvalues.clear();
	// Output titles and units of the selcted MBs/SWs
	updateMWSWqueryListContent();
	// *** Enable/Disable all GUI-elements:
	// Labels + tables:
	MBSWtitle_label->setEnabled( ok );
	MBSWrefreshTimeTitle_label->setEnabled( ok );
	MBSWrefreshTimeValue_label->setEnabled( ok );
	selectedMBsSWs_tableWidget->setEnabled( ok );
	// Time mode push-button:
	timemode_pushButton->setEnabled( ok );
	// Disable "Add"-button, if all supported MBs/SWs are already selected:
	if (_MBSWmetaList.size() >= (_supportedMBs.size()+_supportedSWs.size()))
		mbswadd_pushButton->setEnabled(false);
	else
		mbswadd_pushButton->setEnabled(true);
	// Enable/disable "Delete"/"Move Up"/"Move Down"-buttons:
	setManipulateMBSWItemsButtonsStatus();
	// Disable "Start"-button:
	startstopmbreading_pushButton->setEnabled(false);
	// Return result:
	return ok;
}


bool CUcontent_MBsSWs::setMBSWselection(std::vector<MBSWmetadata_dt> MBSWmetaList)
{
	unsigned int k = 0;
	// Check if MBSW-reading (and monitoring !) is in progress:
	if ((mbswadd_pushButton->isEnabled() == false) && (_MBSWmetaList.size() < (_supportedMBs.size() + _supportedSWs.size())))
		return false;
	// Check if the selected MBs/SWs are available:
	for (k=0; k<MBSWmetaList.size(); k++)
	{
		if (MBSWmetaList.at(k).blockType == 0)
		{
			if (MBSWmetaList.at(k).nativeIndex > (_supportedMBs.size()-1))
				return false;
		}
		else
		{
			if (MBSWmetaList.at(k).nativeIndex > (_supportedSWs.size()-1))
				return false;
		}
	}
	// Save MB/SW-list:
	_MBSWmetaList = MBSWmetaList;
	// Clear last recieved values:
	_lastvalues.clear();
	// Update MB/SW table content:
	updateMWSWqueryListContent();
	// Clear time information:
	MBSWrefreshTimeValue_label->setText("---      ");
	// Activate/deactivate buttons:
	if (_MBSWmetaList.size() > 0)
	{
		startstopmbreading_pushButton->setEnabled(true);
		mbswdelete_pushButton->setEnabled(true);
		connect(_SSMP2dev, SIGNAL( startedMBSWreading() ), this, SLOT( callStart() ));
	}
	else
		disconnect(_SSMP2dev, SIGNAL( startedMBSWreading() ), this, SLOT( callStart() ));
	if (_MBSWmetaList.size() >= (_supportedMBs.size() + _supportedSWs.size()))
		mbswadd_pushButton->setEnabled(false);	// "Add"-button aktivieren
	return true;
}


void CUcontent_MBsSWs::startstopMBsSWsButtonPressed()
{
	if (_SSMP2dev->state() == SSMprotocol2::state_MBSWreading)
		callStop();
	else
		callStart();
}


void CUcontent_MBsSWs::callStart()
{
	if (!startMBSWreading())
		communicationError(tr("=> Couldn't stop Measuring Blocks Reading."));
}


void CUcontent_MBsSWs::callStop()
{
	if (!stopMBSWreading())
		communicationError(tr("=> Couldn't start Measuring Blocks Reading."));
}


bool CUcontent_MBsSWs::startMBSWreading()
{
	SSMprotocol2::state_dt state = SSMprotocol2::state_needSetup;
	std::vector<MBSWmetadata_dt> usedMBSWmetaList;
	unsigned int k = 0;
	bool consistent = true;
	// Check premises:
	state = _SSMP2dev->state();
	if (state == SSMprotocol2::state_normal)
	{
		if (_MBSWmetaList.empty()) return false;
		disconnect( _SSMP2dev, SIGNAL( startedMBSWreading() ), this, SLOT( callStart() ) );
		// Start MB/SW-reading:
		if (!_SSMP2dev->startMBSWreading(_MBSWmetaList))
		{
			connect( _SSMP2dev, SIGNAL( startedMBSWreading() ), this, SLOT( callStart() ) );
			return false;
		}
	}
	else if (state == SSMprotocol2::state_MBSWreading)
	{
		// Verify consistency:
		if (!_SSMP2dev->getLastMBSWselection(&usedMBSWmetaList))
			return false;
		if (_MBSWmetaList.size() == usedMBSWmetaList.size())
		{
			for (k=0; k<_MBSWmetaList.size(); k++)
			{
				if ((_MBSWmetaList.at(k).blockType != usedMBSWmetaList.at(k).blockType) || (_MBSWmetaList.at(k).nativeIndex != usedMBSWmetaList.at(k).nativeIndex))
				{
					consistent = false;
					break;
				}
			}
		}
		if (!consistent)	// inconsistency detected !
		{
			// Stop MBSW-reading:
			stopMBSWreading();
			return false;
		}
	}
	else
		return false;
	// Connect signals and slots:
	connect( _SSMP2dev, SIGNAL( newMBSWrawValues(std::vector<unsigned int>, int) ), this, SLOT( processMBSWRawValues(std::vector<unsigned int>, int) ) );
	connect( _SSMP2dev , SIGNAL( stoppedMBSWreading() ), this, SLOT( callStop() ) );
	// Disable item manipulation buttons:
	mbswmoveup_pushButton->setEnabled(false);
	mbswmovedown_pushButton->setEnabled(false);
	mbswdelete_pushButton->setEnabled(false);
	mbswadd_pushButton->setEnabled(false);
	// Set text+icon of start/stop-button:
	startstopmbreading_pushButton->setText(tr(" Stop  "));
	QIcon startstopmbreadingicon(QString::fromUtf8(":/icons/chrystal/32x32/player_stop.png"));
	QSize startstopmbreadingiconsize(24,24);
	startstopmbreading_pushButton->setIcon(startstopmbreadingicon);
	startstopmbreading_pushButton->setIconSize(startstopmbreadingiconsize);
	return true;
}


bool CUcontent_MBsSWs::stopMBSWreading()
{
	disconnect( _SSMP2dev , SIGNAL( stoppedMBSWreading() ), this, SLOT( callStop() ) ); // must be disconnected before stopMBSWreading is called
	if (!_SSMP2dev->stopMBSWreading())
	{
		connect( _SSMP2dev , SIGNAL( stoppedMBSWreading() ), this, SLOT( callStop() ) ); // must be disconnected before stopMBSWreading is called
		return false;
	}
	disconnect( _SSMP2dev, SIGNAL( newMBSWrawValues(std::vector<unsigned int>, int) ), this, SLOT( processMBSWRawValues(std::vector<unsigned int>, int) ) );
	connect( _SSMP2dev, SIGNAL( startedMBSWreading() ), this, SLOT( callStart() ) );
	// set text+icon of start/stop-button:
	startstopmbreading_pushButton->setText(tr(" Start  "));
	QIcon startstopmbreadingicon(QString::fromUtf8(":/icons/chrystal/32x32/player_play.png"));
	QSize startstopmbreadingiconsize(24,24);
	startstopmbreading_pushButton->setIcon(startstopmbreadingicon);
	startstopmbreading_pushButton->setIconSize(startstopmbreadingiconsize);
	// Enable item manipulation buttons (depending on list content and selection):
	setManipulateMBSWItemsButtonsStatus();
	if (_MBSWmetaList.size() < (_supportedMBs.size() + _supportedSWs.size()))
		mbswadd_pushButton->setEnabled(true);
	return true;
}


void CUcontent_MBsSWs::processMBSWRawValues(std::vector<unsigned int> rawValues, int refreshduration_ms)
{
	QString defstr;
	QString rvstr;
	QString scaledValueStr;
	unsigned int k = 0;
	QStringList valueStrList;
	QStringList unitStrList;
	// SCALE ALL MBs AND SWs:
	for (k=0; k<_MBSWmetaList.size(); k++)	// MB/SW LOOP
	{
		if (_MBSWmetaList.at(k).blockType == 0)
		{
			if (libFSSM::raw2scaled( rawValues.at(k), _supportedMBs.at( _MBSWmetaList.at(k).nativeIndex ).scaleformula, _supportedMBs.at( _MBSWmetaList.at(k).nativeIndex ).precision, &scaledValueStr))
			{
				valueStrList.append(scaledValueStr);
				unitStrList.append(_supportedMBs.at( _MBSWmetaList.at(k).nativeIndex ).unit);
			}
			else
			{
				// USE RAW VALUE:
				valueStrList.append(QString::number(rawValues.at(k), 10));
				unitStrList.append("[RAW]");
			}
		}
		else
		{
			// GET UNIT OF THE SWITCH:
			if (rawValues.at(k) == 0)
			{
				valueStrList.append(_supportedSWs.at( _MBSWmetaList.at(k).nativeIndex ).unit.section('/',0,0));
			}
			else if (rawValues.at(k) == 1)
			{
				valueStrList.append(_supportedSWs.at( _MBSWmetaList.at(k).nativeIndex ).unit.section('/',1,1));
			}
			unitStrList.append("");
		}
	}
	// Display new values:
	updateMBSWvalues(valueStrList, unitStrList, refreshduration_ms);
}
// BETA

void CUcontent_MBsSWs::updateMBSWvalues(QStringList valueStrList, QStringList unitStrList, int refreshduration_ms)
{
	int k = 0;
	double secs_ilen = 0;
	double datarate = 0;

	_lastrefreshduration_ms = refreshduration_ms; // save last refresh duration
	QString timeValStr = "";
	QTableWidgetItem *valuetableelement;
	QTableWidgetItem *unittableelement;
	for (k=0; k<valueStrList.size(); k++)
	{
		// Output value:
		valuetableelement = new QTableWidgetItem( valueStrList.at(k) );
		valuetableelement->setTextAlignment(Qt::AlignCenter);
		selectedMBsSWs_tableWidget->setItem(k, 1, valuetableelement);
		// Output unit:
		unittableelement = new QTableWidgetItem( unitStrList.at(k) );
		selectedMBsSWs_tableWidget->setItem(k, 2, unittableelement);
	}
	// save current value strings:
	_lastvalues = valueStrList;
	// Output refresh duration:
	secs_ilen = static_cast<double>(refreshduration_ms) / 1000;
	if (_timemode == 0)
		timeValStr = QString::number(secs_ilen, 'f', 3) + " s";
	else
	{
		datarate = valueStrList.size() / secs_ilen;
		timeValStr = QString::number(datarate, 'f', 1) + " B/s";
	}
	MBSWrefreshTimeValue_label->setText(timeValStr);
}


void CUcontent_MBsSWs::addMBsSWs()
{
	unsigned int MBSWmetaList_len_old = _MBSWmetaList.size();
	// Open selection dialog:
	AddMBsSWsDlg *dlg = new AddMBsSWsDlg(this, _supportedMBs, _supportedSWs, &_MBSWmetaList);
	dlg->exec();
	delete dlg;
	// Update table:
	if (_MBSWmetaList.size() != MBSWmetaList_len_old)
	{
		// Clear last recieved values:
		_lastvalues.clear();
		// Update MB/SW table content:
		updateMWSWqueryListContent();
		// Clear time information:
		MBSWrefreshTimeValue_label->setText("---      ");
		// Select new MBs/SWs:
		QTableWidgetSelectionRange selrange((MBSWmetaList_len_old), 0, (_MBSWmetaList.size()-1), 2);
		selectedMBsSWs_tableWidget->setRangeSelected(selrange , true);
		if (_MBSWmetaList.size() >= _maxrowsvisible)
			// Scroll to end of the table:
			selectedMBsSWs_tableWidget->scrollToBottom();
	}
	// Activate/deactivate buttons:
	if (_MBSWmetaList.size() > 0)
	{
		startstopmbreading_pushButton->setEnabled(true);
		mbswdelete_pushButton->setEnabled(true);
		connect(_SSMP2dev, SIGNAL( startedMBSWreading() ), this, SLOT( callStart() ));
	}
	if (_MBSWmetaList.size() >= (_supportedMBs.size() + _supportedSWs.size()))
		mbswadd_pushButton->setEnabled(false);	// "Add"-button aktivieren
}


void CUcontent_MBsSWs::deleteMBsSWs()
{
	unsigned int k = 0;
	unsigned int selectedRowsIndexes[300] = {0,};
	unsigned int nrofselectedRows = 0;
	unsigned int startindex = 0;
	unsigned int endindex = 0;

	// GET INDEXES OF SELECTED ROWS:
	getSelectedTableWidgetRows(selectedMBsSWs_tableWidget, selectedRowsIndexes, &nrofselectedRows);
	if (nrofselectedRows < 1) return;
	// CHECK AND CORRECT START AND END INDEXES:
	startindex = selectedRowsIndexes[0];
	if (startindex > (_MBSWmetaList.size()-1)) return; // Cancel, if only empty table lines are selected
	endindex = selectedRowsIndexes[nrofselectedRows-1];
	if (endindex > (_MBSWmetaList.size()-1))
		endindex = (_MBSWmetaList.size()-1); // correct last index, if section exceeds end of list
	// DELETE MB/SWs FROM SELECTION LIST (METALIST):
	_MBSWmetaList.erase(_MBSWmetaList.begin()+startindex, _MBSWmetaList.begin()+endindex+1);
	// DELETE LAST RECIEVED VALUE(S):
	for (k=0; k<nrofselectedRows; k++)
		_lastvalues.removeAt(startindex + k);
	// UPDATE MB/SW TABLE CONTENT:
	updateMWSWqueryListContent();
	// Clear time information:
	MBSWrefreshTimeValue_label->setText("---      ");
	// ACTIVATE/DEACTIVATE BUTTONS:
	if (_MBSWmetaList.empty())
	{
		startstopmbreading_pushButton->setEnabled(false);
		mbswdelete_pushButton->setEnabled(false);
		disconnect(_SSMP2dev, SIGNAL( startedMBSWreading() ), this, SLOT( callStart() ));
	}
	if (_MBSWmetaList.size() < (_supportedMBs.size() + _supportedSWs.size()))	// if not all MBs/SWs are selected
		mbswadd_pushButton->setEnabled(true);
}


void CUcontent_MBsSWs::moveupMBsSWs()
{
	unsigned int selectedRowsIndexes[300] = {0,};
	unsigned int nrofselectedRows = 0;
	int rowToMoveDownIndex = 0;
	int rowToMoveDownTargetIndex = 0;
	MBSWmetadata_dt datablockToMoveDown = {0, 0};
	unsigned int k = 0;
	
	// GET SELECTED ROWS:
	getSelectedTableWidgetRows(selectedMBsSWs_tableWidget, selectedRowsIndexes, &nrofselectedRows);
	// CHECK AND CORRECT SELECTED ROWS:
	if ((nrofselectedRows < 1) || (selectedRowsIndexes[0] < 1) || (1 + selectedRowsIndexes[0] > _MBSWmetaList.size()))
		return;	// Cancel, if moving up is not possible
	if ((selectedRowsIndexes[0] + nrofselectedRows) > _MBSWmetaList.size()) // if selection exceed the end of the list
		nrofselectedRows = _MBSWmetaList.size() - selectedRowsIndexes[0];
	// NOTE: IN FACT WE ARE MOVING 1 ROW DOWN... 
	// GET START AND TERGET INDEX OF THE ROW THAT WILL BE MOVED:
	rowToMoveDownIndex = selectedRowsIndexes[0]-1;	
	rowToMoveDownTargetIndex = selectedRowsIndexes[nrofselectedRows-1];
	// MOVE MBs/SWs AT SELECTION LIST (METALIST):
	datablockToMoveDown = _MBSWmetaList.at(rowToMoveDownIndex);
	for (k=1; k<=nrofselectedRows; k++)
		_MBSWmetaList.at(rowToMoveDownIndex + (k-1)) = _MBSWmetaList.at(rowToMoveDownIndex + k);
	_MBSWmetaList.at(rowToMoveDownTargetIndex) = datablockToMoveDown;
	// MOVE LAST RECIEVED VALUE:
	if (_lastvalues.size() > rowToMoveDownTargetIndex)
		_lastvalues.move(rowToMoveDownIndex, rowToMoveDownTargetIndex);
	// UPDATE MB/SW TABLE CONTENT:
	updateMWSWqueryListContent();
	// RESELECT MOVED ROWS:
	QTableWidgetSelectionRange selrange(rowToMoveDownIndex, 0, (rowToMoveDownTargetIndex-1), 2);
	selectedMBsSWs_tableWidget->setRangeSelected(selrange, true);
	// SCROLL TO POSTION OF FIRST SELCTED ROW:
	QTableWidgetItem *item = new QTableWidgetItem;
	item = selectedMBsSWs_tableWidget->item(rowToMoveDownIndex, 0);
	selectedMBsSWs_tableWidget->scrollToItem(item, QAbstractItemView::EnsureVisible);
}


void CUcontent_MBsSWs::movedownMBsSWs()
{
	unsigned int selectedRowsIndexes[300] = {0,};
	unsigned int nrofselectedRows = 0;
	int rowToMoveUpIndex = 0;
	int rowToMoveUpTargetIndex = 0;
	MBSWmetadata_dt datablockToMoveUp = {0,0};
	unsigned int k = 0;
	QTableWidgetItem *item;

	// GET SELECTED ROWS:
	getSelectedTableWidgetRows(selectedMBsSWs_tableWidget, selectedRowsIndexes, &nrofselectedRows);
	// CHECK AND CORRECT SELECTED ROWS:
	if ((nrofselectedRows < 1) | (selectedRowsIndexes[nrofselectedRows-1]+1 >= _MBSWmetaList.size()))
		return;	// Cancle if moving is not possible
	// NOTE: IN FACT WE ARE MOVING 1 ROW UP... 
	// GET START AND TERGET INDEX OF THE ROW THAT WILL BE MOVED:
	rowToMoveUpIndex = selectedRowsIndexes[nrofselectedRows-1]+1;
	rowToMoveUpTargetIndex = selectedRowsIndexes[0];
	// MOVE MBs/SWs AT SELECTION LIST (METALIST):
	datablockToMoveUp = _MBSWmetaList.at(rowToMoveUpIndex);
	for (k=1; k<=nrofselectedRows; k++)
		_MBSWmetaList.at(rowToMoveUpIndex - (k-1)) = _MBSWmetaList.at(rowToMoveUpIndex - k);
	_MBSWmetaList.at(rowToMoveUpTargetIndex) = datablockToMoveUp;
	// MOVE LAST RECIEVED VALUE:
	if (_lastvalues.size() > rowToMoveUpIndex)
		_lastvalues.move(rowToMoveUpIndex, rowToMoveUpTargetIndex);
	// UPDATE MB/SW TABLE CONTENT:
	updateMWSWqueryListContent();
	// RESELECT MOVED ROWS:
	QTableWidgetSelectionRange selrange((rowToMoveUpTargetIndex+1), 0, rowToMoveUpIndex, 2);
	selectedMBsSWs_tableWidget->setRangeSelected(selrange , true);
	// SCROLL TO POSTION OF LAST SELCTED ROW:
	item = new QTableWidgetItem;
	item = selectedMBsSWs_tableWidget->item(rowToMoveUpIndex,0);
	selectedMBsSWs_tableWidget->scrollToItem(item, QAbstractItemView::EnsureVisible);
}


void CUcontent_MBsSWs::setManipulateMBSWItemsButtonsStatus()
{
	if (_SSMP2dev->state() == SSMprotocol2::state_MBSWreading) return;
	QList<QTableWidgetItem*> selitemslist;
	selitemslist = selectedMBsSWs_tableWidget->selectedItems();
	// NOTE: this retruns the nr. of selected cells, NOT THE NR OF ROWS ! Empty cells are not included ! 
	if (selitemslist.size() < 1)
	{
		mbswdelete_pushButton->setEnabled(false);
		mbswmovedown_pushButton->setEnabled(false);
		mbswmoveup_pushButton->setEnabled(false);
	}
	else
	{
		if (_MBSWmetaList.size() < 2)
		{
			mbswmoveup_pushButton->setEnabled(false);
			mbswmovedown_pushButton->setEnabled(false);
		}
		else
		{
			mbswmoveup_pushButton->setEnabled(true);
			mbswmovedown_pushButton->setEnabled(true);

		}
		mbswdelete_pushButton->setEnabled(true);
	}
}


void CUcontent_MBsSWs::updateMWSWqueryListContent()
{
	unsigned int k=0;
	int firstrowvisibleindex = 0;
	QTableWidgetItem *tableelement;

	// Delete table content:
	selectedMBsSWs_tableWidget->clearContents();
	// Set number of rows and vertical scroll bar policy:
	if (_MBSWmetaList.size() >= _maxrowsvisible)
	{
		selectedMBsSWs_tableWidget->setRowCount(_MBSWmetaList.size());
		selectedMBsSWs_tableWidget->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
		// Check if get white space area at the bottom of the table:
		firstrowvisibleindex = selectedMBsSWs_tableWidget->rowAt(0);
		if (firstrowvisibleindex+_maxrowsvisible > _MBSWmetaList.size())
		{
			selectedMBsSWs_tableWidget->setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );
			selectedMBsSWs_tableWidget->scrollToBottom();
			selectedMBsSWs_tableWidget->setVerticalScrollMode( QAbstractItemView::ScrollPerItem );
		}
	}
	else
	{
		selectedMBsSWs_tableWidget->setRowCount(_maxrowsvisible);
		selectedMBsSWs_tableWidget->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );	// only necessary for delete
		selectedMBsSWs_tableWidget->scrollToTop();						// only necessary for delete
	}
	// Fill table:
	for (k=0; k<_MBSWmetaList.size(); k++)
	{
		if (_MBSWmetaList.at(k).blockType == 0)		// MB
		{
			// Output title:
			tableelement = new QTableWidgetItem( _supportedMBs.at(_MBSWmetaList.at(k).nativeIndex).title );
			selectedMBsSWs_tableWidget->setItem(k, 0, tableelement);
			// Output unit:
			tableelement = new QTableWidgetItem( _supportedMBs.at(_MBSWmetaList.at(k).nativeIndex).unit );
			selectedMBsSWs_tableWidget->setItem(k, 2, tableelement);
		}
		else	// SW
		{
			// Output title:
			tableelement = new QTableWidgetItem( _supportedSWs.at(_MBSWmetaList.at(k).nativeIndex).title );
			selectedMBsSWs_tableWidget->setItem(k, 0, tableelement);
		}
		// Output last value string:
		if (static_cast<unsigned int>(_lastvalues.size()) > k)	// size() = last index + 1
		{
			tableelement = new QTableWidgetItem( _lastvalues.at(k) );
			tableelement->setTextAlignment(Qt::AlignCenter);
			selectedMBsSWs_tableWidget->setItem(k, 1, tableelement);
		}
	}
}


void CUcontent_MBsSWs::switchTimeMode()
{
	QString timeValStr = "---      ";
	_timemode = !_timemode;
	if (_timemode)
	{
		MBSWrefreshTimeTitle_label->setText(tr("Block transfer rate:   "));
		if (_lastrefreshduration_ms > 0)
		{
			double datarate = static_cast<double>(1000 * _MBSWmetaList.size()) / _lastrefreshduration_ms;
			timeValStr = QString::number(datarate, 'f', 1) + " B/s";
		}
	}
	else
	{
		MBSWrefreshTimeTitle_label->setText(tr("Refresh duration:"));
		if (_lastrefreshduration_ms > 0)
		{
			double sec = static_cast<double>(_lastrefreshduration_ms) / 1000;
			timeValStr = QString::number(sec, 'f', 3) + " s";
		}
	}
	MBSWrefreshTimeValue_label->setText(timeValStr);
}


void CUcontent_MBsSWs::getSelectedTableWidgetRows(QTableWidget *tablewidget, unsigned int *selectedRowsIndexes, unsigned int *nrofselectedRows)
{
	// GET INDEXES OF SELCTED ROWS:
	*nrofselectedRows = 0;
	QList<QTableWidgetSelectionRange> selectedRanges;
	selectedRanges = tablewidget->selectedRanges();//selectedIndexes();
	if (selectedRanges.size() < 1) return;
	int k=0;
	int m=0;
	int rows=0;
	for (k=0; k<selectedRanges.size(); k++)
	{
		rows = selectedRanges.at(k).bottomRow() - selectedRanges.at(k).topRow() + 1;
		for (m=0; m<rows; m++)
		{
			selectedRowsIndexes[*nrofselectedRows] = selectedRanges.at(k).topRow() + m;
			(*nrofselectedRows)++;
		}
	}
}


void CUcontent_MBsSWs::getCurrentMBSWselection(std::vector<MBSWmetadata_dt> *MBSWmetaList)
{
	*MBSWmetaList = _MBSWmetaList;
}


void CUcontent_MBsSWs::getCurrentTimeMode(bool *timemode)
{
	*timemode = _timemode;
}


void CUcontent_MBsSWs::resizeEvent(QResizeEvent *event)
{
	int rowheight = 0;
	int vspace = 0;
	QHeaderView *headerview;
	unsigned int minnrofrows = 0;
	// Get available vertical space (for rows) and height per row:
	if (selectedMBsSWs_tableWidget->rowCount() < 1)
		selectedMBsSWs_tableWidget->setRowCount(1); // Temporary create a row to get the row hight
	rowheight = selectedMBsSWs_tableWidget->rowHeight(0);
	headerview = selectedMBsSWs_tableWidget->horizontalHeader();
	vspace = selectedMBsSWs_tableWidget->viewport()->height();
	// Temporary switch to "Scroll per Pixel"-mode to ensure auto-scroll (prevent white space between bottom of the last row and the lower table border)
	selectedMBsSWs_tableWidget->setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );
	// Calculate and set nr. of rows:
	_maxrowsvisible = static_cast<unsigned int>(trunc((vspace-1)/rowheight) + 1);
	if (_maxrowsvisible < _MBSWmetaList.size())
		minnrofrows = _MBSWmetaList.size();
	else
		minnrofrows = _maxrowsvisible;
	selectedMBsSWs_tableWidget->setRowCount(minnrofrows);
	// Set vertical scroll bar policy:
	if (minnrofrows > _MBSWmetaList.size())
		selectedMBsSWs_tableWidget->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	else
		selectedMBsSWs_tableWidget->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
	// Switch back to "Scroll per Item"-mode:
	selectedMBsSWs_tableWidget->setVerticalScrollMode( QAbstractItemView::ScrollPerItem ); // auto-scroll is triggered; Maybe this is a Qt-Bug, we don't want that  here...
	// accept event:
	event->accept();
}


bool CUcontent_MBsSWs::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == selectedMBsSWs_tableWidget->viewport())
	{
		if (event->type() == QEvent::Wheel)
		{
			if (selectedMBsSWs_tableWidget->verticalScrollBarPolicy() ==  Qt::ScrollBarAlwaysOff)
			// ...or _maxrowsvisible > _MBSWmetaList_len
				return true;	// filter out
			else
				return false;
		}
	}
	// Pass the event on to the parent class
	return QWidget::eventFilter(obj, event);
}


void CUcontent_MBsSWs::communicationError(QString adstr)
{
	if (adstr.size() > 0) adstr.prepend('\n');
	QMessageBox msg( QMessageBox::Critical, tr("Communication Error"), tr("Communication Error:\n- No or invalid answer from Control Unit -") + adstr, QMessageBox::Ok, this);
	QFont msgfont = msg.font();
	msgfont.setPixelSize(12); // 9pts
	msg.setFont( msgfont );
	msg.show();
	msg.exec();
	msg.close();
	emit error();
}


void CUcontent_MBsSWs::setupUiFonts()
{
	// SET FONT FAMILY AND FONT SIZE
	// OVERWRITES SETTINGS OF ui_FreeSSM.h (made with QDesigner)
	QFont contentfont = QApplication::font();
	contentfont.setPixelSize(12);// 9pts
	contentfont.setBold(false);
	this->setFont(contentfont);
	// Table title:
	QFont tabletitlefont = contentfont;
	tabletitlefont.setUnderline(true);
	MBSWtitle_label->setFont(tabletitlefont);
	// Table:
	selectedMBsSWs_tableWidget->setFont(contentfont);
	// Buttons:
	startstopmbreading_pushButton->setFont(contentfont);
	mbswadd_pushButton->setFont(contentfont);
	mbswdelete_pushButton->setFont(contentfont);
	mbswmoveup_pushButton->setFont(contentfont);
	mbswmovedown_pushButton->setFont(contentfont);
	// Refresh interval labels:
	MBSWrefreshTimeTitle_label->setFont(contentfont);
	MBSWrefreshTimeValue_label->setFont(contentfont);
}

