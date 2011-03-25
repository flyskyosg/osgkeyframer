#include "stdafx.h"
#include "CRSpline.h"

CRSpline::CRSpline()
: vp(), delta_t(0)
{
}

CRSpline::CRSpline(const CRSpline& s)
{
    for (int i = 0; i < (int)s.vp.size(); i++)
        vp.push_back(s.vp[i]);
    delta_t = s.delta_t;
}

CRSpline::~CRSpline()
{}

// Solve the Catmull-Rom parametric equation for a given time(t) and vector quadruple (p1,p2,p3,p4)
vec3 CRSpline::Eq(float t, const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4)
{
    float t2 = t * t;
    float t3 = t2 * t;

    float b1 = .5 * (  -t3 + 2*t2 - t);
    float b2 = .5 * ( 3*t3 - 5*t2 + 2);
    float b3 = .5 * (-3*t3 + 4*t2 + t);
    float b4 = .5 * (   t3 -   t2    );

    return (p1*b1 + p2*b2 + p3*b3 + p4*b4); 
}

float derive_x(const vec3& p0, const vec3& p1, const vec3& p2)
{
	float delta = p2.t-p0.t;if(delta == 0) return 0;
	
	return (p2.x-p0.x) / (p2.t - p0.t);
}
float derive_y(const vec3& p0, const vec3& p1, const vec3& p2)
{
	float delta = p2.t-p0.t;if(delta == 0) return 0;
	return (p2.y-p0.y) / (p2.t - p0.t);
}
float derive_z(const vec3& p0, const vec3& p1, const vec3& p2)
{
	float delta = p2.t-p0.t;if(delta == 0) return 0;
	return (p2.z-p0.z) / (p2.t - p0.t);
}
float derive_rx(const vec3& p0, const vec3& p1, const vec3& p2)
{
	float delta = p2.t-p0.t;if(delta == 0) return 0;
	return (p2.rx-p0.rx) / (p2.t - p0.t);
}
float derive_ry(const vec3& p0, const vec3& p1, const vec3& p2)
{
	float delta = p2.t-p0.t;if(delta == 0) return 0;
	return (p2.ry-p0.ry) / (p2.t - p0.t);
}
float derive_rz(const vec3& p0, const vec3& p1, const vec3& p2)
{
	float delta = p2.t-p0.t;if(delta == 0) return 0;
	return (p2.rz-p0.rz) / (p2.t - p0.t);
}
float derive_ra(const vec3& p0, const vec3& p1, const vec3& p2)
{
	float delta = p2.t-p0.t;if(delta == 0) return 0;
	return (p2.ra-p0.ra) / (p2.t - p0.t);
}
//float derive_x(const vec3& p0, const vec3& p1, const vec3& p2)
//{
//	float delta = p1.t-p0.t;if(delta == 0) return 0;
//	
//	return (1 /(2*delta)) * ( (p1.x - p0.x) + (p2.x - p1.x));
//}
//float derive_y(const vec3& p0, const vec3& p1, const vec3& p2)
//{
//	float delta = p1.t-p0.t;if(delta == 0) return 0;
//	return (1 /(2*delta)) * ( (p1.y - p0.y) + (p2.y - p1.y));
//}
//float derive_z(const vec3& p0, const vec3& p1, const vec3& p2)
//{
//	float delta = p1.t-p0.t;if(delta == 0) return 0;
//	return (1 /(2*delta)) * ( (p1.z - p0.z) + (p2.z - p1.z));
//}
//float derive_rx(const vec3& p0, const vec3& p1, const vec3& p2)
//{
//	float delta = p1.t-p0.t;if(delta == 0) return 0;
//	return (1 /(2*delta)) * ( (p1.rx - p0.rx) + (p2.rx - p1.rx));
//}
//float derive_ry(const vec3& p0, const vec3& p1, const vec3& p2)
//{
//	float delta = p1.t-p0.t;if(delta == 0) return 0;
//	return (1 /(2*delta)) * ( (p1.ry - p0.ry) + (p2.ry - p1.ry));
//}
//float derive_rz(const vec3& p0, const vec3& p1, const vec3& p2)
//{
//	float delta = p1.t-p0.t;if(delta == 0) return 0;
//	return (1 /(2*delta)) * ( (p1.rz - p0.rz) + (p2.rz - p1.rz));
//}
//float derive_ra(const vec3& p0, const vec3& p1, const vec3& p2)
//{
//	float delta = p1.t-p0.t;if(delta == 0) return 0;
//	return (1 /(2*delta)) * ( (p1.ra - p0.ra) + (p2.ra - p1.ra));
//}

vec3 Catmull(float u, float deltax, const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4, float tight = 0)
{
	float u2 = u*u;
	float u3 = u2*u;
	vec3 result;	

	result.x =      p2.x*(2*u3 - 3*u2 +1);
	result.x +=     p3.x*(3*u2 - 2*u3);
	result.x += ((1-tight)*derive_x(p1,p2,p3)) * deltax *(u3- 2*u2+u);
	result.x += ((1-tight)*derive_x(p2,p3,p4)) * deltax *(u3- u2);

	result.y =		p2.y*(2*u3 - 3*u2 +1);
	result.y +=		p3.y*(3*u2 - 2*u3);
	result.y += ((1-tight)*derive_y(p1,p2,p3)) * deltax *(u3- 2*u2+u);
	result.y += ((1-tight)*derive_y(p2,p3,p4)) * deltax *(u3- u2);

	result.z =      p2.z*(2*u3 - 3*u2 +1);
	result.z +=     p3.z*(3*u2 - 2*u3);
	result.z += ((1-tight)*derive_z(p1,p2,p3)) * deltax *(u3- 2*u2+u);
	result.z += ((1-tight)*derive_z(p2,p3,p4)) * deltax *(u3- u2);

	result.rx =      p2.rx*(2*u3 - 3*u2 +1);
	result.rx +=     p3.rx*(3*u2 - 2*u3);
	result.rx += ((1-tight)*derive_rx(p1,p2,p3)) * deltax *(u3- 2*u2+u);
	result.rx += ((1-tight)*derive_rx(p2,p3,p4)) * deltax *(u3- u2);

	result.ry =      p2.ry*(2*u3 - 3*u2 +1);
	result.ry +=     p3.ry*(3*u2 - 2*u3);
	result.ry += ((1-tight)*derive_ry(p1,p2,p3)) * deltax *(u3- 2*u2+u);
	result.ry += ((1-tight)*derive_ry(p2,p3,p4)) * deltax *(u3- u2);

	result.rz =      p2.rz*(2*u3 - 3*u2 +1);
	result.rz +=     p3.rz*(3*u2 - 2*u3);
	result.rz += ((1-tight)*derive_rz(p1,p2,p3)) * deltax *(u3- 2*u2+u);
	result.rz += ((1-tight)*derive_rz(p2,p3,p4)) * deltax *(u3- u2);

	result.ra =      p2.ra*(2*u3 - 3*u2 +1);
	result.ra +=     p3.ra*(3*u2 - 2*u3);
	result.ra += ((1-tight)*derive_ra(p1,p2,p3)) * deltax *(u3- 2*u2+u);
	result.ra += ((1-tight)*derive_ra(p2,p3,p4)) * deltax *(u3- u2);

	return result;

}
void CRSpline::AddSplinePoint(const vec3& v)
{

    vp.push_back(v);
	
    delta_t = (float)1 / (float)(vp.size());
}


vec3 CRSpline::GetInterpolatedSplinePoint(float t)
{
    // Find out in which interval we are on the spline
    int p = (int)(t / delta_t);
    // Compute local control point indices
#define BOUNDS(pp) { if (pp < 0) pp = 0; else if (pp >= (int)vp.size()-1) pp = vp.size() - 1; }
    int p0 = p - 1;     BOUNDS(p0);
    int p1 = p;         BOUNDS(p1);
    int p2 = p + 1;     BOUNDS(p2);
    int p3 = p + 2;     BOUNDS(p3);
    // Relative (local) time 
	float lt = (t - delta_t*(float)p) / delta_t;
	// Interpolate
    return CRSpline::Eq(lt, vp[p0], vp[p1], vp[p2], vp[p3]);
}

vec3 CRSpline::GetInterpolatedSplineKeyFrame(float time, float tight )
{
	if(vp[vp.size()-1].t < time)
		return vec3(0,0,0);
    // Find out in which interval we are on the spline
	int p = 0;
	if(time >= vp[vp.size()-1].t) 
		p = vp.size()-2;
	else
		while (time >= vp[p+1].t) p++;

	//int p = vp.size()/2;
	//

	//while( !(vp[p].t <= time && vp[p+1].t >= time) )
	//{
	//	if(vp[p].t > time)
	//		p = p/2;
	//	else
	//		p +=(vp.size() - p)/2;
	//}
	int p0 = p > 0 ? p-1 : p;
	int p1 = p;
	int p2 = p == (vp.size()-1) ? p : p+1;
	int p3 = p >= (vp.size()-2) ? p : p+2;
	
    // Relative (local) time 
	//float lt =  (time - vp[p1].x) / (vp[p2].x -vp[p1].x);
	float deltax = vp[p2].t - vp[p1].t;
	float u = (time - vp[p1].t)/deltax;
	vec3 result = Catmull(u,deltax,vp[p0], vp[p1], vp[p2], vp[p3], tight);

	result.t = time;
	return result;

}

int CRSpline::GetNumPoints()
{
	return vp.size();
}

vec3& CRSpline::GetNthPoint(int n)
{
	return vp[n];
}


