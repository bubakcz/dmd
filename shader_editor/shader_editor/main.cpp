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

#include "shader_editor.h"
#include <fstream>
#include <QtGui/QApplication>
#include <QMessageBox>
#include <exception>
#include <QTextStream>
#include "logger.h"

shader_editor * w_g;
QTextStream qt_debug_output;

void QtMessageOutput(QtMsgType type, const char *msg)
{
	if(w_g)
		w_g->writeToConsoleQt( type, msg );
	if (qt_debug_output.device()!=NULL)
		qt_debug_output << msg << "\n";
}

int main(int argc, char *argv[])
{
	const GLubyte * test =  glGetString(GL_VENDOR);
	int glerror = glGetError();


	QFile qt_debug_output_file("defaultQtDebug.txt");
	if (qt_debug_output_file.open(QIODevice::WriteOnly | QIODevice::Text))
		qt_debug_output.setDevice(&qt_debug_output_file);


	//qt_debug_output = std::ofstream("defaultQtDebug.txt");
	QApplication a(argc, argv);
	shader_editor w;
	w_g = &w;
	logger::set_logger_target(&w);
	qInstallMsgHandler(QtMessageOutput);
	w.show();
	try
	{
		return a.exec();
	}
	catch (std::exception& e)
	{
		QMessageBox * this_exception = new QMessageBox;
		this_exception->setText(QString("Došlo k vyjímce:\n") + QString(e.what()));
		this_exception->show();
	}
	
}
