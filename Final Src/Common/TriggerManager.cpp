#include "pch.h"
#include "TriggerManager.h"


TriggerManager* TriggerManager::m_instance = NULL;

TriggerManager* TriggerManager::Instance(LPWSTR path, LuaBindings* bindings){
	if (!m_instance){
		m_instance = new TriggerManager(path);
	}
	else if (m_instance->getScriptFolder().compare(L"") == 0){
		m_instance = new TriggerManager(path);
	}
	m_instance->h_bindings = bindings;
	return m_instance;
}

TriggerManager* TriggerManager::Instance(){
	if (!m_instance){
		m_instance = new TriggerManager();
	}
	return m_instance;
}

TriggerManager::TriggerManager(LPWSTR path) : TriggerManager(){
	loadLuaData(path);
}

TriggerManager::TriggerManager(){
	m_trigDebugMode = true;
	m_worldEditMode = true;
	
	buildD3DResources();
}

TriggerManager::~TriggerManager(){
	// Clean up m_renderSet
	std::set<LuaTrigger*, ptr_less>::iterator trigIt;
	for (trigIt = m_activeTriggers.begin(); trigIt != m_activeTriggers.end(); trigIt++){
		delete (*trigIt);
	}
	m_VB.Reset();
	m_font.reset(nullptr);
	m_spriteBatch.reset(nullptr);
}

void TriggerManager::stepTriggers(float dt){
	for (auto it = m_activeTriggers.begin(); it != m_activeTriggers.end(); it++){
		(*it)->step(dt);
	}
}

void TriggerManager::interpolate(float alpha){
	for (auto it = m_activeTriggers.begin(); it != m_activeTriggers.end(); it++){
		(*it)->interpolate(alpha);
	}
}

void TriggerManager::debugRender(Camera *cam){
	if (m_trigDebugMode){
		ID3D11DeviceContext *context = D3DManager::Instance()->getContext();

		UINT stride = sizeof(CrusadeVertexTypes::TexturedVertex);
		UINT offset = 0;

		D3D11_MAPPED_SUBRESOURCE mappedVB;
		context->Map(m_VB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB);

		CrusadeVertexTypes::TexturedVertex *v = reinterpret_cast<CrusadeVertexTypes::TexturedVertex*>(mappedVB.pData);
		int i = 0;
		for (auto it = m_activeTriggers.begin(); it != m_activeTriggers.end(); it++){
			v[i].Pos = Vector3((*it)->getRenderPosition().x, (*it)->getRenderPosition().y, .1f);
			v[i].Rotation = (*it)->getRotation();
			v[i].Size = (*it)->getRenderSize();
			v[i].TexIndex = 0;
			v[i].Color = (*it)->getColor();
			i++;
		}

		context->Unmap(m_VB.Get(), 0);

		CrusadeEffects::TextureFX->SetColorCorrect(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		CrusadeEffects::TextureFX->m_ColorBoxTech->GetPassByIndex(0)->Apply(0, context);
		context->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &stride, &offset);

		context->Draw(m_activeTriggers.size(), 0);

		context->GSSetShader(nullptr, nullptr, 0);

		m_effect->Apply(context);
		context->IASetInputLayout(m_inputLayout.Get());
		context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//m_spriteBatch->Begin(SpriteSortMode_Deferred, nullptr, nullptr, nullptr, nullptr, nullptr, viewproj);
		m_spriteBatch->Begin();

		for (auto it = m_activeTriggers.begin(); it != m_activeTriggers.end(); it++){
			m_font->DrawString(m_spriteBatch.get(), (*it)->getFilename().c_str(), cam->WorldToScreenPoint((*it)->getRenderPosition()), Colors::White, 0.0f, Vector2(0.0f, 0.0f), Vector2(.5f, .5f));
		}

		m_spriteBatch->End();
	}
}

bool TriggerManager::loadLuaData(LPWSTR folder_path){
	using namespace std;
	using namespace std::tr2::sys;
	m_filepaths.clear();
	m_luascripts.clear();

	m_scriptFolder = folder_path;
	path scriptDir = CW2A(folder_path);

	USES_CONVERSION;

	for (directory_iterator i(scriptDir), end; i != end; i++){
		path extension = i->path().extension();
		if (i->path().extension().compare(".lua") == 0)
		{
			m_filepaths.insert(m_filepaths.end(), A2CW(i->path().filename().c_str()));
			std::string script = CrusadeUtil::loadFileAsString(A2W(i->path().string().c_str()));
			m_luascripts.insert(m_luascripts.end(), script);

		}
		else{
			CrusadeUtil::debug("\nNot a lua script: ");
			CrusadeUtil::debug(i->path());
		}
	}

	return true;
}

void TriggerManager::populateListbox(HWND listbox_hwnd){
	SendMessage(listbox_hwnd, LB_RESETCONTENT, 0, 0);
	int pos = (int)SendMessage(listbox_hwnd, LB_ADDSTRING, 0, (LPARAM)(L"ERASER"));
	if (!m_instance){
		return;
	}
	for (int i = 0; (UINT)i < m_filepaths.size(); i++){
		int pos = (int)SendMessage(listbox_hwnd, LB_ADDSTRING, 0, (LPARAM)(m_filepaths.at(i).c_str()));
		SendMessage(listbox_hwnd, LB_SETITEMDATA, pos, (LPARAM)(i));
	}
}

std::wstring TriggerManager::getScriptFolder(){
	return m_scriptFolder;
}

bool TriggerManager::isWorldEditMode(){
	return m_worldEditMode;
}
void TriggerManager::setWorldEditMode(bool mode){
	m_worldEditMode = mode;
}

bool TriggerManager::isDebugMode(){
	return m_trigDebugMode;
}

void TriggerManager::setDebugMode(bool mode){
	m_trigDebugMode = mode;
}

bool TriggerManager::LuaDataIsLoaded(){
	if (m_luascripts.size() == 0)
		return false;
	else
		return true;
}

bool TriggerManager::createTrigger(std::wstring filename, Vector2 p1, Vector2 p2, _Out_ LuaTrigger** trigger_out){
	// first we have to look up our filename in our filenames vector to get the index into our scripts vector
	int index = -1;
	for (int i = 0; (UINT)i < m_filepaths.size(); i++){
		if (m_filepaths[i].compare(filename) == 0){
			index = i;
			break;
		}
	}

	LuaTrigger *trig = new LuaTrigger(h_bindings->getLuaState(), "TRIGGER", "LUATRIGGER", filename, p1, Vector2(p2.x - p1.x, p2.y - p1.y), 0, 0);
	if (index == -1){
		trig->setColor(Vector4(1.0f, 0.0f, 0.0f, .5f));
	}
	m_activeTriggers.insert(trig);

	(*trigger_out) = trig;

	return true;
}

bool TriggerManager::deleteTriggerAt(float worldx, float worldy){
	for (auto it = m_activeTriggers.begin(); it != m_activeTriggers.end(); it++){
		if (CrusadeUtil::isPointIntersecting(Vector2(worldx, worldy), (*it)->getPosition(), (*it)->getSize())){
			delete (*it);
			m_activeTriggers.erase(it++);
			return true;
		}
	}
	return false;
}

bool TriggerManager::deleteTrigger(LuaTrigger* trigger){
	for (auto it = m_activeTriggers.begin(); it != m_activeTriggers.end(); it++){
		if (trigger == (*it)){
			delete (*it);
			m_activeTriggers.erase(it);
		}
	}

	return false;
}

void TriggerManager::deleteAllTriggers(){
	for (auto it = m_activeTriggers.begin(); it != m_activeTriggers.end(); it++){
		delete (*it);
	}
	m_activeTriggers.clear();
}

lua_State* TriggerManager::runScript(std::wstring filepath, LuaTrigger* trigger, GameObject* collider){
	for (int i = 0; (UINT)i < m_filepaths.size(); i++){
		if (filepath.compare(m_filepaths[i]) == 0){
			int error = luaL_loadstring(h_bindings->getLuaState(), m_luascripts[i].c_str());
			if (error){
				CrusadeUtil::debug(lua_tostring(h_bindings->getLuaState(), -1));
			}
			// define the functions onEnterTrigger and onExitTrigger on the global table.
			if (lua_pcall(h_bindings->getLuaState(), 0, 0, 0)){
				std::string err = lua_tostring(h_bindings->getLuaState(), -1);
				OutputDebugStringA(err.c_str());
				lua_error(h_bindings->getLuaState());
			}
			// create a new coroutine thread and push it onto the stack. It gets its own local stack, so we can have multiple programs
			// running different definitions of the same function
			lua_State *thread = lua_newthread(h_bindings->getLuaState());

			// we have two functions and one variable to push on the stack
			//		string precondition
			//		onEnterTrigger(context, trigger, collider)
			//		onExitTrigger(context, trigger, collider)

			// context is a variable that refers to the binder.



			// on the main lua state, lets push the definition of onExitTrigger on to the stack.
			lua_getglobal(h_bindings->getLuaState(), "onExitTrigger");
			// move the top 1 element of our main lua state to our threads stack. should be onExitTrigger
			lua_xmove(h_bindings->getLuaState(), thread, 1);
			// place the functions arguments on to the stack
			lua_pushlightuserdata(thread, h_bindings);
			lua_pushlightuserdata(thread, trigger);
			lua_pushlightuserdata(thread, collider);

			// push onEnterTrigger on the top of the stack since it will be called first.
			lua_getglobal(h_bindings->getLuaState(), "onEnterTrigger");
			lua_xmove(h_bindings->getLuaState(), thread, 1);
			lua_pushlightuserdata(thread, h_bindings);
			lua_pushlightuserdata(thread, trigger);
			lua_pushlightuserdata(thread, collider);

			// lets get the precondition last, which the luatrigger will immediately check
			lua_getglobal(h_bindings->getLuaState(), "precondition");
			// move the top 1 element of our main lua state to our threads stack. should be precondition
			lua_xmove(h_bindings->getLuaState(), thread, 1);

			//reset the global table
			lua_pushnil(h_bindings->getLuaState());
			lua_setglobal(h_bindings->getLuaState(), "onExitTrigger");

			lua_pushnil(h_bindings->getLuaState());
			lua_setglobal(h_bindings->getLuaState(), "onEnterTrigger");

			lua_pushnil(h_bindings->getLuaState());
			lua_setglobal(h_bindings->getLuaState(), "precondition");

			return thread;

		}
	}
	return nullptr;
}

void TriggerManager::anyKeyPressed(){
	for (auto it = m_activeTriggers.begin(); it != m_activeTriggers.end(); it++){
		(*it)->anyKeyPressed();
	}
}


std::multiset<LuaTrigger*, TriggerManager::ptr_less>* TriggerManager::getActiveTriggers(){
	return &m_activeTriggers;
}

void TriggerManager::setLuaBindings(LuaBindings* bindings){
	h_bindings = bindings;
}

LuaBindings *TriggerManager::getLuaBindings(){
	return h_bindings;
}


void TriggerManager::buildD3DResources(){
	//Non textured effect for grid

	m_font.reset(new SpriteFont(D3DManager::Instance()->getDevice(), L"../img/courier.spritefont"));
	m_spriteBatch.reset(new SpriteBatch(D3DManager::Instance()->getContext()));

	// create the vertex buffer

	CrusadeVertexTypes::TexturedVertex temp[MAX_TRIGGERS];

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(CrusadeVertexTypes::TexturedVertex) * MAX_TRIGGERS;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &temp;

	DX::ThrowIfFailed(D3DManager::Instance()->getDevice()->CreateBuffer(&vbd, &vinitData, m_VB.ReleaseAndGetAddressOf()));

	m_effect.reset(new BasicEffect(D3DManager::Instance()->getDevice()));
	m_effect->SetVertexColorEnabled(true);

	void const* shaderByteCode;
	size_t byteCodeLength;

	m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	DX::ThrowIfFailed(D3DManager::Instance()->getDevice()->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
		shaderByteCode, byteCodeLength,
		m_inputLayout.ReleaseAndGetAddressOf()));

}