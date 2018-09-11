#pragma once

#include "pch.h"
#include "Emitter.h"

using namespace DirectX;

class ParticleManager{

public:
	struct ptr_less{
		template<class T>
		bool operator()(const T& left, const T& right) const{
			return ((*left) < (*right));
		}

	};
	static const int MAX_PARTICLES = 30000;

	static ParticleManager* Instance();
	static void Initialize();
	~ParticleManager();

	void setTileSetTextureArray(ID3D11ShaderResourceView* arr);

	void Pause();
	void Resume();

	void Update(float dt);
	void Draw(Matrix worldViewProj);

	Emitter* CreateEmitter(Vector2 pos, Vector2 size, Vector2 direction, UINT etype, float emitInterval, UINT m_texIndex = 0);
	void EmitParticles(Vector2 pos, Vector2 size, Vector2 direction, UINT etype, UINT m_texIndex = 0);
	void DeleteEmitter(Emitter*);


	void Clear();

	void CreateResources(ID3D11Device *device);

private:
	static ParticleManager* m_instance;
	ParticleManager();

	void CreateRandomTexture(ID3D11Device *device);

	std::multiset<Emitter*, ptr_less> m_emitterSet;

	Microsoft::WRL::ComPtr<ID3D11Texture1D>				m_randTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_randTexRV;

	Microsoft::WRL::ComPtr<ID3D11Query>					m_SOQuery;

	ID3D11ShaderResourceView*							h_tileSetSRV;

	ID3D11Buffer*			m_stagingBuff;
	ID3D11Buffer*			m_streamTo;
	ID3D11Buffer*			m_drawFrom;

	float m_timer;
	int m_updateSize;
	int m_numParticles;
	bool m_paused;
	bool m_emitterSetChanged;
	bool m_first;

};