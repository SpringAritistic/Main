#pragma once
#ifndef __HMGLOBALTOOL_TOOLS_MD5_TOOLS_H__	
#define __HMGLOBALTOOL_TOOLS_MD5_TOOLS_H__

#include <string>
#include <istream>


/* MD5 declaration. */
class X_FRAMEWORK_API md5_value
{
public:
	md5_value();
	md5_value(const void* input, size_t length);
	md5_value(const std::string& str);
	md5_value(std::ifstream& in);
	void update(const void* input, size_t length);
	void update(const std::string& str);
	void update(std::ifstream& in);
	const unsigned char* digest();
	std::string to_string();
	void reset();

public:
	// 将一个字符串进行MD5加密，如果无法正常完成加密，则返回空字符串，count 为加密次数
	// 不返回原字符串是为防止不检测返回值导致明文误用造成泄漏
	static void make_md5_value(const CString& text, CString& result, int count = 1);
	static CString make_md5_value(const CString& text, int count = 1);

private:
	void update(const unsigned char* input, size_t length);
	void final();
	void transform(const unsigned char block[64]);
	void encode(const unsigned long* input, unsigned char* output, size_t length);
	void decode(const unsigned char* input, unsigned long* output, size_t length);
	std::string bytes_to_hex_string(const unsigned char* input, size_t length);

	/* class uncopyable */
	md5_value(const md5_value&);
	md5_value& operator=(const md5_value&);

private:
	unsigned long m_state[4];	/* state (ABCD) */
	unsigned long m_count[2];	/* number of bits, modulo 2^64 (low-order word first) */
	unsigned char m_buffer[64];	/* input buffer */
	unsigned char m_digest[16];	/* message digest */
	bool m_finished;		/* calculate finished ? */

	static const unsigned char PADDING[64];	/* padding for calculate */
	static const char HEX[16];
	static const size_t BUFFER_SIZE = 1024;
};

#endif //__HMGLOBALTOOL_TOOLS_MD5_TOOLS_H__
