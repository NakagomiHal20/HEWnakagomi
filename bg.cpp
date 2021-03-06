//=============================================================================
//
// 背景処理 [bg.cpp]
// Author : HAL東京昼間部 2年制ゲーム学科 GP11B341 24 中込和輝
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "bg.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
// テクスチャ場所
#define TEXTURE_BG			_T("data/TEXTURE/bg.png")

// 背景のサイズ
#define BG_SIZE_X			(SCREEN_WIDTH * 2)
#define BG_SIZE_Y			(SCREEN_HEIGHT)	

// 背景の座標
#define INIT_POS_X			(0.0f)
#define MOVE_SPEED_X		(1.0f)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexBg(void);					// 頂点の作成
void SetVertexBg(void);						// 頂点座標の設定
void SetColorBg(void);						// 頂点カラーの設定
void SetTextureBg(void);					// テクスチャ座標の設定


//*****************************************************************************
// グローバル変数
//*****************************************************************************
GAMEBG bg;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBg(int type)
{
	if (type == 0)
	{
		// テクスチャの読み込み
		LPDIRECT3DDEVICE9 pDevice = GetDevice();
		D3DXCreateTextureFromFile(pDevice,
			TEXTURE_BG,
			&bg.pTexture);
	}

	bg.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	bg.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	MakeVertexBg();

	return S_OK;
}


//=============================================================================
// 終了処理
//=============================================================================
void UninitBg(void)
{
	SAFE_RELEASE(bg.pTexture)
}


//=============================================================================
// 更新処理
//=============================================================================
void UpdateBg(void)
{
	if (IsButtonPressed(0, BUTTON_RIGHT) || GetKeyboardPress(DIK_RIGHT))
	{
		bg.pos.x -= MOVE_SPEED_X;
	}
	else if (IsButtonPressed(0, BUTTON_LEFT) || GetKeyboardPress(DIK_LEFT))
	{
		bg.pos.x += MOVE_SPEED_X;
	}

	if (bg.pos.x < -BG_SIZE_X / 2)
	{
		bg.pos.x = INIT_POS_X;
	}
	if (bg.pos.x > INIT_POS_X)
	{
		bg.pos.x = -BG_SIZE_X / 2;
	}

	SetVertexBg();
}


//=============================================================================
// 描画処理
//=============================================================================
void DrawBg(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture( 0, bg.pTexture );

	// ポリゴンの描画
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, NUM_POLYGON, bg.vertexWk, sizeof(VERTEX_2D));
}


//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT MakeVertexBg(void)
{
	// 頂点座標の設定	
	SetVertexBg();

	// 頂点カラーの設定
	SetColorBg();

	// テクスチャ座標の設定
	SetTextureBg();

	// rhwの設定
	bg.vertexWk[0].rhw =
		bg.vertexWk[1].rhw =
		bg.vertexWk[2].rhw =
		bg.vertexWk[3].rhw = 1.0f;

	return S_OK;
}


//=============================================================================
// 頂点座標の設定
//=============================================================================
void SetVertexBg(void)
{
	bg.vertexWk[0].vtx = D3DXVECTOR3(bg.pos.x, bg.pos.y, bg.pos.z);
	bg.vertexWk[1].vtx = D3DXVECTOR3(bg.pos.x + BG_SIZE_X, bg.pos.y, bg.pos.z);
	bg.vertexWk[2].vtx = D3DXVECTOR3(bg.pos.x, bg.pos.y + BG_SIZE_Y, bg.pos.z);
	bg.vertexWk[3].vtx = D3DXVECTOR3(bg.pos.x + BG_SIZE_X, bg.pos.y + BG_SIZE_Y, bg.pos.z);
}


//=============================================================================
// 頂点カラーの設定
//=============================================================================
void SetColorBg(void)
{
	bg.vertexWk[0].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	bg.vertexWk[1].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	bg.vertexWk[2].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	bg.vertexWk[3].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
}


//=============================================================================
// テクスチャ座標の設定
//=============================================================================
void SetTextureBg(void)
{
	bg.vertexWk[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	bg.vertexWk[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	bg.vertexWk[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	bg.vertexWk[3].tex = D3DXVECTOR2(1.0f, 1.0f);
}
