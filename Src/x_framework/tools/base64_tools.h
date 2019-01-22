//  *********************************************************************
//  * Base 64 de- and encoding class                                    *
//  *                                                                   *
//  * ================================================================= *
//  * fixed decode bug while encode include CRLF on 07.10.2017          *
//  * added encode with CRLF flag on 07.10.2017                         *
//  * by Wang You-zhou [wyz5@163.com]                                   *
//  *                                                                   *
//  * ================================================================= *
//  * This class was wrapped on 28.03.2017                              *
//  * by Wang You-zhou [wyz5@163.com]                                   *
//  *                                                                   *
//  * ================================================================= *
//  * This class was written on 28.05.2003                              *
//  * by Jan Raddatz [jan-raddatz@web.de]                               *
//  *                                                                   *
//  * ================================================================= *
//  * Copyright (c) by Jan Raddatz                                      *
//  * This class was published @ codeguru.com                           *
//  * 28.05.2003                                                        *
//  *                                                                   *
//  *********************************************************************

#pragma once  
#ifndef __HMGLOBALTOOL_TOOLS_BASE64_TOOLS_H__
#define __HMGLOBALTOOL_TOOLS_BASE64_TOOLS_H__

#include <vector>

class X_FRAMEWORK_API base64_tools
{
private:
	base64_tools() = delete;
	~base64_tools() = delete;

public:
	// 编码或解码缓冲区内容，输入、输出缓存区均由外部预先创建好（请确保空间足够大）
	// 编码是返回的缓冲区会在最后面加一个字节并填充为'\0'
	// base64编码在有些领域（如通信）会有相应的行业标准限定编码超过76个字符时要换行（插入"\r\n"），以保证每行的
	// 字符不超过76个字符对于这类情况，可以通过auto_wrap为设置，如果为true表示会自动换行，否则不插入换行
	// 注意：解码时会去跳过非base64编码字符（如回车、换行等），因此解码后实际用到的缓存冲区字节数可能会比调
	//      用calculate_decode_output_buffer_in_byte计算出来的少，因此要返回实际解码得到的字节数，编码时计
	//      算得到的空间大小是真实的，因此不需要返回最终的结果数
	static void encode_buffer(const char* input_buffer, unsigned int input_buffer_count_in_byte, char* output_buffer, bool auto_wrap/* = false*/);
	static unsigned int decode_buffer(const char* input_buffer, unsigned int input_buffer_count_in_byte, char* output_buffer);

	// 计算编码或解码所需的缓存区，内存由接受者负责管理(解码时需要加密内容才能计算所需缓冲区字节数)
	// 注意，此处计算出来的结果可能比实际解码后的长度还要大，因为此处只是按理论算法来计算结果的长度，实际解码时还要考虑去除非法字符等操作
	static unsigned int calculate_encode_output_buffer_in_byte(unsigned int input_buffer_count_in_byte, bool auto_wrap = false);
	static unsigned int calculate_decode_output_buffer_in_byte(const char* input_buffer, unsigned int input_buffer_count_in_byte);

	// 内存数据按base64进行编码，或将base64编码解码为内存数据
	// 由于base64编码不存在东亚字符的unicode编码差异问题，因此不unicode模式下强烈推荐使用std::string作为参数，
	// 而不是通用字符串，这样可以节省一半的内存
	static void encode_buffer_to_string(const char* input_buffer, unsigned int input_buffer_count_in_byte, CString& output_text, bool auto_wrap = false);
	static void encode_buffer_to_string(const char* input_buffer, unsigned int input_buffer_count_in_byte, std::string& output_text, bool auto_wrap = false);
	static void encode_buffer_to_string(const std::vector<char>& input_buffer, CString& output_text, bool auto_wrap = false);
	static void encode_buffer_to_string(const std::vector<char>& input_buffer, std::string& output_text, bool auto_wrap = false);
	static void decode_string_to_buffer(const CString& input_text, std::vector<char>& output_buffer);
	static void decode_string_to_buffer(const std::string& input_text, std::vector<char>& output_buffer);
};
#endif //__HMGLOBALTOOL_TOOLS_BASE64_TOOLS_H__
