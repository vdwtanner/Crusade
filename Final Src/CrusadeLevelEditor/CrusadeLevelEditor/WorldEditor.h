#pragma once

#include "pch.h"
#include "Grid.h"
#include "Game.h"
#include "PrimitiveBatch.h"
#include "VertexTypes.h"
#include "Effects.h"
#include "Camera.h"
#include "SimpleMath.h"
#include "Grid.h"
#include "TileSet.h"
#include "DrawTool.h"
#include "BoxTool.h"
#include "EnemyTool.h"
#include "PlayerSpawnTool.h"
#include "WorldTileMatrix.h"
#include "SpriteFont.h"
#include <sstream>
#include "Box2DSingleton.h"
#include "Character.h"
#include "LuaBindings.h"
#include "LuaTriggerTool.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Grid;
class DrawTool;
class TileSet;
class BoxTool;
class Tool;
class WorldTileMatrix;

class WorldEditor{

public:
	WorldEditor(ID3D11Device *device, ID3D11DeviceContext *context);
	~WorldEditor();

	void Render();
	void Update(float dt);
	Grid* getGrid();
	void toggleGrid();
	
	TileSet *getTileSet();
	
	Tool *getTool();
	void setTool(int);

	WorldTileMatrix* getWorldTileMatrix();
	
	void CreateNewWorld(int width, int height, int layers, int active);
	void CreateNewWorld(LPWSTR path);
	void placeTile(UINT id, Vector2 pos);
	void placeTile(UINT id, float x, float y);

	void incCurrLayer();
	void decCurrLayer();

	bool isGridVis();

	void setCharacterPosition(Vector2 position);
	Character* getCharacter();
	Vector2 getCharacterPosition();

	void setDeviceAndContextHandle(ID3D11Device *device, ID3D11DeviceContext *context);

private:
	void BuildRenderEffects();
	
	ID3D11DeviceContext							*h_context;
	ID3D11Device								*h_device;

	std::unique_ptr<LuaBindings>				m_bindings;

	std::unique_ptr<WorldTileMatrix>			m_worldTileMatrix;

	std::unique_ptr<TileSet>					m_tileSet;

	std::unique_ptr<Tool>						m_tool;

	std::unique_ptr<Grid>						m_grid;
	std::unique_ptr<PrimitiveBatch<VertexPositionColor>>			m_gridBatch;
	std::unique_ptr<PrimitiveBatch<VertexPositionColorTexture>>		m_textureBatch;

	std::unique_ptr<BasicEffect>				m_effect;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;

	std::unique_ptr<BasicEffect>				m_textureEffect;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_textureInputLayout;

	std::unique_ptr<DirectX::SpriteFont>		m_font;
	DirectX::SimpleMath::Vector2				m_fontPos;
	std::unique_ptr<DirectX::SpriteBatch>		m_spriteBatch;

	std::wstringstream							m_stream;

	std::unique_ptr<Character>					m_character;

	bool										m_gridvis;
	int											m_currentLayer;
};
