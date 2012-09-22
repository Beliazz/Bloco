#ifndef Bloco_h__
#define Bloco_h__

#ifdef BLOCO_EXPORTS
	#define BLOCO_API __declspec(dllexport)
#else
	#define BLOCO_API __declspec(dllimport)
#endif

#include <BlocoUtility.h>
#pragma comment(lib, "BlocoUtility.lib")

#include <ClearEventManager.h>
#pragma comment(lib, "ClearEventManager.lib")

#include <ClearPhysicsSDK.h>
#pragma comment(lib, "ClearPhysicsSDK.lib")

#include <cgl.h>
#pragma comment(lib, "ClearGraphicsLibrary.lib")

// Input
#include <cil.h>
#pragma comment(lib, "ClearInputLibrary.lib")

// Model SDK
#include <ClearModelSDK.h>
#pragma comment(lib, "ClearModelSDK.lib")

// Font Wrapper
#include <font_wrapper/include/FW1FontWrapper.h>
#pragma comment(lib, "FW1FontWrapper.lib")

//XACT
#include <xact3.h>
#include <xact3d3.h>
#pragma comment (lib, "x3daudio.lib")


#define kpWildcardEventType "*"

#include "MainLoop/CProcess.h"
#include "MainLoop/CProcessManager.h"

#include "EventManager/EventManagerImpl.h"

#include "Scripting/ScriptEventListener.h"

#include "Audio/XACTAudio.h"
#include "Audio/XACTSound.h"

#include "Physic/PhysicsEventListener.h"

#include "Actors.h"
#include "EventManager/Events.h"

#include "Graphic/D3D11VertexTypes.h"
#include "Graphic/D3D11Material.h"
#include "Graphic/D3D11ModelCommon.h"
#include "Graphic/D3D11Mesh.h"
#include "Graphic/D3D11Skeleton.h"
#include "Graphic/D3D11Model.h"
#include "Graphic/D3D11Renderer.h"
#include "Graphic/D3D11Font.h"


#include "Scripting/ScriptFunctions.h"

#include "SceneGraph/Octree.h"
#include "SceneGraph/ModelManager.h"
#include "SceneGraph/Scene.h"
#include "SceneGraph/ModelSceneNode.h"

#include "GameView/HumanView.h"
#include "GameLogic/BaseGameLogic.h"

#include "Application.h"

#endif // main_h__
