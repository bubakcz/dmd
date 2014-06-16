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

#pragma once

#include <osg/Node>
#include <osg/NodeVisitor>
#include <unordered_set>
#include <osg/Texture2D>
#include <osg/Texture1D>


class imageNodeVisitor :
	public osg::NodeVisitor
{
public:
	imageNodeVisitor(void) : NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN) {};

	void apply(osg::Node &search_node)
	{		
		if (search_node.getStateSet()!=NULL)
		{
			osg::StateSet::TextureAttributeList list = search_node.getStateSet()->getTextureAttributeList();
			for (int i = 0; i < list.size(); i++)
			{
				for (osg::StateSet::AttributeList::iterator it = list[i].begin(); it!=list[i].end(); it++)
				{
					if((*it).second.first != NULL)
						if ((*it).second.first->asTexture())						
						{
							if ((*it).second.first->asTexture()->className() == "Texture2D")
								textureSet.emplace(dynamic_cast<osg::Texture2D *>((*it).second.first->asTexture())->getImage());;
							if ((*it).second.first->asTexture()->className() == "Texture1D")
								textureSet.emplace(dynamic_cast<osg::Texture1D *>((*it).second.first->asTexture())->getImage());;
						}
				}
				//textureSet.emplace((list[i]).);
			}
		}
		traverse(search_node);
	};

	std::unordered_set<osg::Image *> textureSet;

	~imageNodeVisitor(){return;};
};