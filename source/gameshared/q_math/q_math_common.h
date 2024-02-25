

//----------------------------------------------------------------------------
// floatX
//----------------------------------------------------------------------------
// A simple structure containing 3 floating point values.
// float3 is always guaranteed to be 3 floats in size. Only use when a very
// specific size is required (like defining structures that need to be the
// same across platforms, or the same on CPU and GPU (like constant and
// structured buffers) In all other cases you should opt to use Vector3, since
// it uses SIMD optimizations whenever possible. float3 does not.
//----------------------------------------------------------------------------

struct float4 {
  union {
	  struct {
		  float x, y, z, w;
	  };
	  float v[4];
  };
};

struct float3 {
  union {
	  struct {
		  float x, y, z;
	  };
	  float v[3];
  };
};

struct float2 {
  union {
	  struct {
		  float x, y;
	  };
	  float v[2];
  };
};

