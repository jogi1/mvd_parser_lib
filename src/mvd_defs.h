#define __LITTLE_ENDIAN__Q__
#ifdef __BIG_ENDIAN__Q__
#define BigShort(x)(x)
#define BigLong(x)(x)
#define BigFloat(x)(x)
#define LittleShort(x)ShortSwap(x)
#define LittleLong(x)LongSwap(x)
#define LittleFloat(x)FloatSwap(x)
#elif defined(__LITTLE_ENDIAN__Q__)
#define BigShort(x)ShortSwap(x)
#define BigLong(x)LongSwap(x)
#define BigFloat(x)FloatSwap(x)
#define LittleShort(x)(x)
#define LittleLong(x)(x)
#define LittleFloat(x)(x)
#elif defined(__PDP_ENDIAN__Q__)
#define BigShort(x)ShortSwap(x)
#define BigLong(x)LongSwapPDP2Big(x)
#define BigFloat(x)FloatSwapPDP2Big(x)
#define LittleShort(x)(x)
#define LittleLong(x)LongSwapPDP2Lit(x)
#define LittleFloat(x)FloatSwapPDP2Lit(x)
#else
#error Unknown byte order type!
#endif

typedef struct sizebuf_s
{
    bool	allowoverflow;	// if false, do a Sys_Error
    bool	overflowed;		// set to true if the buffer size failed
    byte	*data;
    int		maxsize;
    int		cursize;
} sizebuf_t;

#define VectorCopy(a,b)                       ((b)[0] = (a)[0], (b)[1] = (a)[1], (b)[2] = (a)[2])

#define VectorSubtract(a,b,c) ((c)[0] = (a)[0] - (b)[0], (c)[1] = (a)[1] - (b)[1], (c)[2] = (a)[2] - (b)[2])

