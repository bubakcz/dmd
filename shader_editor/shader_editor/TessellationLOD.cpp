/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/
#include "TessellationLOD.h"
#include <osg/CullStack>

#include <algorithm>

using namespace osg;

TessellationLOD::TessellationLOD():
    _centerMode(USE_BOUNDING_SPHERE_CENTER),
    _radius(-1.0f),
//    _rangeMode(DISTANCE_FROM_EYE_POINT),
	_closestLevel(16.0f),
	_decreaseRate(16.0f/50.0f),
	_highestDistance(50.0f)
{
}

TessellationLOD::TessellationLOD(float closestLevel, float highestDistance):
_centerMode(USE_BOUNDING_SPHERE_CENTER),
	_radius(-1.0f),
//	_rangeMode(DISTANCE_FROM_EYE_POINT),
	_closestLevel(closestLevel),
	_decreaseRate(closestLevel / highestDistance),
	_highestDistance(highestDistance)
{
}


TessellationLOD::TessellationLOD(const TessellationLOD& lod,const CopyOp& copyop):
        Group(lod,copyop),
        _centerMode(lod._centerMode),
        _userDefinedCenter(lod._userDefinedCenter),
        _radius(lod._radius),
		_closestLevel(lod._closestLevel),
		_decreaseRate(lod._decreaseRate),
		_highestDistance(lod._highestDistance)
{
}


void TessellationLOD::traverse(NodeVisitor& nv)
{
    switch(nv.getTraversalMode())
    {
        case(NodeVisitor::TRAVERSE_ALL_CHILDREN):
            std::for_each(_children.begin(),_children.end(),NodeAcceptOp(nv));
            break;
        case(NodeVisitor::TRAVERSE_ACTIVE_CHILDREN):
        {
            float distance; 
			
			distance = nv.getDistanceToViewPoint(/*getCenter()*/getBound().center(),true);

			osg::Uniform * currentUniform;

			float level =  getCurrentLevel(distance);
			OSG_DEBUG << "TessellationLOD calculated level: " << level << std::endl;

			if((currentUniform = this->getOrCreateStateSet()->getUniform("DefaultTessLevel")) != NULL)
			{
				currentUniform->set(level);
			}
			else
				this->getOrCreateStateSet()->addUniform(new Uniform("DefaultTessLevel", level));

			if((currentUniform = this->getOrCreateStateSet()->getUniform("tcBBCenter")) != NULL)
			{
				currentUniform->set(getBound().center());
			}
			else
				this->getOrCreateStateSet()->addUniform(new Uniform("tcBBCenter", getBound().center()));

			if((currentUniform = this->getOrCreateStateSet()->getUniform("tcViewPointCenter")) != NULL)
			{
				currentUniform->set(nv.getViewPoint());
			}
			else
				this->getOrCreateStateSet()->addUniform(new Uniform("tcViewPointCenter", nv.getViewPoint()));

			for(CustomCenters::iterator it = _centerUniformMap.begin(); it != _centerUniformMap.end(); it++)
			{
				distance = nv.getDistanceToViewPoint(it->second,true);
				if(currentUniform = this->getOrCreateStateSet()->getUniform(it->first.c_str()))
					currentUniform->set(getCurrentLevel(distance));
				else
					this->getOrCreateStateSet()->addUniform(new Uniform(it->first.c_str(), getCurrentLevel(distance)));
			}

            unsigned int numChildren = _children.size();

            for(unsigned int i=0;i<numChildren;++i)
            {
				_children[i]->accept(nv);
            }
           break;
        }
        default:
            break;
    }
}

BoundingSphere TessellationLOD::computeBound() const
{
    if (_centerMode==USER_DEFINED_CENTER && _radius>=0.0f)
    {
        return BoundingSphere(_userDefinedCenter,_radius);
    }
    else if (_centerMode==UNION_OF_BOUNDING_SPHERE_AND_USER_DEFINED && _radius>=0.0f)
    {
        BoundingSphere bs = BoundingSphere(_userDefinedCenter,_radius);
        bs.expandBy(Group::computeBound());
        //alternative (used in TxpPagedLOD)
        // bs.expandRadiusBy(Group::computeBound());
        return bs;
    }
    else
    {
        return Group::computeBound();
    }
}

/*
bool LOD::addChild( Node *child )
{
    if (Group::addChild(child))
    {

        if (_children.size()>_rangeList.size())
        {
            float maxRange = !_rangeList.empty() ? _rangeList.back().second : 0.0f;

            _rangeList.resize(_children.size(),MinMaxPair(maxRange,maxRange));
        }

        return true;
    }
    return false;
}


bool LOD::addChild(Node *child, float min, float max)
{
    if (Group::addChild(child))
    {
        if (_children.size()>_rangeList.size()) _rangeList.resize(_children.size(),MinMaxPair(min,min));
        _rangeList[_children.size()-1].first = min;
        _rangeList[_children.size()-1].second = max;
        return true;
    }
    return false;
}

bool LOD::removeChildren( unsigned int pos,unsigned int numChildrenToRemove)
{
    if (pos<_rangeList.size()) _rangeList.erase(_rangeList.begin()+pos, osg::minimum(_rangeList.begin()+(pos+numChildrenToRemove), _rangeList.end()) );

    return Group::removeChildren(pos,numChildrenToRemove);
}

void LOD::setRange(unsigned int childNo, float min,float max)
{
    if (childNo>=_rangeList.size()) _rangeList.resize(childNo+1,MinMaxPair(min,min));
    _rangeList[childNo].first=min;
    _rangeList[childNo].second=max;
}*/
