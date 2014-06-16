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

#include "SceneGraphWidget.h"
#include <osg/Group>


SceneGraphWidget::SceneGraphWidget(QWidget *parent = 0) 
	: QTreeWidget(parent)
{
	invisibleRootItem()->setFlags(invisibleRootItem()->flags() & (~Qt::ItemFlag::ItemIsDropEnabled));
}


SceneGraphWidget::~SceneGraphWidget(void)
{
}

bool SceneGraphWidget::dropMimeData( QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action )
{
	return QTreeWidget::dropMimeData(parent,index,data,action);
// 	QTreeWidgetItem * test = this->currentItem();
// 	QList<QTreeWidgetItem *> test2= selectedItems();
// 	QModelIndex idx;
// 	if (parent) idx = indexFromItem(parent);
// 	return model()->QAbstractItemModel::dropMimeData(data, action , index, 0, idx);
}


/*
 dropevent, zajišuje konzistenci Qt a OSG stromù
*/
void SceneGraphWidget::dropEvent( QDropEvent * event )
{

	QTreeWidget::dropEvent(event);

	int retval = this->topLevelItemCount();

	QTreeWidgetItem * firstitem = topLevelItem(0);
	QTreeWidgetItem * seconditem = topLevelItem(1);

	itemToFix = NULL;

	bool nonconsistency_check = false;

	for (int i = 0; i<firstitem->childCount(); i++)
	{
		nonconsistency_check |= getNonconsistentTreeItem(static_cast<osgNodeTreeItem *>(firstitem->child(i)));
	}
	for (int i = 0; i<seconditem->childCount(); i++)
	{
		nonconsistency_check |= getNonconsistentTreeItem(static_cast<osgNodeTreeItem *>(seconditem->child(i)));
	}

	if(!nonconsistency_check) return;

	osgNodeTreeItem * newParent = static_cast<osgNodeTreeItem *>(itemToFix->parent());
	osgNodeTreeItem * oldParent = itemToFix->parentTreeItem;
	if (oldParent->isCopy)
		unlinkUp(oldParent);


	if (oldParent==firstitem)
	{
		emit sceneGraphEmptied();
	}
	else if (oldParent != seconditem)
		if (oldParent->this_node->asGroup()!= NULL)
		{
			osg::Group * parent_group = oldParent->this_node->asGroup();
/*
			if(itemToFix->isCopy)
				itemToFix->isCopy=false;
			else*/
				parent_group->removeChild(itemToFix->this_node);
		}

	if (newParent == firstitem)
	{
		emit sceneGraphSet(itemToFix->this_node);
	}
	else if (newParent != seconditem)
		if (newParent->this_node->asGroup()!= NULL)
		{
			osg::Group * parent_group = newParent->this_node->asGroup();
			if (!parent_group->containsNode(itemToFix->this_node.get()))
				parent_group->addChild(itemToFix->this_node);
			else
				itemToFix->isCopy = true;
		}

	if (firstitem->childCount()==0)
		firstitem->setFlags(firstitem->flags() | Qt::ItemFlag::ItemIsDropEnabled);
	else
		firstitem->setFlags(firstitem->flags() & (~Qt::ItemFlag::ItemIsDropEnabled));

	itemToFix->parentTreeItem = newParent;

	emit sceneGraphModified();
}

bool SceneGraphWidget::getNonconsistentTreeItem( osgNodeTreeItem * thisItem )
{
	if (thisItem->parentTreeItem != thisItem->parent())
	{
		itemToFix = thisItem;
		return true;
	}
	else
	{
		bool retval=false;
		for (int i=0; i<thisItem->childCount() && !retval ; i++)
		{
			retval |= getNonconsistentTreeItem(static_cast<osgNodeTreeItem *>(thisItem->child(i)));
		}
		return retval;
	}
}

void SceneGraphWidget::insertNewSubtree( osg::Node * nodeToInsert )
{
	if (nodeToInsert == NULL)
	{
		return;
	}
	sgNodeVisitor visitor;
	nodeToInsert->accept(visitor);
	osgNodeTreeItem * tmp = (osgNodeTreeItem *)this->topLevelItem(1);
	for (std::vector<osgNodeTreeItem *>::iterator i = visitor.SceneTreeItemList.begin(); i != visitor.SceneTreeItemList.end();i++)
	{
		tmp->addChild(*i);
		(*i)->parentTreeItem = tmp;
		//ui.treeWidget->insertTopLevelItem(k,*i);
	}
}


void SceneGraphWidget::unlink( osgNodeTreeItem * thisItem )
{
	if (thisItem->isCopy)
	{
		unlinkUp(thisItem);
	}
}


void SceneGraphWidget::unlinkUp( osgNodeTreeItem * thisItem )
{
	if (!thisItem->isCopy && thisItem->this_node!=NULL)
	{
		if (thisItem->this_node->asGroup())
		{
			thisItem->this_node->asGroup()->removeAllChildren();
			for (int i = 0; i < thisItem->childCount(); i++)
			{
				thisItem->this_node->asGroup()->addChild(dynamic_cast<osgNodeTreeItem *>(thisItem->child(i))->this_node);
			}
		}
		return;
	}
	if (thisItem->this_node==NULL || this->topLevelItem(0)==thisItem)
	{
		emit sceneGraphSet(dynamic_cast<osgNodeTreeItem *>(thisItem->child(0))->this_node);
		return;
	}
	//unlinkUp(static_cast<osgNodeTreeItem*>(thisItem->parent()));


	osg::ref_ptr<osg::Node> newNode = dynamic_cast<osg::Node *>(thisItem->this_node->clone(osg::CopyOp::SHALLOW_COPY));
	osg::ref_ptr<osg::Group> newGroup;
	if (newGroup = newNode->asGroup())
	{
		newGroup->removeAllChildren();
		for (int i = 0; i < thisItem->childCount(); i++)
		{
			newGroup->addChild(dynamic_cast<osgNodeTreeItem *>(thisItem->child(i))->this_node);
		}
	}

	thisItem->this_node = newNode;	


/*
	if (thisItem->parentTreeItem)
		if (thisItem->parentTreeItem->isCopy)*/
			unlinkUp(thisItem->parentTreeItem);
	thisItem->isCopy = false;
}

void SceneGraphWidget::unlinkDown( osgNodeTreeItem * thisItem )
{
	if (thisItem->childCount() != 0)
	{
		for (int i = 0; i < thisItem->childCount(); i++)
		unlinkDown(dynamic_cast<osgNodeTreeItem *>(thisItem->child(i)));
	}

	if (thisItem->parentTreeItem)
		if (thisItem->parentTreeItem->isCopy)
			unlinkDown(thisItem->parentTreeItem);

	osg::ref_ptr<osg::Node> newNode = dynamic_cast<osg::Node *>(thisItem->this_node->clone(osg::CopyOp::SHALLOW_COPY));
	osg::ref_ptr<osg::Group> newGroup;
	if (newGroup = newNode->asGroup())
	{
		newGroup->removeAllChildren();
		for (int i = 0; i < thisItem->childCount(); i++)
		{
			newGroup->addChild(dynamic_cast<osgNodeTreeItem *>(thisItem->child(i))->this_node);
		}

	}
	thisItem->isCopy = false;
}
