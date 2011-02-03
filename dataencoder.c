#include "dataencoder.h"
#include "util.h"

#include <stdio.h>

void encodeByte(bitstream* bs, const byte* data, int dataSize);
void encodeNumeric(bitstream* bs, const byte* data, int dataSize);
void encodeAlpha(bitstream* bs, const byte* data, int dataSize);

byte alphaValue(byte alpha);


void encodeData(bitstream* bs, const byte* data, EncodeModeIndicator mode, int dataSize)
{
	void (*encode)(bitstream* bs, const byte* data, int dataSize) = NULL; /* Mode specific encode routine */
	CharacterCountBitsCount ccbc = 0           ;                          /* Mode specific size of Character Count Information */

	switch(mode) {
	case ModeByte:
		ccbc = CountByte;
		encode = &encodeByte;
	break;
	case ModeAlpha:
		ccbc = CountAlpha;
		encode = &encodeAlpha;
	break;
	case ModeNumeric:
		ccbc = CountNumeric;
		encode = &encodeNumeric;
	break;
	default:
		fprintf(stderr, "Mode not supported: %d", mode);
		return;
	}

	bs_add_b(bs, mode, 4);               /* add Mode information*/
	bs_add_i(bs, dataSize, ccbc);        /* add Character count information */
	(*encode)(bs, data, dataSize);       /* add data, mode speicific */
	bs_add_b(bs, Terminator, 4);         /* add terminator */
	bs_add_b(bs, 0, 8 - (bs->size % 8)); /* fill up remaining bits */
}


int getBitCount(int numChars, EncodeModeIndicator mode)
{
	switch(mode) {
	case ModeNumeric:
		return 4 + CountNumeric + 10 * (numChars / 3) + ((numChars % 3 == 0) ? 0 : ((numChars % 3 == 1) ? 4 : 7));
	case ModeAlpha:
		return 4 + CountAlpha + 11 * (numChars / 2) + 6 *(numChars % 2);
	case ModeByte:
		return 4 + CountByte + 8 * numChars;
	}
	return 0;
}

void encodeByte(bitstream* bs, const byte* data, int dataSize)
{
	bs_add_bs(bs, data, dataSize, dataSize*8);
}

void encodeNumeric(bitstream* bs, const byte* data, int dataSize)
{
	int i;
	unsigned int block; 
	int blockSize = 10;
	byte* tdata = malloc(dataSize);
	byte* tmp = tdata;

	//change from alpha to int
	for (i = 0; i < dataSize; i++) {
		if (data[i]< '0' || data[i] > '9') {
			fprintf(stderr, "Not a number: %c", data[i]);
			return; 
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
}

void encodeAlpha(bitstream* bs, const byte* data, int dataSize)
{
	unsigned int block; 
	int blockSize = 11;
	// group 2 chars together c1*45+c2
	while(dataSize >= 2) {
		block = alphaValue(*data) * 45 + alphaValue(*(data + 1));
		printf("%d\n", block);
		dataSize -= 2;
		data += 2;
		bs_add_i(bs, block, blockSize);
	}
	// add last char if there is one
	if (dataSize) {
		block = alphaValue(*data);
		bs_add_i(bs, block, blockSize - 5);
		printf("%d\n", block);
	}
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
	fprintf(stderr, "Character not usable in alpha mode: %c", alpha);

	return -1;

}
