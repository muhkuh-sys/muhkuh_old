/*
 *    Copyright (C) 2009 Christoph Thelen
 *
 *    This library is free software; you can redistribute it and/or modify it 
 *    under the terms of the GNU Library General Public License as published 
 *    by the Free Software Foundation; either version 2 of the License, or 
 *    (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Library General Public License for more details.
 *
 *    You should have received a copy of the GNU Library General Public
 *    License along with this library; if not, write to the
 *    Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *    Boston, MA 02111-1307, USA.
 */


#include "libdefs.h"

#ifdef ENABLE_CRC16

#include "mhash_crc16.h"


static unsigned int crc16(mutils_word16 usCrc, mutils_word8 ucData)
{
	usCrc  = (usCrc >> 8) | ((usCrc & 0xff) << 8);
	usCrc ^= ucData;
	usCrc ^= (usCrc & 0xff) >> 4;
	usCrc ^= (usCrc & 0x0f) << 12;
	usCrc ^= ((usCrc & 0xff) << 4) << 1;

	return usCrc;
}


void Crc16_Init(struct Crc16Context *context)
{
	context->checksum = 0;
}

void Crc16_Update(struct Crc16Context *context, mutils_word8 __const *buf, mutils_word32 len)
{
	mutils_word8 __const *pucCnt;
	mutils_word8 __const *pucEnd;
	mutils_word16 checksum;


	checksum = context->checksum;

	pucCnt = buf;
	pucEnd = pucCnt + len;

	while(pucCnt<pucEnd)
	{
		checksum = crc16(checksum, *(pucCnt++));
	}

	context->checksum = checksum;
}

void Crc16_Final(struct Crc16Context *context, mutils_word8 *digest)
{
	mutils_word32 tmp;

	tmp = context->checksum;

#if defined(WORDS_BIGENDIAN)
	tmp = mutils_word32swap(tmp);
#endif
	if (digest != NULL)
	{
		mutils_memcpy(digest, &tmp, sizeof(mutils_word16));	
	}

	memset(context, 0, sizeof(struct Crc16Context));
}


#endif /* ENABLE_CRC16 */
