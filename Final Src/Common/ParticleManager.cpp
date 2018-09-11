#include "pch.h"
#include "ParticleManager.h"
#include "CrusadeVertexTypes.h"

ParticleManager* ParticleManager::m_instance = nullptr;

ParticleManager* ParticleManager::Instance(){
	return m_instance;
}

void ParticleManager::Initialize(){
	m_instance = new ParticleManager();
}

ParticleManager::ParticleManager(){
	m_stagingBuff = nullptr;
	m_streamTo = nullptr;
	m_drawFrom = nullptr;
	m_paused = false;
	m_emitterSetChanged = false;
	m_first = true;
	m_updateSize = 0;
	m_numParticles = 0;
	m_timer = 0;

	ID3D11Device *device = D3DManager::Instance()->getDevice();

	CreateResources(device);
}

ParticleManager::~ParticleManager(){
	for (auto it = m_emitterSet.begin(); it != m_emitterSet.end(); it++){
		delete (*it);
	}
	m_emitterSet.clear();
	m_randTexRV.Reset();
	m_randTex.Reset();
	m_SOQuery.Reset();
	m_stagingBuff->Release();
	m_streamTo->Release();
	m_drawFrom->Release();
}

void ParticleManager::Update(float dt){
	ID3D11Device *device = D3DManager::Instance()->getDevice();
	ID3D11DeviceContext *context = D3DManager::Instance()->getContext();
	if (!m_paused){
		m_timer += dt;

		D3D11_MAPPED_SUBRESOURCE mappedStaging;
		// update the staging buffer with any changes to the emitters
		context->Map(m_stagingBuff, 0, D3D11_MAP_READ_WRITE, 0, &mappedStaging);
		int i = 0;
		for (int j = 0; j < 10; j++){
			CrusadeVertexTypes::Particle *p = reinterpret_cast<CrusadeVertexTypes::Particle*>(mappedStaging.pData);
			int a = p[j].type;
		}
		CrusadeVertexTypes::Particle *p = reinterpret_cast<CrusadeVertexTypes::Particle*>(mappedStaging.pData);
		for (auto it = m_emitterSet.begin(); it != m_emitterSet.end();){
			(*it)->update(dt);

			p[i].pos = (*it)->getPosition();
			p[i].vel = (*it)->getVelocity();
			p[i].size = (*it)->getSize();
			p[i].emitInterval = (*it)->getInterval();
			p[i].timer = (*it)->getTimer();
			p[i].type = (*it)->getType();
			p[i].etype = (*it)->getEmitType();
			p[i].texIndex = (*it)->getTextureIndex();
			it++;
			i++;
		}

		context->Unmap(m_stagingBuff, 0);

		// prepend the stagingbuffer on to the drawfrom buffer
		D3D11_BOX srcBox{};
		srcBox.left = 0;
		srcBox.right = m_numParticles * sizeof(CrusadeVertexTypes::Particle);
		srcBox.top = 0;
		srcBox.bottom = 1;
		srcBox.front = 0;
		srcBox.back = 1;
		context->CopySubresourceRegion(m_stagingBuff, 0, m_emitterSet.size()*sizeof(CrusadeVertexTypes::Particle), 0, 0, m_drawFrom, 0, &srcBox);
		// debug!
		context->Map(m_stagingBuff, 0, D3D11_MAP_READ_WRITE, 0, &mappedStaging);
		for (int j = 0; j < 10; j++){
			CrusadeVertexTypes::Particle *p = reinterpret_cast<CrusadeVertexTypes::Particle*>(mappedStaging.pData);
			int a = p[j].type;
		}
		context->Unmap(m_stagingBuff, 0);


		context->CopyResource(m_drawFrom, m_stagingBuff);

		//call GPU updatefunction. Resulting output will only have particles
		// update the constant buffers of our shader
		ParticleEffect *pFX = CrusadeEffects::ParticleFX;
		pFX->SetGravity(Vector4(0.0f, -10.0f, 0.0f, 0.0f));
		pFX->SetRandTexture(m_randTexRV.Get());
		pFX->SetElaspedTime(dt);
		pFX->SetGlobalTime(m_timer);
		pFX->SetBloodSize(.04f);

		ID3D11Buffer* pBuffers[1];
		pBuffers[0] = m_drawFrom;
		UINT stride[1] = { sizeof(CrusadeVertexTypes::Particle) };
		UINT offset[1] = { 0 };

		context->IASetVertexBuffers(0, 1, pBuffers, stride, offset);
		context->IASetInputLayout(InputLayouts::ParticleVertexLayout);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		pBuffers[0] = m_streamTo;
		context->SOSetTargets(1, pBuffers, offset);

		//draw
		D3DX11_TECHNIQUE_DESC desc;
		pFX->m_StreamTech->GetDesc(&desc);

		context->Begin(m_SOQuery.Get());

		for (UINT p = 0; p < desc.Passes; p++)
		{
			pFX->m_StreamTech->GetPassByIndex(p)->Apply(0, context);
			context->Draw(m_emitterSet.size() + m_numParticles, 0);
		}

		context->End(m_SOQuery.Get());
		D3D11_QUERY_DATA_SO_STATISTICS stats = { 0 };
		int s = sizeof(D3D11_QUERY_DATA_SO_STATISTICS);
		while (context->GetData(m_SOQuery.Get(), &stats, sizeof(D3D11_QUERY_DATA_SO_STATISTICS), 0) != S_OK){
			// wait until the stat data is available
		}

		m_numParticles = (int)stats.NumPrimitivesWritten;

		//Swap buffers
		ID3D11Buffer* temp = m_drawFrom;
		m_drawFrom = m_streamTo;
		m_streamTo = temp;

		m_first = false;

		pBuffers[0] = nullptr;
		context->SOSetTargets(1, pBuffers, offset);

		context->GSSetShader(nullptr, nullptr, 0);

		// delete the one time emitters
		for (auto it = m_emitterSet.begin(); it != m_emitterSet.end();){
			if ((*it)->getInterval() == -1.0f)
			{
				delete (*it);
				it = m_emitterSet.erase(it);
			}
			else
			{
				it++;
			}
		}

	}
}

void ParticleManager::Draw(Matrix worldViewProj){
	ID3D11Device *device = D3DManager::Instance()->getDevice();
	ID3D11DeviceContext *context = D3DManager::Instance()->getContext();

	ID3D11Buffer* pBuffers[1] = { m_drawFrom };
	UINT stride[1] = { sizeof(CrusadeVertexTypes::Particle) };
	UINT offset[1] = { 0 };
	
	context->IASetVertexBuffers(0, 1, pBuffers, stride, offset);
	context->IASetInputLayout(InputLayouts::ParticleVertexLayout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// set the constant buffers of our shader
	ParticleEffect *pFX = CrusadeEffects::ParticleFX;
	pFX->SetWorldViewProj(worldViewProj);
	pFX->SetDiffuseMaps(h_tileSetSRV);

	//draw
	D3DX11_TECHNIQUE_DESC desc;
	pFX->m_DrawParticleTech->GetDesc(&desc);

	for (UINT p = 0; p < desc.Passes; p++)
	{
		pFX->m_DrawParticleTech->GetPassByIndex(p)->Apply(0, context);
		context->Draw(m_numParticles, 0);
	}

	context->GSSetShader(nullptr, nullptr, 0);
}

void ParticleManager::setTileSetTextureArray(ID3D11ShaderResourceView* arr){
	h_tileSetSRV = arr;
}

void ParticleManager::Pause(){
	m_paused = true;
}

void ParticleManager::Resume(){
	m_paused = false;
}

Emitter* ParticleManager::CreateEmitter(Vector2 pos, Vector2 size, Vector2 direction, UINT etype, float emitInterval, UINT m_texIndex){
	Emitter *ptr;
	// ensure the emitInterval is greater than or equal to 0.001
	float interval = 0.001f;
	if (emitInterval > 0.001f)
		interval = emitInterval;
	if (etype == PT_BLOCK)
		ptr = new Emitter(Vector3(pos.x, pos.y, USE_TEXTURE), direction, size, interval, (UINT)PT_EMITTER, etype, m_texIndex);
	else
		ptr = new Emitter(Vector3(pos.x, pos.y, USE_COLOR_ONLY), direction, size, interval, (UINT)PT_EMITTER, etype, m_texIndex);

	m_emitterSet.insert(ptr);
	return ptr;
}

void ParticleManager::EmitParticles(Vector2 pos, Vector2 size, Vector2 direction, UINT etype, UINT m_texIndex){
	Emitter *ptr;
	if (etype == PT_BLOCK)
		ptr = new Emitter(Vector3(pos.x, pos.y, USE_TEXTURE), direction, size, -1.0f, (UINT)PT_EMITTER, etype, m_texIndex);
	else
		ptr = new Emitter(Vector3(pos.x, pos.y, USE_COLOR_ONLY), direction, size, -1.0f, (UINT)PT_EMITTER, etype, m_texIndex);

	m_emitterSet.insert(ptr);
}

void ParticleManager::DeleteEmitter(Emitter* emitter){
	for(auto it = m_emitterSet.begin(); it != m_emitterSet.end(); it++){
		if ((*it) == emitter){
			delete (*it);
			m_emitterSet.erase(it);
			break;
		}
	}

}

void ParticleManager::Clear(){
	m_first = true;
	m_updateSize = 0;
	m_numParticles = 0;
	m_timer = 0;
	for (auto it = m_emitterSet.begin(); it != m_emitterSet.end(); it++){
		delete (*it);
	}
	m_emitterSet.clear();
}

void ParticleManager::CreateResources(ID3D11Device *device){

	if (m_streamTo)
		m_streamTo->Release();
	if (m_drawFrom)
		m_drawFrom->Release();
	if (m_stagingBuff)
		m_stagingBuff->Release();

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(CrusadeVertexTypes::Particle) * MAX_PARTICLES;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
	vbd.CPUAccessFlags = 0;
	vbd.StructureByteStride = sizeof(CrusadeVertexTypes::Particle);
	vbd.MiscFlags = 0;
	// our drawing and streaming buffers are exactly the same
	DX::ThrowIfFailed(device->CreateBuffer(&vbd, nullptr, &m_streamTo));
	DX::ThrowIfFailed(device->CreateBuffer(&vbd, nullptr, &m_drawFrom));

	D3D11_BUFFER_DESC sbd;
	sbd.Usage = D3D11_USAGE_STAGING;
	sbd.ByteWidth = sizeof(CrusadeVertexTypes::Particle) * MAX_PARTICLES;
	sbd.BindFlags = 0;
	sbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	sbd.StructureByteStride = sizeof(CrusadeVertexTypes::Particle);
	sbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	DX::ThrowIfFailed(device->CreateBuffer(&sbd, nullptr, &m_stagingBuff));

	//get the number of verts
	D3D11_QUERY_DESC queryDesc;
	queryDesc.Query = D3D11_QUERY_SO_STATISTICS;
	queryDesc.MiscFlags = 0;

	DX::ThrowIfFailed(device->CreateQuery(&queryDesc, m_SOQuery.ReleaseAndGetAddressOf()));

	CreateRandomTexture(device);
}

void ParticleManager::CreateRandomTexture(ID3D11Device *device){

	int numRandVals = 1024;
	srand((UINT)time(nullptr));

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = new float[numRandVals * 4];
	data.SysMemPitch = numRandVals * 4 * sizeof(float);
	data.SysMemSlicePitch = numRandVals * 4 * sizeof(float);
	for (int i = 0; i < numRandVals * 4; i++){
		((float*)data.pSysMem)[i] = float((rand() % 10000) - 5000);
	}

	D3D11_TEXTURE1D_DESC dstex;
	dstex.Width = numRandVals;
	dstex.MipLevels = 1;
	dstex.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	dstex.Usage = D3D11_USAGE_DEFAULT;
	dstex.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	dstex.CPUAccessFlags = 0;
	dstex.MiscFlags = 0;
	dstex.ArraySize = 1;
	DX::ThrowIfFailed(device->CreateTexture1D(&dstex, &data, m_randTex.ReleaseAndGetAddressOf()));
	delete[] data.pSysMem;
	data.pSysMem = nullptr;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = dstex.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	srvDesc.Texture2D.MipLevels = dstex.MipLevels;
	DX::ThrowIfFailed(device->CreateShaderResourceView(m_randTex.Get(), &srvDesc, m_randTexRV.ReleaseAndGetAddressOf()));

}