#include "bloco.h"

BaseActor::~BaseActor()
{

}

 BLOCO_API ActorParams::ActorParams() 
{
	m_Mat = MatIdentity();
	m_Pos=Vec(0,0,0); 
	m_Type=AT_Unknown; 
	m_Col = Col(1.0f,1.0f,1.0f,1.0f);
	strcpy( m_OnCreateLuaFunctionName, "" );
	strcpy( m_sScriptFilename, "" );
	strcpy( m_sName, "" );
	strcpy( m_OnDestroyLuaFunctionName, "" );
	m_Size=sizeof(ActorParams); 
	m_IsModified= false;
	m_bPhysicActor = true;
	m_bHasScript = false;
}

 BLOCO_API ActorParams::~ActorParams()
 {
	 m_pActor.reset();
 }

//
// ActorParams::VInit					- Chapter 19, page 684
//
bool BLOCO_API ActorParams::VInit(std::istrstream &in)
{ 
	int hasActorId = 0;

	in >> m_Size;
	in >> hasActorId;
	if (hasActorId)
	{
		in >> hasActorId;
		m_Id = hasActorId;
	}

	m_Pos = Vec( in );
	m_Col = Vec( in );

	in >> m_OnCreateLuaFunctionName;
	in >> m_OnDestroyLuaFunctionName;
	return true;
}

//
// ActorParams::VInit						- Chpater 19, page 685
//
bool BLOCO_API ActorParams::VInit( LuaObject srcData, TErrorMessageList & errorMessages )
{
	LuaObject actorIDObj = srcData[ "ActorID" ];
	if ( actorIDObj.IsInteger() )
	{
		m_Id = actorIDObj.GetInteger();
	}


	LuaObject posObj = srcData[ "Pos" ];
	if ( posObj.IsTable() )
	{
		const int tableCount = posObj.GetTableCount();
		if ( 3 != tableCount )
		{
			const std::string err( "Incorrect number of parameters in the 'Pos' member." );
			errorMessages.push_back( err );
			return false;
		}
		else
		{
			//Get the three values.
			m_Pos.SetX( posObj[ 1 ].GetFloat());
			m_Pos.SetY( posObj[ 2 ].GetFloat());
			m_Pos.SetZ( posObj[ 3 ].GetFloat());
		}
	}

	LuaObject ColObj = srcData[ "Col" ];
	if ( ColObj.IsTable() )
	{
		//Get the RGBA off of it.
		LuaObject r = ColObj[ "R" ];
		if ( r.IsNumber() )
		{
			m_Col.SetX(r.GetFloat());
		}
		LuaObject g = ColObj[ "G" ];
		if ( g.IsNumber() )
		{
			m_Col.SetY(g.GetFloat());
		}
		LuaObject b = ColObj[ "B" ];
		if ( b.IsNumber() )
		{
			m_Col.SetZ(b.GetFloat());
		}
		LuaObject a = ColObj[ "A" ];
		if ( a.IsNumber() )
		{
			m_Col.SetW( a.GetFloat());
		}
	}

	//See if we have any on create/destroy handlers.
	LuaObject onCreateObj = srcData[ "OnCreateFunc" ];
	if ( onCreateObj.IsString() )
	{
		const char * pString = onCreateObj.GetString();
		strcpy_s( m_OnCreateLuaFunctionName, sk_MaxFuncName, onCreateObj.GetString() );
	}
	LuaObject onDestroyObj = srcData[ "OnDestroyFunc" ];
	if ( onDestroyObj.IsString() )
	{
		strcpy_s( m_OnDestroyLuaFunctionName, sk_MaxFuncName, onDestroyObj.GetString() );
	}

	return true;
}

//
// ActorParams::VSerialize					- Chapter 19, 685
//
void BLOCO_API ActorParams::VSerialize(std::ostrstream &out) const
{
	out << m_Type << " ";
	out << m_Size << " ";
	out << static_cast<int>(m_Id.valid()) << " ";
	if (m_Id.valid())
	{
		out << *m_Id << " ";
	}

	m_Pos.Serialize(out);
	m_Col.Serialize(out);

	out << m_OnCreateLuaFunctionName << " ";
	out << m_OnDestroyLuaFunctionName << " ";
}

//
// ActorParams::CreateFromStream					- Chapter 19, page 687
//	AT_Unknown,
ActorParams BLOCO_API *ActorParams::CreateFromStream(std::istrstream &in)
{
	int actorType;
	in >> actorType;

	ActorParams *actor = NULL;
	switch (actorType)
	{
		case AT_Model:
			actor = DEBUG_CLIENTBLOCK ModelObjectParams();
			break;

		case AT_Camera:
			actor = DEBUG_CLIENTBLOCK CameraObjectParams();
			break;

		default:
			assert(0 && _T("Unimplemented actor type in stream"));
			return 0;
	}

	if (! actor->VInit(in))
	{
		// something went wrong with the serialization...
		assert(0 && _T("Error in Actor stream initialization"));
		SAFE_DELETE(actor);
	}

	return actor;

}

//
// ActorParams::CreateFromLuaObj				- Chapter 19, page 688 
//
ActorParams BLOCO_API * ActorParams::CreateFromLuaObj( LuaObject srcData )
{
	//Make sure this is legit.
	if ( false == srcData.IsTable() )
	{
		assert( 0 && "Requested to create an actor, but no table was passed with actor params!" );
		return NULL;
	}

	//Find out the actor type.
	LuaObject actorTypeObj = srcData[ "ActorType" ];
	if ( false == actorTypeObj.IsString() )
	{
		assert( 0 && "Member 'ActorType' wasn't found!" );
		return NULL;
	}

	//OK, we've got a string.  Match it up with the appropriate constructor to build the data.
	const char * pActorType = actorTypeObj.GetString();
	ActorParams * pActorParams = NULL;
	if ( 0 == strcmp( pActorType, "Model" ) )
	{
		pActorParams = DEBUG_CLIENTBLOCK ModelObjectParams();
	}
	else if ( 0 == strcmp( pActorType, "Camera" ) )
	{
		pActorParams = DEBUG_CLIENTBLOCK CameraObjectParams();
	}
	else
	{
		assert( 0 && "Unknown/unsupported member in 'ActorType' encountered!" );
		return NULL;
	}

	if ( NULL != pActorParams )
	{
		TErrorMessageList errorMessages;
		if ( false == pActorParams->VInit( srcData, errorMessages ) )
		{
			//Spit out all the error messages.
			OutputDebugStringA( "ACTOR PARAMETER ERRORS:" );
			for ( TErrorMessageList::const_iterator iter = errorMessages.begin(), end = errorMessages.end(); iter != end; ++iter )
			{
				OutputDebugStringA( "\n" );
				const std::string & error = *iter;
				OutputDebugStringA( error.c_str() );
			}
			assert( 0 && "Error in actor parameter creation from script!" );
			SAFE_DELETE( pActorParams );
		}
	}

	return pActorParams;
}




/************************************************************************/
/*ModelObjectParams                                                     */
/************************************************************************/
 BLOCO_API ModelObjectParams::ModelObjectParams()
{
	m_Mat = MatIdentity();
	m_Type=AT_Model; 
	m_Col= Vec(0.5f,0.5f,0.5f,0.5f); 
	strcpy( m_ModellFilename, "-" );
	strcpy( m_FXFileName, "-" );
	strcpy( m_FXTechniqueName,  "-" );
	strcpy( m_sScriptFilename, "" );
	m_Size=sizeof(ModelObjectParams); 
	m_IsModified = false;
	m_AABB = Vec(10.0f,10.0f,10.0f);
	m_bPhysicActor = true;
	strcpy( m_sName, "" );
}

 BLOCO_API ModelObjectParams::ModelObjectParams( const char* modelFilename, const char* fxFilename, const char* fxTechinqueName )
{
	m_Mat = MatIdentity();
	m_Type=AT_Model; 
	m_Col= Vec(0.5f,0.5f,0.5f,0.5f); 
	strcpy(m_ModellFilename, modelFilename);
	strcpy( m_FXFileName, fxFilename );
	strcpy( m_FXTechniqueName, fxTechinqueName );
	m_Size=sizeof(ModelObjectParams); 
	m_IsModified = false;
	m_bPhysicActor = true;
	strcpy( m_sName, GetTitle(m_ModellFilename).c_str() );
}

bool BLOCO_API ModelObjectParams::VInit( std::istrstream &in )
{
	if (ActorParams::VInit(in))
	{
		m_Type=AT_Model; 
		in >> m_ModellFilename;
		in >> m_FXFileName;
		for (int i=0; i<4; ++i)
			for (int j=0; j<4; ++j)
				in >> m_Mat.GetStorage().m[i][j];
		return true;
	}
	return false;
}

bool BLOCO_API ModelObjectParams::VInit( LuaObject srcData, TErrorMessageList & errorMessages )
{
	if ( false == ActorParams::VInit( srcData, errorMessages ) )
	{
		return false;
	}

	m_Type = AT_Model;
	m_bDynamic = true;
	m_Mat = MatIdentity();


	//Positon
	LuaObject positionObj = srcData[ "Position" ];
	if ( positionObj.IsTable() )
	{
		float x = positionObj[ "x" ].GetFloat();
		float y = positionObj[ "y" ].GetFloat();
		float z = positionObj[ "z" ].GetFloat();

		m_Mat = MatTranslation( x, y, z );
	}


	LuaObject matObj = srcData[ "Mat" ];
	if ( matObj.IsTable() )
	{
		const int tableCount = matObj.GetTableCount();
		if ( 16 != tableCount )
		{
			const std::string err( "Incorrect number of parameters in the 'Mat' member." );
			errorMessages.push_back( err );
			return false;
		}
		else
		{
			char name[4] = "_00";

			for( int i = 1; i <= 4; ++i ) 
			{
				name[1] = '0' + i;

				for( int j = 1; j <= 4; ++j ) 
				{
					name[2] = '0' + j;

					LuaObject entry = matObj[ name ];
					if( entry.IsNumber() ) 
					{
						m_Mat.GetStorage().m[i - 1][j - 1] = entry.GetFloat();
					}
				}
			}
		}
	}


	LuaObject XFileObj = srcData[ "ModelFile" ];
	if ( XFileObj.IsString() )
	{
		//Check if it is a Path
		bool bPath = false;
		const char * filename = XFileObj.GetString();

		strcpy( m_ModellFilename, MODEL_PATH(filename) );

		m_bHasScript = true;
		string tmp = m_ModellFilename;
		tmp.erase(tmp.end()-3,tmp.end());
		tmp += "lua";

		strcpy( m_sScriptFilename, tmp.c_str() );
	}

	//Physic
	LuaObject PhysicObj = srcData[ "Physic" ];
	if ( PhysicObj.IsString() )
	{
		string strPhysic = PhysicObj.GetString();

		if ( strPhysic == "static")
			m_bDynamic = false;
		else if( strPhysic == "dynamic")
			m_bDynamic = true;
		else if( strPhysic == "none")
			m_bPhysicActor = false;
	}


	LuaObject FXFileObj = srcData[ "FXFile" ];
	if ( FXFileObj.IsString() )
	{
		const char * pFXFile = FXFileObj.GetString();
		strcpy( m_FXFileName, pFXFile );
	}

	LuaObject FXTechniqueObj = srcData[ "FXTechnique" ];
	if ( FXTechniqueObj.IsString() )
	{
		const char * pFXTechnique = FXTechniqueObj.GetString();
		strcpy( m_FXTechniqueName, pFXTechnique );
	}

	return true;
}

void BLOCO_API ModelObjectParams::VSerialize( std::ostrstream &out ) const
{
	ActorParams::VSerialize(out);
	out << m_ModellFilename;
	out << m_FXFileName;

	m_Mat.Serialize( out );
}

shared_ptr<IActor> BLOCO_API ModelObjectParams::VCreate( BaseGameLogic *logic )
{
	m_pActor = shared_ptr<IActor>(DEBUG_CLIENTBLOCK BaseActor( m_Mat, AT_Model, shared_ptr<ModelObjectParams>(DEBUG_CLIENTBLOCK ModelObjectParams(*this))));
	logic->VAddActor(m_pActor, this);

	return m_pActor;
}

shared_ptr<SceneNode> BLOCO_API ModelObjectParams::VCreateSceneNode(shared_ptr<Scene> pScene)
{
	shared_ptr<ModelSceneNode> meshFile(DEBUG_CLIENTBLOCK ModelSceneNode(m_Id, m_sName, m_ModellFilename, m_pActor->VGetMat()));
	return meshFile;
}


/************************************************************************/
/*ModelChildObjectParams                                                */
/************************************************************************/
 BLOCO_API ModelChildObjectParams::ModelChildObjectParams( ActorType type )
{
	m_Mat = MatIdentity();
	m_Type=type; 
	m_Col= Vec(0.5f,0.5f,0.5f,0.5f); 
	strcpy( m_ModellFilename, "-" );
	strcpy( m_FXFileName, "-" );
	strcpy( m_sScriptFilename, "" );
	strcpy( m_FXTechniqueName,  "-" );
	strcpy( m_sName, "" );
	m_Size=sizeof(ModelChildObjectParams); 
	m_IsModified = false;
	m_AABB = Vec(10.0f,10.0f,10.0f);
	m_bPhysicActor = false;

	m_bHasScript = false;
}

 BLOCO_API ModelChildObjectParams::ModelChildObjectParams( const char* modelFilename, const char* fxFilename, const char* fxTechinqueName, int actorID )
{
	m_Mat = MatIdentity();
	m_Id = actorID;
	m_Type=AT_ModelChild_Mesh; 
	m_Col= Vec(0.5f,0.5f,0.5f,0.5f); 
	m_AABB = Vec(10.0f,10.0f,10.0f);
	strcpy( m_ModellFilename, modelFilename );
	strcpy( m_FXFileName, fxFilename );
	strcpy( m_FXTechniqueName, fxTechinqueName );
	strcpy( m_sName, "" );
	m_Size=sizeof(ModelChildObjectParams); 
	m_IsModified = false;

	m_bHasScript = false;
}

 BLOCO_API ModelChildObjectParams::ModelChildObjectParams(  int actorID )
{
	m_Mat = MatIdentity();
	m_Id = actorID;
	m_Type=AT_ModelChild_Mesh; 
	m_Col= Vec(0.5f,0.5f,0.5f,0.5f); 
	strcpy( m_ModellFilename, "" );
	strcpy( m_FXFileName, "" );
	strcpy( m_FXTechniqueName, "" );
	m_AABB = Vec(10.0f,10.0f,10.0f);
	m_Size=sizeof(ModelChildObjectParams); 
	m_IsModified = false;
	m_bDynamic = true;

	m_bHasScript = false;
}

shared_ptr<IActor> BLOCO_API ModelChildObjectParams::VCreate( BaseGameLogic *logic )
{
	m_pActor = shared_ptr<IActor>(DEBUG_CLIENTBLOCK BaseActor( m_Mat, m_Type, shared_ptr<ModelObjectParams>(DEBUG_CLIENTBLOCK ModelObjectParams(*this))));
	logic->VAddActor(m_pActor, this);
	
	return m_pActor;
}

shared_ptr<SceneNode> BLOCO_API ModelChildObjectParams::VCreateSceneNode( shared_ptr<Scene> pScene )
{
	switch(m_Type)
	{
	case AT_ModelChild_Mesh:
		{
			shared_ptr<MeshSceneNode> meshFile(DEBUG_CLIENTBLOCK MeshSceneNode( m_pModelElement, m_Id, m_pRootNode, m_pActor->VGetMat() ));
			return meshFile;		
		}

	case AT_ModelChild_Bone:
		{
			shared_ptr<BoneSceneNode> meshFile(DEBUG_CLIENTBLOCK BoneSceneNode( m_pModelElement, m_Id, m_pRootNode, m_pActor->VGetMat() ));
			return meshFile;		
		}
	}

	return nullptr;
}




/************************************************************************/
/*ModelObjectParams                                                     */
/************************************************************************/
BLOCO_API PhysicObjectParams::PhysicObjectParams()
{
	m_Mat = MatIdentity();
	m_Type=AT_Model; 
	m_Col= Vec(0.5f,0.5f,0.5f,0.5f); 
	m_Size=sizeof(ModelObjectParams); 
	m_IsModified = false;
	m_AABB = Vec(10.0f,10.0f,10.0f);
	strcpy( m_sScriptFilename, "" );
	strcpy( m_sName, "" );
	m_bPhysicActor = true;

	m_bHasScript = false;
}

BLOCO_API PhysicObjectParams::PhysicObjectParams( int actorID )
{
	m_Mat = MatIdentity();
	m_Id = actorID;
	m_Type=AT_Model; 
	m_Col= Vec(0.5f,0.5f,0.5f,0.5f); 
	m_Size=sizeof(ModelObjectParams); 
	strcpy( m_sName, "" );
	m_IsModified = false;
	m_bHasScript = false;
}

bool BLOCO_API PhysicObjectParams::VInit( std::istrstream &in )
{
	if (ActorParams::VInit(in))
	{
		m_Type=AT_Model; 
		for (int i=0; i<4; ++i)
			for (int j=0; j<4; ++j)
				in >> m_Mat.GetStorage().m[i][j];
		return true;
	}
	return false;
}

bool BLOCO_API PhysicObjectParams::VInit( LuaObject srcData, TErrorMessageList & errorMessages )
{
	if ( false == ActorParams::VInit( srcData, errorMessages ) )
	{
		return false;
	}

	m_Type = AT_Model;
	m_bDynamic = true;
	m_Mat = MatIdentity();


	//Positon
	LuaObject positionObj = srcData[ "Position" ];
	if ( positionObj.IsTable() )
	{
		float x = positionObj[ "x" ].GetFloat();
		float y = positionObj[ "y" ].GetFloat();
		float z = positionObj[ "z" ].GetFloat();

		m_Mat = MatTranslation( x, y, z );
	}


	LuaObject matObj = srcData[ "Mat" ];
	if ( matObj.IsTable() )
	{
		const int tableCount = matObj.GetTableCount();
		if ( 16 != tableCount )
		{
			const std::string err( "Incorrect number of parameters in the 'Mat' member." );
			errorMessages.push_back( err );
			return false;
		}
		else
		{
			char name[4] = "_00";

			for( int i = 1; i <= 4; ++i ) 
			{
				name[1] = '0' + i;

				for( int j = 1; j <= 4; ++j ) 
				{
					name[2] = '0' + j;

					LuaObject entry = matObj[ name ];
					if( entry.IsNumber() ) 
					{
						m_Mat.GetStorage().m[i - 1][j - 1] = entry.GetFloat();
					}
				}
			}
		}
	}

	//Physic
	LuaObject PhysicObj = srcData[ "Physic" ];
	if ( PhysicObj.IsString() )
	{
		string strPhysic = PhysicObj.GetString();

		if ( strPhysic == "static")
			m_bDynamic = false;
		else
			m_bDynamic = true;
	}



	return true;
}

void BLOCO_API PhysicObjectParams::VSerialize( std::ostrstream &out ) const
{
	ActorParams::VSerialize(out);

	m_Mat.Serialize( out );
}

shared_ptr<IActor> BLOCO_API PhysicObjectParams::VCreate( BaseGameLogic *logic )
{
	m_Mat = MatIdentity();
	shared_ptr<IActor> pModelObjectParams(DEBUG_CLIENTBLOCK BaseActor( m_Mat, AT_Model, shared_ptr<PhysicObjectParams>(DEBUG_CLIENTBLOCK PhysicObjectParams(*this))));
	logic->VAddActor(pModelObjectParams, this);

	return pModelObjectParams;
}

shared_ptr<SceneNode> BLOCO_API PhysicObjectParams::VCreateSceneNode(shared_ptr<Scene> pScene)
{
	return NULL;
}






/************************************************************************/
/*Camera                                                                */
/************************************************************************/
CameraObjectParams::CameraObjectParams()
{
	m_Type=AT_Camera; 
	m_Col= Vec(0.5f,0.5f,0.5f,0.5f);
	m_Mat= MatIdentity();
	m_Size=sizeof(CameraObjectParams);
	strcpy( m_sScriptFilename, "" );
	strcpy( m_sName, "" );
	m_IsModified = false;

	m_bHasScript = false;
}

CameraObjectParams::CameraObjectParams( int actorID )
{
	m_Id = actorID;
	m_Type=AT_Camera; 
	m_Col= Vec(0.5f,0.5f,0.5f,0.5f); 
	m_Mat= MatIdentity();
	m_Size=sizeof(CameraObjectParams);
	strcpy( m_sName, "" );
	m_IsModified = false;
}

bool CameraObjectParams::VInit( std::istrstream &in )
{
	if (ActorParams::VInit(in))
	{
		m_Type=AT_Camera; 
		for (int i=0; i<4; ++i)
			for (int j=0; j<4; ++j)
				in >> m_Mat.GetStorage().m[i][j];
		return true;
	}
	return false;
}

bool CameraObjectParams::VInit( LuaObject srcData, TErrorMessageList & errorMessages )
{
	if ( false == ActorParams::VInit( srcData, errorMessages ) )
	{
		return false;
	}

	m_Type = AT_Camera;

	m_Mat = MatIdentity();

	LuaObject matObj = srcData[ "Mat" ];
	if ( matObj.IsTable() )
	{
		const int tableCount = matObj.GetTableCount();
		if ( 16 != tableCount )
		{
			const std::string err( "Incorrect number of parameters in the 'Mat' member." );
			errorMessages.push_back( err );
			return false;
		}
		else
		{
			char name[4] = "_00";

			for( int i = 1; i <= 4; ++i ) 
			{
				name[1] = '0' + i;

				for( int j = 1; j <= 4; ++j ) 
				{
					name[2] = '0' + j;

					LuaObject entry = matObj[ name ];
					if( entry.IsNumber() ) 
					{
						m_Mat.GetStorage().m[i - 1][j - 1] = entry.GetFloat();
					}
				}
			}
		}
	}

	return true;
}

void CameraObjectParams::VSerialize( std::ostrstream &out ) const
{
	ActorParams::VSerialize(out);

	m_Mat.Serialize( out );
}

shared_ptr<IActor> CameraObjectParams::VCreate( BaseGameLogic *logic )
{
	Mat mat;
	mat = m_Mat;
	shared_ptr<IActor> pCameraObjectParams(DEBUG_CLIENTBLOCK BaseActor(mat, AT_Camera, shared_ptr<CameraObjectParams>(DEBUG_CLIENTBLOCK CameraObjectParams(*this))));
	logic->VAddActor(pCameraObjectParams, this);
	//m_pPhysics->VAddBox(Vec(p.m_Squares/2.0f, 0.01f, p.m_Squares/2.0f), &*pGrid, SpecificGravity(PhysDens_Infinite), PhysMat_Slippery);
	return pCameraObjectParams;
}

shared_ptr<SceneNode> CameraObjectParams::VCreateSceneNode(shared_ptr<Scene> pScene)
{
	// 	size_t origsize = strlen(m_ModellFilename) + 1;
	// 	const size_t newsize = StaticMeshObjectParams::sk_MaxFileNameLen ;
	//     size_t convertedChars = 0;
	//     wchar_t meshFileName[newsize];
	//     mbstowcs_s(&convertedChars, meshFileName, origsize, m_ModellFilename, _TRUNCATE);
	// 	
	// 	origsize = strlen(m_FXFileName) + 1;
	// 	wchar_t effectFileName[newsize];
	// 	mbstowcs_s(&convertedChars, effectFileName, origsize, m_FXFileName, _TRUNCATE);



	stringstream stmName;

	stmName << "Object";
	stmName << (*m_Id);

	Frustum frustum;
	frustum.Init(D3DX_PI/4.0f, 1.0f, 0.1f, 10000.0f);

	CameraNode* node = DEBUG_CLIENTBLOCK CameraNode(m_Id, stmName.str(), m_Mat ,frustum);

	shared_ptr<SceneNode> cameraNode(node);
	pScene->SetCamera( shared_ptr<CameraNode>((CameraNode*)node) );
	return cameraNode;
}





BoneObjectParams::BoneObjectParams( string name, string parentName, Mat global, Mat bindPose )
{
	m_Mat = MatIdentity();
	m_Type=AT_Model; 
	m_Col= Vec(0.5f,0.5f,0.5f,0.5f); 
	m_Size=sizeof(BoneObjectParams); 
	m_IsModified = false;
	m_AABB = Vec(10.0f,10.0f,10.0f);
	strcpy( m_sScriptFilename, "" );
	strcpy( m_sName, "" );
	m_bPhysicActor = true;

	m_bHasScript = false;
}

shared_ptr<IActor> BoneObjectParams::VCreate( BaseGameLogic *logic )
{
	m_pActor = shared_ptr<IActor>(DEBUG_CLIENTBLOCK BaseActor( m_Mat, m_Type, shared_ptr<BoneObjectParams>(DEBUG_CLIENTBLOCK BoneObjectParams(*this))));
	logic->VAddActor(m_pActor, this);

	return m_pActor;
}

shared_ptr<SceneNode> BoneObjectParams::VCreateSceneNode( shared_ptr<Scene> pScene )
{
	//shared_ptr<BoneSceneNode> meshFile(DEBUG_CLIENTBLOCK BoneSceneNode( NULL, m_Id, NULL ,m_pActor->VGetMat() ));
	return nullptr;		
}
