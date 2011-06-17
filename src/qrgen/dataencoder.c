#include "dataencoder.h"
#include "symbolinfo.h"
#include "util.h"

#include <stdio.h>
#include <math.h>

bool encodeByte(bitstream* bs, const byte* data, int dataSize);
bool encodeNumeric(bitstream* bs, const byte* data, int dataSize);
bool encodeAlpha(bitstream* bs, const byte* data, int dataSize);

byte alphaValue(byte alpha);


bool encodeData(bitstream* bs, const SymbolInfo* si)
{
	if (!bs || !si) return false;


	bool (*encode)(bitstream* bs, const byte* data, int dataSize) = NULL; /* Mode specific encode routine */
	int ccbc = 0;                                                         /* Mode specific size of Character Count Information */

	switch(si->encodeMode) {
	case ModeByte:
		encode = &encodeByte;
	break;
	case ModeAlpha:
		encode = &encodeAlpha;
	break;
	case ModeNumeric:
		encode = &encodeNumeric;
	break;
	default:
		error("Mode not supported: %d", si->encodeMode);
		return false;
	}

	ccbc = getCharacterCountBitCount(si->encodeMode, si->version);

	bs_add_b(bs, si->encodeMode, 4);                          /* add Mode information*/
	bs_add_i(bs, si->dataCount, ccbc);                        /* add Character count information */
	bool ret = (*encode)(bs, si->inputData, si->dataCount);   /* add data, mode speicific */
	if (!ret) return false;

	// terminator, doesn't need to be added if
	// the data fills out the whole capacaty
	int bitsLeft = si->totalCodeWords - bs->size;
	if (bitsLeft > 0) {
		int terminatorLen = ((bitsLeft > 4) ? 4 : bitsLeft);
		bs_add_b(bs, Terminator, terminatorLen);              /* add terminator */
	}

	if (bs->size % 8) bs_add_b(bs, 0, 8 - (bs->size % 8));    /* fill up remaining bits of last byte */

	bool b = false;
	while(bs->size / 8 < si->dataCodeWords) {
		bs_add_b(bs, Padding[b], 8);                          /* add padding bytes */
		b = !b;
	}

	return true;
}


int getBitCount(int numChars, EncodeModeIndicator mode, int version)
{
	switch(mode) {
	case ModeNumeric:
		return 4 + getCharacterCountBitCount(mode, version) + 10 * (numChars / 3) + ((numChars % 3 == 0) ? 0 : ((numChars % 3 == 1) ? 4 : 7));
	case ModeAlpha:
		return 4 + getCharacterCountBitCount(mode, version) + 11 * (numChars / 2) + 6 *(numChars % 2);
	case ModeByte:
		return 4 + getCharacterCountBitCount(mode, version) + 8 * numChars;
	}
	return 0;
}

int getByteCount(int numChars, EncodeModeIndicator mode, int version)
{
	int tmp = getBitCount(numChars, mode, version);
	return tmp / 8 + ((tmp % 8) ? 1 : 0);
}

bool encodeByte(bitstream* bs, const byte* data, int dataSize)
{
	bs_add_bs(bs, data, dataSize, dataSize*8);
	return true;
}

bool encodeNumeric(bitstream* bs, const byte* data, int dataSize)
{
	int i;
	unsigned int block; 
	int blockSize = 10;
	byte* tmp = malloc(dataSize);
	byte* tdata = tmp;

	//change from alpha to int
	for (i = 0; i < dataSize; i++) {
		if (data[i]< '0' || data[i] > '9') {
			error("Not a number: %c", data[i]);
			return false; 
		}
		tdata[i] = data[i] - '0';
	}

	// group 3 digits together
	while(dataSize >= 3) {
		block = *tdata * 100 + *(tdata + 1) * 10 + *(tdata + 2);
		dataSize -= 3;
		tdata += 3;
		bs_add_i(bs, block, blockSize);
	}
	// add last 2 or 1 remaining digit
	if (dataSize == 2) {
		block = *tdata * 10 + *(tdata + 1);
		bs_add_i(bs, block, blockSize - 3);
	} else if (dataSize == 1) {
		bs_add_i(bs, *tdata, blockSize - 6);
	}

	free(tmp);
	tmp = NULL;
	tdata = NULL;

	return true;
}

bool encodeAlpha(bitstream* bs, const byte* data, int dataSize)
{
	unsigned int block; 
	int blockSize = 11;
	// group 2 chars together c1*45+c2
	while(dataSize >= 2) {
		byte a1 = alphaValue(*data);
		byte a2 = alphaValue(*(data + 1));
		if (a1 == (byte)-1 || a2 == (byte)-1) return false;
		block = a1 * 45 + a2;
		dataSize -= 2;
		data += 2;
		bs_add_i(bs, block, blockSize);
	}
	// add last char if there is one
	if (dataSize) {
		block = alphaValue(*data);
		if (block == -1) return false;
		bs_add_i(bs, block, blockSize - 5);
	}

	return true;
}

byte alphaValue(byte alpha)
{

	/*
	 * Characters need to be mapped:
	 * 0->0 A->10 K->20 U->30 +->40 
	 * 1->1 B->11 L->21 V->31 -->41
	 * 2->2 C->12 M->22 W->32 .->42
	 * 3->3 D->13 N->23 X->33 /->43
	 * 4->4 E->14 O->24 Y->34 :->44
	 * 5->5 F->15 P->25 Z->35
	 * 6->6 G->16 Q->26 ' '->36
	 * 7->7 H->17 R->27 $->37
	 * 8->8 I->18 S->28 %->38
	 * 9->9 J->19 T->29 *->39
	 */

	if (alpha >= '0' && alpha <= '9') return alpha - '0';
	if (alpha >= 'a' && alpha <= 'z') return alpha - 'a' + 10;
	if (alpha >= 'A' && alpha <= 'Z') return alpha - 'A' + 10;
	byte others[] = {' ', '$', '%', '*', '+', '-', '.', '/', ':'};
	byte ret = 36;
	int i;
	for (i = 0; i < 9; i++) {
		if (others[i] == alpha) return ret+i;
	}
	error("Character not usable in alpha mode: %c", alpha);

	return -1;
}

int getCharacterCountBitCount(EncodeModeIndicator mode, int version)
{
/*
 * Version    Numeric   Alpha   Byte
 * 1 -  9     10         9       8
 * 10 - 26    12        11      16
 * 27 - 40	  13        13      16
 */
	switch(mode)
	{
	case ModeNumeric:
		if (version >= 1 && version <= 9)
			return 10;
		else if (version >= 10 && version <= 26)
			return 12;
		else if (version >= 27 && version <= 40)
			return 13;
		break;
	case ModeAlpha:
		if (version >= 1 && version <= 9)
			return 9;
		else if (version >= 10 && version <= 26)
			return 11;
		else if (version >= 27 && version <= 40)
			return 13;
		break;
	case ModeByte:
		if (version >= 1 && version <= 9)
			return 8;
		else if (version >= 10 && version <= 26)
			return 16;
		else if (version >= 27 && version <= 40)
			return 16;
		break;
	default:
		error("Mode %d invalid", mode);
		return -1;
	}
	error("Version  %d invalid", version);
	return -1;
}
