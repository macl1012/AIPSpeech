#ifndef __AIP_SPEECH_H__
#define __AIP_SPEECH_H__

#include "base/base.h"

#include <json/json.h>

namespace aip {
    
    class Speech : public AipBase
    {
    public:
        
        std::string _asr = "https://vop.baidu.com/server_api";
        
        std::string _tts = "http://tsn.baidu.com/text2audio";
        
		Speech(const std::string app_id, const std::string & ak, const std::string & sk) : AipBase(app_id, ak, sk)
		{

		}

		Json::Value request_tts(const std::string url, std::map<std::string, std::string> & data, std::string & file_content)
		{
			std::string response;
			Json::Value obj;
			Json::Value file_json;
			int status_code = this->client.post(url, nullptr, data, nullptr, &response);
			if (status_code != CURLcode::CURLE_OK) {
				obj[aip::CURL_ERROR_CODE] = status_code;
				return obj;
			}
			std::string error;
			std::unique_ptr<Json::CharReader> reader(crbuilder.newCharReader());
			reader->parse(response.data(), response.data() + response.size(), &obj, &error);

			if (error.empty()) {
				// 接口返回错误信息
				obj[aip::CURL_ERROR_CODE] = 0;
			}
			else {
				// 接口返回语音文件
				file_content = response;
			}
			return obj;
		}

#ifndef _MSC_VER
		inline int code_convert(char *from_charset, char *to_charset, char *inbuf, size_t inlen,
			char *outbuf, size_t outlen) {
			iconv_t cd;
			char **pin = &inbuf;
			char **pout = &outbuf;

			cd = iconv_open(to_charset, from_charset);
			if (cd == 0)
				return -1;
			memset(outbuf, 0, outlen);
			if (iconv(cd, pin, &inlen, pout, &outlen) == -1)
				return -1;
			iconv_close(cd);
			*pout = '\0';

			return 0;
		}
#endif

		std::string gbkToUtf8(const std::string& strGbk)//传入的strGbk是GBK编码
		{
#ifdef _MSC_VER
			// GBK转Unicode  
			int len = MultiByteToWideChar(CP_ACP, 0, strGbk.c_str(), -1, NULL, 0);
			wchar_t *strUnicode = new wchar_t[len];
			wmemset(strUnicode, 0, len);
			MultiByteToWideChar(CP_ACP, 0, strGbk.c_str(), -1, strUnicode, len);

			// Unicode转UTF-8  
			len = WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, NULL, 0, NULL, NULL);
			char * strUtf8 = new char[len];
			WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, strUtf8, len, NULL, NULL);

			std::string strTemp(strUtf8);//此时的strTemp是UTF-8编码  
			delete[]strUnicode;
			delete[]strUtf8;
			strUnicode = NULL;
			strUtf8 = NULL;
			return strTemp;
#else 
			char inBuf[4096];
			char outBuf[4096];
			memcpy(inBuf, strGbk.c_str(), strGbk.length());

			code_convert("gb2312", "utf-8", inBuf, strlen(inBuf), outBuf, sizeof(outBuf));

			return std::string(outBuf);
#endif
		}

        Json::Value text2audio(const std::string & text, std::map<std::string, std::string> const & options, std::string & file_content)
		{
			std::map<std::string, std::string> data;
			std::map<std::string, std::string>::const_iterator it;

			for (it = options.begin(); it != options.end(); it++)
			{
				data[it->first] = it->second;
			}

			std::string token;
			int i = 0;
			for (; i < 3; i++) // 尝试3次
			{
				if (this->getAccessToken(token))
					break;
			}
			printf("[%d] accessToken: %s\n", i, token.c_str());
			

			data["tex"] = gbkToUtf8(text);
			data["lan"] = "zh";
			data["ctp"] = "1";
			data["tok"] = token;
			data["cuid"] = this->getAk();

			Json::Value result = this->request_tts(_tts, data, file_content);
			return result;
		}
        
    };
}
#endif
