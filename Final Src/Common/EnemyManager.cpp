#include "pch.h"
#include "EnemyManager.h"

EnemyManager* EnemyManager::m_instance = NULL;

EnemyManager* EnemyManager::Instance(std::string path){
	if (!m_instance){
		m_instance = new EnemyManager(path);
	}
	else if (m_instance->getXMLPath().compare("") == 0){
		m_instance = new EnemyManager(path);
	}
	return m_instance;
}

EnemyManager* EnemyManager::Instance(LPWSTR path){
	if (!m_instance){
		m_instance = new EnemyManager(path);
	}
	else if (m_instance->getXMLPath().compare("") == 0){
		m_instance = new EnemyManager(path);
	}
	return m_instance;
}

EnemyManager* EnemyManager::Instance(){
	if (!m_instance){
		m_instance = new EnemyManager();
	}
	return m_instance;
}

//Alternate constructor using LPWSTR
EnemyManager::EnemyManager(LPWSTR path) : EnemyManager(){
	this->m_xmlPath = "LPWSTR path used";
	m_worldEditMode = true;
	loadEnemyData(path);
}

//Creates a new EnemyManager. THERE SHOULD ONLY BE ONE! If one already exists, it calls changeEnemyList with the parameter `path` before deleting the second EnemyManager.
EnemyManager::EnemyManager(std::string path) : EnemyManager(){
	m_worldEditMode = true;
	this->m_xmlPath = path;
	loadEnemyData();
}

//boring default
EnemyManager::EnemyManager(){
	m_worldEditMode = true;
	m_paused = false;
	// create the vertex buffer

	CrusadeVertexTypes::TexturedVertex temp[MAX_ENEMIES];

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(CrusadeVertexTypes::TexturedVertex) * MAX_ENEMIES;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &temp;

	DX::ThrowIfFailed(D3DManager::Instance()->getDevice()->CreateBuffer(&vbd, &vinitData, m_VB.ReleaseAndGetAddressOf()));
}

EnemyManager::~EnemyManager(){
	m_instance = nullptr;
	// Clean up m_enemies
	std::map<UINT, Enemy*>::iterator it;
	for (it = m_enemies.begin(); it != m_enemies.end(); it++){
		delete it->second;
	}

	// Clean up m_renderSet
	std::set<Enemy*, ptr_less>::iterator renIt;
	for (renIt = m_renderSet.begin(); renIt != m_renderSet.end(); renIt++){
		delete (*renIt);
	}

	// Clean up m_texPaths
	std::map<UINT, std::vector<wchar_t*>>::iterator pathIt;
	for (pathIt = m_texPaths.begin(); pathIt != m_texPaths.end(); pathIt++){
		for (size_t i = 0; i < pathIt->second.size(); i++){
			delete[] pathIt->second.at(i);
		}
	}

	// Clean up m_texArrays
	std::map<UINT, ID3D11ShaderResourceView *>::iterator texIt;
	for (texIt = m_texArrays.begin(); texIt != m_texArrays.end(); texIt++){
		texIt->second->Release();
	}

	// Clean up vertex buffer
	m_VB.Reset();
}

// overwrite vertex buffer with latest data and draw
void EnemyManager::renderEnemies(){

	ID3D11DeviceContext *context = D3DManager::Instance()->getContext();

	UINT stride = sizeof(CrusadeVertexTypes::TexturedVertex);
	UINT offset = 0;

	D3D11_MAPPED_SUBRESOURCE mappedVB;
	context->Map(m_VB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB);

	CrusadeVertexTypes::TexturedVertex *v = reinterpret_cast<CrusadeVertexTypes::TexturedVertex*>(mappedVB.pData);
	std::multiset<Enemy*, ptr_less>::iterator it;
	int i = 0;
	for (it = m_renderSet.begin(); it != m_renderSet.end(); it++){
		v[i].Pos = Vector3((*it)->getRenderPosition().x, (*it)->getRenderPosition().y, .1f);
		v[i].Rotation = (*it)->getRotation();
		v[i].Size = (*it)->getRenderSize();
		v[i].TexIndex = (*it)->getTexIndex();
		v[i].Color = (*it)->getColor();
		i++;
	}

	context->Unmap(m_VB.Get(), 0);

	// now draw each type
	std::map<UINT, std::vector<std::pair<UINT, UINT>>>::iterator rendIt;
	UINT count = 0;
	UINT start = 0;
	// for each type
	for (rendIt = m_indexSizeMap.begin(); rendIt != m_indexSizeMap.end(); rendIt++){
		// tell the D3D pipeline how our verts are formatted
		context->IASetInputLayout(InputLayouts::TexturedVertexLayout);
		// We want to draw points because our geometry shader expands them to quads.
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		// Use our vertex buffer
		context->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &stride, &offset);
		// Use the array of textures for this type of ability (rendIt->first is the ID of the type we're drawing)
		CrusadeEffects::TextureFX->SetDiffuseMaps(m_texArrays[rendIt->first]);
		// Set the color correction.
		CrusadeEffects::TextureFX->SetColorCorrect(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		// apply the shaders we're using
		CrusadeEffects::TextureFX->m_SpriteTech->GetPassByIndex(0)->Apply(0, context);

		// now we have to figure out the correct values of count and start for this loop
		// the verts we're drawing are grouped together by type because of our m_renderSet multiset variable
		// like so,
		/*
		VERT_BUFFER = { SHEEP_VERT, SHEEP_VERT, SHEEP_VERT, SHEEP_VERT, SHEEPLE_VERT, SHEEPLE_VERT, DEMON_VERT }

		for the first iteration in this example, we need to draw sheep. The start position would be 0,
		and the count would be the number of sheep we want to draw, in this case, 4

		in the next iteration, we need to draw the sheeple. The start position would be equal to the previous count: 4,
		and the count would be 2.

		in the final iteration, we need to draw the demon(s). The start position would be equal to the cumulative previous counts: 7
		and the count would be 1
		*/
		// We can see from this example that for each iteration, we need to iterate our renderset and increment start until we
		// hit the first ability with the same id as the id we're looking for, so until ability->id == rendIt->first

		// Then, we continue iterating our renderset and begin incrementing count instead of start, until ability->id != rendIt->first

		std::multiset<Enemy*, ptr_less>::iterator setIt;
		setIt = m_renderSet.begin();

		// figure out our start val
		for (; setIt != m_renderSet.end(); setIt++){
			if ((*setIt)->getId() == rendIt->first)
				break;
			start++;
		}
		// figure out our count val
		for (; setIt != m_renderSet.end(); setIt++){
			if ((*setIt)->getId() != rendIt->first)
				break;
			count++;
		}

		// if start is equal to the renderset size then we had no verts of that type to be drawn
		if (start != m_renderSet.size())
			context->Draw(count, start);

		count = 0;
		start = 0;
	}
}

/*Handles all Enemy updating. dt is the change in time since the last update.*/
void EnemyManager::stepEnemies(float dt){
	if (!m_paused){
		Vector2 worldSize;
		if (WorldTileMatrix::Instance() != nullptr){
			worldSize.x = (float)WorldTileMatrix::Instance()->getWidth();
			worldSize.y = (float)WorldTileMatrix::Instance()->getHeight();

			std::multiset<Enemy*, ptr_less>::iterator it;
			for (it = m_renderSet.begin(); it != m_renderSet.end();){
				if (!CrusadeUtil::isPointIntersecting((*it)->getCenterPosition(), Vector2(0, 0), worldSize)){
					if ((*it)->m_containingTrigger)
						(*it)->m_containingTrigger->notifyOfDeath((*it));
					delete(*it);
					auto tempIt = it;
					it++;
					m_renderSet.erase(tempIt);
					continue;
				}
				if (EnemyManager::Instance()->isWorldEditMode()){
					it++;
				}
				else{
					(*it)->step(dt);
					(*it)->checkHP();
					if ((*it)->isDead()){
						if ((*it)->m_containingTrigger)
							(*it)->m_containingTrigger->notifyOfDeath((*it));
						delete (*it);
						it = m_renderSet.erase(it);
					}
					else{
						it++;
					}
				}
			}
		}
	}
}

void EnemyManager::interpolate(float alpha){
	std::multiset<Enemy*, ptr_less>::iterator it;
	for (it = m_renderSet.begin(); it != m_renderSet.end(); it++){
		(*it)->interpolate(alpha);
	}
}

//Load enemy data from m_xmlPath and store for future use
bool EnemyManager::loadEnemyData(){
	wchar_t *w_xmlPath;
	CrusadeUtil::s_to_ws(m_xmlPath, &w_xmlPath);
	bool ret = loadEnemyData((LPWSTR)w_xmlPath);
	delete[] w_xmlPath;
	return ret;
}

bool EnemyManager::loadEnemyData(LPWSTR path){
	std::tr2::sys::path filePath = std::tr2::sys::path(CW2A(path));
	relativePathAdjust = filePath.remove_filename();//This is so that paths for the enemy animations will be correct
	xml_document<> doc;
	std::string  xml = CrusadeUtil::loadFileAsString(path);
	const char *c_xml = xml.c_str();
	//strcpy(c_xml, xml.c_str());
	doc.parse<0>((char*)c_xml);
	xml_node<> *enemy = doc.first_node()->first_node("Enemy");
	if (!enemy)
		return false;
	do{
		int id = std::stoi(enemy->first_node("ID")->value());
		char *name = enemy->first_node("Name")->value();
		char *assetPath = enemy->first_node("Path")->value();

		float textureWidth = std::stof(enemy->first_node("TextureWidth")->value());
		float textureHeight = std::stof(enemy->first_node("TextureHeight")->value());
		float hitBoxWidth = std::stof(enemy->first_node("HitBoxWidth")->value());
		float hitBoxHeight = std::stof(enemy->first_node("HitBoxHeight")->value());
		float hitBoxOffsetX = std::stof(enemy->first_node("HitBoxOffsetX")->value());
		float hitBoxOffsetY = std::stof(enemy->first_node("HitBoxOffsetY")->value());

		float attackMultiplier = std::stof(enemy->first_node("AttackMultiplier")->value());
		int hp = std::stoi(enemy->first_node("HP")->value());
		float speed = std::stof(enemy->first_node("Speed")->value());
		int exp = std::stoi(enemy->first_node("EXP")->value());
		float sight = std::stof(enemy->first_node("Sight")->value());
		float hearing = std::stof(enemy->first_node("Hearing")->value());
		float massDensity = std::stof(enemy->first_node("MassDensity")->value());
		int courage = std::stoi(enemy->first_node("Courage")->value());
		float strength = std::stof(enemy->first_node("Strength")->value());
		float jumpStrength = std::stof(enemy->first_node("JumpStrength")->value());
		std::string aiType = CrusadeUtil::toLowerCase(enemy->first_node("AI")->value());
		UINT aiFlags = Enemy::parseAIFlags(enemy->first_node("AI_Flags")->value());
		std::string path = enemy->first_node("Path")->value();
		std::string abilities;
		if (enemy->first_node("Abilities")){
			abilities = enemy->first_node("Abilities")->value();
		}
		else{
			abilities = "null";
		}
		Enemy::SoundDescription sounds;
		if (enemy->first_node("EngageSounds")){
			sounds.engageSounds = parseSounds(enemy->first_node("EngageSounds")->value());
		}
		else{
			sounds.engageSounds = parseSounds("");
		}
		if (enemy->first_node("HitSounds")){
			sounds.hitSounds = parseSounds(enemy->first_node("HitSounds")->value());
		}
		else{
			sounds.hitSounds = parseSounds("");
		}
		if (enemy->first_node("TauntSounds")){
			sounds.tauntSounds = parseSounds(enemy->first_node("TauntSounds")->value());
		}
		else{
			sounds.tauntSounds = parseSounds("");
		}
		if (enemy->first_node("DeathSounds")){
			sounds.deathSounds = parseSounds(enemy->first_node("DeathSounds")->value());
		}
		else{
			sounds.deathSounds = parseSounds("");
		}
		if (enemy->first_node("FleeSounds")){
			sounds.fleeSounds = parseSounds(enemy->first_node("FleeSounds")->value());
		}
		else{
			sounds.fleeSounds = parseSounds("");
		}
		//SendMessage(listbox_hwnd, LB_ADDSTRING, 0, (LPARAM)(s_to_ws(name)));
		CrusadeUtil::debug(aiType);
		CrusadeUtil::debug("\n");

		
		//wchar_t *wpath;
		//CrusadeUtil::s_to_ws(assetPath, &wpath);
		if (aiType.compare(Enemy::MELEE)==0){
			m_enemies.insert(m_enemies.end(), std::pair<UINT, Enemy*>((UINT)id, new MeleeEnemy("ENEMY", name, Vector2(0, 0), Vector2(textureWidth, textureHeight), Vector2(hitBoxWidth, hitBoxHeight), Vector2(hitBoxOffsetX, hitBoxOffsetY), 0, id, hp, speed, sight, hearing, massDensity, attackMultiplier, strength, jumpStrength, courage, exp, Enemy::MELEE, aiFlags, abilities, sounds)));
			
			CrusadeUtil::debug(std::to_string(m_enemies.size()));
			CrusadeUtil::debug("\n");
		}
		else if (aiType.compare(Enemy::RANGED)==0){
			m_enemies.insert(m_enemies.end(), std::pair<UINT, Enemy*>((UINT)id, new MeleeEnemy("ENEMY", name, Vector2(0, 0), Vector2(textureWidth, textureHeight), Vector2(hitBoxWidth, hitBoxHeight), Vector2(hitBoxOffsetX, hitBoxOffsetY), 0, id, hp, speed, sight, hearing, massDensity, attackMultiplier, strength, jumpStrength, courage, exp, Enemy::RANGED, aiFlags, abilities, sounds)));

			CrusadeUtil::debug(std::to_string(m_enemies.size()));
			CrusadeUtil::debug("\n");
		}
		else if (aiType.compare(Enemy::MAGE)==0){
			m_enemies.insert(m_enemies.end(), std::pair<UINT, Enemy*>((UINT)id, new MageEnemy("ENEMY", name, Vector2(0, 0), Vector2(textureWidth, textureHeight), Vector2(hitBoxWidth, hitBoxHeight), Vector2(hitBoxOffsetX, hitBoxOffsetY), 0, id, hp, speed, sight, hearing, massDensity, attackMultiplier, strength, jumpStrength, courage, exp, Enemy::MAGE, aiFlags, abilities, sounds)));

			CrusadeUtil::debug(std::to_string(m_enemies.size()));
			CrusadeUtil::debug("\n");
		}
		loadEnemyAnimations(id, path);

		enemy = enemy->next_sibling();
	} while (enemy);
	CrusadeUtil::debug(std::to_string(m_enemies.size()));
	CrusadeUtil::debug("\n");
	return true;
}

//Load all of the animations for the Enemy with the specified id
void EnemyManager::loadEnemyAnimations(UINT id, std::tr2::sys::path anim_path){
	using namespace std;
	using namespace std::tr2::sys;
	anim_path = relativePathAdjust / anim_path;
	path idle = anim_path;
	path walk = anim_path;
	path jump = anim_path;
	path death = anim_path;
	path cast = anim_path;
	path slash = anim_path;

	// the /= operator is overloaded to add the string on to the path with a prepended '/'
	idle /= "ANIM_IDLE";
	walk /= "ANIM_WALK";
	jump /= "ANIM_JUMP";
	death /= "ANIM_DEATH";
	cast /= "ANIM_CAST";
	slash /= "ANIM_SLASH";

	std::vector<wchar_t*> wPaths;
	int count = 1;
	int start = 1;

	// LOAD IDLE ANIMATION
	for (directory_iterator i(idle), end; i != end; i++){
		path extension = i->path().extension();
		if (i->path().extension().compare(".png") == 0)
		{
			count++;
			CrusadeUtil::debug(to_string(count));
			wchar_t *buffer;
			CrusadeUtil::s_to_ws(i->path(), &buffer);
			wPaths.insert(wPaths.end(), buffer);
		}
		else{
			CrusadeUtil::debug("\nNot a png: ");
			CrusadeUtil::debug(i->path());
		}
			
	}
	m_indexSizeMap[id].insert(m_indexSizeMap[id].end(), std::pair<UINT, UINT>(start, count-start));
	start = count;

	// LOAD WALK ANIMATION
	for (directory_iterator i(walk), end; i != end; i++){
		path extension = i->path().extension();
		if (i->path().extension().compare(".png") == 0)
		{
			count++;
			CrusadeUtil::debug(to_string(count));
			wchar_t *buffer;
			CrusadeUtil::s_to_ws(i->path(), &buffer);
			wPaths.insert(wPaths.end(), buffer);
		}
		else{
			CrusadeUtil::debug("\nNot a png: ");
			CrusadeUtil::debug(i->path());
		}
	}
	m_indexSizeMap[id].insert(m_indexSizeMap[id].end(), std::pair<UINT, UINT>(start, count-start));
	start = count;

	// LOAD JUMP ANIMATION
	for (directory_iterator i(jump), end; i != end; i++){
		path extension = i->path().extension();
		if (i->path().extension().compare(".png") == 0)
		{
			count++;
			CrusadeUtil::debug(to_string(count));
			wchar_t *buffer;
			CrusadeUtil::s_to_ws(i->path(), &buffer);
			wPaths.insert(wPaths.end(), buffer);
		}
		else{
			CrusadeUtil::debug("\nNot a png: ");
			CrusadeUtil::debug(i->path());
		}
	}
	m_indexSizeMap[id].insert(m_indexSizeMap[id].end(), std::pair<UINT, UINT>(start, count-start));
	start = count;

	// LOAD DEATH ANIMATION
	for (directory_iterator i(death), end; i != end; i++){
		path extension = i->path().extension();
		if (i->path().extension().compare(".png") == 0)
		{
			count++;
			CrusadeUtil::debug(to_string(count));
			wchar_t *buffer;
			CrusadeUtil::s_to_ws(i->path(), &buffer);
			wPaths.insert(wPaths.end(), buffer);
		}
		else{
			CrusadeUtil::debug("\nNot a png: ");
			CrusadeUtil::debug(i->path());
		}
	}
	m_indexSizeMap[id].insert(m_indexSizeMap[id].end(), std::pair<UINT, UINT>(start, count-start));
	start = count;

	// LOAD CAST ANIMATION
	for (directory_iterator i(cast), end; i != end; i++){
		path extension = i->path().extension();
		if (i->path().extension().compare(".png") == 0)
		{
			count++;
			CrusadeUtil::debug(to_string(count));
			wchar_t *buffer;
			CrusadeUtil::s_to_ws(i->path(), &buffer);
			wPaths.insert(wPaths.end(), buffer);
		}
		else{
			CrusadeUtil::debug("\nNot a png: ");
			CrusadeUtil::debug(i->path());
		}

	}
	m_indexSizeMap[id].insert(m_indexSizeMap[id].end(), std::pair<UINT, UINT>(start, count - start));
	start = count;

	// LOAD SLASH ANIMATION
	for (directory_iterator i(slash), end; i != end; i++){
		path extension = i->path().extension();
		if (i->path().extension().compare(".png") == 0)
		{
			count++;
			CrusadeUtil::debug(to_string(count));
			wchar_t *buffer;
			CrusadeUtil::s_to_ws(i->path(), &buffer);
			wPaths.insert(wPaths.end(), buffer);
		}
		else{
			CrusadeUtil::debug("\nNot a png: ");
			CrusadeUtil::debug(i->path());
		}

	}
	m_indexSizeMap[id].insert(m_indexSizeMap[id].end(), std::pair<UINT, UINT>(start, count - start));
	start = count;

	m_texPaths[id] = wPaths;

	ID3D11ShaderResourceView *tempView;
	// offset of one for extra buffer verts that wont be rendered
	TextureLoader::LoadSeqTextureArray(D3DManager::Instance()->getDevice(), D3DManager::Instance()->getContext(), wPaths, 1, &tempView);

	m_texArrays[id] = tempView;

	//CrusadeUtil::debug(to_string(wPaths.size()));


}

bool EnemyManager::changeEnemyList(std::string path){
	this->m_xmlPath = path;
	return loadEnemyData();
}

//Populates the listbox in the editor with the current List of enemies assigned to this enemy factory
void EnemyManager::populateListbox(HWND listbox_hwnd){
	SendMessage(listbox_hwnd, LB_RESETCONTENT, 0, 0);
	int pos = (int)SendMessage(listbox_hwnd, LB_ADDSTRING, 0, (LPARAM)(L"ERASER"));
	if (!m_instance){
		return;
	}
	std::map<UINT, Enemy*>::iterator it;
	for (it = m_enemies.begin(); it != m_enemies.end(); it++){
		std::string name=m_enemies.at(it->first)->getName();
		wchar_t *temp;
		CrusadeUtil::s_to_ws(name, &temp);
		int pos = (int)SendMessage(listbox_hwnd, LB_ADDSTRING, 0, (LPARAM)(temp));
		SendMessage(listbox_hwnd, LB_SETITEMDATA, pos, (LPARAM)it->first);
		delete[] temp;
	}
}

/*Provide a UINT id and get an Enemy pointer to the coresponding enemy unit.
 Will point to the actual enemy sub type so that you get the correct mapping.
 Technically possible to map to a different subtype so that the AI for the enemy seems to be very off (mage the bum rushes like a fighter would fo instance)
 Don't forget to check the type and then cast to the correct subtype of enemy before creating a new one based on that template!
 Useage:
 Enemy* enemy; //NOTE also possible to pass the correct enemy type if you already know what it is
 if(!EnemyManager->createEnemy(2, &enemy)){
	//well shit, the requested enemy doesn't exist :/
 }else{
	//Yay it works, now cast to appropriate type and you is good!
 }

*/
bool EnemyManager::createEnemy(UINT id, _Out_ Enemy** enemy_out){
	auto search = m_enemies.find(id);
	if (search != m_enemies.end()) {
		Enemy* enemy= search->second;
		if (enemy->getType().compare(Enemy::MELEE)==0){
			MeleeEnemy* ptr = ((MeleeEnemy*)enemy)->clone();
			m_renderSet.insert(ptr);
			*enemy_out = ptr;
		}
		else if (enemy->getType().compare(Enemy::RANGED)==0){
			MeleeEnemy* ptr = ((MeleeEnemy*)enemy)->clone();
			m_renderSet.insert(ptr);
			*enemy_out = ptr;
		}
		else if (enemy->getType().compare(Enemy::MAGE)==0){
			MeleeEnemy* ptr = ((MeleeEnemy*)enemy)->clone();
			m_renderSet.insert(ptr);
			*enemy_out = ptr;
		}

		return true;
	}
	return false;
}

bool EnemyManager::deleteEnemyAt(float worldx, float worldy){
	std::multiset<Enemy*, ptr_less>::iterator it;
	for (it = m_renderSet.begin(); it != m_renderSet.end(); it++){
		if (CrusadeUtil::isPointIntersecting(Vector2(worldx, worldy), (*it)->getPosition(), (*it)->getSize())){
			delete (*it);
			m_renderSet.erase(it++);
			return true;
		}
	}
	return false;
}

/*Delete Enemy by pointer*/
bool EnemyManager::deleteEnemy(Enemy* enemy){
	std::multiset<Enemy*, ptr_less>::iterator it;
	for (it = m_renderSet.begin(); it != m_renderSet.end(); it++){
		if ((*it)==enemy){
			delete (*it);
			m_renderSet.erase(it++);
			return true;
		}
	}
	return false;
}

void EnemyManager::deleteAllEnemies(){
	std::multiset<Enemy*, ptr_less>::iterator it;
	for (it = m_renderSet.begin(); it != m_renderSet.end(); it++){
		delete (*it);
	}
	m_renderSet.clear();
}

bool EnemyManager::EnemySetIsLoaded(){
	return (m_enemies.size() != 0);
}

UINT EnemyManager::getRenderSetSize(){
	return m_renderSet.size();
}

std::string EnemyManager::getXMLPath(){
	return m_xmlPath;
}

std::multiset<Enemy*, EnemyManager::ptr_less>* EnemyManager::getRenderSet(){
	return &m_renderSet;
}

void EnemyManager::setDeviceAndContextHandle(ID3D11Device *device, ID3D11DeviceContext *context){
	h_device = device;
	h_context = context;
}

UINT EnemyManager::getAnimSize(UINT id, Enemy::ANIMATION_TYPE type){
	return m_indexSizeMap.at(id).at(type).second;
}

UINT EnemyManager::getAnimIndex(UINT id, Enemy::ANIMATION_TYPE type){
	return m_indexSizeMap[id].at(type).first;
}

bool EnemyManager::isWorldEditMode(){
	return m_worldEditMode;
}

/*True: Only physics occurs.
 False: Enemies move around. Save temp world before this?*/
void EnemyManager::setWorldEditMode(bool mode){
	m_worldEditMode = mode;
}

void EnemyManager::pause(){
	m_paused = true;
}

void EnemyManager::resume(){
	m_paused = false;
}

/*A sound should be defined as a set of ordered pairs delimited by semicolons.
Example: (ogre_ow_1, .333); (ogre_ow_2, .333); (ogre_ow_3, .334)*/
map<float, string> EnemyManager::parseSounds(string csv){
	map<float, string> sounds;
	if (csv.length() < 8){
		sounds.emplace(1.0f, "NO AUDIO");
		return sounds;
	}
	csv.erase(remove_if(csv.begin(), csv.end(), isspace), csv.end());
	vector<string> pairs = CrusadeUtil::split(csv, ';');
	float chance = 0.0f;
	for (string s : pairs){
		s = s.substr(1, s.length() - 2);
		vector<string> parts = CrusadeUtil::split(s, ',');
		chance += stof(parts[1]);
		sounds.emplace(chance, parts[0]);
	}
	return sounds;
}