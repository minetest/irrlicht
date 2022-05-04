

class burning_shader_class : public IBurningShader
{
public:

	//! constructor
	burning_shader_class(CBurningVideoDriver* driver);

	//! draws an indexed triangle list
	virtual void drawTriangle(const s4DVertex* burning_restrict a, const s4DVertex* burning_restrict b, const s4DVertex* burning_restrict c) IRR_OVERRIDE;
	virtual bool canWireFrame() IRR_OVERRIDE { return true; }

	virtual void OnSetMaterialBurning(const SBurningShaderMaterial& material) IRR_OVERRIDE;

private:

	// fragment shader
	typedef void (burning_shader_class::*tFragmentShader) ();
	void fragment_depth_less_equal_depth_write_blend_one_zero();
	void fragment_depth_less_equal_no_depth_write_blend_one_zero();
	void fragment_nodepth_perspective_blend_one_zero();
	void fragment_nodepth_noperspective_blend_one_zero(); // 2D Gradient

	void fragment_depth_less_equal_depth_write_blend_src_alpha_one_minus_src_alpha();
	void fragment_depth_less_equal_no_depth_write_blend_src_alpha_one_minus_src_alpha();
	void fragment_nodepth_perspective_blend_src_alpha_one_minus_src_alpha();
	void fragment_nodepth_noperspective_blend_src_alpha_one_minus_src_alpha();

	void fragment_depth_less_equal_no_depth_write_colormask_none();

	tFragmentShader fragmentShader;

};


//! constructor
burning_shader_class::burning_shader_class(CBurningVideoDriver* driver)
	: IBurningShader(driver,EMT_SOLID)
{
#ifdef _DEBUG
	setDebugName(burning_stringify(burning_shader_class) );
#endif

	fragmentShader = &burning_shader_class::fragment_depth_less_equal_depth_write_blend_one_zero;

}


IBurningShader* burning_create(burning_shader_class)(CBurningVideoDriver* driver)
{
	return new burning_shader_class(driver);
}



// compile flag for this triangle
#include "burning_shader_compile_start.h"
#define SUBTEXEL
#define IPOL_W
#define IPOL_C0
#define USE_ZBUFFER
#define CMP_W
#include "burning_shader_compile_triangle.h"

// compile flag for this scanline fragment
#include "burning_shader_compile_start.h"
#define burning_shader_fragment fragment_nodepth_noperspective_blend_one_zero
#define SUBTEXEL
#define IPOL_C0
#define INVERSE_W_RANGE FIX_POINT_COLOR_MAX_CENTER
#include "burning_shader_compile_fragment_start.h"
#include burning_shader_frag
#include "burning_shader_compile_fragment_end.h"

#include "burning_shader_compile_start.h"
#define burning_shader_fragment fragment_nodepth_perspective_blend_one_zero
#define SUBTEXEL
#define INVERSE_W
#define IPOL_W
#define IPOL_C0
#define INVERSE_W_RANGE FIX_POINT_COLOR_MAX_CENTER
#include "burning_shader_compile_fragment_start.h"
#include burning_shader_frag
#include "burning_shader_compile_fragment_end.h"

#include "burning_shader_compile_start.h"
#define burning_shader_fragment fragment_depth_less_equal_no_depth_write_blend_one_zero
#define SUBTEXEL
#define INVERSE_W
#define IPOL_W
#define IPOL_C0
#define USE_ZBUFFER
#define CMP_W
#define INVERSE_W_RANGE FIX_POINT_COLOR_MAX_CENTER
#include "burning_shader_compile_fragment_start.h"
#include burning_shader_frag
#include "burning_shader_compile_fragment_end.h"

#include "burning_shader_compile_start.h"
#define burning_shader_fragment fragment_depth_less_equal_depth_write_blend_one_zero
#define SUBTEXEL
#define INVERSE_W
#define IPOL_W
#define IPOL_C0
#define USE_ZBUFFER
#define CMP_W
#define WRITE_W
#define INVERSE_W_RANGE FIX_POINT_COLOR_MAX_CENTER
#include "burning_shader_compile_fragment_start.h"
#include burning_shader_frag
#include "burning_shader_compile_fragment_end.h"


// compile flag for this scanline fragment
#include "burning_shader_compile_start.h"
#define burning_shader_fragment fragment_nodepth_noperspective_blend_src_alpha_one_minus_src_alpha
#define SUBTEXEL
#define IPOL_C0
#define IPOL_A0
#define INVERSE_W_RANGE FIX_POINT_COLOR_MAX_CENTER
#include "burning_shader_compile_fragment_start.h"
#include burning_shader_frag
#include "burning_shader_compile_fragment_end.h"

#include "burning_shader_compile_start.h"
#define burning_shader_fragment fragment_nodepth_perspective_blend_src_alpha_one_minus_src_alpha
#define SUBTEXEL
#define INVERSE_W
#define IPOL_W
#define IPOL_C0
#define IPOL_A0
#define INVERSE_W_RANGE FIX_POINT_COLOR_MAX_CENTER
#include "burning_shader_compile_fragment_start.h"
#include burning_shader_frag
#include "burning_shader_compile_fragment_end.h"

#include "burning_shader_compile_start.h"
#define burning_shader_fragment fragment_depth_less_equal_no_depth_write_blend_src_alpha_one_minus_src_alpha
#define SUBTEXEL
#define INVERSE_W
#define IPOL_W
#define IPOL_C0
#define IPOL_A0
#define USE_ZBUFFER
#define CMP_W
#define INVERSE_W_RANGE FIX_POINT_COLOR_MAX_CENTER
#include "burning_shader_compile_fragment_start.h"
#include burning_shader_frag
#include "burning_shader_compile_fragment_end.h"

#include "burning_shader_compile_start.h"
#define burning_shader_fragment fragment_depth_less_equal_depth_write_blend_src_alpha_one_minus_src_alpha
#define SUBTEXEL
#define INVERSE_W
#define IPOL_W
#define IPOL_C0
#define IPOL_A0
#define USE_ZBUFFER
#define CMP_W
#define WRITE_W
#define INVERSE_W_RANGE FIX_POINT_COLOR_MAX_CENTER
#include "burning_shader_compile_fragment_start.h"
#include burning_shader_frag
#include "burning_shader_compile_fragment_end.h"

//occlusion query
#include "burning_shader_compile_start.h"
#define burning_shader_fragment fragment_depth_less_equal_no_depth_write_colormask_none
#define IPOL_W
#define USE_ZBUFFER
#define CMP_W
#define burning_shader_colormask
#include "burning_shader_compile_fragment_start.h"
#include burning_shader_frag
#include "burning_shader_compile_fragment_end.h"
