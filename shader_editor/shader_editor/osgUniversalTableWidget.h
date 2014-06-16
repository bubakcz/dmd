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
    
    Widget pro všechno, pokud potøebuju v libovolné tabulce uložit nìjaký odkaz 
    na nìco z OSG, tenhle poslouží  
    Dle potøeby možné cokoliv doplnit   
*/

#pragma once
#include <QTableWidget>
#include <QListWidget>
#include <osg/StateSet>
#include <osg/Program>


class osgUniversalTableWidget :
	public QTableWidgetItem
{
public:
	osgUniversalTableWidget(const QString &text, osg::StateSet * stateset, int type = 0);
	osgUniversalTableWidget(const QString &text, osg::Program * program, int type = 0);
	osgUniversalTableWidget(const QString &text, osg::StateSet * stateset, osg::StateAttribute::Type attributetype, int type = 0);
	~osgUniversalTableWidget(void);

	//osg::StateSet * this_stateset;
	osg::ref_ptr<osg::StateSet> this_stateset;
	osg::ref_ptr<osg::Program> this_program;
	osg::ref_ptr<osg::StateAttribute> this_stateAttribute;
	osg::StateAttribute::Type this_attributetype;

	int textureUnit;

	std::vector<osg::ref_ptr<osg::Shader> > this_tess_backup;
	osg::ref_ptr<osg::Shader> this_geom_backup;
	bool programParseValid;

	enum
	{
		EVALUATION = 0,
		CONTROL
	};
};

class osgUniformListWidget : 
	public QListWidgetItem
{
public:
	osgUniformListWidget(  const QString & text, osg::Uniform::Type uniformType, QListWidget * parent = 0, int type = Type ): QListWidgetItem ( text, parent, type )
	{
		this->uniform_type = uniformType;
	};

	osg::Uniform::Type uniform_type;
	~osgUniformListWidget() {};
};