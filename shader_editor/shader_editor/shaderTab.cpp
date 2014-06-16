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
    viz header    
*/

#include "shaderTab.h"
#include "osgUniversalTableWidget.h"
#include <vector>
#include <osg/Program>
#include <QFileDialog>
#include <QTextStream>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileNameUtils>
#include <QShortcut>
#include <QMessageBox>

shaderTab::shaderTab(shader_editor * parent) : QObject(parent)
{
	this->parent = parent;
	bool retbool = true;

	parent->ui.listOfShaderPrograms->setSelectionBehavior(QAbstractItemView::SelectRows);

	QFontMetrics fm(parent->ui.textEdit_VS->font());
	int tab_length = 4;
	parent->ui.textEdit_VS->setTabStopWidth(tab_length*fm.width(QChar(' ')));
	parent->ui.textEdit_TCS->setTabStopWidth(tab_length*fm.width(QChar(' ')));
	parent->ui.textEdit_TES->setTabStopWidth(tab_length*fm.width(QChar(' ')));
	parent->ui.textEdit_GS->setTabStopWidth(tab_length*fm.width(QChar(' ')));
	parent->ui.textEdit_FS->setTabStopWidth(tab_length*fm.width(QChar(' ')));

	retbool &= connect(parent->ui.hideShaderProgramField, SIGNAL(pressed()),this,SLOT(hideShaderBox()));
	retbool &= connect(parent->ui.showShaderProgramField, SIGNAL(pressed()),this,SLOT(showShaderBox()));
	retbool &= connect(parent->ui.saveProgram, SIGNAL(pressed()),this,SLOT(saveShaderProgramTab()));
	retbool &= connect(parent->ui.loadProgram, SIGNAL(pressed()),this,SLOT(loadShaderProgramTab()));
	retbool &= connect(parent->ui.listOfShaderPrograms, SIGNAL(cellClicked(int, int)),this,SLOT(changeCurrentShaderProgram(int, int)));
	retbool &= connect(parent->ui.createShaderProgram, SIGNAL(pressed()),this,SLOT(createNewProgram()));

	QShortcut * f5 = new QShortcut(QKeySequence(Qt::Key_F5),parent->ui.mainTab_ShaderEditor);
	retbool &= connect(f5,SIGNAL(activated()),this,SLOT(shaderSaveChanges()));

	retbool &= connect(parent->ui.shaderTab_saveShaderChanges, SIGNAL(pressed()),this,SLOT(shaderSaveChanges()));
	retbool &= connect(parent->ui.shaderTab_resetShaderChanges, SIGNAL(pressed()),this,SLOT(shaderResetChanges()));

	retbool &= connect(parent->ui.useTessShader, SIGNAL(stateChanged(int)),this,SLOT(useTessStateChanged(int)));
	retbool &= connect(parent->ui.useGeometryShader, SIGNAL(stateChanged(int)),this,SLOT(useGeometryStateChanged(int)));

	retbool &= connect(parent->ui.textEdit_VS, SIGNAL(textChanged()),this,SLOT(vertexEditChanged()));
	retbool &= connect(parent->ui.textEdit_TCS, SIGNAL(textChanged()),this,SLOT(tessControlEditChanged()));
	retbool &= connect(parent->ui.textEdit_TES, SIGNAL(textChanged()),this,SLOT(tessEvalEditChanged()));
	retbool &= connect(parent->ui.textEdit_GS, SIGNAL(textChanged()),this,SLOT(geometryEditChanged()));
	retbool &= connect(parent->ui.textEdit_FS, SIGNAL(textChanged()),this,SLOT(fragmentEditChanged()));

	retbool &= connect(parent->ui.bindingsRegion_ShowHide, SIGNAL(toggled(bool)), this, SLOT(toggleVisibilityBindings(bool)));
	retbool &= connect(parent->ui.uniformRegion_ShowHide, SIGNAL(toggled(bool)), this, SLOT(toggleVisibilityUniforms(bool)));

	retbool &= connect(parent->ui.addVABinding, SIGNAL(pressed()), this, SLOT(addBinding()));
	retbool &= connect(parent->ui.removeVABinding, SIGNAL(pressed()), this, SLOT(removeBinding()));
	retbool &= connect(parent->ui.updateBindings, SIGNAL(pressed()), this, SLOT(updateBindings()));

	parent->ui.uniformRegion_ShowHide->setChecked(false);
}


shaderTab::~shaderTab(void)
{
}


osg::ref_ptr<osg::Program> shaderTab::getCurrentProgram(int row)
{
	osgUniversalTableWidget * prog_table_item = static_cast<osgUniversalTableWidget *>(parent->ui.listOfShaderPrograms->item(row,0));
	if (prog_table_item==NULL)
		return NULL;
	return prog_table_item->this_program;
}

osg::ref_ptr<osg::Program> shaderTab::getCurrentProgram()
{
	return getCurrentProgram(parent->ui.listOfShaderPrograms->currentRow());
}


void shaderTab::hideShaderBox()
{
	parent->ui.shaderProgramWidget->hide();
	parent->ui.showShaderProgramField->show();
}

void shaderTab::showShaderBox()
{
	parent->ui.shaderProgramWidget->show();
	parent->ui.showShaderProgramField->hide();
}

void shaderTab::appendToTable( std::vector<loadedProgram> listOfPrograms )
{
	int originalRowCount = parent->ui.listOfShaderPrograms->rowCount();
	vertexShaderList.resize(vertexShaderList.size() + listOfPrograms.size());
	tessControlShaderList.resize(tessControlShaderList.size() + listOfPrograms.size());
	tessEvalShaderList.resize(tessEvalShaderList.size() + listOfPrograms.size());
	geometryShaderList.resize(geometryShaderList.size() + listOfPrograms.size());
	fragmentShaderList.resize(fragmentShaderList.size() + listOfPrograms.size());
	parent->ui.listOfShaderPrograms->setRowCount(parent->ui.listOfShaderPrograms->rowCount() + listOfPrograms.size());

	std::vector<QString> g_t_list;
	std::vector<QString> p_name_list;
	std::vector<QString> ss_count_list;
	g_t_list.resize(listOfPrograms.size());
	p_name_list.resize(listOfPrograms.size());
	ss_count_list.resize(listOfPrograms.size());

	for (unsigned i = 0; i < listOfPrograms.size(); i++)
	{
		if (listOfPrograms[i].program_itself->getName().empty())
			p_name_list[i] = QString("undefined");
		else
			p_name_list[i] = QString::fromStdString(listOfPrograms[i].program_itself->getName());

		ss_count_list[i] = QString("%1").arg(listOfPrograms[i].program_itself->getNumParents());

		unsigned vertex = 0;
		unsigned tesscontrol = 0;
		unsigned tesseval = 0;
		unsigned geometry = 0;
		unsigned fragment = 0;
		for (unsigned k = 0; k < listOfPrograms[i].program_itself->getNumShaders(); k++)
		{
			switch(listOfPrograms[i].program_itself->getShader(k)->getType())
			{
			case osg::Shader::VERTEX: vertex++; vertexShaderList[originalRowCount+i] = QString::fromStdString(listOfPrograms[i].program_itself->getShader(k)->getShaderSource()); break;
			case osg::Shader::TESSCONTROL: tesscontrol++; tessControlShaderList[originalRowCount+i] = QString::fromStdString(listOfPrograms[i].program_itself->getShader(k)->getShaderSource()); break;
			case osg::Shader::TESSEVALUATION: tesseval++; tessEvalShaderList[originalRowCount+i] = QString::fromStdString(listOfPrograms[i].program_itself->getShader(k)->getShaderSource()); break;
			case osg::Shader::GEOMETRY: geometry++; geometryShaderList[originalRowCount+i] = QString::fromStdString(listOfPrograms[i].program_itself->getShader(k)->getShaderSource()); break;
			case osg::Shader::FRAGMENT: fragment++; fragmentShaderList[originalRowCount+i] = QString::fromStdString(listOfPrograms[i].program_itself->getShader(k)->getShaderSource()); break;
			}
		}
		if (tesscontrol == 1 && tesseval == 1)
			g_t_list[i] = "Ano";
		else	
			g_t_list[i] = "Ne";
		if (geometry == 1)
			g_t_list[i] += " / Ano";
		else	
			g_t_list[i] += " / Ne";
	}

	for (unsigned i = originalRowCount, apIt = 0; i < parent->ui.listOfShaderPrograms->rowCount(); i++, apIt++)
	{
		osgUniversalTableWidget * newTableItem = new osgUniversalTableWidget(p_name_list[apIt],listOfPrograms[apIt].program_itself.get());
		parent->ui.listOfShaderPrograms->setItem(i,0,newTableItem);
		/*osgUniversalTableWidget **/ newTableItem = new osgUniversalTableWidget(g_t_list[apIt],listOfPrograms[apIt].program_itself.get());
		parent->ui.listOfShaderPrograms->setItem(i,1,newTableItem);
		/*osgUniversalTableWidget **/ newTableItem = new osgUniversalTableWidget(ss_count_list[apIt],listOfPrograms[apIt].program_itself.get());
		parent->ui.listOfShaderPrograms->setItem(i,2,newTableItem);
	}
}


void shaderTab::updateProgramTableParentAndType()
{
	for (unsigned i = 0; i < parent->ui.listOfShaderPrograms->rowCount(); i++)
	{
		osg::Program * tmp = (dynamic_cast<osgUniversalTableWidget *>(parent->ui.listOfShaderPrograms->item(i,0)))->this_program.get();
		parent->ui.listOfShaderPrograms->item(i,2)->setText(QString("%1").arg(tmp->getNumParents()));
	}
}




void shaderTab::buildProgramList()
{

	clearAll();
	std::vector<osg::ref_ptr<osg::Program>> program_set;
	bool found;


	for (unsigned i = 0; i < parent->ui.stateSetTableWidget->rowCount(); i++)
	{
		osg::StateAttribute * prog = static_cast<osgUniversalTableWidget *>(parent->ui.stateSetTableWidget->item(i,0))->this_stateset->getAttribute(osg::StateAttribute::PROGRAM);

		if (prog!=NULL)
		{
			osg::ref_ptr<osg::Program> act_prog = static_cast<osg::Program *>(prog);
			found = false;
			for (unsigned k = 0; k < program_set.size(); k++)
			{
				if (act_prog == program_set[k])
				{
					found = true;
					break;
				}
			}
			if (!found)
				program_set.push_back(act_prog);
		}
	}

	std::vector<loadedProgram> pV;
	for (int i = 0; i <program_set.size(); i++)
		pV.push_back(parseProgramAsLoadedProgramStructure(program_set[i]));
	appendToTable(pV);
/*
	vertexShaderList.resize(program_set.size());
	tessControlShaderList.resize(program_set.size());
	tessEvalShaderList.resize(program_set.size());
	geometryShaderList.resize(program_set.size());
	fragmentShaderList.resize(program_set.size());
	parent->ui.listOfShaderPrograms->setRowCount(program_set.size());

	std::vector<QString> g_t_list;
	std::vector<QString> p_name_list;
	std::vector<QString> ss_count_list;
	g_t_list.resize(program_set.size());
	p_name_list.resize(program_set.size());
	ss_count_list.resize(program_set.size());

	for (unsigned i = 0; i < program_set.size(); i++)
	{
		if (program_set[i]->getName().empty())
			p_name_list[i] = QString("undefined");
		else
			p_name_list[i] = QString::fromStdString(program_set[i]->getName());

		ss_count_list[i] = QString("%1").arg(program_set[i]->getNumParents());

		unsigned vertex = 0;
		unsigned tesscontrol = 0;
		unsigned tesseval = 0;
		unsigned geometry = 0;
		unsigned fragment = 0;
		for (unsigned k = 0; k < program_set[i]->getNumShaders(); k++)
		{
			switch(program_set[i]->getShader(k)->getType())
			{
				case osg::Shader::VERTEX: vertex++; vertexShaderList[i] = QString::fromStdString(program_set[i]->getShader(k)->getShaderSource()); break;
				case osg::Shader::TESSCONTROL: tesscontrol++; tessControlShaderList[i] = QString::fromStdString(program_set[i]->getShader(k)->getShaderSource()); break;
				case osg::Shader::TESSEVALUATION: tesseval++; tessEvalShaderList[i] = QString::fromStdString(program_set[i]->getShader(k)->getShaderSource()); break;
				case osg::Shader::GEOMETRY: geometry++; geometryShaderList[i] = QString::fromStdString(program_set[i]->getShader(k)->getShaderSource()); break;
				case osg::Shader::FRAGMENT: fragment++; fragmentShaderList[i] = QString::fromStdString(program_set[i]->getShader(k)->getShaderSource()); break;
			}
		}
		if (tesscontrol == 1 && tesseval == 1)
			g_t_list[i] = "Ano";
		else	
			g_t_list[i] = "Ne";
		if (geometry == 1)
			g_t_list[i] += " / Ano";
		else	
			g_t_list[i] += " / Ne";
	}

	for (unsigned i = 0; i < program_set.size(); i++)
	{
		osgUniversalTableWidget * newTableItem = new osgUniversalTableWidget(p_name_list[i],program_set[i].get());
		parent->ui.listOfShaderPrograms->setItem(i,0,newTableItem);
		/ *osgUniversalTableWidget ** / newTableItem = new osgUniversalTableWidget(g_t_list[i],program_set[i].get());
		parent->ui.listOfShaderPrograms->setItem(i,1,newTableItem);
		/ *osgUniversalTableWidget ** / newTableItem = new osgUniversalTableWidget(ss_count_list[i],program_set[i].get());
		parent->ui.listOfShaderPrograms->setItem(i,2,newTableItem);
	}*/
}

const char * default_vertex_shader;

const char * default_pixel_shader;

void shaderTab::createNewProgram()
{
	osg::ref_ptr<osg::Program> n_prog = new osg::Program();

	osg::ref_ptr<osg::Shader> v_shader = new osg::Shader(osg::Shader::VERTEX);
	osg::ref_ptr<osg::Shader> f_shader = new osg::Shader(osg::Shader::FRAGMENT);

	n_prog->addShader(v_shader);
	n_prog->addShader(f_shader);

	parent->ui.listOfShaderPrograms->setRowCount(parent->ui.listOfShaderPrograms->rowCount()+1);

	vertexShaderList.resize(vertexShaderList.size()+1);
	tessControlShaderList.resize(tessControlShaderList.size()+1);
	tessEvalShaderList.resize(tessEvalShaderList.size()+1);
	geometryShaderList.resize(geometryShaderList.size()+1);
	fragmentShaderList.resize(fragmentShaderList.size()+1);

	parent->ui.useTessShader->setChecked(false);
	parent->ui.useGeometryShader->setChecked(false);

	osgUniversalTableWidget * newTableItem = new osgUniversalTableWidget("undefined",n_prog);
	parent->ui.listOfShaderPrograms->setItem(parent->ui.listOfShaderPrograms->rowCount()-1,0,newTableItem);
	/*osgUniversalTableWidget **/ newTableItem = new osgUniversalTableWidget("Ne / Ne",n_prog);
	parent->ui.listOfShaderPrograms->setItem(parent->ui.listOfShaderPrograms->rowCount()-1,1,newTableItem);
	/*osgUniversalTableWidget **/ newTableItem = new osgUniversalTableWidget("0",n_prog);
	parent->ui.listOfShaderPrograms->setItem(parent->ui.listOfShaderPrograms->rowCount()-1,2,newTableItem);
}


void shaderTab::changeCurrentShaderProgram( int row, int column )
{
	osgUniversalTableWidget * prog_table_item = static_cast<osgUniversalTableWidget *>(parent->ui.listOfShaderPrograms->item(row,0));

	osg::ref_ptr<osg::Program> prog = prog_table_item->this_program;
	if (prog == NULL)
		return;

	unsigned vertex = 0;
	unsigned tesscontrol = 0;
	unsigned tesseval = 0;
	unsigned geometry = 0;
	unsigned fragment = 0;
	for (unsigned k = 0; k < prog->getNumShaders(); k++)
	{
		switch(prog->getShader(k)->getType())
		{
		case osg::Shader::VERTEX: vertex++; break;
		case osg::Shader::TESSCONTROL: tesscontrol++; break;
		case osg::Shader::TESSEVALUATION: tesseval++; break;
		case osg::Shader::GEOMETRY: geometry++; break;
		case osg::Shader::FRAGMENT: fragment++; break;
		}
	}

	if (vertex!=1 || fragment!=1 || geometry>1 || tesscontrol!=tesseval || tesseval > 1)
	{
		parent->writeToConsoleQt(QtWarningMsg,QString("Nelze otevøít program - nevalidní struktura - VS:%1 TCS:%2 TES:%3 GS:%4 FS:%5").arg(vertex).arg(tesscontrol).arg(tesseval).arg(geometry).arg(fragment));
		parent->ui.textEdit_VS->clear();
		parent->ui.textEdit_VS->setEnabled(false);
		parent->ui.textEdit_TCS->clear();
		parent->ui.textEdit_TCS->setEnabled(false);
		parent->ui.textEdit_TES->clear();
		parent->ui.textEdit_TES->setEnabled(false);
		parent->ui.textEdit_GS->clear();
		parent->ui.textEdit_GS->setEnabled(false);
		parent->ui.textEdit_FS->clear();
		parent->ui.textEdit_FS->setEnabled(false);
		return;
	}
	parent->ui.textEdit_VS->setEnabled(true);
	parent->ui.textEdit_TCS->setEnabled(true);
	parent->ui.textEdit_TES->setEnabled(true);
	parent->ui.textEdit_GS->setEnabled(true);
	parent->ui.textEdit_FS->setEnabled(true);

	if (!geometry && prog_table_item->this_geom_backup!=NULL)
		parent->ui.textEdit_GS->setPlainText(QString::fromStdString(prog_table_item->this_geom_backup->getShaderSource()));
	if (!tesscontrol && prog_table_item->this_geom_backup!=NULL)
	{
		parent->ui.textEdit_GS->setPlainText(QString::fromStdString(prog_table_item->this_tess_backup[osgUniversalTableWidget::CONTROL]->getShaderSource()));
		parent->ui.textEdit_TES->setPlainText(QString::fromStdString(prog_table_item->this_tess_backup[osgUniversalTableWidget::EVALUATION]->getShaderSource()));
	}

	if (tesscontrol && tesseval)
		parent->ui.useTessShader->setChecked(true);
	else	
		parent->ui.useTessShader->setChecked(false);
	if (geometry)
		parent->ui.useGeometryShader->setChecked(true);
	else	
		parent->ui.useGeometryShader->setChecked(false);

	for (unsigned k = 0; k < prog->getNumShaders(); k++)
	{
		switch(prog->getShader(k)->getType())
		{
		case osg::Shader::VERTEX: parent->ui.textEdit_VS->setPlainText(vertexShaderList[row]); break;
		case osg::Shader::TESSCONTROL: parent->ui.textEdit_TCS->setPlainText(tessControlShaderList[row]); break;
		case osg::Shader::TESSEVALUATION: parent->ui.textEdit_TES->setPlainText(tessEvalShaderList[row]); break;
		case osg::Shader::GEOMETRY: parent->ui.textEdit_GS->setPlainText(geometryShaderList[row]); break;
		case osg::Shader::FRAGMENT: parent->ui.textEdit_FS->setPlainText(fragmentShaderList[row]); break;
/*
		case osg::Shader::TESSCONTROL: parent->ui.textEdit_TCS->setText(QString::fromStdString(prog->getShader(k)->getShaderSource())); break;
		case osg::Shader::TESSEVALUATION: parent->ui.textEdit_TES->setText(QString::fromStdString(prog->getShader(k)->getShaderSource())); break;
		case osg::Shader::GEOMETRY: parent->ui.textEdit_GS->setText(QString::fromStdString(prog->getShader(k)->getShaderSource())); break;
		case osg::Shader::FRAGMENT: parent->ui.textEdit_FS->setText(QString::fromStdString(prog->getShader(k)->getShaderSource())); break;*/
		}
	}

	loadBindings(prog);
	
}

void shaderTab::saveShaderProgramTab()
{
	int c_row = parent->ui.listOfShaderPrograms->currentRow();

	if (c_row<0)
		return;
	osgUniversalTableWidget * prog_table_item = static_cast<osgUniversalTableWidget *>(parent->ui.listOfShaderPrograms->item(c_row,0));

	osg::ref_ptr<osg::Program> prog = prog_table_item->this_program;
	if (prog == NULL)
		return;
	QString fileName = QFileDialog::getSaveFileName(parent, tr("Uložit shader program"),
		"/",
		tr("Soubor s programem (*.osg)"));
	if (fileName=="")
		return;

	saveProgramToOSGFile(fileName);
	return;
/*	QFile outFileBase(fileName);
	QTextStream outFile;
	if (outFileBase.open(QFile::WriteOnly | QFile::Truncate)) {
		outFile.setDevice(&outFileBase);
	}
	else return;


// 	osgUniversalTableWidget * prog_table_item = static_cast<osgUniversalTableWidget *>(parent->ui.listOfShaderPrograms->item(c_row,0));
// 
// 	osg::ref_ptr<osg::Program> prog = prog_table_item->this_program;
// 	if (prog == NULL)
// 		return;


	unsigned vertex = 0;
	unsigned tesscontrol = 0;
	unsigned tesseval = 0;
	unsigned geometry = 0;
	unsigned fragment = 0;

	for (unsigned k = 0; k < prog->getNumShaders(); k++)
	{
		switch(prog->getShader(k)->getType())
		{
		case osg::Shader::VERTEX: vertex++; break;
		case osg::Shader::TESSCONTROL: tesscontrol++; break;
		case osg::Shader::TESSEVALUATION: tesseval++; break;
		case osg::Shader::GEOMETRY: geometry++; break;
		case osg::Shader::FRAGMENT: fragment++; break;
		}
	}

	outFile << "ShaderProgram\n";
	if (vertex!=1 || fragment!=1 || geometry>1 || tesscontrol!=tesseval || tesseval > 1)
	{
		parent->writeToConsoleQt(QtWarningMsg,QString("Nevalidní struktura programu - VS:%1 TCS:%2 TES:%3 GS:%4 FS:%5\nProvádí se prostý výpis").arg(vertex).arg(tesscontrol).arg(tesseval).arg(geometry).arg(fragment));
		return;
		outFile << "valid = no\n";
		outFile << "tesselation = no\n";
		outFile << "geometry = no\n";
		outFile << "name = " << QString::fromStdString(prog->getName()) << "\n";
		for (unsigned i = 0; i <prog->getNumShaders(); i++)
		{
			QString act_sh_prog = QString::fromStdString(prog->getShader(i)->getShaderSource());
			QStringList act_sh_prog_string_list = act_sh_prog.split("\n");
			switch(prog->getShader(i)->getType())
			{
				case osg::Shader::VERTEX: outFile << "VERTEX\n"; break;
				case osg::Shader::TESSCONTROL: outFile << "TESSCONTROL\n"; break;
				case osg::Shader::TESSEVALUATION: outFile << "TESSEVALUATION\n"; break;
				case osg::Shader::GEOMETRY: outFile << "GEOMETRY\n"; break;
				case osg::Shader::FRAGMENT: outFile << "FRAGMENT\n"; break;
			}
			outFile << QString("lines = %1\n").arg(act_sh_prog_string_list.size());
			for (unsigned k = 0; k < act_sh_prog_string_list.size(); k++)
				outFile << act_sh_prog_string_list[k] << "\n";
		}
	}
	else
	{
		outFile << "valid = yes\n";
		if (parent->ui.useTessShader)
			outFile << "tesselation = yes\n";
		else
			outFile << "tesselation = no\n";
		if (parent->ui.useGeometryShader)
			outFile << "geometry = yes\n";
		else
			outFile << "geometry = no\n";

		outFile << "name = " << QString::fromStdString(prog->getName()) << "\n";

		QString act_sh_prog;
		QStringList act_sh_prog_string_list;

		act_sh_prog = parent->ui.textEdit_VS->toPlainText();
		act_sh_prog_string_list = act_sh_prog.split("\n");
		outFile << "VERTEX\n";
		outFile << QString("lines = %1\n").arg(act_sh_prog_string_list.size());
		for (unsigned k = 0; k < act_sh_prog_string_list.size(); k++)
			outFile << act_sh_prog_string_list[k] << "\n";

		act_sh_prog = parent->ui.textEdit_TCS->toPlainText();
		act_sh_prog_string_list = act_sh_prog.split("\n");
		outFile << "TESSCONTROL\n";
		outFile << QString("lines = %1\n").arg(act_sh_prog_string_list.size());
		for (unsigned k = 0; k < act_sh_prog_string_list.size(); k++)
			outFile << act_sh_prog_string_list[k] << "\n";

		act_sh_prog = parent->ui.textEdit_TES->toPlainText();
		act_sh_prog_string_list = act_sh_prog.split("\n");
		outFile << "TESSEVALUATION\n";
		outFile << QString("lines = %1\n").arg(act_sh_prog_string_list.size());
		for (unsigned k = 0; k < act_sh_prog_string_list.size(); k++)
			outFile << act_sh_prog_string_list[k] << "\n";

		act_sh_prog = parent->ui.textEdit_GS->toPlainText();
		act_sh_prog_string_list = act_sh_prog.split("\n");
		outFile << "GEOMETRY\n";
		outFile << QString("lines = %1\n").arg(act_sh_prog_string_list.size());
		for (unsigned k = 0; k < act_sh_prog_string_list.size(); k++)
			outFile << act_sh_prog_string_list[k] << "\n";

		act_sh_prog = parent->ui.textEdit_FS->toPlainText();
		act_sh_prog_string_list = act_sh_prog.split("\n");
		outFile << "FRAGMENT\n";
		outFile << QString("lines = %1\n").arg(act_sh_prog_string_list.size());
		for (unsigned k = 0; k < act_sh_prog_string_list.size(); k++)
			outFile << act_sh_prog_string_list[k] << "\n";
	}




/ *
unsigned vertex = 0;
unsigned tesscontrol = 0;
unsigned tesseval = 0;
unsigned geometry = 0;
unsigned fragment = 0;

	for (unsigned k = 0; k < prog->getNumShaders(); k++)
	{
		switch(prog->getShader(k)->getType())
		{
		case osg::Shader::VERTEX: vertex++; break;
		case osg::Shader::TESSCONTROL: tesscontrol++; break;
		case osg::Shader::TESSEVALUATION: tesseval++; break;
		case osg::Shader::GEOMETRY: geometry++; break;
		case osg::Shader::FRAGMENT: fragment++; break;
		}
	}* /

/ *
	if (vertex!=1 || fragment!=1 || geometry>1 || tesscontrol!=tesseval || tesseval > 1)
	{
		parent->writeToConsoleQt(QtWarningMsg,QString("Nelze otevøít program - nevalidní struktura - VS:%1 TCS:%2 TES:%3 GS:%4 FS:%5").arg(vertex).arg(tesscontrol).arg(tesseval).arg(geometry).arg(fragment));
		return;
	}* /

/ *
	if (tesscontrol && tesseval)
		parent->ui.useTessShader->setChecked(true);
	if (geometry)
		parent->ui.useGeometryShader->setChecked(true);* /

	for (unsigned k = 0; k < prog->getNumShaders(); k++)
	{
		switch(prog->getShader(k)->getType())
		{
		case osg::Shader::VERTEX: parent->ui.textEdit_VS->setText(QString::fromStdString(prog->getShader(k)->getShaderSource())); break;
		case osg::Shader::TESSCONTROL: parent->ui.textEdit_TCS->setText(QString::fromStdString(prog->getShader(k)->getShaderSource())); break;
		case osg::Shader::TESSEVALUATION: parent->ui.textEdit_TES->setText(QString::fromStdString(prog->getShader(k)->getShaderSource())); break;
		case osg::Shader::GEOMETRY: parent->ui.textEdit_GS->setText(QString::fromStdString(prog->getShader(k)->getShaderSource())); break;
		case osg::Shader::FRAGMENT: parent->ui.textEdit_FS->setText(QString::fromStdString(prog->getShader(k)->getShaderSource())); break;
		}
	}*/

}

void shaderTab::loadShaderProgramTab()
{
	QString fileName = QFileDialog::getOpenFileName(parent, tr("Naèíst shader program"),
		"/",
		tr("Soubor s programem (*.osg)"));
/*
	QFile inFileBase(fileName);
	QTextStream inFile;
	if (inFileBase.open(QFile::ReadOnly | QFile::Truncate)) {
		inFile.setDevice(&inFileBase);
	}
	else return;
	/ *outFile << "valid = no\n";
	outFile << "tesselation : no\n";
	outFile << "geometry : no\n";
	outFile << "name = " << prog->getName() << "\n";* /
	QString header = inFile.readLine(0); //ShaderProgram
	QString is_valid = inFile.readLine(0);
	QStringList is_valid_list = is_valid.split(" ");
	QString is_tess = inFile.readLine(0);
	QStringList is_tess_list = is_tess.split(" ");
	QString is_geom = inFile.readLine(0);
	QStringList is_geom_list = is_geom.split(" ");
	QString prog_name = inFile.readLine(0);
	QStringList prog_name_list = prog_name.split(" ");
	if (header!="ShaderProgram" || is_valid_list.size() != 3 || is_tess_list.size()!=3 || is_geom_list.size() !=3 || prog_name_list.size() !=3 ||
		is_valid_list[0] != "valid" || is_tess_list[0] != "tesselation" || is_geom_list[0] != "geometry" || 
		is_valid_list[1] != "=" || is_tess_list[1] != "=" ||  is_geom_list[1] != "=" ||
		(is_valid_list[2] != "yes" && is_valid_list[2] != "no") || 
		(is_tess_list[2] != "yes" && is_tess_list[2] != "no") ||
		(is_geom_list[2] != "yes" && is_geom_list[2] != "no") ||
		(prog_name_list[0] != "name" || prog_name_list[1] != "="))
		return;
	osg::ref_ptr<osg::Program> prog  = new osg::Program();
	prog->setName(prog_name_list[2].toStdString());

	unsigned vertex = 0;
	unsigned tesscontrol = 0;
	unsigned tesseval = 0;
	unsigned geometry = 0;
	unsigned fragment = 0;

	while (!inFile.atEnd())
	{
		QString sh_type = inFile.readLine(0);
		QString sh_lines = inFile.readLine(0);
		QStringList sh_lines_parse_list = sh_lines.split(" ");
		unsigned lines_count;
		if (sh_lines_parse_list[0]!="lines" || sh_lines_parse_list[1]!="=" || sh_lines_parse_list.size()!=3)
			return;
		else
		{
			bool test_touint;
			lines_count = sh_lines_parse_list[2].toUInt(&test_touint);
			if(!test_touint)
				return;
		}

		QString shader_content;

		for (unsigned i = 0; i<lines_count; i++)
			shader_content += inFile.readLine(0) + "\n";

		osg::ref_ptr<osg::Shader> this_shader;

		if(sh_type== "VERTEX")	{this_shader = new osg::Shader(osg::Shader::VERTEX); vertex++; }
		if(sh_type== "TESSCONTROL")	{this_shader = new osg::Shader(osg::Shader::TESSCONTROL); tesscontrol++; }
		if(sh_type== "TESSEVALUATION")	{this_shader = new osg::Shader(osg::Shader::TESSEVALUATION); tesseval++; }
		if(sh_type== "GEOMETRY")	{this_shader = new osg::Shader(osg::Shader::GEOMETRY); geometry++; }
		if(sh_type== "FRAGMENT")	{this_shader = new osg::Shader(osg::Shader::FRAGMENT); fragment++; }

		this_shader->setShaderSource(shader_content.toStdString());
		prog->addShader(this_shader);
	}*/

	/*loadedProgram ret_program =*/ loadProgramFromOSGFile(fileName);
/*
	if (ret_program.program_itself == NULL)
		return;
	appendToTable(ret_program);*/
/*
	osg::ref_ptr<osg::Program> prog = ret_program.program_itself;

	osg::ref_ptr<osg::Shader> savedGeometry = NULL;
	osg::ref_ptr<osg::Shader> savedControl = NULL;
	osg::ref_ptr<osg::Shader> savedEval = NULL;

	for (unsigned i = 0; i < ret_program.unused.size(); i++)
	{
		switch(ret_program.unused[i]->getType())
		{
		case osg::Shader::TESSCONTROL: savedControl=ret_program.unused[i]; break;
		case osg::Shader::TESSEVALUATION: savedEval=ret_program.unused[i]; break;
		case osg::Shader::GEOMETRY: savedGeometry=ret_program.unused[i]; break;
		}
	}

	parent->ui.listOfShaderPrograms->setRowCount(parent->ui.listOfShaderPrograms->rowCount()+1);
	QString g_t_ident_string;
	if (ret_program.tess)
		g_t_ident_string = "Ano";
	else	
		g_t_ident_string = "Ne";
	if (ret_program.geometry)
		g_t_ident_string += " / Ano";
	else	
		g_t_ident_string += " / Ne";

	osgUniversalTableWidget * newTableItem = new osgUniversalTableWidget("undefined",prog);
	newTableItem->this_geom_backup = savedGeometry;
	newTableItem->this_tess_backup[osgUniversalTableWidget::EVALUATION] = savedEval;
	newTableItem->this_tess_backup[osgUniversalTableWidget::CONTROL] = savedControl;
	parent->ui.listOfShaderPrograms->setItem(parent->ui.listOfShaderPrograms->rowCount()-1,0,newTableItem);
	/ *osgUniversalTableWidget ** / newTableItem = new osgUniversalTableWidget(g_t_ident_string,prog);
	newTableItem->this_geom_backup = savedGeometry;
	newTableItem->this_tess_backup[osgUniversalTableWidget::EVALUATION] = savedEval;
	newTableItem->this_tess_backup[osgUniversalTableWidget::CONTROL] = savedControl;
	parent->ui.listOfShaderPrograms->setItem(parent->ui.listOfShaderPrograms->rowCount()-1,1,newTableItem);
	/ *osgUniversalTableWidget ** / newTableItem = new osgUniversalTableWidget("0",prog);
	newTableItem->this_geom_backup = savedGeometry;
	newTableItem->this_tess_backup[osgUniversalTableWidget::EVALUATION] = savedEval;
	newTableItem->this_tess_backup[osgUniversalTableWidget::CONTROL] = savedControl;
	parent->ui.listOfShaderPrograms->setItem(parent->ui.listOfShaderPrograms->rowCount()-1,2,newTableItem);*/

}

void shaderTab::useTessStateChanged( int state )
{
	int c_row = parent->ui.listOfShaderPrograms->currentRow();

	if (c_row<0)
		return;
	osgUniversalTableWidget * prog_table_item = static_cast<osgUniversalTableWidget *>(parent->ui.listOfShaderPrograms->item(c_row,0));
	osg::ref_ptr<osg::Program> prog = prog_table_item->this_program;
	if (prog == NULL)
		return;

	if (state == false)
	{
		std::vector<osg::ref_ptr<osg::Shader> > to_delete;
		for (unsigned i = 0; i < prog->getNumShaders(); i++)
		{
			if (prog->getShader(i)->getType() == osg::Shader::TESSCONTROL)
			{
				to_delete.push_back(prog->getShader(i));
				prog_table_item->this_tess_backup[osgUniversalTableWidget::CONTROL] = prog->getShader(i);
			}
			else if (prog->getShader(i)->getType() == osg::Shader::TESSEVALUATION)
			{
				to_delete.push_back(prog->getShader(i));
				prog_table_item->this_tess_backup[osgUniversalTableWidget::EVALUATION] = prog->getShader(i);
			}
		}

		for (unsigned i = 0; i < to_delete.size(); i++)
			prog->removeShader(to_delete[i]);
	}
	else	
	{
		prog->addShader(prog_table_item->this_tess_backup[osgUniversalTableWidget::CONTROL]);
		prog->addShader(prog_table_item->this_tess_backup[osgUniversalTableWidget::EVALUATION]);
	}
}

void shaderTab::useGeometryStateChanged( int state )
{
	int c_row = parent->ui.listOfShaderPrograms->currentRow();

	if (c_row<0)
		return;
	osgUniversalTableWidget * prog_table_item = static_cast<osgUniversalTableWidget *>(parent->ui.listOfShaderPrograms->item(c_row,0));
	osg::ref_ptr<osg::Program> prog = prog_table_item->this_program;
	if (prog == NULL)
		return;

	if (state == false)
	{
		std::vector<osg::ref_ptr<osg::Shader> > to_delete;
		for (unsigned i = 0; i < prog->getNumShaders(); i++)
		{
			if (prog->getShader(i)->getType() == osg::Shader::GEOMETRY)
			{
				to_delete.push_back(prog->getShader(i));
				prog_table_item->this_geom_backup = prog->getShader(i);
			}
		}

		for (unsigned i = 0; i < to_delete.size(); i++)
			prog->removeShader(to_delete[i]);
	}
	else	
		prog->addShader(prog_table_item->this_geom_backup);
}

loadedProgram shaderTab::loadShaderProgramInner( QString filename )
{
	loadedProgram this_program;
	this_program.program_itself = NULL;

	QFile inFileBase(filename);
	QTextStream inFile;
	if (inFileBase.open(QFile::ReadOnly | QFile::Truncate)) {
		inFile.setDevice(&inFileBase);
	}
	else return this_program;
	/*outFile << "valid = no\n";
	outFile << "tesselation : no\n";
	outFile << "geometry : no\n";
	outFile << "name = " << prog->getName() << "\n";*/
	QString header = inFile.readLine(0); //ShaderProgram
	QString is_valid = inFile.readLine(0);
	QStringList is_valid_list = is_valid.split(" ");
	QString is_tess = inFile.readLine(0);
	QStringList is_tess_list = is_tess.split(" ");
	QString is_geom = inFile.readLine(0);
	QStringList is_geom_list = is_geom.split(" ");
	QString prog_name = inFile.readLine(0);
	QStringList prog_name_list = prog_name.split(" ");
	if (header!="ShaderProgram" || is_valid_list.size() != 3 || is_tess_list.size()!=3 || is_geom_list.size() !=3 || prog_name_list.size() !=3 ||
		is_valid_list[0] != "valid" || is_tess_list[0] != "tesselation" || is_geom_list[0] != "geometry" || 
		is_valid_list[1] != "=" || is_tess_list[1] != "=" ||  is_geom_list[1] != "=" ||
		(is_valid_list[2] != "yes" && is_valid_list[2] != "no") || 
		(is_tess_list[2] != "yes" && is_tess_list[2] != "no") ||
		(is_geom_list[2] != "yes" && is_geom_list[2] != "no") ||
		(prog_name_list[0] != "name" || prog_name_list[1] != "="))
		return this_program;
	osg::ref_ptr<osg::Program> prog  = new osg::Program();
	prog->setName(prog_name_list[2].toStdString());

	unsigned vertex = 0;
	unsigned tesscontrol = 0;
	unsigned tesseval = 0;
	unsigned geometry = 0;
	unsigned fragment = 0;

	while (!inFile.atEnd())
	{
		QString sh_type = inFile.readLine(0);
		QString sh_lines = inFile.readLine(0);
		QStringList sh_lines_parse_list = sh_lines.split(" ");
		unsigned lines_count;
		if (sh_lines_parse_list[0]!="lines" || sh_lines_parse_list[1]!="=" || sh_lines_parse_list.size()!=3)
			return this_program;
		else
		{
			bool test_touint;
			lines_count = sh_lines_parse_list[2].toUInt(&test_touint);
			if(!test_touint)
				return this_program;
		}

		QString shader_content;

		for (unsigned i = 0; i<lines_count; i++)
			shader_content += inFile.readLine(0) + "\n";

		osg::ref_ptr<osg::Shader> this_shader;

		bool side = false;
		if(sh_type== "VERTEX")	{this_shader = new osg::Shader(osg::Shader::VERTEX); vertex++; side=false;}
		if(sh_type== "TESSCONTROL")	{this_shader = new osg::Shader(osg::Shader::TESSCONTROL); tesscontrol++; side = is_tess_list[2] != "yes" && is_valid_list[2] == "yes";}
		if(sh_type== "TESSEVALUATION")	{this_shader = new osg::Shader(osg::Shader::TESSEVALUATION); tesseval++; side = is_tess_list[2] != "yes" && is_valid_list[2] == "yes";}
		if(sh_type== "GEOMETRY")	{this_shader = new osg::Shader(osg::Shader::GEOMETRY); geometry++; side = is_geom_list[2] != "yes" && is_valid_list[2] == "yes";}
		if(sh_type== "FRAGMENT")	{this_shader = new osg::Shader(osg::Shader::FRAGMENT); fragment++; side=false;}

		this_shader->setShaderSource(shader_content.toStdString());

		if (side)
			this_program.unused.push_back(this_shader);
		else
			prog->addShader(this_shader);
	}
	this_program.tess = tesseval;
	this_program.geometry = geometry;
	this_program.program_itself = prog;
	return this_program;
}

osg::ref_ptr<osg::Program> shaderTab::loadShaderProgram( QString filename )
{
	return loadShaderProgramInner(filename).program_itself;
}

void shaderTab::shaderSaveChanges()
{
	int c_row = parent->ui.listOfShaderPrograms->currentRow();

	if (c_row<0)
		return;
	osgUniversalTableWidget * prog_table_item = static_cast<osgUniversalTableWidget *>(parent->ui.listOfShaderPrograms->item(c_row,0));

	osg::ref_ptr<osg::Program> prog = prog_table_item->this_program;
	if (prog == NULL)
		return;

	for (unsigned i = 0; i < prog->getNumShaders(); i++)
	{
		switch(prog->getShader(i)->getType())
		{
			case osg::Shader::VERTEX: prog->getShader(i)->setShaderSource(parent->ui.textEdit_VS->toPlainText().toStdString()); break;
			case osg::Shader::TESSCONTROL: prog->getShader(i)->setShaderSource(parent->ui.textEdit_TCS->toPlainText().toStdString()); break;
			case osg::Shader::TESSEVALUATION: prog->getShader(i)->setShaderSource(parent->ui.textEdit_TES->toPlainText().toStdString()); break;
			case osg::Shader::GEOMETRY: prog->getShader(i)->setShaderSource(parent->ui.textEdit_GS->toPlainText().toStdString()); break;
			case osg::Shader::FRAGMENT: prog->getShader(i)->setShaderSource(parent->ui.textEdit_FS->toPlainText().toStdString()); break;
		}
	}
	parent->ui.osgGLWidget->forcePaintGL();
}

void shaderTab::shaderResetChanges()
{
	int c_row = parent->ui.listOfShaderPrograms->currentRow();

	if (c_row<0)
		return;
	osgUniversalTableWidget * prog_table_item = static_cast<osgUniversalTableWidget *>(parent->ui.listOfShaderPrograms->item(c_row,0));

	osg::ref_ptr<osg::Program> prog = prog_table_item->this_program;
	if (prog == NULL)
		return;

	for (unsigned i = 0; i < prog->getNumShaders(); i++)
	{
		switch(prog->getShader(i)->getType())
		{
		case osg::Shader::VERTEX: parent->ui.textEdit_VS->setPlainText(QString::fromStdString(prog->getShader(i)->getShaderSource())); break;
		case osg::Shader::TESSCONTROL: parent->ui.textEdit_TCS->setPlainText(QString::fromStdString(prog->getShader(i)->getShaderSource())); break;
		case osg::Shader::TESSEVALUATION: parent->ui.textEdit_TES->setPlainText(QString::fromStdString(prog->getShader(i)->getShaderSource())); break;
		case osg::Shader::GEOMETRY: parent->ui.textEdit_GS->setPlainText(QString::fromStdString(prog->getShader(i)->getShaderSource())); break;
		case osg::Shader::FRAGMENT: parent->ui.textEdit_FS->setPlainText(QString::fromStdString(prog->getShader(i)->getShaderSource())); break;
		}
	}
}

void shaderTab::vertexEditChanged()
{
	saveCurrentText(vertexShaderList, parent->ui.textEdit_VS);
}

void shaderTab::tessControlEditChanged()
{
	saveCurrentText(tessControlShaderList, parent->ui.textEdit_TCS);
}

void shaderTab::tessEvalEditChanged()
{
	saveCurrentText(tessEvalShaderList, parent->ui.textEdit_TES);
}

void shaderTab::geometryEditChanged()
{
	saveCurrentText(geometryShaderList, parent->ui.textEdit_GS);
}

void shaderTab::fragmentEditChanged()
{
	saveCurrentText(fragmentShaderList, parent->ui.textEdit_FS);
}

void shaderTab::saveCurrentText( std::vector<QString> &targetList, QPlainTextEdit * sourceEdit)
{
	int c_row = parent->ui.listOfShaderPrograms->currentRow();
	if (c_row<0)
		return;
	targetList[c_row] = sourceEdit->toPlainText();
}

void shaderTab::toggleVisibilityBindings( bool checked )
{
	parent->ui.bindingRegion->setVisible(checked);
}

void shaderTab::toggleVisibilityUniforms( bool checked )
{
	parent->ui.uniformRegion->setVisible(checked);
}

void shaderTab::loadBindings( osg::ref_ptr<osg::Program> currentProgram )
{
	parent->ui.bindingListTable->setRowCount(0);
	osg::Program::AttribBindingList currentBindingList = currentProgram->getAttribBindingList();
	unsigned i = 0; 
	for (osg::Program::AttribBindingList::iterator it = currentBindingList.begin(); it != currentBindingList.end(); it++, i++)
	{
		parent->ui.bindingListTable->setRowCount(parent->ui.bindingListTable->rowCount()+1);
		QTableWidgetItem * newItem = new QTableWidgetItem(QString::fromStdString(it->first));
		parent->ui.bindingListTable->setItem(i,0,newItem);
		newItem = new QTableWidgetItem(QString("%1").arg(it->second));
		parent->ui.bindingListTable->setItem(i,1,newItem);
	}
}

void shaderTab::removeBinding()
{
	parent->ui.bindingListTable->removeRow(parent->ui.bindingListTable->currentRow());
}

void shaderTab::addBinding()
{
	parent->ui.bindingListTable->setRowCount(parent->ui.bindingListTable->rowCount()+1);
	QTableWidgetItem * newItem = new QTableWidgetItem("nazev");
	parent->ui.bindingListTable->setItem(parent->ui.bindingListTable->rowCount()-1,0,newItem);
	newItem = new QTableWidgetItem("0");
	parent->ui.bindingListTable->setItem(parent->ui.bindingListTable->rowCount()-1,1,newItem);
}

void shaderTab::updateBindings()
{
	osg::ref_ptr<osg::Program> currentProgram = getCurrentProgram(parent->ui.listOfShaderPrograms->currentRow());
	if (currentProgram == NULL)
		return;
	osg::Program::AttribBindingList currentBindingList = currentProgram->getAttribBindingList();
	std::map<std::string, int> newBindingList;
	for (unsigned i = 0; i < parent->ui.bindingListTable->rowCount(); i++)
	{
		if (parent->ui.bindingListTable->item(i,0)->text().isEmpty())
			return;
		bool ok;
		int location = parent->ui.bindingListTable->item(i,1)->text().toInt(&ok);
		if (!ok)
			return;
		newBindingList[parent->ui.bindingListTable->item(i,0)->text().toStdString()] = location;
	}

	for (osg::Program::AttribBindingList::iterator it = currentBindingList.begin(); it != currentBindingList.end(); it++)
		currentProgram->removeBindAttribLocation(it->first);
	for (std::map<std::string, int>::iterator it = newBindingList.begin(); it != newBindingList.end(); it++)
		currentProgram->addBindAttribLocation(it->first, it->second);
}



loadedProgram shaderTab::parseProgramAsLoadedProgramStructure( osg::Program * actualProgram, bool * ok /*= NULL */ )
{
	loadedProgram currentLoadedProgram;
	currentLoadedProgram.program_itself = actualProgram;
	currentLoadedProgram.geometry = 0;
	currentLoadedProgram.tess = 0;
	if (actualProgram!=NULL)
	{
		for (unsigned i = 0; i < actualProgram->getNumShaders(); i++)	{
			if (actualProgram->getShader(i)->getType() == osg::Shader::GEOMETRY)
				currentLoadedProgram.geometry = 0;
			else if (actualProgram->getShader(i)->getType() == osg::Shader::TESSCONTROL)
				currentLoadedProgram.tess |= 2;			
			else if (actualProgram->getShader(i)->getType() == osg::Shader::TESSEVALUATION)
				currentLoadedProgram.tess |= 1;			
			if (currentLoadedProgram.tess != 3)
				currentLoadedProgram.tess = 0;
		}
	}
	return currentLoadedProgram;
}


loadedProgram shaderTab::parseNodeAsLoadedProgramStructure( osg::Group * actualNode , bool * ok)
{
	loadedProgram currentLoadedProgram;
	currentLoadedProgram.program_itself = 0;
	currentLoadedProgram.geometry = 0;
	currentLoadedProgram.tess = 0;
	if ((actualNode == NULL || actualNode->getNumChildren()!=2) && ok!=NULL)
	{
		*ok=false;
		return currentLoadedProgram;
	}
	if (actualNode->getChild(0) != NULL && actualNode->getChild(0)->getStateSet()!=NULL)
	{
		currentLoadedProgram = parseProgramAsLoadedProgramStructure(dynamic_cast<osg::Program *>(actualNode->getChild(0)->getStateSet()->getAttribute(osg::StateAttribute::PROGRAM)));
	}
	if (actualNode->getChild(1) != NULL && actualNode->getChild(1)->getStateSet()!=NULL)
	{
		osg::Program * unusedShaders = dynamic_cast<osg::Program *>(actualNode->getChild(1)->getStateSet()->getAttribute(osg::StateAttribute::PROGRAM));
		for (unsigned i = 0; i < unusedShaders->getNumShaders(); i++)	{
			if (unusedShaders->getShader(i)->getType() == osg::Shader::GEOMETRY && currentLoadedProgram.geometry)
				currentLoadedProgram.unused.push_back(unusedShaders->getShader(i));
			if ((unusedShaders->getShader(i)->getType() == osg::Shader::TESSCONTROL || unusedShaders->getShader(i)->getType() == osg::Shader::TESSEVALUATION) && currentLoadedProgram.tess)
				currentLoadedProgram.unused.push_back(unusedShaders->getShader(i));

		}

	}	

	return currentLoadedProgram;
}


void shaderTab::loadProgramFromOSGFile( QString filename )
{
	osg::ref_ptr<osg::Node> loadedNode = osgDB::readNodeFile(filename.toStdString());
	osg::ref_ptr<osg::Group> loadedGroup;// = osgDB::readNodeFile(filename.toStdString());
	if (loadedNode == NULL)
		return;
	if (loadedNode->asGroup()==NULL)
		return;
	else
		loadedGroup = loadedNode->asGroup();
/*	std::vector<loadedProgram> programList;*/
	bool ok = true;
	loadedProgram currentLoadedProgram = parseNodeAsLoadedProgramStructure(loadedGroup, &ok);
	if (ok)
		appendToTable(currentLoadedProgram);
/*
	for (unsigned i = 0; i < loadedGroup->getNumChildren(); i++)
	{

		loadedProgram currentLoadedProgram = parseNodeAsLoadedProgramStructure(loadedGroup, &ok);
		if(ok)
			programList.push_back(currentLoadedProgram);
	}*/



}

void shaderTab::clearShaderTable()
{
	parent->ui.listOfShaderPrograms->clear();
	parent->ui.listOfShaderPrograms->setRowCount(0);
}

void shaderTab::saveProgramToOSGFile( QString filename )
{
/*
	if (currentProgram == NULL)
	{
		parent->writeToConsole(osg::WARN,"Nebyl vybran zadny Program");
		return;
	}*/
	osg::ref_ptr<osg::Program> currentProgram;
	osg::ref_ptr<osg::Program> unusedShaderContainer;
	//osg::ref_ptr<osg::Group> containerGroup = new osg::Group;
/*
	std::vector<osgUniversalTableWidget> selectedNodes = getSelectedRows();
	for (unsigned i = 0; i < parent->ui.listOfShaderPrograms->rowCount(); i++)
	{*/
	int row = parent->ui.listOfShaderPrograms->currentRow();
	QTableWidgetItem * currentItem = parent->ui.listOfShaderPrograms->currentItem();
	if (row < 0 || row >= parent->ui.listOfShaderPrograms->rowCount())
	{
		parent->writeToConsole(osg::WARN,"Nebyl vybran zadny Program");
		return;
	}	
		unusedShaderContainer = new osg::Program();
		currentProgram = dynamic_cast<osgUniversalTableWidget *>(parent->ui.listOfShaderPrograms->item(row,0))->this_program;
		loadedProgram currentProgramStruct = parseProgramAsLoadedProgramStructure(currentProgram);
		if (currentProgramStruct.geometry == 0){
			osg::ref_ptr<osg::Shader> tmp = new	osg::Shader(osg::Shader::GEOMETRY, geometryShaderList[row].toStdString());
			unusedShaderContainer->addShader(tmp);
		}
		if (currentProgramStruct.tess == 0){
			osg::ref_ptr<osg::Shader> tmp = new	osg::Shader(osg::Shader::TESSEVALUATION, tessEvalShaderList[row].toStdString());
			unusedShaderContainer->addShader(tmp);
			tmp = new	osg::Shader(osg::Shader::TESSCONTROL, tessControlShaderList[row].toStdString());
			unusedShaderContainer->addShader(tmp);
		}
		osg::ref_ptr<osg::Group> containerGroup = new osg::Group;
		osg::ref_ptr<osg::Node> tmpNode = new osg::Node;
		tmpNode->getOrCreateStateSet()->setAttribute(currentProgram);
/*		tmpGroup->addChild(tmpNode);*/
		tmpNode->setName("program");
		containerGroup->addChild(tmpNode);
		tmpNode = new osg::Node;
		tmpNode->getOrCreateStateSet()->setAttribute(unusedShaderContainer);
/*		tmpGroup->addChild(tmpNode);*/
		tmpNode->setName("unused");
		containerGroup->addChild(tmpNode);
/*	}*/
	osgDB::writeNodeFile(*(containerGroup.get()), filename.toStdString());
}

std::vector<osgUniversalTableWidget> shaderTab::getSelectedRows()
{
	std::vector<osgUniversalTableWidget> selectedList;
	return selectedList;
}

void shaderTab::appendFromTree( osg::Node * currentNode, std::vector<loadedProgram> * nodeList /*= 0*/ )
{
	bool parent=false;
	if (nodeList == NULL)
	{
		parent = true;
		nodeList = new std::vector<loadedProgram>;
	}
	if (currentNode->getStateSet())
		if (currentNode->getStateSet()->getAttribute(osg::StateAttribute::PROGRAM))
		{
			nodeList->push_back(parseProgramAsLoadedProgramStructure(dynamic_cast<osg::Program *>(currentNode->getStateSet()->getAttribute(osg::StateAttribute::PROGRAM))));
		}
	if (currentNode->asGroup())
		for (int i = 0; i <currentNode->asGroup()->getNumChildren(); i++)
			appendFromTree(currentNode->asGroup()->getChild(i),nodeList);
	if (parent)
		appendToTable(*nodeList);
}

osg::Program * shaderTab::buildProgramFromFiles( QString VS, QString TCS, QString TES, QString GS, QString FS )
{
	QFile vsFile(VS);
	QFile tcsFile(TCS);
	QFile tesFile(TES);
	QFile gsFile(GS);
	QFile fsFile(FS);
	if (!vsFile.open(QFile::ReadOnly | QFile::Text) || !fsFile.open(QFile::ReadOnly | QFile::Text) ||
		(!tcsFile.open(QFile::ReadOnly | QFile::Text) && !TCS.isEmpty()) || (!tesFile.open(QFile::ReadOnly | QFile::Text) && !TES.isEmpty()) ||
		(!gsFile.open(QFile::ReadOnly | QFile::Text)  && !GS.isEmpty()))
	{
		QMessageBox msgBox;

		msgBox.setWindowTitle("Chyba pøi naèítání souboru");
		msgBox.setText(QString("Nepodaøilo se otevøít nìkterý ze souborù:%1\n%2\n%3\n%4\n%5").arg(VS).arg(TCS).arg(TES).arg(GS).arg(FS));
		msgBox.exec();
		return NULL;
	}
	osg::Program * np = new osg::Program;
	np->addShader(new osg::Shader(osg::Shader::VERTEX,QString(vsFile.readAll()).toStdString()));
	if(!TCS.isEmpty())
		np->addShader(new osg::Shader(osg::Shader::TESSCONTROL,QString(tcsFile.readAll()).toStdString()));
	if(!TES.isEmpty())
		np->addShader(new osg::Shader(osg::Shader::TESSEVALUATION,QString(tesFile.readAll()).toStdString()));
	if(!GS.isEmpty())
		np->addShader(new osg::Shader(osg::Shader::GEOMETRY,QString(gsFile.readAll()).toStdString()));
	np->addShader(new osg::Shader(osg::Shader::FRAGMENT,QString(fsFile.readAll()).toStdString()));
	return np;
}




