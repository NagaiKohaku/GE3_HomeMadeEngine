#pragma once
#include "xaudio2.h"

#include "fstream"
#include "Windows.h"
#include "wrl.h"

#pragma comment(lib,"xaudio2.lib")

//音声データ
struct SoundData {
	WAVEFORMATEX wfex;       //波形フォーマット
	BYTE* pBuffer;           //バッファの先頭アドレス
	unsigned int bufferSize; //バッファサイズ
};

class Audio {

public:

	//チャンクヘッダ
	struct ChunkHeader {
		char id[4];   //チャンク毎のID
		int32_t size; //チャンクサイズ
	};

	//RIFFヘッダチャンク
	struct RiffHeader {
		ChunkHeader chunk; //"RIFF"
		char type[4];      //"WAVE
	};

	//FMTチャンク
	struct FormatChunk {
		ChunkHeader chunk; //"fmt"
		WAVEFORMATEX fmt;  //波形フォーマット
	};

	/// <summary>
	/// 静的インスタンスを取得
	/// </summary>
	/// <returns>静的インスタンス</returns>
	static Audio* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 音声データのロード
	/// </summary>
	/// <param name="filename">ファイル名</param>
	/// <returns>音声データ</returns>
	SoundData SoundLoadWave(const char* filename);

	/// <summary>
	/// 音声データの解放
	/// </summary>
	/// <param name="soundData">音声データ</param>
	void SoundUnLoad(SoundData* soundData);

	/// <summary>
	/// 音声データの再生
	/// </summary>
	/// <param name="xAudio2">xAudio2エンジン</param>
	/// <param name="soundData">音声データ</param>
	void SoundPlayWave(const SoundData& soundData);

private:

	//xAudio2エンジン
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;

	//マスターボイス
	IXAudio2MasteringVoice* masterVoice_;

};