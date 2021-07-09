// pixelshader
#ifdef IPOL_C0

#ifdef IPOL_A0
vec4_to_fix(a0, r0, g0, b0, line.c[0][0], inversew);
if (a0 > AlphaRef)
{
	color_to_fix(r1, g1, b1, dst[i]);

	fix_color_norm(a0);
	r0 = r1 + imulFix(a0, r0 - r1);
	g0 = g1 + imulFix(a0, g0 - g1);
	b0 = b1 + imulFix(a0, b0 - b1);
	dst[i] = fix_to_sample(r0, g0, b0);
}
#else
vec4_to_fix(r0, g0, b0, line.c[0][0], inversew);
dst[i] = fix_to_sample(r0, g0, b0);
#endif

#else
dst[i] = PrimitiveColor;
#endif

