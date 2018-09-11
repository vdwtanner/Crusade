#include "pch.h"
#include "Camera.h"
#include <algorithm>

Camera::Camera(bool orthographic, _In_opt_ Vector2 viewport = Vector2(1,1)) : m_roll(0), m_pitch(0), m_turn(0){
	this->orthographic = orthographic;
	this->m_viewport = viewport;
	this->m_origviewportx = viewport.x;
	m_followPlayer = true;
	m_moveto = false;
	m_zoomto = false;
}

Camera::~Camera(){

}

void Camera::Init(Vector3 pos, Vector3 target){
	SetPos(pos);
	LookAt(target);
	m_zoomLevel = 1.0;
	m_up = Vector3(0, 1, 0);
	m_followPlayer = true;
	m_moveto = false;
	boundingBox();
	Update();
}

void Camera::Init(){
	this->Init(Vector3(0, 0, -10), Vector3(0, 0, 0));
}

bool Camera::inCameraBounds(Vector3 pos){
	return (abs(pos.x+.5 - m_pos.x) * 2 < (1 + m_viewport.x) &&
			abs(pos.y+.5 - m_pos.y) * 2 < (1 + m_viewport.y));
}

void Camera::Move(Vector3 translation){
	m_pos += translation;
	m_target += translation;
}

void Camera::Move(float x, float y, float z){
	m_pos.x += x;
	m_pos.y += y;
	m_pos.z += z;

	m_target.x += x;
	m_target.y += y;
	m_target.z += z;
}

void Camera::SetPos(Vector3 pos){
	m_pos = pos;
}

void Camera::Roll(float radians){
	m_roll += radians;
}

void Camera::Pitch(float radians){
	m_pitch += radians;
}

void Camera::Turn(float radians){
	m_turn += radians;
}

void Camera::LookAt(Vector3 target){
	m_target = target;
}

//Zoom camera and update zoomLevel. Zoom Level is used to make movement speed make more sense. It only keeps track of zoom between .8 and 120, thoughit is possible to zoom to .5
void Camera::ZoomOrtho(float deltaPercent){
	m_zoomLevel = m_viewport.x / m_origviewportx;
	
	m_viewport.x += m_viewport.x*deltaPercent;
	m_viewport.x = std::max<float>(m_viewport.x, .5f);
	m_viewport.x = std::min<float>(m_viewport.x, 120.0f);
	m_viewport.y += m_viewport.y*deltaPercent;
	m_viewport.y = std::max<float>(m_viewport.y, .5f*(1 / m_aspectRatio));
	m_viewport.y = std::min<float>(m_viewport.y, 120.0f*(1/m_aspectRatio));
}

void Camera::SetZoomLevel(float zoomlevel){
	m_zoomLevel = zoomlevel;
	m_viewport.x = m_zoomLevel * m_origviewportx;
	m_viewport.y = m_viewport.x * ((float)m_screeny / (float)m_screenx);
}

void Camera::ZoomTo(float level, float time){
	m_zoomto = true;
	m_zoomtoVal = level;
	m_zoomtoTime = time;
	m_zoomTimer = 0.0f;
}

void Camera::SetViewportOrtho(float x, float y){
	m_viewport.x = x;
	m_viewport.y = y;
}

void Camera::Resize(float screenX, float screenY){
	m_screenx = (int)screenX;
	m_screeny = (int)screenY;
	if (orthographic)
		m_Proj = Matrix::CreateOrthographic(m_viewport.x, m_viewport.y, .1f, 1000.0f);
	else
		m_Proj = Matrix::CreatePerspectiveFieldOfView((float)M_PI / 4, screenY / screenX, .1f, 1000.0f);
	
	SetZoomLevel(1.0f);

	boundingBox();
}

void Camera::Update(){
	Matrix m = Matrix::CreateFromYawPitchRoll(m_turn, m_pitch, m_roll);
	m_up = Vector3::TransformNormal(m_up, m);
	
	m_View = Matrix::CreateLookAt(m_pos, m_target, m_up);
	if(orthographic)
		m_Proj = Matrix::CreateOrthographic(m_viewport.x, m_viewport.y, .1f, 1000.0f);
	m_aspectRatio = m_viewport.x / m_viewport.y;
	
}

Matrix Camera::getViewProj(){
	return m_View * m_Proj;
}

Matrix Camera::getView(){
	return m_View;
}

Matrix Camera::getProj(){
	return m_Proj;
}

float Camera::getViewportWidth(){
	return m_viewport.x;
}

float Camera::getViewportHeight(){
	return m_viewport.y;
}

float Camera::getX(){
	return m_pos.x;
}

float Camera::getY(){
	return m_pos.y;
}

//Returns the position of the Camera as a Vector2 (x, y)
Vector2 Camera::getPosition(){
	return Vector2(m_pos);
}

//Calculate where the mouse is in relation to the world based off of screen coordinates
Vector2 Camera::ScreenToWorldPoint(Vector2 position){
	Vector2 worldPoint = Vector2(((position.x / m_screenx) * m_viewport.x) - (m_viewport.x / 2), ((m_viewport.y / 2) - ((position.y / m_screeny) * m_viewport.y)));
	worldPoint.x += m_pos.x;
	worldPoint.y += m_pos.y;
	return worldPoint;
}

Vector2 Camera::WorldToScreenPoint(Vector2 position){
	if (inCameraBounds(Vector3(position.x, position.y, 0)))
	{
		position.x -= (m_pos.x - m_viewport.x / 2);
		position.y -= (m_pos.y - m_viewport.y / 2);
		Vector2 worldPoint = Vector2(((position.x * m_screenx) / m_viewport.x), ((m_screeny) - ((position.y * m_screeny) / m_viewport.y)));
		return worldPoint;
	}
	else
	{
		return Vector2((float)m_screenx, (float)m_screeny);
	}
}

float Camera::getAspectRatio(){
	return m_aspectRatio;
}

float Camera::getZoomLevel(){
	return m_zoomLevel;
}

void Camera::rePosition(Vector2 pos, float dt){
	if (m_followPlayer){
		//setting variables for easy math
		float tempx = pos.x - m_pos.x;
		float tempy = pos.y - m_pos.y;

		float mxworldx = m_worldWidth - m_viewport.x / 2.0f;
		float mxworldy = m_worldHeight - m_viewport.y / 2.0f;
		float mnworldx = m_viewport.x / 2.0f;
		float mnworldy = m_viewport.y / 2.0f;
		//bounding box code
		if (tempx > m_boundx){
			Move(tempx - m_boundx, 0, 0);
		}
		if (tempx < -m_boundx){
			Move(tempx + m_boundx, 0, 0);
		}
		if (tempy < -m_boundy){
			Move(0, tempy + m_boundy, 0);
		}
		if (tempy > m_boundy){
			Move(0, tempy - m_boundy, 0);
		}
		//code to keep camera in world
		if (m_pos.x > mxworldx){
			Move(mxworldx - m_pos.x, 0, 0);
		}
		if (m_pos.x < mnworldx){
			Move(mnworldx - m_pos.x, 0, 0);
		}
		if (m_pos.y < mnworldy){
			Move(0, mnworldy - m_pos.y, 0);
		}
		if (m_pos.y > mxworldy){
			Move(0, mxworldy - m_pos.y, 0);
		}
	}
	else if (m_moveto && m_movetoTime != 0)
	{
		float xdisPerSec = (m_movetoPos.x - m_pos.x) / m_movetoTime;
		float ydisPerSec = (m_movetoPos.y - m_pos.y) / m_movetoTime;

		Move(xdisPerSec * dt, ydisPerSec * dt, 0);

		m_movetoTime -= dt;
		if (m_movetoTime <= 0){
			m_moveto = false;
		}

		float mxworldx = m_worldWidth - m_viewport.x / 2.0f;
		float mxworldy = m_worldHeight - m_viewport.y / 2.0f;
		float mnworldx = m_viewport.x / 2.0f;
		float mnworldy = m_viewport.y / 2.0f;

		if (m_pos.x > mxworldx){
			Move(mxworldx - m_pos.x, 0, 0);
		}
		if (m_pos.x < mnworldx){
			Move(mnworldx - m_pos.x, 0, 0);
		}
		if (m_pos.y < mnworldy){
			Move(0, mnworldy - m_pos.y, 0);
		}
		if (m_pos.y > mxworldy){
			Move(0, mxworldy - m_pos.y, 0);
		}
	}
	if (m_zoomto)
	{
		float zoomPerSec = (m_zoomtoVal - m_zoomLevel) / m_zoomtoTime;

		SetZoomLevel(getZoomLevel() + (zoomPerSec * dt));

		m_zoomtoTime -= dt;
		if (m_zoomtoTime <= 0){
			m_zoomto = false;
		}
	
	}
}

void Camera::boundingBox(){
	Vector2 bound;
	bound = ScreenToWorldPoint(Vector2(0.75f * m_screenx, 0.35f * m_screeny));
	bound.x -= m_pos.x;
	bound.y -= m_pos.y;

	m_boundx = bound.x;
	m_boundy = bound.y;
}

void Camera::setWorld(float height, float width){
	m_worldHeight = height;
	m_worldWidth = width;
}


void Camera::EnableFollowPlayer(){
	m_followPlayer = true;
	m_moveto = false;
	m_zoomto = false;
	SetZoomLevel(1.0f);
}

void Camera::DisableFollowPlayer(){
	m_followPlayer = false;
}

void Camera::moveTo(float x, float y, float z, float time){
	DisableFollowPlayer();
	m_moveto = true;
	m_movetoPos = Vector3(x, y, z);
	m_movetoTime = time;
}