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
    osgNodeTreeItem: widget pro udržování OSG stromu v TreeWidgetu
    
    sgNodeVisitor: nodeVisitor, z osg stromu vybuduje Qt strom
        
*/

#pragma once
#include <QTreeWidgetItem>
#include <osg/Node>
#include <osg/NodeVisitor>
#include <QTreeWidget>

#define TREE_ITEM_FIXED QTreeWidgetItem::UserType
#define TREE_ITEM_GEODE TREE_ITEM_FIXED+1

class osgNodeTreeItem :
	public QTreeWidgetItem
{
public:
	osgNodeTreeItem(osg::Node * this_node, QTreeWidgetItem *parent, const QStringList &strings, int type/* = Type*/);
	osgNodeTreeItem(osg::Node * this_node, const QStringList &strings, int type/* = Type*/);
	osgNodeTreeItem(const osgNodeTreeItem &original);
	~osgNodeTreeItem(void);

	osgNodeTreeItem * duplicateSceneSubTree(osgNodeTreeItem * original, osgNodeTreeItem * created);

/*	osgNodeTreeItem * clone ();*/


	osg::ref_ptr<osg::Node> this_node;
	osgNodeTreeItem * parentTreeItem;
	bool isCopy;

};


class sgNodeVisitor :
	public osg::NodeVisitor
{
public:
	sgNodeVisitor(void) : NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN) {};

	void apply(osg::Node &search_node);

	void search_for_parent_main(osg::Node * this_node);
	int search_for_parent(osg::Node * this_node, osgNodeTreeItem * search_node);

	std::vector<osgNodeTreeItem *> SceneTreeItemList;

	~sgNodeVisitor(){return;};
};

