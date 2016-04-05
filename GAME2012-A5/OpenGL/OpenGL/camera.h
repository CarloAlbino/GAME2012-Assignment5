#pragma once

#include "graphicsmath.h"

class Camera
{
public:
	Camera(int WindowWidth, int WindowHeight);
	Camera(int WindowWidth, int WindowHeight,
		const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up);

	void OnRender();

	const Vector3f& GetPos() { return m_pos; }
	const Vector3f& GetTarget() { return m_target; }
	const Vector3f& GetUp() { return m_up; }

private:
	void Init();
	void Update();

private:
	Vector3f m_pos;
	Vector3f m_target;
	Vector3f m_up;

	float m_AngleH;
	float m_AngleV;

	int m_windowWidth;
	int m_windowHeight;

	bool m_onUpperEdge;
	bool m_onLowerEdge;
	bool m_onLeftEdge;
	bool m_onRightEdge;
};