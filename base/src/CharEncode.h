/*
 * CharEncode.h
 *
 *  Created on: 2015年9月11日
 *      Author: dengxu
 */

#ifndef BASE_SRC_CHARENCODE_H_
#define BASE_SRC_CHARENCODE_H_

#include<iconv.h>

namespace lynetx {

class CharEncode
{
public:
	CharEncode();
	virtual ~CharEncode();
public:
	bool InitEncode(const char *toCode="UTF-8", const char *fromCode="GBK");
	bool DestoryEncode();
	size_t Convert(char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);
	static CharEncode *Instance(){static CharEncode en;return &en;};
private:
	iconv_t m_cd;
};

} /* namespace lynetx */

#endif /* BASE_SRC_CHARENCODE_H_ */
