/*
 * CharEncode.cpp
 *
 *  Created on: 2015年9月11日
 *      Author: dengxu
 */

#include "CharEncode.h"
#include "logdef.h"

namespace lynetx {

CharEncode::CharEncode() :
		m_cd(0)
{

}

CharEncode::~CharEncode()
{
	if(this->m_cd > 0)
	{
		DestoryEncode();
	}
}

bool CharEncode::InitEncode(const char *toCode, const char *fromCode)
{
	if(this->m_cd <=0)
	{
		m_cd = iconv_open(toCode, fromCode);
		if(m_cd == (iconv_t)(-1))
		{
			TRACE_ERR(LOG_ERRORS, errno, 100, "iconv_open is error");
			return false;
		}
		return true;
	}
	return false;
}

bool CharEncode::DestoryEncode()
{
	if(this->m_cd > 0)
	{
		int ret = iconv_close(this->m_cd);
		if(ret != 0)
		{
			TRACE_ERR(LOG_ERRORS, errno, 100, "iconv_close is error");
			return false;
		}
		return true;
	}
	return false;
}

size_t CharEncode::Convert(char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft)
{
	size_t ret = iconv(m_cd, inbuf, inbytesleft, outbuf, outbytesleft);
	if(ret == -1)
	{
		TRACE_ERR(LOG_ERRORS, errno, 100,"Convert of iconv is error");
	}
	return ret;
}


} /* namespace lynetx */
