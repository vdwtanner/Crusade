#include "pch.h"
#include "WorldTileMatrix.h"
#include "TriggerManager.h"

WorldTileMatrix* WorldTileMatrix::m_instance = nullptr;

WorldTileMatrix::WorldTileMatrix(ID3D11Device *device, ID3D11DeviceContext *context, int width, int height, int layers, int activeLayer)
	: h_tileSet(0), m_currentLayer(0)
{
	h_context = context;
	h_device = device;
	m_width = width;
	m_height = height;
	m_layers = layers;
	m_musicPath = L"";
	m_activeLayer = activeLayer;
	//m_worldTileArr = new UINT[width * height * layers]();
	m_worldTileArr = new WorldTile[width * height * layers]();
	BuildEmptyWorld();
	m_currViewState = LayerViewState::VIEW_TRANS;
	m_spriteBatch.reset(new SpriteBatch(context));
	Box2DSingleton::Instance()->setWorldSize(Vector2((float)width, (float)height));
	m_instance = this;
	m_copyStageBuff = false;
}

WorldTileMatrix::WorldTileMatrix(ID3D11Device *device, ID3D11DeviceContext *context, LPWSTR filepath, TileSet* tileSet)
	: m_currentLayer(0)
{
	h_context = context;
	h_device = device;
	h_tileSet = tileSet;
	m_musicPath = L"";
	m_background.Reset();

	loadWorld(filepath);
	m_currViewState = LayerViewState::VIEW_TRANS;
	m_spriteBatch.reset(new SpriteBatch(context));
	m_instance = this;
	m_copyStageBuff = false;
}

WorldTileMatrix::~WorldTileMatrix(){
	m_instance = nullptr;
	delete[] m_worldTileArr;
	m_worldVB.Reset();
	m_stagingVB.Reset();
	m_background.Reset();
	m_spriteBatch.reset(nullptr);
}

/*Allows other classes to get a handle to the world's WorldTileMatrix. Returns a nullptr if the constructor hasn't been called yet by an appropriate function.*/
WorldTileMatrix* WorldTileMatrix::Instance(){
	return m_instance;
}

void WorldTileMatrix::RenderBackground(){
	if (m_background.Get()){
		m_spriteBatch->Begin();
		m_spriteBatch->Draw(m_background.Get(), m_rect);
		m_spriteBatch->End();
	}
}

void WorldTileMatrix::RenderLayers(Matrix viewProj, int start, int size){
	UINT stride = sizeof(CrusadeVertexTypes::TexturedVertex);
	UINT offset = 0;

	if (m_copyStageBuff){
		copyStageTileVertsToVB();
		m_copyStageBuff = false;
	}

	float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	h_context->OMSetBlendState(PipelineStates::BasicAlpha, blendFactors, 0xffffffff);
	
	h_context->IASetInputLayout(InputLayouts::TexturedVertexLayout);
	h_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	

	CrusadeEffects::TextureFX->SetWorldViewProj(viewProj); 
	CrusadeEffects::TextureFX->SetWorld(Matrix::Identity);

	int l = (m_currentLayer > (start - (size-1))) ? m_currentLayer : (start - (size-1));

	switch (m_currViewState){
	case LayerViewState::VIEW_STACKED:
		for (int i = (start); i >= l; i--){
			h_context->IASetVertexBuffers(0, 1, m_worldVB.GetAddressOf(), &stride, &offset);
			CrusadeEffects::TextureFX->SetDiffuseMaps(h_tileSet->getTextureArr());
			CrusadeEffects::TextureFX->SetColorCorrect(Vector4(1, 1, 1, 1.0f));
			CrusadeEffects::TextureFX->m_SpriteTech->GetPassByIndex(0)->Apply(0, h_context);
			h_context->Draw((m_width * m_height), (m_width * m_height * i));
		}
		break;
	case LayerViewState::VIEW_ALL:
		for (int i = (start); i >= (start-(size-1)); i--){
			h_context->IASetVertexBuffers(0, 1, m_worldVB.GetAddressOf(), &stride, &offset);
			CrusadeEffects::TextureFX->SetDiffuseMaps(h_tileSet->getTextureArr());
			CrusadeEffects::TextureFX->SetColorCorrect(Vector4(1, 1, 1, 1.0f));
			CrusadeEffects::TextureFX->m_SpriteTech->GetPassByIndex(0)->Apply(0, h_context);
			h_context->Draw((m_width * m_height), (m_width * m_height * i));
		}
		break;
	case LayerViewState::VIEW_SINGLE:
		if (m_currentLayer <= start && m_currentLayer >= (start - (size-1))){
			h_context->IASetVertexBuffers(0, 1, m_worldVB.GetAddressOf(), &stride, &offset);
			CrusadeEffects::TextureFX->SetDiffuseMaps(h_tileSet->getTextureArr());
			CrusadeEffects::TextureFX->SetColorCorrect(Vector4(1, 1, 1, 1.0f));
			CrusadeEffects::TextureFX->m_SpriteTech->GetPassByIndex(0)->Apply(0, h_context);
			h_context->Draw((m_width * m_height), (m_width * m_height * m_currentLayer));
		}
		break;
	case LayerViewState::VIEW_TRANS:
		for (int i = (start); i >= (start-(size-1)); i--){
			CrusadeEffects::TextureFX->SetColorCorrect(Vector4(1, 1, 1, 1.0f - (.4f * abs(m_currentLayer - i))));
			h_context->IASetVertexBuffers(0, 1, m_worldVB.GetAddressOf(), &stride, &offset);
			CrusadeEffects::TextureFX->SetDiffuseMaps(h_tileSet->getTextureArr());
			CrusadeEffects::TextureFX->m_SpriteTech->GetPassByIndex(0)->Apply(0, h_context);
			h_context->Draw((m_width * m_height), (m_width * m_height * i));

		}
		break;
	}

	CrusadeEffects::TextureFX->SetColorCorrect(Vector4(1, 1, 1, 1));



	h_context->GSSetShader(nullptr, nullptr, 0);
}

void WorldTileMatrix::update(float dt){

}

// builds an empty world with all tiles id = 0
void WorldTileMatrix::BuildEmptyWorld(){
	std::vector<CrusadeVertexTypes::TexturedVertex> tempVect;
	for (int k = 0; k < m_layers; k++){
		for (int j = 0; j < m_height; j++){
			for (int i = 0; i < m_width; i++){
				//m_worldTileArr[i + j*m_width + k*m_width*m_height] = 0;   //This is already covered by the default constructor of WorldTile()
				// add the vertices to our vector
				m_worldTileArr[i + j*m_width + k*m_width*m_height].setHp(-1);
				m_worldTileArr[i + j*m_width + k*m_width*m_height].setHpMax(-1);
				m_worldTileArr[i + j*m_width + k*m_width*m_height].setPosition(Vector2((float)i, (float)j));
				m_worldTileArr[i + j*m_width + k*m_width*m_height].setLayer(k);

				if (k == m_activeLayer){
					b2BodyDef def;
					def.position.Set((float)i + .5f, (float)j + .5f);
					m_worldTileArr[i + j*m_width + k*m_width*m_height].initRigidbody(def);
					m_worldTileArr[i + j*m_width + k*m_width*m_height].setSideEnableCollisionWith(WorldTile::ALL, Box2DSingleton::TRIGGER);
				}
				CrusadeVertexTypes::TexturedVertex temp = {};
				temp.Pos = Vector3((float)i, (float)j, -.5f + (-.1f * k));
				temp.Size = Vector2(1, 1);
				temp.Rotation = 0;
				temp.TexIndex = 0;
				temp.Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
				tempVect.push_back(temp);
			}
		}
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(CrusadeVertexTypes::TexturedVertex) * (m_width*m_height*m_layers);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &tempVect[0];
	DX::ThrowIfFailed(h_device->CreateBuffer(&vbd, &vinitData, m_worldVB.ReleaseAndGetAddressOf()));

	// We need to modify the world periodically whenever things blow up..
	// But just modifying our vertex buffer directly is somewhat slow
	// So instead lets have a "Double buffering" solution
	// We add verts to this staging buffer which is on the GPU,
	// Then override the real buffer with the staging buffer.
	// (It copys on the GPU, which is reeeeally fast)
	D3D11_BUFFER_DESC sbd;
	sbd.Usage = D3D11_USAGE_STAGING;
	sbd.ByteWidth = sizeof(CrusadeVertexTypes::TexturedVertex) * (m_width*m_height*m_layers);
	sbd.BindFlags = 0;
	sbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	sbd.StructureByteStride = sizeof(CrusadeVertexTypes::TexturedVertex);
	sbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	
	DX::ThrowIfFailed(h_device->CreateBuffer(&sbd, 0, m_stagingVB.ReleaseAndGetAddressOf()));

	h_context->CopyResource(m_stagingVB.Get(), m_worldVB.Get());
	m_musicPath = L"";
}

bool WorldTileMatrix::saveWorld(LPWSTR filepath){
	std::ofstream file(filepath, std::ios::out | std::ios::binary);
	if (!file)
		return false;
	file.write((char *)&m_width, 4);	// first we write the width, height, and layers to the file
	file.write((char *)&m_height, 4);
	file.write((char *)&m_layers, 4);
	file.write((char *)&m_activeLayer, 4);

	UINT backgroundSize = m_backgroundPath.size();
	UINT musicSize = m_musicPath.size();
	//UINT enemySize = m_enemyArr.size();
	UINT enemySize = EnemyManager::Instance()->getRenderSetSize();
	UINT entitySize = TriggerManager::Instance()->getActiveTriggers()->size();

	file.write((char *)&backgroundSize, 4);
	file.write((char *)m_backgroundPath.c_str(), backgroundSize * sizeof(wchar_t));
	file.write((char *)&musicSize, 4);
	file.write((char *)m_musicPath.c_str(), musicSize * sizeof(wchar_t));
	file.write((char *)&enemySize, 4);
	file.write((char *)&entitySize, 4);

	file.write((char *)&m_playerSpawn.x, 4);
	file.write((char *)&m_playerSpawn.y, 4);

	//file.write((char *)m_worldTileArr, (m_width * m_height * m_layers) * 4);
	for (int index = 0; index < (m_width*m_height*m_layers); index++){
		int id = m_worldTileArr[index].getId();
		file.write((char *)&id, 4);
	}

	// save enemies
	std::multiset<Enemy*, EnemyManager::ptr_less> *renderSet = EnemyManager::Instance()->getRenderSet();
	std::multiset<Enemy*, EnemyManager::ptr_less>::iterator it;
	for (it = renderSet->begin(); it != renderSet->end(); it++){
		UINT eID = ((Enemy*)*it)->getId();
		Vector2 pos = ((Enemy*)*it)->getPosition();
		file.write((char *)&eID, 4);
		file.write((char *)&pos.x, 4);
		file.write((char *)&pos.y, 4);
	}

	// save entities
	std::multiset<LuaTrigger*, TriggerManager::ptr_less> *activeTrigs = TriggerManager::Instance()->getActiveTriggers();
	for (auto it = activeTrigs->begin(); it != activeTrigs->end(); it++){
		// each trigger is typed by its filename, which is saved in the world file. 
		// first we have to save the length of the name so the loader can know how many wchar_t
		// to load after.
		int length = (*it)->getFilename().length();
		file.write((char *)&length, 4);

		// We then write the actual filename to memory
		file.write((char *)(*it)->getFilename().c_str(), length * sizeof(wchar_t));

		// then we store the dimensions of this trigger.
		float x = (*it)->getRenderPosition().x;
		float y = (*it)->getRenderPosition().y;
		float sx = (*it)->getRenderSize().x;
		float sy = (*it)->getRenderSize().y;
		file.write((char *)&x, 4);
		file.write((char *)&y, 4);
		file.write((char *)&sx, 4);
		file.write((char *)&sy, 4);
	}

	file.close();

	return true;

}

bool WorldTileMatrix::loadWorld(LPWSTR filepath){
	std::ifstream file(filepath, std::ios::in | std::ios::binary);
	if (!file)
		return false;

	UINT numEnemy = 0;
	UINT numTriggers = 0;
	m_playerSpawn = Vector2(-1,-1);

	file.seekg(0, std::ios::beg);
	file.read((char *)&m_width, 4);
	file.read((char *)&m_height, 4);
	Box2DSingleton::Instance()->setWorldSize(Vector2((float)m_width, (float)m_height));

	file.read((char *)&m_layers, 4);
	file.read((char *)&m_activeLayer, 4);

	UINT pathSize = 0;
	file.read((char *)&pathSize, 4);
	wchar_t buffer[261];
	file.read((char *)buffer, pathSize * sizeof(wchar_t));
	buffer[pathSize] = '\0';
	m_backgroundPath = std::wstring(buffer);
	if (m_backgroundPath.length() > 0)
		loadBackgroundFromPath();

	//READ Music Filepath
	UINT musicPathSize = 0;
	file.read((char *)&musicPathSize, 4);
	wchar_t musicBuffer[261];
	file.read((char *)musicBuffer, musicPathSize * sizeof(wchar_t));
	musicBuffer[musicPathSize] = '\0';
	m_musicPath = wstring(musicBuffer);

	file.read((char *)&numEnemy, 4);
	file.read((char *)&numTriggers, 4);

	file.read((char *)&m_playerSpawn.x, 4);
	file.read((char *)&m_playerSpawn.y, 4);

	m_worldTileArr = new WorldTile[m_width * m_height * m_layers]();

	Box2DSingleton::Instance()->setWorldSize(Vector2((float)m_width, (float)m_height));

	//Load the WorldTiles and assign default static rigidbodies
	for (int layer = 0; layer < m_layers; layer++){
		for (int row = 0; row < m_height; row++){
			for (int col = 0; col < m_width; col++){
				int id;
				file.read((char*)&id, 4);
				m_worldTileArr[col + m_width*row + m_width*m_height*layer].setId(id);
				if (id == 0)
				{
					m_worldTileArr[col + m_width*row + m_width*m_height*layer].setHp(-1);
					m_worldTileArr[col + m_width*row + m_width*m_height*layer].setHpMax(-1);
				}
				else
				{
					m_worldTileArr[col + m_width*row + m_width*m_height*layer].setHp(h_tileSet->getHpFromId(id));
					m_worldTileArr[col + m_width*row + m_width*m_height*layer].setHpMax(h_tileSet->getHpFromId(id));
				}
				m_worldTileArr[col + m_width*row + m_width*m_height*layer].setPosition(Vector2((float)col, (float)row));
				m_worldTileArr[col + m_width*row + m_width*m_height*layer].setLayer(layer);

				if (layer == m_activeLayer){
					b2BodyDef def;
					def.position.Set((float)col + .5f, (float)row + .5f);
					m_worldTileArr[col + m_width*row + m_width*m_height*layer].initRigidbody(def);
					if (id == 0)
						m_worldTileArr[col + m_width*row + m_width*m_height*layer].setSideEnableCollisionWith(WorldTile::ALL, Box2DSingleton::TRIGGER);

				}
			}
		}
	}

	for (size_t i = 0; i < numEnemy; i++){
		UINT id = 0;
		Vector2 pos;
		file.read((char *)&id, 4);
		file.read((char *)&pos.x, 4);
		file.read((char *)&pos.y, 4);

		// TODO create enemies here later
		Enemy *handle;
		if(EnemyManager::Instance()->createEnemy(id, &handle))
			handle->setPosition(pos);
	}

	/*for (int i = 0; i < numTriggers; i++){
		UINT id = 0;
		Vector2 pos;
		file.read((char *)&id, 4);
		file.read((char *)&pos.x, 4);
		file.read((char *)&pos.y, 4);

		// TODO create entities here later
	}*/

	for (size_t i = 0; i < numTriggers; i++){
		int length = 0;
		file.read((char *)&length, 4);
		wchar_t *filename = new wchar_t[length + 1];
		file.read((char *)filename, length * sizeof(wchar_t));
		filename[length] = '\0';

		Vector2 pos;
		Vector2 size;
		file.read((char *)&pos.x, 4);
		file.read((char *)&pos.y, 4);
		file.read((char *)&size.x, 4);
		file.read((char *)&size.y, 4);

		LuaTrigger *trig;
		TriggerManager::Instance()->createTrigger(filename, pos, Vector2(pos.x + size.x, pos.y + size.y), &trig);

		delete[] filename;

	}

	//file.close();

	// We have the data we need, but there is a couple steps left before we can render

	// Create the vertex buffer for the world
	std::vector<CrusadeVertexTypes::TexturedVertex> tempVect;
	for (int k = 0; k < m_layers; k++){
		for (int j = 0; j < m_height; j++){
			for (int i = 0; i < m_width; i++){
				// add the vertices to our vector
				CrusadeVertexTypes::TexturedVertex temp = {};
				temp.Pos = Vector3((float)i, (float)j, -.5f + (-.1f * k));
				temp.Size = Vector2(1, 1);
				temp.Rotation = 0;
				temp.TexIndex = h_tileSet->getTextureIndex(m_worldTileArr[i + j*m_width + k*m_width*m_height].getId());
				temp.Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
				tempVect.push_back(temp);
			}
		}
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(CrusadeVertexTypes::TexturedVertex) * (m_width*m_height*m_layers);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &tempVect[0];
	DX::ThrowIfFailed(h_device->CreateBuffer(&vbd, &vinitData, m_worldVB.ReleaseAndGetAddressOf()));

	// Create the staging buffer.
	D3D11_BUFFER_DESC sbd;
	sbd.Usage = D3D11_USAGE_STAGING;
	sbd.ByteWidth = sizeof(CrusadeVertexTypes::TexturedVertex) * (m_width*m_height*m_layers);
	sbd.BindFlags = 0;
	sbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	sbd.StructureByteStride = sizeof(CrusadeVertexTypes::TexturedVertex);
	sbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	DX::ThrowIfFailed(h_device->CreateBuffer(&sbd, 0, m_stagingVB.ReleaseAndGetAddressOf()));

	h_context->CopyResource(m_stagingVB.Get(), m_worldVB.Get());
	return true;
}

void WorldTileMatrix::updateStagingTileVerts(){
	D3D11_MAPPED_SUBRESOURCE mappedStaging;

	h_context->Map(m_stagingVB.Get(), 0, D3D11_MAP_WRITE, 0, &mappedStaging);

	for (int i = 0; i < m_width*m_height*m_layers; i++){
		CrusadeVertexTypes::TexturedVertex *v = reinterpret_cast<CrusadeVertexTypes::TexturedVertex*>(mappedStaging.pData);
		v[i].TexIndex = h_tileSet->getTextureIndex(m_worldTileArr[i].getId());
		v[i].Color = m_worldTileArr[i].getColor();
	}
	h_context->Unmap(m_stagingVB.Get(), 0);
	m_copyStageBuff = true;
}

void WorldTileMatrix::updateStagingTile(int x, int y, int layer){
	if (x >= m_width || y >= m_height || layer >= m_layers || x < 0 || y < 0 || layer < 0){
		//throw std::invalid_argument("Array index out of bounds in WorldTileMatrix");
		return;	// failed; array index out of bounds
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE mappedStaging;

		WorldTile* tile = &m_worldTileArr[x + y*m_width + layer*m_width*m_height];

		h_context->Map(m_stagingVB.Get(), 0, D3D11_MAP_WRITE, 0, &mappedStaging);

		CrusadeVertexTypes::TexturedVertex *v = reinterpret_cast<CrusadeVertexTypes::TexturedVertex*>(mappedStaging.pData);
		v[x + y*m_width + layer*m_width*m_height].TexIndex = h_tileSet->getTextureIndex(tile->getId());
		v[x + y*m_width + layer*m_width*m_height].Color = tile->getColor();


		h_context->Unmap(m_stagingVB.Get(), 0);

		m_copyStageBuff = true;
	}
}

//Get a pointer to the WorldTile at the specified location and layer
WorldTile* WorldTileMatrix::getTileIdAt(int x, int y, int layer){
	if (x >= m_width || y >= m_height || layer >= m_layers || x < 0 || y < 0 || layer < 0){
		//throw std::invalid_argument("Array index out of bounds in WorldTileMatrix");
		return NULL;	// failed; array index out of bounds
	}
	else
	{
		return &m_worldTileArr[x + y*m_width + layer*m_width*m_height];
	}
}

bool WorldTileMatrix::placeTile(UINT id, int x, int y, int layer){
	if (x >= m_width || y >= m_height || layer >= m_layers || x < 0 || y < 0 || layer < 0){
		//throw std::invalid_argument("Array index out of bounds in WorldTileMatrix");
		return 0;	// failed; array index out of bounds
	}
	else
	{
		m_worldTileArr[x + y*m_width + layer*m_width*m_height].setId(id);
		m_worldTileArr[x + y*m_width + layer*m_width*m_height].setPosition(Vector2((float)x, (float)y));
		if (layer == m_activeLayer && id != 0)
		{
			m_worldTileArr[x + y*m_width + layer*m_width*m_height].setSideEnableCollisionWith(WorldTile::ALL, Box2DSingleton::ALL);
			m_worldTileArr[x + y*m_width + layer*m_width*m_height].setHp(h_tileSet->getHpFromId(id));
			m_worldTileArr[x + y*m_width + layer*m_width*m_height].setHpMax(h_tileSet->getHpFromId(id));
		}
		else if (layer == m_activeLayer && id == 0)
		{
			m_worldTileArr[x + y*m_width + layer*m_width*m_height].setSideEnableCollisionWith(WorldTile::ALL, Box2DSingleton::TRIGGER);
			m_worldTileArr[x + y*m_width + layer*m_width*m_height].setHp(-1);
			m_worldTileArr[x + y*m_width + layer*m_width*m_height].setHp(-1);
		}
		else
		{
			
		}

		D3D11_MAPPED_SUBRESOURCE mappedStaging;

		h_context->Map(m_stagingVB.Get(), 0, D3D11_MAP_WRITE, 0, &mappedStaging);

		CrusadeVertexTypes::TexturedVertex *v = reinterpret_cast<CrusadeVertexTypes::TexturedVertex*>(mappedStaging.pData);
		v[x + y*m_width + layer*m_width*m_height].TexIndex = h_tileSet->getTextureIndex(id);
		v[x + y*m_width + layer*m_width*m_height].Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);


		h_context->Unmap(m_stagingVB.Get(), 0);

		m_copyStageBuff = true;

		return true;

	}
}

// Call this after removing or placing tiles. Try to batch together tile placements
void WorldTileMatrix::copyStageTileVertsToVB(){
	h_context->CopyResource(m_worldVB.Get(), m_stagingVB.Get());
}

UINT WorldTileMatrix::removeTile(int x, int y, int layer){
	UINT retVal = 0;
	if (x >= m_width || y >= m_height || layer >= m_layers || x < 0 || y < 0 || layer < 0){
		//throw std::invalid_argument("Array index out of bounds in WorldTileMatrix");
		return 0;	// failed; array index out of bounds
	}
	else
	{
		retVal = m_worldTileArr[x + y*m_width + layer*m_width*m_height].getId();

		placeTile(0, x, y, layer);
	}

	return retVal;
}

void WorldTileMatrix::damageTilesInFront(int x, int y, int layer, int num){
	if (x >= m_width || y >= m_height || layer >= m_layers || x < 0 || y < 0 || layer < 0){
		//throw std::invalid_argument("Array index out of bounds in WorldTileMatrix");
		return;	// failed; array index out of bounds
	}
	else
	{
		if (layer == 0)
			return;
		for (int l = layer - 1; l >= 0; l--)
			m_worldTileArr[x + y*m_width + l*m_width*m_height].damageThisOnly(num);
	}
}

void WorldTileMatrix::damageTile(int x, int y, int layer, int num){
	if (x >= m_width || y >= m_height || layer >= m_layers || x < 0 || y < 0 || layer < 0){
		//throw std::invalid_argument("Array index out of bounds in WorldTileMatrix");
		return;	// failed; array index out of bounds
	}
	else
	{
		m_worldTileArr[x + y*m_width + layer*m_width*m_height].damageThisOnly(num);
	}
}

UINT WorldTileMatrix::getTextureIndex(UINT id){
	return h_tileSet->getTextureIndex(id);
}

UINT WorldTileMatrix::totalSize(){
	return m_width * m_height * m_layers;
}

UINT WorldTileMatrix::getWidth(){
	return m_width;
}

UINT WorldTileMatrix::getHeight(){
	return m_height;
}

UINT WorldTileMatrix::getLayers(){
	return m_layers;
}

UINT WorldTileMatrix::getActiveLayer(){
	return m_activeLayer;
}

void WorldTileMatrix::setCurrLayer(int currLayer){
	m_currentLayer = currLayer;
}

void WorldTileMatrix::setLayerViewState(LayerViewState state){
	m_currViewState = state;
}

WorldTileMatrix::LayerViewState WorldTileMatrix::getLayerViewState(){
	return m_currViewState;
}

void WorldTileMatrix::setTileSet(TileSet *ts){
	h_tileSet = ts;
}

void WorldTileMatrix::vectorRemoveEle(std::vector<WorldTile*> *vect, WorldTile *ele){
	std::vector<WorldTile*>::iterator it;
	for (it = vect->begin(); it != vect->end(); it++){
		if (*it == ele){
			vect->erase(it);
			return;
		}
	}
}

void WorldTileMatrix::setDeviceAndContextHandle(ID3D11Device *device, ID3D11DeviceContext *context){
	h_device = device;
	h_context = context;
}

void WorldTileMatrix::setBackground(LPWSTR filepath, RECT rc){
	m_rect = rc;
	//wchar_t buffer[300];
	//CrusadeUtil::getRelativePathTo(filepath, buffer);
	m_backgroundPath = std::wstring(filepath);
	loadBackgroundFromPath();
}

void WorldTileMatrix::setBackgroundRect(RECT rc){
	m_rect = rc;
}

void WorldTileMatrix::loadBackgroundFromPath(){
	DX::ThrowIfFailed(CreateWICTextureFromFile(h_device, m_backgroundPath.c_str(), nullptr, m_background.ReleaseAndGetAddressOf()));
}

/*Set the spawn point for this world tile matrix*/
void WorldTileMatrix::setPlayerSpawn(Vector2 spawnPoint){
	m_playerSpawn = spawnPoint;
}

/*Get the player spawn point*/
Vector2 WorldTileMatrix::getPlayerSpawn(){
	return m_playerSpawn;
}

void WorldTileMatrix::setMusicPath(LPWSTR filepath){
	m_musicPath = filepath;
}

wstring WorldTileMatrix::getMusicPath(){
	return m_musicPath;
}