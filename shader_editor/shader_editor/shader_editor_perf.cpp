/*
    Copyright (C) 2013  Martin Kuèeròák
    Contact: email:  xkucer64@stud.fit.vutbr.cz
             email2: bubak671415@seznam.cz

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    Description:
    (kódování: WindowsCP1250)
    funkce shader_editor, pracují s performance èítaèi
    Nvidia a AMD funkce
        
*/

#include "shader_editor.h"
#include "perf_display_organizer.h"
#include "logger.h"
#include <cstdio>
#include <sstream>
#define GPAQTT_DESCRIPTION 0
#define GPAQTT_VALUE 1
#define GPAQTT_PURE_VALUE 2

// ********************************************************
// Set up NVPMAPI
#define NVPM_INITGUID
#include "Include/NVidia/NvPmApi.Manager.h"
#include "Include/NVidia/NvPmApi.h"
//#include "Include/NVidia/NvPmApi.h"

// Simple singleton implementation for grabbing the NvPmApi
static NvPmApiManager S_NVPMManager;
extern NvPmApiManager *GetNvPmApiManager() {return &S_NVPMManager;}
const NvPmApi *GetNvPmApi() {return S_NVPMManager.Api();}
NVPMContext hNVPMContext(0);

shader_editor * mainClass_Pointer;

std::map<int, NVPMCounterID> NVApi_TableRowToCounterID;
std::map<NVPMCounterID, int> NVApi_counterIDToTableRow;

void shader_editor::takePerfSnapshot_ATI()
{
	std::vector<QTableWidgetItem*> itemVector = getItems_takePerfSnapshot_ATI();

	//writeToConsoleQt(QtDebugMsg,QString("takePerfSnapshot_ATI with ")+QString::number(itemVector.size())+QString(" items"));

	for (int i = 0; i < itemVector.size(); i++)
	{
		ui.GPA_counter_list->setItem(perfUniversal_to_table[i],GPAQTT_VALUE, itemVector[i]);
	}
}

std::vector<QTableWidgetItem*> shader_editor::getItems_takePerfSnapshot_ATI()
{
	std::vector<QTableWidgetItem * > itemVector;
	if (!perfATI_setup)
	{
		//vypsat chybu
		logger::debug("perfATI_setup failed");
		return itemVector;
	}

	GPA_Status retval[3];
	static gpa_uint32 currentWaitSessionID = 1;
	gpa_uint32 sessionID;
	retval[0]=GPA_BeginSession( &sessionID );
	gpa_uint32 numRequiredPasses;
	GPA_GetPassCount( &numRequiredPasses );
	for (unsigned i=0; i<numRequiredPasses;i++)
	{
		retval[1]=GPA_BeginPass();
			retval[2]=GPA_BeginSample(0);
				ui.osgGLWidget->forcePaintGL();
			GPA_EndSample();
		GPA_EndPass();
	}
	GPA_EndSession();

	bool readyResult = false;
	GPA_Status sessionStatus;
	do 
	{
 		 		sessionStatus = GPA_IsSessionReady( &readyResult,
 		 			sessionID );
	} while (!readyResult);


	if ( readyResult )
	{
		gpa_uint32 counter_count;
		GPA_Status retval;
		if((retval=GPA_GetNumCounters(&counter_count))!=GPA_STATUS_OK)
		{
			//vypsat chybu
			writeToConsoleQt(QtCriticalMsg,QString("GPA_GetNumCounters failed "));
			return itemVector;
		}

		//writeToConsoleQt(QtDebugMsg,QString("getItems_takePerfSnapshot_ATI with ")+QString::number(counter_count)+QString(" counters"));



		QTableWidgetItem * this_item;
		gpa_uint32 result_ui32;
		gpa_uint64 result_ui64;
		gpa_float32 result_f32;
		gpa_float64 result_f64;
		GPA_Type dataType;
		GPA_Usage_Type usageType;
		QString this_item_string;
		const char * debugname;
// 		GPA_Status GPA_GetSampleUInt32( gpa_uint32 sessionID,
// 			gpa_uint32 sampleID,
// 			gpa_uint32 counterID,
// 			gpa_uint32* result );
		//		retval	GPA_STATUS_ERROR_SAMPLE_NOT_FOUND	GPA_Status
		//GPA_STATUS_ERROR_SAMPLE_NOT_FOUND
		/*GPA_Status GPA_GetCounterUsageType( gpa_uint32 index,
			GPA_Usage_Type usageType );*/


		for (unsigned i=0; i<counter_count; i++)
		{

			GPA_GetCounterName(i,&debugname);
			retval=GPA_GetCounterDataType( i,&dataType);

			gpa_uint32 enabledCounterIndex;
			GPA_GetEnabledIndex( i, &enabledCounterIndex );

			//char buffer[100];
			//buffer[0] = '\0';

			retval=GPA_GetCounterUsageType( enabledCounterIndex, &usageType );

			if(dataType == GPA_TYPE_UINT32)
			{				
				retval=GPA_GetSampleUInt32(sessionID,0,enabledCounterIndex,&result_ui32);
				//this_item_string=QString::number((double)result_ui32, 'f', -1);//this_item_string.setNum((double)result_ui32, 'f', 0);
				//sprintf(buffer, "%u",result_ui32);
				std::stringstream ss;
				unsigned long fuck_it = result_ui32;
				ss << std::fixed << fuck_it;
				//ss.str
				this_item_string = QString(ss.str().c_str());
			}
			if(dataType == GPA_TYPE_UINT64)
			{
				retval=GPA_GetSampleUInt64(sessionID,0,enabledCounterIndex,&result_ui64);
				//this_item_string=QString::number((double)result_ui32, 'f', -1);//this_item_string.setNum((double)result_ui64, 'f', 0);
				/*sprintf(buffer, "%lu",result_ui64);
				this_item_string = QString(buffer);*/
				std::stringstream ss;
				unsigned long fuck_it = result_ui64;
				ss << std::fixed << fuck_it;
				//ss.str
				this_item_string = QString(ss.str().c_str());
			}
			if(dataType == GPA_TYPE_FLOAT32)
			{
				retval=GPA_GetSampleFloat32(sessionID,0,enabledCounterIndex,&result_f32);
				this_item_string=this_item_string.setNum(result_f32, 'f');
			}
			if(dataType == GPA_TYPE_FLOAT64)
			{
				retval=GPA_GetSampleFloat64(sessionID,0,enabledCounterIndex,&result_f64);
				if(usageType == GPA_Usage_Type::GPA_USAGE_TYPE_ITEMS)
					this_item_string=this_item_string.setNum(result_f64, 'f', 0);
				else
					this_item_string=this_item_string.setNum(result_f64);
			}
			if (retval!=GPA_STATUS_OK)
			{
				this_item_string=enumToStringGPA(retval);
			}
			//retval=GPA_GetCounterUsageType( enabledCounterIndex, &usageType );
			this_item_string+=enumToStringGPA_UsageType(usageType);
			//retval=GPA_GetSampleUInt32(sessionID,1,i,&result);
			this_item = new QTableWidgetItem(this_item_string);
			this_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			this_item->setToolTip("Skutecna hodnota");
			itemVector.push_back(this_item);
			//ui.GPA_counter_list->setItem(perfUniversal_to_table[i],GPAQTT_VALUE, this_item);

// 			this_item = new QTableWidgetItem(this_item_string);
// 			this_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
// 			this_item->setToolTip("Hodnota citace");
// 			ui.GPA_counter_list->setItem(i,GPAQTT_PURE_VALUE, this_item);
		}

		
	}
	//writeToConsoleQt(QtDebugMsg,QString("getItems_takePerfSnapshot_ATI returning with ")+QString::number(itemVector.size())+QString(" items"));
	return itemVector;
}

void shader_editor::initPerfCounters_ATI()
{
	if (!perfATI_initialized)
	{
		void (*loggingCallback)(GPA_Logging_Type, const char* );
		//GPA_LoggingCallbackPtrType * logginCallback = &logger::GPA_logging_function;
		loggingCallback = &(logger::GPA_logging_function);
		//GPA_RegisterLoggingCallback(GPA_Logging_Type::GPA_LOGGING_ALL,loggingCallback);
		HGLRC glhandle = ui.osgGLWidget->getGLContext();
		GPA_Status retval;
		if ((retval =  GPA_OpenContext(glhandle))!=GPA_STATUS_OK)
		{

			//sem doplnit nìco na chybu
			QString errmsg;
			logger::debug(errmsg.append("perfATI_initialized failed <").append(get_GPA_Status_string(retval)).append(">").toStdString().c_str());
			return;
		}
		perfATI_initialized = true;

	}
	gpa_uint32 counter_count;
	GPA_Status retval;
	if((retval=GPA_GetNumCounters(&counter_count))!=GPA_STATUS_OK)
	{
		//vypsat chybu
		return;
	}

	setup_hiearchy(gpuVendor);
	ui.GPA_counter_list->setRowCount(counter_count+perf_dv_groups->size()+1); //po jednom roce zjisteno ze tam ma byt +1
	ui.GPA_counter_list->setColumnCount(2);
	ui.GPA_counter_list->setHorizontalHeaderItem(GPAQTT_DESCRIPTION,new QTableWidgetItem("Counters"));
	ui.GPA_counter_list->setHorizontalHeaderItem(GPAQTT_VALUE,new QTableWidgetItem("Value"));
	//ui.GPA_counter_list->setHorizontalHeaderItem(GPAQTT_PURE_VALUE,new QTableWidgetItem("Pure Value"));

	const char * description;
	const char * name;
	QTableWidgetItem * this_item;
	std::vector<std::string> counter_name_list;
	std::vector<std::string> counter_desc_list;
	std::vector<bool>		counter_name_done;
	//std::map<std::string,> hiearchy;
	for(unsigned i = 0; i < counter_count; i++)
	{
		GPA_GetCounterName(i,&name);/*static_cast<const char **>(&name)*/
		GPA_GetCounterDescription(i,&description);/*static_cast<const char **>(&description)*/

		counter_name_list.push_back(name);
		counter_desc_list.push_back(std::string(description));
		counter_name_done.push_back(false);
	}
	int this_row=0;
	std::string this_group;

	QBrush major_background(QColor(191,255,191));

	{
		const char * this_item_group;

		for (std::vector<std::string>::iterator it=perf_dv_groups->begin(); it!=perf_dv_groups->end(); it++)
		{
			this_group = *it;
			this_item = new QTableWidgetItem(it->c_str());
			this_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			//this_item->setToolTip(Qt::convertFromPlainText(description));
			this_item->setBackground(major_background);
			ui.GPA_counter_list->setItem(this_row, GPAQTT_DESCRIPTION, this_item);
			this_item = new QTableWidgetItem();
			this_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			//this_item->setToolTip(Qt::convertFromPlainText(description));
			this_item->setBackground(major_background);
			ui.GPA_counter_list->setItem(this_row, GPAQTT_VALUE, this_item);
			perf_dv_group_start->push_back(this_row);
			perfUniversal_table_colapsed.push_back(false);
			this_row++;

			for(int i=0; i<counter_name_list.size(); i++)
			{

				if(counter_name_done[i] == false)
				{
					//this_item_group = counter_name_list[i];
					//this_item_group = perf_dv_hiearchy[counter_name_list[i]];
					if (perf_dv_hiearchy[counter_name_list[i]] == this_group)
					{
						counter_name_done[i]=true;
						this_item = new QTableWidgetItem(counter_name_list[i].c_str());
						this_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
						this_item->setToolTip(QString(counter_desc_list[i].c_str()));
						//this_item->setBackground(major_background);
						ui.GPA_counter_list->setItem(this_row, GPAQTT_DESCRIPTION, this_item);
						perfUniversal_to_table[i] = this_row;
						this_row++;
					}
				}
			}
		}
		bool unrecognized=false;
		std::vector<int> unrecognized_list;
		for (int i=0; i<counter_name_done.size();i++)
		{
			if(counter_name_done[i] == false)
			{
				unrecognized=true;
				unrecognized_list.push_back(i);
			}
		}
		if (unrecognized)
		{
			this_item = new QTableWidgetItem("unrecognized");
			this_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			this_item->setBackground(major_background);
			ui.GPA_counter_list->setItem(this_row, GPAQTT_DESCRIPTION, this_item);

			this_item = new QTableWidgetItem();
			this_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			this_item->setBackground(major_background);
			ui.GPA_counter_list->setItem(this_row, GPAQTT_VALUE, this_item);

			perf_dv_group_start->push_back(this_row);
			perfUniversal_table_colapsed.push_back(false);
			this_row++;
			for (int i=0;i<unrecognized_list.size();i++)
			{
				this_item = new QTableWidgetItem(counter_name_list[unrecognized_list[i]].c_str());
				this_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
				this_item->setToolTip(QString(counter_desc_list[unrecognized_list[i]].c_str()));
				ui.GPA_counter_list->setItem(this_row, GPAQTT_DESCRIPTION, this_item);
				perfUniversal_to_table[unrecognized_list[i]] = this_row;
				this_row++;
			}
		}
	}
	perf_dv_group_start->push_back(this_row);
	perfUniversal_table_colapsed.push_back(false);

// 		this_item = new QTableWidgetItem(name);
// 		this_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
// 		this_item->setToolTip(Qt::convertFromPlainText(description));
// 		ui.GPA_counter_list->setItem(i,GPAQTT_DESCRIPTION, this_item);


	retval=GPA_EnableAllCounters();
	perfATI_setup = true;
}

void shader_editor::hidePerfRow_ATI( int row, int column )
{
	int group_start_position=0;
	int group_end_position;
	for(std::vector<int>::iterator it = perf_dv_group_start->begin(); it!=perf_dv_group_start->end(); it++, group_start_position++)
	{
		if (*it==row)
		{
			group_end_position = (*perf_dv_group_start)[group_start_position+1];
			if (perfUniversal_table_colapsed[group_start_position])
			{
				perfUniversal_table_colapsed[group_start_position]=false;
				for (int st=(*perf_dv_group_start)[group_start_position]+1; st<group_end_position; st++)
					ui.GPA_counter_list->showRow(st);
			}
			else
			{
				perfUniversal_table_colapsed[group_start_position]=true;
				for (int st=(*perf_dv_group_start)[group_start_position]+1; st<group_end_position; st++)
					ui.GPA_counter_list->hideRow(st);
			}
			return;
		}
	}
	// 	if (isCtrlPressed())
	// 	{
	// 		ui.GPA_counter_list->hideRow(row);
	// 	}
}

char* shader_editor::enumToStringGPA( int val )
{
	switch(val)
	{	
	case	GPA_STATUS_OK: return "GPA_STATUS_OK";
	case	GPA_STATUS_ERROR_NULL_POINTER: return "GPA_STATUS_ERROR_NULL_POINTER";
	case	GPA_STATUS_ERROR_COUNTERS_NOT_OPEN: return "GPA_STATUS_ERROR_COUNTERS_NOT_OPEN";
	case	GPA_STATUS_ERROR_COUNTERS_ALREADY_OPEN: return "GPA_STATUS_ERROR_COUNTERS_ALREADY_OPEN";
	case	GPA_STATUS_ERROR_INDEX_OUT_OF_RANGE: return "GPA_STATUS_ERROR_INDEX_OUT_OF_RANGE";
	case	GPA_STATUS_ERROR_NOT_FOUND: return "GPA_STATUS_ERROR_NOT_FOUND";
	case	GPA_STATUS_ERROR_ALREADY_ENABLED: return "GPA_STATUS_ERROR_ALREADY_ENABLED";
	case	GPA_STATUS_ERROR_NO_COUNTERS_ENABLED: return "GPA_STATUS_ERROR_NO_COUNTERS_ENABLED";
	case	GPA_STATUS_ERROR_NOT_ENABLED: return "GPA_STATUS_ERROR_NOT_ENABLED";
	case	GPA_STATUS_ERROR_SAMPLING_NOT_STARTED: return "GPA_STATUS_ERROR_SAMPLING_NOT_STARTED";
	case	GPA_STATUS_ERROR_SAMPLING_ALREADY_STARTED: return "GPA_STATUS_ERROR_SAMPLING_ALREADY_STARTED";
	case	GPA_STATUS_ERROR_SAMPLING_NOT_ENDED: return "GPA_STATUS_ERROR_SAMPLING_NOT_ENDED";
	case	GPA_STATUS_ERROR_NOT_ENOUGH_PASSES: return "GPA_STATUS_ERROR_NOT_ENOUGH_PASSES";
	case	GPA_STATUS_ERROR_PASS_NOT_ENDED: return "GPA_STATUS_ERROR_PASS_NOT_ENDED";
	case	GPA_STATUS_ERROR_PASS_NOT_STARTED: return "GPA_STATUS_ERROR_PASS_NOT_STARTED";
	case	GPA_STATUS_ERROR_PASS_ALREADY_STARTED: return "GPA_STATUS_ERROR_PASS_ALREADY_STARTED";
	case	GPA_STATUS_ERROR_SAMPLE_NOT_STARTED: return "GPA_STATUS_ERROR_SAMPLE_NOT_STARTED";
	case	GPA_STATUS_ERROR_SAMPLE_ALREADY_STARTED: return "GPA_STATUS_ERROR_SAMPLE_ALREADY_STARTED";
	case	GPA_STATUS_ERROR_SAMPLE_NOT_ENDED: return "GPA_STATUS_ERROR_SAMPLE_NOT_ENDED";
	case	GPA_STATUS_ERROR_CANNOT_CHANGE_COUNTERS_WHEN_SAMPLING: return "GPA_STATUS_ERROR_CANNOT_CHANGE_COUNTERS_WHEN_SAMPLING";
	case	GPA_STATUS_ERROR_SESSION_NOT_FOUND: return "GPA_STATUS_ERROR_SESSION_NOT_FOUND";
	case	GPA_STATUS_ERROR_SAMPLE_NOT_FOUND: return "GPA_STATUS_ERROR_SAMPLE_NOT_FOUND";
	case	GPA_STATUS_ERROR_SAMPLE_NOT_FOUND_IN_ALL_PASSES: return "GPA_STATUS_ERROR_SAMPLE_NOT_FOUND_IN_ALL_PASSES";
	case	GPA_STATUS_ERROR_COUNTER_NOT_OF_SPECIFIED_TYPE: return "GPA_STATUS_ERROR_COUNTER_NOT_OF_SPECIFIED_TYPE";
	case	GPA_STATUS_ERROR_READING_COUNTER_RESULT: return "GPA_STATUS_ERROR_READING_COUNTER_RESULT";
	case	GPA_STATUS_ERROR_VARIABLE_NUMBER_OF_SAMPLES_IN_PASSES: return "GPA_STATUS_ERROR_VARIABLE_NUMBER_OF_SAMPLES_IN_PASSES";
	case	GPA_STATUS_ERROR_FAILED: return "GPA_STATUS_ERROR_FAILED";
	case	GPA_STATUS_ERROR_HARDWARE_NOT_SUPPORTED: return "GPA_STATUS_ERROR_HARDWARE_NOT_SUPPORTED";
	}
	return "GPA_UNKNOWN";
}

char * shader_editor::enumToStringGPA_UsageType( int val )
{
	switch(val)
	{	
	case	GPA_USAGE_TYPE_RATIO: return " ratio";
	case	GPA_USAGE_TYPE_PERCENTAGE: return "%";
	case	GPA_USAGE_TYPE_CYCLES: return " cyklu";
	case	GPA_USAGE_TYPE_MILLISECONDS: return "ms";
	case	GPA_USAGE_TYPE_BYTES: return "B";
	case	GPA_USAGE_TYPE_ITEMS: return " items";
	case	GPA_USAGE_TYPE_KILOBYTES: return "kB";
	case	GPA_USAGE_TYPE__LAST: return "last";
	}
	return "unknown_type";
}

char * shader_editor::enumToStringNV_UsageType( int val )
{
	switch(val)
	{	
	case	NVPM_CDH_PERCENT: return " %";
	case	NVPM_CDH_RAW: return "";
	case	NVPM_CDH_EXPRONLY: return "%";
	}
	return "unknown_type";
}

char * enumToStringNVP_RetVal(int val)
{
	switch(val)
	{	
		/// Performance disabled in registry
	case	NVPM_FAILURE_DISABLED: return "NVPM_FAILURE_DISABLED - Performance disabled in registry";
			/// Mixed mode (32bit client 64bit kernel) unsupported
	case	NVPM_FAILURE_32BIT_ON_64BIT: return "NVPM_FAILURE_32BIT_ON_64BIT - Mixed mode (32bit client 64bit kernel) unsupported";
			/// Returned when NVPMInit has not been called or failed
	case	NVPM_NO_IMPLEMENTATION: return "NVPM_NO_IMPLEMENTATION -Returned when NVPMInit has not been called or failed";
			/// nvpmapi.dll was not found
	case	NVPM_LIBRARY_NOT_FOUND: return "NVPM_LIBRARY_NOT_FOUND - nvpmapi.dll was not found";
			/// General, internal failure when initializing
	case	NVPM_FAILURE: return "NVPM_FAILURE - General, internal failure when initializings";
			/// Finished successfully
	case	NVPM_OK: return "NVPM_OK - Finished successfully";
			/// Invalid parameter found
	case	NVPM_ERROR_INVALID_PARAMETER: return "NVPM_ERROR_INVALID_PARAMETER - Invalid parameter found";
			/// Driver version mismatch ?
	case	NVPM_ERROR_DRIVER_MISMATCH: return "NVPM_ERROR_DRIVER_MISMATCH - Driver version mismatch ?";
			/// Not initialized when trying to use
	case	NVPM_ERROR_NOT_INITIALIZED: return "NVPM_ERROR_NOT_INITIALIZED - Not initialized when trying to use";
			/// Already initialized when trying to initialize
	case	NVPM_ERROR_ALREADY_INITIALIZED: return "NVPM_ERROR_ALREADY_INITIALIZED - Already initialized when trying to initialize";
			/// Bad enumerator found
	case	NVPM_ERROR_BAD_ENUMERATOR: return "NVPM_ERROR_BAD_ENUMERATOR - Bad enumerator found";
			/// String is too small
	case	NVPM_ERROR_STRING_TOO_SMALL: return "NVPM_ERROR_STRING_TOO_SMALL - String is too small";
			/// Invalid counter found
	case	NVPM_ERROR_INVALID_COUNTER: return "NVPM_ERROR_INVALID_COUNTER - Invalid counter found";
			/// No more memory to be allocated
	case	NVPM_ERROR_OUT_OF_MEMORY: return "NVPM_ERROR_OUT_OF_MEMORY - No more memory to be allocated";
	case	NVPM_ERROR_EXPERIMENT_INCOMPLETE: return "NVPM_ERROR_EXPERIMENT_INCOMPLETE";
	case	NVPM_ERROR_INVALID_PASS: return "NVPM_ERROR_INVALID_PASS";
	case	NVPM_ERROR_INVALID_OBJECT: return "NVPM_ERROR_INVALID_OBJECT";
	case	NVPM_ERROR_COUNTER_NOT_ENABLED: return "NVPM_ERROR_COUNTER_NOT_ENABLED";
	case	NVPM_ERROR_COUNTER_NOT_FOUND: return "NVPM_ERROR_COUNTER_NOT_FOUND";
	case	NVPM_ERROR_EXPERIMENT_NOT_RUN: return "NVPM_ERROR_EXPERIMENT_NOT_RUN";
	case	NVPM_ERROR_32BIT_ON_64BIT: return "NVPM_ERROR_32BIT_ON_64BIT";
	case	NVPM_ERROR_STATE_MACHINE: return "NVPM_ERROR_STATE_MACHINE";
	case	NVPM_ERROR_INTERNAL: return "NVPM_ERROR_INTERNAL";
	case	NVPM_WARNING_ENDED_EARLY: return "NVPM_WARNING_ENDED_EARLY";
	case	NVPN_ERROR_TIME_OUT: return "NVPN_ERROR_TIME_OUT";
	case	NVPM_WARNING_DUPLICATE: return "NVPM_WARNING_DUPLICATE";
	case	NVPM_ERROR_COUNTERS_ENABLED: return "NVPM_ERROR_COUNTERS_ENABLED";
	case	NVPM_WARNING_CONTEXT_NOT_SUPPORTED: return "NVPM_WARNING_CONTEXT_NOT_SUPPORTED";
	case	NVPM_ERROR_INVALID_CONTEXT: return "NVPM_ERROR_INVALID_CONTEXT";
	case	NVPM_ERROR_GPU_UNSUPPORTED: return "NVPM_OK";
	case	NVPM_ERROR_MAX: return "NVPM_ERROR_MAX";
	}
	return "unknown_type";
};
struct NVCounter
{
	std::string counterName;
	NVPMCounterID counterID;
	NVPMCounterID pureCounterID; //tmp
};

std::vector<NVCounter *> NVCounterList;
std::map<int, NVCounter *> rowToNVCounterList;
int bottleneckInfoRow;

void shader_editor::takePerfSnapshot_NV()
{
	if (!perfNV_setup)
	{
		//vypsat chybu
		return;
	}

	NVPMRESULT nvResult;
	NVPMRESULT nvResult_s;


	 
	nvResult = GetNvPmApi()->ReserveObjects(hNVPMContext, 1);
	if (nvResult!=NVPM_OK)
	{
		writeToConsoleQt(QtCriticalMsg,"Debug - nvResult = GetNvPmApi()->ReserveObjects(hNVPMContext, 1);");
		writeToConsoleQt(QtCriticalMsg,enumToStringNVP_RetVal(nvResult));
		return;
	}

	NVPMUINT punNumPasses=1;
	nvResult = GetNvPmApi()->BeginExperiment(hNVPMContext, &punNumPasses);
	if (nvResult!=NVPM_OK)
	{
		writeToConsoleQt(QtCriticalMsg,"Debug - nvResult = GetNvPmApi()->BeginExperiment(hNVPMContext, &punNumPasses);");
		writeToConsoleQt(QtCriticalMsg,enumToStringNVP_RetVal(nvResult));
		return;
	}

	NVPMRESULT nvResultArray[4];

	NVPMUINT sampleIntUnused;
	//GetNvPmApi()->Sample(hNVPMContext,NULL,&sampleIntUnused);

	NVPMSampleValue sampleArray[400];
	NVPMUINT sampleCount = 400;

	for (NVPMUINT i=0; i<punNumPasses;i++)
	{

		nvResultArray[0]= GetNvPmApi()->BeginPass(hNVPMContext,i);
		nvResultArray[1]=GetNvPmApi()->BeginObject(hNVPMContext,0);
		ui.osgGLWidget->forcePaintGL();
		//GetNvPmApi()->Sample(hNVPMContext,NULL,&sampleIntUnused);
		nvResult_s = GetNvPmApi()->Sample(hNVPMContext,sampleArray,&sampleCount);
		nvResultArray[2]=GetNvPmApi()->EndObject(hNVPMContext,0);
		nvResultArray[3]= GetNvPmApi()->EndPass(hNVPMContext, i);


	}
	for (unsigned i=0; i<sampleCount; i++)
	{
		if (NVApi_counterIDToTableRow.find(sampleArray[i].unCounterID)!=NVApi_counterIDToTableRow.end())
		{

			NVPMUINT64 counterValue = sampleArray[i].ulValue;
			NVPMUINT64 cycles = sampleArray[i].ulCycles;
			NVPMUINT64 displayType;


			QTableWidgetItem * this_item;
			QString this_item_string;//NVCounterList[it->first]->counterName;
			nvResult_s = GetNvPmApi()->GetCounterAttribute(sampleArray[i].unCounterID,NVPMA_COUNTER_VALUE,&displayType);
			if (displayType==NVPM_CDH_PERCENT && cycles)
			{
				double counterValueDouble_P1=counterValue;
				double counterValueDouble_P2=cycles;
				double counterValueDouble = 100.0 * (counterValueDouble_P1/counterValueDouble_P2);
				this_item_string = QString::number(counterValueDouble);
				this_item_string+=enumToStringNV_UsageType(displayType);
			}
			else if (displayType==NVPM_CDH_PERCENT)
			{
				this_item_string = QString::number(counterValue);
				this_item_string+=enumToStringNV_UsageType(displayType);
			}
			else
				this_item_string = QString::number(counterValue);
			this_item = new QTableWidgetItem(this_item_string);
			this_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			this_item->setToolTip("Skutecna hodnota");
			ui.GPA_counter_list->setItem(NVApi_counterIDToTableRow[sampleArray[i].unCounterID],GPAQTT_VALUE, this_item);
		}
	}
	nvResult = GetNvPmApi()->EndExperiment(hNVPMContext);
	if (nvResult!=NVPM_OK)
	{
		writeToConsoleQt(QtCriticalMsg,"Debug - nvResult = GetNvPmApi()->EndExperiment(hNVPMContext);");
		writeToConsoleQt(QtCriticalMsg,enumToStringNVP_RetVal(nvResult));
		return;
	}

/*
	if ( 0 )
	{
		//GetNvPmApi()->GetCounterValue(hNVPMContext,)



		for (std::map<int, int>::iterator it = perfUniversal_to_table.begin(); it!=perfUniversal_to_table.end();it++)
		{
			int row = it->second;
			NVPMCounterID counterID = NVApi_TableRowToCounterID[row];
			NVPMUINT64 counterValue = 8;
			NVPMUINT64 pureCounterValue = 8;
			NVPMUINT64 cycles = 0;
			NVPMUINT64 pureCycles = 0;
			NVPMUINT64 displayType;
			NVPMUINT64 pureDisplayType;
			nvResult = GetNvPmApi()->GetCounterValueByName(hNVPMContext,NVCounterList[it->first]->counterName.c_str(),0,&counterValue,&cycles);
			//nvResult = GetNvPmApi()->GetCounterValue(hNVPMContext,NVCounterList[it->first]->counterID,0,&counterValue,&cycles);
			nvResult_s = GetNvPmApi()->GetCounterAttribute(NVCounterList[it->first]->counterID,NVPMA_COUNTER_VALUE,&displayType);
			if (nvResult != NVPM_OK || nvResult_s != NVPM_OK)
			{
				writeToConsoleQt(QtCriticalMsg,"Debug - nvResult = GetNvPmApi()->GetCounterValueByName(hNVPMContext,ui.GPA_counter_list->item(row,GPAQTT_DESCRIPTION)->text().toStdString().c_str(),0,&counterValue,&cycles);");
				writeToConsoleQt(QtCriticalMsg,"Debug - nvResult_s = GetNvPmApi()->GetCounterAttribute(NVCounterList[it->first]->counterID,NVPMA_COUNTER_DISPLAY_HINT,&displayType);");
				writeToConsoleQt(QtCriticalMsg,enumToStringNVP_RetVal(nvResult));
				writeToConsoleQt(QtCriticalMsg,enumToStringNVP_RetVal(nvResult_s));
				writeToConsoleQt(QtCriticalMsg,ui.GPA_counter_list->item(row,GPAQTT_DESCRIPTION)->text().toStdString().c_str());
				return;
			}

			QTableWidgetItem * this_item;
			QString this_item_string;//NVCounterList[it->first]->counterName;
			if (displayType==NVPM_CDH_PERCENT && cycles)
			{
				double counterValueDouble_P1=counterValue;
				double counterValueDouble_P2=cycles;
				double counterValueDouble = 100.0 * (counterValueDouble_P1/counterValueDouble_P2);
				this_item_string = QString::number(counterValueDouble);
				this_item_string+=enumToStringNV_UsageType(displayType);
			}
			else if (displayType==NVPM_CDH_PERCENT)
			{
				this_item_string = QString::number(counterValue);
				this_item_string+=enumToStringNV_UsageType(displayType);
			}
			else
				this_item_string = QString::number(counterValue);
// 			this_item_string += "   ";
// 			if (pureDisplayType==NVPM_CDH_PERCENT && pureCycles)
// 			{
// 				double counterValueDouble_P1=pureCounterValue;
// 				double counterValueDouble_P2=pureCycles;
// 				double counterValueDouble = counterValueDouble_P1/counterValueDouble_P2;
// 				this_item_string += QString::number(counterValueDouble);
// 			}
// 			else
// 				this_item_string += QString::number(pureCounterValue);
// 			this_item_string+=" ";
// 			this_item_string += QString::number(cycles);
/ *			this_item_string+=enumToStringNV_UsageType(displayType);* /
// 			if (it->second == bottleneckInfoRow)
// 			{
// 				char bottleneckName[200];
// 				nvResult = GetNvPmApi()->GetGPUBottleneckName(hNVPMContext,counterValue,bottleneckName);
// 				this_item_string = bottleneckName;
// 			}
			if (1/ *cycles* /)
			{
				this_item = new QTableWidgetItem(this_item_string);
				this_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
				this_item->setToolTip("Skutecna hodnota");
				ui.GPA_counter_list->setItem(row,GPAQTT_VALUE, this_item);
			}


		}
	}*/

	GetNvPmApi()->DeleteObjects(hNVPMContext);
/*

	nvResult=GetNvPmApi()->RemoveAllCounters(hNVPMContext);
	if (nvResult!=NVPM_OK)
	{
		writeToConsoleQt(QtCriticalMsg,"Debug - nvResult=GetNvPmApi()->RemoveAllCounters(hNVPMContext);;");
		writeToConsoleQt(QtCriticalMsg,enumToStringNVP_RetVal(nvResult));
		return;
	}*/
}

bool NVAPI_inicialized;

void shader_editor::initNVAPI()
{
	NVAPI_inicialized = false;
	HRESULT retval;
	if((retval =GetNvPmApiManager()->Construct(L"Bin\\nv_x86\\NvPmApi.Core.dll")) != S_OK)
	{
		writeToConsoleQt(QtCriticalMsg,"Debug - došlo k selhání GetNvPmApiManager()->Construct(L\"Bin\\nv_x86\\NvPmApi.Core.dll\") != S_OK");
		writeToConsoleQt(QtCriticalMsg,QString::number(retval).toStdString().c_str());
		return; // This is an error condition
	}
	else
	{
		NVPMRESULT nvResult;
		if((nvResult = GetNvPmApi()->Init()) != NVPM_OK)
		{
			writeToConsoleQt(QtCriticalMsg,"Debug - došlo k selhání nvResult = GetNvPmApi()->Init()) != NVPM_OK");
		}

	}
	NVAPI_inicialized= true;
}



int getPerfCounters_NV(NVPMCounterID unCounterID, const char *pcCounterName)
{
	NVPMUINT64 displayType;
	GetNvPmApi()->GetCounterAttribute(unCounterID,NVPMA_COUNTER_TYPE,&displayType);
	if (displayType == NVPM_CT_D3D)
	{
		return NVPM_OK;
	}
	NVCounter * counterRecord = new NVCounter;
	counterRecord->counterID = unCounterID;
	counterRecord->counterName = pcCounterName;
	NVCounterList.push_back(counterRecord);
	return NVPM_OK;
}

//typedef NVPMRESULT (*NVPMEnumFunc)(NVPMCounterID unCounterID, char *pcCounterName);

void shader_editor::initPerfCounters_NV()
{
	if (!NVAPI_inicialized)
	{
		return;
	}
	mainClass_Pointer = this;
	NVPMRESULT nvResult;

	if (!perfNV_initialized)
	{
		//NVPMRESULT nvResult;		
		if((nvResult = GetNvPmApi()->CreateContextFromOGLContext((APIContextHandle)(ui.osgGLWidget->getGLContext()),&hNVPMContext)) != NVPM_OK)
		{
			writeToConsoleQt(QtCriticalMsg,"Debug - došlo k selhání nvResult = GetNvPmApi()->CreateContextFromOGLContext((APIContextHandle)(ui.osgGLWidget->getGLContext()),&hNVPMContext)");
			return;
		}
		perfNV_initialized = true;
	}

	//nvResult = GetNvPmApi()->GetNumCountersByContext(hNVPMContext, UINT *punCount);

	NVPMEnumFunc getCounterCallback = &getPerfCounters_NV;
	nvResult = GetNvPmApi()->EnumCountersByContext(hNVPMContext, getCounterCallback);
	if (nvResult!=NVPM_OK)
	{
		writeToConsoleQt(QtCriticalMsg,"Debug - nvResult = GetNvPmApi()->EnumCountersByContext(hNVPMContext, getCounterCallback);");
		writeToConsoleQt(QtCriticalMsg,enumToStringNVP_RetVal(nvResult));
		writeToConsoleQt(QtCriticalMsg,QString::number(hNVPMContext));
		writeToConsoleQt(QtCriticalMsg,"Selhal proces zjistovani dostupnych citacu, aplikaci je treba restartovat"); 
		//*((int *)0) = 0;
		return;
	}

	

	int counter_count = NVCounterList.size();

	setup_hiearchy(gpuVendor);
	ui.GPA_counter_list->setRowCount(counter_count+perf_dv_groups->size());
	ui.GPA_counter_list->setColumnCount(2);
	ui.GPA_counter_list->setHorizontalHeaderItem(GPAQTT_DESCRIPTION,new QTableWidgetItem("Counters"));
	ui.GPA_counter_list->setHorizontalHeaderItem(GPAQTT_VALUE,new QTableWidgetItem("Value"));
	//ui.GPA_counter_list->setHorizontalHeaderItem(GPAQTT_PURE_VALUE,new QTableWidgetItem("Pure Value"));

	QTableWidgetItem * this_item;
	//std::vector<std::string> counter_name_list;
	std::vector<std::string> counter_desc_list;
	std::vector<bool>		counter_name_done;
	//std::map<std::string,> hiearchy;
	NVPMUINT length=2000;
	char * descString = new char[length];
	for(unsigned i = 0; i < counter_count; i++)
	{
		//counter_name_list.push_back(NVCounterList[i]->counterName);

		nvResult = GetNvPmApi()->GetCounterDescription(NVCounterList[i]->counterID, descString, &length);
		if (nvResult!=NVPM_OK)
		{
			writeToConsoleQt(QtCriticalMsg,"Debug - chyba konverze èítaèù - nvResult = GetNvPmApi()->GetCounterDescription(NVCounterList[i]->counterID, descString, &length)");
			writeToConsoleQt(QtCriticalMsg,enumToStringNVP_RetVal(nvResult));
			std::string retval = "ID counter: ";
			retval += NVCounterList[i]->counterID;
			retval += "; Jmeno counteru: ";
			retval += NVCounterList[i]->counterName;
			retval += "\n";
			writeToConsoleQt(QtCriticalMsg,retval.c_str());
			return;
		}

		counter_desc_list.push_back(std::string(descString));
		counter_name_done.push_back(false);
	}
	delete descString;
	int this_row=0;
	std::string this_group;

	QBrush major_background(QColor(191,255,191));

	{
		const char * this_item_group;

		for (std::vector<std::string>::iterator it=perf_dv_groups->begin(); it!=perf_dv_groups->end(); it++)
		{
			this_group = *it;
			this_item = new QTableWidgetItem(it->c_str());
			this_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			//this_item->setToolTip(Qt::convertFromPlainText(description));
			this_item->setBackground(major_background);
			ui.GPA_counter_list->setItem(this_row, GPAQTT_DESCRIPTION, this_item);
			this_item = new QTableWidgetItem();
			this_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			//this_item->setToolTip(Qt::convertFromPlainText(description));
			this_item->setBackground(major_background);
			ui.GPA_counter_list->setItem(this_row, GPAQTT_VALUE, this_item);
			perf_dv_group_start->push_back(this_row);
			perfUniversal_table_colapsed.push_back(false);
			this_row++;

			for(int i=0; i<NVCounterList.size(); i++)
			{

				if(counter_name_done[i] == false)
				{
					//this_item_group = counter_name_list[i];
					//this_item_group = perf_dv_hiearchy[counter_name_list[i]];
					if (perf_dv_hiearchy[NVCounterList[i]->counterName] == this_group)
					{
						counter_name_done[i]=true;
						this_item = new QTableWidgetItem(NVCounterList[i]->counterName.c_str());
						this_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
						if (NV_CounterTooltips.find(NVCounterList[i]->counterName)!=NV_CounterTooltips.end())
							this_item->setToolTip(NV_CounterTooltips[NVCounterList[i]->counterName].c_str());
						else
							this_item->setToolTip(QString(counter_desc_list[i].c_str()));
						ui.GPA_counter_list->setItem(this_row, GPAQTT_DESCRIPTION, this_item);
						perfUniversal_to_table[i] = this_row;
						NVApi_TableRowToCounterID[this_row] = NVCounterList[i]->counterID;
						NVApi_counterIDToTableRow[NVCounterList[i]->counterID] = this_row;
						rowToNVCounterList[this_row] = NVCounterList[i];
						this_row++;
					}
					else if (std::string(getCustomGroup(NVCounterList[i]->counterName.c_str()))==this_group)
					{
						counter_name_done[i]=true;
						this_item = new QTableWidgetItem(NVCounterList[i]->counterName.c_str());
						this_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
						if (NV_CounterTooltips.find(NVCounterList[i]->counterName)!=NV_CounterTooltips.end())
							this_item->setToolTip(NV_CounterTooltips[NVCounterList[i]->counterName].c_str());
						else
							this_item->setToolTip(QString(counter_desc_list[i].c_str()));
						ui.GPA_counter_list->setItem(this_row, GPAQTT_DESCRIPTION, this_item);
						perfUniversal_to_table[i] = this_row;
						NVApi_TableRowToCounterID[this_row] = NVCounterList[i]->counterID;
						NVApi_counterIDToTableRow[NVCounterList[i]->counterID] = this_row;
						rowToNVCounterList[this_row] = NVCounterList[i];
						this_row++;
					}
				}
			}
		}
		bool unrecognized=false;
		std::vector<int> unrecognized_list;
		for (int i=0; i<counter_name_done.size();i++)
		{
			if(counter_name_done[i] == false)
			{
				unrecognized=true;
				unrecognized_list.push_back(i);
			}
		}
		if (unrecognized)
		{
			this_item = new QTableWidgetItem("unrecognized");
			this_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			this_item->setBackground(major_background);
			ui.GPA_counter_list->setItem(this_row, GPAQTT_DESCRIPTION, this_item);

			this_item = new QTableWidgetItem();
			this_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			this_item->setBackground(major_background);
			ui.GPA_counter_list->setItem(this_row, GPAQTT_VALUE, this_item);

			perf_dv_group_start->push_back(this_row);
			perfUniversal_table_colapsed.push_back(false);
			this_row++;
			for (int i=0;i<unrecognized_list.size();i++)
			{
				this_item = new QTableWidgetItem(NVCounterList[unrecognized_list[i]]->counterName.c_str());//NVCounterList[i]->counterName
				this_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
				if (NV_CounterTooltips.find(NVCounterList[unrecognized_list[i]]->counterName)!=NV_CounterTooltips.end())
					this_item->setToolTip(NV_CounterTooltips[NVCounterList[unrecognized_list[i]]->counterName].c_str());
				else
					this_item->setToolTip(QString(counter_desc_list[unrecognized_list[i]].c_str()));
				ui.GPA_counter_list->setItem(this_row, GPAQTT_DESCRIPTION, this_item);
				perfUniversal_to_table[unrecognized_list[i]] = this_row;
				NVApi_TableRowToCounterID[this_row] = NVCounterList[unrecognized_list[i]]->counterID;
				NVApi_counterIDToTableRow[NVCounterList[unrecognized_list[i]]->counterID] = this_row;
				rowToNVCounterList[this_row] = NVCounterList[unrecognized_list[i]];
				this_row++;
			}
		}
	}
	perf_dv_group_start->push_back(this_row);
	perfUniversal_table_colapsed.push_back(false);

// 		this_item = new QTableWidgetItem(name);
// 		this_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
// 		this_item->setToolTip(Qt::convertFromPlainText(description));
// 		ui.GPA_counter_list->setItem(i,GPAQTT_DESCRIPTION, this_item);


	//retval=GPA_EnableAllCounters();
	perfNV_setup = true;

	bottleneckInfoRow = -1;
	for (std::map<int, NVCounter *>::iterator it = rowToNVCounterList.begin();it!=rowToNVCounterList.end();it++)
	{
		if (it->second->counterName == "GPU Bottleneck")
		{
			bottleneckInfoRow = it->first;
		}
	}

	for (int i = 0; i<NVCounterList.size() ;i++)
	{
		NVPMCounterID counterID;
		if (GetNvPmApi()->GetCounterIDByContext(hNVPMContext, (char *)(NVCounterList[i]->counterName.c_str()), &counterID) != NVPM_OK)
		{
			return;
		}
		NVCounterList[i]->pureCounterID = NVCounterList[i]->counterID;
		NVCounterList[i]->counterID = counterID;
	}

	for (int i=0; i<NVCounterList.size();i++)
	{
		nvResult=GetNvPmApi()->AddCounterByName(hNVPMContext, NVCounterList[i]->counterName.c_str());
		//nvResult=GetNvPmApi()->AddCounter(hNVPMContext, NVCounterList[i]->counterID);
		if (nvResult!=NVPM_OK)
		{
			writeToConsoleQt(QtCriticalMsg,"Debug - nvResult = GetNvPmApi()->AddCounterByName(hNVPMContext, NVCounterList[i]->counterName.c_str());");
			writeToConsoleQt(QtCriticalMsg,enumToStringNVP_RetVal(nvResult));
			writeToConsoleQt(QtCriticalMsg,NVCounterList[i]->counterName.c_str());
			return;
		}
	}
	//GetCounterIDByContext


}

void shader_editor::hidePerfRow_NV( int row, int column )
{
	int group_start_position=0;
	int group_end_position;
	for(std::vector<int>::iterator it = perf_dv_group_start->begin(); it!=perf_dv_group_start->end(); it++, group_start_position++)
	{
		if (*it==row)
		{
			group_end_position = (*perf_dv_group_start)[group_start_position+1];
			if (perfUniversal_table_colapsed[group_start_position])
			{
				perfUniversal_table_colapsed[group_start_position]=false;
				for (int st=(*perf_dv_group_start)[group_start_position]+1; st<group_end_position; st++)
					ui.GPA_counter_list->showRow(st);
			}
			else
			{
				perfUniversal_table_colapsed[group_start_position]=true;
				for (int st=(*perf_dv_group_start)[group_start_position]+1; st<group_end_position; st++)
					ui.GPA_counter_list->hideRow(st);
			}
			return;
		}
	}
}





/*
void shader_editor::takePerfSnapshot_NV()
{
	if (!perfNV_setup)
	{
		//vypsat chybu
		return;
	}

	NVPMRESULT nvResult;
	NVPMRESULT nvResult_s;

	for (int i=0; i<NVCounterList.size();i++)
	{
		nvResult=GetNvPmApi()->AddCounterByName(hNVPMContext, NVCounterList[i]->counterName.c_str());
		//nvResult=GetNvPmApi()->AddCounter(hNVPMContext, NVCounterList[i]->counterID);
		if (nvResult!=NVPM_OK)
		{
			writeToConsoleQt(QtCriticalMsg,"Debug - nvResult = GetNvPmApi()->AddCounterByName(hNVPMContext, NVCounterList[i]->counterName.c_str());");
			writeToConsoleQt(QtCriticalMsg,enumToStringNVP_RetVal(nvResult));
			writeToConsoleQt(QtCriticalMsg,NVCounterList[i]->counterName.c_str());
			return;
		}
	}

	nvResult = GetNvPmApi()->ReserveObjects(hNVPMContext, 1);
	if (nvResult!=NVPM_OK)
	{
		writeToConsoleQt(QtCriticalMsg,"Debug - nvResult = GetNvPmApi()->ReserveObjects(hNVPMContext, 1);");
		writeToConsoleQt(QtCriticalMsg,enumToStringNVP_RetVal(nvResult));
		return;
	}

	NVPMUINT punNumPasses=1;
	nvResult = GetNvPmApi()->BeginExperiment(hNVPMContext, &punNumPasses);
	if (nvResult!=NVPM_OK)
	{
		writeToConsoleQt(QtCriticalMsg,"Debug - nvResult = GetNvPmApi()->BeginExperiment(hNVPMContext, &punNumPasses);");
		writeToConsoleQt(QtCriticalMsg,enumToStringNVP_RetVal(nvResult));
		return;
	}

	NVPMRESULT nvResultArray[4];

	NVPMUINT sampleIntUnused;
	//GetNvPmApi()->Sample(hNVPMContext,NULL,&sampleIntUnused);

	NVPMUINT sampleCount = 100;
	NVPMSampleValue sampleArray[100];

	for (NVPMUINT i=0; i<punNumPasses;i++)
	{
		nvResultArray[0]= GetNvPmApi()->BeginPass(hNVPMContext,i);
		nvResultArray[1]=GetNvPmApi()->BeginObject(hNVPMContext,0);
		ui.osgGLWidget->forcePaintGL();
		//GetNvPmApi()->Sample(hNVPMContext,NULL,&sampleIntUnused);
		nvResultArray[2]=GetNvPmApi()->EndObject(hNVPMContext,0);
		nvResultArray[3]= GetNvPmApi()->EndPass(hNVPMContext, i);
		GetNvPmApi()->Sample(hNVPMContext,NULL,&sampleIntUnused);
	}
	// 	for(int i=0; i<4; i++)
	// 	{
	// 		if (nvResultArray[i]!=NVPM_OK)
	// 		{
	// 			QString errorMsg = "nvResultArray[";
	// 			errorMsg += QString::number(i);
	// 			errorMsg += "] = ";
	// 			errorMsg += enumToStringNVP_RetVal(nvResultArray[i]);
	// 			writeToConsoleQt(QtCriticalMsg,errorMsg.toStdString().c_str());
	// 			return;
	// 		}
	// 	}

	nvResult = GetNvPmApi()->EndExperiment(hNVPMContext);
	if (nvResult!=NVPM_OK)
	{
		writeToConsoleQt(QtCriticalMsg,"Debug - nvResult = GetNvPmApi()->EndExperiment(hNVPMContext);");
		writeToConsoleQt(QtCriticalMsg,enumToStringNVP_RetVal(nvResult));
		return;
	}

	/ *	if ( 0 )* /
	{
		//GetNvPmApi()->GetCounterValue(hNVPMContext,)



		for (std::map<int, int>::iterator it = perfUniversal_to_table.begin(); it!=perfUniversal_to_table.end();it++)
		{
			int row = it->second;
			NVPMCounterID counterID = NVApi_TableRowToCounterID[row];
			NVPMUINT64 counterValue = 8;
			NVPMUINT64 pureCounterValue = 8;
			NVPMUINT64 cycles = 0;
			NVPMUINT64 pureCycles = 0;
			NVPMUINT64 displayType;
			NVPMUINT64 pureDisplayType;
			nvResult = GetNvPmApi()->GetCounterValueByName(hNVPMContext,NVCounterList[it->first]->counterName.c_str(),0,&counterValue,&cycles);
			//nvResult = GetNvPmApi()->GetCounterValue(hNVPMContext,NVCounterList[it->first]->counterID,0,&counterValue,&cycles);
			nvResult_s = GetNvPmApi()->GetCounterAttribute(NVCounterList[it->first]->counterID,NVPMA_COUNTER_VALUE,&displayType);
			if (nvResult != NVPM_OK || nvResult_s != NVPM_OK)
			{
				writeToConsoleQt(QtCriticalMsg,"Debug - nvResult = GetNvPmApi()->GetCounterValueByName(hNVPMContext,ui.GPA_counter_list->item(row,GPAQTT_DESCRIPTION)->text().toStdString().c_str(),0,&counterValue,&cycles);");
				writeToConsoleQt(QtCriticalMsg,"Debug - nvResult_s = GetNvPmApi()->GetCounterAttribute(NVCounterList[it->first]->counterID,NVPMA_COUNTER_DISPLAY_HINT,&displayType);");
				writeToConsoleQt(QtCriticalMsg,enumToStringNVP_RetVal(nvResult));
				writeToConsoleQt(QtCriticalMsg,enumToStringNVP_RetVal(nvResult_s));
				writeToConsoleQt(QtCriticalMsg,ui.GPA_counter_list->item(row,GPAQTT_DESCRIPTION)->text().toStdString().c_str());
				return;
			}

			QTableWidgetItem * this_item;
			QString this_item_string;//NVCounterList[it->first]->counterName;
			if (displayType==NVPM_CDH_PERCENT && cycles)
			{
				double counterValueDouble_P1=counterValue;
				double counterValueDouble_P2=cycles;
				double counterValueDouble = 100.0 * (counterValueDouble_P1/counterValueDouble_P2);
				this_item_string = QString::number(counterValueDouble);
				this_item_string+=enumToStringNV_UsageType(displayType);
			}
			else if (displayType==NVPM_CDH_PERCENT)
			{
				this_item_string = QString::number(counterValue);
				this_item_string+=enumToStringNV_UsageType(displayType);
			}
			else
				this_item_string = QString::number(counterValue);
			// 			this_item_string += "   ";
			// 			if (pureDisplayType==NVPM_CDH_PERCENT && pureCycles)
			// 			{
			// 				double counterValueDouble_P1=pureCounterValue;
			// 				double counterValueDouble_P2=pureCycles;
			// 				double counterValueDouble = counterValueDouble_P1/counterValueDouble_P2;
			// 				this_item_string += QString::number(counterValueDouble);
			// 			}
			// 			else
			// 				this_item_string += QString::number(pureCounterValue);
			// 			this_item_string+=" ";
			// 			this_item_string += QString::number(cycles);
			/ *			this_item_string+=enumToStringNV_UsageType(displayType);* /
			// 			if (it->second == bottleneckInfoRow)
			// 			{
			// 				char bottleneckName[200];
			// 				nvResult = GetNvPmApi()->GetGPUBottleneckName(hNVPMContext,counterValue,bottleneckName);
			// 				this_item_string = bottleneckName;
			// 			}
			if (1/ *cycles* /)
			{
				this_item = new QTableWidgetItem(this_item_string);
				this_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
				this_item->setToolTip("Skutecna hodnota");
				ui.GPA_counter_list->setItem(row,GPAQTT_VALUE, this_item);
			}


		}
	}

	GetNvPmApi()->DeleteObjects(hNVPMContext);

	nvResult=GetNvPmApi()->RemoveAllCounters(hNVPMContext);
	if (nvResult!=NVPM_OK)
	{
		writeToConsoleQt(QtCriticalMsg,"Debug - nvResult=GetNvPmApi()->RemoveAllCounters(hNVPMContext);;");
		writeToConsoleQt(QtCriticalMsg,enumToStringNVP_RetVal(nvResult));
		return;
	}
}
*/
