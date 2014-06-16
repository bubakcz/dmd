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

#include "osgNodeTreeItem.h"
#include "QStack"

osgNodeTreeItem::osgNodeTreeItem(osg::Node * this_node, QTreeWidgetItem *parent, const QStringList &strings, int type = QTreeWidgetItem::Type) 
: QTreeWidgetItem(parent, strings, type)
{
	parentTreeItem = static_cast<osgNodeTreeItem *>(parent);
	this->this_node = this_node;
	isCopy = false;
	/*	osg::ref_ptr<osg::Node> this_node;
	osgNodeTreeItem * parentTreeItem;*/
	if (type == TREE_ITEM_FIXED)
		setFlags(flags() & (~Qt::ItemFlag::ItemIsDragEnabled) | Qt::ItemFlag::ItemIsDropEnabled);
	else if(type == TREE_ITEM_GEODE)
		setFlags((flags() | Qt::ItemFlag::ItemIsDragEnabled) & (~Qt::ItemFlag::ItemIsDropEnabled));
	else
		setFlags(flags() | Qt::ItemFlag::ItemIsDragEnabled | Qt::ItemFlag::ItemIsDropEnabled);
}

osgNodeTreeItem::osgNodeTreeItem(osg::Node * this_node, const QStringList &strings, int type = QTreeWidgetItem::Type) 
	: QTreeWidgetItem(strings, type)
{
	parentTreeItem = NULL;
	this->this_node = this_node;
	isCopy = false;
	if (type == TREE_ITEM_FIXED)
		setFlags(flags() & (~Qt::ItemFlag::ItemIsDragEnabled) | Qt::ItemFlag::ItemIsDropEnabled);
	else if(type == TREE_ITEM_GEODE)
		setFlags((flags() | Qt::ItemFlag::ItemIsDragEnabled) & (~Qt::ItemFlag::ItemIsDropEnabled));
	else
		setFlags(flags() | Qt::ItemFlag::ItemIsDragEnabled | Qt::ItemFlag::ItemIsDropEnabled);
}

osgNodeTreeItem::osgNodeTreeItem( const osgNodeTreeItem &original ) : QTreeWidgetItem (original)
{
	this_node = original.this_node;
	parentTreeItem = original.parentTreeItem;
	isCopy = original.isCopy;
	//this->typ = original.type();

}



osgNodeTreeItem::~osgNodeTreeItem(void)
{
}

osgNodeTreeItem * osgNodeTreeItem::duplicateSceneSubTree( osgNodeTreeItem * original, osgNodeTreeItem * created )
{
	osgNodeTreeItem * newCreated = new osgNodeTreeItem(*original);
	newCreated->parentTreeItem=created;
	return newCreated;
}

/*
osgNodeTreeItem * osgNodeTreeItem::clone()
{
	QTreeWidgetItem *copy = 0;

	QStack<const osgNodeTreeItem*> stack;
	QStack<osgNodeTreeItem*> parentStack;
	stack.push(this);
	parentStack.push(0);

	osgNodeTreeItem *root = 0;
	const osgNodeTreeItem *item = 0;
	osgNodeTreeItem *parent = 0;
	while (!stack.isEmpty()) {
		// get current item, and copied parent
		item = stack.pop();
		parent = parentStack.pop();

		// copy item
		copy = new osgNodeTreeItem(*item);
		if (!root)
			root = copy;

		// set parent and add to parents children list
		if (parent) {
			copy-> = parent;
			parent->children.insert(0, copy);
		}

		for (int i = 0; i < item->childCount(); ++i) {
			stack.push(item->child(i));
			parentStack.push(copy);
		}
	}
	return root;
}*/

void sgNodeVisitor::search_for_parent_main(osg::Node * search_node)
{
	int retval = 0;
	for (std::vector<osgNodeTreeItem *>::iterator i = SceneTreeItemList.begin(); i != SceneTreeItemList.end(); i++)
	{
		retval += search_for_parent(search_node, *i);
	}
	if (retval==0)
	{
		QString classname=search_node->className();
		QString getname= search_node->getName().c_str();
		if (getname.isEmpty())
		{
			getname = "name_not_set";
		}
		QStringList node_name_list;  
		node_name_list << classname << getname;
		
		std::string class_type(search_node->className());
		osgNodeTreeItem * tmp;
		if (class_type == std::string("Geode"))
			tmp = new osgNodeTreeItem(search_node,node_name_list, TREE_ITEM_GEODE);
		else
			tmp = new osgNodeTreeItem(search_node,node_name_list);
		tmp->setExpanded(true);
		SceneTreeItemList.push_back(tmp);
	}
}

int sgNodeVisitor::search_for_parent(osg::Node * search_node, osgNodeTreeItem * this_node )
{
	int retval = 0;
	for (int i = 0; i< this_node->childCount(); i++)
	{
		retval+=search_for_parent(search_node, static_cast<osgNodeTreeItem*>(this_node->child(i)));
	}
	bool is_child= false;
	for (int i=0; i<search_node->getNumParents(); i++)
	{
		if (static_cast<void*>(this_node->this_node)==static_cast<void*>(search_node->getParent(i)))
		{
			is_child = true; 
			break;
		}
	}
// 	osgNodeTreeItem * tmp;
// 	if (class_type == std::string("Geode"))
// 		tmp = new osgNodeTreeItem(search_node,this_node,node_name_list, TREE_ITEM_GEODE);
// 	else
// 		tmp = new osgNodeTreeItem(search_node,this_node,node_name_list);
	if (is_child)
	{
		retval++;
		QString classname=search_node->className();
		QString getname= search_node->getName().c_str();
		if (getname.isEmpty())
		{
			getname = "name_not_set";
		}
		QStringList node_name_list;  
		node_name_list << classname << getname;
		//osgNodeTreeItem * tmp=new osgNodeTreeItem(search_node,this_node,node_name_list);
		std::string class_type(search_node->className());
		osgNodeTreeItem * tmp;
		if (class_type == std::string("Geode"))
		{
			tmp = new osgNodeTreeItem(search_node,this_node,node_name_list, TREE_ITEM_GEODE);
/*			tmp->setIcon(2,QIcon("setting_icon.png"));*/
		}
		else
			tmp = new osgNodeTreeItem(search_node,this_node,node_name_list);
		tmp->setExpanded(true);
		//tmp->parentTreeItem = this_node;
	}

	return retval;
}

void sgNodeVisitor::apply( osg::Node &search_node )
{
	search_for_parent_main(&search_node);
	traverse(search_node);
	 //visitor_output << "classname: " << search_node.className() << " getname: " << search_node.getName() << std::endl;
										/*std::vector< osg::Group * > nodeparents = search_node.ParentList;
										for(std::vector::iterator it = nodeparents.begin(); it!=nodeparents.end(); it++)
											visitor_output << (osg::Node)it->className() << " " << (osg::Node)it->*/

}
