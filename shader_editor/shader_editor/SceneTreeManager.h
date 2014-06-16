/*
    Copyright (C) 2013  Martin KuËerÚ·k
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
    (kÛdov·nÌ: WindowsCP1250)
    Spr·vce strom˘, zajiöùuje tvorbu, maz·nÌ, kopÌrov·nÌ
       
*/

#pragma once
#include <QObject>
#include <QStringList>
#include <QTableWidgetItem>
#include "ui_SceneTreeModWidget.h"
#include "ui_geode_convert_form.h"
#include <osg/Vec3>
#include <osg/Node>
#include "shader_editor.h"

#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/PolygonMode>
#include <osg/Material>
#include <osg/LineWidth>
#include <osg/Light>
#include <osgGA/NodeTrackerManipulator>

class NodeHightLighter;
class ConvertWidget;
class SceneTreeManager :
	public QObject
{
	Q_OBJECT
public:

	enum
	{
		NodeName = 0,
		NodeType, 
		Node_HasStateSet,
		Node_Setting
	};

	SceneTreeManager(shader_editor * parent);
	~SceneTreeManager(void);

	
	void modifyActualNode(osg::Node *  actualNode);

	void rebuildSceneTree();

	void appendNodeToTarget(osgNodeTreeItem * appendToThisItem, osg::Node * appendedItems, bool isCopy = false);
	void appendNodeToInactive(osg::Node * nodeToInsert);

	void clear();

	void markHasStateSet(osgNodeTreeItem * thisNode){thisNode->setIcon(Node_HasStateSet,QIcon("conf_g.png"));};
	void markHasSetting(osgNodeTreeItem * thisNode){thisNode->setIcon(Node_Setting,QIcon("setting_icon.png"));};

public slots:
	void higlightButtonStateChanged(bool checked);
	void validateTableWidgetItemAsNumber( QTableWidgetItem * item);
	void nodeTypeChanged(int i);
	void nodeActivated(QTreeWidgetItem * item, int column);
	//void showBasicWidgets();
	void saveButtonPressed();
	void duplicateLinkedSubTree();

	void createNewNode();
	void deleteCurrentNode();//	void deleteCurrentNode();


signals:
	//void setForGroup();
	//void setForMatrixTransform();
	//void setForPositionAttitudeTransform();
	void nodeTreeItemActivated(QTreeWidgetItem * item);
	void highlightStateSet(osg::StateSet * lastClickedNodeStateSet);
	//void newNodeCreated(osg::Node * newNode);
	//void nodeModified(osg::Node * newNode);
protected:

	osg::ref_ptr<NodeHightLighter> nhl;

	osg::ref_ptr<osgGA::NodeTrackerManipulator> manip;

	enum
	{
		BAD,
		OK,
		CANCEL
	};

	enum ModWidgetStatus
	{
		FREE,
		NEW,
		MODIFICATION
	};

	std::map<std::string, int> correctConfWidgetMap;

	ModWidgetStatus modWidgetStatus;

	shader_editor * parent;
	osg::ref_ptr<osg::Node>  actualNode;
	int state;
	osg::Vec3 tableToVec3(QTableWidget * widgetToConverse, bool * ok = NULL);
	osg::Matrix tableToMat4(QTableWidget * widgetToConverse, bool * ok = NULL);
	QWidget * nodeWidget;
	void itemValidChanged(bool i);
	QStringList supportedNodes;
	Ui::SceneTreeModificationWidget ui;

	bool checkNodeHasStateSet(osgNodeTreeItem * thisNode);
	bool checkNodeHasSetting(osgNodeTreeItem * thisNode);

	void traverseTree_MarkAsCopy(osgNodeTreeItem * thisNode);
	void traverseTree_CheckStateSets(osgNodeTreeItem * thisNode);
	void traverseTree_HasSetting(osgNodeTreeItem * thisNode);

/*
	void showPositionAttitudeTransformWidgets();
	void showMatrixTransform();*/



	void initInputs(int index);
	void initInputs(int index, osg::Node * modified);
	void initInputs(osg::MatrixTransform * modified);
	void initInputs(osg::Group * modified);
	void initInputs(osg::PositionAttitudeTransform * modified);

	ConvertWidget * geodeWidget;
/*	int getNodeTypeIndex(osg::Node *  actualNode);*/
};


class ConvertWidget :
	public QObject
{
	Q_OBJECT
public:
	ConvertWidget(SceneTreeManager * parent);
	~ConvertWidget(void){};


	Ui::GeodeCovertor ui;

	void setCurrentGeode(osg::Geode * newGeode) {currentGeode = newGeode; parseNewGeode();};

public slots:
	void currentGeometrySelectionChanged(int i);
	void convertToVAA();
	void show() {if(geodeWidget)geodeWidget->show();};
	void updateVertexAttribArrays();
	void updateGeometryPrimitiveSetMode(int index);
	void setUseVBO(bool checked);
	void buildDoubleUVs();

signals:
		//void setForGroup();
		//void setForMatrixTransform();
		//void setForPositionAttitudeTransform();
protected:
	std::vector<osg::Geometry::ArrayData> currentAttribArrays;
	std::vector<int> currentAttribArraysIndexes;
	std::map<int, osg::PrimitiveSet::Mode> indexToMode;
	void parseNewGeode();
	osg::ref_ptr<osg::Array> getArray(int i, int tex_unit, osg::Geometry * currentGeometry);
	osg::ref_ptr<osg::Array> getArray(int i, osg::Geometry * currentGeometry);
	osg::ref_ptr<osg::Array> getArray(QString tableCellText, osg::Geometry * currentGeometry);
	osg::ref_ptr<osg::Array> getArrayWithRemove(int i, osg::Geometry * currentGeometry);
	osg::ref_ptr<osg::Array> getArrayWithRemove(int i, int tex_unit, osg::Geometry * currentGeometry);
	void initPrimitiveSetComboBox();
	void setCurrentPrimitiveSetMode(osg::Geometry * currentGeometry);



	//void updateVertexAttribArrays();

	osg::ref_ptr<osg::Geode> currentGeode;
	SceneTreeManager * parent;
	QWidget * geodeWidget;

	
	/*	int getNodeTypeIndex(osg::Node *  actualNode);*/
};



class NodeHightLighter : public osg::PositionAttitudeTransform
{
public:
	NodeHightLighter() : osg::PositionAttitudeTransform()
	{
		Box = new osg::Geode;

		osg::Geometry * ng = new osg::Geometry;
		osg::Vec3Array * v3a = new osg::Vec3Array;
		v3a->push_back(osg::Vec3(1.0,1.0,1.0));
		v3a->push_back(osg::Vec3(1.0,-1.0,1.0));
		v3a->push_back(osg::Vec3(-1.0,-1.0,1.0));
		v3a->push_back(osg::Vec3(-1.0,1.0,1.0));

		v3a->push_back(osg::Vec3(1.0,1.0,-1.0));
		v3a->push_back(osg::Vec3(1.0,-1.0,-1.0));
		v3a->push_back(osg::Vec3(-1.0,-1.0,-1.0));
		v3a->push_back(osg::Vec3(-1.0,1.0,-1.0));


		GLuint arr[24] = {	0,1,
							1,2,
							2,3,
							3,0,
							
							4,5,
							5,6,
							6,7,
							7,4,

							0,4,
							1,5,
							2,6,
							3,7
		};

		ng->setVertexArray(v3a);
		ng->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::LINES,24,arr));

		Box->addDrawable(ng);
		Box->getOrCreateStateSet()->setAttribute(new osg::PolygonMode(osg::PolygonMode::Face::FRONT_AND_BACK,osg::PolygonMode::Mode::LINE));
		osg::Material * nm = new osg::Material;
		nm->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.5, 1.0, 0.0, 1.0));
		nm->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(0.5, 1.0, 0.0, 1.0));
		Box->getOrCreateStateSet()->setAttribute(nm);
		Box->getOrCreateStateSet()->setAttribute(new osg::LineWidth(3));
		osg::Light * lm = new osg::Light;
		lm->setAmbient(osg::Vec4(0.5, 1.0, 0.0, 1.0));
		Box->getOrCreateStateSet()->setAttribute(lm);
		this->addChild(Box);

		//osg::Mate
	};

	void dimensionsFromBoundingBox(osg::BoundingSphere bs)
	{
/*
		float scaleX = bs->xMax() - bs->xMin();
		float scaleY = bs->yMax() - bs->yMin();
		float scaleZ = bs->zMax() - bs->zMin();*/

		this->setPosition(bs.center());
		this->setScale(osg::Vec3(bs.radius(),bs.radius(),bs.radius()));
	};

	void attachToNode(osg::Node * target)
	{

		detach();
		if (target == NULL)
			return;
		for (unsigned i = 0; i < target->getNumParents(); i++)
		{
			if (target->getParent(i)->asGroup())
			{
				target->getParent(i)->asGroup()->addChild(this);
			}
			
		}
		dimensionsFromBoundingBox(target->getBound());
	};

	void detach()
	{
		for (unsigned i = 0; i < this->getNumParents(); i++)
		{
			if (this->getParent(i)->asGroup())
			{
				this->getParent(i)->asGroup()->removeChild(this);//target->getParent(i)->asGroup()->addChild(this);
			}

		}
	};
protected:
	osg::ref_ptr<osg::Geode> Box;
};

