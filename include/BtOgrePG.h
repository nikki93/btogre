/*
 * =====================================================================================
 *
 *       Filename:  BtOgrePG.h
 *
 *    Description:  The part of BtOgre that handles information transfer from Bullet to 
 *                  Ogre (like updating graphics object positions).
 *
 *        Version:  1.0
 *        Created:  27/12/2008 03:40:56 AM
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef _BtOgreGP_H_
#define _BtOgreGP_H_

#include "btBulletDynamicsCommon.h"
#include "OgreSceneNode.h"
#include "BtOgreExtras.h"

namespace BtOgre {

//A MotionState is Bullet's way of informing you about updates to an object.
//Pass this MotionState to a btRigidBody to have your SceneNode updated automaticaly.
class RigidBodyState : public btMotionState 
{
    public:
	RigidBodyState(const btTransform &transform, Ogre::SceneNode *node)
	    : mNode(node),
	    mTransform(transform)
	{
	}

	RigidBodyState(Ogre::SceneNode *node)
	    : mNode(node),
	      mTransform(((node != NULL) ? BtOgre::Convert::toBullet(node->getOrientation()) : btQuaternion(0,0,0,1)), 
		         ((node != NULL) ? BtOgre::Convert::toBullet(node->getPosition())    : btVector3(0,0,0)))
	{
	    
	}

	virtual ~RigidBodyState() 
	{
	}

	void setNode(Ogre::SceneNode *node) 
	{
	    mNode = node;
	}

	virtual void getWorldTransform(btTransform &retVal) const 
	{
	    retVal = mTransform;
	}

	void setKinematicPos(btTransform &transform) 
	{
	    mTransform = transform;
	}

	virtual void setWorldTransform(const btTransform &transform) 
	{
	    if (mNode == NULL)
		return;

	    btQuaternion rot = transform.getRotation();
	    mNode->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
	    btVector3 pos = transform.getOrigin();
	    mNode->setPosition(pos.x(), pos.y(), pos.z());
	}

    protected:
	Ogre::SceneNode *mNode;
	btTransform mTransform;
};

//Softbody-Ogre connection goes here!

}

#endif
