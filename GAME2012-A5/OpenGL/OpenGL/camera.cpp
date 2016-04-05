#include "camera.h"

#define STEP_SCALE 1.0f
#define EDGE_STEP 0.5f
#define MARGIN = 10

Camera::Camera(int WindowWidth, int WindowHeight)
{
	m_windowWidth = WindowWidth;
	m_windowHeight = WindowHeight;

	m_pos = Vector3f(0.f);
	m_target = Vector3f(0.f, 0.f, 1.f);
	m_target.Normalize();
	m_up = Vector3f(0.f, 1.f, 0.f);

	Init();
}

Camera::Camera(int WindowWidth, int WindowHeight,
	const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up)
{
	m_windowWidth = WindowWidth;
	m_windowHeight = WindowHeight;

	m_pos = Pos;
	m_target = Target;
	m_target.Normalize();

	m_up = Up;
	m_up.Normalize();

	Init();
}

void Camera::Init()
{
	Vector3f HTarget(m_target.x, 0.f, m_target.z);
	HTarget.Normalize();

	if (HTarget.z >= 0.f)
	{
		if (HTarget.x >= 0.f)
		{
			m_AngleH = 360.f - ToDegree(asin(HTarget.z));
		}
		else
		{
			m_AngleH = 180.f + ToDegree(asin(HTarget.z));
		}
	}
	else
	{
		if (HTarget.x >= 0.f)
		{
			m_AngleH = ToDegree(asin(-HTarget.z));
		}
		else
		{
			m_AngleH = 90.f + ToDegree(asin(-HTarget.z));
		}
	}

	m_AngleV = -ToDegree(asin(m_target.y));

	m_onUpperEdge = false;
	m_onLowerEdge = false;
	m_onRightEdge = false;
	m_onLeftEdge = false;
}

void Camera::OnRender()
{
	bool ShouldUpdate = false;

	if (m_onLeftEdge)
	{
		m_AngleH -= EDGE_STEP;
		ShouldUpdate = true;
	}
	else if (m_onRightEdge)
	{
		m_AngleH += EDGE_STEP;
		ShouldUpdate = true;
	}

	if (m_onUpperEdge)
	{
		if (m_AngleV > 90.f)
		{
			m_AngleV -= EDGE_STEP;
			ShouldUpdate = true;
		}
	}
	else if (m_onLowerEdge)
	{
		if (m_AngleV < 90.f)
		{
			m_AngleV += EDGE_STEP;
			ShouldUpdate = true;
		}
	}

	if (ShouldUpdate)
	{
		Update();
	}
}

void Camera::Update()
{
	/*const Vector3f Vaxis(0.f, 1.f, 0.f);

	Vector3f View(1.f, 0.f, 0.f);
	View.Rotate(m_AngleH, Vaxis);
	View.Normalize();

	Vector3f Haxis = Vaxis.Cross(View);
	Haxis.Normalize();
	View.Rotate(m_AngleV, Haxis);

	m_target = View;
	m_target.Normalize();

	m_up = m_target.Cross(Haxis);
	m_up.Normalize();*/
}