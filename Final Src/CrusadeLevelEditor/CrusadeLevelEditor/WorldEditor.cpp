#include "pch.h"
#include "WorldEditor.h"
#include "resource.h"
#include "LuaBindings.h"
#include "TriggerManager.h"

WorldEditor::WorldEditor(ID3D11Device *device, ID3D11DeviceContext *context){

	m_grid.reset(new Grid());
	m_tool.reset(new DrawTool(this));
	m_worldTileMatrix.reset(nullptr);
	m_tileSet.reset(new TileSet(device, context));
	m_gridvis = true;
	m_character = nullptr;
	m_gridBatch.reset(new PrimitiveBatch<VertexPositionColor>(context));
	m_textureBatch.reset(new PrimitiveBatch<VertexPositionColorTexture>(context));

	m_font.reset(new SpriteFont(device, L"../img/courier.spritefont"));
	m_spriteBatch.reset(new SpriteBatch(context));
	m_character.reset(new Character(Vector2(-1, -1), 0));

	m_bindings.reset(new LuaBindings(m_character.get()));
	TriggerManager::Instance(L"../img/Scripts/", m_bindings.get());

	m_fontPos.x = 10;
	m_fontPos.y = 10;

	m_currentLayer = 0;

	setDeviceAndContextHandle(device, context);
	AbilityManager::Instance("../img/Abilities.xml");
	BuildRenderEffects();
}


WorldEditor::~WorldEditor(){
	m_worldTileMatrix.reset(nullptr);
	m_tileSet.reset(nullptr);
	m_tool.reset(nullptr);
	m_grid.reset(nullptr);
	m_gridBatch.reset(nullptr);
	m_textureBatch.reset(nullptr);
	m_effect.reset(nullptr);
	m_textureEffect.reset(nullptr);
	m_font.reset(nullptr);
	m_spriteBatch.reset(nullptr);
	m_inputLayout.Reset();
	m_textureInputLayout.Reset();

	delete AbilityManager::Instance();
	delete EnemyManager::Instance();
}

void WorldEditor::Render(){
	ID3D11DeviceContext *context = D3DManager::Instance()->getContext();

	// give our shader the view and projection matrices that represent our camera
	m_effect->SetView(Game::m_camera->getView());
	m_effect->SetProjection(Game::m_camera->getProj());

	m_textureEffect->SetView(Game::m_camera->getView());
	m_textureEffect->SetProjection(Game::m_camera->getProj());
	
	// if the user has created or loaded a world
	if (m_worldTileMatrix.get()){

		m_worldTileMatrix->RenderBackground();
		m_worldTileMatrix->RenderLayers(Game::m_camera->getViewProj(), m_worldTileMatrix->getLayers()-1, (m_worldTileMatrix->getLayers() - m_worldTileMatrix->getActiveLayer()) + 1);
		EnemyManager::Instance()->renderEnemies();
		if (m_character->getPosition().x >= 0 || m_character->getPosition().y >= 0){
			m_character->renderCharacter();
		}
		TriggerManager::Instance()->debugRender(Game::m_camera);
		m_worldTileMatrix->RenderLayers(Game::m_camera->getViewProj(), m_worldTileMatrix->getActiveLayer() - 1, m_worldTileMatrix->getActiveLayer());

		if (m_gridvis){
			// the grids world matrix sets it in front of the camera at all times
			m_effect->SetWorld(Matrix::Identity);
			// put the shader we've built on to the gpu pipeline for use in rendering
			m_effect->Apply(context);
			context->IASetInputLayout(m_inputLayout.Get());
			context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

			// set up the vertex buffer that will be linked to the pipeline
			m_gridBatch->Begin();
			// add vertices to the vertex buffer
			m_grid->Draw(m_gridBatch.get());
			// send those vertices to the gpu pipeline to be rendered to the screen
			m_gridBatch->End();
		}
		// m_tool's world matrix follows the users mouse cursor
		m_effect->SetWorld(m_tool->getWorld());
		m_effect->Apply(context);
		context->IASetInputLayout(m_inputLayout.Get());
		context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		m_gridBatch->Begin();
		m_tool->DrawCursor(m_gridBatch.get());
		m_gridBatch->End();

		m_spriteBatch->Begin();

		m_stream.str(std::wstring());
		m_stream.clear();

		std::wstring output = L"Current Layer: ";
		m_stream << output << m_currentLayer << endl;
		m_stream << "Tool Pos: "<< "X " << m_tool->getX() << " Y " << m_tool->getY() << endl;
		m_stream << "Camera Pos: " << "X " << Game::m_camera->getPosition().x << " Y " << Game::m_camera->getPosition().y << endl;
		m_stream << "Music: " << m_worldTileMatrix->getMusicPath() << endl;
		Vector2 origin(0.0f, 0.0f);


		m_font->DrawString(m_spriteBatch.get(), m_stream.str().c_str(), m_fontPos, Colors::White, 0.0f, origin, Vector2(.5f, .5f));

		m_spriteBatch->End();
	}
}

void WorldEditor::Update(float dt){
	float x = Game::m_camera->getX();
	float y = Game::m_camera->getY();
	if (m_worldTileMatrix){
		m_worldTileMatrix->update(dt);
		Box2DSingleton::Instance()->stepWorld(dt);
		m_grid->setWorld(Matrix::CreateTranslation(floorf(x), floorf(y), 0));
		EnemyManager::Instance()->stepEnemies(dt);
	}
	if (m_character->getPosition().x >= 0 || m_character->getPosition().y >= 0){
		m_character->step(dt);
		m_worldTileMatrix->setPlayerSpawn(m_character->getPosition());
	}
}

Grid* WorldEditor::getGrid(){
	return m_grid.get();
}

void WorldEditor::BuildRenderEffects(){
	ID3D11Device *device = D3DManager::Instance()->getDevice();

	//Non textured effect for grid
	m_effect.reset(new BasicEffect(device));
	m_effect->SetVertexColorEnabled(true);

	void const* shaderByteCode;
	size_t byteCodeLength;

	m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	DX::ThrowIfFailed(device->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
		shaderByteCode, byteCodeLength,
		m_inputLayout.ReleaseAndGetAddressOf()));

	//Textured effect for everything else
	m_textureEffect.reset(new BasicEffect(device));
	m_textureEffect->SetVertexColorEnabled(false);
	m_textureEffect->SetTextureEnabled(true);

	m_textureEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	DX::ThrowIfFailed(device->CreateInputLayout(VertexPositionColorTexture::InputElements, VertexPositionColorTexture::InputElementCount,
		shaderByteCode, byteCodeLength,
		m_textureInputLayout.ReleaseAndGetAddressOf()));
}

void WorldEditor::toggleGrid(){
	m_gridvis = !m_gridvis;
}

bool WorldEditor::isGridVis(){
	return m_gridvis;
}

TileSet* WorldEditor::getTileSet(){
	return m_tileSet.get();
}

Tool* WorldEditor::getTool(){
	return m_tool.get();
}

void WorldEditor::setTool(int idm_tool_id){
	UINT sel;
	switch (idm_tool_id){
		case IDM_TOOL_BOX:
			sel = m_tool->getTileType();
			m_tool.reset(new BoxTool(this));
			m_tool->setTileType(sel);
			break;
		case IDM_TOOL_PENCIL:
			sel = m_tool->getTileType();
			m_tool.reset(new DrawTool(this));
			m_tool->setTileType(sel);
			break;
		case IDM_TOOL_ENEMY:
			sel = m_tool->getTileType();
			m_tool.reset(new EnemyTool(this));
			m_tool->setTileType(sel);
			break;
		case IDM_TOOL_PLAYERSPAWNPOINT:
			m_tool.reset(new PlayerSpawnTool(this));
			m_tool->setTileType(0);//This should be the ID of setting a player spawn (no erase function, it just repositions the spawnpoint
			break;
		case IDM_TOOL_LUATRIGGER:
			m_tool.reset(new LuaTriggerTool(this));
			m_tool->setSelectedString(L"ERASER");
			break;
	}
}

void WorldEditor::CreateNewWorld(int width, int height, int layers, int active){
	EnemyManager::Instance()->deleteAllEnemies();
	TriggerManager::Instance()->deleteAllTriggers();
	Box2DSingleton::Instance()->clearWorld();

	m_character.reset(nullptr);
	m_worldTileMatrix.reset(nullptr);
	m_grid.reset(nullptr);
	m_gridBatch.reset(nullptr);
	
	Box2DSingleton::Instance()->initWorld();
	m_character.reset(new Character(Vector2(-1, -1), 0));
	m_worldTileMatrix.reset(new WorldTileMatrix(h_device, h_context, width, height, layers, active));
	m_grid.reset(new Grid(width, height));
	m_gridBatch.reset(new PrimitiveBatch<VertexPositionColor>(h_context));
	m_worldTileMatrix->setTileSet(m_tileSet.get());
}

void WorldEditor::CreateNewWorld(LPWSTR path){
	EnemyManager::Instance()->deleteAllEnemies();
	TriggerManager::Instance()->deleteAllTriggers();
	Box2DSingleton::Instance()->clearWorld();

	m_character.reset(nullptr);
	m_worldTileMatrix.reset(nullptr);
	m_grid.reset(nullptr);
	m_gridBatch.reset(nullptr);

	Box2DSingleton::Instance()->initWorld();
	m_character.reset(new Character(Vector2(-1, -1), 0));
	m_worldTileMatrix.reset(new WorldTileMatrix(h_device, h_context, path, m_tileSet.get()));
	m_grid.reset(new Grid(m_worldTileMatrix->getWidth(), m_worldTileMatrix->getHeight()));
	m_gridBatch.reset(new PrimitiveBatch<VertexPositionColor>(h_context));
	if (m_worldTileMatrix->getPlayerSpawn() != Vector2(-1, -1)){
		setCharacterPosition(m_worldTileMatrix->getPlayerSpawn());
	}
}

WorldTileMatrix* WorldEditor::getWorldTileMatrix(){
	return m_worldTileMatrix.get();
}

void WorldEditor::placeTile(UINT id, Vector2 pos){
	m_worldTileMatrix->placeTile(id, pos.x, pos.y, m_currentLayer);
}

void WorldEditor::placeTile(UINT id, float x, float y){
	m_worldTileMatrix->placeTile(id, x, y, m_currentLayer);
}

void WorldEditor::setDeviceAndContextHandle(ID3D11Device *device, ID3D11DeviceContext *context){
	h_device = device;
	h_context = context;

	m_tileSet->setDeviceAndContextHandle(device, context);

	if (m_worldTileMatrix)
		m_worldTileMatrix->setDeviceAndContextHandle(device, context);
	EnemyManager::Instance()->setDeviceAndContextHandle(device, context);

	m_gridBatch.reset(new PrimitiveBatch<VertexPositionColor>(context));
	m_textureBatch.reset(new PrimitiveBatch<VertexPositionColorTexture>(context));

	BuildRenderEffects();
}

void WorldEditor::incCurrLayer(){
	int num2 = (m_worldTileMatrix->getLayers() - 1);
	bool testtrue = m_currentLayer < num2;
	bool istrue = m_currentLayer < (m_worldTileMatrix->getLayers() - 1);

	if (testtrue){
		m_currentLayer++;
		m_worldTileMatrix->setCurrLayer(m_currentLayer);
	}
}

void WorldEditor::decCurrLayer(){
	if (m_currentLayer > 0){
		m_currentLayer--;
		m_worldTileMatrix->setCurrLayer(m_currentLayer);
	}
}

/*Set the position of this world editor's character. Create a new character if one doesn't already exist.*/
void WorldEditor::setCharacterPosition(Vector2 position){
	m_character->setPosition(position);
}

/*Get the position of this world editor's character*/
Vector2 WorldEditor::getCharacterPosition(){
	return m_character->getPosition();
}

/*Get the character of this world editor*/
Character* WorldEditor::getCharacter(){
	return m_character.get();
}