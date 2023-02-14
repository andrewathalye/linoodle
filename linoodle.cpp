
#include "windows_library.h"
#include <iostream>

typedef __attribute__((ms_abi)) size_t(*tDecompressFunc)(uint8_t * srcBuf, size_t srcLen, uint8_t * dstBuf, size_t dstLen, int64_t fuzz, int64_t crc, int64_t verbose, uint8_t * dstBase, size_t e, void * cb, void * cbCtx, void * scratch, size_t scratchSize, int64_t threadPhase);

typedef __attribute__((ms_abi)) size_t(*tCompressFunc)(int64_t codec, uint8_t * srcBuf, size_t srcLen, uint8_t * dstBuf, int64_t level, void * opts, size_t offs, size_t unused, void * scratch, size_t scratchSize);

WindowsLibrary* wl = NULL;
tDecompressFunc m_decompressFunc;
tCompressFunc m_compressFunc;

void initWrapper (void) {
    try {
    	wl = new WindowsLibrary(WindowsLibrary::Load("ext_lib/oo2core_3_win64.dll"));
    	m_decompressFunc = reinterpret_cast<tDecompressFunc>(wl->GetExport("OodleLZ_Decompress"));
    	m_compressFunc = reinterpret_cast<tCompressFunc>(wl->GetExport("OodleLZ_Compress"));
    } catch (std::exception &e) {
    	std::cerr << "[Error] Could not initialise Oodle library: " << e.what() << "\n";
	exit(-1);
    }
}

extern "C" size_t OodleLZ_Decompress(uint8_t * srcBuf, size_t srcLen, uint8_t * dstBuf, size_t dstLen, int64_t fuzz, int64_t crc, int64_t verbose, uint8_t * dstBase, size_t e, void * cb, void * cbCtx, void * scratch, size_t scratchSize, int64_t threadPhase)
{
	if (!wl) {
		initWrapper();
	}

	WindowsLibrary::SetupCall();
	return m_decompressFunc(srcBuf, srcLen, dstBuf, dstLen, fuzz, crc, verbose, dstBase, e, cb, cbCtx, scratch, scratchSize, threadPhase);
}

extern "C" size_t OodleLZ_Compress(int64_t codec, uint8_t * srcBuf, size_t srcLen, uint8_t * dstBuf, int64_t level, void * opts, size_t offs, size_t unused, void * scratch, size_t scratchSize)
{
	if (!wl) {
		initWrapper();
	}

	WindowsLibrary::SetupCall();
	return m_compressFunc(codec, srcBuf, srcLen, dstBuf, level, opts, offs, unused, scratch, scratchSize);
}
