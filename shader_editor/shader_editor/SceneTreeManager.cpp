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

#include "SceneTreeManager.h"
#include <osg/Geometry>
#include <osgUtil/MeshOptimizers>
#include <QInputDialog>

SceneTreeManager::SceneTreeManager(shader_editor * parent)
{
// 	existing_node = NULL;
	modWidgetStatus = FREE;
	this->parent=parent;
 	actualNode = NULL;
	nodeWidget = new QWidget;
	nodeWidget->setWindowModality(Qt::ApplicationModal);
	ui.setupUi(nodeWidget);
	supportedNodes.push_back("Group");
	supportedNodes.push_back("MatrixTransform");//PositionAttitudeTransform 
	supportedNodes.push_back("PositionAttitudeTransform");//PositionAttitudeTransform 
	correctConfWidgetMap["PositionAttitudeTransform"] = ui.nodeStackedWidget->indexOf(ui.positionAttitudeTransform);
	correctConfWidgetMap["MatrixTransform"] = ui.nodeStackedWidget->indexOf(ui.matrixTransform);
	correctConfWidgetMap["Group"] = ui.nodeStackedWidget->indexOf(ui.group);
	ui.nodeType->addItems(supportedNodes);
	bool retval = true;

	parent->ui.sceneTreeWidget->setExpandsOnDoubleClick(false);

	//parent->ui.sceneTreeWidget->header()->resizeSection(2,20);
	//parent->ui.sceneTreeWidget->header()->setResizeMode(2, QHeaderView::Fixed);  
	parent->ui.sceneTreeWidget->header()->setStretchLastSection(false);
	parent->ui.sceneTreeWidget->header()->setResizeMode(NodeType, QHeaderView::Stretch);  
	parent->ui.sceneTreeWidget->header()->setResizeMode(NodeName, QHeaderView::Stretch);  
	parent->ui.sceneTreeWidget->header()->setResizeMode(Node_HasStateSet, QHeaderView::ResizeToContents);  
	parent->ui.sceneTreeWidget->header()->resizeSection(Node_Setting,30);

	retval &= connect(parent->ui.treeWidgetCopy, SIGNAL(pressed()), this, SLOT(duplicateLinkedSubTree()));
	retval &= connect(ui.pivotVectorTable,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(validateTableWidgetItemAsNumber(QTableWidgetItem *)));
	retval &= connect(ui.positionVectorTable,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(validateTableWidgetItemAsNumber(QTableWidgetItem *)));
	retval &= connect(ui.scaleVectorTable,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(validateTableWidgetItemAsNumber(QTableWidgetItem *)));
	retval &= connect(ui.saveButton,SIGNAL(pressed()),this,SLOT(saveButtonPressed()));
	retval &= connect(ui.nodeType, SIGNAL(currentIndexChanged(int)),this, SLOT(nodeTypeChanged(int)));
	retval &= connect(parent->ui.sceneTreeWidget,SIGNAL(itemActivated(QTreeWidgetItem * , int )),this,SLOT(nodeActivated(QTreeWidgetItem * , int )));
	retval &= connect(parent->ui.sceneTreeWidget,SIGNAL(itemClicked(QTreeWidgetItem * , int )),this,SLOT(nodeActivated(QTreeWidgetItem * , int )));
	retval &= connect(parent->ui.treeWidgetDelete,SIGNAL(pressed ()),this,SLOT(deleteCurrentNode())); 
	retval &= connect(parent->ui.treeWidgetHighlightCurrentNode,SIGNAL(toggled (bool)),this,SLOT(higlightButtonStateChanged(bool))); 

	nhl = new NodeHightLighter();

	geodeWidget = new ConvertWidget(this);

// 	retval &= connect(this,SIGNAL(setForGroup()),ui.pivotVectorTable,SLOT(hide()));
// 	retval &= connect(this,SIGNAL(setForGroup()),ui.pivotVectorTable,SLOT(hide()));
}


SceneTreeManager::~SceneTreeManager(void)
{

}

void SceneTreeManager::validateTableWidgetItemAsNumber( QTableWidgetItem * item )
{
	QString validateItem = item->text();
	bool valid = true;
	double itemValue = validateItem.toDouble(&valid);
	itemValidChanged(valid);
}

void SceneTreeManager::itemValidChanged( bool i )
{
	ui.saveButton->setEnabled(i);
}

//osg vector z qt tabulky
osg::Vec3 SceneTreeManager::tableToVec3( QTableWidget * widgetToConverse, bool * ok /*= NULL*/ )
{
	if (widgetToConverse==NULL)
	{
		osg::Vec3 retval(0,0,0);
		return retval;
	}
	if (widgetToConverse->rowCount()!=1 || widgetToConverse->columnCount()!=3)
	{
		if (ok) *ok = false;
		osg::Vec3 retval(0,0,0);
		return retval;
	}
	osg::Vec3 returnVec(widgetToConverse->item(0,0)->text().toDouble(ok),widgetToConverse->item(0,1)->text().toDouble(ok),widgetToConverse->item(0,2)->text().toDouble(ok));
	return returnVec;
}

//osg matrix z qt tabulky
osg::Matrix SceneTreeManager::tableToMat4( QTableWidget * widgetToConverse, bool * ok /*= NULL*/ )
{
	osg::Matrixd newMatrix;
	double a[4][4];
	for (unsigned i = 0; i < widgetToConverse->rowCount(); i++)
	{
		for (unsigned k = 0; k < widgetToConverse->columnCount(); k++)
		{
			a[i][k] = ui.matrixTable->item(i,k)->text().toDouble(ok);
			if (ok!=NULL && !(*ok))
				break;
		}
		if (ok!=NULL && !(*ok))
			break;
	}

	newMatrix.set(	a[0][0],a[0][1],a[0][2],a[0][3],
					a[1][0],a[1][1],a[1][2],a[1][3],
					a[2][0],a[2][1],a[2][2],a[2][3],
					a[3][0],a[3][1],a[3][2],a[3][3]);
	return newMatrix;
}


void SceneTreeManager::nodeTypeChanged( int i )
{

	if (correctConfWidgetMap.find(ui.nodeType->currentText().toStdString())==correctConfWidgetMap.end())
		return;
	ui.nodeStackedWidget->setCurrentIndex(correctConfWidgetMap[ui.nodeType->currentText().toStdString()]);
}



void SceneTreeManager::createNewNode()
{
	modWidgetStatus = NEW;
	state = CANCEL;
	ui.label_nodeType->setEnabled(true);
	nodeWidget->setWindowModality(Qt::WindowModality::ApplicationModal);
	initInputs(-1);
	nodeTypeChanged(ui.nodeType->currentIndex());
	nodeWidget->show();
/*
	if (state == CANCEL)
		return NULL;
	return actualNode;*/
}

//uprava nodu
void SceneTreeManager::modifyActualNode(osg::Node *  actualNode)
{
	modWidgetStatus = MODIFICATION;
	//nodeWidget->setWindowModality(Qt::WindowModality::ApplicationModal);
	state = CANCEL;
	this->actualNode = actualNode;
	int index = supportedNodes.indexOf(actualNode->className());
	if (index==-1)
	{
		return;
	}
	nodeTypeChanged(index);
	ui.label_nodeType->setEnabled(false);
	nodeWidget->show();
}

//ulozeni zmen noveho/soucasneho osg nodu
void SceneTreeManager::saveButtonPressed()
{
	//state = CANCEL;
	if (modWidgetStatus == MODIFICATION)
	{
		osg::Node * newNode = NULL;
		if (ui.nodeType->currentText() == "Group")
		{
			osg::Group * newGroup = dynamic_cast<osg::Group *> (actualNode.get());
			newNode = newGroup;
		}
		else if (ui.nodeType->currentText()=="MatrixTransform")
		{
			osg::MatrixTransform * newMatrixTransform = dynamic_cast<osg::MatrixTransform *> (actualNode.get());
			newNode = newMatrixTransform;
			bool retval = true;
			osg::Matrix newMatrix = tableToMat4(ui.matrixTable,&retval);
			if (!retval)
				return;

			newMatrixTransform->setMatrix(newMatrix);
		}
		else if (ui.nodeType->currentText()=="PositionAttitudeTransform")
		{
			osg::PositionAttitudeTransform * newPosAttTransform = dynamic_cast<osg::PositionAttitudeTransform *> (actualNode.get());
			newNode = newPosAttTransform;
			bool retval = true;
			osg::Vec3 posVector = tableToVec3(ui.positionVectorTable,&retval);
			if (!retval)	return;
			osg::Vec3 pivotVector = tableToVec3(ui.pivotVectorTable,&retval);
			if (!retval)	return;
			osg::Vec3 scaleVector = tableToVec3(ui.scaleVectorTable,&retval);
			if (!retval)	return;

			newPosAttTransform->setPosition(posVector);
			newPosAttTransform->setPivotPoint(pivotVector);
			newPosAttTransform->setScale(scaleVector);
		}
		if (newNode!=NULL)
		{
			newNode->setName(ui.nodeName->text().toStdString());
		}
	}
	else if (modWidgetStatus == NEW)
	{
		osg::ref_ptr<osg::Node> newNode;
		if (ui.nodeType->currentText() == "Group")
		{
			osg::Group * newGroup = new osg::Group;
			newNode = newGroup;
		}
		else if (ui.nodeType->currentText()=="MatrixTransform")
		{
			osg::ref_ptr<osg::MatrixTransform> newMatrixTransform = new osg::MatrixTransform;
			newNode = newMatrixTransform;

			bool retval = true;
			osg::Matrix newMatrix = tableToMat4(ui.matrixTable,&retval);
			if (!retval)
				return;

			newMatrixTransform->setMatrix(newMatrix);
		}
		else if (ui.nodeType->currentText()=="PositionAttitudeTransform")
		{
			osg::ref_ptr<osg::PositionAttitudeTransform> newPosAttTransform = new osg::PositionAttitudeTransform;
			newNode = newPosAttTransform;

			bool retval = true;
			osg::Vec3 posVector = tableToVec3(ui.positionVectorTable,&retval);
			if (!retval)	return;
			osg::Vec3 pivotVector = tableToVec3(ui.pivotVectorTable,&retval);
			if (!retval)	return;
			osg::Vec3 scaleVector = tableToVec3(ui.scaleVectorTable,&retval);
			if (!retval)	return;

			newPosAttTransform->setPosition(posVector);
			newPosAttTransform->setPivotPoint(pivotVector);
			newPosAttTransform->setScale(scaleVector);
		}
		//parent->ui.sceneTreeWidget->insertNewSubtree(newNode.get());
		appendNodeToInactive(newNode.get());
		newNode->setName(ui.nodeName->text().toStdString());
		nodeWidget->hide();
	}
	modWidgetStatus = FREE;
}

//vybudovani stromu. zahodi predchozi obsah
void SceneTreeManager::rebuildSceneTree()
{
	parent->ui.sceneTreeWidget->clear();
/*	sgNodeVisitor visitor;*/
	osg::Node * start_node = parent->ui.osgGLWidget->getSceneData();
	if (start_node == NULL)
	{
		return;
	}
/*	start_node->accept(visitor);*/

	QStringList node_name_list;  
	node_name_list << "Strom scény" << "";
	osgNodeTreeItem * tmp=new osgNodeTreeItem(NULL,node_name_list, TREE_ITEM_FIXED);
	appendNodeToTarget(tmp,start_node);
/*
	int k=0;
	for (std::vector<osgNodeTreeItem *>::iterator i = visitor.SceneTreeItemList.begin(); i != visitor.SceneTreeItemList.end();i++)
	{
		tmp->addChild(*i);
		(*i)->parentTreeItem = tmp;
		//ui.treeWidget->insertTopLevelItem(k,*i);
		traverseTree_CheckStateSets(tmp);
		traverseTree_HasSetting(tmp);
		k++;
	}*/
	parent->ui.sceneTreeWidget->insertTopLevelItem(0,tmp);

	node_name_list.clear();  
	node_name_list << "Neaktivní prvky scény" << "";
	tmp=new osgNodeTreeItem(NULL,node_name_list, TREE_ITEM_FIXED);
	parent->ui.sceneTreeWidget->insertTopLevelItem(1,tmp);



	parent->ui.sceneTreeWidget->expandAll();	
}

//pripojeni vytvoreneho stromu k jinemu uzlu aktualniho stromu
void SceneTreeManager::appendNodeToTarget( osgNodeTreeItem * appendToThisItem, osg::Node * appendedItems, bool isCopy)
{
	sgNodeVisitor visitor;
	appendedItems->accept(visitor);
	for (std::vector<osgNodeTreeItem *>::iterator i = visitor.SceneTreeItemList.begin(); i != visitor.SceneTreeItemList.end();i++)
	{
		appendToThisItem->addChild(*i);
		(*i)->parentTreeItem = appendToThisItem;
		traverseTree_CheckStateSets(*i);
		traverseTree_HasSetting(*i);
		//for (int i = 0; i < thisNode->childCount(); i++)
		if (isCopy)
			traverseTree_MarkAsCopy(*i);
	}
/*
	if (isCopy)
	{
		traverseTree_MarkAsCopy(appendToThisItem);
		//traverseTree_MarkAsCopy(appendToThisItem);
		//appendedItems
	}*/

}


void SceneTreeManager::appendNodeToInactive( osg::Node * nodeToInsert )
{
	osgNodeTreeItem * tmp = dynamic_cast<osgNodeTreeItem *>(parent->ui.sceneTreeWidget->topLevelItem(1));
	if (!tmp || !nodeToInsert) return;
	appendNodeToTarget(tmp,nodeToInsert);
}


void SceneTreeManager::initInputs( int index )
{
	int first=0;
	int last=ui.nodeType->count()-1;
	if (index >= 0)
	{
		first = index;
		last = index;
	}
	for (int i = first; i <=last; i++)
	{
		if (supportedNodes[i]=="Group")
		{
		}
		if (supportedNodes[i]=="MatrixTransform")
		{
			for (unsigned k = 0; k < ui.matrixTable->rowCount(); k++)
				for (unsigned j = 0; j < ui.matrixTable->columnCount(); j++)
				{
					if (ui.matrixTable->item(k,j) == NULL)
						ui.matrixTable->setItem(k,j, new QTableWidgetItem());
					if (k==j)
						ui.matrixTable->item(k,j)->setText("1");
					else
						ui.matrixTable->item(k,j)->setText("0");
				}
		}
		if (supportedNodes[i]=="PositionAttitudeTransform")
		{
			for (unsigned k = 0; k < ui.positionVectorTable->columnCount(); k++)
			{
				if (ui.positionVectorTable->item(0,k) == NULL)
					ui.positionVectorTable->setItem(0,k, new QTableWidgetItem());
				ui.positionVectorTable->item(0,k)->setText("0");
			}
			for (unsigned k = 0; k < ui.pivotVectorTable->columnCount(); k++)
			{
				if (ui.pivotVectorTable->item(0,k) == NULL)
					ui.pivotVectorTable->setItem(0,k, new QTableWidgetItem());
				ui.pivotVectorTable->item(0,k)->setText("0");
			}
			for (unsigned k = 0; k < ui.scaleVectorTable->columnCount(); k++)
			{
				if (ui.scaleVectorTable->item(0,k) == NULL)
					ui.scaleVectorTable->setItem(0,k, new QTableWidgetItem());
				ui.scaleVectorTable->item(0,k)->setText("1");
			}
		}
	}
}

void SceneTreeManager::initInputs( int index, osg::Node * modified )
{
	if (supportedNodes[index]=="Group")
		initInputs(dynamic_cast<osg::Group *>(modified));
	if (supportedNodes[index]=="MatrixTransform")
		initInputs(dynamic_cast<osg::MatrixTransform *>(modified));
	if (supportedNodes[index]=="PositionAttitudeTransform")
		initInputs(dynamic_cast<osg::PositionAttitudeTransform *>(modified));
}

void SceneTreeManager::initInputs( osg::MatrixTransform * modified )
{
	for (unsigned k = 0; k < ui.matrixTable->rowCount(); k++)
		for (unsigned j = 0; j < ui.matrixTable->columnCount(); j++)
			ui.matrixTable->item(k,j)->setText(QString("%1").arg(modified->getMatrix()(k,j)));
}

void SceneTreeManager::initInputs( osg::Group * modified )
{

}

void SceneTreeManager::initInputs( osg::PositionAttitudeTransform * modified )
{
	for (unsigned k = 0; k < ui.positionVectorTable->rowCount(); k++)
		ui.positionVectorTable->item(0,k)->setText(QString("%1").arg(modified->getPosition()[k]));
	for (unsigned k = 0; k < ui.pivotVectorTable->rowCount(); k++)
		ui.pivotVectorTable->item(0,k)->setText(QString("%1").arg(modified->getPivotPoint()[k]));
	for (unsigned k = 0; k < ui.scaleVectorTable->rowCount(); k++)
		ui.scaleVectorTable->item(0,k)->setText(QString("%1").arg(modified->getScale()[k]));
}

//byl aktivovan node. Qt tree widget nerozlisuje u signalu sloupce, tak je musime analyzovat tady
void SceneTreeManager::nodeActivated( QTreeWidgetItem * item, int column )
{
	static QTreeWidgetItem * lastItem = NULL;
	if (column!=Node_Setting && column!=Node_HasStateSet )
	{
		emit nodeTreeItemActivated(item);
		if (lastItem == parent->ui.sceneTreeWidget->currentItem())
		{
			item->setExpanded(!(item->isExpanded()));
		}
		lastItem = item;

		if (parent->ui.treeWidgetHighlightCurrentNode->isChecked())
		{
			nhl->attachToNode(static_cast<osgNodeTreeItem *>(item)->this_node);
			parent->ui.osgGLWidget->forcePaintGL();
		}
		return;

	}

	lastItem = item;
	osgNodeTreeItem * actual_node = static_cast<osgNodeTreeItem *>(item);

	if (column == Node_Setting)
	{	
		QIcon test = item->icon(Node_Setting);
		if (!test.isNull() && actual_node->this_node->asGeode())
		{
			geodeWidget->setCurrentGeode(actual_node->this_node->asGeode());
			geodeWidget->show();
		}
		else if (!test.isNull())
		{
			modifyActualNode(actual_node->this_node);
		}
	}
	else
		emit nodeTreeItemActivated(item);
}

bool SceneTreeManager::checkNodeHasStateSet( osgNodeTreeItem * thisNode )
{
	return thisNode->this_node->getStateSet() != NULL;
}

bool SceneTreeManager::checkNodeHasSetting( osgNodeTreeItem * thisNode )
{	 
	return correctConfWidgetMap.find(thisNode->this_node->className()) != correctConfWidgetMap.end() || thisNode->this_node->asGeode() != NULL;
}

//projdeme podstrom a oznacime upravitelne se statesetem uzly
void SceneTreeManager::traverseTree_CheckStateSets( osgNodeTreeItem * thisNode )
{
	if (checkNodeHasStateSet(thisNode))
	{
			markHasStateSet(thisNode);
	}
	for (unsigned i = 0; i < thisNode->childCount(); i++)
	{
		traverseTree_CheckStateSets(dynamic_cast<osgNodeTreeItem *>(thisNode->child(i)));
	}
}
//projdeme podstrom a oznacime upravitelne uzly
void SceneTreeManager::traverseTree_HasSetting( osgNodeTreeItem * thisNode )
{
	if (checkNodeHasSetting(thisNode))
	{
		if(thisNode->this_node->asGeode())
			thisNode->setIcon(Node_Setting,QIcon("setting_icon.png"));
		else
			thisNode->setIcon(Node_Setting,QIcon("setting_icon.png"));
	}
	for (unsigned i = 0; i < thisNode->childCount(); i++)
	{
		traverseTree_HasSetting(dynamic_cast<osgNodeTreeItem *>(thisNode->child(i)));
	}
}

//vytvori kopii oznaceneho podstromu. ta bude provazana s puvodni, dokud nedojde k uprave kterekoliv z nich
void SceneTreeManager::duplicateLinkedSubTree()
{
	/*osg::Group * copyContainer = new osg::Group;
	copyContainer->setName(dynamic_cast<osgNodeTreeItem*>(parent->ui.sceneTreeWidget->currentItem())->this_node->getName()+std::string("_copy"));
	copyContainer->addChild(dynamic_cast<osgNodeTreeItem*>(parent->ui.sceneTreeWidget->currentItem())->this_node);*/

	appendNodeToTarget(dynamic_cast<osgNodeTreeItem*>(parent->ui.sceneTreeWidget->topLevelItem(1)),dynamic_cast<osgNodeTreeItem*>(parent->ui.sceneTreeWidget->currentItem())->this_node, true);
	traverseTree_MarkAsCopy(dynamic_cast<osgNodeTreeItem*>(parent->ui.sceneTreeWidget->currentItem()));

// 	for (int i = 0; i < thisNode->childCount(); i++);
// 		traverseTree_MarkAsCopy(dynamic_cast<osgNodeTreeItem*>(thisNode->child(i)));
	/*osg::Node * originalNode = (dynamic_cast<osgNodeTreeItem*>(parent->ui.sceneTreeWidget->currentItem()))->this_node;*/
	/*osg::Node * copiedNode = new osg::Node(*originalNode);*/
}

//oznaci strom s korenem thisNode jako kopii
void SceneTreeManager::traverseTree_MarkAsCopy( osgNodeTreeItem * thisNode )
{
	thisNode->isCopy = true;
	for (int i = 0; i < thisNode->childCount(); i++)
	{
		traverseTree_MarkAsCopy(dynamic_cast<osgNodeTreeItem*>(thisNode->child(i)));
	}
}

//smaze aktualne oznaceny uzel, a odstrani jej jak z OSG tak Qt stromu
void SceneTreeManager::deleteCurrentNode()
{
	if (parent->ui.sceneTreeWidget->currentItem() == NULL)
		return;
	osgNodeTreeItem * itemToDelete = dynamic_cast<osgNodeTreeItem *>(parent->ui.sceneTreeWidget->currentItem());
	parent->ui.sceneTreeWidget->unlink(itemToDelete); //kontrola kopie a zruseni linkovani
	osgNodeTreeItem * itemToDelete_Parent = itemToDelete->parentTreeItem;
	if (parent->ui.sceneTreeWidget->indexOfTopLevelItem(itemToDelete)!=-1)
		return;
	int i ;
	if ( (i = parent->ui.sceneTreeWidget->indexOfTopLevelItem(itemToDelete_Parent)) >= 0)
	{
		if (i == 0)
		{
			parent->ui.osgGLWidget->setSceneData(NULL);//vymazani sceny
			parent->ui.sceneTreeWidget->topLevelItem(i)->setFlags(parent->ui.sceneTreeWidget->topLevelItem(i)->flags() | Qt::ItemFlag::ItemIsDropEnabled);//uvolneni korene sceny
		}
		delete itemToDelete;
		return;
	}
	else if (!itemToDelete->isCopy && itemToDelete_Parent->this_node && itemToDelete_Parent->this_node->asGroup())
		itemToDelete_Parent->this_node->asGroup()->removeChild(itemToDelete->this_node);

	delete itemToDelete;
	
	parent->ui.osgGLWidget->forcePaintGL();

}

//vymaze vse, i neaktivni
void SceneTreeManager::clear()
{
	parent->ui.osgGLWidget->setSceneData(NULL);
	for (int i = 0; i < parent->ui.sceneTreeWidget->topLevelItem(0)->childCount(); i++)
	{
		delete parent->ui.sceneTreeWidget->topLevelItem(0)->child(i);
	}
	for (int i = 0; i < parent->ui.sceneTreeWidget->topLevelItem(1)->childCount(); i++)
	{
		delete parent->ui.sceneTreeWidget->topLevelItem(1)->child(i);
	}
	parent->ui.osgGLWidget->forcePaintGL();
}

void SceneTreeManager::higlightButtonStateChanged( bool checked )
{
	if (checked)
	{
		osgNodeTreeItem * item = static_cast<osgNodeTreeItem*>(parent->ui.sceneTreeWidget->currentItem());
		if (item)
		{
			nhl->attachToNode(item->this_node);
		}
	}
	else
		nhl->detach();
	parent->ui.osgGLWidget->forcePaintGL();
}


/*Funkce convert widget*/

std::vector<std::string> standardArrays;// = {, "Normal", "Color", "SecondaryColor", "FogCoord", "TexCoord"};

ConvertWidget::ConvertWidget( SceneTreeManager * parent )
{
	if (standardArrays.size()==0)
	{
		standardArrays.resize(6);
		standardArrays[0] = "Vertex";
		standardArrays[1] = "Normal";
		standardArrays[2] = "Color";
		standardArrays[3] = "SecondaryColor";
		standardArrays[4] = "FogCoord";
		standardArrays[5] = "TexCoord";
	}
	this->parent = parent;
	geodeWidget = NULL;
	geodeWidget = new QWidget();
	ui.setupUi(geodeWidget);

	bool retval = connect(ui.geometryIndexList,SIGNAL(currentIndexChanged(int)), this, SLOT(currentGeometrySelectionChanged(int)));
	retval &= connect(ui.updateVertexAttribArray,SIGNAL(pressed()), this, SLOT(updateVertexAttribArrays()));
	retval &= connect(ui.convertButton,SIGNAL(pressed()), this, SLOT(convertToVAA()));
	retval &= connect(ui.primitiveSetGLMode,SIGNAL(currentIndexChanged(int)),this,SLOT(updateGeometryPrimitiveSetMode(int)));
	retval &= connect(ui.useVBO,SIGNAL(toggled(bool)),this,SLOT(setUseVBO(bool)));
	retval &= connect(ui.buildDoubleUVs,SIGNAL(pressed()),this,SLOT(buildDoubleUVs()));

	initPrimitiveSetComboBox();
}

void ConvertWidget::parseNewGeode()
{
	QStringList geometryList;
	for (unsigned i = 0; i < currentGeode->getNumDrawables(); i++)
	{
		geometryList.push_back(QString::fromStdString(currentGeode->getDrawable(i)->getName()));
	}
	ui.geometryIndexList->clear();
	ui.geometryIndexList->addItems(geometryList);
	currentGeometrySelectionChanged(ui.geometryIndexList->currentIndex());
}



osg::ref_ptr<osg::Array> ConvertWidget::getArray(int i, osg::Geometry * currentGeometry)
{
	switch(i)
	{
		case 0: return currentGeometry->getVertexArray();
		case 1: return currentGeometry->getNormalArray();
		case 2: return currentGeometry->getColorArray();
		case 3: return currentGeometry->getSecondaryColorArray();
		case 4: return currentGeometry->getFogCoordArray();
		//case 5: return currentGeometry->getTexCoordArray();
	}
	return NULL;
}

osg::ref_ptr<osg::Array> ConvertWidget::getArray(int i, int tex_unit, osg::Geometry * currentGeometry)
{
	switch(i)
	{
		case 5: return currentGeometry->getTexCoordArray(tex_unit);
	}
	return NULL;
}

osg::ref_ptr<osg::Array> ConvertWidget::getArrayWithRemove(int i, osg::Geometry * currentGeometry)
{
	osg::ref_ptr<osg::Array> this_array = NULL;
	switch(i)
	{
	case 0: this_array = currentGeometry->getVertexArray(); currentGeometry->setVertexArray(NULL); break;
	case 1: this_array = currentGeometry->getNormalArray(); currentGeometry->setNormalArray(NULL); break;
	case 2: this_array = currentGeometry->getColorArray(); currentGeometry->setColorArray(NULL); break;
	case 3: this_array = currentGeometry->getSecondaryColorArray(); currentGeometry->setSecondaryColorArray(NULL); break;
	case 4: this_array = currentGeometry->getFogCoordArray(); currentGeometry->setFogCoordArray(NULL); break;
		//case 5: return currentGeometry->getTexCoordArray();
	}
	return this_array;
}

osg::ref_ptr<osg::Array> ConvertWidget::getArrayWithRemove(int i, int tex_unit, osg::Geometry * currentGeometry)
{
	osg::ref_ptr<osg::Array> this_array = NULL;
	switch(i)
	{
	case 5: this_array = currentGeometry->getTexCoordArray(tex_unit); currentGeometry->setTexCoordArray(tex_unit,NULL);
	}
	return this_array;
}

osg::ref_ptr<osg::Array> ConvertWidget::getArray( QString tableCellText, osg::Geometry * currentGeometry )
{
	for (unsigned i = 0; i < 5; i++)
		if (tableCellText==QString::fromStdString(standardArrays[i]))
			return getArrayWithRemove(i,currentGeometry);
	for (unsigned k = 0; k < currentGeometry->getTexCoordArrayList().size(); k++)
		if (tableCellText == (QString::fromStdString(standardArrays[5])+" %1").arg(k))
			return  getArrayWithRemove(5,k,currentGeometry);
	return NULL;
}

void ConvertWidget::currentGeometrySelectionChanged( int i )
{
	if(i<0)
		return;
	osg::Geometry * currentGeometry = currentGeode->getDrawable(i)->asGeometry();
	if(currentGeometry==NULL)
		return;

	setCurrentPrimitiveSetMode(currentGeometry);

	QStringList arrayList;
	for (unsigned i = 0; i < 6; i++)
	{
		osg::ref_ptr<osg::Array> currentArray = getArray(i, currentGeometry);
		if (currentArray != NULL)
			arrayList.push_back(QString::fromStdString(standardArrays[i]));
		if (i==5)
		for (unsigned k = 0; k < currentGeometry->getTexCoordArrayList().size(); k++)
		{
			osg::ref_ptr<osg::Array> currentArray = getArray(i, k, currentGeometry);
			if (currentArray != NULL)
				arrayList.push_back((QString::fromStdString(standardArrays[i])+" %1").arg(k));
		}
	}
	ui.originalArraysTable->setRowCount(arrayList.size());
	for (unsigned i = 0; i < arrayList.size(); i++)
	{
		QTableWidgetItem * newItem = new QTableWidgetItem(arrayList[i]);
		newItem->setFlags(newItem->flags() & ~(Qt::ItemIsEditable));
		ui.originalArraysTable->setItem(i,0,newItem);
	}

	currentAttribArrays.clear();
	ui.vertexAttribArray->setRowCount(0);
	osg::Geometry::ArrayDataList currentGeometryAttribArray = currentGeometry->getVertexAttribArrayList();
	for (unsigned i = 0, k = 0; i < currentGeometryAttribArray.size(); i++)
	{
		osg::Geometry::ArrayData thisArray = currentGeometryAttribArray[i];
		if(!thisArray.empty())
		{
			ui.vertexAttribArray->setRowCount(ui.vertexAttribArray->rowCount()+1);
			QTableWidgetItem * newItem = new QTableWidgetItem(QString::fromStdString(thisArray.array->getName()));
			newItem->setFlags(newItem->flags() & ~(Qt::ItemIsEditable));
			ui.vertexAttribArray->setItem(k,0,newItem);
	
			newItem = new QTableWidgetItem(QString("%1").arg(i));
			newItem->setFlags(newItem->flags() & ~(Qt::ItemIsEditable));
			ui.vertexAttribArray->setItem(k,1,newItem);

			currentAttribArrays.push_back(thisArray);
			currentAttribArraysIndexes.push_back(i);
			k++;
		}
	}

	//ui.originalArraysTable->setColumnCount(1);



	/*
	Vertex
	Normal
	Color
	SecondaryColor
	FogCoord
	TexCoord
	*/

}

void ConvertWidget::convertToVAA()
{
	osg::Geometry * currentGeometry = currentGeode->getDrawable(ui.geometryIndexList->currentIndex())->asGeometry();
	if(currentGeometry==NULL)
		return;
	for (unsigned i = 0; i < ui.originalArraysTable->rowCount(); i++)
	{
		osg::ref_ptr<osg::Array> this_array = getArray(ui.originalArraysTable->item(i,0)->text(),currentGeometry);
		if (this_array)
		{
			QTableWidgetItem * thisItem = ui.originalArraysTable->item(i,0);
			std::string arrayName = thisItem->text().toStdString();
			this_array->setName(arrayName.c_str());
			currentGeometry->setVertexAttribArray(i,this_array);
			currentGeometry->setVertexAttribBinding(i,osg::Geometry::BIND_PER_VERTEX);
		}

	}

	currentGeometrySelectionChanged( ui.geometryIndexList->currentIndex() );

}

void ConvertWidget::updateVertexAttribArrays()
{
	osg::Geometry * currentGeometry = currentGeode->getDrawable(ui.geometryIndexList->currentIndex())->asGeometry();
	std::map<int, int> indexes;
	for (unsigned i = 0; i < currentAttribArraysIndexes.size(); i++)
	{
		if(indexes.find(currentAttribArraysIndexes[i])==indexes.end())
			indexes[currentAttribArraysIndexes[i]]=1;
		else
			indexes[currentAttribArraysIndexes[i]]=indexes[i]+1;
	}
	for (std::map<int, int>::iterator it = indexes.begin(); it!= indexes.end(); it++)
	{
		if(it->second != 1)
			return;
	}

	std::vector<int> new_indexes;
	for (unsigned i = 0; i < ui.vertexAttribArray->rowCount(); i++)
	{
		bool	ok;
		int k = ui.vertexAttribArray->item(i,1)->text().toInt(&ok);
		if (!ok)
			return;
		new_indexes.push_back(k);
	}

	for (unsigned i = 0; i < currentAttribArraysIndexes.size(); i++)
		currentGeometry->setVertexAttribArray(currentAttribArraysIndexes[i],NULL);

	for (unsigned i = 0; i < currentAttribArraysIndexes.size(); i++)
		currentGeometry->setVertexAttribArray(new_indexes[i],currentAttribArrays[i].array);
}

void ConvertWidget::initPrimitiveSetComboBox()
{
	static bool done = false;
	if (done)
		return;
	done = true;
	QStringList nameList;
	nameList << "POINTS" << "LINES" << "LINE_STRIP" << "LINE_LOOP" << "TRIANGLES" << "TRIANGLE_STRIP" << "TRIANGLE_FAN" << "QUADS" << "QUAD_STRIP" << "POLYGON"
		<< "LINES_ADJACENCY" << "LINE_STRIP_ADJACENCY" << "TRIANGLES_ADJACENCY" << "TRIANGLE_STRIP_ADJACENCY" << "PATCHES"; 
	/*            LINES_ADJACENCY = GL_LINES_ADJACENCY_EXT,
	LINE_STRIP_ADJACENCY = GL_LINE_STRIP_ADJACENCY_EXT,
	TRIANGLES_ADJACENCY = GL_TRIANGLES_ADJACENCY_EXT,
	TRIANGLE_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY_EXT,
	PATCHES = GL_PATCHES*/
	ui.primitiveSetGLMode->addItems(nameList);

	indexToMode[0] = osg::PrimitiveSet::POINTS;
	indexToMode[1] = osg::PrimitiveSet::LINES;
	indexToMode[2] = osg::PrimitiveSet::LINE_STRIP;
	indexToMode[3] = osg::PrimitiveSet::LINE_LOOP;
	indexToMode[4] = osg::PrimitiveSet::TRIANGLES;
	indexToMode[5] = osg::PrimitiveSet::TRIANGLE_STRIP;
	indexToMode[6] = osg::PrimitiveSet::TRIANGLE_FAN;
	indexToMode[7] = osg::PrimitiveSet::QUADS;
	indexToMode[8] = osg::PrimitiveSet::QUAD_STRIP;
	indexToMode[9] = osg::PrimitiveSet::POLYGON;
	indexToMode[10] = osg::PrimitiveSet::LINES_ADJACENCY;
	indexToMode[11] = osg::PrimitiveSet::LINE_STRIP_ADJACENCY;
	indexToMode[12] = osg::PrimitiveSet::TRIANGLES_ADJACENCY;
	indexToMode[13] = osg::PrimitiveSet::TRIANGLE_STRIP_ADJACENCY;
	indexToMode[14] = osg::PrimitiveSet::PATCHES;

}

void ConvertWidget::updateGeometryPrimitiveSetMode(int index)
{
	if (!currentGeode)
		return;
	for (int i = 0; i < currentGeode->getNumDrawables(); i++)
	{
		osg::Geometry * g = currentGeode->getDrawable(i)->asGeometry();
		if (g)
		{
			bool convert=false;
/*
			if (indexToMode[index] == osg::PrimitiveSet::PATCHES || indexToMode[index] == osg::PrimitiveSet::TRIANGLES)
				osgUtil::Optimizer::*/
				
			for (int k = 0; k < g->getNumPrimitiveSets(); k++)
			{
				g->getPrimitiveSet(k)->setMode(indexToMode[index]);
			}
		}
	}
}



void ConvertWidget::setCurrentPrimitiveSetMode(osg::Geometry * currentGeometry)
{
	for (std::map<int, osg::PrimitiveSet::Mode>::iterator it = indexToMode.begin(); it!=indexToMode.end(); it++)
	{
		if(currentGeometry->getNumPrimitiveSets()>0)
			if (currentGeometry->getPrimitiveSet(0)->getMode() == it->second)
			{
				ui.primitiveSetGLMode->setCurrentIndex(it->first);
				return;
			}
	}
}

void ConvertWidget::setUseVBO( bool checked )
{
	int index = ui.geometryIndexList->currentIndex();
	if (index<0)
		return;

	currentGeode->getDrawable(index)->setUseVertexBufferObjects(checked);
	currentGeode->getDrawable(index)->setUseDisplayList(!checked);
}

void ConvertWidget::buildDoubleUVs()
{
	int index = ui.geometryIndexList->currentIndex();
	if (index<0)
		return;
	bool ok;
	int vertex_index = QInputDialog::getInt(geodeWidget,tr("Èíslo VAA s vrcholy"),tr("Èíslo VAA"),0,0,1000,1,&ok);
	if (!ok)
		 return;

	int uv_index = QInputDialog::getInt(geodeWidget,tr("Èíslo VAA s vrcholy"),tr("Èíslo VAA"),0,0,1000,1,&ok);
	if (!ok)
		return;

	//std::vector<std::vector<osg::Vec2> > firstUVsVector;
	//std::vector<std::vector<osg::Vec2> > secondUVsVector;
	std::map<osg::Vec3, std::vector<int> > vertex_indexes;

/*	std::map<osg::Vec3,std::vector<osg::Vec2> > */

	if(!currentGeode->getDrawable(index)->asGeometry()) return;

	osg::Geometry * g = currentGeode->getDrawable(index)->asGeometry();

	if (!g->getVertexAttribArray(vertex_index) || !g->getVertexAttribArray(uv_index))	return;

	if (!g->getVertexAttribArray(vertex_index)->asArray() || !g->getVertexAttribArray(uv_index)->asArray())	return;
	osg::Array * a1 = g->getVertexAttribArray(vertex_index)->asArray();
	osg::Array * a2 = g->getVertexAttribArray(uv_index)->asArray();
	if (a1->getType()!=osg::Array::Type::Vec3ArrayType || a2->getType()!=osg::Array::Type::Vec2ArrayType)	return;
	osg::Vec2Array * uvs = dynamic_cast<osg::Vec2Array *>(g->getVertexAttribArray(uv_index)->asArray());
	osg::Vec3Array * vertexs = dynamic_cast<osg::Vec3Array *>(g->getVertexAttribArray(vertex_index)->asArray());
	if (uvs->size() != vertexs->size()) return;

	//firstUVsVector.resize(uvs->size());
	//secondUVsVector.resize(uvs->size());
	osg::ref_ptr<osg::Vec2Array> second_uvs = new osg::Vec2Array;
	second_uvs->resize(uvs->size());
	for (int i = 0; i < vertexs->size(); i++)
	{
		vertex_indexes[(*vertexs)[i]].push_back(i);
	}

	for (std::map<osg::Vec3, std::vector<int> >::iterator it = vertex_indexes.begin(); it!=vertex_indexes.end(); it++)
	{
		if (it->second.size()==1)
		{
			second_uvs->at(it->second[0])=uvs->at(it->second[0]);
		}
		else if (it->second.size()==2)
		{
			second_uvs->at(it->second[1])=uvs->at(it->second[0]);
			second_uvs->at(it->second[0])=uvs->at(it->second[1]);
		}
		else
			return;
	}

	ui.originalArraysTable->setRowCount(ui.originalArraysTable->rowCount()+1);
	QTableWidgetItem * thisItem = ui.originalArraysTable->item(ui.originalArraysTable->rowCount()-1,0);
	//std::string arrayName = thisItem->text().toStdString();
	second_uvs->setName("second_uvs");
	g->setVertexAttribArray(g->getNumVertexAttribArrays(),second_uvs);
	g->setVertexAttribBinding(g->getNumVertexAttribArrays()-1,osg::Geometry::BIND_PER_VERTEX);

	currentGeometrySelectionChanged( ui.geometryIndexList->currentIndex() );
}



