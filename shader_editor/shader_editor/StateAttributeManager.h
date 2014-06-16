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
    
    Správce DockWidgetu se StateAttributy, reaguje na zmìny a podle nich 
    zobrazuje GUI
    
    Pro pøípad pøidání dalšího StateAttributu je tøeba dopsat/upravit metody:
    get<JménoAttributu>
    init<JménoAttributu> (pøípadnì)
    doplnit pøíkazy do initCurrentAttribute
    doplnit pøíkazy do createNewAttribute
    doplnit GUI, zapojit sloty&signály    
*/

#pragma once
#include <QObject>
#include <osg/StateAttribute>
#include <osg/PolygonMode>
#include <osg/Texture2D>
#include <osg/CullFace>
#include "shader_editor.h"
#include <QImage>
#include <osg/Image>
#include <unordered_set>



class StateAttributeManager :
	public QObject
{
	Q_OBJECT
public:
	StateAttributeManager(shader_editor * parent);
	~StateAttributeManager(void);

	std::map<osg::ref_ptr<osg::Image>, QImage> previewMap;
	std::set<osg::ref_ptr<osg::Image> > allImages;

	void addImage(osg::Image * newImage);

	void setProgram(osg::Program * newProgram);

public slots:

	void setCurrentStateAttribute(osg::StateAttribute * newCurrentStateAttribute) {currentStateAttribute = newCurrentStateAttribute; initCurrentAttribute();};
	void PolygonMode_ModeChanged();
	void PolygonMode_FaceChanged();
	void Texture_MagFilterChanged();
	void Texture_MinFilterChanged();
	void Texture_WrapSChanged();
	void Texture_WrapTChanged();
	void Texture_ImageChanged();
	void rebuildAttributeList(osg::StateSet * this_set);
	void rebuildAttributesTable_StateSet(int row, int column);
	void rebuildAttributesTable_Node(QTreeWidgetItem * this_item);
	void createNewAttribute();
	void stateAttributeTableItemActivated(QTableWidgetItem * item);
	void CullFace_setMode();
	void Texture_TextureSelected(int i);

	void loadTextureFromFile();

protected:

	int lastCreateNewAttributeIndex;

	void realizeChange(){parent->ui.osgGLWidget->forcePaintGL();};

	void initCurrentAttribute();
	void initPolygonMode();
	void initTexture();
	void initCullFace();

	shader_editor * parent;

	osg::ref_ptr<osg::StateAttribute> currentStateAttribute;
	osg::ref_ptr<osg::StateSet> currentStateSet;


	osg::PolygonMode * getPolygonMode(osg::StateAttribute * stateAttributeForConversion) 
	{
		std::string test = stateAttributeForConversion->className();
		if (test == "PolygonMode")
			return dynamic_cast<osg::PolygonMode *>(stateAttributeForConversion);
		else
			return NULL;
	};

	QImage qImageFromOSGImage(osg::Image * image);
	void texture_RebuildList();
	osg::Image * texture_fromList();
	int findTextureInList(osg::Image * toFind);
	void texture_showPreview(osg::Image * toShow);
	int texture_GetTextureUnit();

	unsigned getFreeTextureUnit();
	std::map<QString,osg::Texture::WrapMode> TextureStringToWrapParameter;
	std::map<QString,osg::Texture::FilterMode> TextureStringToFilterParameter;
	osg::Texture2D * getTexture2D(osg::StateAttribute * stateAttributeForConversion)
	{
		std::string test = stateAttributeForConversion->className();
		if (test == "Texture2D")
			return dynamic_cast<osg::Texture2D *>(stateAttributeForConversion);
		else
			return NULL;
	};



	osg::Program * getProgram(osg::StateAttribute * stateAttributeForConversion)
	{
		std::string test = stateAttributeForConversion->className();
		if (test == "Program")
			return dynamic_cast<osg::Program *>(stateAttributeForConversion);
		else
			return NULL;
	};


	std::map<QString, osg::CullFace::Mode> cullFace_NameToMode;
	void CullFace_GetMode();
	osg::CullFace * getCullFace(osg::StateAttribute * stateAttributeForConversion)
	{
		if (!stateAttributeForConversion) return NULL;
		std::string test = stateAttributeForConversion->className();
		if (test == "CullFace")
			return dynamic_cast<osg::CullFace *>(stateAttributeForConversion);
		else
			return NULL;
	};
};

