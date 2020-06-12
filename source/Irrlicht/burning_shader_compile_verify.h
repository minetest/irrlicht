// apply global override
#ifndef SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
#undef INVERSE_W
#endif

#ifndef SOFTWARE_DRIVER_2_SUBTEXEL
#undef SUBTEXEL
#endif

#if BURNING_MATERIAL_MAX_COLORS < 1
#undef IPOL_C0
#undef IPOL_A0
#endif

#if BURNING_MATERIAL_MAX_LIGHT_TANGENT < 1
#undef IPOL_L0
#endif


// 1/x * FIX_POINT
#if !defined(INVERSE_W_RANGE)
	#define INVERSE_W_RANGE FIX_POINT_F32_MUL
#endif

#if defined ( SOFTWARE_DRIVER_2_USE_WBUFFER ) || defined ( SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT )
#else

#ifdef IPOL_W
	#undef IPOL_W
	#define IPOL_Z
#endif

#ifdef CMP_W
	#undef CMP_W
	#define CMP_Z
#endif

#ifdef WRITE_W
	#undef WRITE_W
	#define WRITE_Z
#endif

#endif
