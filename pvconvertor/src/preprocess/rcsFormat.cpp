

#include <string.h>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

#include "public/rcsTypes.h"
#include "rcsFormat.h"

rcsFormat::rcsFormat(const char *pFileName):m_pFileName(pFileName)
{
}

bool
rcsFormat::execute()
{
    std::ifstream reader(m_pFileName);
    if(!reader)
    {
        


    }

    std::ostringstream os;
    os << reader.rdbuf();
    std::string sLineBuf = os.str();

	formatTvfFunWithoutComment(sLineBuf);
	formatTvfFunForSetLayer(sLineBuf);

	std::ofstream writer(m_pFileName);

	writer << sLineBuf;

	writer.flush();
	reader.close();
	writer.close();

    return true;
}

void
rcsFormat::formatTvfFunWithoutComment(std::string &sLineBuf)
{
	std::string::size_type begin = 0;
	std::string::size_type insertPos = 0;

	while((begin = sLineBuf.find("TVF FUNCTION", begin)) != std::string::npos)
	{
		std::string::size_type nextTvfFun = 0;
		if((nextTvfFun = sLineBuf.find("TVF FUNCTION", begin + 12)) == std::string::npos)
		{
			nextTvfFun = sLineBuf.size();
		}
		std::string singleTvfFunStr = sLineBuf.substr(begin, nextTvfFun - begin);
		std::string::size_type localBegin = 0;
		if((insertPos = singleTvfFunStr.find_first_of("[", localBegin)) != std::string::npos)
		{
			localBegin = insertPos;
			if(singleTvfFunStr.find("/*", localBegin) != std::string::npos)
				return;
			else if((localBegin = singleTvfFunStr.find_first_of("tvf::", localBegin)) != std::string::npos)
			{
				singleTvfFunStr.insert(insertPos + 1, "/*");
				if((insertPos = singleTvfFunStr.find_last_of("]")) != std::string::npos)
				{
					localBegin = insertPos;
					singleTvfFunStr.insert(insertPos - 1, "*/");
				}

				sLineBuf.replace(begin, nextTvfFun -begin, singleTvfFunStr);
				begin = nextTvfFun;
			}
		}
	}
}

void
rcsFormat::formatTvfFunForSetLayer(std::string &sLineBuf)
{
	std::string::size_type begin = 0;
	while((begin = sLineBuf.find("=", begin)) != std::string::npos)
	{
		
		if(sLineBuf.find("text \"") != std::string::npos)
		{
			break;
		}
		
		if(sLineBuf[++begin] != ' ')
		{
			if(sLineBuf[begin] == '=')
			{
				begin++;
				continue;
			}
			else
			{
				sLineBuf.insert(begin, " ");
			}
		}
	}
}

