#include "burning_shader_compile_verify.h"

/*!
*/
void burning_shader_class::burning_shader_fragment()
{
#ifdef burning_shader_colormask
#else
	tVideoSample *dst;
#endif

#ifdef USE_ZBUFFER
	fp24 *z;
#endif

	s32 xStart;
	s32 xEnd;
	s32 dx;

#ifdef SUBTEXEL
	f32 subPixel;
#endif

#ifdef IPOL_Z
	f32 slopeZ;
#endif
#ifdef IPOL_W
	fp24 slopeW;
#endif
#ifdef IPOL_C0
	sVec4 slopeC[BURNING_MATERIAL_MAX_COLORS];
#endif
#ifdef IPOL_T0
	sVec2 slopeT[BURNING_MATERIAL_MAX_TEXTURES];
#endif

	// apply top-left fill-convention, left
	xStart = fill_convention_left(line.x[0]);
	xEnd = fill_convention_right(line.x[1]);

	dx = xEnd - xStart;
	if (dx < 0)
		return;

	// slopes
	const f32 invDeltaX = fill_step_x(line.x[1] - line.x[0]);

#ifdef IPOL_Z
	slopeZ = (line.z[1] - line.z[0]) * invDeltaX;
#endif
#ifdef IPOL_W
	slopeW = (line.w[1] - line.w[0]) * invDeltaX;
#endif
#ifdef IPOL_C0
	slopeC[0] = (line.c[0][1] - line.c[0][0]) * invDeltaX;
#endif
#ifdef IPOL_C1
	slopeC[1] = (line.c[1][1] - line.c[1][0]) * invDeltaX;
#endif
#ifdef IPOL_T0
	slopeT[0] = (line.t[0][1] - line.t[0][0]) * invDeltaX;
#endif
#ifdef IPOL_T1
	slopeT[1] = (line.t[1][1] - line.t[1][0]) * invDeltaX;
#endif

#ifdef SUBTEXEL
	subPixel = ((f32)xStart) - line.x[0];
#ifdef IPOL_Z
	line.z[0] += slopeZ * subPixel;
#endif
#ifdef IPOL_W
	line.w[0] += slopeW * subPixel;
#endif
#ifdef IPOL_C0
	line.c[0][0] += slopeC[0] * subPixel;
#endif
#ifdef IPOL_C1
	line.c[1][0] += slopeC[1] * subPixel;
#endif
#ifdef IPOL_T0
	line.t[0][0] += slopeT[0] * subPixel;
#endif
#ifdef IPOL_T1
	line.t[1][0] += slopeT[1] * subPixel;
#endif
#endif

	SOFTWARE_DRIVER_2_CLIPCHECK;
#ifdef burning_shader_colormask
#else
	dst = (tVideoSample*)RenderTarget->getData() + (line.y * RenderTarget->getDimension().Width) + xStart;
#endif

#ifdef USE_ZBUFFER
	z = (fp24*)DepthBuffer->lock() + (line.y * RenderTarget->getDimension().Width) + xStart;
#endif

#ifdef burning_shader_colormask
#else
	f32 inversew = INVERSE_W_RANGE;
#endif

#ifdef IPOL_C0
	tFixPoint a0,r0, g0, b0;
#endif

#ifdef IPOL_C1
	tFixPoint r2, g2, b2;
#endif

#ifdef IPOL_A0
	tFixPoint r1, g1, b1;
#endif

	for (s32 i = 0; i <= dx; i+= SOFTWARE_DRIVER_2_STEP_X)
	{
		if ((0 == EdgeTestPass) & (i > line.x_edgetest)) break;

#ifdef CMP_Z
		if (line.z[0] < z[i])
#endif
#ifdef CMP_W
		if (line.w[0] >= z[i])
#endif
		{
#ifdef WRITE_Z
			z[i] = line.z[0];
#endif
#ifdef WRITE_W
			z[i] = line.w[0];
#endif
		/* Pixel Shader here */
#ifdef INVERSE_W
			inversew = (INVERSE_W_RANGE) / line.w[0]; /* fix_inverse32(line.w[0]);*/
#endif
