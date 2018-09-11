#pragma once
#define _USE_MATH_DEFINES

#include "pch.h"
#include "SimpleMath.h"
#include <math.h>


using namespace DirectX::SimpleMath;

class Camera{
private:
	Matrix		m_View;
	Matrix		m_Proj;

	Vector3		m_pos;
	Vector3		m_target;
	Vector3		m_up;
	Vector2		m_viewport;

	float m_origviewportx;

	int m_screenx;
	int m_screeny;
	float m_aspectRatio;
	float m_zoomLevel;

	float m_boundx;
	float m_boundy;
	float m_worldHeight;
	float m_worldWidth;

	float m_roll;
	float m_pitch;
	float m_turn;

	bool		m_moveto;
	Vector3		m_movetoPos;
	float		m_movetoTime;

	bool		m_zoomto;
	float		m_zoomtoVal;
	float		m_zoomtoTime;
	float		m_zoomTimer;

	bool		orthographic;
	bool		m_followPlayer;

public:
	Camera(bool, _In_opt_ Vector2 viewport);
	~Camera();
	void Init();
	void Init(Vector3 pos, Vector3 target);

	bool inCameraBounds(Vector3 pos);

	//bool inCullingBounds(Vector3 )
	void Move(Vector3 translation);
	void Move(float x, float y, float z);
	void SetPos(Vector3 pos);
	void Roll(float radians);
	void Pitch(float radians);
	void Turn(float radians);

	void ZoomOrtho(float deltaPercent);
	void SetZoomLevel(float zoomlevel);
	void ZoomTo(float level, float time);

	void SetViewportOrtho(float x, float y);

	void LookAt(Vector3 target);
	void Resize(float screenX, float screenY);
	void Update();

	void EnableFollowPlayer();
	void DisableFollowPlayer();

	void moveTo(float x, float y, float z, float time);

	float getViewportWidth();
	float getViewportHeight();
	float getX();
	float getY();
	float getAspectRatio();
	float getZoomLevel();

	Matrix getViewProj();
	Matrix getView();
	Matrix getProj();

	Vector2 ScreenToWorldPoint(Vector2);
	Vector2 WorldToScreenPoint(Vector2);
	Vector2 getPosition();
	void rePosition(Vector2 pos, float dt);
	void boundingBox();
	void setWorld(float height, float width);
};