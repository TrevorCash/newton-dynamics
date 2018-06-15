/* Copyright (c) <2003-2016> <Julio Jerez, Newton Game Dynamics>
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
*
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef _DG_BODY_SSE_H_
#define _DG_BODY_SSE_H_

#include "dgNewtonPluginStdafx.h"
#include "dgWorldBase.h"
#include "dgMathSse.h"

class dgBodySse
{
	public:
	dgBodySse (dgMemoryAllocator* const allocator);
	void Reserve (dgInt32 count);
	void GetVeloc(dgInt32 index, dgDynamicBody** const bodyArray);
	void GetOmega(dgInt32 index, dgDynamicBody** const bodyArray);
	void GetMatrix(dgInt32 index, dgDynamicBody** const bodyArray);
	void GetInvMassMatrix(dgInt32 index, dgDynamicBody** const bodyArray);
	void GetDampingCoef(dgInt32 index, dgDynamicBody** const bodyArray, float timestep);

	void ApplyDampingAndCalculateInvInertia(dgInt32 index);

	dgGlobalArray<dgVector3Sse> m_veloc;
	dgGlobalArray<dgVector3Sse> m_omega;
	dgGlobalArray<dgVector3Sse> m_localInvInertia;
	dgGlobalArray<dgFloatSse> m_invMass;
	dgGlobalArray<dgFloatSse> m_linearDamp;
	dgGlobalArray<dgVector3Sse> m_angularDamp;
	dgGlobalArray<dgFloatSse> m_weigh;
	dgGlobalArray<dgFloatSse> m_invWeigh;
	dgGlobalArray<dgMatrix3x3Sse> m_rotation;
	dgGlobalArray<dgMatrix3x3Sse> m_invInertia;

	dgGlobalArray<dgVector6Sse> m_veloc0;
	dgGlobalArray<dgVector6Sse> m_internalForces;
	dgInt32 m_count;
};


DG_INLINE void dgBodySse::ApplyDampingAndCalculateInvInertia(dgInt32 index)
{
	dgVector3Sse veloc(m_veloc[index].Scale(m_linearDamp[index]));
	dgVector3Sse omega(m_rotation[index].RotateVector(m_angularDamp[index] * m_rotation[index].UnrotateVector(m_omega[index])));
	veloc.Store(&m_veloc[index]);
	omega.Store(&m_omega[index]);
	veloc.Store (&m_veloc0[index].m_linear);
	omega.Store(&m_veloc0[index].m_angular);

	dgMatrix3x3Sse invInertia(m_rotation[index].Transposed());
	invInertia.m_front = invInertia.m_front * m_localInvInertia[index];
	invInertia.m_up = invInertia.m_up * m_localInvInertia[index];
	invInertia.m_right = invInertia.m_right * m_localInvInertia[index];
	invInertia = invInertia * m_rotation[index];
	invInertia.Store(&m_invInertia[index]);
}


#endif

