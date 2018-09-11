#include "pch.h"
#include "LoadingScreen.h"

LoadingScreen::LoadingScreen(string text, float width, float height){
	m_text = text;
	m_screenWidth = width;
	m_screenHeight = height;

	initResources();
}

LoadingScreen::~LoadingScreen(){
	m_loadBatch.reset(nullptr);
	m_font.reset(nullptr);
	m_backgroundSRV.Reset();
	h_d3d = nullptr;
}

void LoadingScreen::initResources(){
	m_loadBatch.reset(new SpriteBatch(D3DManager::Instance()->getContext()));
	m_font.reset(new SpriteFont(D3DManager::Instance()->getDevice(), L"../img/courier.spritefont"));
	h_d3d = D3DManager::Instance();
	DX::ThrowIfFailed(CreateWICTextureFromFile(h_d3d->getDevice(),
		L"../img/Backgrounds/loadingScreen.png",
		nullptr,
		m_backgroundSRV.ReleaseAndGetAddressOf()));
}

void LoadingScreen::setText(string text){
	m_text = text;
}

string LoadingScreen::getText(){
	return m_text;
}

void LoadingScreen::dispose(){
	
}

void LoadingScreen::hide(){

}
void LoadingScreen::pause(){

}

void LoadingScreen::render(float dt){
	RECT screenRc;
	screenRc.top = 0;
	screenRc.left = 0;
	screenRc.bottom = (long)m_screenHeight;
	screenRc.right = (long)m_screenWidth;


	wchar_t* text;
	CrusadeUtil::s_to_ws(m_text, &text);
	Vector2 textSize = m_font->MeasureString(text);
	m_loadBatch->Begin();
	m_loadBatch->Draw(m_backgroundSRV.Get(), screenRc);
	m_font->DrawString(m_loadBatch.get(), text, Vector2(m_screenWidth*.5f - textSize.x/2.0f, m_screenHeight*.5f - textSize.y/2), Colors::White, 0.0f, Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));
	m_loadBatch->End();
}

void LoadingScreen::resize(int width, int height){
	m_screenWidth = (float)width;
	m_screenHeight = (float)height;
}

void LoadingScreen::resume(){

}

void LoadingScreen::show(){
	RECT screenRc;
	screenRc.top = 0;
	screenRc.left = 0;
	screenRc.bottom = (long)m_screenHeight;
	screenRc.right = (long)m_screenWidth;
	m_loadBatch->Begin();
	m_loadBatch->Draw(m_backgroundSRV.Get(), screenRc);
	m_loadBatch->End();
}