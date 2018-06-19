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

#ifndef _DG_SOLVER_H_
#define _DG_SOLVER_H_


#include "dgPhysicsStdafx.h"

#if 0
class dgBodyInfo;
class dgJointInfo;
class dgBodyCluster;

#define DG_WORK_GROUP_SIZE	8 


DG_MSC_VECTOR_ALIGMENT
class dgWorkGroupFloat
{
	public:
	DG_INLINE dgWorkGroupFloat()
	{
	}

	DG_INLINE dgWorkGroupFloat(const dgWorkGroupFloat& me)
		:m_low(me.m_low)
		,m_high(me.m_high)
	{
	}

	DG_INLINE dgWorkGroupFloat(const dgVector& v, const dgVector& high)
		:m_low(v)
		,m_high(high)
	{
	}

	DG_INLINE dgFloat32& operator[] (dgInt32 i)
	{
		dgAssert(i < DG_WORK_GROUP_SIZE);
		dgAssert(i >= 0);
		return m_f[i];
	}

	DG_INLINE const dgFloat32& operator[] (dgInt32 i) const
	{
		dgAssert(i < DG_WORK_GROUP_SIZE);
		dgAssert(i >= 0);
		return m_f[i];
	}

	DG_INLINE dgWorkGroupFloat operator+ (const dgWorkGroupFloat& A) const
	{
		return dgWorkGroupFloat(m_low + A.m_low, m_high + A.m_high);
	}

	DG_INLINE dgWorkGroupFloat operator- (const dgWorkGroupFloat& A) const
	{
		return dgWorkGroupFloat(m_low - A.m_low, m_high - A.m_high);
	}

	DG_INLINE dgWorkGroupFloat operator* (const dgWorkGroupFloat& A) const
	{
		return dgWorkGroupFloat(m_low * A.m_low, m_high * A.m_high);
	}

	DG_INLINE dgWorkGroupFloat operator> (const dgWorkGroupFloat& A) const
	{
		return dgWorkGroupFloat(m_low > A.m_low, m_high > A.m_high);
	}

	DG_INLINE dgWorkGroupFloat operator< (const dgWorkGroupFloat& A) const
	{
		return dgWorkGroupFloat(m_low < A.m_low, m_high < A.m_high);
	}

	DG_INLINE dgWorkGroupFloat operator| (const dgWorkGroupFloat& A) const
	{
		return dgWorkGroupFloat(m_low | A.m_low, m_high | A.m_high);
	}

	DG_INLINE dgWorkGroupFloat AndNot (const dgWorkGroupFloat& A) const
	{
		return dgWorkGroupFloat(m_low.AndNot(A.m_low), m_high.AndNot(A.m_high));
	}

	DG_INLINE dgWorkGroupFloat GetMin(const dgWorkGroupFloat& A) const
	{
		return dgWorkGroupFloat(m_low.GetMin(A.m_low), m_high.GetMin(A.m_high));
	}

	DG_INLINE dgWorkGroupFloat GetMax(const dgWorkGroupFloat& A) const
	{
		return dgWorkGroupFloat(m_low.GetMax(A.m_low), m_high.GetMax(A.m_high));
	}

	DG_INLINE dgFloat32 GetMax() const
	{
		return (m_low.GetMax(m_high)).GetMax();
	}

	DG_INLINE static void Transpose4x8(dgWorkGroupFloat& src0, dgWorkGroupFloat& src1, dgWorkGroupFloat& src2, dgWorkGroupFloat& src3)
	{
		dgVector::Transpose4x4(src0.m_low, src1.m_low, src2.m_low, src3.m_low, src0.m_low, src1.m_low, src2.m_low, src3.m_low);
		dgVector::Transpose4x4(src0.m_high, src1.m_high, src2.m_high, src3.m_high, src0.m_high, src1.m_high, src2.m_high, src3.m_high);
	}

	union {
		dgFloat32 m_f[DG_WORK_GROUP_SIZE];
		dgInt32 m_i[DG_WORK_GROUP_SIZE];
		struct {
			dgVector m_low;
			dgVector m_high;
		};
	};

	static dgWorkGroupFloat m_one;
	static dgWorkGroupFloat m_zero;
} DG_GCC_VECTOR_ALIGMENT;


DG_MSC_VECTOR_ALIGMENT
class dgWorkGroupVector3
{
	public:
	dgWorkGroupFloat m_x;
	dgWorkGroupFloat m_y;
	dgWorkGroupFloat m_z;
} DG_GCC_VECTOR_ALIGMENT;


DG_MSC_VECTOR_ALIGMENT
class dgWorkGroupVector6
{
	public:
	dgWorkGroupVector3 m_linear;
	dgWorkGroupVector3 m_angular;
} DG_GCC_AVX_ALIGMENT;

DG_MSC_VECTOR_ALIGMENT
class dgSolverSoaJacobianPair
{
	public:
	dgWorkGroupVector6 m_jacobianM0;
	dgWorkGroupVector6 m_jacobianM1;
} DG_GCC_VECTOR_ALIGMENT;


DG_MSC_VECTOR_ALIGMENT
class dgSolverSoaElement
{
	public:
	dgSolverSoaJacobianPair m_Jt;
	dgSolverSoaJacobianPair m_JMinv;

	dgWorkGroupFloat m_force;
	dgWorkGroupFloat m_diagDamp;
	dgWorkGroupFloat m_invJinvMJt;
	dgWorkGroupFloat m_coordenateAccel;
	dgWorkGroupFloat m_normalForceIndex;
	dgWorkGroupFloat m_lowerBoundFrictionCoefficent;
	dgWorkGroupFloat m_upperBoundFrictionCoefficent;
} DG_GCC_VECTOR_ALIGMENT;
#endif

class dgSolver: public dgParallelBodySolver
{
	public:
	dgSolver();
	~dgSolver();
	void CalculateJointForces(dgBodyCluster& cluster, dgBodyInfo* const bodyArray, dgJointInfo* const jointArray, dgFloat32 timestep);

	private:
	void InitWeights();
	void InitBodyArray();
	void CalculateForces();
	void InitJacobianMatrix();
	void CalculateBodyForce();
	void UpdateForceFeedback();
	void CalculateJointsForce();
	void IntegrateBodiesVelocity();
	void UpdateKinematicFeedback();
	void CalculateJointsAcceleration();
	void CalculateBodiesAcceleration();
	
	void InitWeights(dgInt32 threadID);
	void InitBodyArray(dgInt32 threadID);
	void InitJacobianMatrix(dgInt32 threadID);
	void CalculateBodyForce(dgInt32 threadID);
	void UpdateForceFeedback(dgInt32 threadID);
	void TransposeMassMatrix(dgInt32 threadID);
	void CalculateJointsForce(dgInt32 threadID);
	void UpdateRowAcceleration(dgInt32 threadID);
	void IntegrateBodiesVelocity(dgInt32 threadID);
	void UpdateKinematicFeedback(dgInt32 threadID);
	void CalculateJointsAcceleration(dgInt32 threadID);
	void CalculateBodiesAcceleration(dgInt32 threadID);

	static void InitWeightKernel(void* const context, void* const, dgInt32 threadID);
	static void InitBodyArrayKernel(void* const context, void* const, dgInt32 threadID);
	static void InitJacobianMatrixKernel(void* const context, void* const, dgInt32 threadID);
	static void CalculateBodyForceKernel(void* const context, void* const, dgInt32 threadID);
	static void UpdateForceFeedbackKernel(void* const context, void* const, dgInt32 threadID);
	static void TransposeMassMatrixKernel(void* const context, void* const, dgInt32 threadID);
	static void CalculateJointsForceKernel(void* const context, void* const, dgInt32 threadID);
	static void UpdateRowAccelerationKernel(void* const context, void* const, dgInt32 threadID);
	static void IntegrateBodiesVelocityKernel(void* const context, void* const, dgInt32 threadID);
	static void UpdateKinematicFeedbackKernel(void* const context, void* const, dgInt32 threadID);
	static void CalculateBodiesAccelerationKernel(void* const context, void* const, dgInt32 threadID);
	static void CalculateJointsAccelerationKernel(void* const context, void* const, dgInt32 threadID);
	static dgInt32 CompareJointInfos(const dgJointInfo* const infoA, const dgJointInfo* const infoB, void* notUsed);

	void TransposeRow (dgSolverSoaElement* const row, const dgJointInfo* const jointInfoArray, dgInt32 index);
	void BuildJacobianMatrix(dgJointInfo* const jointInfo, dgLeftHandSide* const leftHandSide, dgRightHandSide* const righHandSide, dgJacobian* const internalForces);
	dgFloat32 CalculateJointForce(const dgJointInfo* const jointInfo, dgSolverSoaElement* const massMatrix, const dgJacobian* const internalForces) const;
};


#endif

