/* Copyright (c) <2003-2016> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

#include "dStdafxVehicle.h"
#include "dVehicleManager.h"
#include "dVehicleChassis.h"
#include "dVehicleSingleBody.h"
#include "dVehicleVirtualTire.h"

dVehicleChassis::dVehicleChassis ()
	:m_localFrame(dGetIdentityMatrix())
	,m_solver()
	,m_gravity(0.0f)
	,m_obbSize(0.0f)
	,m_obbOrigin(0.0f)
	,m_vehicle(NULL)
	,m_brakeControl(NULL)
	,m_handBrakeControl(NULL)
	,m_steeringControl(NULL)
{
}

void dVehicleChassis::Init(NewtonCollision* const chassisShape, dFloat mass, const dMatrix& localFrame, NewtonApplyForceAndTorque forceAndTorque, dFloat gravityMag)
{
	dVehicleManager* const manager = (dVehicleManager*)GetManager();
	NewtonWorld* const world = manager->GetWorld();

	// create a body and call the low level init function
	dMatrix locationMatrix(dGetIdentityMatrix());
	NewtonBody* const body = NewtonCreateDynamicBody(world, chassisShape, &locationMatrix[0][0]);

	// set vehicle mass, inertia and center of mass
	NewtonBodySetMassProperties(body, mass, chassisShape);

	// initialize 
	Init(body, localFrame, forceAndTorque, gravityMag);
}

void dVehicleChassis::Init(NewtonBody* const body, const dMatrix& localFrame, NewtonApplyForceAndTorque forceAndTorque, dFloat gravityMag)
{
	m_body = body;
	NewtonBodySetForceAndTorqueCallback(m_body, forceAndTorque);
	m_vehicle = new dVehicleSingleBody(this);

	m_localFrame = localFrame;
	m_localFrame.m_posit = dVector(0.0f, 0.0f, 0.0f, 1.0f);
	dAssert(m_localFrame.TestOrthogonal());

	m_gravity = dVector (0.0f, -dAbs(gravityMag), 0.0f, 0.0f);

	// set linear and angular drag to zero
	dVector drag(0.0f);
	NewtonBodySetLinearDamping(m_body, 0.0f);
	NewtonBodySetAngularDamping(m_body, &drag[0]);

/*
	m_speed = 0.0f;
	m_sideSlip = 0.0f;
	m_prevSideSlip = 0.0f;
	m_finalized = false;
	m_gravityMag = dAbs(gravityMag);
	m_weightDistribution = 0.5f;
	m_aerodynamicsDownForce0 = 0.0f;
	m_aerodynamicsDownForce1 = 0.0f;
	m_aerodynamicsDownSpeedCutOff = 0.0f;
	m_aerodynamicsDownForceCoefficient = 0.0f;


	m_forceAndTorqueCallback = forceAndTorque;

	dCustomVehicleControllerManager* const manager = (dCustomVehicleControllerManager*)GetManager();
	NewtonWorld* const world = manager->GetWorld();


	// set the standard force and torque call back
	NewtonBodySetForceAndTorqueCallback(body, m_forceAndTorqueCallback);

	m_contactFilter = new dTireFrictionModel(this);


	m_collisionAggregate = NewtonCollisionAggregateCreate(world);
	NewtonCollisionAggregateSetSelfCollision(m_collisionAggregate, 0);
	NewtonCollisionAggregateAddBody(m_collisionAggregate, m_body);

	m_bodyList.Append(m_body);

	SetAerodynamicsDownforceCoefficient(0.5f, 0.4f, 1.0f);

#ifdef D_PLOT_ENGINE_CURVE 
	file_xxx = fopen("vehiceLog.csv", "wb");
	fprintf(file_xxx, "eng_rpm, eng_torque, eng_nominalTorque,\n");
#endif
*/

//	m_engine = NULL;
//	m_brakesControl = NULL;
//	m_engineControl = NULL;
//	m_steeringControl = NULL;
//	m_handBrakesControl = NULL;
}

void dVehicleChassis::Cleanup()
{
	if (m_brakeControl) {
		delete m_brakeControl;
	}

	if (m_handBrakeControl) {
		delete m_handBrakeControl;
	}

	if (m_steeringControl) {
		delete m_steeringControl;
	}

	if (m_vehicle) {
		delete m_vehicle;
	}
}

dVehicleBrakeControl* dVehicleChassis::GetBrakeControl()
{
	if (!m_brakeControl) {
		m_brakeControl = new dVehicleBrakeControl(this);
	}
	return m_brakeControl;
}

dVehicleBrakeControl* dVehicleChassis::GetHandBrakeControl()
{
	if (!m_handBrakeControl) {
		m_handBrakeControl = new dVehicleBrakeControl(this);
	}
	return m_handBrakeControl;
}

dVehicleSteeringControl* dVehicleChassis::GetSteeringControl ()
{
	if (!m_steeringControl) {
		m_steeringControl = new dVehicleSteeringControl(this);
	}
	return m_steeringControl;
}

dVehicleTireInterface* dVehicleChassis::AddTire (const dMatrix& locationInGlobalSpace, const dVehicleTireInterface::dTireInfo& tireInfo)
{
	return m_vehicle->AddTire(locationInGlobalSpace, tireInfo, m_localFrame);
}

dVehicleDifferentialInterface* dVehicleChassis::AddDifferential(dVehicleTireInterface* const leftTire, dVehicleTireInterface* const rightTire)
{
	return m_vehicle->AddDifferential(leftTire, rightTire);
}

dVehicleEngineInterface* dVehicleChassis::AddEngine(const dVehicleEngineInterface::dEngineInfo& engineInfo, dVehicleDifferentialInterface* const differential)
{
	return m_vehicle->AddEngine(engineInfo, differential);
}

void dVehicleChassis::Debug(dCustomJoint::dDebugDisplay* const debugContext) const
{
	m_vehicle->Debug(debugContext);
}

void dVehicleChassis::Finalize()
{
	dVector minP;
	dVector maxP;
	m_vehicle->CalculateNodeAABB(dGetIdentityMatrix(), minP, maxP);

	const dList<dVehicleNode*>& children = m_vehicle->GetChildren();
	for (dList<dVehicleNode*>::dListNode* tireNode = children.GetFirst(); tireNode; tireNode = tireNode->GetNext()) {
		dVehicleVirtualTire* const tire = (dVehicleVirtualTire*)tireNode->GetInfo()->GetAsTire();
		if (tire) {
			dVector tireMinP;
			dVector tireMaxP;

			dMatrix tireMatrix(tire->GetHardpointMatrix(0.0f));
			tire->CalculateNodeAABB(tireMatrix, tireMinP, tireMaxP);

			minP = minP.Min (tireMinP);
			maxP = maxP.Max (tireMaxP);
		}
	}

	m_obbOrigin = (maxP + minP).Scale (0.5f);
	m_obbSize = (maxP - minP).Scale (0.5f) + dVector (0.1f, 0.1f, 0.1f, 0.0f);

	m_vehicle->RigidBodyToStates();
	m_solver.Finalize(this);
}

void dVehicleChassis::ApplyExternalForces(dFloat timestep)
{
	m_vehicle->ApplyExternalForce();
	CalculateSuspensionForces(timestep);
	CalculateTireContacts(timestep);
}

int dVehicleChassis::OnAABBOverlap(const NewtonBody * const body, void* const context)
{
	dCollectCollidingBodies* const bodyList = (dCollectCollidingBodies*)context;
	if (body != bodyList->m_exclude) {
		dFloat mass;
		dFloat Ixx;
		dFloat Iyy;
		dFloat Izz;
		NewtonBodyGetMass(body, &mass, &Ixx, &Iyy, &Izz);
		bodyList->m_array[bodyList->m_count] = (NewtonBody*)body;
		if (mass == 0.0f) {
			for (int i = bodyList->m_count; i > 0; i --) {
				bodyList->m_array[i] = bodyList->m_array[i - 1];
			}
			bodyList->m_array[0] = (NewtonBody*)body;
			bodyList->m_staticCount++;
		}
		bodyList->m_count++;
		
		dAssert(bodyList->m_count < sizeof(bodyList->m_array) / sizeof(bodyList->m_array[1]));
	}
	return 1;
}

void dVehicleChassis::CalculateTireContacts(dFloat timestep)
{
	dComplementaritySolver::dBodyState* const chassisBody = m_vehicle->GetBody();
	const dMatrix& matrix = chassisBody->GetMatrix();
	dVector origin(matrix.TransformVector(m_obbOrigin));
	dVector size(matrix.m_front.Abs().Scale(m_obbSize.m_x) + matrix.m_up.Abs().Scale(m_obbSize.m_y) + matrix.m_right.Abs().Scale(m_obbSize.m_z));

	dVector p0 (origin - size);
	dVector p1 (origin + size);

	dCollectCollidingBodies bodyList(GetBody());
	NewtonWorld* const world = NewtonBodyGetWorld(GetBody());
	NewtonWorldForEachBodyInAABBDo(world, &p0.m_x, &p1.m_x, OnAABBOverlap, &bodyList);

	const dList<dVehicleNode*>& children = m_vehicle->GetChildren();
	for (dList<dVehicleNode*>::dListNode* tireNode = children.GetFirst(); tireNode; tireNode = tireNode->GetNext()) {
		dVehicleVirtualTire* const tire = (dVehicleVirtualTire*)tireNode->GetInfo()->GetAsTire();
		if (tire) {
			tire->CalculateContacts(bodyList, timestep);
		}
	}
}

int dVehicleChassis::GetKinematicLoops(dKinematicLoopJoint** const jointArray)
{
	return m_vehicle->GetKinematicLoops(jointArray);
}

void dVehicleChassis::PostUpdate(dFloat timestep, int threadIndex)
{
	m_vehicle->RigidBodyToStates();
}

void dVehicleChassis::PreUpdate(dFloat timestep, int threadIndex)
{
	dVehicleManager* const manager = (dVehicleManager*)GetManager();
	manager->UpdateDriverInput(this, timestep);

	if (m_steeringControl) {
		m_steeringControl->Update(timestep);
	}

	if (m_brakeControl) {
		m_brakeControl->Update(timestep);
	}

	if (m_handBrakeControl) {
		m_handBrakeControl->Update(timestep);
	}

	
	m_vehicle->RigidBodyToStates();
	m_solver.Update(timestep);
	m_vehicle->Integrate(timestep);
	m_vehicle->StatesToRigidBody(timestep);
}

void dVehicleChassis::ApplyDriverInputs(const dDriverInput& driveInputs, dFloat timestep)
{
	if (m_steeringControl) {
		m_steeringControl->SetParam(driveInputs.m_steeringValue);
	}

	if (m_brakeControl) {
		m_brakeControl->SetParam(driveInputs.m_brakePedal);
	}

	if (m_handBrakeControl) {
		m_handBrakeControl->SetParam(driveInputs.m_handBrakeValue);
	}

/*
	if (m_engineControl) {
		m_engineControl->SetDifferentialLock(driveInputs.m_lockDifferential ? true : false);

		switch (m_engineControl->m_drivingState) {
		case dEngineController::m_engineOff:
		{
			if (driveInputs.m_ignitionKey) {
				m_engineControl->SetIgnition(true);
				m_engineControl->SetGear(m_engineControl->GetNeutralGear());
				m_engineControl->m_drivingState = dEngineController::m_engineIdle;
				if (m_handBrakesControl) {
					m_handBrakesControl->SetParam(driveInputs.m_handBrakeValue);
				}
			}
			else {
				m_engineControl->SetIgnition(false);
				m_engineControl->SetGear(m_engineControl->GetFirstGear());
				if (m_handBrakesControl) {
					m_handBrakesControl->SetParam(1.0f - driveInputs.m_handBrakeValue);
				}
			}
			break;
		}

		case dEngineController::m_engineStop:
		{
			m_engineControl->SetParam(0.0f);
			m_engineControl->SetClutchParam(1.0f);
			if (dAbs(m_engineControl->GetSpeed()) < 4.0f) {
				m_engineControl->m_stopDelay = int(2.0f / timestep);
				m_engineControl->m_drivingState = dEngineController::m_engineStopDelay;
			}
			if (m_brakesControl) {
				m_engineControl->SetGear(m_engineControl->GetNeutralGear());
				m_brakesControl->SetParam(1.0f);
				m_brakesControl->SetParam(1.0f);
			}
			break;
		}

		case dEngineController::m_engineStopDelay:
		{
			m_engineControl->m_stopDelay--;
			if ((m_engineControl->m_stopDelay < 0) || driveInputs.m_ignitionKey) {
				m_engineControl->m_drivingState = dEngineController::m_engineIdle;
			}
			m_engineControl->SetGear(m_engineControl->GetNeutralGear());
			m_brakesControl->SetParam(1.0f);
			m_brakesControl->SetParam(1.0f);
			break;
		}

		case dEngineController::m_engineIdle:
		{
			if (!driveInputs.m_ignitionKey) {
				m_engineControl->m_drivingState = dEngineController::m_engineOff;
			}
			else {
				m_engineControl->SetGear(driveInputs.m_gear);
				m_engineControl->SetParam(driveInputs.m_throttle);

				if (m_engineControl->m_automaticTransmissionMode) {
					m_engineControl->SetClutchParam(0.0f);
				}
				else {
					m_engineControl->SetClutchParam(1.0f - driveInputs.m_clutchPedal);
				}
				if (m_handBrakesControl) {
					m_handBrakesControl->SetParam(driveInputs.m_handBrakeValue);
				}

				if (m_engineControl->GetGear() == m_engineControl->GetReverseGear()) {
					m_engineControl->m_drivingState = dEngineController::m_driveReverse;
				}
				else if (m_engineControl->GetGear() != m_engineControl->GetNeutralGear()) {
					m_engineControl->m_drivingState = dEngineController::m_driveForward;
				}
			}
			break;
		}

		case dEngineController::m_driveForward:
		{
			m_engineControl->SetParam(driveInputs.m_throttle);

			if ((driveInputs.m_brakePedal > 0.1f) && (m_engineControl->GetRPM() < 1.1f * m_engineControl->GetIdleRPM())) {
				m_engineControl->SetClutchParam(0.0f);
			}
			else {
				m_engineControl->SetClutchParam(driveInputs.m_clutchPedal);
			}
			if (m_handBrakesControl) {
				m_handBrakesControl->SetParam(driveInputs.m_handBrakeValue);
			}

			if (!m_engineControl->GetTransmissionMode()) {
				dAssert(0);
				//m_engineControl->SetGear(driveInputs.m_gear);
			}
			else {
				if (m_engineControl->GetSpeed() < 5.0f) {
					if (driveInputs.m_gear == m_engineControl->GetReverseGear()) {
						m_engineControl->SetGear(driveInputs.m_gear);
						if (m_brakesControl) {
							m_brakesControl->SetParam(1.0f);
							m_brakesControl->SetParam(1.0f);
						}
						m_engineControl->m_drivingState = dEngineController::m_engineIdle;
					}
					else if (driveInputs.m_gear == m_engineControl->GetNeutralGear()) {
						m_engineControl->SetGear(driveInputs.m_gear);
						m_engineControl->m_drivingState = dEngineController::m_engineIdle;
					}
				}
			}
			if (!driveInputs.m_ignitionKey) {
				m_engineControl->m_drivingState = dEngineController::m_engineStop;
			}
			break;
		}

		case dEngineController::m_driveReverse:
		{
			m_engineControl->SetParam(driveInputs.m_throttle);

			if ((driveInputs.m_brakePedal > 0.1f) && (m_engineControl->GetRPM() < 1.1f * m_engineControl->GetIdleRPM())) {
				m_engineControl->SetClutchParam(0.0f);
			}
			else {
				m_engineControl->SetClutchParam(driveInputs.m_clutchPedal);
			}
			if (m_handBrakesControl) {
				m_handBrakesControl->SetParam(driveInputs.m_handBrakeValue);
			}

			if (!m_engineControl->GetTransmissionMode()) {
				dAssert(0);
				//m_engineControl->SetGear(driveInputs.m_gear);
			}
			else {
				if (m_engineControl->GetSpeed() < 5.0f) {
					if (driveInputs.m_gear == m_engineControl->GetNeutralGear()) {
						m_engineControl->SetGear(driveInputs.m_gear);
						m_engineControl->m_drivingState = dEngineController::m_engineIdle;
					}
					else if (driveInputs.m_gear != m_engineControl->GetReverseGear()) {
						m_engineControl->SetGear(driveInputs.m_gear);
						if (m_brakesControl) {
							m_brakesControl->SetParam(1.0f);
							m_brakesControl->SetParam(1.0f);
						}
						m_engineControl->m_drivingState = dEngineController::m_engineIdle;
					}
				}
			}
			if (!driveInputs.m_ignitionKey) {
				m_engineControl->m_drivingState = dEngineController::m_engineStop;
			}
			break;
		}

		default:
			dAssert(0);
		}

	}
	else if (m_handBrakesControl) {
		m_handBrakesControl->SetParam(driveInputs.m_handBrakeValue);
	}
*/
}

void dVehicleChassis::CalculateSuspensionForces(dFloat timestep)
{
	const int maxSize = 64;
	dComplementaritySolver::dJacobianPair m_jt[maxSize];
	dComplementaritySolver::dJacobianPair m_jInvMass[maxSize];
	dVehicleVirtualTire* tires[maxSize];
	dFloat massMatrix[maxSize * maxSize];
	dFloat accel[maxSize];
	
	dComplementaritySolver::dBodyState* const chassisBody = m_vehicle->GetBody();

	const dMatrix& chassisMatrix = chassisBody->GetMatrix(); 
	const dMatrix& chassisInvInertia = chassisBody->GetInvInertia();
	dVector chassisOrigin (chassisMatrix.TransformVector (chassisBody->GetCOM()));
	dFloat chassisInvMass = chassisBody->GetInvMass();

	int tireCount = 0;
	const dList<dVehicleNode*>& children = m_vehicle->GetChildren();
	for (dList<dVehicleNode*>::dListNode* tireNode = children.GetFirst(); tireNode; tireNode = tireNode->GetNext()) {
		dVehicleVirtualTire* const tire = (dVehicleVirtualTire*) tireNode->GetInfo()->GetAsTire();
		if (tire) {
			const dVehicleVirtualTire::dTireInfo& info = tire->m_info;
			tires[tireCount] = tire;
			dFloat x = tire->m_position;
			dFloat v = tire->m_speed;
			dFloat weight = 1.0f;
/*
			switch (tire->m_suspentionType)
			{
				case m_offroad:
					weight = 0.9f;
					break;
				case m_confort:
					weight = 1.0f;
					break;
				case m_race:
					weight = 1.1f;
					break;
			}
*/
			dComplementaritySolver::dBodyState* const tireBody = tire->GetBody();

			const dFloat invMass = tireBody->GetInvMass();
			const dFloat kv = info.m_dampingRatio * invMass;
			const dFloat ks = info.m_springStiffness * invMass;
			accel[tireCount] = -NewtonCalculateSpringDamperAcceleration(timestep, ks * weight, x, kv, v);

			const dMatrix& tireMatrix = tireBody->GetMatrix(); 
			const dMatrix& tireInvInertia = tireBody->GetInvInertia();
			dFloat tireMass = tireBody->GetInvMass();

			m_jt[tireCount].m_jacobian_J01.m_linear = chassisMatrix.m_up.Scale(-1.0f);
			m_jt[tireCount].m_jacobian_J01.m_angular = dVector(0.0f);
			m_jt[tireCount].m_jacobian_J10.m_linear = chassisMatrix.m_up;
			m_jt[tireCount].m_jacobian_J10.m_angular = (tireMatrix.m_posit - chassisOrigin).CrossProduct(chassisMatrix.m_up);

			m_jInvMass[tireCount].m_jacobian_J01.m_linear = m_jt[tireCount].m_jacobian_J01.m_linear.Scale(tireMass);
			m_jInvMass[tireCount].m_jacobian_J01.m_angular = tireInvInertia.RotateVector(m_jt[tireCount].m_jacobian_J01.m_angular);
			m_jInvMass[tireCount].m_jacobian_J10.m_linear = m_jt[tireCount].m_jacobian_J10.m_linear.Scale(chassisInvMass);
			m_jInvMass[tireCount].m_jacobian_J10.m_angular = chassisInvInertia.RotateVector(m_jt[tireCount].m_jacobian_J10.m_angular);

			tireCount++;
		}
	}

	for (int i = 0; i < tireCount; i++) {
		dFloat* const row = &massMatrix[i * tireCount];

		dFloat aii = m_jInvMass[i].m_jacobian_J01.m_linear.DotProduct3(m_jt[i].m_jacobian_J01.m_linear) + m_jInvMass[i].m_jacobian_J01.m_angular.DotProduct3(m_jt[i].m_jacobian_J01.m_angular) +
					 m_jInvMass[i].m_jacobian_J10.m_linear.DotProduct3(m_jt[i].m_jacobian_J10.m_linear) + m_jInvMass[i].m_jacobian_J10.m_angular.DotProduct3(m_jt[i].m_jacobian_J10.m_angular);

		row[i] = aii * 1.0001f;
		for (int j = i + 1; j < tireCount; j++) {
			dFloat aij = m_jInvMass[i].m_jacobian_J10.m_linear.DotProduct3(m_jt[j].m_jacobian_J10.m_linear) + m_jInvMass[i].m_jacobian_J10.m_angular.DotProduct3(m_jt[j].m_jacobian_J10.m_angular);
			row[j] = aij;
			massMatrix[j * tireCount + i] = aij;
		}
	}

	dCholeskyFactorization(tireCount, massMatrix);
	dCholeskySolve(tireCount, tireCount, massMatrix, accel);

	dVector chassisForce(0.0f);
	dVector chassisTorque(0.0f);
	for (int i = 0; i < tireCount; i++) {
		dVehicleVirtualTire* const tire = tires[i];
		dComplementaritySolver::dBodyState* const tireBody = tire->GetBody();

		tires[i]->m_tireLoad = dMax(dFloat(1.0f), accel[i]);
		dVector tireForce(m_jt[i].m_jacobian_J01.m_linear.Scale(accel[i]));

		tireBody->SetForce(tireBody->GetForce() + tireForce);
		chassisForce += m_jt[i].m_jacobian_J10.m_linear.Scale(accel[i]);
		chassisTorque += m_jt[i].m_jacobian_J10.m_angular.Scale(accel[i]);
	}
	chassisBody->SetForce(chassisBody->GetForce() + chassisForce);
	chassisBody->SetTorque(chassisBody->GetTorque() + chassisTorque);
}
