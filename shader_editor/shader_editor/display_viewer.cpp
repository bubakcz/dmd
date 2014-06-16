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
    Funkce t¯Ìdy display_viewer, ta zajiöùuje vykreslov·nÌ OSG vieweru na Qt Widget
    Udrûuje si v sobÏ instanci vieweru a zajiötuje rozhranÌ  
    Metody p¯ekl·dajÌcÌ shadery lze povaûovat za deprecated (tuhle t¯Ìdu jsem 
    psal jeötÏ nÏkdy na podzim, a od tÈ doby jsem na nic moc neö·hl)
*/


#include "display_viewer.h"
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <osgViewer/ViewerEventHandlers>
#include <osg/DisplaySettings>

QGLFormat getFormat()
{
	QGLFormat newFormat;
	newFormat.setSampleBuffers(true);
	newFormat.setSamples(16);

	return newFormat;
}

display_viewer::~display_viewer(void)
{
}

display_viewer::display_viewer(QWidget * parent_widget): QGLWidget(getFormat(), parent_widget)
{
	initGLCamera.isSet = 0;

	firstFrame = true;
	isContextSet = false;
	viewer = new osgViewer::Viewer;
	viewer->setReleaseContextAtEndOfFrameHint(false);
	osg::DisplaySettings::instance()->setNumMultiSamples( 4 );
}

void display_viewer::initializeGL()
{
	window = viewer->setUpViewerAsEmbeddedInWindow(0, 0, width(), height());
	osgViewer::Viewer::Windows windows; 
	viewer->getWindows(windows);


	osgGA::TrackballManipulator * cameraManip = new osgGA::TrackballManipulator;
	viewer->setCameraManipulator(cameraManip);
	for(osgViewer::Viewer::Windows::iterator itr = windows.begin();
		itr != windows.end();
		++itr)
	{
		osg::State *s=(*itr)->getState();
		s->setUseModelViewAndProjectionUniforms(true);
		//s->setUse
	}

	if(initGLCamera.isSet)
		cameraManip->setTransformation(initGLCamera.eye, initGLCamera.center, initGLCamera.up);

}

void display_viewer::resizeGL( int width, int height )
{
	if (window.valid())
	{
		window->resized(window->getTraits()->x, window->getTraits()->y, width, height);
		window->getEventQueue()->windowResize(window->getTraits()->x, window->getTraits()->y, width, height);
	}
}


void display_viewer::setSceneData( osg::Node* newnode )
{
	viewer->setSceneData(newnode);
	if(!viewer->isRealized())
		viewer->realize();
}


void display_viewer::paintGL()
{
	if (viewer.valid())
	{
		viewer->frame();
		isContextSet=true;
		glcontext = wglGetCurrentContext();
		if (firstFrame)
		{
			firstFrame=false;
			emit firstFrameDrawn();
		}

		osg::Node * test_node = viewer->getSceneData();


	}
}

void display_viewer::keyPressEvent( QKeyEvent * keyevent )
{
	window->getEventQueue()->keyPress(KeyMap[keyevent->key()]);
	update();
}

void display_viewer::keyReleaseEvent( QKeyEvent * keyevent )
{
	window->getEventQueue()->keyRelease(KeyMap[keyevent->key()]);
	update();
}

void display_viewer::mouseMoveEvent( QMouseEvent * mouseevent )
{
	window->getEventQueue()->mouseMotion(mouseevent->x(), mouseevent->y());
	update();
}

void display_viewer::mouseReleaseEvent( QMouseEvent * mouseevent )
{
	window->getEventQueue()->mouseMotion(mouseevent->x(), mouseevent->y());

	unsigned int this_button=0;

	switch(mouseevent->button())
	{
		case Qt::LeftButton: this_button = 1; break;
		case Qt::RightButton: this_button = 3; break;
		case Qt::MiddleButton: this_button = 2; break;
	}
	window->getEventQueue()->mouseButtonRelease(mouseevent->x(),mouseevent->y(),this_button);
	update();
}

void display_viewer::mousePressEvent( QMouseEvent * mouseevent )
{
	window->getEventQueue()->mouseMotion(mouseevent->x(), mouseevent->y());
	unsigned int this_button=0;

	switch(mouseevent->button())
	{
	case Qt::LeftButton: this_button = 1; break;
	case Qt::RightButton: this_button = 3; break;
	case Qt::MiddleButton: this_button = 2; break;
	}
	window->getEventQueue()->mouseButtonPress(mouseevent->x(),mouseevent->y(),this_button);
	update();
}




HGLRC display_viewer::getGLContext()
{
	if (isContextSet)
		return glcontext;
	else
		return NULL;

}

void display_viewer::forcePaintGL()
{
	if (viewer.valid())
		//viewer->advance();
	{
		//viewer->setDone(false);
		viewer->frame(viewer->elapsedTime()+1);
		update();
/*
		window->getEventQueue()->mouseButtonPress(0,0,1);
		window->getEventQueue()->mouseButtonRelease(0,0,1);*/
		//viewer->setDone(true);
	}
}

osg::Program * display_viewer::compileShader( QString * vertex, QString * tesscontrol, QString * tesseval, QString * geometry, QString * fragment, char ** status)
{
	if(fragment == NULL)
	{
		*status = "Nebyl p¯ed·n odkaz na kÛd fragment shaderu";
		return NULL; 
	}
	if(vertex==NULL)
	{
		*status = "Nebyl p¯ed·n odkaz na kÛd vertex shaderu";
		return NULL; 
	}
	osg::Program * shaderprogram = new osg::Program;
	

	osg::ref_ptr<osg::Shader> vertexsh = new osg::Shader(osg::Shader::VERTEX);
	osg::ref_ptr<osg::Shader> tesscontrolsh = new osg::Shader(osg::Shader::TESSCONTROL);
	osg::ref_ptr<osg::Shader> tessevalsh = new osg::Shader(osg::Shader::TESSEVALUATION);
	osg::ref_ptr<osg::Shader> geometrysh = new osg::Shader(osg::Shader::GEOMETRY);
	osg::ref_ptr<osg::Shader> fragmentsh = new osg::Shader(osg::Shader::FRAGMENT);

	vertexsh->setShaderSource(vertex->toStdString());
	shaderprogram->addShader(vertexsh);

	if (tesscontrol != NULL && tesseval != NULL)
	{
		tesscontrolsh->setShaderSource(tesscontrol->toStdString());
		shaderprogram->addShader(tesscontrolsh);


		tessevalsh->setShaderSource(tesseval->toStdString());
		shaderprogram->addShader(tessevalsh);
	}


	if (geometry!=NULL)
	{
		geometrysh->setShaderSource(geometry->toStdString());
		shaderprogram->addShader(geometrysh);
	}

	
	
	fragmentsh->setShaderSource(fragment->toStdString());
	shaderprogram->addShader(fragmentsh);
	return shaderprogram;
}

bool display_viewer::compileShader(osg::Node * targetNode, QString * vertex, QString * tesscontrol, QString * tesseval, QString * geometry, QString * fragment, char ** status)
{
	if(fragment == NULL)
	{
		*status = "Nebyl p¯ed·n odkaz na kÛd fragment shaderu";
		return false; 
	}
	if(vertex==NULL)
	{
		*status = "Nebyl p¯ed·n odkaz na kÛd vertex shaderu";
		return false; 
	}
	osg::ref_ptr<osg::StateSet> modelstateset = targetNode->getOrCreateStateSet();
	osg::ref_ptr<osg::Program> shaderprogram = new osg::Program;
	

	osg::ref_ptr<osg::Shader> vertexsh = new osg::Shader(osg::Shader::VERTEX);
	osg::ref_ptr<osg::Shader> tesscontrolsh = new osg::Shader(osg::Shader::TESSCONTROL);
	osg::ref_ptr<osg::Shader> tessevalsh = new osg::Shader(osg::Shader::TESSEVALUATION);
	osg::ref_ptr<osg::Shader> geometrysh = new osg::Shader(osg::Shader::GEOMETRY);
	osg::ref_ptr<osg::Shader> fragmentsh = new osg::Shader(osg::Shader::FRAGMENT);

	vertexsh->setShaderSource(vertex->toStdString());
	shaderprogram->addShader(vertexsh);

	if (tesscontrol != NULL && tesseval != NULL)
	{
		tesscontrolsh->setShaderSource(tesscontrol->toStdString());
		shaderprogram->addShader(tesscontrolsh);


		tessevalsh->setShaderSource(tesseval->toStdString());
		shaderprogram->addShader(tessevalsh);
	}


	if (geometry!=NULL)
	{
		geometrysh->setShaderSource(geometry->toStdString());
		shaderprogram->addShader(geometrysh);
	}



	fragmentsh->setShaderSource(fragment->toStdString());
	shaderprogram->addShader(fragmentsh);



	modelstateset->setAttributeAndModes(shaderprogram, osg::StateAttribute::ON);

	return true;
}

void display_viewer::loadNewModel( QString filename )
{
	osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(filename.toStdString());
	if (!loadedModel)
	{
		throw 0;
		return;
	}
	setSceneData(loadedModel);
}

osg::Node * display_viewer::getSceneData()
{
	return viewer->getSceneData();
}

void display_viewer::setQtKeyboardMap()
{
	KeyMap[Qt::Key_Escape     ] = osgGA::GUIEventAdapter::KEY_Escape;
	KeyMap[Qt::Key_Delete   ] = osgGA::GUIEventAdapter::KEY_Delete;
	KeyMap[Qt::Key_Home       ] = osgGA::GUIEventAdapter::KEY_Home;
	KeyMap[Qt::Key_Enter      ] = osgGA::GUIEventAdapter::KEY_KP_Enter;
	KeyMap[Qt::Key_End        ] = osgGA::GUIEventAdapter::KEY_End;
	KeyMap[Qt::Key_Return     ] = osgGA::GUIEventAdapter::KEY_Return;
	KeyMap[Qt::Key_PageUp     ] = osgGA::GUIEventAdapter::KEY_Page_Up;
	KeyMap[Qt::Key_PageDown   ] = osgGA::GUIEventAdapter::KEY_Page_Down;
	KeyMap[Qt::Key_Left       ] = osgGA::GUIEventAdapter::KEY_Left;
	KeyMap[Qt::Key_Right      ] = osgGA::GUIEventAdapter::KEY_Right;
	KeyMap[Qt::Key_Up         ] = osgGA::GUIEventAdapter::KEY_Up;
	KeyMap[Qt::Key_Down       ] = osgGA::GUIEventAdapter::KEY_Down;
	KeyMap[Qt::Key_Backspace  ] = osgGA::GUIEventAdapter::KEY_BackSpace;
	KeyMap[Qt::Key_Tab        ] = osgGA::GUIEventAdapter::KEY_Tab;
	KeyMap[Qt::Key_Space      ] = osgGA::GUIEventAdapter::KEY_Space;
	KeyMap[Qt::Key_Delete     ] = osgGA::GUIEventAdapter::KEY_Delete;
	KeyMap[Qt::Key_Alt      ] = osgGA::GUIEventAdapter::KEY_Alt_L;
	KeyMap[Qt::Key_Shift    ] = osgGA::GUIEventAdapter::KEY_Shift_L;
	KeyMap[Qt::Key_Control  ] = osgGA::GUIEventAdapter::KEY_Control_L;
	KeyMap[Qt::Key_Meta     ] = osgGA::GUIEventAdapter::KEY_Meta_L;

	KeyMap[Qt::Key_F1             ] = osgGA::GUIEventAdapter::KEY_F1;
	KeyMap[Qt::Key_F2             ] = osgGA::GUIEventAdapter::KEY_F2;
	KeyMap[Qt::Key_F3             ] = osgGA::GUIEventAdapter::KEY_F3;
	KeyMap[Qt::Key_F4             ] = osgGA::GUIEventAdapter::KEY_F4;
	KeyMap[Qt::Key_F5             ] = osgGA::GUIEventAdapter::KEY_F5;
	KeyMap[Qt::Key_F6             ] = osgGA::GUIEventAdapter::KEY_F6;
	KeyMap[Qt::Key_F7             ] = osgGA::GUIEventAdapter::KEY_F7;
	KeyMap[Qt::Key_F8             ] = osgGA::GUIEventAdapter::KEY_F8;
	KeyMap[Qt::Key_F9             ] = osgGA::GUIEventAdapter::KEY_F9;
	KeyMap[Qt::Key_F10            ] = osgGA::GUIEventAdapter::KEY_F10;
	KeyMap[Qt::Key_F11            ] = osgGA::GUIEventAdapter::KEY_F11;
	KeyMap[Qt::Key_F12            ] = osgGA::GUIEventAdapter::KEY_F12;
	KeyMap[Qt::Key_F13            ] = osgGA::GUIEventAdapter::KEY_F13;
	KeyMap[Qt::Key_F14            ] = osgGA::GUIEventAdapter::KEY_F14;
	KeyMap[Qt::Key_F15            ] = osgGA::GUIEventAdapter::KEY_F15;
	KeyMap[Qt::Key_F16            ] = osgGA::GUIEventAdapter::KEY_F16;
	KeyMap[Qt::Key_F17            ] = osgGA::GUIEventAdapter::KEY_F17;
	KeyMap[Qt::Key_F18            ] = osgGA::GUIEventAdapter::KEY_F18;
	KeyMap[Qt::Key_F19            ] = osgGA::GUIEventAdapter::KEY_F19;
	KeyMap[Qt::Key_F20            ] = osgGA::GUIEventAdapter::KEY_F20;

	KeyMap[Qt::Key_hyphen         ] = '-';
	KeyMap[Qt::Key_Equal         ] = '=';

	KeyMap[Qt::Key_division      ] = osgGA::GUIEventAdapter::KEY_KP_Divide;
	KeyMap[Qt::Key_multiply      ] = osgGA::GUIEventAdapter::KEY_KP_Multiply;
	KeyMap[Qt::Key_Minus         ] = '-';
	KeyMap[Qt::Key_Plus          ] = '+';
	//KeyMap[Qt::Key_H              ] = osgGA::GUIEventAdapter::KEY_KP_Home;
	//KeyMap[Qt::Key_                    ] = osgGA::GUIEventAdapter::KEY_KP_Up;
	//KeyMap[92                    ] = osgGA::GUIEventAdapter::KEY_KP_Page_Up;
	//KeyMap[86                    ] = osgGA::GUIEventAdapter::KEY_KP_Left;
	//KeyMap[87                    ] = osgGA::GUIEventAdapter::KEY_KP_Begin;
	//KeyMap[88                    ] = osgGA::GUIEventAdapter::KEY_KP_Right;
	//KeyMap[83                    ] = osgGA::GUIEventAdapter::KEY_KP_End;
	//KeyMap[84                    ] = osgGA::GUIEventAdapter::KEY_KP_Down;
	//KeyMap[85                    ] = osgGA::GUIEventAdapter::KEY_KP_Page_Down;
	KeyMap[Qt::Key_Insert        ] = osgGA::GUIEventAdapter::KEY_KP_Insert;
	//KeyMap[Qt::Key_Delete        ] = osgGA::GUIEventAdapter::KEY_KP_Delete;
}

void display_viewer::setCamera( osg::Camera * camera )
{
	viewer->setCamera(camera);
}

void display_viewer::setCamera( osg::Vec3d eye, osg::Vec3d center, osg::Vec3d up )
{
	viewer->getCamera()->setViewMatrixAsLookAt(eye, center, up);

	initGLCamera.center = center;
	initGLCamera.eye = eye;
	initGLCamera.up = up;
	initGLCamera.isSet = 1;

	if(viewer->getCameraManipulator() == NULL)
		return ;
	if(strcmp(viewer->getCameraManipulator()->className(), "TrackballManipulator") != 0)
		return;
	//viewer->cameramani
	osgGA::TrackballManipulator * cameraManip = (osgGA::TrackballManipulator *)viewer->getCameraManipulator();
	cameraManip->setTransformation(eye,center,up);
	forcePaintGL();
}





