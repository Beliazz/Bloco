#ifndef __CD3D11Material_h
#define __CD3D11Material_h

#include "Bloco.h"

// EmmisionColor:	 		Float[4]		0-1		Default: 0-0-0-1
// AmbientColor:			Float[4]		0-1		Default: 0-0-0-1		
// DiffuseColor:			Float[4]		0-1		Default: 1-1-1-1
// EnvironmentColor: 		Float[4]		0-1		Default: 1-1-1-1
// SpecularColor:			Float[4]		0-1		Default: 1-1-1-1
// TransparentColor:		Float[4]		0-1		Default: 0-0-0-1
// SpecularShininess:		Float			0-1		Default: 0.5
// Reflectivity:			Float			0-1		Default: 0.5
// Transparency:			Float			0-1		Default: 1
// Index_Of_Refraction:		Float			0-1		Default: 1


class CD3D11Material
{
public:
	CD3D11Material(cgl::PD3D11Effect pEffect);
	CD3D11Material(cgl::PD3D11Effect pEffect, ClearModelSDK::sMaterial__ mat );
	~CD3D11Material(void);

	bool Init();
	bool Restore();
	bool Set();

	bool FromModel( ClearModelSDK::sMaterial__ mat );

	void SetName( string name );

	bool SetEmmisionColor(Vec value);
	bool SetAmbientColor(Vec value);
	bool SetDiffuseColor(Vec value);
	bool SetSpecularColor(Vec value);

	bool SetShininess(float value);
	bool SetReflectivity(float value);
	bool SetTransparency(float value);

private:
	cgl::PD3D11Effect m_pEffect;

	string m_sName;

	Vec m_EmmisionColor;
	Vec m_AmbientColor;
	Vec m_DiffuseColor;
	Vec m_SpecularColor;

	float m_Shininess;
	float m_Reflectivity;
	float m_Transparency;

	//Effect Variables
	cgl::PD3D11EffectVariable m_pevEmmisionColor;
	cgl::PD3D11EffectVariable m_pevAmbientColor;
	cgl::PD3D11EffectVariable m_pevDiffuseColor;
	cgl::PD3D11EffectVariable m_pevSpecularColor;

	cgl::PD3D11EffectVariable m_pevShininess;
	cgl::PD3D11EffectVariable m_pevReflectivity;
	cgl::PD3D11EffectVariable m_pevTransparency;
};

#endif