/*
    Copyright (C) 2013  Martin Ku�er��k
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
    (k�dov�n�: WindowsCP1250)
    Hlavi�kov� soubor t��dy SceneGraphWidget. Zaji��uje konzistenci mezi Qt 
    stromem a OSG stromem sc�ny.
    Subclassuje standardn� reakce na ud�losti, kter� reaguj� na p�esuny polo�ek 
    stromu.
*/

#pragma once
#include <QTreeWidget>/*c:\qt\4.8.4\src\gui\itemviews\*/
#include "osgNodeTreeItem.h"
class SceneGraphWidget :
	public QTreeWidget
{
	Q_OBJECT
public:
	SceneGraphWidget(QWidget *parent);
	~SceneGraphWidget(void);
	void insertNewSubtree(osg::Node * nodeToInsert);

	void unlink(osgNodeTreeItem * thisItem);


protected:
	bool dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action);
	void dropEvent ( QDropEvent * event );

	osgNodeTreeItem * itemToFix;

	bool getNonconsistentTreeItem(osgNodeTreeItem * thisItem);
	void unlinkUp(osgNodeTreeItem * thisItem);
	void unlinkDown(osgNodeTreeItem * thisItem);
signals:
	void sceneGraphModified();
	void sceneGraphEmptied();
	void sceneGraphSet(osg::Node * currentNode);
};

