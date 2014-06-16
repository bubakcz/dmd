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

#include "osgUniversalTableWidget.h"


osgUniversalTableWidget::osgUniversalTableWidget(const QString &text, osg::StateSet * stateset, osg::StateAttribute::Type attributetype, int type)
	: QTableWidgetItem(text, type)
{
	this_attributetype = attributetype;
	this_stateset = stateset;
	//osg::ref_ptr<osg::StateSet> this_stateset;
	this_program = NULL;
	this_geom_backup = NULL;
	this_tess_backup.resize(2);
	this_tess_backup[EVALUATION] = NULL;
	this_tess_backup[CONTROL] = NULL;
	//this_tess_backup = std::vector<osg::ref_ptr<osg::Shader> >(2,NULL);
	//osg::StateAttribute::Type this_attributetype;
}

osgUniversalTableWidget::osgUniversalTableWidget(const QString &text, osg::StateSet * stateset, int type)
	: QTableWidgetItem(text, type)
{
	this_stateset = stateset;
	this_program = NULL;
	this_geom_backup = NULL;
	this_tess_backup.resize(2);
	this_tess_backup[EVALUATION] = NULL;
	this_tess_backup[CONTROL] = NULL;
}

osgUniversalTableWidget::osgUniversalTableWidget( const QString &text, osg::Program * program, int type /*= 0*/ )
	: QTableWidgetItem(text, type)
{
	this_program = program;
	this_stateset = NULL;
	this_geom_backup = NULL;
	this_tess_backup.resize(2);
	this_tess_backup[EVALUATION] = NULL;
	this_tess_backup[CONTROL] = NULL;
}


osgUniversalTableWidget::~osgUniversalTableWidget(void)
{
}
