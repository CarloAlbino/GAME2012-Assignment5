#pragma once

#include "graphicsmath.h"
#include "camera.h"

class Transform
{
public:
	Transform()
	{
		m_scale = Vector3f(1.0f);
		m_position = Vector3f();
		m_rotation = Vector3f();
	}

	void Scale(float x, float y, float z)
	{
		m_scale.x = x;
		m_scale.y = y;
		m_scale.z = z;
	}

	void Position(float x, float y, float z)
	{
		m_position.x = x;
		m_position.y = y;
		m_position.z = z;
	}

	void Rotation(float x, float y, float z)
	{
		m_rotation.x = x;
		m_rotation.y = y;
		m_rotation.z = z;
	}

	const Matrix4f* GetProjectionTransform()
	{
		m_projectionTransformation.InitPerspectiveProjection(m_perspective);
		return &m_projectionTransformation;
	}

	const Matrix4f* GetViewTransform()
	{
		Matrix4f CameraTranslation;
		Matrix4f CameraRotation;

		CameraTranslation.InitTranslationTransform(-m_camera.Pos.x, -m_camera.Pos.y, -m_camera.Pos.z);
		CameraRotation.InitCameraTransform(m_camera.Target, m_camera.Up);

		m_viewTransformation = CameraRotation * CameraTranslation;

		return &m_viewTransformation;
	}

	const Matrix4f* GetViewProjectionTransform()
	{
		GetViewTransform();
		GetProjectionTransform();

		m_viewProjectionTransformation = m_projectionTransformation * m_viewTransformation;

		return &m_viewProjectionTransformation;
	}

	const Matrix4f* GetWorldTransform()
	{
		Matrix4f ScaleTransform;
		Matrix4f RotationTransform;
		Matrix4f PositionTransform;

		ScaleTransform.InitScaleTransform(m_scale.x, m_scale.y, m_scale.z);
		RotationTransform.InitRotateTransform(m_rotation.x, m_rotation.y, m_rotation.z);
		PositionTransform.InitTranslationTransform(m_position.x, m_position.y, m_position.z);

		m_worldTransformation = PositionTransform * RotationTransform * ScaleTransform;

		return &m_worldTransformation;
	}

	const Matrix4f* GetWorldPerspectiveTransform()
	{
		Matrix4f PerspectiveTransform;
		PerspectiveTransform.InitPerspectiveProjection(m_perspective);

		GetWorldTransform();

		m_worldPerspectiveTransformation = PerspectiveTransform * m_worldTransformation;
		return &m_worldPerspectiveTransformation;
	}

	const Matrix4f* GetWorldViewProjectionTransform()
	{
		GetWorldTransform();
		GetViewProjectionTransform();

		m_worldViewProjectionTransformation = m_viewProjectionTransformation * m_worldTransformation;

		return &m_worldViewProjectionTransformation;
	}

	void SetPerspective(float FOV, float Width, float Height, float zNear, float zFar)
	{
		m_perspective.FOV = FOV;
		m_perspective.Width = Width;
		m_perspective.Height = Height;
		m_perspective.zNear = zNear;
		m_perspective.zFar = zFar;
	}

	void SetCamera(const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up)
	{
		m_camera.Pos = Pos;
		m_camera.Target = Target;
		m_camera.Up = Up;
	}

	void SetCamera(Camera& camera)
	{
		SetCamera(camera.GetPos(), camera.GetTarget(), camera.GetUp());
	}

private:
	Vector3f m_scale;
	Vector3f m_position;
	Vector3f m_rotation;

	Matrix4f m_worldTransformation;
	Matrix4f m_worldPerspectiveTransformation;
	Matrix4f m_projectionTransformation;
	Matrix4f m_viewTransformation;
	Matrix4f m_viewProjectionTransformation;
	Matrix4f m_worldViewProjectionTransformation;
	Perspective m_perspective;

	struct {
		Vector3f Pos;
		Vector3f Target;
		Vector3f Up;
	} m_camera;
};