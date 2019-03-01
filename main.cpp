//=============================================================================
// メイン処理 [main.cpp]
//
// Author : GP11B341 24 中込和輝
// 作成日 : 2019/2/25
//=============================================================================

//*****************************************************************************
// インクルード
//*****************************************************************************
#include "main.h"
#include "input.h"
#include "sound.h"
#include <stdio.h>
//#include "title.h"
#include "font.h"
#include "debugproc.h"
#include "player.h"
#include "bullet.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"Team Adult Game"	// ウインドウのクラス名
#define WINDOW_NAME		"Team Adult Game"	// ウインドウのキャプション名


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);


//*****************************************************************************
// グローバル変数:
//*****************************************************************************
LPDIRECT3D9			Direct3D = NULL;		// Direct3D オブジェクト
LPDIRECT3DDEVICE9	pD3DDevice = NULL;		// Deviceオブジェクト(描画に必要)
STAGE				stage;					// 現在のステージ
DWORD				currentTime;			// 現在のシステム時刻
#ifdef _DEBUG
int					cntFPS;					// FPSカウンタ
#endif


//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	// 時間計測
	DWORD execLastTime;
	DWORD fpsLastTime;
	DWORD frameCount;

	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		LoadIcon(hInstance, MAKEINTRESOURCE(NULL)),
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND hWnd;
	MSG msg;
	
	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindowEx(0,
						CLASS_NAME,
						WINDOW_NAME,
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,
						SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),
						NULL,
						NULL,
						hInstance,
						NULL);

	// 初期化処理(ウィンドウを作成してから行う)
	if(FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}

	//フレームカウント初期化
	timeBeginPeriod(1);				// 分解能を設定
	execLastTime = 
	fpsLastTime = timeGetTime();
	currentTime = 0;
	frameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// メッセージループ
	while(1)
	{

		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳とディスパッチ
				TranslateMessage(&msg);

				DispatchMessage(&msg);
			}
        }
		else
		{
			currentTime = timeGetTime();
			if((currentTime - fpsLastTime) >= 500)	// 0.5秒ごとに実行
			{
#ifdef _DEBUG
				cntFPS =frameCount * 1000 / (currentTime - fpsLastTime);
#endif
				fpsLastTime = currentTime;
				frameCount = 0;
			}

			if((currentTime - execLastTime) >= (1000 / FPS))
			{
				execLastTime = currentTime;

				// 更新処理
				Update();

				// 描画処理
				Draw();

				frameCount++;
			}
		}
	}
	
	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	timeEndPeriod(1);				// 分解能を戻す

	return (int)msg.wParam;
}


//=============================================================================
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	D3DPRESENT_PARAMETERS d3dpp;
    D3DDISPLAYMODE d3ddm;

	// Direct3Dオブジェクトの作成
	Direct3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(Direct3D == NULL)
	{
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
    if(FAILED(Direct3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));							// ワークをゼロクリア
	d3dpp.BackBufferCount			= 1;						// バックバッファの数
	d3dpp.BackBufferWidth			= SCREEN_WIDTH;				// ゲーム画面サイズ(幅)
	d3dpp.BackBufferHeight			= SCREEN_HEIGHT;			// ゲーム画面サイズ(高さ)
	d3dpp.BackBufferFormat			= d3ddm.Format;				// バックバッファフォーマットはディスプレイモードに合わせて使う
	d3dpp.SwapEffect				= D3DSWAPEFFECT_DISCARD;	// 映像信号に同期してフリップする
	d3dpp.Windowed					= bWindow;					// ウィンドウモード
	d3dpp.AutoDepthStencilFormat	= D3DFMT_D16;				// デプスバッファとして16bitを使う

	if(bWindow)
	{// ウィンドウモード
		d3dpp.FullScreen_RefreshRateInHz = 0;								// リフレッシュレート
		d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;	// インターバル
	}
	else
	{// フルスクリーンモード
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;			// リフレッシュレート
		d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_DEFAULT;		// インターバル
	}

	// デバイスオブジェクトの生成
	// [デバイス作成制御]<描画>と<頂点処理>をハードウェアで行なう
	if(FAILED(Direct3D->CreateDevice(D3DADAPTER_DEFAULT,
									D3DDEVTYPE_HAL, 
									hWnd, 
									D3DCREATE_HARDWARE_VERTEXPROCESSING, 
									&d3dpp, &pD3DDevice)))
	{
		// 上記の設定が失敗したら
		// [デバイス作成制御]<描画>をハードウェアで行い、<頂点処理>はCPUで行なう
		if(FAILED(Direct3D->CreateDevice(D3DADAPTER_DEFAULT,
										D3DDEVTYPE_HAL, 
										hWnd, 
										D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
										&d3dpp, &pD3DDevice)))
		{
			// 上記の設定が失敗したら
			// [デバイス作成制御]<描画>と<頂点処理>をCPUで行なう
			if(FAILED(Direct3D->CreateDevice(D3DADAPTER_DEFAULT,
											D3DDEVTYPE_REF,
											hWnd, 
											D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
											&d3dpp, &pD3DDevice)))
			{
				// 初期化失敗
				return E_FAIL;
			}
		}
	}

	// レンダーステートパラメータの設定
    pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// 裏面をカリング
	pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);				// αブレンドを行う
	pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);			// αソースカラーの指定
	pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);		// αデスティネーションカラーの指定

	// サンプラーステートパラメータの設定
	pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);		// テクスチャアドレッシング方法(U値)を設定
	pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);		// テクスチャアドレッシング方法(V値)を設定
	pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);		// テクスチャ縮小フィルタモードを設定
	pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);		// テクスチャ拡大フィルタモードを設定

	// テクスチャステージステートの設定
	pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);	// アルファブレンディング処理
	pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);	// 最初のアルファ引数
	pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);	// ２番目のアルファ引数

	// ゲームシステムの初期化
	InitInput(hInstance, hWnd);	// 入力処理の初期化
	InitSound(hWnd);			// サウンドの初期化
	InitFont();					// フォントの初期化
	InitPlayer();				// プレイヤーの初期化
	InitBullet();				// バレットの初期化
#ifdef _DEBUG
	InitDebugProc();			// デバッグ表示の初期化
#endif

	// 音量調節
	//GetSound(BGM_BATTLE_1)->SetVolume(-200);

	// 最初のステージを設定
	//stage = TITLE;
	stage = GAME;

	return S_OK;
}


//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
#ifdef _DEBUG
	UninitDebugProc();			// デバッグ表示の終了
#endif

	//UninitTitle();				// タイトルの終了処理
	UninitInput();				// 入力処理の終了処理
	UninitSound();				// サウンドの終了処理
	UninitFont();				// フォントの終了処理
	UninitPlayer();				// プレイヤーの初期化
	UninitBullet();				// バレットの初期化
	// デバイスの開放
	if(pD3DDevice != NULL)
	{
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}

	// Direct3Dオブジェクトの開放
	if(Direct3D != NULL)
	{
		Direct3D->Release();
		Direct3D = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{

	UpdateInput();				// 入力更新
		
	switch (stage)
	{
	case SPLASH:
		break;

	case TITLE:
		break;

	case TUTORIAL:
		break;

	case GAME:
		UpdatePlayer();				// プレイヤーの初期化
		UpdateBullet();				// バレットの初期化
		break;

	case PAUSE:
		break;

	case RESULT:
		break;

	case EXIT:
		PostQuitMessage(0);
		break;
	}

#ifdef _DEBUG
	UpdateDebugProc();			// デバッグ表示の更新
#endif
}


//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	// バックバッファのクリア
	pD3DDevice->Clear(0, NULL, (D3DCLEAR_TARGET), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	// Direct3Dによる描画の開始
	if(SUCCEEDED(pD3DDevice->BeginScene()))
	{
		switch (stage)
		{
		case SPLASH:
			break;

		case TITLE:
			break;

		case TUTORIAL:
			break;

		case GAME:
			DrawPlayer();				// プレイヤーの初期化
			DrawBullet();				// バレットの初期化
			break;

		case PAUSE:
			break;

		case RESULT:
			break;

		case EXIT:
			break;
		}

#ifdef _DEBUG
		DrawDebugProc();		// デバッグ表示の描画
#endif

		// Direct3Dによる描画の終了
		pD3DDevice->EndScene();
	}

	// バックバッファとフロントバッファの入れ替え
	pD3DDevice->Present(NULL, NULL, NULL, NULL);
}


//=============================================================================
// デバイス取得関数
//-----------------------------------------------------------------------------
// 戻り値：LPDIRECT3DDEVICE9 デバイスへのポインタ
// 引数  ：void
//=============================================================================
LPDIRECT3DDEVICE9 GetDevice(void)
{
	return(pD3DDevice);
}


//=============================================================================
// ステージを設定
//-----------------------------------------------------------------------------
// 戻り値：void
// 引数  ：STAGE　設定するステージ
//=============================================================================
void SetStage(STAGE set)
{
	stage = set;
}


//=============================================================================
// 現在時刻取得取得
//-----------------------------------------------------------------------------
// 戻り値：DWORD　現在のシステム時刻
// 引数  ：void
//=============================================================================
DWORD GetTime(void)
{
	return currentTime;
}


#ifdef _DEBUG
//=============================================================================
// FPS取得
//-----------------------------------------------------------------------------
// 戻り値：int　現在のフレームレート
// 引数  ：void
//=============================================================================
int GetFPS(void)
{
	return cntFPS;
}
#endif
