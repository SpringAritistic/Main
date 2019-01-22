#include "stdafx.h"
#include "base64_tools.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


const static char base64_alphabet[64] =
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', //   0 -   9
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', //  10 -  19
	'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', //  20 -  29
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', //  30 -  39
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', //  40 -  49
	'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', //  50 -  59
	'8', '9', '+', '/'                                //  60 -  63
};

const static char base64_dealphabet[128] =
{
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //   0 -   9
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //  10 -  19
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //  20 -  29
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //  30 -  39
	0,  0,  0, 62,  0,  0,  0, 63, 52, 53, //  40 -  49
	54, 55, 56, 57, 58, 59, 60, 61,  0,  0, //  50 -  59
	0, 61,  0,  0,  0,  0,  1,  2,  3,  4, //  60 -  69
	5,  6,  7,  8,  9, 10, 11, 12, 13, 14, //  70 -  79
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, //  80 -  89
	25,  0,  0,  0,  0,  0,  0, 26, 27, 28, //  90 -  99
	29, 30, 31, 32, 33, 34, 35, 36, 37, 38, // 100 - 109
	39, 40, 41, 42, 43, 44, 45, 46, 47, 48, // 110 - 119
	49, 50, 51,  0,  0,  0,  0,  0          // 120 - 127
};


//////////////////////////////////////////////////////////////////////////

void encode_byte_triple(const char* input_buffer, unsigned int input_buffer_count_in_byte, char* output_buffer)
{
	unsigned int mask = 0xfc000000;
	unsigned int buffer = 0;

	char* temp = (char*)&buffer;
	temp[3] = input_buffer[0];
	if (input_buffer_count_in_byte > 1)
		temp[2] = input_buffer[1];
	if (input_buffer_count_in_byte > 2)
		temp[1] = input_buffer[2];

	switch (input_buffer_count_in_byte)
	{
	case 3:
	{
		output_buffer[0] = base64_alphabet[(buffer & mask) >> 26];
		buffer = buffer << 6;
		output_buffer[1] = base64_alphabet[(buffer & mask) >> 26];
		buffer = buffer << 6;
		output_buffer[2] = base64_alphabet[(buffer & mask) >> 26];
		buffer = buffer << 6;
		output_buffer[3] = base64_alphabet[(buffer & mask) >> 26];
		break;
	}
	case 2:
	{
		output_buffer[0] = base64_alphabet[(buffer & mask) >> 26];
		buffer = buffer << 6;
		output_buffer[1] = base64_alphabet[(buffer & mask) >> 26];
		buffer = buffer << 6;
		output_buffer[2] = base64_alphabet[(buffer & mask) >> 26];
		output_buffer[3] = '=';
		break;
	}
	case 1:
	{
		output_buffer[0] = base64_alphabet[(buffer & mask) >> 26];
		buffer = buffer << 6;
		output_buffer[1] = base64_alphabet[(buffer & mask) >> 26];
		output_buffer[2] = '=';
		output_buffer[3] = '=';
		break;
	}
	}
}

unsigned int decode_byte_quartet(const char* input_buffer, char* output_buffer)
{
	unsigned int buffer = 0;

	if (input_buffer[3] == '=')
	{
		if (input_buffer[2] == '=')
		{
			buffer = (buffer | base64_dealphabet[input_buffer[0]]) << 6;
			buffer = (buffer | base64_dealphabet[input_buffer[1]]) << 6;
			buffer = buffer << 14;

			char* temp = (char*)&buffer;
			output_buffer[0] = temp[3];

			return 1;
		}
		else
		{
			buffer = (buffer | base64_dealphabet[input_buffer[0]]) << 6;
			buffer = (buffer | base64_dealphabet[input_buffer[1]]) << 6;
			buffer = (buffer | base64_dealphabet[input_buffer[2]]) << 6;
			buffer = buffer << 8;

			char* temp = (char*)&buffer;
			output_buffer[0] = temp[3];
			output_buffer[1] = temp[2];

			return 2;
		}
	}
	else
	{
		buffer = (buffer | base64_dealphabet[input_buffer[0]]) << 6;
		buffer = (buffer | base64_dealphabet[input_buffer[1]]) << 6;
		buffer = (buffer | base64_dealphabet[input_buffer[2]]) << 6;
		buffer = (buffer | base64_dealphabet[input_buffer[3]]) << 6;
		buffer = buffer << 2;

		char* temp = (char*)&buffer;
		output_buffer[0] = temp[3];
		output_buffer[1] = temp[2];
		output_buffer[2] = temp[1];

		return 3;
	}

	return -1;
}

unsigned int base64_tools::calculate_encode_output_buffer_in_byte(
	unsigned int input_buffer_count_in_byte, bool auto_wrap/* = false*/)
{
	div_t result = div(input_buffer_count_in_byte, 3);

	unsigned int required_bytes = 0;
	if (result.rem == 0)
	{
		// Number of encoded characters  
		required_bytes = result.quot * 4;

		if (auto_wrap)
		{
			// CRLF -> "\r\n" each 76 characters  
			result = div(required_bytes, 76);
			required_bytes += result.quot * 2;
		}

		// Terminating null for the Encoded String  
		required_bytes += 1;

		return required_bytes;
	}
	else
	{
		// Number of encoded characters  
		required_bytes = result.quot * 4 + 4;

		if (auto_wrap)
		{
			// CRLF -> "\r\n" each 76 characters  
			result = div(required_bytes, 76);
			required_bytes += result.quot * 2;
		}

		// Terminating null for the Encoded String  
		required_bytes += 1;

		return required_bytes;
	}
}

unsigned int base64_tools::calculate_decode_output_buffer_in_byte(
	const char* input_buffer, unsigned int input_buffer_count_in_byte)
{
	// base64编码原理可知，编码后字符串最少为4个字符
	if (!input_buffer || input_buffer_count_in_byte < 4) return 0;

	unsigned int valid_ch_count = input_buffer_count_in_byte;
	unsigned int last_valid_index = input_buffer_count_in_byte - 1;
	
	// 编码时可能会每76个字符插入一个回车换行，解码时总是会去除回车换行符
	for (unsigned int i = 0; i < input_buffer_count_in_byte; ++i)
	{
		if ('\r' != input_buffer[i] && '\n' != input_buffer[i])
		{
			++valid_ch_count;
			last_valid_index = i;
		}
	}

	div_t result = div(valid_ch_count, 4);

	if (input_buffer[last_valid_index] != '=')
	{
		return result.quot * 3;
	}
	else
	{
		if (input_buffer[last_valid_index - 1] == '=')
			return result.quot * 3 - 2;
		else
			return result.quot * 3 - 1;
	}
}

void base64_tools::encode_buffer_to_string(const char* input_buffer,
	unsigned int input_buffer_count_in_byte, CString& output_text, bool auto_wrap/* = false*/)
{
	output_text = _T("");
	
	string tmp_text;
	encode_buffer_to_string(input_buffer, input_buffer_count_in_byte, tmp_text, auto_wrap);
	
	// 编码后的内容全部为半角字符，可以强制转换以节省转换时间，没有必要像东亚字符的转换一样考虑本地化转换
	for (unsigned int i = 0; i < tmp_text.length(); ++i)
		output_text += (TCHAR) tmp_text[i];
}

void base64_tools::encode_buffer_to_string(const char* input_buffer,
	unsigned int input_buffer_count_in_byte, std::string& output_text, bool auto_wrap/* = false*/)
{
	output_text = "";
	unsigned int required_count_in_byte = calculate_encode_output_buffer_in_byte(input_buffer_count_in_byte);
	if (required_count_in_byte == 0) return;

	char* output_buffer = new char[required_count_in_byte];
	encode_buffer(input_buffer, input_buffer_count_in_byte, output_buffer, auto_wrap);

	// 编码后的内容全部为半角字符，可以强制转换以节省转换时间，没有必要像东亚字符的转换一样考虑本地化转换
	for (unsigned int i = 0; i < required_count_in_byte; ++i)
		output_text += (char) output_buffer[i];

	safe_delete_array(output_buffer);
}

void base64_tools::encode_buffer_to_string(const std::vector<char>& input_buffer, std::string& output_text, bool auto_wrap /*= false*/)
{
	output_text = "";
	unsigned int required_count_in_byte = calculate_encode_output_buffer_in_byte(input_buffer.size());
	if (required_count_in_byte == 0) return;

	// 编码后的内容全部为半角字符，可以强制转换以节省转换时间，没有必要像东亚字符的转换一样考虑本地化转换
	output_text.resize(required_count_in_byte);
	encode_buffer(input_buffer.data(), input_buffer.size(), (char*)output_text.data(), auto_wrap);
}

void base64_tools::encode_buffer_to_string(const std::vector<char>& input_buffer, CString& output_text, bool auto_wrap /*= false*/)
{
	output_text = _T("");

	string tmp_text;
	encode_buffer_to_string(input_buffer, tmp_text, auto_wrap);

	// 编码后的内容全部为半角字符，可以强制转换以节省转换时间，没有必要像东亚字符的转换一样考虑本地化转换
	for (unsigned int i = 0; i < tmp_text.length(); ++i)
		output_text += (TCHAR) tmp_text[i];
}

void base64_tools::decode_string_to_buffer(const CString& input_text, std::vector<char>& output_buffer)
{
	output_buffer.clear();
	if (input_text.IsEmpty()) return;

	// 要转换的base64编码字符串一定为半角字符，可以强制转换以节省转换时间，没有必要像东亚字符的转换一样考虑本地化转换
	string tmp_text;
	for (int i = 0; i < input_text.GetLength(); ++i)
		tmp_text += (char)input_text[i];

	decode_string_to_buffer(tmp_text, output_buffer);
}

void base64_tools::decode_string_to_buffer(const std::string& input_text, std::vector<char>& output_buffer)
{
	output_buffer.clear();
	if (input_text.empty()) return;

	unsigned int required_count_in_byte = calculate_decode_output_buffer_in_byte(input_text.data(), input_text.length());
	if (required_count_in_byte == 0) return;

	output_buffer.resize(required_count_in_byte);
	unsigned int real_count = decode_buffer(input_text.data(), input_text.length(), output_buffer.data());
	output_buffer.resize(real_count);
}

void base64_tools::encode_buffer(const char* input_buffer,
	unsigned int input_buffer_count_in_byte, char* output_buffer, bool auto_wrap/* = false*/)
{
	unsigned int finished_byte_quartets_per_line = 0;
	unsigned int input_buffer_index = 0;
	unsigned int output_buffer_index = 0;

	memset(output_buffer, 0, calculate_encode_output_buffer_in_byte(input_buffer_count_in_byte));

	while (input_buffer_index < input_buffer_count_in_byte)
	{
		if (input_buffer_count_in_byte - input_buffer_index <= 2)
		{
			finished_byte_quartets_per_line++;
			encode_byte_triple(input_buffer + input_buffer_index, input_buffer_count_in_byte - input_buffer_index, output_buffer + output_buffer_index);
			break;
		}
		else
		{
			finished_byte_quartets_per_line++;
			encode_byte_triple(input_buffer + input_buffer_index, 3, output_buffer + output_buffer_index);
			input_buffer_index += 3;
			output_buffer_index += 4;
		}

		if (finished_byte_quartets_per_line == 19 && auto_wrap)
		{
			output_buffer[output_buffer_index] = '\r';
			output_buffer[output_buffer_index + 1] = '\n';
			output_buffer += 2;
			finished_byte_quartets_per_line = 0;
		}
	}
}

unsigned int base64_tools::decode_buffer(const char* input_buffer,
	unsigned int input_buffer_count_in_byte, char* output_buffer)
{
	if (!input_buffer || input_buffer_count_in_byte < 4 || !output_buffer) return 0;

	unsigned int input_buffer_index = 0;
	unsigned int output_buffer_index = 0;

	char byte_quartet[4];

	while (input_buffer_index < input_buffer_count_in_byte)
	{
		for (int i = 0; i < 4; i++)
		{
			byte_quartet[i] = input_buffer[input_buffer_index];

			// Ignore all characters except the ones in BASE64_ALPHABET  
			if ((byte_quartet[i] >= 48 && byte_quartet[i] <= 57) ||
				(byte_quartet[i] >= 65 && byte_quartet[i] <= 90) ||
				(byte_quartet[i] >= 97 && byte_quartet[i] <= 122) ||
				byte_quartet[i] == '+' || byte_quartet[i] == '/' || byte_quartet[i] == '=')
			{
				// nothing;
			}
			else
			{
				// Invalid character  
				--i;
			}

			++input_buffer_index;
		}

		output_buffer_index += decode_byte_quartet(byte_quartet, output_buffer + output_buffer_index);
	}

	// output_buffer_index gives us the next position of the next decoded character  
	// inside our output buffer and thus represents the number of decoded characters  
	// in our buffer.  
	return output_buffer_index;
}
