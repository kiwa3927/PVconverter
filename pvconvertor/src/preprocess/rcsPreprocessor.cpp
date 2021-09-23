



#include <string.h>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

#include "public/manager/rcErrorManager.h"
#include "public/manager/rcsManager.h"
#include "public/synnode/rcsSynRootNode.h"
#include "public/util/rcsStringUtil.hpp"
#include "main/rcsSvrf2Pvrs.h"
#include "tvf/rcsTvfCompiler.h"

#include "rcsPreprocessor.h"

std::set<std::string>              rcsPreProcessor_T::m_sgsetFileCollection;
hvUInt32                           rcsPreProcessor_T::m_sgnGlobalLineNo = 0;
rcsPreProcessor_T::DEFINITION_MAP     rcsPreProcessor_T::m_sgmapNameDefinition;
rcsPreProcessor_T::CONDITIONALS_STACK rcsPreProcessor_T::m_sgvConditionalsStack;
bool                               rcsPreProcessor_T::m_sgisEnabled = true;

rcsPreProcessor_T::rcsPreProcessor_T(const char *pFileName, bool convertSwitch,
                                     std::ofstream &fOutStream) :
                m_convertSwitch(convertSwitch), m_pFileName(pFileName), m_fOutStream(fOutStream)
{
	m_vIncludeFile.clear();
}

static CONDITIONALS_TYPE
judgeFirstDirective(const std::string &sLineValue)
{
    if(sLineValue.empty() || sLineValue.size() < 5)
    {
        return INVALID;
    }

    if('#' == sLineValue[0])
    {
        if(0 == strncasecmp(sLineValue.c_str(), "#define", 7))
        {
            return DEFINE;
        }
        if(0 == strncasecmp(sLineValue.c_str(), "#redefine", 9))
        {
            return REDEFINE;
        }
        if(0 == strncasecmp(sLineValue.c_str(), "#undefine", 9))
        {
            return UNDEFINE;
        }
        if(0 == strncasecmp(sLineValue.c_str(), "#reundefine", 11))
        {
            return REUNDEFINE;
        }
        if(0 == strncasecmp(sLineValue.c_str(), "#ifdef", 6))
        {
            return IFDEF;
        }
        if(0 == strncasecmp(sLineValue.c_str(), "#ifndef", 7))
        {
            return IFNDEF;
        }
        if(0 == strncasecmp(sLineValue.c_str(), "#else", 5))
        {
            return ELSE;
        }
        if(0 == strncasecmp(sLineValue.c_str(), "#endif", 6))
        {
            return ENDIF;
        }
        if(0 == strncasecmp(sLineValue.c_str(), "#decrypt", 8))
        {
            return DECRYPT;
        }
        if(0 == strncasecmp(sLineValue.c_str(), "#endcrypt", 9) ||
           0 == strncasecmp(sLineValue.c_str(), "#encrypt", 8))
        {
            return ENDCRYPT;
        }
        if(0 == strncasecmp(sLineValue.c_str(), "#pragma", 7))
        {
            return COND_PRAGMA;
        }
    }
    else if(0 == strncasecmp(sLineValue.c_str(), "include", 7))
    {
        return INCL;
    }

    return INVALID;
}

void rcsPreProcessor_T::initial(hvUInt32 nGlobalLine)
{
    m_sgmapNameDefinition.clear();
    m_sgsetFileCollection.clear();
    m_sgvConditionalsStack.clear();
    m_sgnGlobalLineNo = nGlobalLine;
    m_sgisEnabled     = true;

    m_vIncludeFile.clear();
}

static
bool isTVFRule(const char* pFilename, bool* isTVF = NULL, bool* isTRS = NULL)
{
    FILE* fp = fopen(pFilename, "r");
    if(fp == NULL)
    {
        printf("Error: Can't open file %s\n", pFilename);
        return false;
    }

    char line[1024];
    size_t len = 0;
    bool bTVF = false;
    len = fread(line, 1, 1023, fp);
    line[len]='\0';

    if(len  >  0)
    {
        size_t i = 0;
        while( i<len )
        {
            if(' ' != line[i] && '\t' != line[i] )
            {
                break;
            }
            ++i;
        }
        if( i < len && strncmp( line+i, "#!", 2 ) == 0 )
        {
        	rcsManager_T::getInstance()->setRuleFileHasPrefix(true);
            i += 2; 
            while( i<len )
            {
                if( ' ' != line[i] && '\t' != line[i] )
                {
                    break;
                }
                ++i;
            }
            if(i < len)
            {
            	if(strncasecmp( line + i, "tvf", 3 ) == 0 )
            	{
            		bTVF = true;
            		if(isTVF)
            			*isTVF = true;
            	}
            	else if(strncasecmp( line + i, "trs", 3 ) == 0)
            	{
            		if(isTRS)
            			*isTRS = true;
            	}

            }
        }
        else if( i < len )
        {
        	if(strncasecmp( line+i, "tvf::", 5) == 0 )
        	{
        		bTVF = true;
        		if(isTVF)
        			*isTVF = true;
        	}
        	else if(strncasecmp( line+i, "trs::", 5 ) == 0)
        	{
        		if(isTRS)
        			*isTRS = true;
        	}
        }
    }
    else
    {
        printf("Error: file %s is empty\n", pFilename);
        
        
    }

    fclose(fp);

    return bTVF;
}


static void
split(std::vector<std::string> &result, std::string &input, const char* regex)
{
	int pos = 0;
	int npos = 0;
	int regexlen = strlen(regex);
	while((npos=input.find(regex, pos)) != -1)
	{
		string tmp = input.substr(pos,npos-pos);
		result.push_back(tmp);
		pos = npos+regexlen;
	}
	result.push_back(input.substr(pos,input.length()-pos));
}

void rcsPreProcessor_T::process_GetLayerArgs(std::string sLineBuf)
{
	std::string sArgs = sLineBuf.substr(sizeof("tvf::GET_LAYER_ARGS"));
	std::stringstream ssArgs(sArgs);
	std::string sLayerName;
	while(ssArgs >> sLayerName)
	{
		m_sLayerArgs.insert(sLayerName);
	}

}

void rcsPreProcessor_T::process_SetLayer(std::string &sLineBuf)
{
	std::set<string>::iterator iter = m_sLayerArgs.begin();
	while(iter != m_sLayerArgs.end())
	{
		string sVarLayer = "$" + *iter;
		std::string::size_type nPos = sLineBuf.find(sVarLayer);
		if(std::string::npos != nPos)
		{
			sLineBuf.insert(nPos," ");
		}
		iter++;
	}


	std::string::size_type nEnd = sLineBuf.find('=');
	std::string::size_type nBegin = sizeof("tvf::SETLAYER");
	std::string sDefineLayer = sLineBuf.substr(nBegin,nEnd-nBegin);
	std::stringstream ssArgs(sDefineLayer);
	std::string sLayerName;
	while(ssArgs >> sLayerName)
	{
		m_sLayerArgs.insert(sLayerName);
	}
}

bool
rcsPreProcessor_T::execute()
{
    std::ifstream reader(m_pFileName);
    if(!reader)
    {
        
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, INCL1,
                                            m_sgnGlobalLineNo, std::string(m_pFileName)));
    }

    std::string sLineBuf;
    hvUInt32    nLocalLineNo = 0;

    while(std::getline(reader, sLineBuf))
    {
#if 0
        std::cout << "INDEX: " << m_pFileName << " " << m_sgnGlobalLineNo << " " << nLocalLineNo <<" " << sLineBuf <<std::endl;
#endif
        s_errManager.MapLineIndex(++m_sgnGlobalLineNo, ++nLocalLineNo,
                                  std::string(m_pFileName));

        std::string sValue = sLineBuf;
        std::string::size_type nPos = sValue.find("\\$");
        while(nPos != std::string::npos)
        {
        	sValue.insert(nPos, "\\");
        	nPos = sValue.find("\\$", nPos + 2);
        }
        trim(sLineBuf);


        if(string::npos != sLineBuf.find("tvf::GET_LAYER_ARGS "))
        {
        	process_GetLayerArgs(sLineBuf);

        }
        if(string::npos != sLineBuf.find("tvf::SETLAYER "))
		{
        	process_SetLayer(sValue);

		}

        switch(judgeFirstDirective(sLineBuf))
        {
            case INCL:
            {
            	if(rcsManager_T::getInstance()->isConvertIncludeFile())
            	{
            		m_fOutStream << "include ";

            		bool isWithQuotation = sLineBuf.find("\"") != string::npos;

                	std::string originalFileName = sLineBuf.substr(8);
                	trim(originalFileName, " \t\r\n\"");

                	if(isWithQuotation)
                	{
                		m_fOutStream << "\"";
                	}

                    std::string originalFileHead = originalFileName;
                    bool isWithDollar = originalFileName.find("$") != string::npos;
                    if(isWithDollar)
                    	processEnv(originalFileName);

                    std::string includeFileName;
                    if(isTVFRule(originalFileName.c_str()))
                    {
                    	includeFileName = originalFileHead + ".trs";
                    	originalFileName += ".trs";
                    }
                    else
                    {
                    	includeFileName = originalFileHead + ".pvrs";
                    	originalFileName += ".pvrs";
                    }
                    m_fOutStream << includeFileName;

                    if(isWithQuotation)
					{
						m_fOutStream << "\"\n";
					}

                    pushIncludeFileName(originalFileName);
            	}
            	else
            	{
            		m_fOutStream << '\n';
                    if(m_sgisEnabled)
                        processInclude(sLineBuf);
            	}
                continue;
            }
            case DEFINE:
            {
                if(m_convertSwitch)
                {
                    m_fOutStream << sValue;
                    break;
                }
                processDefine(sLineBuf);
                break;
            }
            case REDEFINE:
            {
                if(m_convertSwitch)
                {
                    m_fOutStream << sValue;
                    break;
                }
                processRedefine(sLineBuf);
                break;
            }
            case UNDEFINE:
            {
                if(m_convertSwitch)
                {
                    m_fOutStream << sValue;
                    break;
                }
                processUndefine(sLineBuf);
                break;
            }
            case REUNDEFINE:
            {
                if(m_convertSwitch)
                {
                    m_fOutStream << sValue;
                    break;
                }
                processReundefine(sLineBuf);
                break;
            }
            case IFDEF:
            {
                if(m_convertSwitch)
                {
                    m_fOutStream << sValue;
                    break;
                }
                processIfdef(sLineBuf);
                break;
            }
            case ELSE:
            {
                if(m_convertSwitch)
                {
                    m_fOutStream << sValue;
                    break;
                }
                processElse(sLineBuf);
                break;
            }
            case ENDIF:
            {
                if(m_convertSwitch)
                {
                    m_fOutStream << sValue;
                    break;
                }
                processEndif(sLineBuf);
                break;
            }
            case IFNDEF:
            {
                if(m_convertSwitch)
                {
                    m_fOutStream << sValue;
                    break;
                }
                processIfndef(sLineBuf);
                break;
            }
            case DECRYPT:
            {
                m_fOutStream << std::endl;

                s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, CON3,
                                                    m_sgnGlobalLineNo));

                while(std::getline(reader, sLineBuf))
                {
#if 0
                    s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, CON1,
                                                        m_sgnGlobalLineNo, sLineBuf));
#endif
                    s_errManager.MapLineIndex(++m_sgnGlobalLineNo, ++nLocalLineNo,
                                              std::string(m_pFileName));

                    trim(sLineBuf);
                    if(judgeFirstDirective(sLineBuf) == ENDCRYPT)
                    {
                        break;
                    }
                    m_fOutStream << '\n';
                }
                break;
            }
            case ENDCRYPT:
            {
#ifdef DEBUG
            	std::cout << "sLineBuf = " << sLineBuf << std::endl;
            	std::cout << "m_pFileName = " << m_pFileName << std::endl;
#endif


                if(m_convertSwitch)
                {
                    m_fOutStream << sValue;
                    break;
                }
                break;
            }
            case COND_PRAGMA:
            {
                processPragma(sLineBuf);
                m_fOutStream << sValue;
                break;
            }
            case INVALID:
            {
                if(m_sgisEnabled)
                    m_fOutStream << sValue;
                break;
            }
        }
        m_fOutStream << '\n';
    }

    reader.close();

    return true;
}

static inline void
parseOperands(std::string &str, std::vector<std::string> &vOperands)
{
    trim(str);
    if(str.empty())
        return;

    if(str[0] == '\"' || str[0] == '\'')
    {
        std::string::size_type iRight = str.find_first_of(str[0], 1);
        if(iRight == (str.length() - 1))
        {
            vOperands.push_back(str.substr(1, iRight - 1));
        }
        else if(iRight == std::string::npos)
        {
            vOperands.push_back(str.substr(1));
            return;
        }
        else
        {
            vOperands.push_back(str.substr(1, iRight - 1));

            std::string remainder = str.substr(iRight + 1);
            trim(remainder);
            if(0 == strncmp(remainder.c_str(), "//", 2))
            {
                return;
            }
            parseOperands(remainder, vOperands);
        }
    }
    else
    {
        std::string::size_type iRight = str.find_first_of(" \t\r\"\'", 1);
        std::string::size_type iComment = str.find("//", 0);
        if(0 == iComment)
            return;

        else if(iComment < iRight)
        {
            vOperands.push_back(str.substr(0, iComment));
            return;
        }
        if(iRight == std::string::npos)
        {
            Utassert(iComment == std::string::npos);
            vOperands.push_back(str.substr(0));
            return;
        }
        vOperands.push_back(str.substr(0, iRight));
        std::string remainder = str.substr(iRight);
        parseOperands(remainder, vOperands);
    }
}

void
rcsPreProcessor_T::processEnv(std::string &str)
{
	std::string tmp = "";
	std::string envVar;

	std::vector<std::string> pathVec;
	split(pathVec, str, "/");

	for(hvUInt32 i = 0; i < pathVec.size(); i++)
	{
		if(pathVec[i].find("$") != string::npos)
		{
			envVar = pathVec[i].substr(1);
            const char *pValue = g_warpper_getenv(envVar.c_str());
			std::string strValue = pValue;
			tmp += strValue;
		}
		else
		{
			tmp += pathVec[i];
		}
		if(i < pathVec.size() - 1)
		{
			tmp += "/";
		}
	}

	str = tmp;
}



static std::string getTvf2SvrfTmpFileName()
{
    static hvUInt32 index = 0;
    std::string sFileName = "._SVRF2PVRS_GENERATE_PVRS_FROM_TCL_FORMAT_";
    sFileName += itoa(index);
    ++index;
    return sFileName;
}

void
rcsPreProcessor_T::processInclude(const std::string &sLineBuf)
{
    Utassert(0 == strncasecmp(sLineBuf.c_str(), "include", 7));
    std::string sFileName = sLineBuf.substr(7);
    std::vector<std::string> vOperands;
    parseOperands(sFileName, vOperands);
    if(1 == vOperands.size())
    {
        std::string sFilePath;
        std::string tvf2SvrfTmpFile;
        AutoFileRemover fileRemover;
        if(!parseFileName(vOperands[0], sFilePath))
        {
            s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, ENV1,
                                                m_sgnGlobalLineNo, sFilePath));
        }
        if(sFilePath[0] != '/' && sFilePath[0] != '~')
        {
            char *pBuf = getcwd(NULL, 0);
            std::string cwd = pBuf;
            cwd += "/";
            sFilePath.insert(0, cwd);
            free(pBuf);
        }
        
        if(!m_sgsetFileCollection.insert(sFilePath).second)
        {
            s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, INCL2,
                                                m_sgnGlobalLineNo, vOperands[0]));
        }
        else
        {
            if(access(sFilePath.c_str(), F_OK|R_OK) != 0)
            {
                
                s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, INCL1,
                                                    m_sgnGlobalLineNo, sFilePath));
                return;
            }
            struct stat filestat;
            stat(sFilePath.c_str(), &filestat);
            if(!S_ISREG(filestat.st_mode))
            {
                
                s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, INCL1,
                                                    m_sgnGlobalLineNo, sFilePath));
            }
            else
            {
                if(rcsSvrf2Pvrs::isTVFRule(sFilePath.c_str()))
                {
                    std::string sSvrfFileName = getTvf2SvrfTmpFileName();
                    fileRemover.setFile(sSvrfFileName);
                    rcsTVFCompiler_T tvfCompiler(sFilePath,
                                                 sSvrfFileName);

                    if (tvfCompiler.tvf_compiler() != TCL_OK)
                    {
                        s_errManager.Report(std::cout);
                        rcsManager_T::getInstance()->endWrite();
                        std::cout << "\n failed to convert tvf: "<<sFilePath << "\n";
                        std::cout << "------        " << "  RULE FILE CONVERTOR FAILED          ------\n\n";
                        exit(-100);
                    }
                    tvf2SvrfTmpFile = sSvrfFileName;
                }

                

                std::string realFile = sFilePath;
                if (!tvf2SvrfTmpFile.empty())
                    realFile = tvf2SvrfTmpFile;

                rcsPreProcessor_T *pNewProcessor = new rcsPreProcessor_T(realFile.c_str(), m_convertSwitch,
                                                                         m_fOutStream);
                pNewProcessor->execute();
                m_sgsetFileCollection.erase(sFilePath);
                delete pNewProcessor;
            }
        }
    }
    else
    {
        if(vOperands.empty())
        {
            
            s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, INCL3, m_sgnGlobalLineNo));
        }
        else
        {
            
            s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, INCL4, m_sgnGlobalLineNo));
        }
    }
}

void
rcsPreProcessor_T::processDefine(const std::string &sLineBuf)
{
    Utassert(0 == strncasecmp(sLineBuf.c_str(), "#define", 7));
    std::string sOperands = sLineBuf.substr(7);
    std::vector<std::string> vOperands;
    parseOperands(sOperands, vOperands);
    if(1 == vOperands.size())
    {
        if(!m_sgisEnabled)
            return;

        if(!m_sgvConditionalsStack.empty()) 
        {
            insert_def_force(vOperands[0], "");
        }
        else
            insert_def(vOperands[0], "");
    }
    else if(2 == vOperands.size())
    {
        if(!m_sgisEnabled)
            return;

        if(!m_sgvConditionalsStack.empty()) 
        {
            insert_def_force(vOperands[0], vOperands[1]);
        }
        else
            insert_def(vOperands[0], vOperands[1]);
    }
    else if(0 == vOperands.size())
    {
        
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, DEF1,
                                            m_sgnGlobalLineNo, "#define"));
    }
    else
    {
        
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, DEF2,
                                            m_sgnGlobalLineNo, "#define"));
    }
}

void
rcsPreProcessor_T::processUndefine(const std::string &sLineBuf)
{
    Utassert(0 == strncasecmp(sLineBuf.c_str(), "#undefine", 9));
    std::string sOperands = sLineBuf.substr(9);
    std::vector<std::string> vOperands;
    parseOperands(sOperands, vOperands);
    if(1 == vOperands.size() && m_sgisEnabled)
    {
        
        if(!m_sgvConditionalsStack.empty()) 
        {
            insert_undef_force(vOperands[0]);
        }
        else
        {
            insert_undef(vOperands[0]);
        }
    }
    else if(0 == vOperands.size())
    {
        
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, DEF1,
                                            m_sgnGlobalLineNo, "#undefine"));
    }
    else if(vOperands.size() > 1)
    {
        
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, DEF2,
                                            m_sgnGlobalLineNo, "#undefine"));
    }
}

void rcsPreProcessor_T::processReundefine(const std::string &sLineBuf)
{
    Utassert(0 == strncasecmp(sLineBuf.c_str(), "#reundefine", 11));
    std::string sOperands = sLineBuf.substr(11);
    std::vector<std::string> vOperands;
    parseOperands(sOperands, vOperands);

    if(1 == vOperands.size() && m_sgisEnabled)
    {
        if (!m_sgisEnabled)
            return;

        
        insert_reundef(vOperands[0]);
    }
    else if(0 == vOperands.size())
    {
        
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, DEF1,
                                            m_sgnGlobalLineNo, "#reundefine"));
    }
    else if(vOperands.size() > 1)
    {
        
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, DEF2,
                                            m_sgnGlobalLineNo, "#reundefine"));
    }
}

void
rcsPreProcessor_T::processIfdef(const std::string &sLineBuf)
{
    m_sgvConditionalsStack.push_back(std::make_pair(IFDEF, m_sgisEnabled));

    Utassert(0 == strncasecmp(sLineBuf.c_str(), "#ifdef", 6));
    std::string sOperands = sLineBuf.substr(6);
    std::vector<std::string> vOperands;
    parseOperands(sOperands, vOperands);
    if(1 == vOperands.size())
    {
        if(!m_sgisEnabled)
            return;

        std::string sName = vOperands[0];
#if 0
        toUpper(sName);
        const DEFINITION_MAP::iterator iter = m_sgmapNameDefinition.lower_bound(sName);

        
        if((iter != m_sgmapNameDefinition.end()) && (iter->first == sName))
        {
            m_sgisEnabled = (iter->second).isDefined;
            return;
        }
#endif
        if (has_def(sName))
        {
            m_sgisEnabled = is_def(sName);
            return;
        }
        else if('$' == vOperands[0][0])
        {
            std::string envName = vOperands[0].substr(1);
            const char *pValue = g_warpper_getenv(envName.c_str());
            if(pValue != NULL && strlen(pValue) != 0)
            {
                
                m_sgisEnabled = true;
                return;
            }
        }

        
        m_sgisEnabled = false;
    }
    else if(2 == vOperands.size())
    {
        if(!m_sgisEnabled)
            return;

        std::string sName = vOperands[0];
#if 0
        toUpper(sName);
        const DEFINITION_MAP::iterator iter = m_sgmapNameDefinition.lower_bound(sName);

        
        if((iter != m_sgmapNameDefinition.end()) && (iter->first == sName))
        {
            m_sgisEnabled = (((iter->second).isDefined) && (0 == strcasecmp(
                           iter->second.sValue.c_str(), vOperands[1].c_str())));
            return;
        }
#endif
        if (has_def(sName))
        {
            m_sgisEnabled = is_def_val(sName, vOperands[1]);
            return;
        }
        else if('$' == vOperands[0][0])
        {
            std::string envName = vOperands[0].substr(1);
            const char *pValue = g_warpper_getenv(envName.c_str());
            if(pValue != NULL)
            {
                
                m_sgisEnabled = (0 == strcasecmp(vOperands[1].c_str(), pValue));
                return;
            }
        }

        
        m_sgisEnabled = false;
    }
    else if(0 == vOperands.size())
    {
        
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, DEF1,
                                            m_sgnGlobalLineNo, "#ifdef"));
    }
    else
    {
        
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, DEF2,
                                            m_sgnGlobalLineNo, "#ifdef"));
    }
}

void
rcsPreProcessor_T::processIfndef(const std::string &sLineBuf)
{
    m_sgvConditionalsStack.push_back(std::make_pair(IFNDEF, m_sgisEnabled));

    Utassert(0 == strncasecmp(sLineBuf.c_str(), "#ifndef", 7));
    std::string sOperands = sLineBuf.substr(7);
    std::vector<std::string> vOperands;
    parseOperands(sOperands, vOperands);
    if(1 == vOperands.size())
    {
        if(!m_sgisEnabled)
        {
            return;
        }

        std::string envName = vOperands[0].substr(1);
#if 0
        toUpper(vOperands[0]);
        const DEFINITION_MAP::iterator iter = m_sgmapNameDefinition.lower_bound(vOperands[0]);

        
        if((iter != m_sgmapNameDefinition.end()) && (iter->first == vOperands[0]))
        {
            m_sgisEnabled = !(iter->second).isDefined;
            return;
        }
#endif
        if (has_def(vOperands[0]))
        {
            m_sgisEnabled = !is_def(vOperands[0]);
            return;
        }
        else if('$' == vOperands[0][0])
        {
            const char *pValue = g_warpper_getenv(envName.c_str());
            if(pValue != NULL && 0 != strlen(pValue))
            {
                

                m_sgisEnabled = false;
                return;
            }
        }

        
        m_sgisEnabled = true;
    }
    else if(2 == vOperands.size())
    {
        if(!m_sgisEnabled)
        {
            return;
        }

        std::string envName = vOperands[0].substr(1);
#if 0
        toUpper(vOperands[0]);
        const DEFINITION_MAP::iterator iter = m_sgmapNameDefinition.lower_bound(vOperands[0]);

        
        if((iter != m_sgmapNameDefinition.end()) && (iter->first == vOperands[0]))
        {
            m_sgisEnabled = !(((iter->second).isDefined) &&
                             (0 == strcasecmp(iter->second.sValue.c_str(),
                                              vOperands[1].c_str())));
            return;
        }
#endif
        if (has_def(vOperands[0]))
        {
            m_sgisEnabled = !is_def_val(vOperands[0], vOperands[1]);
            return;
        }
        else if('$' == vOperands[0][0])
        {
            const char *pValue = g_warpper_getenv(envName.c_str());
            if(pValue != NULL)
            {
                
                m_sgisEnabled = (0 != strcasecmp(vOperands[1].c_str(), pValue));
                return;
            }
        }

        
        m_sgisEnabled = true;
    }
    else if(0 == vOperands.size())
    {
        
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, DEF1,
                                            m_sgnGlobalLineNo, "#ifndef"));
    }
    else
    {
        
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, DEF2,
                                            m_sgnGlobalLineNo, "#ifndef"));
    }
}

void
rcsPreProcessor_T::processElse(const std::string &sLineBuf)
{
    
    
    if(m_sgvConditionalsStack.empty() || m_sgvConditionalsStack.back().first == ELSE)
    {
        
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, DEF3, m_sgnGlobalLineNo));
    }

    Utassert(0 == strncasecmp(sLineBuf.c_str(), "#else", 5));
    std::string sOperands = sLineBuf.substr(5);
    std::vector<std::string> vOperands;
    parseOperands(sOperands, vOperands);
    if(!vOperands.empty())
    {
        
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, DEF2,
                                            m_sgnGlobalLineNo, "#else"));
    }

    m_sgisEnabled = !(m_sgisEnabled == m_sgvConditionalsStack.back().second);
    
    m_sgvConditionalsStack.push_back(std::make_pair(ELSE,m_sgisEnabled));
}

void
rcsPreProcessor_T::processEndif(const std::string &sLineBuf)
{
    if(m_sgvConditionalsStack.empty())
    {
        
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, DEF4,
                                            m_sgnGlobalLineNo));
    }

    Utassert(0 == strncasecmp(sLineBuf.c_str(), "#endif", 6));
    std::string sOperands = sLineBuf.substr(6);
    std::vector<std::string> vOperands;
    parseOperands(sOperands, vOperands);
    if(!vOperands.empty())
    {
        
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, DEF2,
                                            m_sgnGlobalLineNo, "#endif"));
    }

    if(ELSE == m_sgvConditionalsStack.back().first)
    {
        m_sgvConditionalsStack.pop_back();
        Utassert(!m_sgvConditionalsStack.empty());
    }

    
    m_sgisEnabled = m_sgvConditionalsStack.back().second;
    m_sgvConditionalsStack.pop_back();
}

void rcsPreProcessor_T::processPragma(const std::string &sLineBuf)
{
    
    static const std::string g_env = "env";
    const char* p = strcasestr(sLineBuf.c_str(), g_env.c_str());
    if (p == NULL || *(p+3) == '\0' || !isspace(*(p+3)))
    {
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::ERROR, PRAGMA6, m_sgnGlobalLineNo));
        return;
    }

    std::string sOperands = p + 4;
    std::vector<std::string> vOperands;
    parseOperands(sOperands, vOperands);

    if (vOperands.size() != 2)
    {
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::ERROR, PRAGMA8, m_sgnGlobalLineNo));
        return;
    }

    const std::string &key = vOperands[0];
    const std::string &val = vOperands[1];

    if (!rcsManager_T::getInstance()->getPragmaManager().insertEnvVar(key, val))
    {
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::ERROR, PRAGMA9, m_sgnGlobalLineNo, key));
        return;
    }
}

void
rcsPreProcessor_T::processRedefine(const std::string &sLineBuf)
{
    Utassert(0 == strncasecmp(sLineBuf.c_str(), "#redefine", 9));
    std::string sOperands = sLineBuf.substr(9);
    std::vector<std::string> vOperands;
    parseOperands(sOperands, vOperands);
    if(1 == vOperands.size())
    {
        if(m_sgisEnabled)
        {
            insert_redef(vOperands[0], "");
        }
    }
    else if(2 == vOperands.size())
    {
        if(m_sgisEnabled)
        {
            insert_redef(vOperands[0], vOperands[1]);
        }
    }
    else if(0 == vOperands.size())
    {
        
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, DEF1,
                                            m_sgnGlobalLineNo, "#redefine"));
    }
    else
    {
        
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, DEF2,
                                            m_sgnGlobalLineNo, "#redefine"));
    }
}

hvUInt32
rcsPreProcessor_T::getIncludeFileVectorSize()
{
	return m_vIncludeFile.size();
}

std::string
rcsPreProcessor_T::getIncludeFile(hvUInt32 index)
{
	return m_vIncludeFile[index];
}

void
rcsPreProcessor_T::pushIncludeFileName(std::string includeFileName)
{
	m_vIncludeFile.push_back(includeFileName);
}

void rcsPreProcessor_T::insert_def(string key, const string &val)
{
    toUpper(key);
    rcsPreProcessor_T::NAME_VALUE &refVal = m_sgmapNameDefinition[key];
    if(!refVal.guiPro.isNone())
        return;

    refVal.isDefined = true;
    refVal.sValue = val;
}

void rcsPreProcessor_T::insert_def_force(string key, const string &val)
{
    toUpper(key);
    rcsPreProcessor_T::NAME_VALUE &refVal = m_sgmapNameDefinition[key];
    refVal.isDefined = true;
    refVal.sValue = val;
}

void rcsPreProcessor_T::insert_undef(string key)
{
    toUpper(key);
    rcsPreProcessor_T::NAME_VALUE &refVal = m_sgmapNameDefinition[key];
    if(!refVal.guiPro.isNone())
        return;

    refVal.isDefined = false;
    refVal.sValue = "";
}

void rcsPreProcessor_T::insert_undef_force(string key)
{
    toUpper(key);
    rcsPreProcessor_T::NAME_VALUE &refVal = m_sgmapNameDefinition[key];
    refVal.isDefined = false;
    refVal.sValue = "";
}

void rcsPreProcessor_T::insert_redef(string key, const string &val)
{
    toUpper(key);
    rcsPreProcessor_T::NAME_VALUE &refVal = m_sgmapNameDefinition[key];
    refVal.isDefined = true;
    refVal.sValue = val;
    refVal.guiPro.setDirective(REDEFINE);
}

void rcsPreProcessor_T::insert_reundef(string key)
{
    toUpper(key);
    rcsPreProcessor_T::NAME_VALUE &refVal = m_sgmapNameDefinition[key];
    refVal.isDefined = false;
    refVal.sValue = "";
    refVal.guiPro.setDirective(REUNDEFINE);
}

bool rcsPreProcessor_T::has_def(string key)
{
    toUpper(key);
    return m_sgmapNameDefinition.find(key) != m_sgmapNameDefinition.end();
}

bool rcsPreProcessor_T::is_def(string key)
{
    Utassert(has_def(key));
    toUpper(key);
    rcsPreProcessor_T::NAME_VALUE &refVal = m_sgmapNameDefinition[key];
    return refVal.isDefined;
}

bool rcsPreProcessor_T::is_def_val(string key, const string &val)
{
    Utassert(has_def(key));
    toUpper(key);
    rcsPreProcessor_T::NAME_VALUE &refVal = m_sgmapNameDefinition[key];
    return refVal.isDefined && 0 == strcasecmp(refVal.sValue.c_str(), val.c_str());
}
