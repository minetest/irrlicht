#include "burning_shader_compile_verify.h"


void burning_shader_class::drawTriangle(const s4DVertex* burning_restrict a, const s4DVertex* burning_restrict b, const s4DVertex* burning_restrict c)
{
	// sort on height, y
	if (a->Pos.y > b->Pos.y) swapVertexPointer(&a, &b);
	if (a->Pos.y > c->Pos.y) swapVertexPointer(&a, &c);
	if (b->Pos.y > c->Pos.y) swapVertexPointer(&b, &c);

	const f32 ca = c->Pos.y - a->Pos.y;
	const f32 ba = b->Pos.y - a->Pos.y;
	const f32 cb = c->Pos.y - b->Pos.y;

	// calculate delta y of the edges
	scan.invDeltaY[0] = reciprocal_edge(ca);
	scan.invDeltaY[1] = reciprocal_edge(ba);
	scan.invDeltaY[2] = reciprocal_edge(cb);

	if (F32_LOWER_EQUAL_0(scan.invDeltaY[0]))
		return;

	// find if the major edge is left or right aligned
	f32 temp[4];

	temp[0] = a->Pos.x - c->Pos.x;
	temp[1] = -ca;
	temp[2] = b->Pos.x - a->Pos.x;
	temp[3] = ba;

	scan.left = (temp[0] * temp[3] - temp[1] * temp[2]) > 0.f ? 0 : 1;
	scan.right = 1 - scan.left;

	// calculate slopes for the major edge
	scan.slopeX[0] = (c->Pos.x - a->Pos.x) * scan.invDeltaY[0];
	scan.x[0] = a->Pos.x;

#ifdef IPOL_Z
	scan.slopeZ[0] = (c->Pos.z - a->Pos.z) * scan.invDeltaY[0];
	scan.z[0] = a->Pos.z;
#endif

#ifdef IPOL_W
	scan.slopeW[0] = (c->Pos.w - a->Pos.w) * scan.invDeltaY[0];
	scan.w[0] = a->Pos.w;
#endif

#ifdef IPOL_C0
	scan.slopeC[0][0] = (c->Color[0] - a->Color[0]) * scan.invDeltaY[0];
	scan.c[0][0] = a->Color[0];
#endif

#ifdef IPOL_T0
	scan.slopeT[0][0] = (c->Tex[0] - a->Tex[0]) * scan.invDeltaY[0];
	scan.t[0][0] = a->Tex[0];
#endif

#ifdef IPOL_T1
	scan.slopeT[1][0] = (c->Tex[1] - a->Tex[1]) * scan.invDeltaY[0];
	scan.t[1][0] = a->Tex[1];
#endif

	// top left fill convention y run
	s32 yStart;
	s32 yEnd;

#ifdef SUBTEXEL
	f32 subPixel;
#endif


	// rasterize upper sub-triangle
	if (F32_GREATER_0(scan.invDeltaY[1]))
	{
		// calculate slopes for top edge
		scan.slopeX[1] = (b->Pos.x - a->Pos.x) * scan.invDeltaY[1];
		scan.x[1] = a->Pos.x;

#ifdef IPOL_Z
		scan.slopeZ[1] = (b->Pos.z - a->Pos.z) * scan.invDeltaY[1];
		scan.z[1] = a->Pos.z;
#endif

#ifdef IPOL_W
		scan.slopeW[1] = (b->Pos.w - a->Pos.w) * scan.invDeltaY[1];
		scan.w[1] = a->Pos.w;
#endif

#ifdef IPOL_C0
		scan.slopeC[0][1] = (b->Color[0] - a->Color[0]) * scan.invDeltaY[1];
		scan.c[0][1] = a->Color[0];
#endif

#ifdef IPOL_T0
		scan.slopeT[0][1] = (b->Tex[0] - a->Tex[0]) * scan.invDeltaY[1];
		scan.t[0][1] = a->Tex[0];
#endif

#ifdef IPOL_T1
		scan.slopeT[1][1] = (b->Tex[1] - a->Tex[1]) * scan.invDeltaY[1];
		scan.t[1][1] = a->Tex[1];
#endif

		// apply top-left fill convention, top part
		yStart = fill_convention_left(a->Pos.y);
		yEnd = fill_convention_right(b->Pos.y);

#ifdef SUBTEXEL
		subPixel = ((f32)yStart) - a->Pos.y;

		// correct to pixel center
		scan.x[0] += scan.slopeX[0] * subPixel;
		scan.x[1] += scan.slopeX[1] * subPixel;

#ifdef IPOL_Z
		scan.z[0] += scan.slopeZ[0] * subPixel;
		scan.z[1] += scan.slopeZ[1] * subPixel;
#endif

#ifdef IPOL_W
		scan.w[0] += scan.slopeW[0] * subPixel;
		scan.w[1] += scan.slopeW[1] * subPixel;
#endif

#ifdef IPOL_C0
		scan.c[0][0] += scan.slopeC[0][0] * subPixel;
		scan.c[0][1] += scan.slopeC[0][1] * subPixel;
#endif

#ifdef IPOL_T0
		scan.t[0][0] += scan.slopeT[0][0] * subPixel;
		scan.t[0][1] += scan.slopeT[0][1] * subPixel;
#endif

#ifdef IPOL_T1
		scan.t[1][0] += scan.slopeT[1][0] * subPixel;
		scan.t[1][1] += scan.slopeT[1][1] * subPixel;
#endif

#endif

		// rasterize the edge scanlines
		for (line.y = yStart; line.y <= yEnd; ++line.y)
		{
			line.x[scan.left] = scan.x[0];
			line.x[scan.right] = scan.x[1];

#ifdef IPOL_Z
			line.z[scan.left] = scan.z[0];
			line.z[scan.right] = scan.z[1];
#endif

#ifdef IPOL_W
			line.w[scan.left] = scan.w[0];
			line.w[scan.right] = scan.w[1];
#endif

#ifdef IPOL_C0
			line.c[0][scan.left] = scan.c[0][0];
			line.c[0][scan.right] = scan.c[0][1];
#endif

#ifdef IPOL_T0
			line.t[0][scan.left] = scan.t[0][0];
			line.t[0][scan.right] = scan.t[0][1];
#endif

#ifdef IPOL_T1
			line.t[1][scan.left] = scan.t[1][0];
			line.t[1][scan.right] = scan.t[1][1];
#endif

			// render a scanline
			(this->*fragmentShader) ();
			if (EdgeTestPass & edge_test_first_line) break;

			scan.x[0] += scan.slopeX[0];
			scan.x[1] += scan.slopeX[1];

#ifdef IPOL_Z
			scan.z[0] += scan.slopeZ[0];
			scan.z[1] += scan.slopeZ[1];
#endif

#ifdef IPOL_W
			scan.w[0] += scan.slopeW[0];
			scan.w[1] += scan.slopeW[1];
#endif

#ifdef IPOL_C0
			scan.c[0][0] += scan.slopeC[0][0];
			scan.c[0][1] += scan.slopeC[0][1];
#endif

#ifdef IPOL_T0
			scan.t[0][0] += scan.slopeT[0][0];
			scan.t[0][1] += scan.slopeT[0][1];
#endif

#ifdef IPOL_T1
			scan.t[1][0] += scan.slopeT[1][0];
			scan.t[1][1] += scan.slopeT[1][1];
#endif
		}
	}

	// rasterize lower sub-triangle
	if (F32_GREATER_0(scan.invDeltaY[2]))
	{
		// advance to middle point
		if (F32_GREATER_0(scan.invDeltaY[1]))
		{
			temp[0] = b->Pos.y - a->Pos.y;	// dy

			scan.x[0] = a->Pos.x + scan.slopeX[0] * temp[0];
#ifdef IPOL_Z
			scan.z[0] = a->Pos.z + scan.slopeZ[0] * temp[0];
#endif
#ifdef IPOL_W
			scan.w[0] = a->Pos.w + scan.slopeW[0] * temp[0];
#endif
#ifdef IPOL_C0
			scan.c[0][0] = a->Color[0] + scan.slopeC[0][0] * temp[0];
#endif
#ifdef IPOL_T0
			scan.t[0][0] = a->Tex[0] + scan.slopeT[0][0] * temp[0];
#endif
#ifdef IPOL_T1
			scan.t[1][0] = a->Tex[1] + scan.slopeT[1][0] * temp[0];
#endif

		}

		// calculate slopes for bottom edge
		scan.slopeX[1] = (c->Pos.x - b->Pos.x) * scan.invDeltaY[2];
		scan.x[1] = b->Pos.x;

#ifdef IPOL_Z
		scan.slopeZ[1] = (c->Pos.z - b->Pos.z) * scan.invDeltaY[2];
		scan.z[1] = b->Pos.z;
#endif

#ifdef IPOL_W
		scan.slopeW[1] = (c->Pos.w - b->Pos.w) * scan.invDeltaY[2];
		scan.w[1] = b->Pos.w;
#endif

#ifdef IPOL_C0
		scan.slopeC[0][1] = (c->Color[0] - b->Color[0]) * scan.invDeltaY[2];
		scan.c[0][1] = b->Color[0];
#endif

#ifdef IPOL_T0
		scan.slopeT[0][1] = (c->Tex[0] - b->Tex[0]) * scan.invDeltaY[2];
		scan.t[0][1] = b->Tex[0];
#endif

#ifdef IPOL_T1
		scan.slopeT[1][1] = (c->Tex[1] - b->Tex[1]) * scan.invDeltaY[2];
		scan.t[1][1] = b->Tex[1];
#endif

		// apply top-left fill convention, top part
		yStart = fill_convention_left(b->Pos.y);
		yEnd = fill_convention_right(c->Pos.y);

#ifdef SUBTEXEL

		subPixel = ((f32)yStart) - b->Pos.y;

		// correct to pixel center
		scan.x[0] += scan.slopeX[0] * subPixel;
		scan.x[1] += scan.slopeX[1] * subPixel;

#ifdef IPOL_Z
		scan.z[0] += scan.slopeZ[0] * subPixel;
		scan.z[1] += scan.slopeZ[1] * subPixel;
#endif

#ifdef IPOL_W
		scan.w[0] += scan.slopeW[0] * subPixel;
		scan.w[1] += scan.slopeW[1] * subPixel;
#endif

#ifdef IPOL_C0
		scan.c[0][0] += scan.slopeC[0][0] * subPixel;
		scan.c[0][1] += scan.slopeC[0][1] * subPixel;
#endif

#ifdef IPOL_T0
		scan.t[0][0] += scan.slopeT[0][0] * subPixel;
		scan.t[0][1] += scan.slopeT[0][1] * subPixel;
#endif

#ifdef IPOL_T1
		scan.t[1][0] += scan.slopeT[1][0] * subPixel;
		scan.t[1][1] += scan.slopeT[1][1] * subPixel;
#endif

#endif

		// rasterize the edge scanlines
		for (line.y = yStart; line.y <= yEnd; ++line.y)
		{
			line.x[scan.left] = scan.x[0];
			line.x[scan.right] = scan.x[1];

#ifdef IPOL_Z
			line.z[scan.left] = scan.z[0];
			line.z[scan.right] = scan.z[1];
#endif

#ifdef IPOL_W
			line.w[scan.left] = scan.w[0];
			line.w[scan.right] = scan.w[1];
#endif

#ifdef IPOL_C0
			line.c[0][scan.left] = scan.c[0][0];
			line.c[0][scan.right] = scan.c[0][1];
#endif

#ifdef IPOL_T0
			line.t[0][scan.left] = scan.t[0][0];
			line.t[0][scan.right] = scan.t[0][1];
#endif

#ifdef IPOL_T1
			line.t[1][scan.left] = scan.t[1][0];
			line.t[1][scan.right] = scan.t[1][1];
#endif

			// render a scanline
			(this->*fragmentShader) ();
			if (EdgeTestPass & edge_test_first_line) break;

			scan.x[0] += scan.slopeX[0];
			scan.x[1] += scan.slopeX[1];

#ifdef IPOL_Z
			scan.z[0] += scan.slopeZ[0];
			scan.z[1] += scan.slopeZ[1];
#endif

#ifdef IPOL_W
			scan.w[0] += scan.slopeW[0];
			scan.w[1] += scan.slopeW[1];
#endif

#ifdef IPOL_C0
			scan.c[0][0] += scan.slopeC[0][0];
			scan.c[0][1] += scan.slopeC[0][1];
#endif

#ifdef IPOL_T0
			scan.t[0][0] += scan.slopeT[0][0];
			scan.t[0][1] += scan.slopeT[0][1];
#endif

#ifdef IPOL_T1
			scan.t[1][0] += scan.slopeT[1][0];
			scan.t[1][1] += scan.slopeT[1][1];
#endif

		}
	}

}
