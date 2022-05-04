// pixelshader
#ifdef IPOL_C0

vec4_to_fix(a0, r0, g0, b0, line.c[0][0], inversew);
#ifdef IPOL_A0
if (a0 > AlphaRef)
{
	color_to_fix(r1, g1, b1, dst[i]);

	fix_color_norm(a0);
	r0 = r1 + imulFix(a0, r0 - r1);
	g0 = g1 + imulFix(a0, g0 - g1);
	b0 = b1 + imulFix(a0, b0 - b1);
	dst[i] = fix_to_sample_nearest(FIX_POINT_COLOR_MAX,r0, g0, b0);
}
#else // IPOL_A0
#ifdef IPOL_C1
vec4_to_fix(r1, g1, b1, line.c[1][0], inversew);
#endif // IPOL_C1
dst[i] = fix_to_sample_nearest(a0,r0, g0, b0);
#endif // IPOL_A0

#else // IPOL_C0
#ifdef burning_shader_colormask
fragment_draw_count += 1;
#else
dst[i] = PrimitiveColor;
#endif
#endif

