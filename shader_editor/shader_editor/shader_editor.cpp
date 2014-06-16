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
*/

#include "shader_editor.h"
#include <iostream>
#include <sstream>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileNameUtils>
#include "osgUniversalTableWidget.h"
#include "SceneTreeManager.h"
#include "StateAttributeManager.h"
#include <QShortcut>
#include <osgUtil/Optimizer>
#include <osgUtil/MeshOptimizers>

#include <QTime>
#include <QSignalMapper>
#include <QFileDialog>
#include <QMessageBox>
#include <Windows.h>
#include <QFile>
#include "osgNodeTreeItem.h"
#include <QDebug>
#include "shaderTab.h"
#include <osgdb/Options>

#define CHECK(EXPR) \
	if(EXPR == false)\
		lineCallFailure(__LINE__, __FILE__, #EXPR);


shader_editor::shader_editor(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{	
	this->osg_debug_output = new std::ofstream("defaultOSGDebug.txt");
	ui.setupUi(this);

	ui.tabWidget->setCurrentIndex(0);

	cuwContainer = new QWidget(NULL);
	cuwContainer->setWindowModality(Qt::ApplicationModal);
	cuw.setupUi(cuwContainer);

	sceneTreeContainer = new SceneTreeManager(this);
	stateAttributeContainer = new StateAttributeManager(this);

	bool retbool[50];
	notifyInstance = new osgNotifyRedirect;

	ui.showShaderProgramField->hide();

	shaderTabContainer = new shaderTab(this);


	CHECK(connect(ui.actionLoadObject,SIGNAL(triggered()),this,SLOT(loadModel())))
	CHECK(connect(ui.actionLoadObjectOptimalizer,SIGNAL(triggered()),this,SLOT(loadModelOptimalizer())));
	CHECK(connect(ui.actionSaveObject,SIGNAL(triggered()),this,SLOT(saveMainSceneNode())));
	CHECK(connect(ui.osgGLWidget,SIGNAL(firstFrameDrawn()),this,SLOT(rebuildSceneTree())));
	CHECK(connect(ui.sceneTreeWidget,SIGNAL(sceneGraphEmptied()),ui.osgGLWidget,SLOT(sceneEmptied())));
	CHECK(connect(ui.sceneTreeWidget,SIGNAL(sceneGraphSet(osg::Node *)),ui.osgGLWidget,SLOT(sceneSet(osg::Node *))));
	
	CHECK(connect(ui.textEdit_VS, SIGNAL(textChanged()),this,SLOT(QTE_vertexModified())));
	CHECK(connect(ui.textEdit_TCS, SIGNAL(textChanged()),this,SLOT(QTE_tessControlModified())));
	CHECK(connect(ui.textEdit_TES, SIGNAL(textChanged()),this,SLOT(QTE_tessEvallModified())));
	CHECK(connect(ui.textEdit_GS, SIGNAL(textChanged()),this,SLOT(QTE_geometryModified())));
	CHECK(connect(ui.textEdit_FS, SIGNAL(textChanged()),this,SLOT(QTE_fragmentModified())));
	CHECK(connect(notifyInstance, SIGNAL(writeToConsoleSignal(osg::NotifySeverity , const char *)), 
	this, SLOT(writeToConsole(osg::NotifySeverity , const char *))));
	CHECK(connect(ui.actionCompileShader, SIGNAL(triggered()),this,SLOT(compileShaders())));
	CHECK(connect(ui.actionExampleTeselace, SIGNAL(triggered()),this,SLOT(loadExampleTeselace())));
	CHECK(connect(ui.actionShowSceneTree, SIGNAL(triggered()),this,SLOT(displayDockSceneTree())));
	CHECK(connect(ui.actionPerfCounters, SIGNAL(triggered()),this,SLOT(displayDockPerf())));
	CHECK(connect(ui.actionTextOutput, SIGNAL(triggered()),this,SLOT(displayDockConsole())));
	CHECK(connect(ui.actionClearScene, SIGNAL(triggered()),this,SLOT(clearScene())));
	CHECK(connect(sceneTreeContainer, SIGNAL(nodeTreeItemActivated (QTreeWidgetItem * )), this, SLOT(rebuildUniformsTable(QTreeWidgetItem *))));
	//CHECK(connect(sceneTreeContainer, SIGNAL(nodeTreeItemActivated (QTreeWidgetItem * )), this, SLOT(setCurrentSceneGraphWidgetItem ( QTreeWidgetItem * ))));
	CHECK(connect(ui.stateSetTableWidget,SIGNAL(cellClicked ( int , int  )),stateAttributeContainer,SLOT(rebuildAttributesTable_StateSet( int , int  ))));
	CHECK(connect(ui.stateSetTableWidget,SIGNAL(cellClicked ( int , int  )),this,SLOT(rebuildUniformsTable_fromStateSetTable( int , int  ))));
	CHECK(connect(sceneTreeContainer,SIGNAL(nodeTreeItemActivated (QTreeWidgetItem * )),stateAttributeContainer,SLOT(rebuildAttributesTable_Node(QTreeWidgetItem *))));
	CHECK(connect(ui.uniformsTableWidget,SIGNAL(cellChanged (int, int )),this,SLOT(uniformTableValueChanged(int))));	
	CHECK(connect(ui.button_dockWidgetUniform_add,SIGNAL(pressed ()),this,SLOT(uniformTableUniformAdd())));
	CHECK(connect(ui.button_dockWidgetUniform_remove,SIGNAL(pressed ()),this,SLOT(uniformTableUniformRemove()))); 
	CHECK(connect(cuw.uniformCreate,SIGNAL(pressed ()),this,SLOT(uniformTableUniformAdd_cuwCreate()))); //uniformTableUniformAdd_cuwSetProtovalue
	CHECK(connect(cuw.uniformList,SIGNAL(itemSelectionChanged  ()),this,SLOT(uniformTableUniformAdd_cuwSetProtovalue()))); //uniformTableUniformAdd_cuwSetProtovalue
	CHECK(connect(ui.stateSetTableWidget_Add,SIGNAL(pressed ()),this,SLOT(statesetTableCreateNew()))); 
	CHECK(connect(ui.treeWidgetSetStateset,SIGNAL(pressed ()),this,SLOT(treeWidgetSetStateSet()))); 
	CHECK(connect(ui.treeWidgetAddNode,SIGNAL(pressed ()),this,SLOT(treeWidgetAddNode()))); 
	//retbool[28] = connect(ui.treeWidgetDelete,SIGNAL(pressed ()),this,SLOT(deleteCurrentNode())); 
	CHECK(connect(ui.actionTest_SceneTreeWidgetu,SIGNAL(triggered ()),this,SLOT(loadExampleSceneTreeTest()))); //actionTest_SceneTreeWidgetu
	CHECK(connect(ui.actionSimpleDisplacement,SIGNAL(triggered ()),this,SLOT(loadExampleSimpleDisplacement())));
	CHECK(connect(ui.actionSimpleDisplacement_v2,SIGNAL(triggered ()),this,SLOT(loadExampleSimpleDisplacement_complexModel())));
	CHECK(connect(ui.actionSimpleDisplacement_v2_difTes,SIGNAL(triggered ()),this,SLOT(loadExampleSimpleDisplacement_complexModel_differentTesselation())));
	CHECK(connect(ui.actionSimpleDisplacement_v2_normalMapping,SIGNAL(triggered ()),this,SLOT(loadExampleSimpleDisplacement_normalMapping())));
	CHECK(connect(ui.actionSimpleDisplacement_v2_normalMapping_v2,SIGNAL(triggered ()),this,SLOT(loadExampleSimpleDisplacement_normalMapping_2())));
	CHECK(connect(ui.actionLargeSceneWithSkybox,SIGNAL(triggered ()),this,SLOT(loadExampleLargeSkyboxScene())));
	CHECK(connect(ui.actionPromenlivaTeselace, SIGNAL(triggered()),this,SLOT(loadExampleVariableTesselation())));
	CHECK(connect(ui.timerInterval, SIGNAL(valueChanged(double)),this,SLOT(setTimerInterval(double))));
	CHECK(connect(ui.perfApi_TimedAnalysis, SIGNAL(toggled(bool)),this,SLOT(restartTimer(bool))));
	CHECK(connect(ui.perfCounters_MassAnalyze, SIGNAL(pressed()),this,SLOT(massPerfCountersAnalyse())));
	CHECK(connect(ui.sceneTreeWidget, SIGNAL(sceneGraphModified()),ui.osgGLWidget,SLOT(forcePaintGL())));
	CHECK(connect(ui.SA_Program_UseCurrentProgram, SIGNAL(pressed()),this,SLOT(SA_Program_SetCurrentProgram())));
	CHECK(connect(ui.actionFloatGLWidget,SIGNAL(toggled (bool)),this,SLOT(makeGLWidgetFloating(bool))));
	CHECK(connect(ui.actionAsteroid, SIGNAL(triggered ()), this, SLOT(loadExampleSimpleDisplacement_complexModel ())));
	CHECK(connect(ui.actionAsteroid_V2, SIGNAL(triggered ()), this, SLOT(loadExampleAsteroid_fromIVE ())));
	CHECK(connect(ui.actionAsteroid_Normal_Mapping_2, SIGNAL(triggered ()), this, SLOT(loadExampleSimpleDisplacement_complexModel_normalMapping ())));
	CHECK(connect(ui.actionAsteroid_Normal_Mapping_Error, SIGNAL(triggered ()), this, SLOT(loadExampleSimpleDisplacement_complexModel_normalMapping_normalMapProblem ())));
//	retbool[49] = connect(ui.actionAsteroid_sewing, SIGNAL(triggered ()), this, SLOT(loadExampleSimpleDisplacement_complexModel_normalMapping_sewing ()));
//	retbool[49] = connect(ui.actionAsteroid_Dynamic, SIGNAL(triggered ()), this, SLOT(loadExampleSimpleDisplacement_complexModel_normalMapping_dynamicTessellation ()));
	CHECK(connect(ui.actionAsteroid_LOD, SIGNAL(triggered ()), this, SLOT(loadExampleAsteroid_BasicLod ())));
	CHECK(connect(ui.actionAsteroid_HighlightGaps_Blender, SIGNAL(triggered()), this, SLOT(loadExample_Issues_Gaps_Blender())));
	CHECK(connect(ui.actionAsteroid_HighlightGaps_MeshMapper, SIGNAL(triggered()), this, SLOT(loadExample_Issues_Gaps_MeshMapper())));
	CHECK(connect(ui.actionAsteroid_Sewing, SIGNAL(triggered()), this, SLOT(loadExample_Issues_Sewing())));
	CHECK(connect(ui.actionAsteroid_UVGapsCorrection_Blender, SIGNAL(triggered()), this, SLOT(loadExample_Issues_UVs_Blender())));
	CHECK(connect(ui.actionAsteroid_UVGapsCorrection_MeshMapper, SIGNAL(triggered()), this, SLOT(loadExample_Issues_UVs_MeshMapper())));
	CHECK(connect(ui.actionAsteroid_UVGapsManualCorrection, SIGNAL(triggered()), this, SLOT(loadExample_Issues_UVsWithManualTexture())));

	CHECK(connect(ui.actionAsteroid_DynamicTessellation_16, SIGNAL(triggered()), this, SLOT(loadExample_LOD_DynamicTess_16())));
	CHECK(connect(ui.actionAsteroid_DynamicTessellation_64, SIGNAL(triggered()), this, SLOT(loadExample_LOD_DynamicTess_64())));

	CHECK(connect(ui.actionAsteroid_DynamicTessellation_16_with_LOD, SIGNAL(triggered()), this, SLOT(loadExample_LOD_DynamicTess_16_with_Common_LOD())));
	CHECK(connect(ui.actionAsteroid_DynamicTessellation_64_with_LOD, SIGNAL(triggered()), this, SLOT(loadExample_LOD_DynamicTess_64_with_Common_LOD())));

	CHECK(connect(ui.actionAsteroid_DynamicTessellation_16_with_LOD_cust_range, SIGNAL(triggered()), this, SLOT(loadExample_LOD_DynamicTess_16_with_Common_LOD_cust_range())));
	CHECK(connect(ui.actionAsteroid_DynamicTessellation_64_with_LOD_cust_range, SIGNAL(triggered()), this, SLOT(loadExample_LOD_DynamicTess_64_with_Common_LOD_cust_range())));
	
	CHECK(connect(ui.actionAsteroid_LOD_no_tessellation, SIGNAL(triggered()), this, SLOT(loadExample_LOD_no_tessellation())));
	CHECK(connect(ui.actionAsteroid_LOD_no_tessellation_custom_range, SIGNAL(triggered()), this, SLOT(loadExample_LOD_no_tessellation_custom_range())));

	CHECK(connect(ui.actionCobblestone_singlepart, SIGNAL(triggered()), this, SLOT(loadExample_Cobblestone_SinglePart())));
	CHECK(connect(ui.actionCobblestone_multipart, SIGNAL(triggered()), this, SLOT(loadExample_Cobblestone_MultiPart())));

	CHECK(connect(ui.actionCobblestone_singlepart_modified_gaps, SIGNAL(triggered()), this, SLOT(loadExample_Cobblestone_SinglePart_Gaps())));
	CHECK(connect(ui.actionCobblestone_multipart_modified_gaps, SIGNAL(triggered()), this, SLOT(loadExample_Cobblestone_MultiPart_Gaps())));

	CHECK(connect(ui.actionCobblestone_equal_spacing, SIGNAL(triggered()), this, SLOT(loadExample_Cobblestone_spacing_equal())));
	CHECK(connect(ui.actionCobblestone_fractional_odd_spacing, SIGNAL(triggered()), this, SLOT(loadExample_Cobblestone_spacing_fractional_odd())));
	CHECK(connect(ui.actionCobblestone_fractional_even_spacing, SIGNAL(triggered()), this, SLOT(loadExample_Cobblestone_spacing_fractional_even())));

	CHECK(connect(ui.actionCobblestone_equal_spacing_wireframe, SIGNAL(triggered()), this, SLOT(loadExample_Cobblestone_spacing_equal_w())));
	CHECK(connect(ui.actionCobblestone_fractional_odd_spacing_wireframe, SIGNAL(triggered()), this, SLOT(loadExample_Cobblestone_spacing_fractional_odd_w())));
	CHECK(connect(ui.actionCobblestone_fractional_even_spacing_wireframe, SIGNAL(triggered()), this, SLOT(loadExample_Cobblestone_spacing_fractional_even_w())));

	CHECK(connect(ui.actionTriangle, SIGNAL(triggered()), this, SLOT(loadExample_Miscellaneous_Triangle())));
	CHECK(connect(ui.actionTriangle_dynamic, SIGNAL(triggered()), this, SLOT(loadExample_Miscellaneous_Triangle_Dynamic())));
	CHECK(connect(ui.actionTriangle_dynamic_regular, SIGNAL(triggered()), this, SLOT(loadExample_Miscellaneous_Triangle_Dynamic_Regular())));

	CHECK(connect(ui.actionQuads, SIGNAL(triggered()), this, SLOT(loadExample_Miscellaneous_Quads())));
	CHECK(connect(ui.actionQuads_dynamic, SIGNAL(triggered()), this, SLOT(loadExample_Miscellaneous_Quads_Dynamic())));

	CHECK(connect(ui.actionPerf_DynamicTessellation_shader, SIGNAL(triggered()), this, SLOT(loadExample_Performance_DynamicTessellation_Shader())));
	CHECK(connect(ui.actionPerf_DynamicTessellation_callback, SIGNAL(triggered()), this, SLOT(loadExample_Performance_DynamicTessellation_Callback())));
	CHECK(connect(ui.actionPerf_DynamicTessellation_fixed, SIGNAL(triggered()), this, SLOT(loadExample_Performance_DynamicTessellation_Fixed())));

	CHECK(connect(ui.actionPerf_DynamicTessellation_Cobblestone_Combined, SIGNAL(triggered()), this, SLOT(loadExample_Performance_DynamicTessellation_Cobblestone_Combined())));
	CHECK(connect(ui.actionPerf_DynamicTessellation_Cobblestone_Shader, SIGNAL(triggered()), this, SLOT(loadExample_Performance_DynamicTessellation_Cobblestone_Shader())));
	CHECK(connect(ui.actionPerf_DynamicTessellation_Cobblestone_Shader_Modified, SIGNAL(triggered()), this, SLOT(loadExample_Performance_DynamicTessellation_Cobblestone_Shader_Modified())));

	CHECK(connect(ui.actionDisplacement_mapping, SIGNAL(triggered()), this, SLOT(loadExample_Performance_Tessellated())));//loadExample_Performance_Tessellated
	CHECK(connect(ui.actionOriginalModel, SIGNAL(triggered()), this, SLOT(loadExample_Performance_FullModel())));

	QSignalMapper * mapper_camera = new QSignalMapper(this);



	CHECK(connect(ui.actionKamera_1, SIGNAL(triggered ()), mapper_camera, SLOT(map() ))); mapper_camera->setMapping(ui.actionKamera_1,0);
	CHECK(connect(ui.actionKamera_2, SIGNAL(triggered ()), mapper_camera, SLOT(map() ))); mapper_camera->setMapping(ui.actionKamera_2,1);
	CHECK(connect(ui.actionKamera_3, SIGNAL(triggered ()), mapper_camera, SLOT(map() ))); mapper_camera->setMapping(ui.actionKamera_3,2);
	CHECK(connect(ui.actionKamera_4, SIGNAL(triggered ()), mapper_camera, SLOT(map() ))); mapper_camera->setMapping(ui.actionKamera_4,3);
	CHECK(connect(ui.actionKamera_5, SIGNAL(triggered ()), mapper_camera, SLOT(map() ))); mapper_camera->setMapping(ui.actionKamera_5,4);
	CHECK(connect(ui.actionKamera_6, SIGNAL(triggered ()), mapper_camera, SLOT(map() ))); mapper_camera->setMapping(ui.actionKamera_6,5);
	CHECK(connect(ui.actionKamera_7, SIGNAL(triggered ()), mapper_camera, SLOT(map() ))); mapper_camera->setMapping(ui.actionKamera_7,6);
	CHECK(connect(ui.actionKamera_8, SIGNAL(triggered ()), mapper_camera, SLOT(map() ))); mapper_camera->setMapping(ui.actionKamera_8,7);
	CHECK(connect(ui.actionKamera_9, SIGNAL(triggered ()), mapper_camera, SLOT(map() ))); mapper_camera->setMapping(ui.actionKamera_9,8);
	CHECK(connect(ui.actionKamera_10, SIGNAL(triggered ()), mapper_camera, SLOT(map() ))); mapper_camera->setMapping(ui.actionKamera_10,9);
	CHECK(connect(ui.actionKamera_11, SIGNAL(triggered ()), mapper_camera, SLOT(map() ))); mapper_camera->setMapping(ui.actionKamera_11,10);

	CHECK(connect(mapper_camera, SIGNAL(mapped( int )), this, SLOT(setCameraMapped( int ))));

	/*void loadExample_Cobblestone_spacing_equal();
	void loadExample_Cobblestone_spacing_fractional_odd();
	void loadExample_Cobblestone_spacing_fractional_even();

	void loadExample_Cobblestone_spacing_equal_w();
	void loadExample_Cobblestone_spacing_fractional_odd_w();
	void loadExample_Cobblestone_spacing_fractional_even_w();*/

	//

	

/*
	retbool[35] = connect(ui.actionKamera_1, SIGNAL(triggered ()), this, SLOT(setCamera1() ));
	retbool[36] = connect(ui.actionKamera_2, SIGNAL(triggered ()), this, SLOT(setCamera2() ));
	retbool[37] = connect(ui.actionKamera_3, SIGNAL(triggered ()), this, SLOT(setCamera3() ));
	retbool[38] = connect(ui.actionKamera_4, SIGNAL(triggered ()), this, SLOT(setCamera4() ));*/




	//retbool[33] = connect()


	ui.listOfShaderPrograms->horizontalHeader()->show();
	ui.listOfShaderPrograms->verticalHeader()->show();

	/*	ui.shaderProgramWidget->show();
	ui.showShaderProgramField->hide();*/

	stateSetKeeper = new osg::Node;

	attributeValuesTooltip="Mozne hodnoty: INHERIT | OFF | ON | OVERRIDE | PROTECTED";
	initAttributeConvContainers();
	initMap_uniformType_isEditable();

	uniformType_isEditable_mapSet = false;
	stopUniformTableValueChanged = false;
	//ATI Technologies Inc.
	//NVIDIA Corporation
	osg::setNotifyLevel(osg::NOTICE);
 	ui.osgGLWidget->makeCurrent();
	gpuVendor = getVendorFromString(glGetString(GL_VENDOR));
	if (gpuVendor == ATI)
	{
		retbool[0] = connect(ui.perfCounters_Analyze,SIGNAL(pressed()),this,SLOT(takePerfSnapshot_ATI()));
		retbool[2] = connect(ui.osgGLWidget,SIGNAL(firstFrameDrawn()),this,SLOT(initPerfCounters_ATI()));
		retbool[4] = connect(ui.GPA_counter_list,SIGNAL(cellClicked ( int , int  )),this,SLOT(hidePerfRow_ATI( int , int  )));
		GPA_Initialize();
	}
	else if (gpuVendor == NVIDIA)
	{
		perfNV_initialized = false;
		retbool[0] = connect(ui.perfCounters_Analyze,SIGNAL(pressed()),this,SLOT(takePerfSnapshot_NV()));
		retbool[2] = connect(ui.osgGLWidget,SIGNAL(firstFrameDrawn()),this,SLOT(initPerfCounters_NV()));
		retbool[4] = connect(ui.GPA_counter_list,SIGNAL(cellClicked ( int , int  )),this,SLOT(hidePerfRow_NV( int , int  )));
		initNVAPI();
	}
	else //force
	{
		writeToConsoleQt(QtCriticalMsg,QString("Chyba - nelze urèit výrobce GPU\nIdentifikovaný výrobce \"%1\" neodpovídá \"%2\" ani \"%3\"\nZkousim inicializovat AMD").arg(gpuVendor).arg(ATI).arg(NVIDIA));
		retbool[0] = connect(ui.perfCounters_Analyze,SIGNAL(triggered()),this,SLOT(takePerfSnapshot_ATI()));
		retbool[2] = connect(ui.osgGLWidget,SIGNAL(firstFrameDrawn()),this,SLOT(initPerfCounters_ATI()));
		retbool[4] = connect(ui.GPA_counter_list,SIGNAL(cellClicked ( int , int  )),this,SLOT(hidePerfRow_ATI( int , int  )));
		GPA_Initialize();
	}
	activeTimer = startTimer(ui.timerInterval->value()*1000);
	osg::setNotifyHandler(notifyInstance);
	ui.osgGLWidget->setTextOutput(ui.textEdit_Console);
	perfATI_initialized = false;
	perfATI_setup = false;
	//ui.dockWidget_Console;

	this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	this->tabifyDockWidget(ui.dockWidget_GPUPerfAPI,ui.dockWidget_Uniforms);
	this->tabifyDockWidget(ui.dockWidget_Uniforms,ui.dockWidget_Attributes);
	this->tabifyDockWidget(ui.dockWidget_Attributes,ui.dockWidget_StateSets);
	ui.dockWidget_GPUPerfAPI->raise();
	for (std::map<osg::Uniform::Type,std::string>::iterator it = uniformType_name.begin(); it!=uniformType_name.end(); it++)
	{
		cuw.uniformList->addItem(new osgUniformListWidget(QString(it->second.c_str()),it->first));
	}

	testRepeater = new test_repeater(this, ui.GPA_counter_list);

	loadExampleTeselace();

	//loadExampleSceneTreeTest();


// 	retbool[11] =  connect(notifyInstance, SIGNAL(notifyInstance::writeToConsole(osg::NotifySeverity , const char *)), 
// 		qApp, SLOT(aboutQt()));

	//GPA_OpenContext()
	//ui.osgGLWidget->realize();
}

void shader_editor::lineCallFailure(int line, const char * file, const char * text)
{
	QString message = QString("%1:%2: chyba nalezena na radku \"%3\"").arg(file).arg(line).arg(text);
	writeToConsoleQt(QtWarningMsg, message);

}

shader_editor::~shader_editor()
{
	this->osg_debug_output->close();
	delete this->osg_debug_output;
	if(perfATI_initialized)
	{
		//GPA_CloseContext();
		GPA_Destroy();
	}
	if (shaderTabContainer)
		delete shaderTabContainer;
}

void shader_editor::initShaderContent()
{
	return ;
	QString initial_vertex = "shader_init/simple_tess_vertex.vs";
	QString initial_fragment = "shader_init/simple_tess_fragment.fs";
	QFile VS_file(initial_vertex);
	QFile FS_file(initial_fragment);
	VS_file.open(QIODevice::ReadOnly);
	FS_file.open(QIODevice::ReadOnly);
	if (!VS_file.isReadable())
	{
		writeToConsoleQt(QtWarningMsg, ("nelze otevrit soubor s pocatecnim vertex shaderem: " + initial_vertex));
	}
	if (!FS_file.isReadable())
	{
		writeToConsoleQt(QtWarningMsg, ("nelze otevrit soubor s pocatecnim fragment shaderem: " + initial_fragment));
	}
	QString VS_string = VS_file.readAll();
	QString FS_string = FS_file.readAll();

	VS_file.close();
	FS_file.close();

	//ui.textEdit_VS->setPlainText(VS_string);
	//ui.textEdit_FS->setPlainText(FS_string);
}



bool shader_editor::isCtrlPressed()
{
	if(GetKeyState(VK_LCONTROL))
		return true;
	else
		return false;
}

bool shader_editor::loadShader( QPlainTextEdit * targetEdit, bool * modified, char * shaderType )
{
	if(*modified=true)
	{
		QMessageBox msgBox;
		msgBox.setText("Dokument " + QString(shaderType) + " má neuložené zmìny.");
		msgBox.setInformativeText("Chcete pokraèovat? Dojde ke ztrátì neuložených dat");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
		msgBox.exec();
		if (msgBox.clickedButton()==msgBox.button(QMessageBox::Yes))
			return true;
	}
	QString filename = QFileDialog::getOpenFileName(this,"Otevrit soubor s " + QString(shaderType),"/");

	QFile loadFile(filename);
	if (!loadFile.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox msgBox;
		msgBox.setText("Nelze otevrit dany soubor");
		msgBox.exec();
		return false;
	}

	//char * filecontent = new char[loadFile.size()];
	//targetEdit->setPlainText(loadFile.readAll());

	*modified=false;
	return true;
}

bool shader_editor::loadShader( QPlainTextEdit * targetEdit, char * shaderType , char * filename)
{
	QFile loadFile(filename);
	if (!loadFile.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox msgBox;
		msgBox.setWindowTitle(filename);
		msgBox.setText("Nelze otevrit dany soubor");
		msgBox.exec();
		return false;
	}

	//char * filecontent = new char[loadFile.size()];
	//targetEdit->setPlainText(loadFile.readAll());
	return true;

}

void shader_editor::saveAsShader( QPlainTextEdit * sourceEdit, bool * modified, char * shaderType )
{

}

void shader_editor::saveShader( QPlainTextEdit * sourceEdit, bool * modified, char * shaderType )
{

}

void shader_editor::compileShaders()
{
	if (actualStateSet == NULL)
	{
		writeToConsoleQt(QtCriticalMsg,"Program nelze zkompilovat - neni urceny aktualni stateset");
		return;
	}
	QString * vertex = new QString;
	QString * tesscontrol = NULL;
	QString * tesseval = NULL;
	QString * geometry = NULL;
	QString * fragment = new QString;
	char * msg;

	*vertex = (ui.textEdit_VS->toPlainText());
	*fragment = (ui.textEdit_FS->toPlainText());
	if(ui.actionUseTessellationShader->isChecked())
	{
		geometry= new QString;
		*geometry=(ui.textEdit_GS->toPlainText());
	}
	if (ui.actionUseTessellationShader->isChecked())
	{
		tesscontrol = new QString;
		tesseval = new QString;
		*tesscontrol=(ui.textEdit_TCS->toPlainText());
		*tesseval=(ui.textEdit_TES->toPlainText());
	}

	osg::ref_ptr<osg::StateSet> modelstateset = actualStateSet;//viewer->getSceneData()->getOrCreateStateSet();
	osg::ref_ptr<osg::Program> shaderprogram = new osg::Program;
	shaderprogram->setName("basic_shader_program");

	osg::ref_ptr<osg::Shader> vertexsh = new osg::Shader(osg::Shader::VERTEX);
	osg::ref_ptr<osg::Shader> tesscontrolsh = new osg::Shader(osg::Shader::TESSCONTROL);
	osg::ref_ptr<osg::Shader> tessevalsh = new osg::Shader(osg::Shader::TESSEVALUATION);
	osg::ref_ptr<osg::Shader> geometrysh = new osg::Shader(osg::Shader::GEOMETRY);
	osg::ref_ptr<osg::Shader> fragmentsh = new osg::Shader(osg::Shader::FRAGMENT);

	vertexsh->setShaderSource(vertex->toStdString());
	shaderprogram->addShader(vertexsh);

	if (tesscontrol != NULL && tesseval != NULL)
	{
		tesscontrolsh->setShaderSource(tesscontrol->toStdString());
		shaderprogram->addShader(tesscontrolsh);


		tessevalsh->setShaderSource(tesseval->toStdString());
		shaderprogram->addShader(tessevalsh);
	}


	if (geometry!=NULL)
	{
		geometrysh->setShaderSource(geometry->toStdString());
		shaderprogram->addShader(geometrysh);
	}



	fragmentsh->setShaderSource(fragment->toStdString());
	shaderprogram->addShader(fragmentsh);

	//shaderprogram->setParameter(GL_GEOMETRY_VERTICES_OUT_EXT, 3);
	shaderprogram->setParameter(GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
	shaderprogram->setParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
	shaderprogram->setParameter(GL_PATCH_VERTICES,3);
	shaderprogram->addBindFragDataLocation("fragColor",0);

	// 	tess_inner_U = new osg::Uniform("TessLevelInner",4.0f);
	// 	tess_outer_U = new osg::Uniform("TessLevelOuter",4.0f);
	// 
	// 	modelstateset->addUniform(tess_inner_U);
	// 	modelstateset->addUniform(tess_outer_U);
	// 	modelstateset->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
	// 	modelstateset->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.0f, 0.75f, 0.75f)));
	// 	modelstateset->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));

	//shaderprogram->addBindFragDataLocation("fragColor",0);
	modelstateset->setAttributeAndModes(shaderprogram, osg::StateAttribute::ON);

	int i = 1;

	delete vertex;
	delete fragment;
	if (geometry)
		delete geometry;
	if (tesseval)
		delete tesseval;
	if (tesscontrol)
		delete tesscontrol;

}

void shader_editor::compileShaders(osg::Node * targetNode, bool tess, bool geom)
{
	QString * vertex = new QString;
	QString * tesscontrol = NULL;
	QString * tesseval = NULL;
	QString * geometry = NULL;
	QString * fragment = new QString;
	char * msg;

	*vertex = (ui.textEdit_VS->toPlainText());
	*fragment = (ui.textEdit_FS->toPlainText());
	if(ui.actionUseTessellationShader->isChecked() || geom)
	{
		geometry= new QString;
		*geometry=(ui.textEdit_GS->toPlainText());
	}
	if (ui.actionUseTessellationShader->isChecked() || tess)
	{
		tesscontrol = new QString;
		tesseval = new QString;
		*tesscontrol=(ui.textEdit_TCS->toPlainText());
		*tesseval=(ui.textEdit_TES->toPlainText());
	}

	if(!ui.osgGLWidget->compileShader(targetNode, vertex,tesscontrol,tesseval,geometry,fragment,&msg))
	{
		writeToConsoleQt(QtWarningMsg,msg);
	}
	delete vertex;
	delete fragment;
	if (geometry)
		delete geometry;
	if (tesseval)
		delete tesseval;
	if (tesscontrol)
		delete tesscontrol;

}



void shader_editor::rebuildSceneTree()
{
	sceneTreeContainer->rebuildSceneTree();
	return;
/*
	ui.sceneTreeWidget->clear();
	sgNodeVisitor visitor;
	osg::Node * start_node = ui.osgGLWidget->getSceneData();
	if (start_node == NULL)
	{
		return;
	}
	start_node->accept(visitor);

	QStringList node_name_list;  
	node_name_list << "Strom scény" << "";
	osgNodeTreeItem * tmp=new osgNodeTreeItem(NULL,node_name_list, TREE_ITEM_FIXED);

	int k=0;
	for (std::vector<osgNodeTreeItem *>::iterator i = visitor.SceneTreeItemList.begin(); i != visitor.SceneTreeItemList.end();i++)
	{
		tmp->addChild(*i);
		(*i)->parentTreeItem = tmp;
		//ui.treeWidget->insertTopLevelItem(k,*i);
		k++;
	}
	ui.sceneTreeWidget->insertTopLevelItem(0,tmp);

	node_name_list.clear();  
	node_name_list << "Neaktivní prvky scény" << "";
	tmp=new osgNodeTreeItem(NULL,node_name_list, TREE_ITEM_FIXED);
	ui.sceneTreeWidget->insertTopLevelItem(1,tmp);



	ui.sceneTreeWidget->expandAll();	*/
}

void shader_editor::writeToConsole( osg::NotifySeverity severity, const char *message )
{
	(*this->osg_debug_output) << message << std::endl;
	if(severity == osg::DEBUG_FP)
	{
		ui.textEdit_Console->setTextColor(Qt::darkGray);
		ui.textEdit_Console->append(message);
	}
	if(severity == osg::DEBUG_INFO)
	{
		ui.textEdit_Console->setTextColor(Qt::darkGray);
		ui.textEdit_Console->append(message);
	}
	else if (severity == osg::FATAL)
	{
		ui.textEdit_Console->setTextColor(Qt::red);
		ui.textEdit_Console->append(message);
// 		QMessageBox::warning(this, tr("OSG Fatal Error"),
// 			tr(message));
	} 
	else if (severity == osg::WARN)
	{
		ui.textEdit_Console->setTextColor(Qt::darkYellow);
		ui.textEdit_Console->append(message);
	}
	else if (severity == osg::NOTICE)
	{
		ui.textEdit_Console->setTextColor(Qt::black);
		ui.textEdit_Console->append(message);
	} 
	else if (severity == osg::INFO)
	{
		ui.textEdit_Console->setTextColor(Qt::black);
		ui.textEdit_Console->append(message);
	} 
}

void shader_editor::writeToConsoleQt( QtMsgType severity, const char *message )
{
	if(severity == QtWarningMsg)
	{
		ui.textEdit_Console->setTextColor(Qt::darkYellow);
		ui.textEdit_Console->append(message);
	}
	else if(severity == QtCriticalMsg)
	{
		ui.textEdit_Console->setTextColor(Qt::red);
		ui.textEdit_Console->append(message);
	} 
	else if(severity == QtDebugMsg)
	{
		ui.textEdit_Console->setTextColor(Qt::black);
		ui.textEdit_Console->append(message);
	}
	else if(severity == QtFatalMsg)
	{
		ui.textEdit_Console->setTextColor(Qt::darkRed);
		ui.textEdit_Console->append(message);
	} 
	else if(severity == QtSystemMsg)
	{
		ui.textEdit_Console->setTextColor(Qt::black);
		ui.textEdit_Console->append(message);
	}

}

void shader_editor::writeToConsoleQt( QtMsgType severity, char *message )
{
	if(severity == QtWarningMsg)
	{
		ui.textEdit_Console->setTextColor(Qt::darkYellow);
		ui.textEdit_Console->append(message);
	}
	else if(severity == QtCriticalMsg)
	{
		ui.textEdit_Console->setTextColor(Qt::red);
		ui.textEdit_Console->append(message);
	} 
	else if(severity == QtDebugMsg)
	{
		ui.textEdit_Console->setTextColor(Qt::black);
		ui.textEdit_Console->append(message);
	}
	else if(severity == QtFatalMsg)
	{
		ui.textEdit_Console->setTextColor(Qt::darkRed);
		ui.textEdit_Console->append(message);
	} 
	else if(severity == QtSystemMsg)
	{
		ui.textEdit_Console->setTextColor(Qt::black);
		ui.textEdit_Console->append(message);
	}
}

void shader_editor::writeToConsoleQt( QtMsgType severity, QString message )
{
	writeToConsoleQt(severity,message.toStdString().c_str());
}

void shader_editor::displayDockSceneTree()
{
	ui.dockWidget_SceneGraph->show();
}

void shader_editor::displayDockPerf()
{
	ui.dockWidget_GPUPerfAPI->show();
}

void shader_editor::displayDockConsole()
{
	ui.dockWidget_Console->show();
}


void shader_editor::rebuildUniformsTable_fromStateSetTable( int row, int column )
{
	rebuildUniformsTable((static_cast<osgUniversalTableWidget *>(ui.stateSetTableWidget->item(row,0)))->this_stateset);
}


void shader_editor::rebuildUniformsTable( QTreeWidgetItem * this_item )
{
	osgNodeTreeItem * actual_node = static_cast<osgNodeTreeItem *>(this_item);
	osg::Node * node = actual_node->this_node;
	if (node == NULL) return;

	rebuildUniformsTable(node->getStateSet());

}

void shader_editor::rebuildUniformsTable(osg::StateSet * this_set)
{
	stopUniformTableValueChanged = true;
// 	osgNodeTreeItem * actual_node = static_cast<osgNodeTreeItem *>(this_item);
// 
// 	osg::Node * node = actual_node->this_node;
	/*uniformTable_ActiveNode = node;*/

	actualStateSet = this_set;

	if (actualStateSet == NULL)
	{
		stopUniformTableValueChanged = false;
		return;
	}

	osg::StateSet::UniformList unilist = actualStateSet->getUniformList();

	int height = unilist.size();
	int width = 2;
	ui.uniformsTableWidget->clear();
	ui.uniformsTableWidget->setColumnCount(width);
	ui.uniformsTableWidget->setRowCount(height);

	unsigned i = 0;
	osg::Uniform::Type uniform_type;
	for( osg::StateSet::UniformList::iterator it = unilist.begin(); it != unilist.end(); ++it , i++)
	{
		//prvni sloupec, jmeno uniformu, obsahuje state kam uniform patri
		ui.uniformsTableWidget->setItem(i,0,new osgUniversalTableWidget(QString(it->first.c_str()),actualStateSet));
		uniform_type = it->second.first->getType();
		ui.uniformsTableWidget->item(i,0)->setFlags(ui.uniformsTableWidget->item(i,0)->flags() & (~Qt::ItemIsEditable));
		//uniformType_name
		//ui.tableWidget->setItem(i,1,new QTableWidgetItem(QString(it->second.first->)));
		if (uniformType_isEditable[uniform_type]==true)
		{
			ui.uniformsTableWidget->setItem(i,1,new QTableWidgetItem( uniformToString(it->second.first)->c_str() ));
			ui.uniformsTableWidget->item(i,1)->setFlags(ui.uniformsTableWidget->item(i,1)->flags() | Qt::ItemIsEditable);
			std::string * value = uniformToString(it->second.first);
			ui.uniformsTableWidget->item(i,1)->setText(value->c_str());
			ui.uniformsTableWidget->item(i,1)->setToolTip(QString(uniformType_name[uniform_type].c_str()));
			
		}
		else
		{
			ui.uniformsTableWidget->setItem(i,1,new QTableWidgetItem( uniformToString(it->second.first)->c_str()));
			ui.uniformsTableWidget->item(i,1)->setFlags(ui.uniformsTableWidget->item(i,1)->flags() & (~Qt::ItemIsEditable));//^ Qt::ItemFlag::ItemIsEditable);
			std::string * value = uniformToString(it->second.first);
			ui.uniformsTableWidget->item(i,1)->setText(value->c_str());
			ui.uniformsTableWidget->item(i,1)->setToolTip(uniformType_name[uniform_type].c_str());
		}

	}
	/*std::ostringstream pointer_string, uniformCountString, attributeCountString;
		pointer_string << stateset_list[i];*/
	std::ostringstream pointer_string;
	pointer_string << this_set;
	stopUniformTableValueChanged = false;
	if (!this_set->getName().empty())
		ui.uniformsTableWidget_activeState->setText(QString::fromStdString(this_set->getName()));
	else
		ui.uniformsTableWidget_activeState->setText(QString::fromStdString(pointer_string.str()));
}

void shader_editor::uniformTableValueChanged( int row )
{
	if(stopUniformTableValueChanged) return;
	osgUniversalTableWidget * this_uniformitem = static_cast<osgUniversalTableWidget *>(ui.uniformsTableWidget->item(row,0));

	osg::StateSet * uniform_owner = this_uniformitem->this_stateset;

	if(uniform_owner == NULL) return;

	std::string uniform_name = this_uniformitem->text().toStdString();
	std::string uniform_stringvalue = ui.uniformsTableWidget->item(row,1)->text().toStdString();

	osg::Uniform * this_uniform = uniform_owner->getUniform(uniform_name);
	if (this_uniform == NULL)
		return;

	if (stringToUniform(&uniform_stringvalue,this_uniform)==false)
	{
		ui.uniformsTableWidget->item(row,1)->setText(uniformToString(this_uniform)->c_str());
	}
	else
		ui.osgGLWidget->repaint();


}

void shader_editor::resetApplicationState()
{
	uniformTable_ActiveNode = NULL;
}

void shader_editor::setCurrentSceneGraphWidgetItem( QTreeWidgetItem * item )
{
	ui.sceneTreeWidget->setCurrentItem(item);
}

void buildStateSetList(std::vector<osg::StateSet *> * stateset_list, osg::Node * this_node)
{
	osg::StateSet * this_stateset = this_node->getStateSet();
	bool found = false;

	for (unsigned i=0; i<stateset_list->size(); i++)
	{
		if ((*stateset_list)[i] == this_stateset)
		{
			found=true;
			break;
		}
	}

	if (!found && this_stateset!=NULL) 
		stateset_list->push_back(this_stateset);

	osg::Group * this_group;
	if(this_node->asGroup() != NULL)
	{
		this_group = this_node->asGroup();
		for (unsigned i = 0; i < this_node->asGroup()->getNumChildren(); i++)
		{
			buildStateSetList(stateset_list, this_node->asGroup()->getChild(i));
		}
	}
}


void shader_editor::appendToStateSetTable( osg::Node * this_node )
{
	static unsigned id=0, unicount=1, attribcount=2;
	std::vector<osg::StateSet *> stateset_list;
	buildStateSetList(&stateset_list,this_node);
	int rows = ui.stateSetTableWidget->rowCount();
	ui.stateSetTableWidget->setRowCount(rows + stateset_list.size());
	osgUniversalTableWidget * statesetItem;
	QTableWidgetItem * uniformCountItem, * attributeCountItem;

	for (unsigned i=0, k = rows; i<stateset_list.size(); i++, k++)
	{
		//stateset_list[i]->addParent(stateSetKeeper);
		std::ostringstream pointer_string, uniformCountString, attributeCountString;
		pointer_string << stateset_list[i];
		uniformCountString << stateset_list[i]->getUniformList().size();
		attributeCountString << stateset_list[i]->getAttributeList().size();

		statesetItem	= new osgUniversalTableWidget(pointer_string.str().c_str(), stateset_list[i]);
		statesetItem->setFlags(statesetItem->flags() & (~Qt::ItemIsEditable));

		uniformCountItem=	new QTableWidgetItem(uniformCountString.str().c_str());
		uniformCountItem->setFlags(uniformCountItem->flags() & (~Qt::ItemIsEditable));

		attributeCountItem=	new QTableWidgetItem(attributeCountString.str().c_str());
		attributeCountItem->setFlags(attributeCountItem->flags() & (~Qt::ItemIsEditable));

		ui.stateSetTableWidget->setItem(k,id,statesetItem);
		ui.stateSetTableWidget->setItem(k,unicount,uniformCountItem);
		ui.stateSetTableWidget->setItem(k,attribcount,attributeCountItem);
	}
}


void shader_editor::rebuildStateSetTable()
{
	static unsigned id=0, unicount=1, attribcount=2;
	osg::Node * this_node = ui.osgGLWidget->getSceneData();
	if(this_node == NULL)
		return;

	unsigned width = 3, height = 0;
	ui.stateSetTableWidget->clear();
	ui.stateSetTableWidget->setColumnCount(width);
	ui.stateSetTableWidget->setRowCount(0);
	ui.stateSetTableWidget->setHorizontalHeaderItem(id,new QTableWidgetItem("id"));
	ui.stateSetTableWidget->setHorizontalHeaderItem(unicount,new QTableWidgetItem("Pocet uniformu"));
	ui.stateSetTableWidget->setHorizontalHeaderItem(attribcount,new QTableWidgetItem("Pocet attributu"));

	appendToStateSetTable(this_node);
}

void shader_editor::setupClean(bool noCamera)
{
	rebuildSceneTree();
	rebuildStateSetTable();
	shaderTabContainer->buildProgramList();
	unsigned width = 2, height = 0;
	ui.uniformsTableWidget->clear();
	ui.uniformsTableWidget->setColumnCount(width);
	ui.uniformsTableWidget->setRowCount(height);

	if(!noCamera)
		setCameraMapped(1);
}

void shader_editor::rebuildAttributeList(osg::StateSet * this_set)
{
	if (this_set == NULL)
		return;
	osg::StateSet::AttributeList this_AL = this_set->getAttributeList();
	const unsigned attr_classname=0, attr_type=1,attr_value=2;
	const unsigned width = 3, height = 0;
	ui.attributesTableWidget->clear();
	ui.attributesTableWidget->setColumnCount(width);
	ui.attributesTableWidget->setRowCount(this_AL.size());


	unsigned i = 0;
	for (osg::StateSet::AttributeList::iterator it = this_AL.begin(); it!=this_AL.end(); it++, i++)
	{
		QString classname_string = 
			attributeValuesToValuesName.count(static_cast<osg::StateAttribute::Values>(it->first.second)) != 0 ? 
			attributeValuesToValuesName[static_cast<osg::StateAttribute::Values>(it->first.second)] : 
			"error: not_found";
		ui.attributesTableWidget->setItem(i,attr_value,new QTableWidgetItem(classname_string));
		ui.attributesTableWidget->item(i,attr_value)->setFlags(ui.attributesTableWidget->item(i,attr_value)->flags() & (~Qt::ItemIsEditable));
		QString typename_string = 
			attributeTypeToAttributeName.count(it->first.first) != 0 ? 
			attributeTypeToAttributeName[it->first.first] : 
			"error: not_found";
 		ui.attributesTableWidget->setItem(i,attr_type,new QTableWidgetItem(typename_string));
		ui.attributesTableWidget->item(i,attr_type)->setFlags(ui.attributesTableWidget->item(i,attr_type)->flags() & (~Qt::ItemIsEditable));

//		ui.attributesTableWidget->setItem(i,attr_type,new QTableWidgetItem(attributeTypeToAttributeName[attributeTypeVector[i]]));
		//ui.stateSetTableWidget->setItem(i,attr_value,new QTableWidgetItem(attributeValuesToValuesName[this_AL[attributeTypeVector[i]].first->valu]));//attributeTypeToAttributeName[attributeTypeVector[i]]));
// 		ui.stateSetTableWidget->setItem(i,attr_value,new QTableWidgetItem(""));
	}
}

void shader_editor::rebuildAttributesTable_StateSet( int row, int column )
{
	osg::StateSet * this_stateset = (static_cast<osgUniversalTableWidget *>(ui.stateSetTableWidget->item(row,0)))->this_stateset;
	actualStateSet = this_stateset;
	std::ostringstream pointer_string;
	pointer_string << this_stateset;
	if (!this_stateset->getName().empty())
		ui.attributesTableWidget_currentState->setText(QString::fromStdString(this_stateset->getName()));
	else
		ui.attributesTableWidget_currentState->setText(QString::fromStdString(pointer_string.str()));
	rebuildAttributeList(this_stateset);
}

void shader_editor::rebuildAttributesTable_Node(QTreeWidgetItem * this_item)
{
	osgNodeTreeItem * actual_node = static_cast<osgNodeTreeItem *>(this_item);
	if (actual_node->this_node->getStateSet()==NULL)
	{
		actualStateSet = NULL;
	}
	else
	{
		std::ostringstream pointer_string;
		pointer_string << actual_node->this_node->getStateSet();
		if (!actual_node->this_node->getStateSet()->getName().empty())
			ui.attributesTableWidget_currentState->setText(QString::fromStdString(actual_node->this_node->getStateSet()->getName()));
		else
			ui.attributesTableWidget_currentState->setText(QString::fromStdString(pointer_string.str()));
	}
	rebuildAttributeList(actual_node->this_node->getStateSet());
}

void shader_editor::uniformTableUniformRemove()
{
	int row_to_delete = ui.uniformsTableWidget->currentRow();
	osgUniversalTableWidget * this_uniformitem = static_cast<osgUniversalTableWidget *>(ui.uniformsTableWidget->item(row_to_delete,0));
	osg::StateSet * this_stateset = this_uniformitem->this_stateset;
	osg::Uniform * this_uniform = this_stateset->getUniform(this_uniformitem->text().toStdString());
	if (this_uniform!=NULL)
		this_stateset->removeUniform(this_uniform);
	ui.uniformsTableWidget->removeRow(row_to_delete);
}

void shader_editor::uniformTableUniformAdd()
{
	cuwContainer->show();
}

void shader_editor::uniformTableUniformAdd_cuwSetProtovalue()
{
	osgUniformListWidget * this_uniformitem = static_cast<osgUniformListWidget *>(cuw.uniformList->currentItem());
	cuw.uniformValue->setText(uniformProtovalue(this_uniformitem->uniform_type)->c_str());
}

void shader_editor::uniformTableUniformAdd_cuwCreate()
{
	std::string uniform_name = cuw.uniformName->text().toStdString();//this_uniformitem->text().toStdString();
	std::string uniform_stringvalue = cuw.uniformValue->text().toStdString();
	if (uniform_name == "")
		return;

	osg::Uniform * this_uniform = new osg::Uniform();

	this_uniform->setType((static_cast<osgUniformListWidget *>(cuw.uniformList->currentItem())->uniform_type));

	if (stringToUniform(&uniform_stringvalue,this_uniform)==false)
	{
		QMessageBox msgBox;
		msgBox.setText("Nelze precist hodnotu promenne, zrejme je spatne zapsana.");
		msgBox.exec();
		return;
	}

	if(actualStateSet->getUniform(uniform_name) != NULL)
	{
		//hláška: jméno už existuje
		QMessageBox msgBox;
		msgBox.setText("Uniform s timto jmenem jiz existuje.");
		msgBox.exec();
		return;
	}

	this_uniform->setName(uniform_name);

	actualStateSet->addUniform(this_uniform);
	cuwContainer->hide();
	rebuildUniformsTable(actualStateSet);
}

void shader_editor::statesetTableCreateNew()
{
	osgUniversalTableWidget * statesetItem;
	QTableWidgetItem * uniformCountItem, * attributeCountItem;
	osg::StateSet * this_stateset = new osg::StateSet();
	std::ostringstream pointer_string, uniformCountString, attributeCountString;
	pointer_string << this_stateset;
	uniformCountString << this_stateset->getUniformList().size();
	attributeCountString << this_stateset->getAttributeList().size();

	statesetItem	= new osgUniversalTableWidget(pointer_string.str().c_str(), this_stateset);
	statesetItem->setFlags(statesetItem->flags() & (~Qt::ItemIsEditable));

	uniformCountItem=	new QTableWidgetItem(uniformCountString.str().c_str());
	uniformCountItem->setFlags(uniformCountItem->flags() & (~Qt::ItemIsEditable));

	attributeCountItem=	new QTableWidgetItem(attributeCountString.str().c_str());
	attributeCountItem->setFlags(attributeCountItem->flags() & (~Qt::ItemIsEditable));

	int pos = ui.stateSetTableWidget->rowCount();
	ui.stateSetTableWidget->setRowCount(pos+1);
 	ui.stateSetTableWidget->setItem(pos,0,statesetItem);
 	ui.stateSetTableWidget->setItem(pos,1,uniformCountItem);
 	ui.stateSetTableWidget->setItem(pos,2,attributeCountItem);
}

void shader_editor::treeWidgetSetStateSet()
{
	osgNodeTreeItem * this_nodeitem = static_cast<osgNodeTreeItem *>(ui.sceneTreeWidget->currentItem());
	int current_stateset = ui.stateSetTableWidget->currentRow();
	if (current_stateset<0)
		return;
	osg::StateSet * this_stateset = static_cast<osgUniversalTableWidget *>(ui.stateSetTableWidget->item(current_stateset,0))->this_stateset;
	if (this_nodeitem->this_node != NULL && this_stateset != NULL)
	{
		this_nodeitem->this_node->setStateSet(this_stateset);
		sceneTreeContainer->markHasStateSet(this_nodeitem);
	}
}

void shader_editor::treeWidgetAddNode()
{
	/*osg::Node * newNode = */sceneTreeContainer->createNewNode();
/*
	if (newNode==NULL)
		return;*/


}

void shader_editor::treeWidgetDeleteNode()
{
	delete ui.sceneTreeWidget->currentItem();
}

GPU_Vendor shader_editor::getVendorFromString( const GLubyte * glGetStringVendorRetVal )
{
	if(glGetStringVendorRetVal==NULL)
	{ 
		std::string consoleOutput = "glGetString(GL_VENDOR): Nezdarila se identifikace graficke karty - navratova hodnota NULL";
		this->writeToConsole(osg::NOTICE, consoleOutput.c_str());
		return NOT_SET;
	}
	//ATI Technologies Inc.
	//NVIDIA Corporation
	std::string vendorString((const char *)glGetStringVendorRetVal);
	if (vendorString == "ATI Technologies Inc.")
	{
		std::string consoleOutput = "glGetString(GL_VENDOR): GPU vendor identifikovany jako \"";
		consoleOutput+=(const char *)glGetStringVendorRetVal;
		consoleOutput+="\", nastavuji API pro citace";
		this->writeToConsole(osg::NOTICE, consoleOutput.c_str());
		return ATI;
	}
	else if (vendorString == "NVIDIA Corporation")
	{
		std::string consoleOutput = "glGetString(GL_VENDOR): GPU vendor identifikovany jako \"";
		consoleOutput+=(const char *)glGetStringVendorRetVal;
		consoleOutput+="\", nastavuji API pro citace";
		this->writeToConsole(osg::NOTICE, consoleOutput.c_str());
		return NVIDIA;
	}
	else
	{
		std::string consoleOutput = "glGetString(GL_VENDOR): Nerozpoznany GPU vendor=\"";
		consoleOutput+=(const char *)glGetStringVendorRetVal;
		consoleOutput+="\". ";
		this->writeToConsole(osg::NOTICE, consoleOutput.c_str());
		return NOT_SET;
	}
}

void shader_editor::timerEvent( QTimerEvent *event )
{
	if (ui.perfApi_TimedAnalysis->isChecked())
	{
		if (gpuVendor == ATI)
		{
			takePerfSnapshot_ATI();
		}
		else if (gpuVendor == NVIDIA)
		{
			takePerfSnapshot_NV();
		}
	}
}

void shader_editor::setTimerInterval( double d )
{
	killTimer(activeTimer);

	activeTimer = startTimer(d*1000);
}

void shader_editor::restartTimer( bool checked )
{
	if(checked)
	{
		killTimer(activeTimer);

		activeTimer = startTimer(ui.timerInterval->value()*1000);
	}

}

void shader_editor::massPerfCountersAnalyse()
{

	testRepeater->run_table();
	/*QFile dumpTextFile;//("COPY_FROM_ME.txt");
	dumpTextFile.setFileName("COPY_FROM_ME.txt");
	//dumpTextFile.open(QIODevice::WriteOnly| QIODevice::Text);
	if (!dumpTextFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		writeToConsoleQt(QtCriticalMsg,"nezdarilo se otevrit soubor pro zapis");
		return;
	}
	QTextStream dumpTextStream(&dumpTextFile);

	for (int i=0; i<ui.GPA_counter_list->rowCount();i++)
	{
		QString dumpString;
		for (int k=0; k<ui.GPA_counter_list->columnCount();k++)
		{
			if (ui.GPA_counter_list->item(i,k))
			{
				if (k!=0)
				{
					dumpString+=" ";
					int howManyAppend = 40 - dumpString.length();
					for (int t=0; t<howManyAppend; t++)
						dumpString.append(' ');
				}
				dumpString+=ui.GPA_counter_list->item(i,k)->text();
			}
		}
		dumpTextStream << dumpString << "\n";
	}
	dumpTextStream.flush();*/
}

void shader_editor::loadModel()
{
	//	ui.osgGLWidget->loadNewModel(QFileDialog::getOpenFileName(this,"Otevrit soubor s novym objektem","/"));
	QString filename = QFileDialog::getOpenFileName(this,"Otevrit soubor s novym objektem","/");
	//osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(filename.toStdString());

	QTime time;
	time.start();

	loadModel(filename, false);

	int difference = time.elapsed();
	QString text; 
	writeToConsoleQt(QtDebugMsg, text.sprintf("Otevirani souboru trvalo: %02d:%02d.%03d ms",difference/60000,(difference%60000)/1000,difference%1000));
	/*writeToConsoleQt(QtDebugMsg, QString("Otevirani souboru trvalo: ").append(QString::number(difference/60000))
																	  .append(":")
																	  .append(QString::number((difference%60000)/1000))
																	  .append(".")
																	  .append(QString::number(difference%1000)));*/

	/*osg::ref_ptr<osgDB::Options> options = new osgDB::Options();
	options->setOptionString("noTriStripPolygons noTesselateLargePolygons");
	//options->("noTesselateLargePolygons");
	//noTesselateLargePolygons
	osg::Node * loadedNode = osgDB::readNodeFile(filename.toStdString(),options);
	if(loadedNode!=NULL)
	{
		osgUtil::Optimizer * opt = new osgUtil::Optimizer();
		//osgUtil::IndexMeshVisitor imv;
		//loadedNode->accept(imv);
		//imv.makeMesh();
		opt->optimize(loadedNode,osgUtil::Optimizer::OptimizationOptions::INDEX_MESH | osgUtil::Optimizer::OptimizationOptions::VERTEX_PRETRANSFORM | osgUtil::Optimizer::OptimizationOptions::VERTEX_POSTTRANSFORM);
		sceneTreeContainer->appendNodeToInactive(loadedNode);
		appendToStateSetTable(loadedNode);
		shaderTabContainer->appendFromTree(loadedNode);
		delete opt;
	}*/
		//ui.sceneTreeWidget->insertNewSubtree(loadedNode);
	//rebuildSceneTree();
	//ui.osgGLWidget->forcePaintGL();
}

void shader_editor::loadModelOptimalizer()
{
	QString filename = QFileDialog::getOpenFileName(this,"Otevrit soubor s novym objektem","/");

	QTime time;
	time.start();

	loadModel(filename, true);

	int difference = time.elapsed();
	QString text; 
	writeToConsoleQt(QtDebugMsg, text.sprintf("Otevirani souboru trvalo: %02d:%02d.%03d ms",difference/60000,(difference%60000)/1000,difference%1000));
/*
	writeToConsoleQt(QtDebugMsg, QString("Otevirani souboru trvalo: ").append(QString::number(difference/60000))
		.append(":")
		.append(QString::number((difference%60000)/1000))
		.append(QString::number(difference%1000)));*/
}

void shader_editor::loadModel(QString filename, bool optimize)
{
	//	ui.osgGLWidget->loadNewModel(QFileDialog::getOpenFileName(this,"Otevrit soubor s novym objektem","/"));
	//QString filename = QFileDialog::getOpenFileName(this,"Otevrit soubor s novym objektem","/");
	//osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(filename.toStdString());
	osg::ref_ptr<osgDB::Options> options = new osgDB::Options();
	options->setOptionString("noTriStripPolygons noTesselateLargePolygons");
	//options->("noTesselateLargePolygons");
	//noTesselateLargePolygons
	osg::Node * loadedNode = osgDB::readNodeFile(filename.toStdString(),options);
	if(loadedNode!=NULL)
	{
		osgUtil::Optimizer * opt =  NULL;
		if (optimize)
		{
			opt = new osgUtil::Optimizer();
			opt->optimize(loadedNode,osgUtil::Optimizer::OptimizationOptions::INDEX_MESH | osgUtil::Optimizer::OptimizationOptions::VERTEX_PRETRANSFORM | osgUtil::Optimizer::OptimizationOptions::VERTEX_POSTTRANSFORM);
		}
		
		sceneTreeContainer->appendNodeToInactive(loadedNode);
		appendToStateSetTable(loadedNode);
		shaderTabContainer->appendFromTree(loadedNode);
		if (opt)
		{
			delete opt;
		}

	}
	//ui.sceneTreeWidget->insertNewSubtree(loadedNode);
	//rebuildSceneTree();
	//ui.osgGLWidget->forcePaintGL();
}

void shader_editor::saveMainSceneNode()
{
	//osgDB::FileNameList plugins = osgDB::listAllAvailablePlugins(); 
	QFileDialog saveFileDialog(this,"Uložit aktuální zobrazený node","/","ASCII node file (*.osg);;Binary node file (*.ive)");
	//QString filename = QFileDialog::getSaveFileName(this,"Uložit aktuální zobrazený node","/","ASCII node file (*.osg);;Binary node file (*.ive)");
	//osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(filename.toStdString());
	QString filename;
	saveFileDialog.setAcceptMode(QFileDialog::AcceptSave);
	if (saveFileDialog.exec())
	{
		filename = saveFileDialog.selectedFiles().value(0);
	}
	else
		return;
	QDir::setCurrent(saveFileDialog.directory().dirName());
	osg::Node * nodeToSave = ui.osgGLWidget->getSceneData();
// 	osg::ref_ptr<osgDB::Options> saveOptions;
// 	saveOptions->
	bool retval = osgDB::writeNodeFile(*nodeToSave,filename.toStdString());
	retval = retval;
	QDir::setCurrent("/");
}

void shader_editor::SA_Program_SetCurrentProgram()
{
	stateAttributeContainer->setProgram(shaderTabContainer->getCurrentProgram());
	shaderTabContainer->updateProgramTableParentAndType();
}

void shader_editor::clearScene()
{
	sceneTreeContainer->clear();
}

void shader_editor::makeGLWidgetFloating( bool checked )
{
	if (checked)
	{
		ui.osgTabLayout->removeWidget(ui.osgGLWidget);
		ui.osgGLWidget->setParent(0);
		ui.osgGLWidget->show();
		//ui.osgTabLayout->removeWidget(ui.osgGLWidget);
	}
	else
	{
		ui.osgGLWidget->setParent(ui.mainTab_ObjectView);
		ui.osgTabLayout->addWidget(ui.osgGLWidget, 0, 0, 1, 1);
		ui.osgGLWidget->show();
	}
}


void shader_editor::setCamera1()
{
	ui.osgGLWidget->setCamera(osg::Vec3d(0.0,0.0,5.0), osg::Vec3d(0.0,0.0,0.0), osg::Vec3d(0.0,0.0,1.0));
	writeToConsole(osg::INFO, "camera set to osg::Vec3d(0.0,0.0,5.0), osg::Vec3d(0.0,0.0,0.0), osg::Vec3d(0.0,0.0,1.0)");
}

void shader_editor::setCamera2()
{
	ui.osgGLWidget->setCamera(osg::Vec3d(0.0,5.0,5.0), osg::Vec3d(0.0,0.0,0.0), osg::Vec3d(0.0,0.0,1.0));
	writeToConsole(osg::INFO, "camera set to osg::Vec3d(0.0,5.0,5.0), osg::Vec3d(0.0,0.0,0.0), osg::Vec3d(0.0,0.0,1.0)");
}

void shader_editor::setCamera3()
{
	ui.osgGLWidget->setCamera(osg::Vec3d(40.0,20.0,10.0), osg::Vec3d(0.0,0.0,0.0), osg::Vec3d(0.0,0.0,1.0));
	writeToConsole(osg::INFO, "camera set to osg::Vec3d(40.0,20.0,10.0), osg::Vec3d(0.0,0.0,0.0), osg::Vec3d(0.0,0.0,1.0)");
}

void shader_editor::setCamera4()
{
	ui.osgGLWidget->setCamera(osg::Vec3d(50.0,20.0,10.0), osg::Vec3d(0.0,0.0,0.0), osg::Vec3d(0.0,0.0,1.0));
	writeToConsole(osg::INFO, "camera set to osg::Vec3d(50.0,20.0,10.0), osg::Vec3d(0.0,0.0,0.0), osg::Vec3d(0.0,0.0,1.0)");
}

const osg::Vec3d camera_positions[] = {
	osg::Vec3d(0.0,0.0,5.0),
	osg::Vec3d(0.0,5.0,5.0),
	osg::Vec3d(0.0,5.0,0.0),
	osg::Vec3d(5.0,0.0,0.0),
	osg::Vec3d(10.0,0.0,0.0),
	osg::Vec3d(15.0,0.0,0.0),
	osg::Vec3d(20.0,0.0,0.0),
	osg::Vec3d(30.0,0.0,0.0),
	osg::Vec3d(40.0,0.0,0.0),
	osg::Vec3d(50.0,0.0,0.0),
	osg::Vec3d(0.0,10.0,10.0)
};

void shader_editor::setCameraMapped(int i)
{
	if(i >= sizeof(camera_positions)/sizeof(camera_positions[0]))
		return;
	ui.osgGLWidget->setCamera(camera_positions[i], osg::Vec3d(0.0,0.0,0.0), osg::Vec3d(0.0,0.0,1.0));
	writeToConsole(osg::INFO, QString("camera set to osg::Vec3d(%1,%2,%3), osg::Vec3d(0.0,0.0,0.0), osg::Vec3d(0.0,0.0,1.0)").arg(camera_positions[i].x())
		.arg(camera_positions[i].y())
		.arg(camera_positions[i].z()).toStdString().c_str());
}

void shader_editor::optimizeDemos( osg::Node * target )
{
	if (ui.actionOptimalizovat_dema->isChecked() == false)
	{
		return;
	}
	osgUtil::Optimizer * opt = new osgUtil::Optimizer();
	opt->optimize(target,osgUtil::Optimizer::OptimizationOptions::INDEX_MESH | osgUtil::Optimizer::OptimizationOptions::VERTEX_PRETRANSFORM | osgUtil::Optimizer::OptimizationOptions::VERTEX_POSTTRANSFORM);
	delete opt;
}








