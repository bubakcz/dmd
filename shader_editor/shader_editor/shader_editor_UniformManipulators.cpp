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
    Funkce tøídy shader_editor, parsují a zapisují uniformy.
    Vyèlìneno pro pøehlednost    
*/

#include "shader_editor.h"
#include <iostream>
#include <sstream>
#include <stdio.h>

std::string * shader_editor::uniformToString( osg::Uniform * uniformRead )
{
	if (uniformRead == NULL)
		return NULL;

	float f;
	int i0,i1,i2,i3;
	unsigned int ui0,ui1,ui2,ui3;
	osg::Vec2 v2;
	osg::Vec3 v3;
	osg::Vec4 v4;
	osg::Matrix2 m2;
	osg::Matrix3 m3;
	osg::Matrixf mf4;
	osg::Matrixd md4;
	bool b0, b1, b2, b3;
	std::ostringstream ostring(std::ostringstream::out);
	std::string * output_str = new std::string;
	switch(uniformRead->getType())
	{
		case osg::Uniform::Type::FLOAT 	: {uniformRead->get(f); ostring << f; break;}
		case osg::Uniform::Type::FLOAT_VEC2 	: {uniformRead->get(v2); ostring << "fv2: ("<< v2.x() << " , " << v2.y() << ")"; break;}
		case osg::Uniform::Type::FLOAT_VEC3 	: {uniformRead->get(v3); ostring << "fv3: (" << v3.x() << " , " << v3.y() << " , " << v3.z() << ")"; break;}
		case osg::Uniform::Type::FLOAT_VEC4 	: {uniformRead->get(v4); ostring << "fv4: (" << v4.x() << " , " << v4.y() << " , " << v4.z() << " , " << v4.a() << ")"; break;}
		case osg::Uniform::Type::INT 	: {uniformRead->get(i0); ostring << i0; break;}
		case osg::Uniform::Type::INT_VEC2 	: {uniformRead->get(i0,i1); ostring << "iv2: ("<< i0 << " , " << i1 << ")"; break;}
		case osg::Uniform::Type::INT_VEC3 	: {uniformRead->get(i0,i1,i2); ostring << "iv3: (" << i0 << " , " << i1 << " , " << i2 << ")"; break;}
		case osg::Uniform::Type::INT_VEC4 	: {uniformRead->get(i0,i1,i2,i3); ostring << "iv4: (" << i0 << " , " << i1 << " , " << i2 << " , " << i3 << ")"; break;}
		case osg::Uniform::Type::BOOL 	: {uniformRead->get(i0); ostring << i0; break;}
		case osg::Uniform::Type::BOOL_VEC2 	: {uniformRead->get(b0,b1); ostring << "iv2: ("<< b0 << " , " << b1 << ")"; break;}
		case osg::Uniform::Type::BOOL_VEC3 	: {uniformRead->get(b0,b1,b2); ostring << "iv3: (" << b0 << " , " << b1 << " , " << b2 << ")"; break;}
		case osg::Uniform::Type::BOOL_VEC4 	: {uniformRead->get(b0,b1,b2,b3); ostring << "iv4: (" << b0 << " , " << b1 << " , " << b2 << " , " << b3 << ")"; break;}
		case osg::Uniform::Type::FLOAT_MAT2 	: {ostring << "FMAT2 - nelze editovat";break;}
		case osg::Uniform::Type::FLOAT_MAT3 	: {ostring << "FMAT3 - nelze editovat";break;}
		case osg::Uniform::Type::FLOAT_MAT4 	: {ostring << "FMAT4 - nelze editovat";break;}
		case osg::Uniform::Type::SAMPLER_1D 	: {ostring << "SAMPLER_1D - nelze editovat";break;}
		case osg::Uniform::Type::SAMPLER_2D 	: {ostring << "SAMPLER_2D - nelze editovat";break;}
		case osg::Uniform::Type::SAMPLER_3D 	: {ostring << "SAMPLER_3D - nelze editovat";break;}
		case osg::Uniform::Type::SAMPLER_CUBE 	: {ostring << "SAMPLER_CUBE - nelze editovat";break;}
		case osg::Uniform::Type::SAMPLER_1D_SHADOW 	: {ostring << "SAMPLER_1D_SHADOW - nelze editovat";break;}
		case osg::Uniform::Type::SAMPLER_2D_SHADOW 	: {ostring << "SAMPLER_2D_SHADOW - nelze editovat";break;}
		case osg::Uniform::Type::SAMPLER_1D_ARRAY 	: {ostring << "SAMPLER_1D_ARRAY - nelze editovat";break;}
		case osg::Uniform::Type::SAMPLER_2D_ARRAY 	: {ostring << "SAMPLER_2D_ARRAY - nelze editovat";break;}
		case osg::Uniform::Type::SAMPLER_1D_ARRAY_SHADOW 	: {ostring << "SAMPLER_1D_ARRAY_SHADOW - nelze editovat";break;}
		case osg::Uniform::Type::SAMPLER_2D_ARRAY_SHADOW 	: {ostring << "SAMPLER_2D_ARRAY_SHADOW - nelze editovat";break;}
		case osg::Uniform::Type::FLOAT_MAT2x3 	: {ostring << "FLOAT_MAT2x3 - nelze editovat";break;}
		case osg::Uniform::Type::FLOAT_MAT2x4 	: {ostring << "FLOAT_MAT2x4 - nelze editovat";break;}
		case osg::Uniform::Type::FLOAT_MAT3x2 	: {ostring << "FLOAT_MAT3x2 - nelze editovat";break;}
		case osg::Uniform::Type::FLOAT_MAT3x4 	: {ostring << "FLOAT_MAT3x4 - nelze editovat";break;}
		case osg::Uniform::Type::FLOAT_MAT4x2 	: {ostring << "FLOAT_MAT4x2 - nelze editovat";break;}
		case osg::Uniform::Type::FLOAT_MAT4x3 	: {ostring << "FLOAT_MAT4x3 - nelze editovat";break;}
		case osg::Uniform::Type::SAMPLER_BUFFER 	: {ostring << "SAMPLER_BUFFER - nelze editovat";break;}
		case osg::Uniform::Type::SAMPLER_CUBE_SHADOW 	: {ostring << "SAMPLER_CUBE_SHADOW - nelze editovat";break;}
		case osg::Uniform::Type::UNSIGNED_INT 	: {uniformRead->get(ui1); ostring << ui1; break;}
		case osg::Uniform::Type::UNSIGNED_INT_VEC2 	: {uniformRead->get(ui0,ui1); ostring << "uiv2: ("<< ui0 << " , " << ui1 << ")"; break;}
		case osg::Uniform::Type::UNSIGNED_INT_VEC3 	: {uniformRead->get(ui0,ui1,ui2); ostring << "uiv3: (" << ui0 << " , " << ui1 << " , " << ui2 << ")"; break;}
		case osg::Uniform::Type::UNSIGNED_INT_VEC4 	: {uniformRead->get(ui0,ui1,ui2,ui3); ostring << "uiv4: (" << ui0 << " , " << ui1 << " , " << ui2 << " , " << ui3 << ")"; break;}
		case osg::Uniform::Type::INT_SAMPLER_1D 	: {ostring << "INT_SAMPLER_1D - nelze editovat";break;}
		case osg::Uniform::Type::INT_SAMPLER_2D 	: {ostring << "INT_SAMPLER_2D - nelze editovat";break;}
		case osg::Uniform::Type::INT_SAMPLER_3D 	: {ostring << "INT_SAMPLER_3D - nelze editovat";break;}
		case osg::Uniform::Type::INT_SAMPLER_CUBE 	: {ostring << "INT_SAMPLER_CUBE - nelze editovat";break;}
		case osg::Uniform::Type::INT_SAMPLER_2D_RECT 	: {ostring << "INT_SAMPLER_2D_RECT - nelze editovat";break;}
		case osg::Uniform::Type::INT_SAMPLER_1D_ARRAY 	: {ostring << "INT_SAMPLER_1D_ARRAY - nelze editovat";break;}
		case osg::Uniform::Type::INT_SAMPLER_2D_ARRAY 	: {ostring << "INT_SAMPLER_2D_ARRAY - nelze editovat";break;}
		case osg::Uniform::Type::INT_SAMPLER_BUFFER 	: {ostring << "INT_SAMPLER_BUFFER - nelze editovat";break;}
		case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_1D 	: {ostring << "UNSIGNED_INT_SAMPLER_1D - nelze editovat";break;}
		case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_2D 	: {ostring << "UNSIGNED_INT_SAMPLER_2D - nelze editovat";break;}
		case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_3D 	: {ostring << "UNSIGNED_INT_SAMPLER_3D - nelze editovat";break;}
		case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_CUBE 	: {ostring << "UNSIGNED_INT_SAMPLER_CUBE - nelze editovat";break;}
		case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_2D_RECT 	: {ostring << "UNSIGNED_INT_SAMPLER_2D_RECT - nelze editovat";break;}
		case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_1D_ARRAY 	: {ostring << "UNSIGNED_INT_SAMPLER_1D_ARRAY - nelze editovat";break;}
		case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_2D_ARRAY 	: {ostring << "UNSIGNED_INT_SAMPLER_2D_ARRAY - nelze editovat";break;}
		case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_BUFFER 	: {ostring << "UNSIGNED_INT_SAMPLER_BUFFER - nelze editovat";break;}
		case osg::Uniform::Type::UNDEFINED : {ostring << "chyba - typ uniformu UNDEFINED";break;}
	}
	(*output_str) = ostring.str();
	return output_str;
}

bool shader_editor::stringToUniform( std::string * stringWrite, osg::Uniform * uniformWrite )
{
	if (uniformWrite == NULL || stringWrite == NULL)
		return false;

	float f;
	float f0,f1,f2,f3;
	int i0,i1,i2,i3;
	unsigned int ui0,ui1,ui2,ui3;
	osg::Vec2 v2;
	osg::Vec3 v3;
	osg::Vec4 v4;
	osg::Matrix2 m2;
	osg::Matrix3 m3;
	osg::Matrixf mf4;
	osg::Matrixd md4;
	bool b0, b1, b2, b3;
	std::ostringstream ostring(std::ostringstream::out);
	std::string * output_str = new std::string;
	switch(uniformWrite->getType())
	{
	case osg::Uniform::Type::FLOAT 	: {if(sscanf(stringWrite->c_str(),"%f",&f) <0) return false; uniformWrite->set(f); break;}
	case osg::Uniform::Type::FLOAT_VEC2 	: {if(sscanf(stringWrite->c_str(),"fv2: (%f , %f)",&f0, &f1) <0) return false; v2.set(f0,f1); uniformWrite->set(v2); break;}
	case osg::Uniform::Type::FLOAT_VEC3 	: {if(sscanf(stringWrite->c_str(),"fv3: (%f , %f , %f)",&f0, &f1, &f2) <0) return false; v3.set(f0,f1,f2); uniformWrite->set(v3); break;}
	case osg::Uniform::Type::FLOAT_VEC4 	: {if(sscanf(stringWrite->c_str(),"fv4: (%f , %f , %f, %f)",&f0, &f1,&f2, &f3) <0) return false; v4.set(f0,f1,f2,f3); uniformWrite->set(v4); break;}
	case osg::Uniform::Type::INT 	: {if(sscanf(stringWrite->c_str(),"%d",&i0) <0) return false; uniformWrite->set(i0); break;}
	case osg::Uniform::Type::INT_VEC2 	: {if(sscanf(stringWrite->c_str(),"iv2: (%d , %d)",&i0, &i1) <0) return false; uniformWrite->set(i0, i1); break;}
	case osg::Uniform::Type::INT_VEC3 	: {if(sscanf(stringWrite->c_str(),"iv3: (%d , %d , %d)",&i0, &i1, &i2) <0) return false; uniformWrite->set(i0, i1, i2); break;}
	case osg::Uniform::Type::INT_VEC4 	: {if(sscanf(stringWrite->c_str(),"iv4: (%d , %d , %d, %d)",&i0, &i1, &i2, &i3) <0) return false; uniformWrite->set(i0, i1, i2, i3); break;}
	case osg::Uniform::Type::BOOL 	: {if((*stringWrite)=="t") uniformWrite->set(true); else if((*stringWrite)=="f") uniformWrite->set(false); else return false; break;}
// 	case osg::Uniform::Type::BOOL_VEC2 	: {uniformWrite->get(b0,b1); ostring << "iv2: ("<< b0 << " , " << b1 << ")"; break;}
// 	case osg::Uniform::Type::BOOL_VEC3 	: {uniformWrite->get(b0,b1,b2); ostring << "iv3: (" << b0 << " , " << b1 << " , " << b2 << ")"; break;}
// 	case osg::Uniform::Type::BOOL_VEC4 	: {uniformWrite->get(b0,b1,b2,b3); ostring << "iv4: (" << b0 << " , " << b1 << " , " << b2 << " , " << b3 << ")"; break;}
// 	case osg::Uniform::Type::FLOAT_MAT2 	: {ostring << "FMAT2 - nelze editovat";break;}
// 	case osg::Uniform::Type::FLOAT_MAT3 	: {ostring << "FMAT3 - nelze editovat";break;}
// 	case osg::Uniform::Type::FLOAT_MAT4 	: {ostring << "FMAT4 - nelze editovat";break;}
// 	case osg::Uniform::Type::SAMPLER_1D 	: {ostring << "SAMPLER_1D - nelze editovat";break;}
// 	case osg::Uniform::Type::SAMPLER_2D 	: {ostring << "SAMPLER_2D - nelze editovat";break;}
// 	case osg::Uniform::Type::SAMPLER_3D 	: {ostring << "SAMPLER_3D - nelze editovat";break;}
// 	case osg::Uniform::Type::SAMPLER_CUBE 	: {ostring << "SAMPLER_CUBE - nelze editovat";break;}
// 	case osg::Uniform::Type::SAMPLER_1D_SHADOW 	: {ostring << "SAMPLER_1D_SHADOW - nelze editovat";break;}
// 	case osg::Uniform::Type::SAMPLER_2D_SHADOW 	: {ostring << "SAMPLER_2D_SHADOW - nelze editovat";break;}
// 	case osg::Uniform::Type::SAMPLER_1D_ARRAY 	: {ostring << "SAMPLER_1D_ARRAY - nelze editovat";break;}
// 	case osg::Uniform::Type::SAMPLER_2D_ARRAY 	: {ostring << "SAMPLER_2D_ARRAY - nelze editovat";break;}
// 	case osg::Uniform::Type::SAMPLER_1D_ARRAY_SHADOW 	: {ostring << "SAMPLER_1D_ARRAY_SHADOW - nelze editovat";break;}
// 	case osg::Uniform::Type::SAMPLER_2D_ARRAY_SHADOW 	: {ostring << "SAMPLER_2D_ARRAY_SHADOW - nelze editovat";break;}
// 	case osg::Uniform::Type::FLOAT_MAT2x3 	: {ostring << "FLOAT_MAT2x3 - nelze editovat";break;}
// 	case osg::Uniform::Type::FLOAT_MAT2x4 	: {ostring << "FLOAT_MAT2x4 - nelze editovat";break;}
// 	case osg::Uniform::Type::FLOAT_MAT3x2 	: {ostring << "FLOAT_MAT3x2 - nelze editovat";break;}
// 	case osg::Uniform::Type::FLOAT_MAT3x4 	: {ostring << "FLOAT_MAT3x4 - nelze editovat";break;}
// 	case osg::Uniform::Type::FLOAT_MAT4x2 	: {ostring << "FLOAT_MAT4x2 - nelze editovat";break;}
// 	case osg::Uniform::Type::FLOAT_MAT4x3 	: {ostring << "FLOAT_MAT4x3 - nelze editovat";break;}
// 	case osg::Uniform::Type::SAMPLER_BUFFER 	: {ostring << "SAMPLER_BUFFER - nelze editovat";break;}
// 	case osg::Uniform::Type::SAMPLER_CUBE_SHADOW 	: {ostring << "SAMPLER_CUBE_SHADOW - nelze editovat";break;}
	case osg::Uniform::Type::UNSIGNED_INT 	: {if(sscanf(stringWrite->c_str(),"%d",&ui0) <0) return false; uniformWrite->set(ui0); break;}
	case osg::Uniform::Type::UNSIGNED_INT_VEC2 	: {if(sscanf(stringWrite->c_str(),"iv2: (%d , %d)",&ui0, &ui1) <0) return false; uniformWrite->set(ui0, ui1); break;}
	case osg::Uniform::Type::UNSIGNED_INT_VEC3 	: {if(sscanf(stringWrite->c_str(),"iv3: (%d , %d , %d)",&ui0, &ui1, &ui2) <0) return false; uniformWrite->set(ui0, ui1, ui2); break;}
	case osg::Uniform::Type::UNSIGNED_INT_VEC4 	: {if(sscanf(stringWrite->c_str(),"iv4: (%d , %d , %d, %d)",&ui0, &ui1, &ui2, &ui3) <0) return false; uniformWrite->set(ui0, ui1, ui2, ui3); break;}
// 	case osg::Uniform::Type::INT_SAMPLER_1D 	: {ostring << "INT_SAMPLER_1D - nelze editovat";break;}
// 	case osg::Uniform::Type::INT_SAMPLER_2D 	: {ostring << "INT_SAMPLER_2D - nelze editovat";break;}
// 	case osg::Uniform::Type::INT_SAMPLER_3D 	: {ostring << "INT_SAMPLER_3D - nelze editovat";break;}
// 	case osg::Uniform::Type::INT_SAMPLER_CUBE 	: {ostring << "INT_SAMPLER_CUBE - nelze editovat";break;}
// 	case osg::Uniform::Type::INT_SAMPLER_2D_RECT 	: {ostring << "INT_SAMPLER_2D_RECT - nelze editovat";break;}
// 	case osg::Uniform::Type::INT_SAMPLER_1D_ARRAY 	: {ostring << "INT_SAMPLER_1D_ARRAY - nelze editovat";break;}
// 	case osg::Uniform::Type::INT_SAMPLER_2D_ARRAY 	: {ostring << "INT_SAMPLER_2D_ARRAY - nelze editovat";break;}
// 	case osg::Uniform::Type::INT_SAMPLER_BUFFER 	: {ostring << "INT_SAMPLER_BUFFER - nelze editovat";break;}
// 	case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_1D 	: {ostring << "UNSIGNED_INT_SAMPLER_1D - nelze editovat";break;}
// 	case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_2D 	: {ostring << "UNSIGNED_INT_SAMPLER_2D - nelze editovat";break;}
// 	case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_3D 	: {ostring << "UNSIGNED_INT_SAMPLER_3D - nelze editovat";break;}
// 	case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_CUBE 	: {ostring << "UNSIGNED_INT_SAMPLER_CUBE - nelze editovat";break;}
// 	case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_2D_RECT 	: {ostring << "UNSIGNED_INT_SAMPLER_2D_RECT - nelze editovat";break;}
// 	case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_1D_ARRAY 	: {ostring << "UNSIGNED_INT_SAMPLER_1D_ARRAY - nelze editovat";break;}
// 	case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_2D_ARRAY 	: {ostring << "UNSIGNED_INT_SAMPLER_2D_ARRAY - nelze editovat";break;}
// 	case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_BUFFER 	: {ostring << "UNSIGNED_INT_SAMPLER_BUFFER - nelze editovat";break;}
// 	case osg::Uniform::Type::UNDEFINED : {ostring << "chyba - typ uniformu UNDEFINED";break;}
	default: break;
	}
	return true;
}

void shader_editor::initMap_uniformType_isEditable()
{
	uniformType_isEditable[osg::Uniform::Type::FLOAT]=true;	
	uniformType_name[osg::Uniform::Type::FLOAT]	= "FLOAT";
	uniformType_isEditable[osg::Uniform::Type::FLOAT_VEC2]=true;	
	uniformType_name[osg::Uniform::Type::FLOAT_VEC2] = "FLOAT_VEC2";
	uniformType_isEditable[osg::Uniform::Type::FLOAT_VEC3]=true;	
	uniformType_name[osg::Uniform::Type::FLOAT_VEC3] = "FLOAT_VEC3";
	uniformType_isEditable[osg::Uniform::Type::FLOAT_VEC4]=true;	
	uniformType_name[osg::Uniform::Type::FLOAT_VEC4] = "FLOAT_VEC4";
	uniformType_isEditable[osg::Uniform::Type::INT]=true;	
	uniformType_name[osg::Uniform::Type::INT] = "INT";
	uniformType_isEditable[osg::Uniform::Type::INT_VEC2]=true;	
	uniformType_name[osg::Uniform::Type::INT_VEC2] = "INT_VEC2";
	uniformType_isEditable[osg::Uniform::Type::INT_VEC3]=true;	
	uniformType_name[osg::Uniform::Type::INT_VEC3] = "INT_VEC3";
	uniformType_isEditable[osg::Uniform::Type::INT_VEC4]=true;	
	uniformType_name[osg::Uniform::Type::INT_VEC4] = "INT_VEC4";
	uniformType_isEditable[osg::Uniform::Type::BOOL]=true;	
	uniformType_name[osg::Uniform::Type::BOOL] = "BOOL";
	uniformType_isEditable[osg::Uniform::Type::BOOL_VEC2]=false;	
	uniformType_name[osg::Uniform::Type::BOOL_VEC2] = "BOOL_VEC2";
	uniformType_isEditable[osg::Uniform::Type::BOOL_VEC3]=false;	
	uniformType_name[osg::Uniform::Type::BOOL_VEC3] = "BOOL_VEC3";
	uniformType_isEditable[osg::Uniform::Type::BOOL_VEC4]=false;	
	uniformType_name[osg::Uniform::Type::BOOL_VEC4] = "BOOL_VEC4";
	uniformType_isEditable[osg::Uniform::Type::FLOAT_MAT2]=false;	
	uniformType_name[osg::Uniform::Type::FLOAT_MAT2] = "FLOAT_MAT2";
	uniformType_isEditable[osg::Uniform::Type::FLOAT_MAT3]=false;	
	uniformType_name[osg::Uniform::Type::FLOAT_MAT3] = "FLOAT_MAT3";
	uniformType_isEditable[osg::Uniform::Type::FLOAT_MAT4]=false;	
	uniformType_name[osg::Uniform::Type::FLOAT_MAT4] = "FLOAT_MAT4";
	uniformType_isEditable[osg::Uniform::Type::SAMPLER_1D]=false;	
	uniformType_name[osg::Uniform::Type::SAMPLER_1D] = "SAMPLER_1D";
	uniformType_isEditable[osg::Uniform::Type::SAMPLER_2D]=false;	
	uniformType_name[osg::Uniform::Type::SAMPLER_2D] = "SAMPLER_2D";
	uniformType_isEditable[osg::Uniform::Type::SAMPLER_3D]=false;	
	uniformType_name[osg::Uniform::Type::SAMPLER_3D] = "SAMPLER_3D";
	uniformType_isEditable[osg::Uniform::Type::SAMPLER_CUBE]=false;	
	uniformType_name[osg::Uniform::Type::SAMPLER_CUBE] = "SAMPLER_CUBE";
	uniformType_isEditable[osg::Uniform::Type::SAMPLER_1D_SHADOW]=false;	
	uniformType_name[osg::Uniform::Type::SAMPLER_1D_SHADOW] = "SAMPLER_1D_SHADOW";
	uniformType_isEditable[osg::Uniform::Type::SAMPLER_2D_SHADOW]=false;	
	uniformType_name[osg::Uniform::Type::SAMPLER_2D_SHADOW] = "SAMPLER_2D_SHADOW";
	uniformType_isEditable[osg::Uniform::Type::SAMPLER_1D_ARRAY]=false;	
	uniformType_name[osg::Uniform::Type::SAMPLER_1D_ARRAY] = "SAMPLER_1D_ARRAY";
	uniformType_isEditable[osg::Uniform::Type::SAMPLER_2D_ARRAY]=false;	
	uniformType_name[osg::Uniform::Type::SAMPLER_2D_ARRAY] = "SAMPLER_2D_ARRAY";
	uniformType_isEditable[osg::Uniform::Type::SAMPLER_1D_ARRAY_SHADOW]=false;	
	uniformType_name[osg::Uniform::Type::SAMPLER_1D_ARRAY_SHADOW] = "SAMPLER_1D_ARRAY_SHADOW";
	uniformType_isEditable[osg::Uniform::Type::SAMPLER_2D_ARRAY_SHADOW]=false;	
	uniformType_name[osg::Uniform::Type::SAMPLER_2D_ARRAY_SHADOW] = "SAMPLER_2D_ARRAY_SHADOW";
	uniformType_isEditable[osg::Uniform::Type::FLOAT_MAT2x3]=false;	
	uniformType_name[osg::Uniform::Type::FLOAT_MAT2x3] = "FLOAT_MAT2x3";
	uniformType_isEditable[osg::Uniform::Type::FLOAT_MAT2x4]=false;	
	uniformType_name[osg::Uniform::Type::FLOAT_MAT2x4] = "FLOAT_MAT2x4";
	uniformType_isEditable[osg::Uniform::Type::FLOAT_MAT3x2]=false;	
	uniformType_name[osg::Uniform::Type::FLOAT_MAT3x2] = "FLOAT_MAT3x2";
	uniformType_isEditable[osg::Uniform::Type::FLOAT_MAT3x4]=false;	
	uniformType_name[osg::Uniform::Type::FLOAT_MAT3x4] = "FLOAT_MAT3x4";
	uniformType_isEditable[osg::Uniform::Type::FLOAT_MAT4x2]=false;	
	uniformType_name[osg::Uniform::Type::FLOAT_MAT4x2] = "FLOAT_MAT4x2";
	uniformType_isEditable[osg::Uniform::Type::FLOAT_MAT4x3]=false;	
	uniformType_name[osg::Uniform::Type::FLOAT_MAT4x3] = "FLOAT_MAT4x3";
	uniformType_isEditable[osg::Uniform::Type::SAMPLER_BUFFER]=false;	
	uniformType_name[osg::Uniform::Type::SAMPLER_BUFFER] = "SAMPLER_BUFFER";
	uniformType_isEditable[osg::Uniform::Type::SAMPLER_CUBE_SHADOW]=false;	
	uniformType_name[osg::Uniform::Type::SAMPLER_CUBE_SHADOW] = "SAMPLER_CUBE_SHADOW";
	uniformType_isEditable[osg::Uniform::Type::UNSIGNED_INT]=true;	
	uniformType_name[osg::Uniform::Type::UNSIGNED_INT] = "UNSIGNED_INT";
	uniformType_isEditable[osg::Uniform::Type::UNSIGNED_INT_VEC2]=true;	
	uniformType_name[osg::Uniform::Type::UNSIGNED_INT_VEC2] = "UNSIGNED_INT_VEC2";
	uniformType_isEditable[osg::Uniform::Type::UNSIGNED_INT_VEC3]=true;	
	uniformType_name[osg::Uniform::Type::UNSIGNED_INT_VEC3] = "UNSIGNED_INT_VEC3";
	uniformType_isEditable[osg::Uniform::Type::UNSIGNED_INT_VEC4]=true;	
	uniformType_name[osg::Uniform::Type::UNSIGNED_INT_VEC4] = "UNSIGNED_INT_VEC4";
	uniformType_isEditable[osg::Uniform::Type::INT_SAMPLER_1D]=false;	
	uniformType_name[osg::Uniform::Type::INT_SAMPLER_1D] = "INT_SAMPLER_1D";
	uniformType_isEditable[osg::Uniform::Type::INT_SAMPLER_2D]=false;	
	uniformType_name[osg::Uniform::Type::INT_SAMPLER_2D] = "INT_SAMPLER_2D";
	uniformType_isEditable[osg::Uniform::Type::INT_SAMPLER_3D]=false;	
	uniformType_name[osg::Uniform::Type::INT_SAMPLER_3D] = "INT_SAMPLER_3D";
	uniformType_isEditable[osg::Uniform::Type::INT_SAMPLER_CUBE]=false;	
	uniformType_name[osg::Uniform::Type::INT_SAMPLER_CUBE] = "INT_SAMPLER_CUBE";
	uniformType_isEditable[osg::Uniform::Type::INT_SAMPLER_2D_RECT]=false;	
	uniformType_name[osg::Uniform::Type::INT_SAMPLER_2D_RECT] = "INT_SAMPLER_2D_RECT";
	uniformType_isEditable[osg::Uniform::Type::INT_SAMPLER_1D_ARRAY]=false;	
	uniformType_name[osg::Uniform::Type::INT_SAMPLER_1D_ARRAY] = "INT_SAMPLER_1D_ARRAY";
	uniformType_isEditable[osg::Uniform::Type::INT_SAMPLER_2D_ARRAY]=false;	
	uniformType_name[osg::Uniform::Type::INT_SAMPLER_2D_ARRAY] = "INT_SAMPLER_2D_ARRAY";
	uniformType_isEditable[osg::Uniform::Type::INT_SAMPLER_BUFFER]=false;	
	uniformType_name[osg::Uniform::Type::INT_SAMPLER_BUFFER] = "INT_SAMPLER_BUFFER";
	uniformType_isEditable[osg::Uniform::Type::UNSIGNED_INT_SAMPLER_1D]=false;	
	uniformType_name[osg::Uniform::Type::UNSIGNED_INT_SAMPLER_1D] = "UNSIGNED_INT_SAMPLER_1D";
	uniformType_isEditable[osg::Uniform::Type::UNSIGNED_INT_SAMPLER_2D]=false;	
	uniformType_name[osg::Uniform::Type::UNSIGNED_INT_SAMPLER_2D] = "UNSIGNED_INT_SAMPLER_2D";
	uniformType_isEditable[osg::Uniform::Type::UNSIGNED_INT_SAMPLER_3D]=false;	
	uniformType_name[osg::Uniform::Type::UNSIGNED_INT_SAMPLER_3D] = "UNSIGNED_INT_SAMPLER_3D";
	uniformType_isEditable[osg::Uniform::Type::UNSIGNED_INT_SAMPLER_CUBE]=false;	
	uniformType_name[osg::Uniform::Type::UNSIGNED_INT_SAMPLER_CUBE] = "UNSIGNED_INT_SAMPLER_CUBE";
	uniformType_isEditable[osg::Uniform::Type::UNSIGNED_INT_SAMPLER_2D_RECT]=false;	
	uniformType_name[osg::Uniform::Type::UNSIGNED_INT_SAMPLER_2D_RECT] = "UNSIGNED_INT_SAMPLER_2D_RECT";
	uniformType_isEditable[osg::Uniform::Type::UNSIGNED_INT_SAMPLER_1D_ARRAY]=false;	
	uniformType_name[osg::Uniform::Type::UNSIGNED_INT_SAMPLER_1D_ARRAY] = "UNSIGNED_INT_SAMPLER_1D_ARRAY";
	uniformType_isEditable[osg::Uniform::Type::UNSIGNED_INT_SAMPLER_2D_ARRAY]=false;	
	uniformType_name[osg::Uniform::Type::UNSIGNED_INT_SAMPLER_2D_ARRAY] = "UNSIGNED_INT_SAMPLER_2D_ARRAY";
	uniformType_isEditable[osg::Uniform::Type::UNSIGNED_INT_SAMPLER_BUFFER]=false;	
	uniformType_name[osg::Uniform::Type::UNSIGNED_INT_SAMPLER_BUFFER] = "UNSIGNED_INT_SAMPLER_BUFFER";
	uniformType_isEditable[osg::Uniform::Type::UNDEFINED]=false;	
	uniformType_name[osg::Uniform::Type::UNDEFINED] = "UNDEFINED";
}

std::string *  shader_editor::uniformProtovalue(osg::Uniform::Type type)
{
	std::ostringstream ostring(std::ostringstream::out);
	std::string * output_str = new std::string;
	switch (type)
	{
			case osg::Uniform::Type::FLOAT 	: {ostring << "0.0"; break;}
			case osg::Uniform::Type::FLOAT_VEC2 	: {ostring << "fv2: ("<< "0.0" << " , " << "0.0" << ")"; break;}
			case osg::Uniform::Type::FLOAT_VEC3 	: {ostring << "fv3: (" << "0.0" << " , " << "0.0" << " , " << "0.0" << ")"; break;}
			case osg::Uniform::Type::FLOAT_VEC4 	: {ostring << "fv4: (" << "0.0" << " , " << "0.0" << " , " << "0.0" << " , " << "0.0" << ")"; break;}
			case osg::Uniform::Type::INT 	: {ostring << "0"; break;}
			case osg::Uniform::Type::INT_VEC2 	: {ostring << "iv2: ("<< "0" << " , " << "0" << ")"; break;}
			case osg::Uniform::Type::INT_VEC3 	: {ostring << "iv3: (" << "0" << " , " << "0" << " , " << "0" << ")"; break;}
			case osg::Uniform::Type::INT_VEC4 	: {ostring << "iv4: (" << "0" << " , " << "0" << " , " << "0" << " , " << "0" << ")"; break;}
			case osg::Uniform::Type::BOOL 	: {ostring << "t"; break;}
			case osg::Uniform::Type::BOOL_VEC2 	: {ostring << "iv2: ("<< "t" << " , " << "t" << ")"; break;}
			case osg::Uniform::Type::BOOL_VEC3 	: {ostring << "iv3: (" << "t" << " , " << "t" << " , " << "t" << ")"; break;}
			case osg::Uniform::Type::BOOL_VEC4 	: {ostring << "iv4: (" << "t" << " , " << "t" << " , " << "t" << " , " << "t" << ")"; break;}
			case osg::Uniform::Type::FLOAT_MAT2 	: {ostring << "FMAT2 - nelze editovat";break;}
			case osg::Uniform::Type::FLOAT_MAT3 	: {ostring << "FMAT3 - nelze editovat";break;}
			case osg::Uniform::Type::FLOAT_MAT4 	: {ostring << "FMAT4 - nelze editovat";break;}
			case osg::Uniform::Type::SAMPLER_1D 	: {ostring << "SAMPLER_1D - nelze editovat";break;}
			case osg::Uniform::Type::SAMPLER_2D 	: {ostring << "SAMPLER_2D - nelze editovat";break;}
			case osg::Uniform::Type::SAMPLER_3D 	: {ostring << "SAMPLER_3D - nelze editovat";break;}
			case osg::Uniform::Type::SAMPLER_CUBE 	: {ostring << "SAMPLER_CUBE - nelze editovat";break;}
			case osg::Uniform::Type::SAMPLER_1D_SHADOW 	: {ostring << "SAMPLER_1D_SHADOW - nelze editovat";break;}
			case osg::Uniform::Type::SAMPLER_2D_SHADOW 	: {ostring << "SAMPLER_2D_SHADOW - nelze editovat";break;}
			case osg::Uniform::Type::SAMPLER_1D_ARRAY 	: {ostring << "SAMPLER_1D_ARRAY - nelze editovat";break;}
			case osg::Uniform::Type::SAMPLER_2D_ARRAY 	: {ostring << "SAMPLER_2D_ARRAY - nelze editovat";break;}
			case osg::Uniform::Type::SAMPLER_1D_ARRAY_SHADOW 	: {ostring << "SAMPLER_1D_ARRAY_SHADOW - nelze editovat";break;}
			case osg::Uniform::Type::SAMPLER_2D_ARRAY_SHADOW 	: {ostring << "SAMPLER_2D_ARRAY_SHADOW - nelze editovat";break;}
			case osg::Uniform::Type::FLOAT_MAT2x3 	: {ostring << "FLOAT_MAT2x3 - nelze editovat";break;}
			case osg::Uniform::Type::FLOAT_MAT2x4 	: {ostring << "FLOAT_MAT2x4 - nelze editovat";break;}
			case osg::Uniform::Type::FLOAT_MAT3x2 	: {ostring << "FLOAT_MAT3x2 - nelze editovat";break;}
			case osg::Uniform::Type::FLOAT_MAT3x4 	: {ostring << "FLOAT_MAT3x4 - nelze editovat";break;}
			case osg::Uniform::Type::FLOAT_MAT4x2 	: {ostring << "FLOAT_MAT4x2 - nelze editovat";break;}
			case osg::Uniform::Type::FLOAT_MAT4x3 	: {ostring << "FLOAT_MAT4x3 - nelze editovat";break;}
			case osg::Uniform::Type::SAMPLER_BUFFER 	: {ostring << "SAMPLER_BUFFER - nelze editovat";break;}
			case osg::Uniform::Type::SAMPLER_CUBE_SHADOW 	: {ostring << "SAMPLER_CUBE_SHADOW - nelze editovat";break;}
			case osg::Uniform::Type::UNSIGNED_INT 	: {ostring << "0"; break;}
			case osg::Uniform::Type::UNSIGNED_INT_VEC2 	: {ostring << "uiv2: ("<< "0" << " , " << "0" << ")"; break;}
			case osg::Uniform::Type::UNSIGNED_INT_VEC3 	: {ostring << "uiv3: (" << "0" << " , " << "0" << " , " << "0" << ")"; break;}
			case osg::Uniform::Type::UNSIGNED_INT_VEC4 	: {ostring << "uiv4: (" << "0" << " , " << "0" << " , " << "0" << " , " << "0" << ")"; break;}
			case osg::Uniform::Type::INT_SAMPLER_1D 	: {ostring << "INT_SAMPLER_1D - nelze editovat";break;}
			case osg::Uniform::Type::INT_SAMPLER_2D 	: {ostring << "INT_SAMPLER_2D - nelze editovat";break;}
			case osg::Uniform::Type::INT_SAMPLER_3D 	: {ostring << "INT_SAMPLER_3D - nelze editovat";break;}
			case osg::Uniform::Type::INT_SAMPLER_CUBE 	: {ostring << "INT_SAMPLER_CUBE - nelze editovat";break;}
			case osg::Uniform::Type::INT_SAMPLER_2D_RECT 	: {ostring << "INT_SAMPLER_2D_RECT - nelze editovat";break;}
			case osg::Uniform::Type::INT_SAMPLER_1D_ARRAY 	: {ostring << "INT_SAMPLER_1D_ARRAY - nelze editovat";break;}
			case osg::Uniform::Type::INT_SAMPLER_2D_ARRAY 	: {ostring << "INT_SAMPLER_2D_ARRAY - nelze editovat";break;}
			case osg::Uniform::Type::INT_SAMPLER_BUFFER 	: {ostring << "INT_SAMPLER_BUFFER - nelze editovat";break;}
			case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_1D 	: {ostring << "UNSIGNED_INT_SAMPLER_1D - nelze editovat";break;}
			case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_2D 	: {ostring << "UNSIGNED_INT_SAMPLER_2D - nelze editovat";break;}
			case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_3D 	: {ostring << "UNSIGNED_INT_SAMPLER_3D - nelze editovat";break;}
			case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_CUBE 	: {ostring << "UNSIGNED_INT_SAMPLER_CUBE - nelze editovat";break;}
			case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_2D_RECT 	: {ostring << "UNSIGNED_INT_SAMPLER_2D_RECT - nelze editovat";break;}
			case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_1D_ARRAY 	: {ostring << "UNSIGNED_INT_SAMPLER_1D_ARRAY - nelze editovat";break;}
			case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_2D_ARRAY 	: {ostring << "UNSIGNED_INT_SAMPLER_2D_ARRAY - nelze editovat";break;}
			case osg::Uniform::Type::UNSIGNED_INT_SAMPLER_BUFFER 	: {ostring << "UNSIGNED_INT_SAMPLER_BUFFER - nelze editovat";break;}
			case osg::Uniform::Type::UNDEFINED : {ostring << "UNDEFINED";break;}
	}
	(*output_str) = ostring.str();
	return output_str;
}