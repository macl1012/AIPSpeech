// 请替换您下载的C++SDK路径
#include "speech.h"
#include <fstream>

struct tts_params
{
	char* pText;
	char* pAudioFile;
	char* pVolume;
};

tts_params opt;

bool TTS(aip::Speech* client, tts_params opt);

// =======================================================================
// 命令行解析
// =======================================================================

int opterr = 1,			/* if error message should be printed */
optind = 1,			    /* index into parent argv vector */
optopt,				    /* character checked for validity */
optreset;			    /* reset getopt */
char *optarg;			/* argument associated with option */

#define	BADCH	(int)'?'
#define	BADARG	(int)':'
#define	EMSG	""


/*
* getopt --
*	Parse argc/argv argument vector.
*/
int getopt(int nargc, char * const *nargv, const char *ostr)
{
#define __progname nargv[0]
	static char *place = EMSG;	/* option letter processing */
	char *oli;			/* option letter list index */

	if (optreset || !*place) {	/* update scanning pointer */
		optreset = 0;
		if (optind >= nargc || *(place = nargv[optind]) != '-') {
			place = EMSG;
			return (-1);
		}
		if (place[1] && *++place == '-') {	/* found "--" */
			++optind;
			place = EMSG;
			return (-1);
		}
	}

	/* option letter okay? */
	if ((optopt = (int)*place++) == (int) ':' ||
		!(oli = strchr((char*)ostr, optopt))) {
		/*
		* if the user didn't specify '-' as an option,
		* assume it means -1.
		*/
		if (optopt == (int) '-')
			return (-1);
		if (!*place)
			++optind;
		if (opterr && *ostr != ':')
			(void)fprintf(stderr,
				"%s: illegal option -- %c\n", __progname, optopt);
		return (BADCH);
	}

	if (*++oli != ':') {		/* don't need argument */
		optarg = NULL;
		if (!*place)
			++optind;
	}
	else {			/* need an argument */
		if (*place)			/* no white space */
			optarg = place;
		else if (nargc <= ++optind) {	/* no arg */
			place = EMSG;
			if (*ostr == ':')
				return (BADARG);
			if (opterr)
				(void)fprintf(stderr,
					"%s: option requires an argument -- %c\n",
					__progname, optopt);
			return (BADCH);
		}
		else			/* white space */
			optarg = nargv[optind];
		place = EMSG;
		++optind;
	}
	return (optopt);		/* dump back option letter */

}

int parse_cmdline(int argc, char **argv, tts_params *params)
{
	/* parse the command line */
#ifdef _MSC_VER
	while (1) {
		int c = getopt(argc, argv, "t:f:v:");
		if (c == -1)
			break;
		switch (c)
		{
		case 't':
		{
			params->pText = optarg;
			printf("pText = %s ", params->pText);
		}
			break;
		case 'f':
		{
			params->pAudioFile = optarg;
			printf("pAudioFile = %s ", params->pAudioFile);
		}
			break;
		case 'v':
		{
			params->pVolume = optarg;
			printf("pVolume = %s ", params->pVolume);
		}
			break;
		default:
			fprintf(stderr, "WARNING -> this option is not valid \"-%c %s\"\n", c, optarg);
			break;
		}
	}
#else
	if (argc < 3)
	{
		fprintf(stderr, "ERROR -> required argument is missing\n");
		return 1;
	}
	params->pText = argv[1];
	printf("pText = %s ", params->pText);

	params->pAudioFile = argv[2];
	printf("pAudioFile = %s ", params->pAudioFile);

	params->pVolume = argv[3];
	printf("pVolume = %s ", params->pVolume);

#endif
	/* check for possible errors */
	if (0 == params->pText || 0 == params->pAudioFile || 0 == params->pVolume)
	{
		fprintf(stderr, "ERROR -> required argument is missing\n");
		return 1;
	}

	printf("\n");
	return 0;
}

// =======================================================================
// help显示
// =======================================================================

void convert_help_display()
{
	fprintf(stdout, "Usage: \n----------------------------\n\n");

	fprintf(stdout, "tts.exe -t 语音合成 -f audio.mp3 -v 10 \n");
	fprintf(stdout, "	-t	text to speech\n");
	fprintf(stdout, "	-f	output mp3 audio file\n");
	fprintf(stdout, "	-v	volume\n");
	fprintf(stdout, "\n");
}

int main(int argc, char **argv)
{
	opt.pText = 0;
	if (0 != parse_cmdline(argc, argv, &opt))
	{
		convert_help_display();
		return -1;
	}

    // 务必替换百度云控制台中新建百度语音应用的 Api Key 和 Secret Key
    aip::Speech * client = new aip::Speech("15997909", "iQLItbHceFlx5SXfGn2tyI3H", "H2ttCoHvyLgxGQCBUWi4s8zGj0u9pPLb");

	if (!TTS(client, opt))
		return -1;
    
    return 0;
}

// 语音合成错误时，添加日志到文件
static void writeErrorToFile(std::string strError)
{
	std::ofstream fs;
	fs.open("aip_error.log", std::ios::in | std::ios::app);
	std::string strOutput = aip::utc_time(aip::now()) + "] tts failed, result: \n" + strError;
	fs.write(strOutput.c_str(), strOutput.length());
	fs.close();
}

/**
 * TTS语音合成示例
 */
bool TTS(aip::Speech* client, tts_params opt) 
{
    std::ofstream ofile;
    std::string file_ret;
    std::map<std::string, std::string> options;
    options["spd"] = "5";
	options["pit"] = "5";
	options["vol"] = opt.pVolume;
    options["per"] = "0";
    ofile.open(opt.pAudioFile, std::ios::out | std::ios::binary);
    Json::Value result = client->text2audio(opt.pText, options, file_ret);
    // 如果file_ret为不为空则说明合成成功，返回mp3文件内容
    if (!file_ret.empty())
    {
        // 合成成功保存文件
        ofile << file_ret;
        std::cout << "语音合成成功" << std::endl;
		return true;
    } else {
        // 合成出错，打印错误信息
		std::string error = result.toStyledString();
		std::cout << error;
		writeErrorToFile(error);

		return false;
    }
}
