
#ifndef vec3HPP
#define vec3HPP

/// Minimal 3-dimensional vector abstraction
class vec3
{
public:

    // Constructors
    vec3() : x(0), y(0), z(0)
	{}
	
	vec3(float vx, float vy, float vz)
	{
		x = vx;
		y = vy;
		z = vz;
	}

	vec3(float vt,float vx, float vy, float vz,float vrx, float vry, float vrz, float vra)
	{
		t = vt;
		x = vx;
		y = vy;
		z = vz;
		rx = vrx;
		ry = vry;
		rz = vrz;
		ra = vra;
	}

	vec3(const vec3& v)
	{
		t =	 v.t;
		x =	 v.x;
		y =	 v.y;
		z =	 v.z;
		rx = v.rx;
		ry = v.ry;
		rz = v.rz;
		ra = v.ra;
	}

	// Destructor
	~vec3() {}

	// A minimal set of vector operations
	vec3 operator * (float mult) const // result = this * arg
	{
		return vec3(x * mult, y * mult, z * mult);
	}

	vec3 operator + (const vec3& v) const // result = this + arg
	{
		return vec3(x + v.x, y + v.y, z + v.z);
	}

	vec3 operator - (const vec3& v) const // result = this - arg
	{
		return vec3(x - v.x, y - v.y, z - v.z);
	}

	float t, x, y, z, rx, ry, rz, ra;
};

#endif
