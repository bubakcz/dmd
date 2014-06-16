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
    Deklarace t¯Ìdy display_viewer, ta zajiöùuje vykreslov·nÌ OSG vieweru na Qt Widget
    Udrûuje si v sobÏ instanci vieweru a zajiötuje rozhranÌ  
    Metody p¯ekl·dajÌcÌ shadery lze povaûovat za deprecated (tuhle t¯Ìdu jsem 
    psal jeötÏ nÏkdy na podzim, a od tÈ doby jsem na nic moc neö·hl)        
*/

#pragma once
//#include "c:\qt\4.8.4\src\gui\kernel\qwidget.h"
#include <QWidget>
#include <osgViewer/Viewer>
#include <osgQt/GraphicsWindowQt>
#include <QKeyEvent>
#include <QMouseEvent>
//#include <QTextEdit>
#include <QtGui/QTextEdit>
#include <WinGDI.h>

class display_viewer :
	public QGLWidget//osgQt::GLWidget
{
	Q_OBJECT
public:
	display_viewer(QWidget * parent_widget);
	~display_viewer(void);

	//osg::Camera* createCamera( int x, int y, int w, int h, const std::string& name="", bool windowDecoration=false );
/*	void setSceneData(osg::Group * newnode);*/
	void setSceneData(osg::Node * newnode);
	osg::Node * getSceneData();

	void setTextOutput(QTextEdit * outputTo){console = outputTo;};
	HGLRC getGLContext();
	//void forcePaintGL();
	osg::Program * compileShader(QString * vertex, QString * tesscontrol, QString * tesseval, QString * geometry, QString * fragment, char ** status);
	bool compileShader(osg::Node * targetNode, QString * vertex, QString * tesscontrol, QString * tesseval, QString * geometry, QString * fragment, char ** status);
	void loadNewModel(QString filename);
	osg::ref_ptr<osgViewer::Viewer> getViewer(){return viewer;};
	osg::observer_ptr<osgViewer::GraphicsWindowEmbedded> getGraphicsWindowEmbedded(){return window;};
	osg::ref_ptr<osg::GraphicsContext> getGraphicsContext(){return viewer->getCamera()->getGraphicsContext();};

	void setCamera( osg::Camera * camera);
	void setCamera( osg::Vec3d eye, osg::Vec3d center, osg::Vec3d up );

protected:

	struct initGLCamera_s{
		osg::Vec3d eye; osg::Vec3d center; osg::Vec3d up;
		int isSet;
	} initGLCamera;

	void keyPressEvent(QKeyEvent * keyevent);
	void keyReleaseEvent(QKeyEvent * keyevent);
	void mouseMoveEvent(QMouseEvent * mouseevent);
	void mousePressEvent(QMouseEvent * mouseevent);
	void mouseReleaseEvent(QMouseEvent * mouseevent);
/*	void wheelEvent(QWheelEvent * wheelevent);*/
	void setQtKeyboardMap();
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	
	
	HGLRC glcontext;
	bool isContextSet;
	bool firstFrame;
	QTextEdit * console;
	osg::ref_ptr<osgViewer::Viewer> viewer;
	osg::observer_ptr<osgViewer::GraphicsWindowEmbedded> window;
	osg::ref_ptr<osg::Node> loadedModel;
	//osg::ref_ptr<osg::MatrixTransform> transformation;
	osg::ref_ptr<osg::Group> Groupnode;
	std::map<unsigned int, int> KeyMap;

signals:
	void firstFrameDrawn();

public slots:
	void forcePaintGL();
	void sceneEmptied() {setSceneData(NULL);};
	void sceneSet(osg::Node * newnode) {setSceneData(newnode);};

	
// private: 
// 	void setUpKeyMap();
// 	std::map<int, Qt::Key> _keyMap;
};
