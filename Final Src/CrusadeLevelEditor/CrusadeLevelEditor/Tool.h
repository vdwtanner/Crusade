#pragma once

#include "pch.h"
#include "PrimitiveBatch.h"
#include "VertexTypes.h"

using namespace DirectX;

class Tool{

public:
	virtual void DrawCursor(PrimitiveBatch<VertexPositionColor> *batch) = 0;
	virtual void OnMouseDown(WPARAM btnstate, float worldx, float worldy) = 0;
	virtual void OnMouseUp(WPARAM btnstate, float worldx, float worldy) = 0;
	virtual void OnMouseMove(WPARAM btnstate, float worldx, float worldy) = 0;
	virtual Matrix getWorld()=0;
	virtual inline void setTileType(UINT id){
		m_currTile = id;
	}
	virtual inline UINT getTileType(){
		return m_currTile;
	}
	virtual inline void setSelectedString(std::wstring str){
		m_selectedStr = str;
	}
	virtual inline std::wstring getSelectedString(){
		return m_selectedStr;
	}
	virtual inline float getX(){
		return m_worldx;
	}
	virtual inline float getY(){
		return m_worldy;
	}

protected:

	UINT m_currTile = 0;
	std::wstring m_selectedStr = L"";
	float m_worldx;
	float m_worldy;
};