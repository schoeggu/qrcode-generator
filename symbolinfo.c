#include "symbolinfo.h"

bool si_init(SymbolInfo* si, const byte* data, int dataCount, EncodeModeIndicator mode, ECLevel level)
{
	si_reset(si);
	si_set_data(si, data, dataCount, mode);
	si_set_eclevel(si, level);
	//TODO ...
	return true;
}

bool si_set_version(SymbolInfo* si, int version)
{
	if (!si || version < 0 || version > 40) return false; 
	si->version = version;
	si->autoVersion = !version; // si_set_version(0) == autoversion

	return si_check_integrity(si);
}

bool si_set_data(SymbolInfo* si, const byte* data, int dataCount, EncodeModeIndicator mode)
{
	int tmp;

	if (!data) return false;

	si->inputData = data;
	si->dataCount = dataCount;
	si->encodeMode = mode;
	
	tmp = getBitCount(dataCount, mode);
	si->encodedDataCount = tmp / 8 + ((tmp % 8) ? 1 : 0);

	return si_check_integrity(si);
}

bool si_set_eclevel(SymbolInfo* si, ECLevel level)
{
	if (!si || level < 1 || level > 4) return false;	
	si->ecLevel = level;

	return si_check_integrity(si);
}

bool si_check_integrity(SymbolInfo* si)
{
	if (!si) return false;

	int version = si->version;
	bool autoVersion = si->autoVersion;

	ECLevel ecLevel = si->ecLevel;

//	int dataCount = si->dataCount; // we dont need capacaty, we already have encodedDataCount with get_data_codewords for version autoselection
	int encodeDataCount = si->encodedDataCount;

	int totalCodeWords;
	int ecCodeWords;
	int dataCodeWords;
	int blocks;
	
	//set here, version if autoversion, encodeDatacount and eclevel
	//totalCodeWords, if version
	//blocks if version
	//eccodewords if version and eclevel
	//datacodewords if version and eclevel

}


void si_reset(SymbolInfo* si)
{
	si->version          = 0;
	si->autoVersion      = true;
	si->ecLevel          = EC_NONE;
	si->encodeMode       = 0;
	si->inputData        = NULL;
	si->dataCount        = 0;
	si->encodedDataCount = 0;
	si->totalCodeWords   = 0;
	si->ecCodeWords      = 0;
	si->dataCodeWords    = 0;
	si->blocks           = 0;
}


