/*
#include "shader_editor.h"
#include "osgUniversalTableWidget.h"
#include <vector>
#include <osg/Program>

void shader_editor::hideShaderBox()
{
	ui.shaderProgramWidget->hide();
	ui.showShaderProgramField->show();
}

void shader_editor::showShaderBox()
{
	ui.shaderProgramWidget->show();
	ui.showShaderProgramField->hide();
}

void shader_editor::buildProgramList()
{
	std::vector<osg::ref_ptr<osg::Program>> program_set;
	bool found;


	for (unsigned i = 0; i < ui.stateSetTableWidget->rowCount(); i++)
	{
		osg::StateAttribute * prog = static_cast<osgUniversalTableWidget *>(ui.stateSetTableWidget->item(i,0))->this_stateset->getAttribute(osg::StateAttribute::PROGRAM);

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

	ui.listOfShaderPrograms->setRowCount(program_set.size());

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
				case osg::Shader::VERTEX: vertex++; break;
				case osg::Shader::TESSCONTROL: tesscontrol++; break;
				case osg::Shader::TESSEVALUATION: tesseval++; break;
				case osg::Shader::GEOMETRY: geometry++; break;
				case osg::Shader::FRAGMENT: fragment++; break;
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
		ui.listOfShaderPrograms->setItem(i,0,newTableItem);
		/ *osgUniversalTableWidget ** / newTableItem = new osgUniversalTableWidget(g_t_list[i],program_set[i].get());
		ui.listOfShaderPrograms->setItem(i,1,newTableItem);
		/ *osgUniversalTableWidget ** / newTableItem = new osgUniversalTableWidget(ss_count_list[i],program_set[i].get());
		ui.listOfShaderPrograms->setItem(i,2,newTableItem);
	}


	/ *	static unsigned id=0, unicount=1, attribcount=2;
	osg::Node * this_node = ui.osgGLWidget->getSceneData();
	if(this_node == NULL)
	return;
	std::vector<osg::StateSet *> stateset_list;
	buildStateSetList(&stateset_list,this_node);
	unsigned width = 3, height = 0;
	ui.stateSetTableWidget->clear();
	ui.stateSetTableWidget->setColumnCount(width);
	ui.stateSetTableWidget->setHorizontalHeaderItem(id,new QTableWidgetItem("id"));
	ui.stateSetTableWidget->setHorizontalHeaderItem(unicount,new QTableWidgetItem("Pocet uniformu"));
	ui.stateSetTableWidget->setHorizontalHeaderItem(attribcount,new QTableWidgetItem("Pocet attributu"));
	ui.stateSetTableWidget->setRowCount(stateset_list.size());
	osgUniversalTableWidget * statesetItem;
	QTableWidgetItem * uniformCountItem, * attributeCountItem;

	for (unsigned i=0; i<stateset_list.size(); i++)
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

	ui.stateSetTableWidget->setItem(i,id,statesetItem);
	ui.stateSetTableWidget->setItem(i,unicount,uniformCountItem);
	ui.stateSetTableWidget->setItem(i,attribcount,attributeCountItem);
	}* /
}

const char * default_vertex_shader;

const char * default_pixel_shader;

void shader_editor::createNewProgram()
{
	osg::ref_ptr<osg::Program> n_prog = new osg::Program();

	osg::ref_ptr<osg::Shader> v_shader = new osg::Shader(osg::Shader::VERTEX);
	osg::ref_ptr<osg::Shader> f_shader = new osg::Shader(osg::Shader::VERTEX);

	n_prog->addShader(v_shader);
	n_prog->addShader(f_shader);

	ui.listOfShaderPrograms->setRowCount(ui.listOfShaderPrograms->rowCount()+1);

	osgUniversalTableWidget * newTableItem = new osgUniversalTableWidget("undefined",n_prog);
	ui.listOfShaderPrograms->setItem(ui.listOfShaderPrograms->rowCount()-1,0,newTableItem);
	/ *osgUniversalTableWidget ** / newTableItem = new osgUniversalTableWidget("Ne / Ne",n_prog);
	ui.listOfShaderPrograms->setItem(ui.listOfShaderPrograms->rowCount()-1,1,newTableItem);
	/ *osgUniversalTableWidget ** / newTableItem = new osgUniversalTableWidget("0",n_prog);
	ui.listOfShaderPrograms->setItem(ui.listOfShaderPrograms->rowCount()-1,2,newTableItem);
}


void shader_editor::changeCurrentShaderProgram( int row, int column )
{
	osgUniversalTableWidget * prog_table_item = static_cast<osgUniversalTableWidget *>(ui.listOfShaderPrograms->item(row,0));

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
		writeToConsoleQt(QtWarningMsg,QString("Nelze otevøít program - nevalidní struktura - VS:%1 TCS:%2 TES:%3 GS:%4 FS:%5").arg(vertex).arg(tesscontrol).arg(tesseval).arg(geometry).arg(fragment));
		return;
	}

	if (tesscontrol && tesseval)
		ui.useTessShader->setChecked(true);
	if (geometry)
		ui.useGeometryShader->setChecked(true);

	for (unsigned k = 0; k < prog->getNumShaders(); k++)
	{
		switch(prog->getShader(k)->getType())
		{
		case osg::Shader::VERTEX: ui.textEdit_VS->setText(QString::fromStdString(prog->getShader(k)->getShaderSource())); break;
		case osg::Shader::TESSCONTROL: ui.textEdit_TCS->setText(QString::fromStdString(prog->getShader(k)->getShaderSource())); break;
		case osg::Shader::TESSEVALUATION: ui.textEdit_TES->setText(QString::fromStdString(prog->getShader(k)->getShaderSource())); break;
		case osg::Shader::GEOMETRY: ui.textEdit_GS->setText(QString::fromStdString(prog->getShader(k)->getShaderSource())); break;
		case osg::Shader::FRAGMENT: ui.textEdit_FS->setText(QString::fromStdString(prog->getShader(k)->getShaderSource())); break;
		}
	}
	
}

*/
