// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#pragma once

#include "IShaderConstantSetCallBack.h"
#include "IMaterialRendererServices.h"

namespace irr
{
namespace video
{

class COpenGL3MaterialBaseCB : public IShaderConstantSetCallBack
{
public:
	COpenGL3MaterialBaseCB();

	virtual void OnSetMaterial(const SMaterial& material);
	virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

protected:
	bool FirstUpdateBase;

	s32 WVPMatrixID;
	s32 WVMatrixID;
	s32 NMatrixID;

	s32 GlobalAmbientID;
	s32 MaterialAmbientID;
	s32 MaterialDiffuseID;
	s32 MaterialEmissiveID;
	s32 MaterialSpecularID;
	s32 MaterialShininessID;

	s32 FogEnableID;
	s32 FogTypeID;
	s32 FogColorID;
	s32 FogStartID;
	s32 FogEndID;
	s32 FogDensityID;

	s32 ThicknessID;

	bool LightEnable;
	SColorf GlobalAmbient;
	SColorf MaterialAmbient;
	SColorf MaterialDiffuse;
	SColorf MaterialEmissive;
	SColorf MaterialSpecular;
	f32 MaterialShininess;

	s32 FogEnable;
	s32 FogType;
	SColorf FogColor;
	f32 FogStart;
	f32 FogEnd;
	f32 FogDensity;

	f32 Thickness;
};

class COpenGL3MaterialSolidCB : public COpenGL3MaterialBaseCB
{
public:
	COpenGL3MaterialSolidCB();

	virtual void OnSetMaterial(const SMaterial& material);
	virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

protected:
	bool FirstUpdate;

	s32 TMatrix0ID;
	s32 AlphaRefID;
	s32 TextureUsage0ID;
	s32 TextureUnit0ID;

	f32 AlphaRef;
	s32 TextureUsage0;
	s32 TextureUnit0;
};

class COpenGL3MaterialSolid2CB : public COpenGL3MaterialBaseCB
{
public:
	COpenGL3MaterialSolid2CB();

	virtual void OnSetMaterial(const SMaterial& material);
	virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

protected:
	bool FirstUpdate;

	s32 TMatrix0ID;
	s32 TMatrix1ID;
	s32 TextureUsage0ID;
	s32 TextureUsage1ID;
	s32 TextureUnit0ID;
	s32 TextureUnit1ID;

	s32 TextureUsage0;
	s32 TextureUsage1;
	s32 TextureUnit0;
	s32 TextureUnit1;
};

class COpenGL3MaterialLightmapCB : public COpenGL3MaterialBaseCB
{
public:
	COpenGL3MaterialLightmapCB(float modulate);

	virtual void OnSetMaterial(const SMaterial& material);
	virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

protected:
	bool FirstUpdate;

	s32 TMatrix0ID;
	s32 TMatrix1ID;
	s32 ModulateID;
	s32 TextureUsage0ID;
	s32 TextureUsage1ID;
	s32 TextureUnit0ID;
	s32 TextureUnit1ID;

	f32 Modulate;
	s32 TextureUsage0;
	s32 TextureUsage1;
	s32 TextureUnit0;
	s32 TextureUnit1;
};

class COpenGL3MaterialReflectionCB : public COpenGL3MaterialBaseCB
{
public:
	COpenGL3MaterialReflectionCB();

	virtual void OnSetMaterial(const SMaterial& material);
	virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

protected:
	bool FirstUpdate;

	s32 TMatrix0ID;
	s32 TextureUsage0ID;
	s32 TextureUsage1ID;
	s32 TextureUnit0ID;
	s32 TextureUnit1ID;

	s32 TextureUsage0;
	s32 TextureUsage1;
	s32 TextureUnit0;
	s32 TextureUnit1;
};

class COpenGL3MaterialOneTextureBlendCB : public COpenGL3MaterialBaseCB
{
public:
	COpenGL3MaterialOneTextureBlendCB();

	virtual void OnSetMaterial(const SMaterial& material);
	virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

protected:
	bool FirstUpdate;

	s32 TMatrix0ID;
	s32 BlendTypeID;
	s32 TextureUsage0ID;
	s32 TextureUnit0ID;

	s32 BlendType;
	s32 TextureUsage0;
	s32 TextureUnit0;
};

}
}
