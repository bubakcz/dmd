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

void shader_editor::initAttributeConvContainers()
{
	//attributeTypeToAttributeName 
	attributeTypeToAttributeName[osg::StateAttribute::TEXTURE 	] = "TEXTURE";
	attributeTypeToAttributeName[osg::StateAttribute::POLYGONMODE 	] = "POLYGONMODE";
	attributeTypeToAttributeName[osg::StateAttribute::POLYGONOFFSET 	] = "POLYGONOFFSET";
	attributeTypeToAttributeName[osg::StateAttribute::MATERIAL 	] = "MATERIAL";
	attributeTypeToAttributeName[osg::StateAttribute::ALPHAFUNC 	] = "ALPHAFUNC";
	attributeTypeToAttributeName[osg::StateAttribute::ANTIALIAS 	] = "ANTIALIAS";
	attributeTypeToAttributeName[osg::StateAttribute::COLORTABLE 	] = "COLORTABLE";
	attributeTypeToAttributeName[osg::StateAttribute::CULLFACE 	] = "CULLFACE";
	attributeTypeToAttributeName[osg::StateAttribute::FOG 	] = "FOG";
	attributeTypeToAttributeName[osg::StateAttribute::FRONTFACE 	] = "FRONTFACE";
	attributeTypeToAttributeName[osg::StateAttribute::LIGHT 	] = "LIGHT";
	attributeTypeToAttributeName[osg::StateAttribute::POINT 	] = "POINT";
	attributeTypeToAttributeName[osg::StateAttribute::LINEWIDTH 	] = "LINEWIDTH";
	attributeTypeToAttributeName[osg::StateAttribute::LINESTIPPLE 	] = "LINESTIPPLE";
	attributeTypeToAttributeName[osg::StateAttribute::POLYGONSTIPPLE 	] = "POLYGONSTIPPLE";
	attributeTypeToAttributeName[osg::StateAttribute::SHADEMODEL 	] = "SHADEMODEL";
	attributeTypeToAttributeName[osg::StateAttribute::TEXENV 	] = "TEXENV";
	attributeTypeToAttributeName[osg::StateAttribute::TEXENVFILTER 	] = "TEXENVFILTER";
	attributeTypeToAttributeName[osg::StateAttribute::TEXGEN 	] = "TEXGEN";
	attributeTypeToAttributeName[osg::StateAttribute::TEXMAT 	] = "TEXMAT";
	attributeTypeToAttributeName[osg::StateAttribute::LIGHTMODEL 	] = "LIGHTMODEL";
	attributeTypeToAttributeName[osg::StateAttribute::BLENDFUNC 	] = "BLENDFUNC";
	attributeTypeToAttributeName[osg::StateAttribute::BLENDEQUATION 	] = "BLENDEQUATION";
	attributeTypeToAttributeName[osg::StateAttribute::LOGICOP 	] = "LOGICOP";
	attributeTypeToAttributeName[osg::StateAttribute::STENCIL 	] = "STENCIL";
	attributeTypeToAttributeName[osg::StateAttribute::COLORMASK 	] = "COLORMASK";
	attributeTypeToAttributeName[osg::StateAttribute::DEPTH 	] = "DEPTH";
	attributeTypeToAttributeName[osg::StateAttribute::VIEWPORT 	] = "VIEWPORT";
	attributeTypeToAttributeName[osg::StateAttribute::SCISSOR 	] = "SCISSOR";
	attributeTypeToAttributeName[osg::StateAttribute::BLENDCOLOR 	] = "BLENDCOLOR";
	attributeTypeToAttributeName[osg::StateAttribute::MULTISAMPLE 	] = "MULTISAMPLE";
	attributeTypeToAttributeName[osg::StateAttribute::CLIPPLANE 	] = "CLIPPLANE";
	attributeTypeToAttributeName[osg::StateAttribute::COLORMATRIX 	] = "COLORMATRIX";
	attributeTypeToAttributeName[osg::StateAttribute::VERTEXPROGRAM 	] = "VERTEXPROGRAM";
	attributeTypeToAttributeName[osg::StateAttribute::FRAGMENTPROGRAM 	] = "FRAGMENTPROGRAM";
	attributeTypeToAttributeName[osg::StateAttribute::POINTSPRITE 	] = "POINTSPRITE";
	attributeTypeToAttributeName[osg::StateAttribute::PROGRAM 	] = "PROGRAM";
	attributeTypeToAttributeName[osg::StateAttribute::CLAMPCOLOR 	] = "CLAMPCOLOR";
	attributeTypeToAttributeName[osg::StateAttribute::HINT 	] = "HINT";
	attributeTypeToAttributeName[osg::StateAttribute::VIEWMATRIXEXTRACTOR 	] = "VIEWMATRIXEXTRACTOR";
	attributeTypeToAttributeName[osg::StateAttribute::OSGNVEXT_TEXTURE_SHADER 	] = "OSGNVEXT_TEXTURE_SHADER";
	attributeTypeToAttributeName[osg::StateAttribute::OSGNVEXT_VERTEX_PROGRAM 	] = "OSGNVEXT_VERTEX_PROGRAM";
	attributeTypeToAttributeName[osg::StateAttribute::OSGNVEXT_REGISTER_COMBINERS 	] = "OSGNVEXT_REGISTER_COMBINERS";
	attributeTypeToAttributeName[osg::StateAttribute::OSGNVSLANG_PROGRAM 	] = "OSGNVSLANG_PROGRAM";
	attributeTypeToAttributeName[osg::StateAttribute::OSGNVPARSE_PROGRAM_PARSER 	] = "OSGNVPARSE_PROGRAM_PARSER";

	attributeTypeVector.push_back(osg::StateAttribute::TEXTURE 	);
	attributeTypeVector.push_back(osg::StateAttribute::POLYGONMODE 	);
	attributeTypeVector.push_back(osg::StateAttribute::POLYGONOFFSET 	);
	attributeTypeVector.push_back(osg::StateAttribute::MATERIAL 	);
	attributeTypeVector.push_back(osg::StateAttribute::ALPHAFUNC 	);
	attributeTypeVector.push_back(osg::StateAttribute::ANTIALIAS 	);
	attributeTypeVector.push_back(osg::StateAttribute::COLORTABLE 	);
	attributeTypeVector.push_back(osg::StateAttribute::CULLFACE 	);
	attributeTypeVector.push_back(osg::StateAttribute::FOG 	);
	attributeTypeVector.push_back(osg::StateAttribute::FRONTFACE 	);
	attributeTypeVector.push_back(osg::StateAttribute::LIGHT 	);
	attributeTypeVector.push_back(osg::StateAttribute::POINT 	);
	attributeTypeVector.push_back(osg::StateAttribute::LINEWIDTH 	);
	attributeTypeVector.push_back(osg::StateAttribute::LINESTIPPLE 	);
	attributeTypeVector.push_back(osg::StateAttribute::POLYGONSTIPPLE 	);
	attributeTypeVector.push_back(osg::StateAttribute::SHADEMODEL 	);
	attributeTypeVector.push_back(osg::StateAttribute::TEXENV 	);
	attributeTypeVector.push_back(osg::StateAttribute::TEXENVFILTER 	);
	attributeTypeVector.push_back(osg::StateAttribute::TEXGEN 	);
	attributeTypeVector.push_back(osg::StateAttribute::TEXMAT 	);
	attributeTypeVector.push_back(osg::StateAttribute::LIGHTMODEL 	);
	attributeTypeVector.push_back(osg::StateAttribute::BLENDFUNC 	);
	attributeTypeVector.push_back(osg::StateAttribute::BLENDEQUATION 	);
	attributeTypeVector.push_back(osg::StateAttribute::LOGICOP 	);
	attributeTypeVector.push_back(osg::StateAttribute::STENCIL 	);
	attributeTypeVector.push_back(osg::StateAttribute::COLORMASK 	);
	attributeTypeVector.push_back(osg::StateAttribute::DEPTH 	);
	attributeTypeVector.push_back(osg::StateAttribute::VIEWPORT 	);
	attributeTypeVector.push_back(osg::StateAttribute::SCISSOR 	);
	attributeTypeVector.push_back(osg::StateAttribute::BLENDCOLOR 	);
	attributeTypeVector.push_back(osg::StateAttribute::MULTISAMPLE 	);
	attributeTypeVector.push_back(osg::StateAttribute::CLIPPLANE 	);
	attributeTypeVector.push_back(osg::StateAttribute::COLORMATRIX 	);
	attributeTypeVector.push_back(osg::StateAttribute::VERTEXPROGRAM 	);
	attributeTypeVector.push_back(osg::StateAttribute::FRAGMENTPROGRAM 	);
	attributeTypeVector.push_back(osg::StateAttribute::POINTSPRITE 	);
	attributeTypeVector.push_back(osg::StateAttribute::PROGRAM 	);
	attributeTypeVector.push_back(osg::StateAttribute::CLAMPCOLOR 	);
	attributeTypeVector.push_back(osg::StateAttribute::HINT 	);
	attributeTypeVector.push_back(osg::StateAttribute::VIEWMATRIXEXTRACTOR 	);
	attributeTypeVector.push_back(osg::StateAttribute::OSGNVEXT_TEXTURE_SHADER 	);
	attributeTypeVector.push_back(osg::StateAttribute::OSGNVEXT_VERTEX_PROGRAM 	);
	attributeTypeVector.push_back(osg::StateAttribute::OSGNVEXT_REGISTER_COMBINERS 	);
	attributeTypeVector.push_back(osg::StateAttribute::OSGNVSLANG_PROGRAM 	);
	attributeTypeVector.push_back(osg::StateAttribute::OSGNVPARSE_PROGRAM_PARSER 	);

	attributeValuesToValuesName[osg::StateAttribute::INHERIT 	] = "INHERIT";
	attributeValuesToValuesName[osg::StateAttribute::OFF 	] = "OFF";
	attributeValuesToValuesName[osg::StateAttribute::ON 	] = "ON";
	attributeValuesToValuesName[osg::StateAttribute::OVERRIDE 	] = "OVERRIDE";
	attributeValuesToValuesName[osg::StateAttribute::PROTECTED 	] = "PROTECTED";

	//valuesNamesToAttributeValues
	valuesNamesToAttributeValues["INHERIT" 	] = osg::StateAttribute::INHERIT;
	valuesNamesToAttributeValues["OFF" 	] =osg::StateAttribute::OFF;
	valuesNamesToAttributeValues["ON" 	] = osg::StateAttribute::ON;
	valuesNamesToAttributeValues["OVERRIDE" 	] = osg::StateAttribute::OVERRIDE;
	valuesNamesToAttributeValues["PROTECTED" 	] = osg::StateAttribute::PROTECTED;
}