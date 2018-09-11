#include "pch.h"
#include "InputProcessor.h"
#include "resource.h"
#include "windowsx.h"
#include <sstream>
#include "CrusadeUtil.h"
#include "TriggerManager.h"
#include <math.h>
#include <Box2DSingleton.h>

namespace{
	InputProcessor *pthis;
}

InputProcessor::InputProcessor(Game *game){
	this->h_game = game;
	this->h_worldEditor = h_game->GetWorldEditor();
	this->h_tool = h_worldEditor->getTool();
	pthis = this;
}

InputProcessor::~InputProcessor(){

}

BOOL CALLBACK InputProcessor::NewDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch (message){
	case WM_INITDIALOG:
		return true;
	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case IDOK:
			EndDialog(hwnd, TRUE);
			break;
		case IDC_NEWCREATE:
			switch (HIWORD(wParam)){
			case BN_CLICKED:
				wchar_t *widthBuff;
				wchar_t *heightBuff;
				wchar_t *layersBuff;
				wchar_t *activeBuff;
				HWND widthHwnd = GetDlgItem(hwnd, IDD_NEW_EDITWIDTH);
				HWND heightHwnd = GetDlgItem(hwnd, IDD_NEW_EDITHEIGHT);
				HWND layersHwnd = GetDlgItem(hwnd, IDD_NEW_EDITLAYERS);
				HWND activeHwnd = GetDlgItem(hwnd, IDD_NEW_EDITACTIVE);

				int widthLength = GetWindowTextLength(widthHwnd);
				int heightLength = GetWindowTextLength(heightHwnd);
				int layersLength = GetWindowTextLength(layersHwnd);
				int activeLength = GetWindowTextLength(activeHwnd);

				widthBuff = new wchar_t[widthLength + 1];
				heightBuff = new wchar_t[heightLength + 1];
				layersBuff = new wchar_t[layersLength + 1];
				activeBuff = new wchar_t[activeLength + 1];

				GetWindowText(widthHwnd, widthBuff, widthLength + 1);
				GetWindowText(heightHwnd, heightBuff, heightLength + 1);
				GetWindowText(layersHwnd, layersBuff, layersLength + 1);
				GetWindowText(activeHwnd, activeBuff, activeLength + 1);

				int w = stoi(widthBuff);
				int h = stoi(heightBuff);
				int l = stoi(layersBuff);
				int a = stoi(activeBuff);

				if (w == 0 || h == 0 || l == 0){
					delete[] widthBuff;
					delete[] heightBuff;
					delete[] layersBuff;
					delete[] activeBuff;
					EndDialog(hwnd, TRUE);
					break;
				}

				if (w * h > 100000){
					delete[] widthBuff;
					delete[] heightBuff;
					delete[] layersBuff;
					delete[] activeBuff;
					MessageBox(nullptr, L"World too large", L"Error", MB_OK);
					EndDialog(hwnd, TRUE);
					break;
				}

				pthis->h_worldEditor->CreateNewWorld(w, h, l, a);

				HMENU menu = GetMenu(pthis->h_hwnd);
				EnableMenuItem(menu, IDM_VIEW_GRID, MF_ENABLED);
				EnableMenuItem(menu, IDM_VIEW_LAYERALL, MF_ENABLED);
				EnableMenuItem(menu, IDM_VIEW_LAYERSTACKED, MF_ENABLED);
				EnableMenuItem(menu, IDM_VIEW_LAYERSINGLE, MF_ENABLED);
				EnableMenuItem(menu, IDM_VIEW_LAYERTRANS, MF_ENABLED);
				EnableMenuItem(menu, IDM_FILE_SAVE, MF_ENABLED);
				EnableMenuItem(menu, IDM_FILE_SAVEAS, MF_ENABLED);

				delete[] widthBuff;
				delete[] heightBuff;
				delete[] layersBuff;
				delete[] activeBuff;
				pthis->m_savePath = L"";
				EndDialog(hwnd, TRUE);
				break;
			}
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hwnd, TRUE);
		break;
	}
	return false;
}

LRESULT CALLBACK InputProcessor::MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	
	int id = -1;
	int code = -1;
	int index = 0;
	static bool m_moving = false;
	static int m_xPos = 0;
	static int m_yPos = 0;
	static WPARAM m_btnstate = 0;
	static HMENU menu = 0;
	static OPENFILENAMEW ofn;
	string path = "";
	wchar_t* relativePath = new wchar_t[MAX_PATH];
	wchar_t* temp = NULL;
	//Save the original path so that we can reset since windows is a bitch
	wchar_t workingDir[300];
	GetCurrentDirectory(300, workingDir);

	switch (message){
	case WM_COMMAND:
		id = LOWORD(wParam);
		code = HIWORD(wParam);
		switch (code){
		case LBN_SELCHANGE:
			int index = SendMessage(h_lb_hwnd, LB_GETCURSEL, 0, 0);
			int len = SendMessage(h_lb_hwnd, LB_GETTEXTLEN, index, 0);
			wchar_t *buff = new wchar_t[len+1];
			SendMessage(h_lb_hwnd, LB_GETTEXT, index, (LPARAM)buff);
			UINT id = SendMessage(h_lb_hwnd, LB_GETITEMDATA, index, 0);
			h_tool->setTileType(id);
			h_tool->setSelectedString(buff);
			delete[] buff;
			break;
		}
		switch (id){
		case IDT_SELECT:
			break;

		case IDT_DRAW:
			break;

		case IDT_ERASE:
			break;

			// FILE MENU CALLBACKS
		case IDM_FILE_NEW:
			if (EnemyManager::Instance()->EnemySetIsLoaded() && h_worldEditor->getTileSet()->getTextureArr())
				DialogBox(nullptr, MAKEINTRESOURCE(IDD_NEW), h_hwnd, NewDialogProc);
			else
				MessageBox(h_hwnd, L"Please load a enemy set and tile set before creating or opening a world", L"Error", MB_OK);

			break;

		case IDM_FILE_OPEN:
			if (EnemyManager::Instance()->EnemySetIsLoaded() && h_worldEditor->getTileSet()->getTextureArr())
			{
				menu = GetMenu(pthis->h_hwnd);
				EnableMenuItem(menu, IDM_FILE_SAVE, MF_ENABLED);
				EnableMenuItem(menu, IDM_FILE_SAVEAS, MF_ENABLED);
				EnableMenuItem(menu, IDM_VIEW_GRID, MF_ENABLED);
				EnableMenuItem(menu, IDM_VIEW_LAYERALL, MF_ENABLED);
				EnableMenuItem(menu, IDM_VIEW_LAYERSTACKED, MF_ENABLED);
				EnableMenuItem(menu, IDM_VIEW_LAYERSINGLE, MF_ENABLED);
				EnableMenuItem(menu, IDM_VIEW_LAYERTRANS, MF_ENABLED);

				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = nullptr;
				ofn.lpstrFile = m_fileName;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(m_fileName);
				ofn.lpstrFilter = L"Crusade World File\0*.crwf\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = nullptr;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				GetOpenFileName(&ofn);
				//Revert back to the original path since windows is a bitch
				SetCurrentDirectory(workingDir);

				if (ofn.lpstrFile[0] != '\0'){
					h_worldEditor->CreateNewWorld(ofn.lpstrFile);
					RECT rc;
					GetClientRect(h_dx_hwnd, &rc);
					h_worldEditor->getWorldTileMatrix()->setBackgroundRect(rc);
				}

				m_savePath = std::wstring(ofn.lpstrFile);

			}
			else
			{
				MessageBox(h_hwnd, L"Please load a enemy set and tile set before creating or opening a world", L"Error", MB_OK);
			}
			break;

		case IDM_FILE_SAVE:
			if (m_savePath.compare(L"") == 0)
			{
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = nullptr;
				ofn.lpstrFile = m_fileName;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(m_fileName);
				ofn.lpstrFilter = L"Crusade World File\0*.crwf\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = nullptr;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.lpstrTitle = L"Save Crusade World As";
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				GetSaveFileName(&ofn);

				path = CW2A(ofn.lpstrFile);

				//Revert back to the original path since windows is a bitch
				SetCurrentDirectory(workingDir);

				path = path.substr(0, path.find('.'));
				path.append(".crwf");
				CrusadeUtil::debug(path);
				CrusadeUtil::s_to_ws(path, &temp);

				if (ofn.lpstrFile[0] != '\0'){
					if (!h_worldEditor->getWorldTileMatrix()->saveWorld((LPWSTR)temp))
						MessageBox(h_hwnd, L"Whoops, that file could not be accessed!", L"Error", MB_OK);
				}
				m_savePath = std::wstring(temp);

			}
			else
			{
				if (!h_worldEditor->getWorldTileMatrix()->saveWorld((LPWSTR)m_savePath.c_str()))
					MessageBox(h_hwnd, L"Whoops, that file could not be accessed!", L"Error", MB_OK);
			}
			break;

		case IDM_FILE_SAVEAS:
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = nullptr;
			ofn.lpstrFile = m_fileName;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(m_fileName);
			ofn.lpstrFilter = L"Crusade World File\0*.crwf\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = nullptr;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.lpstrTitle = L"Save Crusade World As";
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			GetSaveFileName(&ofn);

			path = CW2A(ofn.lpstrFile);
			//Revert back to the original path since windows is a bitch
			SetCurrentDirectory(workingDir);
			path = path.substr(0, path.find('.'));
			path.append(".crwf");
			CrusadeUtil::debug(path);
			CrusadeUtil::s_to_ws(path, &temp);
			if (ofn.lpstrFile[0] != '\0'){
				if (!h_worldEditor->getWorldTileMatrix()->saveWorld((LPWSTR)temp))
					MessageBox(h_hwnd, L"Whoops, that file could not be accessed!", L"Error", MB_OK);
			}

			m_savePath = std::wstring(temp);

			break;

		case IDM_FILE_EXIT:
			EndDialog(hwnd, TRUE);
			break;

			// EDIT MENU CALLBACKS
		case IDM_EDIT_UNDO:
			break;

		case IDM_EDIT_REDO:
			break;

			// VIEW MENU CALLBACKS
		case IDM_VIEW_TOOLBOX:
			break;

		case IDM_VIEW_TILELIST:
			break;

		case IDM_VIEW_LAYERALL:
			menu = GetMenu(h_hwnd);
			CheckMenuItem(menu, IDM_VIEW_LAYERALL, MF_CHECKED);
			CheckMenuItem(menu, IDM_VIEW_LAYERSINGLE, MF_UNCHECKED);
			CheckMenuItem(menu, IDM_VIEW_LAYERSTACKED, MF_UNCHECKED);
			CheckMenuItem(menu, IDM_VIEW_LAYERTRANS, MF_UNCHECKED);
			h_worldEditor->getWorldTileMatrix()->setLayerViewState(WorldTileMatrix::LayerViewState::VIEW_ALL);
			break;

		case IDM_VIEW_LAYERSTACKED:
			menu = GetMenu(h_hwnd);
			CheckMenuItem(menu, IDM_VIEW_LAYERALL, MF_UNCHECKED);
			CheckMenuItem(menu, IDM_VIEW_LAYERSINGLE, MF_UNCHECKED);
			CheckMenuItem(menu, IDM_VIEW_LAYERSTACKED, MF_CHECKED);
			CheckMenuItem(menu, IDM_VIEW_LAYERTRANS, MF_UNCHECKED);
			h_worldEditor->getWorldTileMatrix()->setLayerViewState(WorldTileMatrix::LayerViewState::VIEW_STACKED);
			break;

		case IDM_VIEW_LAYERSINGLE:
			menu = GetMenu(h_hwnd);
			CheckMenuItem(menu, IDM_VIEW_LAYERALL, MF_UNCHECKED);
			CheckMenuItem(menu, IDM_VIEW_LAYERSINGLE, MF_CHECKED);
			CheckMenuItem(menu, IDM_VIEW_LAYERSTACKED, MF_UNCHECKED);
			CheckMenuItem(menu, IDM_VIEW_LAYERTRANS, MF_UNCHECKED);
			h_worldEditor->getWorldTileMatrix()->setLayerViewState(WorldTileMatrix::LayerViewState::VIEW_SINGLE);
			break;

		case IDM_VIEW_LAYERTRANS:
			menu = GetMenu(h_hwnd);
			CheckMenuItem(menu, IDM_VIEW_LAYERALL, MF_UNCHECKED);
			CheckMenuItem(menu, IDM_VIEW_LAYERSINGLE, MF_UNCHECKED);
			CheckMenuItem(menu, IDM_VIEW_LAYERSTACKED, MF_UNCHECKED);
			CheckMenuItem(menu, IDM_VIEW_LAYERTRANS, MF_CHECKED);
			h_worldEditor->getWorldTileMatrix()->setLayerViewState(WorldTileMatrix::LayerViewState::VIEW_TRANS);
			break;

		case IDM_VIEW_GRID:
			menu = GetMenu(h_hwnd);
			if (h_worldEditor->isGridVis())
				CheckMenuItem(menu, IDM_VIEW_GRID, MF_UNCHECKED);
			else
				CheckMenuItem(menu, IDM_VIEW_GRID, MF_CHECKED);

			h_worldEditor->toggleGrid();
			break;

		case IDM_VIEW_TRIGGERS:
			menu = GetMenu(h_hwnd);
			if (TriggerManager::Instance()->isDebugMode())
			{
				CheckMenuItem(menu, IDM_VIEW_TRIGGERS, MF_UNCHECKED);
				TriggerManager::Instance()->setDebugMode(false);
			}
			else
			{
				CheckMenuItem(menu, IDM_VIEW_TRIGGERS, MF_CHECKED);
				TriggerManager::Instance()->setDebugMode(true);
			}
			break;

		// MAP MENU CALLBACKS
		case IDM_MAP_ADDTILESET:
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = nullptr;
			ofn.lpstrFile = m_fileName;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(m_fileName);
			//ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
			ofn.lpstrFilter = L"XML\0*.xml\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = nullptr;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			GetOpenFileName(&ofn);
			//Revert back to the original path since windows is a bitch
			SetCurrentDirectory(workingDir);
			if (ofn.lpstrFile[0] != '\0')
				if (!h_worldEditor->getTileSet()->LoadTileSet(ofn.lpstrFile))
				{
					MessageBox(NULL, L"File does not contain acceptable xml", L"Error", MB_OK);
				}
				else
				{
					h_worldEditor->getTileSet()->PopulateListBox(h_lb_hwnd);
					menu = GetMenu(pthis->h_hwnd);
					EnableMenuItem(menu, IDM_MAP_SHOWTILESET, MF_ENABLED);
					h_worldEditor->setTool(IDM_TOOL_PENCIL);
					updateTool();
				}
			break;

		case IDM_MAP_ADDPLAYER:
			h_worldEditor->setTool(IDM_TOOL_PLAYERSPAWNPOINT);
			updateTool();
			SendMessage(h_lb_hwnd, LB_RESETCONTENT, 0, 0);
			SendMessage(h_lb_hwnd, LB_ADDSTRING, 0, (LPARAM)(L"Player Spawn"));
			break;

		case IDM_MAP_ADDENEMIES:
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = nullptr;
			ofn.lpstrFile = m_fileName;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(m_fileName);
			ofn.lpstrFilter = L"XML\0*.xml\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = nullptr;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			GetOpenFileName(&ofn);
			//Revert back to the original path since windows is a bitch
			SetCurrentDirectory(workingDir);
			if (ofn.lpstrFile[0] != '\0')
				if (!EnemyManager::Instance()->loadEnemyData(ofn.lpstrFile))
				{
					MessageBox(NULL, L"File does not contain acceptable xml", L"Error", MB_OK);
				}
				else
				{
					EnemyManager::Instance()->populateListbox(h_lb_hwnd);
					menu = GetMenu(pthis->h_hwnd);
					EnableMenuItem(menu, IDM_MAP_SHOWENEMIES, MF_ENABLED);
					h_worldEditor->setTool(IDM_TOOL_ENEMY);
					updateTool();
				}
			break;

		case IDM_MAP_SHOWENEMIES:
			if (EnemyManager* f = EnemyManager::Instance()){
				f->populateListbox(h_lb_hwnd);
				h_worldEditor->setTool(IDM_TOOL_ENEMY);
				updateTool();
			}
			break;

		case IDM_MAP_SHOWTILESET:
			h_worldEditor->getTileSet()->PopulateListBox(h_lb_hwnd);
			h_worldEditor->setTool(IDM_TOOL_PENCIL);
			updateTool();
			break;

		case IDM_MAP_ADDBACKGROUND:
			if (EnemyManager::Instance()->EnemySetIsLoaded() && h_worldEditor->getTileSet()->getTextureArr()){
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = nullptr;
				ofn.lpstrFile = m_fileName;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(m_fileName);
				ofn.lpstrFilter = L"All\0*.*\0PNG\0*.png\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = nullptr;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				RECT rc;
				GetClientRect(h_dx_hwnd, &rc);
				GetOpenFileName(&ofn);
				//Revert back to the original path since windows is a bitch
				SetCurrentDirectory(workingDir);

				CrusadeUtil::getRelativePathTo(ofn.lpstrFile, relativePath);

				if (h_worldEditor->getWorldTileMatrix())
					if (ofn.lpstrFile[0] != '\0')
						h_worldEditor->getWorldTileMatrix()->setBackground(relativePath, rc);

				delete[] relativePath;
			}
			break;
		case IDM_MAP_ADDMUSIC:
			if (EnemyManager::Instance()->EnemySetIsLoaded() && h_worldEditor->getTileSet()->getTextureArr()){
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = nullptr;
				ofn.lpstrFile = m_fileName;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(m_fileName);
				ofn.lpstrFilter = L"ADPCM\0*.adpcm\0WAV\0*.wav\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = nullptr;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				
				
				wstring soundDir = (wstring)workingDir + wstring(L"../img/Sounds");
				SetCurrentDirectory((LPCWSTR)soundDir.c_str());
				GetOpenFileName(&ofn);

				CrusadeUtil::getRelativePathTo(ofn.lpstrFile, relativePath);
				wstring wrel = wstring(relativePath);
				if (wrel.substr(0, 2).compare(L".\\")){
					MessageBox(NULL, L"File is not within the Sounds folder.", L"Error", MB_OK);
					break;//not in current directory, don't save
				}
				wrel = wrel.substr(2);
				if (h_worldEditor->getWorldTileMatrix())
					if (ofn.lpstrFile[0] != '\0')
						h_worldEditor->getWorldTileMatrix()->setMusicPath((LPWSTR)wrel.c_str());
				//Revert back to the original path since windows is a bitch
				SetCurrentDirectory(workingDir);
				delete[] relativePath;
			}
			break;
		
		// TOOL MENU CALLBACKS
		case IDM_TOOL_PENCIL:
			h_worldEditor->setTool(IDM_TOOL_PENCIL);
			updateTool();
			h_worldEditor->getTileSet()->PopulateListBox(h_lb_hwnd);
			break;
		case IDM_TOOL_BOX:
			h_worldEditor->setTool(IDM_TOOL_BOX);
			updateTool();
			h_worldEditor->getTileSet()->PopulateListBox(h_lb_hwnd);
			break;
		case IDM_TOOL_ENEMY:
			h_worldEditor->setTool(IDM_TOOL_ENEMY);
			updateTool();
			EnemyManager::Instance()->populateListbox(h_lb_hwnd);
			break;
		case IDM_TOOL_LUATRIGGER:
			h_worldEditor->setTool(IDM_TOOL_LUATRIGGER);
			updateTool();
			TriggerManager::Instance()->loadLuaData((LPWSTR)TriggerManager::Instance()->getScriptFolder().c_str());
			TriggerManager::Instance()->populateListbox(h_lb_hwnd);
			break;
		case IDM_TOOL_PLAYERSPAWNPOINT:
			h_worldEditor->setTool(IDM_TOOL_PLAYERSPAWNPOINT);
			updateTool();
			SendMessage(h_lb_hwnd, LB_RESETCONTENT, 0, 0);
			SendMessage(h_lb_hwnd, LB_ADDSTRING, 0, (LPARAM)(L"Player Spawn"));
			break;
		case IDM_EDIT_MODE_WORLDEDIT:
			EnemyManager::Instance()->setWorldEditMode(true);
			CheckMenuItem(menu, IDM_EDIT_MODE_WORLDEDIT, MF_CHECKED);
			CheckMenuItem(menu, IDM_EDIT_MODE_LIVEWORLDEDIT, MF_UNCHECKED);
			CheckMenuItem(menu, IDM_EDIT_MODE_PLAYTEST, MF_UNCHECKED);
			break;
		case IDM_EDIT_MODE_LIVEWORLDEDIT:
			EnemyManager::Instance()->setWorldEditMode(false);
			CheckMenuItem(menu, IDM_EDIT_MODE_WORLDEDIT, MF_UNCHECKED);
			CheckMenuItem(menu, IDM_EDIT_MODE_LIVEWORLDEDIT, MF_CHECKED);
			CheckMenuItem(menu, IDM_EDIT_MODE_PLAYTEST, MF_UNCHECKED);
			break;
		case IDM_EDIT_MODE_PLAYTEST:
			CheckMenuItem(menu, IDM_EDIT_MODE_WORLDEDIT, MF_UNCHECKED);
			CheckMenuItem(menu, IDM_EDIT_MODE_LIVEWORLDEDIT, MF_UNCHECKED);
			CheckMenuItem(menu, IDM_EDIT_MODE_PLAYTEST, MF_CHECKED);
			break;
		}
		

		break;

	// MOUSE STATE CALLBACKS
	case WM_MOUSEMOVE:
		m_btnstate = wParam;
		m_xPos = GET_X_LPARAM(lParam);
		m_yPos = GET_Y_LPARAM(lParam);
		OnMouseMove(wParam, m_xPos, m_yPos);
		break;

	case WM_LBUTTONDOWN:
		m_btnstate = wParam;
		m_xPos = GET_X_LPARAM(lParam);
		m_yPos = GET_Y_LPARAM(lParam);
		OnMouseDown(wParam, m_xPos, m_yPos);
		break;

	case WM_LBUTTONUP:
		m_btnstate = wParam;
		m_xPos = GET_X_LPARAM(lParam);
		m_yPos = GET_Y_LPARAM(lParam);
		OnMouseUp(wParam, m_xPos, m_yPos);
		break;

	case WM_RBUTTONDOWN:
		m_btnstate = wParam;
		m_xPos = GET_X_LPARAM(lParam);
		m_yPos = GET_Y_LPARAM(lParam);
		OnMouseDown(wParam, m_xPos, m_yPos);
		break;

	case WM_RBUTTONUP:
		m_btnstate = wParam;
		m_xPos = GET_X_LPARAM(lParam);
		m_yPos = GET_Y_LPARAM(lParam);
		OnMouseUp(wParam, m_xPos, m_yPos);
		break;

	case WM_MBUTTONDOWN:
		m_btnstate = wParam;
		m_xPos = GET_X_LPARAM(lParam);
		m_yPos = GET_Y_LPARAM(lParam);
		OnMouseDown(wParam, m_xPos, m_yPos);
		break;

	case WM_MBUTTONUP:
		m_btnstate = wParam;
		m_xPos = GET_X_LPARAM(lParam);
		m_yPos = GET_Y_LPARAM(lParam);
		OnMouseUp(wParam, m_xPos, m_yPos);
		break;

	case WM_MBUTTONDBLCLK:
		m_btnstate = wParam;
		m_xPos = GET_X_LPARAM(lParam);
		m_yPos = GET_Y_LPARAM(lParam);
		OnDoubleClick(wParam, m_xPos, m_yPos);
		break;

	case WM_MOUSEWHEEL:
		if (h_worldEditor->getWorldTileMatrix()){
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
				h_worldEditor->incCurrLayer();
			else
				h_worldEditor->decCurrLayer();
		}
		return 1;	// return 1 defwindowproc doesnt know how to handle this
		break;

	// KEY STATE CALLBACKS
	case WM_KEYUP:
		switch (wParam){
		case VK_UP:
			h_worldEditor->incCurrLayer();
			break;
		case VK_DOWN:
			h_worldEditor->decCurrLayer();
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam){
		case 0x47:	// 'G' key
			if (GetKeyState(VK_CONTROL) & 0x8000){
				menu = GetMenu(h_hwnd);
				if (h_worldEditor->isGridVis())
					CheckMenuItem(menu, IDM_VIEW_GRID, MF_UNCHECKED);
				else
					CheckMenuItem(menu, IDM_VIEW_GRID, MF_CHECKED);

				h_worldEditor->toggleGrid();
			}
			break;
		case 0x56:	// 'V' key
			h_worldEditor->setTool(IDM_TOOL_PENCIL);
			updateTool();
			h_worldEditor->getTileSet()->PopulateListBox(h_lb_hwnd);
			break;
		case 0x42:	// 'B' key
			h_worldEditor->setTool(IDM_TOOL_BOX);
			updateTool();
			h_worldEditor->getTileSet()->PopulateListBox(h_lb_hwnd);
			break;
		case 0x4E:	// 'N' key
			h_worldEditor->setTool(IDM_TOOL_ENEMY);
			updateTool();
			EnemyManager::Instance()->populateListbox(h_lb_hwnd);
			break;
		case 0x4D:	// 'M' key
			h_worldEditor->setTool(IDM_TOOL_PLAYERSPAWNPOINT);
			updateTool();
			SendMessage(h_lb_hwnd, LB_RESETCONTENT, 0, 0);
			SendMessage(h_lb_hwnd, LB_ADDSTRING, 0, (LPARAM)(L"Player Spawn"));
			//EnemyManager::Instance()->populateListbox(h_lb_hwnd);
			break;
		case 0x54:	// 'T' key
			h_worldEditor->setTool(IDM_TOOL_LUATRIGGER);
			updateTool();
			TriggerManager::Instance()->loadLuaData((LPWSTR)TriggerManager::Instance()->getScriptFolder().c_str());
			TriggerManager::Instance()->populateListbox(h_lb_hwnd);
			break;
		}
		break;

	case WM_SIZE:
		if (this){
			RECT rc;
			GetClientRect(h_dx_hwnd, &rc);
			if (h_worldEditor->getWorldTileMatrix())
				h_worldEditor->getWorldTileMatrix()->setBackgroundRect(rc);
		}
		break;
	}
	
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void InputProcessor::OnMouseMove(WPARAM btnstate, int x, int y){
	m_mousePos = h_game->m_camera->ScreenToWorldPoint(Vector2(x, y));
	if(h_worldEditor->getWorldTileMatrix())
		h_tool->OnMouseMove(btnstate, m_mousePos.x, m_mousePos.y);
}

void InputProcessor::OnMouseDown(WPARAM btnstate, int x, int y){
	m_mousePos = h_game->m_camera->ScreenToWorldPoint(Vector2(x, y));
	if (h_worldEditor->getWorldTileMatrix())
		h_tool->OnMouseDown(btnstate, m_mousePos.x, m_mousePos.y);
}

void InputProcessor::OnMouseUp(WPARAM btnstate, int x, int y){
	//CrusadeUtil::debug(s.str());
	if (h_worldEditor->getWorldTileMatrix())
		m_mousePos = h_game->m_camera->ScreenToWorldPoint(Vector2(x, y));
	/*int mouseX = floor(m_mousePos.x);
	int mouseY = floor(m_mousePos.y);
	std::ostringstream s;
	s << "(" << mouseX << ", " << mouseY << ")\n";
	wchar_t *wide;
	CrusadeUtil::s_to_ws(s.str(), &wide);
	SendMessage(h_lb_hwnd, LB_ADDSTRING, 0, (LPARAM)wide);
	delete[] wide;*/
}

void InputProcessor::OnDoubleClick(WPARAM btnstate, int x, int y){

}

void InputProcessor::giveHandles(HWND hwnd, HWND dx_hwnd, HWND lb_hwnd, HWND tb_hwnd){
	h_hwnd = hwnd;
	h_dx_hwnd = dx_hwnd;
	h_lb_hwnd = lb_hwnd;
	h_tb_hwnd = tb_hwnd;
}

Vector2 InputProcessor::getMousePos(){
	return m_mousePos;
}

void InputProcessor::updateTool(){
	this->h_tool = h_worldEditor->getTool();
}