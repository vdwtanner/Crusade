#pragma once

#include "pch.h"
#include "WorldTile.h"
#include "VertexTypes.h"
#include "TileSet.h"
#include "Enemy.h"
#include <map>
#include <set>
#include <fstream>
#include <SpriteBatch.h>
#include "EnemyManager.h"
#include "Character.h"

class WorldTile;

class WorldTileMatrix{
public:
	enum LayerViewState{
		VIEW_ALL,
		VIEW_SINGLE,
		VIEW_STACKED,
		VIEW_TRANS
	};

	WorldTileMatrix(ID3D11Device *device, ID3D11DeviceContext *context, int width, int height, int layers, int activeLayer);
	WorldTileMatrix(ID3D11Device *device, ID3D11DeviceContext *context, LPWSTR filepath, TileSet* tileSet);
	~WorldTileMatrix();

	static WorldTileMatrix* Instance();
	
	void BuildEmptyWorld();
	
	void RenderLayers(Matrix viewProj, int start, int size);
	void RenderBackground();

	WorldTile* getTileIdAt(int x, int y, int layer);
	UINT getTextureIndex(UINT id);
	bool placeTile(UINT id, int x, int y, int layer);
	UINT removeTile(int x, int y, int layer);

	void setTileSet(TileSet *ts);

	void updateStagingTileVerts();
	void updateStagingTile(int x, int y, int layer);

	void damageTilesInFront(int x, int y, int layer, int num);
	void damageTile(int x, int y, int layer, int num);

	UINT totalSize();
	UINT getWidth();
	UINT getHeight();
	UINT getLayers();

	UINT getActiveLayer();

	void setCurrLayer(int currLayer);

	void setDeviceAndContextHandle(ID3D11Device *device, ID3D11DeviceContext *context);

	void setLayerViewState(LayerViewState state);
	LayerViewState getLayerViewState();

	bool saveWorld(LPWSTR filepath);
	bool loadWorld(LPWSTR filepath);
	
	void setBackground(LPWSTR filepath, RECT rc);
	void setBackgroundRect(RECT rc);
	void loadBackgroundFromPath();

	void setMusicPath(LPWSTR filepath);
	wstring getMusicPath();

	void update(float dt);

	WorldTile *m_worldTileArr;

	void setPlayerSpawn(Vector2 spawnPoint);
	Vector2 getPlayerSpawn();

private:
	void vectorRemoveEle(std::vector<WorldTile*> *vect, WorldTile *ele);
	void copyStageTileVertsToVB();
	static WorldTileMatrix* m_instance;
	ID3D11Device		*h_device;
	ID3D11DeviceContext *h_context;

	TileSet				*h_tileSet;

	// first is id, second is count in arr
	//std::map<UINT, UINT>		m_tileTypeCount;
	//UINT	*m_worldTileArr;

	// deprecated, replace with entity manager later
	std::vector<GameObject> m_entityArr;

	Vector2 m_playerSpawn;

	std::wstring m_backgroundPath;
	wstring m_musicPath;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_background;

	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_worldVB;
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_stagingVB;

	std::unique_ptr<SpriteBatch> m_spriteBatch;

	LayerViewState m_currViewState;

	int m_currentLayer;
	int m_activeLayer;

	bool m_copyStageBuff;

	int m_width;
	int m_height;
	int m_layers;
	RECT m_rect;
	
};