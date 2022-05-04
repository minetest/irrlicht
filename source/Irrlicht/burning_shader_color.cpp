// Copyright (C) 2022 Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#include "IBurningShader.h"

#ifdef _IRR_COMPILE_WITH_BURNINGSVIDEO_

namespace irr
{

namespace video
{

#define burning_shader_class burning_shader_color
#define burning_shader_frag "burning_shader_color_fraq.h"
#include "burning_shader_compile_fragment_default.h"


/*!
*/
void burning_shader_class::OnSetMaterialBurning(const SBurningShaderMaterial& material)
{
	switch (material.org.MaterialType)
	{
	case EMT_TRANSPARENT_ADD_COLOR:
	case EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR:
	case EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR:
		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);

	case EMT_TRANSPARENT_ALPHA_CHANNEL:
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	case EMT_TRANSPARENT_ALPHA_CHANNEL_REF:
		//? glBlendFunc(GL_ONE,GL_ZERO) or glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	case EMT_TRANSPARENT_REFLECTION_2_LAYER:

	case EMT_TRANSPARENT_VERTEX_ALPHA:
	case EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA:
	case EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA:
		RenderPass_ShaderIsTransparent = 1;
		AlphaRef = tofix(material.org.MaterialTypeParam, FIX_POINT_COLOR_MAX);
		break;
	default:
		RenderPass_ShaderIsTransparent = 0;
		AlphaRef = 0;
		break;
	}

	if (0 == RenderPass_ShaderIsTransparent)
	{
		if (material.org.ZBuffer == ECFN_LESSEQUAL)
		{
			if (material.org.ColorMask == ECP_NONE)
				fragmentShader = &burning_shader_class::fragment_depth_less_equal_no_depth_write_colormask_none;
			else if (material.depth_write) fragmentShader = &burning_shader_class::fragment_depth_less_equal_depth_write_blend_one_zero;
			else fragmentShader = &burning_shader_class::fragment_depth_less_equal_no_depth_write_blend_one_zero;
		}
		else /*if (material.org.ZBuffer == ECFN_DISABLED)*/
		{
			//check triangle on w = 1.f instead..
#ifdef	SOFTWARE_DRIVER_2_BILINEAR
			if (material.org.TextureLayer[0].BilinearFilter) fragmentShader = &burning_shader_class::fragment_nodepth_perspective_blend_one_zero;
			else
#endif
				fragmentShader = &burning_shader_class::fragment_nodepth_noperspective_blend_one_zero;
		}
	}
	else
	{
		if (material.org.ZBuffer == ECFN_LESSEQUAL)
		{
			if (material.depth_write) fragmentShader = &burning_shader_class::fragment_depth_less_equal_depth_write_blend_src_alpha_one_minus_src_alpha;
			else fragmentShader = &burning_shader_class::fragment_depth_less_equal_no_depth_write_blend_src_alpha_one_minus_src_alpha;
		}
		else /*if (material.org.ZBuffer == ECFN_DISABLED)*/
		{
			//check triangle on w = 1.f instead..
#ifdef	SOFTWARE_DRIVER_2_BILINEAR
			if (material.org.TextureLayer[0].BilinearFilter) fragmentShader = &burning_shader_class::fragment_nodepth_perspective_blend_src_alpha_one_minus_src_alpha;
			else
#endif
				fragmentShader = &burning_shader_class::fragment_nodepth_noperspective_blend_src_alpha_one_minus_src_alpha;
		}
	}

}



} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_BURNINGSVIDEO_

