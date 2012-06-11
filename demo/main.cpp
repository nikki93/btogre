/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  BtOgre test application, main file.
 *
 *        Version:  1.0
 *        Created:  01/14/2009 05:48:31 PM
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#include "ExampleApplication.h"
#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BtOgreExtras.h"

using namespace Ogre;

/*
 * =====================================================================================
 *    Namespace:  Globals
 *  Description:  A dirty 'globals' hack.
 * =====================================================================================
 */

namespace Globals
{
    btDynamicsWorld *phyWorld;
    BtOgre::DebugDrawer *dbgdraw;
}

/*
 * =====================================================================================
 *        Class:  BtOgreTestFrameListener
 *  Description:  Derives from ExampleFrameListener and overrides stuf.
 * =====================================================================================
 */

class BtOgreTestFrameListener : public ExampleFrameListener
{
    public:
	BtOgreTestFrameListener(RenderWindow* win, Camera* cam, SceneManager *sceneMgr)
	    : ExampleFrameListener(win, cam, false, false)
	{
	}

	bool frameStarted(const FrameEvent &evt)
	{
	    //Update Bullet world. Don't forget the debugDrawWorld() part!
	    Globals::phyWorld->stepSimulation(evt.timeSinceLastFrame, 10);
	    Globals::phyWorld->debugDrawWorld();

	    //Shows debug if F3 key down.
	    Globals::dbgdraw->setDebugMode(mKeyboard->isKeyDown(OIS::KC_F3));
	    Globals::dbgdraw->step();

	    return ExampleFrameListener::frameStarted(evt);
	}
};

/*
 * =====================================================================================
 *        Class:  BtOgreTestApplication
 *  Description:  Derives from ExampleApplication and overrides stuff.
 * =====================================================================================
 */

class BtOgreTestApplication : public ExampleApplication
{
    protected:
	btAxisSweep3 *mBroadphase;
	btDefaultCollisionConfiguration *mCollisionConfig;
	btCollisionDispatcher *mDispatcher;
	btSequentialImpulseConstraintSolver *mSolver;

	Ogre::SceneNode *mNinjaNode;
	Ogre::Entity *mNinjaEntity;
	btRigidBody *mNinjaBody;
	btCollisionShape *mNinjaShape;

	Ogre::Entity *mGroundEntity;
	btRigidBody *mGroundBody;
	btBvhTriangleMeshShape *mGroundShape;

    public:
	BtOgreTestApplication()
	{
	    //Bullet initialisation.
	    mBroadphase = new btAxisSweep3(btVector3(-10000,-10000,-10000), btVector3(10000,10000,10000), 1024);
	    mCollisionConfig = new btDefaultCollisionConfiguration();
	    mDispatcher = new btCollisionDispatcher(mCollisionConfig);
	    mSolver = new btSequentialImpulseConstraintSolver();

	    Globals::phyWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfig);
	    Globals::phyWorld->setGravity(btVector3(0,-9.8,0));
	}

	~BtOgreTestApplication()
	{
            //Free rigid bodies
            Globals::phyWorld->removeRigidBody(mNinjaBody);
            delete mNinjaBody->getMotionState();
            delete mNinjaBody;
            delete mNinjaShape;

            Globals::phyWorld->removeRigidBody(mGroundBody);
            delete mGroundBody->getMotionState();
            delete mGroundBody;
            delete mGroundShape->getMeshInterface();
            delete mGroundShape;

	    //Free Bullet stuff.
            delete Globals::dbgdraw;
            delete Globals::phyWorld;

	    delete mSolver;
	    delete mDispatcher;
	    delete mCollisionConfig;
	    delete mBroadphase;
	}

    protected:
	void createScene(void)
	{
	    //Some normal stuff.
	    mSceneMgr->setAmbientLight(ColourValue(0.7,0.7,0.7));
	    mCamera->setPosition(Vector3(10,10,10));
	    mCamera->lookAt(Vector3::ZERO);
	    mCamera->setNearClipDistance(0.05);
	    LogManager::getSingleton().setLogDetail(LL_BOREME);

	    //----------------------------------------------------------
	    // Debug drawing!
	    //----------------------------------------------------------

	    Globals::dbgdraw = new BtOgre::DebugDrawer(mSceneMgr->getRootSceneNode(), Globals::phyWorld);
	    Globals::phyWorld->setDebugDrawer(Globals::dbgdraw);

	    //----------------------------------------------------------
	    // Ninja!
	    //----------------------------------------------------------

	    Vector3 pos = Vector3(0,10,0);
	    Quaternion rot = Quaternion::IDENTITY;

	    //Create Ogre stuff.

	    mNinjaEntity = mSceneMgr->createEntity("ninjaEntity", "Player.mesh");
	    mNinjaNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("ninjaSceneNode", pos, rot);
	    mNinjaNode->attachObject(mNinjaEntity);

	    //Create shape.
	    BtOgre::StaticMeshToShapeConverter converter(mNinjaEntity);
	    mNinjaShape = converter.createSphere();

	    //Calculate inertia.
	    btScalar mass = 5;
	    btVector3 inertia;
	    mNinjaShape->calculateLocalInertia(mass, inertia);

	    //Create BtOgre MotionState (connects Ogre and Bullet).
	    BtOgre::RigidBodyState *ninjaState = new BtOgre::RigidBodyState(mNinjaNode);

	    //Create the Body.
	    mNinjaBody = new btRigidBody(mass, ninjaState, mNinjaShape, inertia);
	    Globals::phyWorld->addRigidBody(mNinjaBody);

	    //----------------------------------------------------------
	    // Ground!
	    //----------------------------------------------------------

	    //Create Ogre stuff.
	    //MeshManager::getSingleton().createPlane("groundPlane", "General", Plane(Vector3::UNIT_Y, 0), 100, 100, 
	    //10, 10, true, 1, 5, 5, Vector3::UNIT_Z);
	    mGroundEntity = mSceneMgr->createEntity("groundEntity", "TestLevel_b0.mesh");
	    //mGroundEntity->setMaterialName("Examples/Rockwall");
	    mSceneMgr->getRootSceneNode()->createChildSceneNode("groundNode")->attachObject(mGroundEntity);

	    //Create the ground shape.
	    BtOgre::StaticMeshToShapeConverter converter2(mGroundEntity);
	    mGroundShape = converter2.createTrimesh();

	    //Create MotionState (no need for BtOgre here, you can use it if you want to though).
	    btDefaultMotionState* groundState = new btDefaultMotionState(
		    btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)));

	    //Create the Body.
	    mGroundBody = new btRigidBody(0, groundState, mGroundShape, btVector3(0,0,0));
	    Globals::phyWorld->addRigidBody(mGroundBody);
	}

	void createFrameListener(void)
	{
	    mFrameListener = new BtOgreTestFrameListener(mWindow, mCamera, mSceneMgr);
	    mRoot->addFrameListener(mFrameListener);
	}
};

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  main() function. Need say more?
 * =====================================================================================
 */

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char **argv)
#endif
{
    // Create application object
    BtOgreTestApplication app;

    try 
    {
	app.go();
    } 
    catch( Exception& e ) 
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	MessageBox( NULL, e.what(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
	fprintf(stderr, "An exception has occurred: %s\n",
		e.what());
#endif
    }

    return 0;
}
