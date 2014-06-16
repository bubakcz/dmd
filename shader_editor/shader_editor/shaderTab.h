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
    Správce shaderù. udržuje aktuálnì vypnuté shadery, tvoøí nové, ukládá a naèítá
    
    TODO: vyhledání uniformù    
*/

#pragma once
#include <QObject>
#include "shader_editor.h"
#include "osgUniversalTableWidget.h"

struct loadedProgram
{
	osg::ref_ptr<osg::Program> program_itself;
	std::vector<osg::ref_ptr<osg::Shader> > unused;
	unsigned tess;
	unsigned geometry;
};


class shaderTab :
	public QObject
{
	Q_OBJECT
public:
	shaderTab(shader_editor * parent);
	~shaderTab(void);
	void buildProgramList();
	void updateProgramTableParentAndType();
	osg::ref_ptr<osg::Program> loadShaderProgram(QString filename); 
	osg::ref_ptr<osg::Program> getCurrentProgram();
	osg::ref_ptr<osg::Program> getCurrentProgram(int row);
	void appendFromTree(osg::Node * currentNode, std::vector<loadedProgram> * nodeList = 0);
	static osg::Program * buildProgramFromFiles(QString VS, QString TCS, QString TES, QString GS, QString FS);
protected: 
	shader_editor * parent;
	std::vector<QString> vertexShaderList;
	std::vector<QString> tessControlShaderList;
	std::vector<QString> tessEvalShaderList;
	std::vector<QString> geometryShaderList;
	std::vector<QString> fragmentShaderList;


	void clearShaderTable();

	loadedProgram loadShaderProgramInner(QString filename); 

	void saveCurrentText(std::vector<QString> &targetList, QPlainTextEdit * sourceEdit);

	void loadBindings(osg::ref_ptr<osg::Program> currentProgram);




	loadedProgram parseNodeAsLoadedProgramStructure(osg::Group * actualNode, bool * ok = NULL );
	loadedProgram parseProgramAsLoadedProgramStructure(osg::Program * actualProgram, bool * ok = NULL );
	

	void appendToTable(std::vector<loadedProgram> listOfPrograms);
	void appendToTable(loadedProgram loaderProgramStructure) { std::vector<loadedProgram> v; v.push_back(loaderProgramStructure); appendToTable(v);};
	void appendToTable(osg::Program * osgProgram) { appendToTable(parseProgramAsLoadedProgramStructure(osgProgram));};
	std::vector<osgUniversalTableWidget> getSelectedRows();
	void clearAll()
	{
		vertexShaderList.clear();
		tessControlShaderList.clear();
		tessEvalShaderList.clear();
		geometryShaderList.clear();
		fragmentShaderList.clear();
		parent->ui.listOfShaderPrograms->setRowCount(0);
	};

public slots:
	void hideShaderBox();
	void showShaderBox();
	void createNewProgram();
	void changeCurrentShaderProgram(int row, int column);
	void saveShaderProgramTab();
	void loadShaderProgramTab();
	void useTessStateChanged(int state);
	void useGeometryStateChanged(int state);
	void shaderSaveChanges();
	void shaderResetChanges();

	void loadProgramFromOSGFile(QString filename);
	void saveProgramToOSGFile(QString filename);

	void removeBinding();
	void addBinding();
	void updateBindings();

	void vertexEditChanged();
	void tessControlEditChanged();
	void tessEvalEditChanged();
	void geometryEditChanged();
	void fragmentEditChanged();

	void toggleVisibilityBindings(bool checked);
	void toggleVisibilityUniforms(bool checked);
};

