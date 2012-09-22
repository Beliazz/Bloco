#include "Bloco.h"


CD3D11Material::CD3D11Material( cgl::PD3D11Effect pEffect ) : m_pEffect(pEffect)
{
	m_EmmisionColor		= Vec(0,0,0,1);
	m_AmbientColor		= Vec(0,0,0,1);
	m_DiffuseColor		= Vec(1,1,1,1);
	m_SpecularColor		= Vec(1,1,1,1);	


	m_Shininess		= 0.5f;
	m_Reflectivity	= 0.5f;
	m_Transparency	= 1.0f;


	m_pevEmmisionColor	= cgl::CD3D11EffectVariableFromName::Create(m_pEffect, "f4EmmisionColor");
	m_pevAmbientColor	= cgl::CD3D11EffectVariableFromName::Create(m_pEffect, "f4AmbientColor");
	m_pevDiffuseColor	= cgl::CD3D11EffectVariableFromName::Create(m_pEffect, "f4DiffuseColor");
	m_pevSpecularColor	= cgl::CD3D11EffectVariableFromName::Create(m_pEffect, "f4SpecularColor");

	m_pevShininess		= cgl::CD3D11EffectVariableFromName::Create(m_pEffect, "fShininess");
	m_pevReflectivity	= cgl::CD3D11EffectVariableFromName::Create(m_pEffect, "fReflectivity");
	m_pevTransparency	= cgl::CD3D11EffectVariableFromName::Create(m_pEffect, "fTransparency");

}

CD3D11Material::CD3D11Material( cgl::PD3D11Effect pEffect, ClearModelSDK::sMaterial__ mat )
{

}

CD3D11Material::~CD3D11Material(void)
{
}

void CD3D11Material::SetName( string name )
{
	m_sName = name;
}

bool CD3D11Material::SetEmmisionColor( Vec value )
{
	m_EmmisionColor = value;

	if (FAILED(m_pevEmmisionColor->get()->AsVector()->SetFloatVector( m_EmmisionColor.GetArray() )))
		return false;

	return true;
}

bool CD3D11Material::SetAmbientColor( Vec value )
{
	m_AmbientColor = value;
	if (FAILED(m_pevAmbientColor->get()->AsVector()->SetFloatVector( m_AmbientColor.GetArray() )))
		return false;

	return true;
}

bool CD3D11Material::SetDiffuseColor( Vec value )
{
	m_DiffuseColor = value;
	if (FAILED(m_pevDiffuseColor->get()->AsVector()->SetFloatVector( m_DiffuseColor.GetArray() )))
		return false;

	return true;
}

bool CD3D11Material::SetSpecularColor( Vec value )
{
	m_SpecularColor = value;
	if (FAILED(m_pevSpecularColor->get()->AsVector()->SetFloatVector( m_SpecularColor.GetArray() )))
		return false;

	return true;
}

bool CD3D11Material::SetShininess( float value )
{
	m_Shininess = value; 

	if (FAILED(m_pevShininess->get()->AsScalar()->SetFloat( m_Shininess )))
		return false;

	return true;
}

bool CD3D11Material::SetReflectivity( float value )
{
	m_Reflectivity = value; 
	if (FAILED(m_pevReflectivity->get()->AsScalar()->SetFloat( m_Shininess )))
		return false;

	return true;
}

bool CD3D11Material::SetTransparency( float value )
{
	m_Transparency = value; 
	if (FAILED(m_pevTransparency->get()->AsScalar()->SetFloat( m_Shininess )))
		return false;

	return true;
}

bool CD3D11Material::Set()
{
	if (FAILED(m_pevEmmisionColor->get()->AsVector()->SetFloatVector( m_EmmisionColor.GetArray() )))
		return false;

	if (FAILED(m_pevAmbientColor->get()->AsVector()->SetFloatVector( m_AmbientColor.GetArray() )))
		return false;

	if (FAILED(m_pevDiffuseColor->get()->AsVector()->SetFloatVector( m_DiffuseColor.GetArray() )))
		return false;

	if (FAILED(m_pevSpecularColor->get()->AsVector()->SetFloatVector( m_SpecularColor.GetArray() )))
		return false;

	if (FAILED(m_pevShininess->get()->AsScalar()->SetFloat( m_Shininess )))
		return false;

	if (FAILED(m_pevReflectivity->get()->AsScalar()->SetFloat( m_Reflectivity )))
		return false;

	if (FAILED(m_pevTransparency->get()->AsScalar()->SetFloat( m_Transparency )))
		return false;

	return true;
}

bool CD3D11Material::Init()
{
	if (!m_pevEmmisionColor->restore())
		return false;

	if (!m_pevAmbientColor->restore())
		return false;

	if (!m_pevDiffuseColor->restore())
		return false;

	if (!m_pevSpecularColor->restore())
		return false;

	if (!m_pevShininess->restore())
		return false;

	if (!m_pevReflectivity->restore())
		return false;

	if (!m_pevTransparency->restore())
		return false;

	return true;
}

bool CD3D11Material::Restore()
{
	if (!m_pevEmmisionColor->restore())
		return false;

	if (!m_pevAmbientColor->restore())
		return false;

	if (!m_pevDiffuseColor->restore())
		return false;

	if (!m_pevSpecularColor->restore())
		return false;

	if (!m_pevShininess->restore())
		return false;

	if (!m_pevReflectivity->restore())
		return false;

	if (!m_pevTransparency->restore())
		return false;

	return true;
}

bool CD3D11Material::FromModel( ClearModelSDK::sMaterial__ mat )
{
	if ( mat.m_sType == "Phong" )
	{
		SetAmbientColor(  Vec( mat.m_phong.m_colDiffuse.GetArray()) ); 
		SetDiffuseColor(  Vec( mat.m_phong.m_colDiffuse.GetArray()) ); 
		SetEmmisionColor( Vec( mat.m_phong.m_colEmissive.GetArray()) ); 
		SetSpecularColor( Vec( mat.m_phong.m_colSpecular.GetArray()) ); 
		
		SetShininess(    mat.m_phong.m_fShininess );
		SetTransparency( mat.m_phong.m_fTransparency );
		SetReflectivity( mat.m_phong.m_fReflectivity );
	}
	else if ( mat.m_sType == "Lambert" )
	{
		SetAmbientColor(  Vec( mat.m_lambert.m_colAmbient.GetArray()) ); 
		SetDiffuseColor(  Vec( mat.m_lambert.m_colDiffuse.GetArray()) ); 
		SetEmmisionColor( Vec( mat.m_lambert.m_colEmissive.GetArray()) ); 

		SetTransparency( mat.m_lambert.m_fTransparency );
	}
	else
	{
		printf("Undefined Material Type [CD3D11Material::FromModel]\n");
		return false;
	}

	return true;
}

