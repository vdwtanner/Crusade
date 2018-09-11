#include "pch.h"
#include "AbilityManager.h"

AbilityManager* AbilityManager::m_instance = NULL;

AbilityManager* AbilityManager::Instance(){
	if (!m_instance){
		m_instance = new AbilityManager();
		m_instance->m_xmlPath = "";
	}
	return m_instance;
}

AbilityManager* AbilityManager::Instance(LPWSTR path){
	if (!m_instance){
		m_instance = new AbilityManager(path);
	}
	else if (m_instance->m_xmlPath.compare("") == 0)
	{
		delete m_instance;
		m_instance = new AbilityManager(path);
	}
	return m_instance;
}

AbilityManager* AbilityManager::Instance(std::string path){
	if (!m_instance){
		m_instance = new AbilityManager(path);
	}
	else if (m_instance->m_xmlPath.compare("") == 0)
	{
		delete m_instance;
		m_instance = new AbilityManager(path);
	}
	return m_instance;
}

AbilityManager::AbilityManager(){

	m_paused = false;

	CrusadeVertexTypes::TexturedVertex temp[MAX_ABILITIES];

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(CrusadeVertexTypes::TexturedVertex) * MAX_ABILITIES;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &temp;

	DX::ThrowIfFailed(D3DManager::Instance()->getDevice()->CreateBuffer(&vbd, &vinitData, m_VB.ReleaseAndGetAddressOf()));

}

AbilityManager::~AbilityManager(){
	m_instance = nullptr;
	// Clean up m_enemies
	std::map<UINT, Ability*>::iterator it;
	for (it = m_abilities.begin(); it != m_abilities.end(); it++){
		delete it->second;
	}

	// Clean up m_renderSet
	std::set<Ability*, ptr_less>::iterator renIt;
	for (renIt = m_renderSet.begin(); renIt != m_renderSet.end(); renIt++){
		delete (*renIt);
	}

	// Clean up m_texPaths
	std::map<UINT, std::vector<wchar_t*>>::iterator pathIt;
	for (pathIt = m_texPaths.begin(); pathIt != m_texPaths.end(); pathIt++){
		for (UINT i = 0; i < pathIt->second.size(); i++){
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

AbilityManager::AbilityManager(std::string path) : AbilityManager() {
	this->m_xmlPath = path;
	loadAbilityData();
}

AbilityManager::AbilityManager(LPWSTR path) : AbilityManager() {
	this->m_xmlPath = CW2A(path);
	loadAbilityData(path);
}

void AbilityManager::renderAbilities(){
	UINT stride = sizeof(CrusadeVertexTypes::TexturedVertex);
	UINT offset = 0;

	D3D11_MAPPED_SUBRESOURCE mappedVB;
	D3DManager::Instance()->getContext()->Map(m_VB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB);

	CrusadeVertexTypes::TexturedVertex *v = reinterpret_cast<CrusadeVertexTypes::TexturedVertex*>(mappedVB.pData);
	std::multiset<Ability*, ptr_less>::iterator it;
	int i = 0;
	for (it = m_renderSet.begin(); it != m_renderSet.end(); it++){
		v[i].Pos = Vector3((*it)->getRenderPosition().x, (*it)->getRenderPosition().y, .1f);
		v[i].Rotation = (*it)->getRotation();
		v[i].Size = (*it)->getRenderSize();
		v[i].TexIndex = (*it)->getTexIndex();
		v[i].Color = (*it)->getColor();
		i++;
	}

	D3DManager::Instance()->getContext()->Unmap(m_VB.Get(), 0);

	// now draw each type
	std::map<UINT, std::pair<UINT, UINT>>::iterator rendIt;
	UINT count = 0;
	UINT start = 0;

	// for each type
	for (rendIt = m_indexSizeMap.begin(); rendIt != m_indexSizeMap.end(); rendIt++){
		// tell the D3D pipeline how our verts are formatted
		D3DManager::Instance()->getContext()->IASetInputLayout(InputLayouts::TexturedVertexLayout);
		// We want to draw points because our geometry shader expands them to quads.
		D3DManager::Instance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		// Use our vertex buffer
		D3DManager::Instance()->getContext()->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &stride, &offset);
		// Use the array of textures for this type of ability (rendIt->first is the ID of the type we're drawing)
		CrusadeEffects::TextureFX->SetDiffuseMaps(m_texArrays[rendIt->first]);
		// Set the color correction.
		CrusadeEffects::TextureFX->SetColorCorrect(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		// apply the shaders we're using
		CrusadeEffects::TextureFX->m_SpriteTech->GetPassByIndex(0)->Apply(0, D3DManager::Instance()->getContext());

		// now we have to figure out the correct values of count and start for this loop
		// the verts we're drawing are grouped together by type because of our m_renderSet multiset variable
		// like so,
		/*
			VERT_BUFFER = { FIREBALL_VERT, FIREBALL_VERT, FIREBALL_VERT, FIREBALL_VERT, MISSLE_VERT, MISSLE_VERT, HEAL_VERT }
		
			for the first iteration in this example, we need to draw fireballs. The start position would be 0,
			and the count would be the number of fireballs we want to draw, in this case, 4

			in the next iteration, we need to draw the missles. The start position would be equal to the previous count: 4,
			and the count would be 2.

			in the final iteration, we need to draw the heal effects. The start position would be equal to the cumulative previous counts: 7
			and the count would be 1
		*/
		// We can see from this example that for each iteration, we need to iterate our renderset and increment start until we
		// hit the first ability with the same id as the id we're looking for, so until ability->id == rendIt->first

		// Then, we continue iterating our renderset and begin incrementing count instead of start, until ability->id != rendIt->first

		std::multiset<Ability*, ptr_less>::iterator setIt;
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
			D3DManager::Instance()->getContext()->Draw(count, start);

		count = 0;
		start = 0;
	}
}

void AbilityManager::stepAbilities(float dt){
	if (!m_paused){
		std::multiset<Ability*, ptr_less>::iterator it;
		for (it = m_renderSet.begin(); it != m_renderSet.end();){
			if (!(*it)->initialized()){
				(*it)->initSensorBody();
				(*it)->onCast();
			}
			(*it)->step(dt);
			if ((*it)->isDead())
			{
				delete (*it);
				it = m_renderSet.erase(it);
			}
			else
			{
				it++;

			}
		}
	}
}

void AbilityManager::interpolate(float alpha){
	std::multiset<Ability*, ptr_less>::iterator it;
	for (it = m_renderSet.begin(); it != m_renderSet.end(); it++){
		if ((*it)->initialized())
			(*it)->interpolate(alpha);
	}
}


bool AbilityManager::loadAbilityData(){
	wchar_t *w_xmlPath;
	CrusadeUtil::s_to_ws(m_xmlPath, &w_xmlPath);
	bool ret = loadAbilityData((LPWSTR)w_xmlPath);
	delete[] w_xmlPath;
	return ret;
}

bool AbilityManager::loadAbilityData(LPWSTR path){
	std::tr2::sys::path filePath = std::tr2::sys::path(CW2A(path));
	relativePathAdjust = filePath.remove_filename();
	xml_document<> doc;
	std::string xml = CrusadeUtil::loadFileAsString(path);
	const char *c_xml = xml.c_str();
	doc.parse<0>((char*)c_xml);
	xml_node<> *ability = doc.first_node()->first_node("Abilities");
	if (!ability){
		return false;
	}
	do{
		int id = std::stoi(ability->first_node("ID")->value());
		std::string name = ability->first_node("Name")->value();
		std::string anim_path = ability->first_node("Path")->value();

		Ability::AbilityDescription desc;
		float texWidth = std::stof(ability->first_node("TextureWidth")->value());
		float texHeight = std::stof(ability->first_node("TextureHeight")->value());
		desc.texSize.x = texWidth;
		desc.texSize.y = texHeight;

		float hitboxWidth = std::stof(ability->first_node("HitBoxWidth")->value());
		float hitboxHeight = std::stof(ability->first_node("HitBoxHeight")->value());
		desc.hitboxSize.x = hitboxWidth;
		desc.hitboxSize.y = hitboxHeight;
		float hitboxOffsetX = std::stof(ability->first_node("HitBoxOffsetX")->value());
		float hitboxOffsetY = std::stof(ability->first_node("HitBoxOffsetY")->value());
		desc.hitboxOffset.x = hitboxOffsetX;
		desc.hitboxOffset.y = hitboxOffsetY;

		desc.animSpeed = std::stof(ability->first_node("AnimSpeed")->value());
		desc.animLoop = std::stoi(ability->first_node("AnimLoop")->value()) > 0;

		desc.damage = std::stoi(ability->first_node("Damage")->value());
		desc.duration = std::stof(ability->first_node("Duration")->value());
		desc.range = std::stof(ability->first_node("Range")->value());
		desc.speed = std::stof(ability->first_node("Speed")->value());
		desc.knockback = std::stof(ability->first_node("Knockback")->value());
		desc.mana = std::stoi(ability->first_node("Mana")->value());
		desc.cooldown = std::stof(ability->first_node("Cooldown")->value());
		desc.scaleFromCenter = std::stoi(ability->first_node("ScaleFromCenter")->value()) > 0;
		if (ability->first_node("Description")){
			desc.description = ability->first_node("Description")->value();
		}
		else{
			desc.description = "No description found.";
		}
		if (ability->first_node("IconPath")){
			desc.iconPath = ability->first_node("IconPath")->value();
		}
		else{
			desc.iconPath = "NO ICON";
		}
		if (ability->first_node("CastAnim")){
			desc.castAnim = ability->first_node("CastAnim")->value();
		}
		else{
			desc.castAnim = "CAST";
		}

		std::string ability_class = ability->first_node("Class")->value();

		if (ability_class.compare("FIREBALL") == 0){
			m_abilities[id] = new Fireball(id, "ABILITY", name, &desc);
			m_abilities[id]->setOwner(nullptr);
		}
		else if (ability_class.compare("HOLYFIRE") == 0){
			m_abilities[id] = new HolyFire(id, "ABILITY", name, &desc);
			m_abilities[id]->setOwner(nullptr);
		}
		else if (ability_class.compare("HOLYMISSILE") == 0){
			m_abilities[id] = new HolyMissile(id, "ABILITY", name, &desc);
			m_abilities[id]->setOwner(nullptr);
		}
		else if (ability_class.compare("SLASH") == 0){
			m_abilities[id] = new Slash(id, "ABILITY", name, &desc);
			m_abilities[id]->setOwner(nullptr);
		}
		else if (ability_class.compare("HEAL") == 0){
			m_abilities[id] = new Heal(id, "ABILITY", name, &desc);
			m_abilities[id]->setOwner(nullptr);
		}
		loadAbilityAnimations(id, anim_path);


		ability = ability->next_sibling();
	} while (ability);
	//CrusadeUtil::debug(std::to_string(m_abilities.size()));
	CrusadeUtil::debug("\n");
	return true;
}

UINT AbilityManager::getAnimSize(UINT id){
	return m_indexSizeMap[id].second;
}

UINT AbilityManager::getAnimIndex(UINT id){
	return m_indexSizeMap[id].first;
}

void AbilityManager::loadAbilityAnimations(UINT id, std::tr2::sys::path anim_path){
	using namespace std;
	using namespace std::tr2::sys;

	path relativePathAdjust("../img");
	anim_path = relativePathAdjust / anim_path;

	std::vector<wchar_t*> wPaths;
	int count = 1;
	int start = 1;

	for (directory_iterator i(anim_path), end; i != end; i++){
		path extension = i->path().extension();
		if (i->path().extension().compare(".png") == 0)
		{
			count++;
			wchar_t *buffer;
			CrusadeUtil::s_to_ws(i->path(), &buffer);
			wPaths.insert(wPaths.end(), buffer);
		}
		else{
			CrusadeUtil::debug("\nNot a png: ");
			CrusadeUtil::debug(i->path());
		}

	}
	m_indexSizeMap[id] = std::pair<UINT, UINT>(start, count - start);

	m_texPaths[id] = wPaths;

	ID3D11ShaderResourceView *tempView;

	if (wPaths.size() > 0){
		TextureLoader::LoadSeqTextureArray(D3DManager::Instance()->getDevice(), D3DManager::Instance()->getContext(), wPaths, 1, &tempView);

		m_texArrays[id] = tempView;
	}

}

bool AbilityManager::castAbility(UINT id, GameObject* caster, _Out_ Ability** ability_out){
	auto search = m_abilities.find(id);
	if (search != m_abilities.end()){
		Ability* ability = search->second;
		Ability* ptr = ability->clone();
		ptr->setOwner(caster);
		m_renderSet.insert(ptr);
		*ability_out = ptr;

		return true;
	}
	return false;
}

bool AbilityManager::deleteAbility(Ability* ability){
	std::multiset<Ability*, ptr_less>::iterator it;
	for (it = m_renderSet.begin(); it != m_renderSet.end(); it++){
		if ((*it) == ability){
			delete (*it);
			m_renderSet.erase(it++);
			return true;
		}
	}
	return false;
}

void AbilityManager::deleteAllAbilities(){
	std::multiset<Ability*, ptr_less>::iterator it;
	for (it = m_renderSet.begin(); it != m_renderSet.end(); it++){
		delete (*it);
	}
	m_renderSet.clear();
}

float AbilityManager::getCooldown(UINT id){
	auto search = m_abilities.find(id);
	if (search != m_abilities.end()){
		Ability* ability = search->second;
		return ability->getCooldown();
	}
	return -1;
}

Vector2 AbilityManager::getMaxHitboxSize(UINT id){
	Vector2 maxSize = Vector2(0, 0);
	do{
		Vector2 tempSize = m_abilities[id]->getHitboxSize();
		if (tempSize.x * tempSize.y > maxSize.x*maxSize.y){
			maxSize = tempSize;
		}
	} while (m_abilities[id++]->getStagesRemaining() > 0);
	return maxSize;
}

void AbilityManager::pause(){
	m_paused = true;
}

void AbilityManager::resume(){
	m_paused = false;
}

//returns INT_MAX if the id doesn't exist (can't cast if you don't have enough mana)
int AbilityManager::getManaCost(UINT id, GameObject* caster){
	if (id <= 0){
		return INT_MAX;
	}
	return m_abilities.at(id)->getManaCost(caster);
}

Ability::AbilityDescription AbilityManager::getAbilityDescription(UINT id){
	return m_abilities.at(id)->getAbilityDesc();
}