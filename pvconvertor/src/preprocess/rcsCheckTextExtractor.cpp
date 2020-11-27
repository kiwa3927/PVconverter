



#undef yyFlexLexer
#define yyFlexLexer ppFlexLexer

#include <fstream>

#include "public/rcsTypes.h"
#include "public/manager/rcsManager.h"
#include "lexical/FlexLexer.h"

#include "rcsCheckTextExtractor.h"

rcsCheckTextExtractor_T::rcsCheckTextExtractor_T(const char *pFile) :
                                                 m_pFile(pFile)
{
}

void
rcsCheckTextExtractor_T::addText(const std::string &sFileName)
{
    std::ifstream reader(m_pFile);
    std::ofstream writer(sFileName.c_str());

    
    std::string sLineBuf;
    hvUInt32 nLineNo = 0;
    while(std::getline(reader, sLineBuf))
    {
        if(sLineBuf.find("||") != std::string::npos ||
           sLineBuf.find("&&") != std::string::npos)
        {
            std::string sValue;
            std::string::size_type i = 0;
            for(; i < sLineBuf.size(); ++i)
            {
                if(sLineBuf[i] == '|' || sLineBuf[i] == '&')
                {
                    char c = sLineBuf[i];
                    if(i + 1 < sLineBuf.size() && sLineBuf[i + 1] == c)
                    {
                        sValue += " ";
                        sValue += c;
                        sValue += c;
                        sValue += " ";
                        ++i;
                    }
                    else
                    {
                        sValue += c;
                    }
                }
                else
                {
                    sValue += sLineBuf[i];
                }
            }
            writer << sValue << std::endl;
        }
        else
        {
            writer << sLineBuf << std::endl;
        }

        ++nLineNo;
        if(!sLineBuf.empty())
        {
            sLineBuf += '\n';
            rcsManager_T::getInstance()->addText(nLineNo, sLineBuf);
        }
    }
    reader.close();
    writer.close();
}

void
rcsCheckTextExtractor_T::execute()
{
    std::string sTmpFile = "_svrf2pvrs_tmp_file__for_text_extract_";
    rcsManager_T::getInstance()->clearMapData();
    addText(sTmpFile);

    {
        std::ifstream reader(m_pFile);
        FlexLexer *pLexer = new ppFlexLexer(&reader);

        hvInt32 eType = pLexer->yylex();
        while(eType)
        {
            if(1 == eType)
            {
                rcsManager_T::getInstance()->addCheckComment(pLexer->lineno() - 1, pLexer->YYText());
            }
            eType = pLexer->yylex();
        }
        reader.close();
        remove(m_pFile);
        delete pLexer;
    }

    {
        std::ifstream reader(sTmpFile.c_str());
        std::ofstream writer(m_pFile);

        
        std::string sLineBuf;
        while(std::getline(reader, sLineBuf))
        {
            writer << sLineBuf << std::endl;
        }
        writer.flush();
        reader.close();
        writer.close();
        remove(sTmpFile.c_str());
    }
}
