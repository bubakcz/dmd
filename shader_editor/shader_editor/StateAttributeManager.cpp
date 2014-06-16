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
    Funkce tøídy StateAttributeManager. podrobnìjší popis v headeru.
*/

#include "StateAttributeManager.h"
#include "osgUniversalTableWidget.h"
#include <QInputDialog>
#include <QFileDialog>
#include <osgDB/ReadFile>
#include <QBitmap>


StateAttributeManager::StateAttributeManager(shader_editor * parent)
{
	this->parent = parent;
	lastCreateNewAttributeIndex = 0;
	currentStateAttribute = NULL;
	currentStateSet = NULL;
	initCurrentAttribute();

	connect(parent->ui.attributesTableWidget_Add,SIGNAL(pressed()),this,SLOT(createNewAttribute()));
	connect(parent->ui.attributesTableWidget,SIGNAL(itemClicked(QTableWidgetItem *)),this,SLOT(stateAttributeTableItemActivated(QTableWidgetItem *)));
	connect(parent->ui.SA_PolygonMode_Face, SIGNAL(activated(int)), this, SLOT(PolygonMode_FaceChanged()));
	connect(parent->ui.SA_PolygonMode_Mode, SIGNAL(activated(int)), this, SLOT(PolygonMode_ModeChanged()));
	connect(parent->ui.SA_Texture_ListOfTextures, SIGNAL(activated(int)), this, SLOT(Texture_TextureSelected(int)));
	connect(parent->ui.SA_Texture_MagFilter, SIGNAL(activated(int)),this,SLOT(Texture_MagFilterChanged()));
	connect(parent->ui.SA_Texture_MinFilter, SIGNAL(activated(int)),this,SLOT(Texture_MinFilterChanged()));
	connect(parent->ui.SA_Texture_WrapS, SIGNAL(activated(int)),this,SLOT(Texture_WrapSChanged()));
	connect(parent->ui.SA_Texture_WrapT, SIGNAL(activated(int)),this,SLOT(Texture_WrapTChanged()));
	connect(parent->ui.SA_Texture_LoadFromFile,SIGNAL(pressed()),this,SLOT(loadTextureFromFile()));
	connect(parent->ui.SA_CullFace_Mode,SIGNAL(activated(int)),this,SLOT(CullFace_setMode()));
}


StateAttributeManager::~StateAttributeManager(void)
{
}

void StateAttributeManager::initCurrentAttribute()
{
	if (currentStateAttribute == NULL)
	{
		parent->ui.attributesStackedWidget->setCurrentIndex(parent->ui.attributesStackedWidget->indexOf(parent->ui.empty));
	}
	else if (currentStateAttribute->getType() == osg::StateAttribute::POLYGONMODE)
	{
		initPolygonMode();
		parent->ui.attributesStackedWidget->setCurrentIndex(parent->ui.attributesStackedWidget->indexOf(parent->ui.PolygonMode));
	}
	else if (currentStateAttribute->getType() == osg::StateAttribute::POLYGONOFFSET)
	{

		parent->ui.attributesStackedWidget->setCurrentIndex(parent->ui.attributesStackedWidget->indexOf(parent->ui.PolygonOffset));
	}
	else if (currentStateAttribute->getType() == osg::StateAttribute::MATERIAL)
	{
		parent->ui.attributesStackedWidget->setCurrentIndex(parent->ui.attributesStackedWidget->indexOf(parent->ui.Material));
	}
	else if (currentStateAttribute->getType() == osg::StateAttribute::CULLFACE)
	{
		initCullFace();
		parent->ui.attributesStackedWidget->setCurrentIndex(parent->ui.attributesStackedWidget->indexOf(parent->ui.CullFace));
	}
	else if (currentStateAttribute->getType() == osg::StateAttribute::TEXTURE)
	{
		initTexture();
		parent->ui.attributesStackedWidget->setCurrentIndex(parent->ui.attributesStackedWidget->indexOf(parent->ui.Texture));
	}
	else if (currentStateAttribute->getType() == osg::StateAttribute::PROGRAM)
	{
		parent->ui.attributesStackedWidget->setCurrentIndex(parent->ui.attributesStackedWidget->indexOf(parent->ui.Program));
	}
	else
		parent->ui.attributesStackedWidget->setCurrentIndex(parent->ui.attributesStackedWidget->indexOf(parent->ui.empty));
}


void StateAttributeManager::stateAttributeTableItemActivated( QTableWidgetItem * item )
{
	osgUniversalTableWidget * currentItem = dynamic_cast<osgUniversalTableWidget *>(item);
	if (currentItem->this_attributetype == osg::StateAttribute::TEXTURE)
	{
		setCurrentStateAttribute(currentStateSet->getTextureAttribute(currentItem->textureUnit,currentItem->this_attributetype));
	}
	else
		setCurrentStateAttribute(currentStateSet->getAttribute(currentItem->this_attributetype));
}


void StateAttributeManager::rebuildAttributeList(osg::StateSet * this_set)
{
	if (this_set == NULL)
		return;
	osg::StateSet::AttributeList this_AL = this_set->getAttributeList();
	const unsigned attr_classname=0, attr_type=1,attr_value=2;
	const unsigned width = 1, height = 0;
	parent->ui.attributesTableWidget->clear();
	parent->ui.attributesTableWidget->setColumnCount(width);
	parent->ui.attributesTableWidget->setRowCount(this_AL.size());


	unsigned i = 0;
	for (osg::StateSet::AttributeList::iterator it = this_AL.begin(); it!=this_AL.end(); it++, i++)
	{
/*
		QString typename_string = 
			parent->attributeTypeToAttributeName.count(it->first.first) != 0 ? 
			parent->attributeTypeToAttributeName[it->first.first] : 
		"error: not_found";*/
		osgUniversalTableWidget * newItem = new osgUniversalTableWidget(it->second.first->className(),currentStateSet,it->second.first->getType(),0);
		newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
		newItem->this_stateAttribute = it->second.first;
		parent->ui.attributesTableWidget->setItem(i,attr_classname,newItem);
/*		parent->ui.attributesTableWidget->item(i,attr_classname)->setFlags(parent->ui.attributesTableWidget->item(i,attr_classname)->flags() & (~Qt::ItemIsEditable));*/
/*
		QString classname_string = 
			parent->attributeValuesToValuesName.count(static_cast<osg::StateAttribute::Values>(it->first.second)) != 0 ? 
			parent->attributeValuesToValuesName[static_cast<osg::StateAttribute::Values>(it->first.second)] : 
		"error: name_not_found";
		//it->second.first->className();
		parent->ui.attributesTableWidget->setItem(i,attr_type,new QTableWidgetItem(it->second.first->className()));
		parent->ui.attributesTableWidget->item(i,attr_type)->setFlags(parent->ui.attributesTableWidget->item(i,attr_type)->flags() & (~Qt::ItemIsEditable));
*/

		//		ui.attributesTableWidget->setItem(i,attr_type,new QTableWidgetItem(attributeTypeToAttributeName[attributeTypeVector[i]]));
		//ui.stateSetTableWidget->setItem(i,attr_value,new QTableWidgetItem(attributeValuesToValuesName[this_AL[attributeTypeVector[i]].first->valu]));//attributeTypeToAttributeName[attributeTypeVector[i]]));
		// 		ui.stateSetTableWidget->setItem(i,attr_value,new QTableWidgetItem(""));
	}
	osg::StateSet::TextureAttributeList this_TAL = this_set->getTextureAttributeList();
	
	for (int k = 0; k < this_TAL.size(); k++)
	{
		if (!this_TAL[k].empty())
		{		
			parent->ui.attributesTableWidget->setRowCount(parent->ui.attributesTableWidget->rowCount()+1);
			osgUniversalTableWidget * newItem = new osgUniversalTableWidget(this_TAL[k].begin()->second.first->className(),currentStateSet,this_TAL[k].begin()->second.first->getType(),0);
			newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
			newItem->textureUnit=k;
			newItem->this_stateAttribute = this_TAL[k].begin()->second.first;
			parent->ui.attributesTableWidget->setItem(i,attr_classname,newItem);
			if (getTexture2D(this_TAL[k].begin()->second.first))
				addImage(getTexture2D(this_TAL[k].begin()->second.first)->getImage());
			i++;
		}
	}
}

void StateAttributeManager::rebuildAttributesTable_StateSet( int row, int column )
{
	osg::StateSet * this_stateset = (static_cast<osgUniversalTableWidget *>(parent->ui.stateSetTableWidget->item(row,0)))->this_stateset;
	currentStateSet = this_stateset;
	std::ostringstream pointer_string;
	pointer_string << this_stateset;
	if (!this_stateset->getName().empty())
		parent->ui.attributesTableWidget_currentState->setText(QString::fromStdString(this_stateset->getName()));
	else
		parent->ui.attributesTableWidget_currentState->setText(QString::fromStdString(pointer_string.str()));
	rebuildAttributeList(this_stateset);
}

void StateAttributeManager::rebuildAttributesTable_Node(QTreeWidgetItem * this_item)
{
	osgNodeTreeItem * actual_node = static_cast<osgNodeTreeItem *>(this_item);
	if (!actual_node || !actual_node->this_node) return;

	if (actual_node->this_node->getStateSet()==NULL)
	{
		currentStateSet = NULL;
		parent->ui.attributesTableWidget_currentState->setText("");
	}
	else
	{
		currentStateSet = actual_node->this_node->getStateSet();
		std::ostringstream pointer_string;
		pointer_string << currentStateSet;
		if (!currentStateSet->getName().empty())
			parent->ui.attributesTableWidget_currentState->setText(QString::fromStdString(currentStateSet->getName()));
		else
			parent->ui.attributesTableWidget_currentState->setText(QString::fromStdString(pointer_string.str()));
	}
	rebuildAttributeList(actual_node->this_node->getStateSet());
}

void StateAttributeManager::createNewAttribute()
{
	if (currentStateSet == NULL)
	{
		return;
	}
	QStringList nameList;
	for (std::map<osg::StateAttribute::Type, QString>::iterator it = parent->attributeTypeToAttributeName.begin(); it != parent->attributeTypeToAttributeName.end(); it++)
	{
		nameList.push_back(it->second);
	}
	bool ok;
	QString answer = QInputDialog::getItem(parent,"Typ noveho attributu","Typ noveho attributu",nameList,lastCreateNewAttributeIndex,false,&ok);



	if (ok && !answer.isEmpty())
	{
		osg::StateAttribute::Type chosenAttribute = osg::StateAttribute::POLYGONMODE;
		int i = 0; 
		for (std::map<osg::StateAttribute::Type, QString>::iterator it = parent->attributeTypeToAttributeName.begin(); it != parent->attributeTypeToAttributeName.end(); it++, i++)
		{
			if (it->second == answer)
			{
				lastCreateNewAttributeIndex = i;
				chosenAttribute = it->first;
				break;
			}
		}
		switch(chosenAttribute)
		{
		case osg::StateAttribute::POLYGONMODE : currentStateSet->setAttribute(new osg::PolygonMode); break;
		case  osg::StateAttribute::TEXTURE : currentStateSet->setTextureAttribute(getFreeTextureUnit(),new osg::Texture2D);break;
		case  osg::StateAttribute::PROGRAM : currentStateSet->setAttribute(new osg::Program);break;
		case osg::StateAttribute::CULLFACE : currentStateSet->setAttributeAndModes(new osg::CullFace(osg::CullFace::FRONT),osg::StateAttribute::Values::OVERRIDE); break;
		default: parent->writeToConsoleQt(QtWarningMsg,QString("Tvorba State Attributu %1 nepodporovana").arg(answer));
		}
		rebuildAttributeList(currentStateSet.get());
	}

}

void StateAttributeManager::initPolygonMode()
{
	osg::PolygonMode * pm = getPolygonMode(currentStateAttribute.get());
	if (pm!=NULL)
	{
		parent->ui.SA_PolygonMode_Face->setCurrentIndex(0);
		switch(pm->getMode(osg::PolygonMode::FRONT_AND_BACK))
		{
			case osg::PolygonMode::FILL: parent->ui.SA_PolygonMode_Mode->setCurrentIndex(0); break;
			case osg::PolygonMode::LINE: parent->ui.SA_PolygonMode_Mode->setCurrentIndex(1); break;
			case osg::PolygonMode::POINT: parent->ui.SA_PolygonMode_Mode->setCurrentIndex(2); break;
		}
	}
}

void StateAttributeManager::PolygonMode_ModeChanged()
{
	osg::PolygonMode * pm = getPolygonMode(currentStateAttribute.get());
	if (pm!=NULL)
	{
		//parent->ui.SA_PolygonMode_Face->setCurrentIndex(0);
		osg::PolygonMode::Mode newMode;
		osg::PolygonMode::Face currentFace;
		if(parent->ui.SA_PolygonMode_Face->currentText() == "FRONT_AND_BACK")
			currentFace = osg::PolygonMode::FRONT_AND_BACK;
		if(parent->ui.SA_PolygonMode_Face->currentText() == "FRONT")
			currentFace = osg::PolygonMode::FRONT;
		if(parent->ui.SA_PolygonMode_Face->currentText() == "BACK")
			currentFace = osg::PolygonMode::BACK;

		if(parent->ui.SA_PolygonMode_Mode->currentIndex() == 0)
			newMode = osg::PolygonMode::FILL;
		if(parent->ui.SA_PolygonMode_Mode->currentIndex() == 1)
			newMode = osg::PolygonMode::LINE;
		if(parent->ui.SA_PolygonMode_Mode->currentIndex() == 2)
			newMode = osg::PolygonMode::POINT;
		pm->setMode(currentFace,newMode);
		realizeChange();
	}
}

void StateAttributeManager::PolygonMode_FaceChanged()
{
	osg::PolygonMode * pm = getPolygonMode(currentStateAttribute.get());
	if (pm!=NULL)
	{
		//parent->ui.SA_PolygonMode_Face->setCurrentIndex(0);
		osg::PolygonMode::Mode currentMode;
		osg::PolygonMode::Face currentFace;
		if(parent->ui.SA_PolygonMode_Face->currentText() == "FRONT_AND_BACK")
			currentFace = osg::PolygonMode::FRONT_AND_BACK;
		if(parent->ui.SA_PolygonMode_Face->currentText() == "FRONT")
			currentFace = osg::PolygonMode::FRONT;
		if(parent->ui.SA_PolygonMode_Face->currentText() == "BACK")
			currentFace = osg::PolygonMode::BACK;

		currentMode = pm->getMode(currentFace);
		if(currentMode == osg::PolygonMode::FILL)
			parent->ui.SA_PolygonMode_Mode->setCurrentIndex(0); //fill
		if(currentMode == osg::PolygonMode::LINE)
			parent->ui.SA_PolygonMode_Mode->setCurrentIndex(1); //line
		if(currentMode == osg::PolygonMode::POINT)
			parent->ui.SA_PolygonMode_Mode->setCurrentIndex(2); //point
	}
}


void StateAttributeManager::initTexture()
{
	static bool comboBox_NotSet = true;
	osg::Texture2D * pm = getTexture2D(currentStateAttribute.get());
	if (comboBox_NotSet)
	{
		comboBox_NotSet = false;
		QStringList Filter;
		Filter << "LINEAR" << "LINEAR_MIPMAP_LINEAR" << "LINEAR_MIPMAP_NEAREST" << "NEAREST" <<	"NEAREST_MIPMAP_LINEAR" << "NEAREST_MIPMAP_NEAREST";
		TextureStringToFilterParameter[Filter[0]] = osg::Texture::LINEAR;
		TextureStringToFilterParameter[Filter[1]] = osg::Texture::LINEAR_MIPMAP_LINEAR;
		TextureStringToFilterParameter[Filter[2]] = osg::Texture::LINEAR_MIPMAP_NEAREST;
		TextureStringToFilterParameter[Filter[3]] = osg::Texture::NEAREST;
		TextureStringToFilterParameter[Filter[4]] = osg::Texture::NEAREST_MIPMAP_LINEAR;
		TextureStringToFilterParameter[Filter[5]] = osg::Texture::NEAREST_MIPMAP_NEAREST;
		QStringList Wrap;
		Wrap << "CLAMP" << "CLAMP_TO_EDGE" << "CLAMP_TO_BORDER" << "REPEAT" << "MIRROR";
		TextureStringToWrapParameter[Wrap[0]] = osg::Texture::CLAMP;
		TextureStringToWrapParameter[Wrap[1]] = osg::Texture::CLAMP_TO_EDGE;
		TextureStringToWrapParameter[Wrap[2]] = osg::Texture::CLAMP_TO_BORDER;
		TextureStringToWrapParameter[Wrap[3]] = osg::Texture::REPEAT;
		TextureStringToWrapParameter[Wrap[4]] = osg::Texture::MIRROR;
		parent->ui.SA_Texture_MagFilter->addItems(Filter);
		parent->ui.SA_Texture_MinFilter->addItems(Filter);
		parent->ui.SA_Texture_WrapS->addItems(Wrap);
		parent->ui.SA_Texture_WrapT->addItems(Wrap);
	}
	if (pm->getImage() != NULL)	
	{
		parent->ui.SA_Texture_ListOfTextures->setCurrentIndex(findTextureInList(pm->getImage()));
	}
	int i = 0;
	for (std::map<QString,osg::Texture::WrapMode>::iterator it = TextureStringToWrapParameter.begin(); it!=TextureStringToWrapParameter.end(); it++, i++)
	{
		if (it->second == pm->getWrap(osg::Texture::WRAP_S))
			parent->ui.SA_Texture_WrapS->setCurrentIndex(i);
		if (it->second == pm->getWrap(osg::Texture::WRAP_T))
			parent->ui.SA_Texture_WrapT->setCurrentIndex(i);
	}
	i = 0;
	for (std::map<QString,osg::Texture::FilterMode>::iterator it = TextureStringToFilterParameter.begin(); it!=TextureStringToFilterParameter.end(); it++, i++)
	{
		if (it->second == pm->getFilter(osg::Texture::MIN_FILTER))
			parent->ui.SA_Texture_MinFilter->setCurrentIndex(i);
		if (it->second == pm->getFilter(osg::Texture::MAG_FILTER))
			parent->ui.SA_Texture_MagFilter->setCurrentIndex(i);
	}
	//osg::StateSet::TextureAttributeList this_TAL = this_set->getTextureAttributeList();

	
/*
	for (int k = 0; k < this_TAL.size(); k++)
	{
		if (!this_TAL[k].empty())
		{		
			/ *parent->ui.attributesTableWidget->setRowCount(parent->ui.attributesTableWidget->rowCount()+1);
			osgUniversalTableWidget * newItem = new osgUniversalTableWidget(this_TAL[k].begin()->second.first->className(),currentStateSet,this_TAL[k].begin()->second.first->getType(),0);
			newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
			newItem->textureUnit=k;
			newItem->this_stateAttribute = this_TAL[k].begin()->second.first;
			parent->ui.attributesTableWidget->setItem(i,attr_classname,newItem);
			if (getTexture2D(this_TAL[k].begin()->second.first))
				addImage(getTexture2D(this_TAL[k].begin()->second.first)->getImage());
			i++;* /
			if (this_TAL[k].begin()->second.first == pm)
			{
				
			}
		}
	}
*/
	parent->ui.SA_Texture_TexUnit->setText(QString("%1").arg(texture_GetTextureUnit()));

	texture_showPreview(pm->getImage());
}


int StateAttributeManager::texture_GetTextureUnit()
{
	osgUniversalTableWidget * cI = dynamic_cast<osgUniversalTableWidget *>(parent->ui.attributesTableWidget->item(parent->ui.attributesTableWidget->currentRow(),0));
	return cI->textureUnit;
}



QImage StateAttributeManager::qImageFromOSGImage( osg::Image * image )
{
	int width = image->s();
	int height = image->s();
	QImage newQImage(width,height,QImage::Format::Format_ARGB32);
	for (int w = 0; w < width; w++)
		for (int h = 0; h < height; h++)
		{
			osg::Vec4 osgcolor = image->getColor(osg::Vec2(w,h));
			QRgb qtcolor = qRgba(osgcolor.r()*255,osgcolor.g()*255,osgcolor.b()*255,255);
			newQImage.setPixel((width-1)-w,h,qtcolor);
		}
	if (width > height)
		return newQImage.scaledToWidth(150,Qt::SmoothTransformation);
	else
		return newQImage.scaledToHeight(150,Qt::SmoothTransformation);
}

void StateAttributeManager::addImage( osg::Image * newImage )
{
	osg::ref_ptr<osg::Image> osgImage = newImage;
	if (allImages.find(osgImage) == allImages.end())
	{
		allImages.emplace(osgImage);
		texture_RebuildList();
		//previewMap[osgImage] = qImageFromOSGImage(newImage);
	}
}

void StateAttributeManager::loadTextureFromFile()
{
	QString text_loc	 = QFileDialog::getOpenFileName(parent,
		tr("Open Image"), "../", tr("Image Files (*.png *.jpg *.bmp *.tga)"));
	if (text_loc.isEmpty()==true)
		return;
	osg::Image * newImage = osgDB::readImageFile(text_loc.toStdString());
	if (newImage==NULL)
		return;
	else
		addImage(newImage);
}


unsigned StateAttributeManager::getFreeTextureUnit()
{
	osg::StateSet::TextureAttributeList this_TAL = currentStateSet->getTextureAttributeList();

	for (int k = 0; k < this_TAL.size(); k++)
	{
		if (this_TAL[k].empty())
		{		
			return k;
		}
	}
	return this_TAL.size();
}


void StateAttributeManager::texture_RebuildList()
{
	QStringList textureList;
	int i = 0;
	for (std::set<osg::ref_ptr<osg::Image> >::iterator it = allImages.begin(); it != allImages.end(); it++, i++)
	{
		if(it->get()->getName() != "")
			textureList.push_back(QString(it->get()->getName().c_str()));
		else if(it->get()->getFileName() != "")
			textureList.push_back(QString(it->get()->getFileName().c_str()));
		else
			textureList.push_back(QString("%1").arg(i));
	}
	parent->ui.SA_Texture_ListOfTextures->clear();
	parent->ui.SA_Texture_ListOfTextures->addItems(textureList);
}


int StateAttributeManager::findTextureInList( osg::Image * toFind )
{
	int i = 0;
	for (std::set<osg::ref_ptr<osg::Image> >::iterator it = allImages.begin(); it != allImages.end(); it++, i++)
	{
		if (it->get() == toFind)
			return i;
	}
	return i;
}


osg::Image * StateAttributeManager::texture_fromList()
{
	int i = 0;
	for (std::set<osg::ref_ptr<osg::Image> >::iterator it = allImages.begin(); it != allImages.end(); it++, i++)
	{
		if (parent->ui.SA_Texture_ListOfTextures->currentIndex() == i)
			return it->get();
	}
	return NULL;
}

void StateAttributeManager::texture_showPreview( osg::Image * toShow )
{
	if (toShow == NULL)
	{
		parent->ui.SA_Texture_Preview->setPixmap(QPixmap());
		return;
	}
	osg::ref_ptr<osg::Image> toFind = toShow;
	if (previewMap.find(toFind) == previewMap.end())
	{
		previewMap[toFind] = qImageFromOSGImage(toShow);
	}
	parent->ui.SA_Texture_Preview->setPixmap(QPixmap::fromImage(previewMap[toFind]));
}

void StateAttributeManager::Texture_TextureSelected( int i )
{
	texture_showPreview(texture_fromList());
	if (currentStateAttribute != NULL)
		if (getTexture2D(currentStateAttribute) != NULL)
			getTexture2D(currentStateAttribute)->setImage(texture_fromList());
	realizeChange();
}

void StateAttributeManager::Texture_MagFilterChanged()
{
	osg::Texture2D * pm = getTexture2D(currentStateAttribute.get());
	if(pm)
		pm->setFilter(osg::Texture::MAG_FILTER,TextureStringToFilterParameter[parent->ui.SA_Texture_MagFilter->currentText()]);
	realizeChange();
}

void StateAttributeManager::Texture_MinFilterChanged()
{
	osg::Texture2D * pm = getTexture2D(currentStateAttribute.get());
	if(pm)
		pm->setFilter(osg::Texture::MIN_FILTER,TextureStringToFilterParameter[parent->ui.SA_Texture_MagFilter->currentText()]);
	realizeChange();
}

void StateAttributeManager::Texture_WrapSChanged()
{
	osg::Texture2D * pm = getTexture2D(currentStateAttribute.get());
	if(pm)
		pm->setWrap(osg::Texture::WRAP_S,TextureStringToWrapParameter[parent->ui.SA_Texture_WrapS->currentText()]);
	realizeChange();
}

void StateAttributeManager::Texture_WrapTChanged()
{
	osg::Texture2D * pm = getTexture2D(currentStateAttribute.get());
	if(pm)
		pm->setWrap(osg::Texture::WRAP_T,TextureStringToWrapParameter[parent->ui.SA_Texture_WrapS->currentText()]);
	realizeChange();
}

void StateAttributeManager::Texture_ImageChanged()
{
	//texture_fromList
	osg::Texture2D * pm = getTexture2D(currentStateAttribute.get());
	if(pm)
		pm->setImage(texture_fromList());
	realizeChange();
}

void StateAttributeManager::setProgram( osg::Program * newProgram )
{
	osg::Program * currentProgram = getProgram(currentStateAttribute);
	if (newProgram==NULL || currentProgram==NULL)
		return;
	currentStateSet->setAttribute(newProgram);
	/*
	for (int i = 0; i < currentProgram->getNumParents(); i++)
	{
	currentProgram->getParent(i)->setAttribute(newProgram);
	}*/
	realizeChange();
}

void StateAttributeManager::CullFace_setMode()
{
	osg::CullFace * cf = getCullFace(currentStateAttribute);
	if (cf)
	{
		
		if (cullFace_NameToMode.find(parent->ui.SA_CullFace_Mode->currentText()) != cullFace_NameToMode.end())
		{
			//(cullFace_NameToMode[parent->ui.SA_CullFace_Mode->currentText()]);
			osg::CullFace * ncf = new osg::CullFace(cullFace_NameToMode[parent->ui.SA_CullFace_Mode->currentText()]);
			currentStateSet->setAttributeAndModes(ncf,osg::StateAttribute::OVERRIDE);
		}
	}
	realizeChange();
}

void StateAttributeManager::CullFace_GetMode()
{
	osg::CullFace * cf = getCullFace(currentStateAttribute);
	if (cf)
	{
		QString newMode;// = cf->getMode();
		for (std::map<QString, osg::CullFace::Mode>::iterator it = cullFace_NameToMode.begin(); it!=cullFace_NameToMode.end(); it++)
			if (it->second == cf->getMode())
				newMode = it->first;
		for (int i = 0; i < parent->ui.SA_CullFace_Mode->count(); i++)
		{
			if (parent->ui.SA_CullFace_Mode->itemText(i) == newMode)
				parent->ui.SA_CullFace_Mode->setCurrentIndex(i);
		}
		//cf->setMode(cullFace_NameToMode[parent->ui.SA_CullFace_Mode->currentText()]);
	}
}

void StateAttributeManager::initCullFace()
{
	if (cullFace_NameToMode.empty())
	{	
		cullFace_NameToMode["FRONT"] = osg::CullFace::FRONT;
		cullFace_NameToMode["BACK"] = osg::CullFace::BACK;
		cullFace_NameToMode["FRONT_AND_BACK"] = osg::CullFace::FRONT_AND_BACK;
		QStringList nameList;
		for (std::map<QString, osg::CullFace::Mode>::iterator it = cullFace_NameToMode.begin(); it!=cullFace_NameToMode.end(); it++)
			nameList.push_back(it->first);
		parent->ui.SA_CullFace_Mode->addItems(nameList);
	}
	CullFace_GetMode();
	/*FRONT 	
	BACK 	
	FRONT_AND_BACK 	*/
}



