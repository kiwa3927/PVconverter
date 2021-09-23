
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/stat.h>

#undef yyFlexLexer
#define yyFlexLexer tvfFlexLexer

#include "tcl.h"
#include "lexical/FlexLexer.h"
#include "public/rcsTypes.h"
#include "main/rcsSvrf2Pvrs.h"
#include "public/token/rcsTokenTypes.h"
#include "public/manager/rcsManager.h"
#include "public/manager/rcErrorManager.h"
#include "public/util/rcsStringUtil.hpp"
#include "tvf/rcsTvfCompiler.h"

#include "rcsTvfConvertor.h"

bool rcsTvfConvertor::m_shasImportInclude = false;
bool rcsTvfConvertor::m_bOutputAllNamespace = false;
std::vector<std::string> rcsTvfConvertor::m_svTvfKeyword;

rcsTvfConvertor::rcsTvfConvertor(const std::string &pFileName)
: m_sFileName(pFileName),
  m_oPVRSStream(&(rcsManager_T::getInstance()->getPVRSOutStream())),
  m_oLogStream(rcsManager_T::getInstance()->getPVRSSumStream()),
  m_pInterp(NULL), m_pScript(NULL)
{
    if(m_svTvfKeyword.empty())
    {
        m_svTvfKeyword.push_back("tvf::");
        m_svTvfKeyword.push_back("device::");
        //add erctcl func
        m_svTvfKeyword.push_back("erc::");
    }
}

rcsTvfConvertor::~rcsTvfConvertor()
{
    if(m_pInterp != NULL)
        Tcl_DeleteInterp(m_pInterp);
}

bool
rcsTvfConvertor::parseStringArgv(Tcl_Parse &parse, Tcl_Token *tokenPtr,
                                 std::string &script)
{
    Utassert(tokenPtr->start[0] == '\"');
    Utassert((tokenPtr->start + tokenPtr->size - 1)[0] == '\"');
    if(tokenPtr->start[0] != '\"' ||
      (tokenPtr->start + tokenPtr->size - 1)[0] != '\"')
        return false;

    Tcl_Parse newParse;
    const char *p, *end;
    p = (tokenPtr + 1)->start;
    end = (tokenPtr + tokenPtr->numComponents)->start + (tokenPtr + tokenPtr->numComponents)->size;
    Utassert(end[0] == '\"');

    while(Tcl_ParseCommand(m_pInterp, p, end - p, 0, &newParse) == TCL_OK)
    {
#ifdef DEBUG
        std::cerr << "Parsing Command:" << std::string(newParse.commandStart, newParse.commandSize) << std::endl;
#endif
        p = newParse.commandStart + newParse.commandSize;
        parseCommandArgv(newParse, script);
        if (p >= end)
            break;

        Tcl_FreeParse(&newParse);
    }

    
    if(p != end)
    {
        int errorLine = 1;
        for(const char *i = m_pScript; i != p; ++i)
        {
            if(i[0] == '\n')
                ++errorLine;
        }

        std::cerr << "Compile-Error on line " << errorLine << " of " << m_sFileName <<  ": "
                  << Tcl_GetString(Tcl_GetObjResult(m_pInterp)) << std::endl;
        Tcl_FreeParse(&parse);
        return false;
    }

    return true;
}


static void insertDoubleQuoteToWord(string &word)
{
    if( (word.size() > 1) && ( ('(' == word[0]) && (')' != word[word.size() - 1]) ) )
    {
        word.erase(0);
        hvUInt32 nLeftBracket = 1;
        while( (word.size() > 1) && ( ('(' == word[0]) && (')' != word[word.size() - 1]) ) )
        {
            word.erase(0);
            nLeftBracket++;
        }
        word.insert(word.begin(), '\"');
        word.insert(word.end(), '\"');
        while(nLeftBracket > 0)
        {
            word.insert(word.begin(), '(');
            nLeftBracket--;
        }
    }
    else if( (word.size() > 1) && ( ('(' != word[0]) && (')' == word[word.size() - 1]) ) )
    {
        word.insert(word.begin(), '\"');

        word.erase(word.size() - 1);
        word += "\"";
        hvUInt32 nRightBracket = 1;
        while( (word.size() > 1) && ( ('(' != word[0]) && (')' == word[word.size() - 1]) ) )
        {
            word.erase(word.size() - 1);
            nRightBracket++;
        }
        while(nRightBracket > 0)
        {
            word += ")";
            nRightBracket--;
        }
    }
    else
    {
    	bool bLeftFlag = true;
    	bool bRightFlag = true;
        if(word.size() > 2)
        {
            if( (('\\' == word[0]) && ('\"' == word[1])) &&
            	(('\\' != word[word.size() - 2]) || ('\"' != word[word.size() - 1])) )
            	bRightFlag = false;
            if( (('\\' == word[word.size() - 2]) && ('\"' == word[word.size() - 1])) &&
            	(('\\' != word[0]) || ('\"' != word[1])) )
            	bLeftFlag = false;
        }
        if(bLeftFlag)
        {
        	word.insert(word.begin(), '\"');
        }
        if(bRightFlag)
        {
        	word.insert(word.end(), '\"');
        }
    }
}

static void insertDoubleQuoteToWord_skipParentheses(string &word)
{
	bool bLeftFlag = true;
	bool bRightFlag = true;
    if(word.size() > 2)
    {
        if( (('\\' == word[0]) && ('\"' == word[1])) &&
        	(('\\' != word[word.size() - 2]) || ('\"' != word[word.size() - 1]))	)
        	bRightFlag = false;
        if( (('\\' == word[word.size() - 2]) && ('\"' == word[word.size() - 1])) &&
        	(('\\' != word[0]) || ('\"' != word[1])) )
        	bLeftFlag = false;
    }
    {
        string::iterator beg = word.begin();
        while (beg != word.end())
        {
            const char &c = *beg;
            if ( !std::isspace(c) && c != '(' )
                break;

            ++beg;
        }
        if(bLeftFlag)
        {
        	word.insert(beg, '\"');
        }
    }

    {
        string::reverse_iterator rbeg = word.rbegin();
        while (rbeg != word.rend())
        {
            const char &c = *rbeg;
            if ( !std::isspace(c) && c != ')' )
                break;

            ++rbeg;
        }
        if(bRightFlag)
        {
        	word.insert(rbeg.base(), '\"');
        }
    }
}

bool
rcsTvfConvertor::parseCommandArgv(Tcl_Parse &parse, std::string &script)
{
    std::string word(parse.commandStart, parse.commandSize);
    if(word[word.size() - 1] == ';')
        word.erase(word.size() - 1);
    if(word.find('{') == std::string::npos && word.find('}') == std::string::npos &&
       word.find('[') == std::string::npos && word.find(']') == std::string::npos &&
       word.find('\\') == std::string::npos && word.find('\"') == std::string::npos)
    {
        script.append(word);
        return true;
    }

    hvUInt32 wordIdx = 0;
    Tcl_Token *tokenPtr = &parse.tokenPtr[0];
    for(; wordIdx < (hvUInt32)parse.numWords; wordIdx++, tokenPtr += (tokenPtr->numComponents + 1))
    {
        std::string word(tokenPtr->start, tokenPtr->size);

        if(wordIdx == parse.numWords-1)
        {
        	trim(word);
        	if(word=="\\")
        	{
        		continue;
        	}
        }

        if(word.find('{') == std::string::npos && word.find('}') == std::string::npos &&
           word.find('[') == std::string::npos && word.find(']') == std::string::npos &&
           word.find('\\') == std::string::npos && word.find('\"') == std::string::npos)
        {
            script += word;
        }
        else if(tokenPtr->start[0] == '\"' && tokenPtr->start[tokenPtr->size - 1] == '\"')
        {
            if(!parseStringArgv(parse, tokenPtr, script))
                return false;
        }
        else if(tokenPtr->start[0] == '{' && tokenPtr->start[tokenPtr->size - 1] == '}')
        {
            if(tokenPtr->numComponents == 1)
            {
            	if(tokenPtr->start[1] == '(')
            	{
            		word.erase(0, 1);
            		word.erase(word.size()-1, 1);
            		script += word;
            	}
            	else
            	{
                    insertDoubleQuoteToWord(word);
                    script += word;
            	}
            }
            else
            {
                insertDoubleQuoteToWord(word);
                script += word;
            }
        }
        else if(tokenPtr->start[0] == '[' && tokenPtr->start[tokenPtr->size - 1] == ']')
        {
            if(tokenPtr->numComponents == 1)
            {
                insertDoubleQuoteToWord(word);
                script += word;
            }
            else
            {
                insertDoubleQuoteToWord(word);
                script += word;
            }
        }
        else if(word.find('[') != std::string::npos || word.find(']') != std::string::npos)
        {
            bool isBuiltInLang = false;
            std::string::size_type i = word.find('[');
            while(i != std::string::npos && i != 0 && word[i - 1] == '\\')
            {
                isBuiltInLang = true;
                word.erase(i - 1, 1);
                i = word.find('[');
            }

            i = word.find(']');
            //while(i != std::string::npos && i != 0 && word[i - 1] == '\\')
            while(i != std::string::npos)
            {
                //isBuiltInLang = true;
                if(i != 0 && word[i - 1] == '\\')
                {
                	isBuiltInLang = true;
                	word.erase(i - 1, 1);
                	i = word.find(']', i);
                }
                else
                {
                	i = word.find(']', i + 1);
                }
            }

            if(isBuiltInLang)
            {
                script += word;
            }
            else
            {
				insertDoubleQuoteToWord(word);
				script += word;
            }
        }
        else
        {
            insertDoubleQuoteToWord_skipParentheses(word);
            script += word;
        }

        script += " ";
#if 0
        if(wordIdx == parse.numWords - 1)
        {
            if(parse.commandStart + parse.commandSize != tokenPtr->start + tokenPtr->size)
            {
                if(tokenPtr->start[tokenPtr->size] != '\\')
                    script.append(tokenPtr->start + tokenPtr->size, parse.commandStart +
                                  parse.commandSize - tokenPtr->start - tokenPtr->size);
                else
                    script.append(" ");
            }
        }
        else if((tokenPtr + tokenPtr->numComponents + 1)->start != tokenPtr->start + tokenPtr->size)
        {
            if(tokenPtr->start[tokenPtr->size] != '\\')
                script.append(tokenPtr->start + tokenPtr->size,
                             (tokenPtr + tokenPtr->numComponents + 1)->start -
                              tokenPtr->start - tokenPtr->size);
            else
                script.append(" ");
        }
#endif
    }

    return true;
}

bool
rcsTvfConvertor::parseTvfCommandArgv(Tcl_Parse &parse, std::string &script,
                                     const bool needParseArgv0)
{
    if(!parseCommandArgv(parse, script))
        return false;

    if(!needParseArgv0)
        script.erase(0, parse.tokenPtr[0].size);

    return true;
}





bool
rcsTvfConvertor::parseSetLayerDefinition(Tcl_Parse &parse)
{
	std::string script(parse.commandStart, parse.commandSize);
	if(script.find('[') != std::string::npos && script.find(']') != std::string::npos &&
			script.find("device::") != std::string::npos)
	{
















		{
			
			script.erase(0, 13);
			(*m_oPVRSStream) << "trs::setlayer " << script;
		}
		return true;
	}
	else if(script.find("-getexpr") != std::string::npos)
	{
		script.erase(0, 13);
		(*m_oPVRSStream) << "trs::setlayer " << script;
		return true;
	}
	else
	{
		return false;
	}
}

bool
rcsTvfConvertor::parseDeviceCommand(std::string sInFile, std::string sOutFile, std::string& result)
{
    rcsTVFCompiler_T tvf2svrf(sInFile, sOutFile);
    int n = tvf2svrf.tvf_compiler();

    if(n == 0)
    {
        std::string sPvrsFile = ".output.pvrs.tmp";;
    	convertSvrfToPvrs(sOutFile, sPvrsFile);
        std::ifstream outFile(sPvrsFile.c_str());
    	std::string line;
    	if(outFile)
    	{
    		while(getline(outFile, line))
    		{
    			result += line;
    			result += "\n";
    		}
    	}
    	outFile.close();
    	return true;
    }
    else
    {
    	return false;
    }
}

bool
rcsTvfConvertor::convertSvrfToPvrs(std::string sInFile, std::string sOutFile)
{
	std::string cmd = "svrf2pvrs -i ";
	cmd += sInFile;
	cmd += " -o ";
	cmd += sOutFile;
	cmd += " 1> /dev/null";
    return system(cmd.c_str()) == 0;
}

std::string rcsTvfConvertor::outputSpecifiedSubString(const std::string& strSrc, const std::string& strSub, const std::string& strSpec)
{
    std::string strResult;
    size_t nSrcPos = 0;
    size_t index = 0;
    while((index = strSrc.find(strSub, index)) != std::string::npos)
    {
        if((index == 0 || std::isspace(strSrc[index - 1])) &&
                (index + strSub.length() >= strSrc.length() || std::isspace(strSrc[index + strSub.length()])))
        {
            strResult += strSrc.substr(nSrcPos, index - nSrcPos);
            strResult += strSpec;
            nSrcPos = index + strSub.length();
        }
        index += strSub.size();
    }
    strResult += strSrc.substr(nSrcPos);
    return strResult;
}

bool rcsTvfConvertor::parseErcSetupParallelDeviceParamsCommand(Tcl_Parse &parse)
{
    Utassert(strncasecmp(parse.tokenPtr[0].start, "erc::setup_parallel_device_parameters", 37) == 0 ||
             strncasecmp(parse.tokenPtr[0].start, "setup_parallel_device_parameters", 32) == 0);

    (*m_oPVRSStream) << "erc::setup_parallel_device_param";

    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("erc::setup_parallel_device_parameters");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    //transform -devices option to -device option
    std::string strSrc(parse.commandStart + parse.tokenPtr[0].size, 0, parse.commandSize - parse.tokenPtr[0].size);
    std::string strSub = "-devices", strSpec = "-device";
    std::string strRes = outputSpecifiedSubString(strSrc, strSub, strSpec);
    (*m_oPVRSStream) << strRes.c_str();

    return true;
}

bool rcsTvfConvertor::parseErcExecuteParallelDeviceParamsCommand(Tcl_Parse &parse)
{
    Utassert(strncasecmp(parse.tokenPtr[0].start, "erc::execute_parallel_device_parameters", 39) == 0 ||
             strncasecmp(parse.tokenPtr[0].start, "execute_parallel_device_parameters", 34) == 0);

    (*m_oPVRSStream) << "erc::execute_parallel_device_param";

    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("erc::execute_parallel_device_parameters");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    (*m_oPVRSStream).write(parse.commandStart + parse.tokenPtr[0].size,
                           parse.commandSize - parse.tokenPtr[0].size);

    return true;
}

bool
rcsTvfConvertor::hasTvfKeyword(const char *p, const char *end)
{
    std::string script(p, end);
    for(std::vector<std::string>::const_iterator iter = m_svTvfKeyword.begin();
        iter != m_svTvfKeyword.end(); ++iter)
    {
        if(script.find(iter->c_str()) != std::string::npos)
            return true;
    }

    return false;
}

bool
rcsTvfConvertor::isTvfKeyword(const char *p, const char *end)
{
    std::string script(p, end);
    if(strncasecmp(script.c_str(), "tvf::", 5) == 0)
        return true;
    else if(strncasecmp(script.c_str(), "device::", 5) == 0)
        return true;
    //add erctcl func
    else if(strncasecmp(script.c_str(), "erc::", 5) == 0)
        return true;

    for(std::vector<std::string>::const_iterator iter = m_svTvfKeyword.begin();
        iter != m_svTvfKeyword.end(); ++iter)
    {
        if(strcasecmp(iter->c_str(), script.c_str()) == 0)
            return true;
    }

    return false;
}

bool
rcsTvfConvertor::parseScript(Tcl_Interp *interp, const char *script,
                             const size_t size)
{
    Tcl_Parse parse;
    const char *p, *end;
    p = script;
    end = p + size;
    while(Tcl_ParseCommand(interp, p, end - p, 0, &parse) == TCL_OK)
    {
#ifdef DEBUG
        std::cerr << "Parsing Command:" << std::string(parse.commandStart, parse.commandSize) << std::endl;
#endif
        if(parse.commentStart != NULL && p != parse.commentStart)
        {
            (*m_oPVRSStream).write(p, parse.commentStart - p);
        }
        else if(parse.commentStart == NULL && parse.commandStart != NULL && p != parse.commandStart)
        {
            (*m_oPVRSStream).write(p, parse.commandStart - p);
        }

        parseTclCommand(parse);
        p = parse.commandStart + parse.commandSize;
        if (p >= end)
            break;
        Tcl_FreeParse(&parse);
    }

    
    if(p != end)
    {
        int errorLine = 1;
        for(const char *i = m_pScript; i != p; ++i)
        {
            if(i[0] == '\n')
                ++errorLine;
        }
        std::cerr << "Compile-Error on line " << errorLine << " of " << m_sFileName <<  ": "
                  << Tcl_GetString(Tcl_GetObjResult(interp)) << std::endl;
        Tcl_FreeParse(&parse);
        return false;
    }

    Tcl_FreeParse(&parse);
    return true;
}

bool
rcsTvfConvertor::parseTvfComment(Tcl_Parse &parse)
{
    Utassert(strncmp(parse.tokenPtr[0].start, "// ", 3) == 0 ||
             strncmp(parse.tokenPtr[0].start, "tvf::// ", 8) == 0);

    (*m_oPVRSStream) << "trs::// ";
    (*m_oPVRSStream).write(parse.commandStart + parse.tokenPtr[0].size,
                           parse.commandSize - parse.tokenPtr[0].size);


    
	hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::COMMENT");
	rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    return true;
}

bool
rcsTvfConvertor::parseTvfRuleCheck(Tcl_Parse &parse)
{
    Utassert(strncasecmp(parse.tokenPtr[0].start, "rulecheck ", 10) == 0 ||
             strncasecmp(parse.tokenPtr[0].start, "tvf::rulecheck ", 15) == 0);

    
	hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::RULECHECK");
	rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);


    if(parse.numWords < 3)
        return false;

    (*m_oPVRSStream) << "trs::rule ";
    int t = parse.tokenPtr[0].numComponents + 1;

    
    if(strncasecmp(parse.tokenPtr[t].start, "rule ", 5) == 0)
        (*m_oPVRSStream) << "\\\"";
    (*m_oPVRSStream).write(parse.tokenPtr[t].start, parse.tokenPtr[t].size);
    if(strncasecmp(parse.tokenPtr[t].start, "rule ", 5) == 0)
        (*m_oPVRSStream) << "\\\"";

    
    t += parse.tokenPtr[t].numComponents + 1;
    Utassert(parse.tokenPtr[t].start[0] == '{' &&
             parse.tokenPtr[t].start[parse.tokenPtr[t].size - 1] == '}');

    (*m_oPVRSStream) << " {";
    size_t iSize = parse.tokenPtr[t].size - 1;
    while(parse.tokenPtr[t].start[iSize] != '}')
    {
        --iSize;
    }
    Utassert(parse.tokenPtr[t].start[iSize] == '}');

    if(parse.tokenPtr[t].start[iSize] == '}')
    {
        if(!parseScript(m_pInterp, parse.tokenPtr[t].start + 1, iSize - 1))
            return false;
    }

    (*m_oPVRSStream) << "}\n";

    return true;
}

bool
rcsTvfConvertor::parseTvfRuleCheckComment(Tcl_Parse &parse)
{
    Utassert(strncmp(parse.tokenPtr[0].start, "@ ", 2) == 0 ||
            strncasecmp(parse.tokenPtr[0].start, "tvf::@ ", 7) == 0 ||
            strncasecmp(parse.tokenPtr[0].start, "comment ", 8) == 0 ||
            strncasecmp(parse.tokenPtr[0].start, "tvf::comment ", 13) == 0);


    (*m_oPVRSStream) << "trs::comment ";
    std::string sSvrfComment(parse.commandStart + parse.tokenPtr[0].size,
                             parse.commandSize - parse.tokenPtr[0].size);

    
	hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::@");
	rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    std::string sPvrsComment;
    if(!_replaceComment(sSvrfComment, sPvrsComment, true, true))
    
        return false;

    (*m_oPVRSStream) << sPvrsComment;
    return true;
}

bool
rcsTvfConvertor::parseTvfSvrfCommand(Tcl_Parse &parse)
{
    std::string script;
    parseTvfCommandArgv(parse, script);
    std::string command(parse.tokenPtr[0].start, parse.tokenPtr[0].size);

    if(strncasecmp(parse.commandStart, "tvf::", 5) == 0)
        command.erase(0, 5);
    if(strncasecmp(command.c_str(), "PEX", 3) == 0 ||
       strncasecmp(command.c_str(), "CAPACITANCE", 11) == 0 ||
       strncasecmp(command.c_str(), "RESISTANCE", 10) == 0 ||
       strncasecmp(command.c_str(), "INDUCTANCE", 10) == 0)
    {
        script.insert(0, command);
        rcsSvrf2Pvrs::executeForTvfCommand(script);
        return true;
    }

    script.insert(0, command);

    if(strncasecmp(script.c_str(), "device::", 8) == 0)
    	(*m_oPVRSStream) << script;
    else if(!rcsSvrf2Pvrs::executeForTvfCommand(script))
        return false;
    return true;
}

bool
rcsTvfConvertor::parseTvfVerbatimCommand(Tcl_Parse &parse)
{
    Utassert(strncasecmp(parse.tokenPtr[0].start, "verbatim ", 9) == 0 ||
             strncasecmp(parse.tokenPtr[0].start, "tvf::verbatim ", 14) == 0);

    
	hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::VERBATIM");
	rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    if(parse.numWords < 2)
        return false;

    (*m_oPVRSStream) << "trs::pvrs_block ";

    (*m_oPVRSStream) << " {\n";
    std::string script(parse.tokenPtr[parse.tokenPtr[0].numComponents + 1].start + 1,
                       parse.tokenPtr[parse.tokenPtr[0].numComponents + 1].size - 2);
    rcsManager_T::getInstance()->setTvf2Trs(false);
    if(!rcsSvrf2Pvrs::executeForTvfCommand(script))
    {
        rcsManager_T::getInstance()->setTvf2Trs(true);
        return false;
    }

    rcsManager_T::getInstance()->setTvf2Trs(true);
    (*m_oPVRSStream) << "}\n";
    return true;
}

bool
rcsTvfConvertor::parseTvfSetLayerCommand(Tcl_Parse &parse)
{
    Utassert(strncasecmp(parse.tokenPtr[0].start, "setlayer ", 9) == 0 ||
             strncasecmp(parse.tokenPtr[0].start, "tvf::setlayer ", 14) == 0);

    
    rcsManager_T::getInstance()->setCallFromSetLayerCommand(true);
    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::SETLAYER");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    Tcl_Token &tokenPtr = parse.tokenPtr[parse.tokenPtr[0].numComponents + 1];
    std::string word(tokenPtr.start, tokenPtr.size);
    if(strncasecmp(word.c_str(), "-getnames", 9) == 0)
    {
    	(*m_oPVRSStream) << "trs::setlayer ";
        (*m_oPVRSStream).write(parse.commandStart + parse.tokenPtr[0].size,
                               parse.commandSize - parse.tokenPtr[0].size);
        rcsManager_T::getInstance()->setCallFromSetLayerCommand(false);
        return true;
    }
    else if(strncasecmp(word.c_str(), "–getexpr", 8) == 0)
    {
        (*m_oPVRSStream).write(parse.commandStart + parse.tokenPtr[0].size,
                               parse.commandSize - parse.tokenPtr[0].size);
        rcsManager_T::getInstance()->setCallFromSetLayerCommand(false);
        return true;
    }
    std::string script;
    parseTvfCommandArgv(parse, script);

    if(!parseSetLayerDefinition(parse))
    {
    	if(!rcsSvrf2Pvrs::executeForTvfCommand(script))
    	{
    		rcsManager_T::getInstance()->setCallFromSetLayerCommand(false);
    		return false;
    	}
    }
	rcsManager_T::getInstance()->setCallFromSetLayerCommand(false);
    return true;
}

bool
rcsTvfConvertor::parseTvfDeleteLayerCommand(Tcl_Parse &parse)
{

    (*m_oPVRSStream) << "trs::DELETE_LAYER ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::DELETE_LAYER");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    std::string script(parse.tokenPtr[parse.tokenPtr[0].numComponents + 1].start,
                       parse.commandStart + parse.commandSize);
    (*m_oPVRSStream) << script;
    return true;
}

bool
rcsTvfConvertor::parseTvfBlockTranscriptCommand(Tcl_Parse &parse)
{
    Utassert(strncasecmp(parse.tokenPtr[0].start, "block_transcript ", 17) == 0 ||
             strncasecmp(parse.tokenPtr[0].start, "tvf::block_transcript ", 22) == 0);

    (*m_oPVRSStream) << "trs::block_log ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::block_transcript");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    (*m_oPVRSStream).write(parse.commandStart + parse.tokenPtr[0].size,
                           parse.commandSize - parse.tokenPtr[0].size);
    (*m_oPVRSStream) << "\n";
    return true;
}

bool
rcsTvfConvertor::parseTvfUnblockTranscriptCommand(Tcl_Parse &parse)
{
    Utassert(strncasecmp(parse.tokenPtr[0].start, "unblock_transcript ", 19) == 0 ||
             strncasecmp(parse.tokenPtr[0].start, "tvf::unblock_transcript ", 24) == 0);

    (*m_oPVRSStream) << "trs::unblock_log ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::unblock_transcript");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    (*m_oPVRSStream).write(parse.commandStart + parse.tokenPtr[0].size,
                        parse.commandSize - parse.tokenPtr[0].size);
    (*m_oPVRSStream) << "\n";
    return true;
}

bool
rcsTvfConvertor:: parseTvfIsTranscriptBlockedCommand(Tcl_Parse &parse)
{
    Utassert(strncasecmp(parse.tokenPtr[0].start, "is_transcript_blocked ", 22) == 0 ||
             strncasecmp(parse.tokenPtr[0].start, "tvf::is_transcript_blocked ", 27) == 0);

    (*m_oPVRSStream) << "trs::is_log_blocked ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::is_transcript_blocked");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    (*m_oPVRSStream).write(parse.commandStart + parse.tokenPtr[0].size,
                        parse.commandSize - parse.tokenPtr[0].size);
    (*m_oPVRSStream) << "\n";
    return true;
}

bool
rcsTvfConvertor::parseTvfEchoSvrfCommand(Tcl_Parse &parse)
{
    Utassert(strncasecmp(parse.tokenPtr[0].start, "echo_svrf ", 9) == 0 ||
             strncasecmp(parse.tokenPtr[0].start, "tvf::echo_svrf ", 14) == 0);

    (*m_oPVRSStream) << "trs::echo_pvrs ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::echo_svrf");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    (*m_oPVRSStream).write(parse.commandStart + parse.tokenPtr[0].size,
                           parse.commandSize - parse.tokenPtr[0].size);
    (*m_oPVRSStream) << "\n";
    return true;
}

bool
rcsTvfConvertor::parseTvfEchoToSvrfFileCommand(Tcl_Parse &parse)
{
    Utassert(strncasecmp(parse.tokenPtr[0].start, "echo_to_svrf_file ", 18) == 0 ||
             strncasecmp(parse.tokenPtr[0].start, "tvf::echo_to_svrf_file ", 23) == 0);

    (*m_oPVRSStream) << "trs::echo_to_pvrs_file ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::echo_to_svrf_file");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    std::string script;
    parseTvfCommandArgv(parse, script);
    if(!rcsSvrf2Pvrs::executeForTvfCommand(script))
        return false;
    return true;
}

bool rcsTvfConvertor::parseTvfOutputToSvrfCommand(Tcl_Parse &parse)
{
    Utassert(strncasecmp(parse.tokenPtr[0].start, "output_to_svrf ", 15) == 0 ||
             strncasecmp(parse.tokenPtr[0].start, "tvf::output_to_svrf ", 15+5) == 0);

    (*m_oPVRSStream) << "trs::output_to_pvrs ";

    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::output_to_svrf");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    std::string script;
    parseTvfCommandArgv(parse, script);
    if(!rcsSvrf2Pvrs::executeForTvfCommand(script))
        return false;
    return true;
}

bool
rcsTvfConvertor::parseTvfEpilogCommand(Tcl_Parse &parse)
{
    Utassert(strncasecmp(parse.tokenPtr[0].start, "epilog", 6) == 0 ||
             strncasecmp(parse.tokenPtr[0].start, "tvf::epilog", 11) == 0);

    (*m_oPVRSStream) << "trs::cleanup ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::epilog");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    (*m_oPVRSStream).write(parse.commandStart + parse.tokenPtr[0].size,
                           parse.commandSize - parse.tokenPtr[0].size);

    return true;
}

bool
rcsTvfConvertor:: parseTvfSetGlobalVariableCommand(Tcl_Parse &parse)
{
    Utassert(strncasecmp(parse.tokenPtr[0].start, "set_global_variable ", 20) == 0 ||
             strncasecmp(parse.tokenPtr[0].start, "tvf::set_global_variable ", 25) == 0);

    (*m_oPVRSStream) << "trs::set_global_var ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::set_global_variable");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    (*m_oPVRSStream).write(parse.commandStart + parse.tokenPtr[0].size,
                           parse.commandSize - parse.tokenPtr[0].size);

    return true;
}

bool
rcsTvfConvertor::parseTvfGetGlobalVariableCommand(Tcl_Parse &parse)
{
    Utassert(strncasecmp(parse.tokenPtr[0].start, "get_global_variable ", 20) == 0 ||
             strncasecmp(parse.tokenPtr[0].start, "tvf::get_global_variable ", 25) == 0);

    (*m_oPVRSStream) << "trs::get_global_var ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::get_global_variable");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);


    (*m_oPVRSStream).write(parse.commandStart + parse.tokenPtr[0].size,
                           parse.commandSize - parse.tokenPtr[0].size);
    return true;
}

bool
rcsTvfConvertor::parseTvfUnsetGlobalVariableCommand(Tcl_Parse &parse)
{
    Utassert(strncasecmp(parse.tokenPtr[0].start, "unset_global_variable ", 22) == 0 ||
             strncasecmp(parse.tokenPtr[0].start, "tvf::unset_global_variable ", 27) == 0);

    (*m_oPVRSStream) << "trs::unset_global_var ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::unset_global_variable");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    (*m_oPVRSStream).write(parse.commandStart + parse.tokenPtr[0].size,
                           parse.commandSize - parse.tokenPtr[0].size);
    return true;
}

bool
rcsTvfConvertor::parseTvfExistsGlobalVariableCommand(Tcl_Parse &parse)
{
    Utassert(strncasecmp(parse.tokenPtr[0].start, "exists_global_variable ", 23) == 0 ||
             strncasecmp(parse.tokenPtr[0].start, "tvf::exists_global_variable ", 28) == 0);

    (*m_oPVRSStream) << "trs::exists_global_var ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::exists_global_variable");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    (*m_oPVRSStream).write(parse.commandStart + parse.tokenPtr[0].size,
                           parse.commandSize - parse.tokenPtr[0].size);
    return true;
}

bool
rcsTvfConvertor:: parseTvfGetCalibreVersionCommand(Tcl_Parse &parse)
{
    (*m_oPVRSStream) << "trs::get_argus_version ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::get_calibre_version");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    if(parse.commandSize - parse.tokenPtr[0].size != 0)
        (*m_oPVRSStream).write(parse.commandStart + parse.tokenPtr[0].size,
                               parse.commandSize - parse.tokenPtr[0].size);
	return true;
}

bool
rcsTvfConvertor:: parseTvfGetTvfArgCommand(Tcl_Parse &parse)
{
	(*m_oPVRSStream) << "trs::get_tvf_arg ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::get_tvf_arg");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

	if(parse.commandSize - parse.tokenPtr[0].size != 0)
		(*m_oPVRSStream).write(parse.commandStart + parse.tokenPtr[0].size,
							   parse.commandSize - parse.tokenPtr[0].size);
	return true;
}

bool
rcsTvfConvertor:: parseTvfGetLayerArgsCommand(Tcl_Parse &parse)
{
    (*m_oPVRSStream) << "trs::get_layer_arg ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::GET_LAYER_ARGS");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    if(parse.tokenPtr[0].numComponents + 1 < parse.numTokens)
    {
		std::string script(parse.tokenPtr[parse.tokenPtr[0].numComponents + 1].start,
						   parse.commandStart + parse.commandSize);
		(*m_oPVRSStream) << script;
    }



    return true;
}

bool
rcsTvfConvertor:: parseTvfGetLayerExtentCommand(Tcl_Parse &parse)
{
    (*m_oPVRSStream) << "trs::get_layer_boundary ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::GET_LAYER_EXTENT");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    if(parse.tokenPtr[0].numComponents + 1 < parse.numTokens)
    {
		std::string script(parse.tokenPtr[parse.tokenPtr[0].numComponents + 1].start,
						   parse.commandStart + parse.commandSize);
		(*m_oPVRSStream) << script;
    }
    return true;
}

bool
rcsTvfConvertor:: parseTvfGetLayerNameCommand(Tcl_Parse &parse)
{
    (*m_oPVRSStream) << "trs::get_layer_name ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::GET_LAYER_NAME");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    if(parse.tokenPtr[0].numComponents + 1 < parse.numTokens)
    {
		std::string script(parse.tokenPtr[parse.tokenPtr[0].numComponents + 1].start,
						   parse.commandStart + parse.commandSize);
		(*m_oPVRSStream) << script;
    }
    return true;
}

bool
rcsTvfConvertor:: parseTvfIsLayerEmptyCommand(Tcl_Parse &parse)
{

    (*m_oPVRSStream) << "trs::IS_LAYER_EMPTY ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::IS_LAYER_EMPTY");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    if(parse.tokenPtr[0].numComponents + 1 < parse.numTokens)
    {
		std::string script(parse.tokenPtr[parse.tokenPtr[0].numComponents + 1].start,
						   parse.commandStart + parse.commandSize);
		(*m_oPVRSStream) << script;
    }
    return true;
}

bool
rcsTvfConvertor:: parseTvfPrintCallStackCommand(Tcl_Parse &parse)
{
    Utassert(strncasecmp(parse.tokenPtr[0].start, "print_call_stack ", 17) == 0 ||
             strncasecmp(parse.tokenPtr[0].start, "tvf::print_call_stack ", 22) == 0);

    (*m_oPVRSStream) << "trs::output_call_stack  ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::print_call_stack");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    (*m_oPVRSStream).write(parse.commandStart + parse.tokenPtr[0].size,
                           parse.commandSize - parse.tokenPtr[0].size);
    return true;
}

bool
rcsTvfConvertor:: parseTvfSetDisclaimerCommand(Tcl_Parse &parse)
{
    Utassert(strncasecmp(parse.tokenPtr[0].start, "set_disclaimer ", 15) == 0 ||
             strncasecmp(parse.tokenPtr[0].start, "tvf::set_disclaimer ", 20) == 0);

    (*m_oPVRSStream) << "trs::set_header ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::set_disclaimer");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    (*m_oPVRSStream).write(parse.commandStart + parse.tokenPtr[0].size,
                           parse.commandSize - parse.tokenPtr[0].size);
    return true;
}

bool
rcsTvfConvertor:: parseTvfSetRuleCheckIndentationCommand(Tcl_Parse &parse)
{
    Utassert(strncasecmp(parse.tokenPtr[0].start, "set_rule_check_indentation ", 27) == 0 ||
             strncasecmp(parse.tokenPtr[0].start, "tvf::set_rule_check_indentation ", 32) == 0);

    (*m_oPVRSStream) << "trs::set_rule_indent ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::set_rule_check_indentation");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    (*m_oPVRSStream).write(parse.commandStart + parse.tokenPtr[0].size,
                           parse.commandSize - parse.tokenPtr[0].size);
    return true;
}

bool
rcsTvfConvertor:: parseTvfSetTracebackLineCommand(Tcl_Parse &parse)
{
    Utassert(strncasecmp(parse.tokenPtr[0].start, "set_traceback_line ", 19) == 0 ||
             strncasecmp(parse.tokenPtr[0].start, "tvf::set_traceback_line ", 24) == 0);

    (*m_oPVRSStream) << "trs::set_trace_line ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::set_traceback_line");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    (*m_oPVRSStream).write(parse.commandStart + parse.tokenPtr[0].size,
                           parse.commandSize - parse.tokenPtr[0].size);
    return true;
}

bool
rcsTvfConvertor::parseTvfSvrfVarCommand(Tcl_Parse &parse)
{
    (*m_oPVRSStream) << "trs::pvrs_var ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::svrf_var");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    std::string script(parse.tokenPtr[parse.tokenPtr[0].numComponents + 1].start,
                       parse.commandStart + parse.commandSize);
    (*m_oPVRSStream) << script ;
    return true;
}

bool
rcsTvfConvertor::parseTvfSystemVarCommand(Tcl_Parse &parse)
{
    (*m_oPVRSStream) << "trs::sys_var  ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::sys_var");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    if(parse.tokenPtr[0].numComponents + 1 < parse.numTokens)
    {
    	std::string script(parse.tokenPtr[parse.tokenPtr[0].numComponents + 1].start,
    	                       parse.commandStart + parse.commandSize);
        (*m_oPVRSStream) << script;
        return true;
    }
    return false;
}

bool
rcsTvfConvertor::parseTvfOutLayerCommand(Tcl_Parse &parse)
{
    (*m_oPVRSStream) << "trs::outputlayer ";

    
    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::OUTLAYER");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    std::string script;
    parseTvfCommandArgv(parse, script);
    if(!rcsSvrf2Pvrs::executeForTvfCommand(script))
        return false;
    return true;
}

bool
rcsTvfConvertor::parseTvfIncludeFileCommand(Tcl_Parse &parse)
{

    hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun("tvf::include");
    rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);

    if(parse.tokenPtr[0].numComponents + 1 < parse.numTokens)
    {
    	std::string script(parse.tokenPtr[parse.tokenPtr[0].numComponents + 1].start,
    	                       parse.commandStart + parse.commandSize);
    	trim(script);
        std::string includeFileName;
        if(isTVFRule(script.c_str()))
        {
        	includeFileName = script + ".trs";
        }
        else
        {
        	includeFileName = script + ".pvrs";
        }
        rcsSvrf2Pvrs convertor;
        std::string switchFile;
        convertor.executeIncludeFile(script, switchFile, includeFileName, std::cout);
        return true;
    }
    return false;
}

bool
rcsTvfConvertor::parseTvfIncludeFileCommandRespectively(Tcl_Parse &parse)
{
    if(parse.tokenPtr[0].numComponents + 1 < parse.numTokens)
    {
		if(strncasecmp(parse.commandStart, "source ", 7) == 0)
		{
			(*m_oPVRSStream) << "source ";
		}
		else if(m_shasImportInclude && strncasecmp(parse.commandStart, "include ", 8) == 0)
		{
			(*m_oPVRSStream) << "include ";
		}
		else if(strncasecmp(parse.commandStart, "tvf::include ", 13) == 0)
		{
			(*m_oPVRSStream) << "trs::include ";
		}

    	std::string script(parse.tokenPtr[parse.tokenPtr[0].numComponents + 1].start,
    	                       parse.commandStart + parse.commandSize);
    	trim(script);
        std::string includeFileName;
        if(isTVFRule(script.c_str()))
        {
        	includeFileName = script + ".trs";
        }
        else
        {
        	includeFileName = script + ".pvrs";
        }
        (*m_oPVRSStream) << includeFileName;
        rcsManager_T::getInstance()->pushIncludeFileName(includeFileName);
        return true;
    }
    return false;
}

bool
rcsTvfConvertor::parseTclCommand(Tcl_Parse &parse)
{
    if(parse.commentSize != 0)
    {
    	
    	std::string tmp(parse.commentStart);
    	hvUInt32 linePos = tmp.find("\n");
    	std::string substr = tmp.substr(0, linePos);
    	if((substr.find("#!") != std::string::npos) &&
    	   (substr.find("tvf") != std::string::npos))
    	{
    		(*m_oPVRSStream).write(parse.commentStart + linePos, parse.commandStart - parse.commentStart - linePos);
    	}else
    	{
    		(*m_oPVRSStream).write(parse.commentStart, parse.commandStart - parse.commentStart);
    	}


        
        
        std::string comment = std::string(parse.commentStart, parse.commandStart - parse.commentStart);
        trim(comment);
        hvUInt32 lineNumInTvf = rcsSvrf2Pvrs::findLineNumInTvfFun(comment);
        rcsSvrf2Pvrs::increaseCurLineNo(lineNumInTvf);
    }

    if(rcsManager_T::getInstance()->isSvrfComment())
    {
        (*m_oPVRSStream) << "# ";
        (*m_oPVRSStream).write(parse.commandStart, parse.commandSize);
    }

    if(parse.numWords > 0)
    {
        if(parse.numWords > 2)
        {
            
            if(parse.tokenPtr[0].type == TCL_TOKEN_SIMPLE_WORD &&
               strncasecmp(parse.tokenPtr[0].start, "namespace import ", 17) == 0)
            {
            	if(!m_bOutputAllNamespace)
            	{
            		(*m_oPVRSStream) << "namespace import trs::* \n";
            		m_bOutputAllNamespace = true;
            	}
                for (int i = 4; i < parse.numTokens; i++)
                {
                    Tcl_Token *tokenPtr = &parse.tokenPtr[i];
                    if(tokenPtr->type == TCL_TOKEN_TEXT &&
                       strncasecmp(tokenPtr->start, "tvf::*", 6) == 0)
                    {
                    	(*m_oPVRSStream) << "namespace import trs::* \n";
                        m_shasImportInclude = true;

                        m_svTvfKeyword.push_back("include");
                        m_svTvfKeyword.push_back("INCLUDE");

                        m_svTvfKeyword.push_back("//");
                        m_svTvfKeyword.push_back("@");

                        m_svTvfKeyword.push_back("comment");
                        m_svTvfKeyword.push_back("COMMENT");

                        m_svTvfKeyword.push_back("echo_to_svrf_file");
                        m_svTvfKeyword.push_back("output_to_svrf");
                        m_svTvfKeyword.push_back("get_tvf_arg");
                        m_svTvfKeyword.push_back("echo_svrf");

                        m_svTvfKeyword.push_back("exists_global_variable");
                        m_svTvfKeyword.push_back("EXISTS_GLOBAL_VARIABLE");

                        m_svTvfKeyword.push_back("get_global_variable");
                        m_svTvfKeyword.push_back("GET_GLOBAL_VARIABLE");

                        m_svTvfKeyword.push_back("set_global_variable");
                        m_svTvfKeyword.push_back("SET_GLOBAL_VARIABLE");

                        m_svTvfKeyword.push_back("unset_global_variable");
                        m_svTvfKeyword.push_back("UNSET_GLOBAL_VARIABLE");

                        m_svTvfKeyword.push_back("outlayer");
                        m_svTvfKeyword.push_back("OUTLAYER");

                        m_svTvfKeyword.push_back("rulecheck");
                        m_svTvfKeyword.push_back("RULECHECK");

                        m_svTvfKeyword.push_back("set_disclaimer");
                        m_svTvfKeyword.push_back("set_rule_check_indentation");
                        m_svTvfKeyword.push_back("set_traceback_line");
                        m_svTvfKeyword.push_back("setlayer");
                        m_svTvfKeyword.push_back("SETLAYER");
                        m_svTvfKeyword.push_back("verbatim");
                        m_svTvfKeyword.push_back("VERBATIM");

                        m_svTvfKeyword.push_back("attach");
                        m_svTvfKeyword.push_back("flag");
                        m_svTvfKeyword.push_back("lvs");
                        m_svTvfKeyword.push_back("snap");
                        m_svTvfKeyword.push_back("capacitance");
                        m_svTvfKeyword.push_back("flatten");
                        m_svTvfKeyword.push_back("mask");
                        m_svTvfKeyword.push_back("connect");
                        m_svTvfKeyword.push_back("fracture");
                        m_svTvfKeyword.push_back("pex");
                        m_svTvfKeyword.push_back("text");
                        m_svTvfKeyword.push_back("copy");
                        m_svTvfKeyword.push_back("group");
                        m_svTvfKeyword.push_back("polygon");
                        m_svTvfKeyword.push_back("title");
                        m_svTvfKeyword.push_back("device");
                        m_svTvfKeyword.push_back("hcell");
                        m_svTvfKeyword.push_back("port");
                        m_svTvfKeyword.push_back("disconnect");
                        m_svTvfKeyword.push_back("precision");
                        m_svTvfKeyword.push_back("tvf");
                        m_svTvfKeyword.push_back("drc");
                        m_svTvfKeyword.push_back("label");
                        m_svTvfKeyword.push_back("push");
                        m_svTvfKeyword.push_back("erc");
                        m_svTvfKeyword.push_back("layer");
                        m_svTvfKeyword.push_back("resistance");
                        m_svTvfKeyword.push_back("exclude");
                        m_svTvfKeyword.push_back("layout");
                        m_svTvfKeyword.push_back("resolution");
                        m_svTvfKeyword.push_back("virtual");
                        m_svTvfKeyword.push_back("expand");
                        m_svTvfKeyword.push_back("litho");
                        m_svTvfKeyword.push_back("sconnect");
                        m_svTvfKeyword.push_back("label order");

                        m_svTvfKeyword.push_back("ATTACH");
                        m_svTvfKeyword.push_back("FLAG");
                        m_svTvfKeyword.push_back("LVS");
                        m_svTvfKeyword.push_back("SNAP");
                        m_svTvfKeyword.push_back("CAPACITANCE");
                        m_svTvfKeyword.push_back("FLATTEN");
                        m_svTvfKeyword.push_back("MASK");
                        m_svTvfKeyword.push_back("SOURCE");
                        m_svTvfKeyword.push_back("CONNECT");
                        m_svTvfKeyword.push_back("FRACTURE");
                        m_svTvfKeyword.push_back("PEX");
                        m_svTvfKeyword.push_back("TEXT");
                        m_svTvfKeyword.push_back("COPY");
                        m_svTvfKeyword.push_back("GROUP");
                        m_svTvfKeyword.push_back("POLYGON");
                        m_svTvfKeyword.push_back("TITLE");
                        m_svTvfKeyword.push_back("DEVICE");
                        m_svTvfKeyword.push_back("HCELL");
                        m_svTvfKeyword.push_back("PORT");
                        m_svTvfKeyword.push_back("TRACE");
                        m_svTvfKeyword.push_back("DISCONNECT");
                        m_svTvfKeyword.push_back("PRECISION");
                        m_svTvfKeyword.push_back("TVF");
                        m_svTvfKeyword.push_back("DRC");
                        m_svTvfKeyword.push_back("LABEL");
                        m_svTvfKeyword.push_back("PUSH");
                        m_svTvfKeyword.push_back("UNIT");
                        m_svTvfKeyword.push_back("ERC");
                        m_svTvfKeyword.push_back("LAYER");
                        m_svTvfKeyword.push_back("RESISTANCE");
                        m_svTvfKeyword.push_back("VARIABLE");
                        m_svTvfKeyword.push_back("EXCLUDE");
                        m_svTvfKeyword.push_back("LAYOUT");
                        m_svTvfKeyword.push_back("RESOLUTION");
                        m_svTvfKeyword.push_back("VIRTUAL");
                        m_svTvfKeyword.push_back("EXPAND");
                        m_svTvfKeyword.push_back("LITHO");
                        m_svTvfKeyword.push_back("SCONNECT");
                        m_svTvfKeyword.push_back("DFM");
                        m_svTvfKeyword.push_back("dfm");
                        m_svTvfKeyword.push_back("LABEL ORDER");
                    }
                    else if(tokenPtr->type == TCL_TOKEN_TEXT &&
                            strncasecmp(tokenPtr->start, "tvf::", 5) == 0)
                    {
                        if(strncasecmp(tokenPtr->start, "tvf::include", 11) == 0)
                        {
                            m_shasImportInclude = true;
                            (*m_oPVRSStream) << "namespace import trs::include \n";
                        }
                        else if(strncasecmp(tokenPtr->start, "tvf::L*", 7) == 0)
                        {
                            m_svTvfKeyword.push_back("lvs");
                            m_svTvfKeyword.push_back("label");
                            m_svTvfKeyword.push_back("layer");
                            m_svTvfKeyword.push_back("layout");
                            m_svTvfKeyword.push_back("litho");

                            m_svTvfKeyword.push_back("LVS");
                            m_svTvfKeyword.push_back("LABEL");
                            m_svTvfKeyword.push_back("LAYER");
                            m_svTvfKeyword.push_back("LAYOUT");
                            m_svTvfKeyword.push_back("LITHO");
                            (*m_oPVRSStream) << "namespace import trs::l* \n";
                        }
                        else
                        {
                        	m_svTvfKeyword.push_back(std::string(tokenPtr->start + 5, tokenPtr->size - 5));
                        	std::string namespace_str(tokenPtr->start + 5, tokenPtr->size - 5);
                        	std::transform(namespace_str.begin(),namespace_str.end(),namespace_str.begin(),::tolower);
                        	(*m_oPVRSStream) << "namespace import trs::" << namespace_str << std::endl;
                        }
                    }
                    else if(tokenPtr->type == TCL_TOKEN_TEXT &&
                            strncasecmp(tokenPtr->start, "device::", 8) == 0)
                    {
                        m_svTvfKeyword.push_back(std::string(tokenPtr->start + 8, tokenPtr->size - 8));
                        
                        if (m_svTvfKeyword.back() == "*")
                            m_svTvfKeyword.pop_back();
                    }
                    else if(tokenPtr->type == TCL_TOKEN_TEXT &&
                            strncasecmp(tokenPtr->start, "erc::*", 6) == 0)
                    {
                        (*m_oPVRSStream) << "namespace import erc::* \n";
                        m_svTvfKeyword.push_back("setup_parallel_device_parameters");
                        m_svTvfKeyword.push_back("execute_parallel_device_parameters");
                    }
                }
                return true;
            }
        }
    }

    if(strncasecmp(parse.commandStart, "source ", 7) == 0 ||
       strncasecmp(parse.commandStart, "tvf::include ", 13) == 0 ||
       (m_shasImportInclude && strncasecmp(parse.commandStart, "include ", 8) == 0))
    {
    	rcsManager_T::getInstance()->setIncludeFile(true);
    	if(rcsManager_T::getInstance()->isConvertIncludeFile()) 
    	{
    		parseTvfIncludeFileCommand(parse);
    	}
    	else 
    	{
    		parseTvfIncludeFileCommandRespectively(parse);
    	}

        return true;
    }

#if 0
    if(isTclPackageLine(parse.commandStart) && strstr(parse.commandStart, "CalibreDFM_DEVICE") == 0)
    {
    	(*m_oPVRSStream) << "\n";
    	return true;
    }
#endif

    if(isTclPackageLine(parse.commandStart) && strstr(parse.commandStart, "CalibreLVS_ERC_TVF") != NULL)
    {
        (*m_oPVRSStream) << "\n";
        return true;
    }

    if(!hasTvfKeyword(parse.commandStart, parse.commandStart + parse.commandSize))
    {
        (*m_oPVRSStream).write(parse.commandStart, parse.commandSize);
        return true;
    }

    if(parse.tokenPtr->type == TCL_TOKEN_SIMPLE_WORD &&
       isTvfKeyword(parse.tokenPtr->start, parse.tokenPtr->start + parse.tokenPtr->size))
    {
        parseTvfCommand(parse);
        return true;
    }

    hvUInt32 wordIdx = 0;
    Tcl_Token *tokenPtr = &parse.tokenPtr[0];
    if(tokenPtr->start != parse.commandStart)
    {
        (*m_oPVRSStream).write(parse.commandStart, tokenPtr->start - parse.commandStart);
    }

    std::string word(parse.tokenPtr[0].start, parse.tokenPtr[0].size);
    bool isProc = word == "proc";
    for(; wordIdx < (hvUInt32)parse.numWords; wordIdx++, tokenPtr += (tokenPtr->numComponents + 1))
    {
        if(!hasTvfKeyword(tokenPtr->start, tokenPtr->start + tokenPtr->size))
        {
            (*m_oPVRSStream).write(tokenPtr->start, tokenPtr->size);
        }
        else if(isProc && wordIdx < 3)
        {
            (*m_oPVRSStream).write(tokenPtr->start, tokenPtr->size);
        }
        else
        {
            Utassert(tokenPtr->numComponents > 0);
            if(tokenPtr->numComponents > 0)
            {
                if(tokenPtr->start[0] == '{' && tokenPtr->start[tokenPtr->size - 1] == '}')
                {
                    (*m_oPVRSStream) << "{";

                    if(!parseScript(m_pInterp, tokenPtr->start + 1, tokenPtr->size - 2))
                        return false;

                    (*m_oPVRSStream) << "}";
                }
                else if(tokenPtr->start[0] == '[' && tokenPtr->start[tokenPtr->size - 1] == ']')
                {
                    (*m_oPVRSStream) << "[";

                    if(!parseScript(m_pInterp, tokenPtr->start + 1, tokenPtr->size - 2))
                        return false;

                    (*m_oPVRSStream) << "]";
                }
                else if(tokenPtr->start[0] == '\"' && tokenPtr->start[tokenPtr->size - 1] == '\"')
                {
                    (*m_oPVRSStream) << "\"";
                    if(!parseScript(m_pInterp, tokenPtr->start + 1, tokenPtr->size - 2))
                        return false;

                    (*m_oPVRSStream) << "\"";
                }
                else if(tokenPtr->start[0] == '!' && tokenPtr->start[tokenPtr->size - 1] == ']')
                {
                    (*m_oPVRSStream) << "![";

                    if(!parseScript(m_pInterp, tokenPtr->start + 2, tokenPtr->size - 3))
                        return false;

                    (*m_oPVRSStream) << "]";
                }
                else
                {
                    if(tokenPtr->start != (tokenPtr + 1)->start)
                        (*m_oPVRSStream).write(tokenPtr->start, (tokenPtr + 1)->start - tokenPtr->start);

#if 0
                    Tcl_Parse newParse;
                    const char *p, *end;
                    p = (tokenPtr + 1)->start;
                    end = (tokenPtr + tokenPtr->numComponents)->start + (tokenPtr + tokenPtr->numComponents)->size;
                    while(Tcl_ParseCommand(m_pInterp, p, end - p, 0, &newParse) == TCL_OK)
                    {
                        if(newParse.commentStart != NULL && p != newParse.commentStart)
                        {
                            (*m_oPVRSStream).write(p, newParse.commentStart - p);
                        }
                        else if(newParse.commentStart == NULL && newParse.commandStart != NULL && p != newParse.commandStart)
                        {
                            (*m_oPVRSStream).write(p, newParse.commandStart - p);
                        }

                        if(!hasTvfKeyword(newParse.commandStart, newParse.commandStart + newParse.commandSize))
                        {
                            (*m_oPVRSStream).write(newParse.commandStart, newParse.commandSize);
                        }
                        else if(newParse.commandStart == p && newParse.commandSize == end - p &&
                                newParse.numWords == 1)
                        {
                            (*m_oPVRSStream).write(newParse.commandStart, newParse.commandSize);
                        }
                        else
                        {
                            parseTclCommand(newParse);
                        }

                        p = newParse.commandStart + newParse.commandSize;
                        if (p >= end)
                            break;
                        Tcl_FreeParse(&newParse);
                    }

                    
                    if(p != end)
                    {
                        int errorLine = 1;
                        for(const char *i = m_pScript; i != p; ++i)
                        {
                            if(i[0] == '\n')
                                ++errorLine;
                        }
                        std::cerr << "Compile-Error on line " << errorLine << " of " << m_sFileName <<  ": "
                                  << Tcl_GetString(Tcl_GetObjResult(m_pInterp)) << std::endl;
                        Tcl_FreeParse(&parse);
                        return false;
                    }

                    if(tokenPtr->start + tokenPtr->size != (tokenPtr + tokenPtr->numComponents)->start +
                      (tokenPtr + tokenPtr->numComponents)->size)
                    {
                        (*m_oPVRSStream).write((tokenPtr + tokenPtr->numComponents)->start +
                                            (tokenPtr + tokenPtr->numComponents)->size,
                                            tokenPtr->start + tokenPtr->size -
                                            (tokenPtr + tokenPtr->numComponents)->start -
                                            (tokenPtr + tokenPtr->numComponents)->size);
                    }
#endif
                    Tcl_Parse newParse;
                    for(hvUInt32 index = 1; index <= tokenPtr->numComponents; index++)
                    {
                      	std::string _tokenScript((tokenPtr + index)->start, (tokenPtr + index)->size);
						if(_tokenScript[0] == '[' && _tokenScript[_tokenScript.size() - 1] == ']')
						{
							(*m_oPVRSStream) << "[";

							if(!parseScript(m_pInterp, _tokenScript.c_str() + 1, _tokenScript.size() - 2))
								return false;

							(*m_oPVRSStream) << "]";

							continue;
						}
					    const char *p, *end;
					    p = _tokenScript.c_str();
					    end = p + _tokenScript.size();
	                    while(Tcl_ParseCommand(m_pInterp, _tokenScript.c_str(), _tokenScript.size(), 0, &newParse) == TCL_OK)
	                    {
	                        if(newParse.commentStart != NULL && p != newParse.commentStart)
	                        {
	                            (*m_oPVRSStream).write(p, newParse.commentStart - p);
	                        }
	                        else if(newParse.commentStart == NULL && newParse.commandStart != NULL && p != newParse.commandStart)
	                        {
	                            (*m_oPVRSStream).write(p, newParse.commandStart - p);
	                        }

	                        if(!hasTvfKeyword(newParse.commandStart, newParse.commandStart + newParse.commandSize))
	                        {
	                            (*m_oPVRSStream).write(newParse.commandStart, newParse.commandSize);
	                        }
	                        else if(newParse.commandStart == p && newParse.commandSize == end - p &&
	                                newParse.numWords == 1)
	                        {
	                            (*m_oPVRSStream).write(newParse.commandStart, newParse.commandSize);
	                        }
	                        else
	                        {
	                            parseTclCommand(newParse);
	                        }

	                        p = newParse.commandStart + newParse.commandSize;
	                        if (p >= end)
	                            break;
	                        Tcl_FreeParse(&newParse);
	                    }

	                    if(p != end)
	                    {
	                        int errorLine = 1;
	                        for(const char *i = m_pScript; i != p; ++i)
	                        {
	                            if(i[0] == '\n')
	                                ++errorLine;
	                        }
	                        std::cerr << "Compile-Error on line " << errorLine << " of " << m_sFileName <<  ": "
	                                  << Tcl_GetString(Tcl_GetObjResult(m_pInterp)) << std::endl;
	                        Tcl_FreeParse(&parse);
	                        return false;
	                    }
	                }
                }
            }


#if 0
            hvUInt32 componentIdx = 0;
            for(; componentIdx < tokenPtr->numComponents; ++componentIdx)
            {
                Tcl_Parse newParse;
                const char *p, *end;
                p = (tokenPtr + componentIdx + 1)->start;
                end = p + (tokenPtr + componentIdx + 1)->size;
                while(Tcl_ParseCommand(m_pInterp, p, end - p, 0, &newParse) == TCL_OK)
                {
                    if(newParse.commentSize != 0 && newParse.commentStart != p)
                    {
                        (*m_oPVRSStream).write(p, newParse.commentStart - p);
                    }
                    else if(newParse.commentSize == 0 && newParse.commandStart != p)
                    {
                        (*m_oPVRSStream).write(p, newParse.commandStart - p);
                    }

                    if(!hasTvfKeyword(newParse.commandStart, newParse.commandStart + newParse.commandSize))
                    {
                        (*m_oPVRSStream).write(newParse.commandStart, newParse.commandSize);
                    }
                    else if(newParse.commandStart == (tokenPtr + componentIdx + 1)->start &&
                            newParse.commandSize == (tokenPtr + componentIdx + 1)->size &&
                            newParse.numWords == 1)
                    {
                        (*m_oPVRSStream).write(newParse.commandStart, newParse.commandSize);
                    }
                    else
                    {
                        parseTclCommand(newParse);
                    }

                    p = newParse.commandStart + newParse.commandSize;
                    if (p >= end)
                        break;
                    Tcl_FreeParse(&newParse);
                }

                
                if(p != end)
                {
                    int errorLine = 1;
                    for(const char *i = m_pScript; i != p; ++i)
                    {
                        if(i[0] == '\n')
                            ++errorLine;
                    }
                    std::cerr << "Compile-Error on line " << errorLine << " of " << m_sFileName <<  ": "
                              << Tcl_GetString(Tcl_GetObjResult(m_pInterp)) << std::endl;
                    Tcl_FreeParse(&parse);
                    return false;
                }

                if(componentIdx == tokenPtr->numComponents - 1)
                {
                    if(tokenPtr->start + tokenPtr->size != (tokenPtr + componentIdx + 1)->start +
                      (tokenPtr + componentIdx + 1)->size)
                    {
                        (*m_oPVRSStream).write((tokenPtr + componentIdx + 1)->start +
                                            (tokenPtr + componentIdx + 1)->size,
                                            tokenPtr->start + tokenPtr->size -
                                            (tokenPtr + componentIdx + 1)->start -
                                            (tokenPtr + componentIdx + 1)->size);
                    }
                }
                else
                {
                    if((tokenPtr + componentIdx + 2)->start != (tokenPtr + componentIdx + 1)->start +
                       (tokenPtr + componentIdx + 1)->size)
                    {
                        (*m_oPVRSStream).write((tokenPtr + componentIdx + 1)->start +
                                            (tokenPtr + componentIdx + 1)->size,
                                            (tokenPtr + componentIdx + 2)->start -
                                            (tokenPtr + componentIdx + 1)->start -
                                            (tokenPtr + componentIdx + 1)->size);
                    }
                }
            }
#endif
        }

        if(wordIdx == (hvUInt32)parse.numWords - 1)
        {
            if(parse.commandStart + parse.commandSize != tokenPtr->start + tokenPtr->size)
            {
                (*m_oPVRSStream).write(tokenPtr->start + tokenPtr->size,
                                    parse.commandStart + parse.commandSize -
                                    tokenPtr->start - tokenPtr->size);
            }
        }
        else if((tokenPtr + tokenPtr->numComponents + 1)->start != tokenPtr->start + tokenPtr->size)
        {
            (*m_oPVRSStream).write(tokenPtr->start + tokenPtr->size,
                               (tokenPtr + tokenPtr->numComponents + 1)->start -
                                tokenPtr->start - tokenPtr->size);
        }
    }

    return true;
}

bool
rcsTvfConvertor::parseTvfCommand(Tcl_Parse &parse)
{
    Tcl_Token *tokenPtr = &parse.tokenPtr[0];
    Utassert(tokenPtr->type == TCL_TOKEN_SIMPLE_WORD);
    std::string script(tokenPtr->start, tokenPtr->size);
    if(script.empty())
        return false;

    std::string cmdInfo(tokenPtr->start, tokenPtr->size);
    std::stringstream cmdParser(cmdInfo);
    FlexLexer *pLexer = new tvfFlexLexer(&cmdParser);
    if(getenv("FLEX_DEBUG") != NULL)
    	pLexer->set_debug(1);
    int eType = pLexer->yylex();

    switch(eType)
    {
        case TVF_COMMENT:
            return parseTvfComment(parse);
        case TVF_RULE_CHECK:
            return parseTvfRuleCheck(parse);
        case TVF_RULE_CHECK_COMMENT:
            return parseTvfRuleCheckComment(parse);
        case TVF_SVRF_COMMAND:
            return parseTvfSvrfCommand(parse);
        case TVF_VERBATIM:
            return parseTvfVerbatimCommand(parse);
        case TVF_SETLAYER:
            return parseTvfSetLayerCommand(parse);
        case TVF_OUTLAYER:
            return parseTvfOutLayerCommand(parse);
        case TVF_BLOCK_TRANSCRIPT:
            return parseTvfBlockTranscriptCommand(parse);
        case TVF_UNBLOCK_TRANSCRIPT:
            return parseTvfUnblockTranscriptCommand(parse);
        case TVF_IS_TRANSCRIPT_BLOCKED:
            return parseTvfIsTranscriptBlockedCommand(parse);
        case TVF_DELETE_LAYER:
            return parseTvfDeleteLayerCommand(parse);
        case TVF_ECHO_SVRF:
            return parseTvfEchoSvrfCommand(parse);
        case TVF_ECHO_TO_SVRF_FILE:
            return parseTvfEchoToSvrfFileCommand(parse);
        case TVF_OUTPUT_TO_SVRF:
            return parseTvfOutputToSvrfCommand(parse);
        case TVF_EPILOG:
            return parseTvfEpilogCommand(parse);
        case TVF_SET_GLOBAL_VARIABLE:
            return parseTvfSetGlobalVariableCommand(parse);
        case TVF_GET_GLOBAL_VARIABLE:
            return parseTvfGetGlobalVariableCommand(parse);
        case TVF_UNSET_GLOBAL_VARIABLE:
            return parseTvfUnsetGlobalVariableCommand(parse);
        case TVF_EXISTS_GLOBAL_VARIABLE:
            return parseTvfExistsGlobalVariableCommand(parse);
        case TVF_GET_CALIBRE_VERSION:
            return parseTvfGetCalibreVersionCommand(parse);
        case TVF_GET_TVF_ARG:
            return parseTvfGetTvfArgCommand(parse);
        case TVF_GET_LAYER_ARGS:
            return parseTvfGetLayerArgsCommand(parse);
        case TVF_GET_LAYER_EXTENT:
            return parseTvfGetLayerExtentCommand(parse);
        case TVF_GET_LAYER_NAME:
            return parseTvfGetLayerNameCommand(parse);
        case TVF_IS_LAYER_EMPTY:
            return parseTvfIsLayerEmptyCommand(parse);
        case TVF_PRINT_CALL_STACK:
            return parseTvfPrintCallStackCommand(parse);
        case TVF_SET_DISCLAIMER:
            return parseTvfSetDisclaimerCommand(parse);
        case TVF_SET_RULE_CHECK_INDENTATION:
            return parseTvfSetRuleCheckIndentationCommand(parse);
        case TVF_SET_TRACEBACK_LINE:
            return parseTvfSetTracebackLineCommand(parse);
        case TVF_SVRF_VAR:
            return parseTvfSvrfVarCommand(parse);
        case TVF_SYS_VAR:
            return parseTvfSystemVarCommand(parse);
        case ERC_SETUP_PARALLEL_DEVICE_PARAMS:
            return parseErcSetupParallelDeviceParamsCommand(parse);
        case ERC_EXECUTE_PARALLEL_DEVICE_PARAMS:
            return parseErcExecuteParallelDeviceParamsCommand(parse);
        default:
            break;
    }

    if(script[0] == '[')
    {
        Utassert(script[tokenPtr->size - 1] == ']');
        (*m_oPVRSStream) << "[";
        parseScript(m_pInterp, &script[1], tokenPtr->size - 2);
        (*m_oPVRSStream) << "]";
    }
    else if(script[0] == '{')
    {
        Utassert(script[tokenPtr->size - 1] == '}');
        (*m_oPVRSStream) << "{";
        parseScript(m_pInterp, &script[1], tokenPtr->size - 2);
        (*m_oPVRSStream) << "}";

    }
    else if(script[0] == '\"')
    {
        Utassert(script[tokenPtr->size - 1] == script[0]);
        (*m_oPVRSStream) << script[0];
        parseScript(m_pInterp, &script[1], tokenPtr->size - 2);
        (*m_oPVRSStream) << script[tokenPtr->size - 1];
    }
    else
    {
        std::cout << script << "\n\n\n\n\n";
    }

    return true;
}

bool
rcsTvfConvertor::execute(const char* pScript)
{
    std::string script;
    if(pScript == NULL)
    {
        FILE *pOut = fopen(m_sFileName.c_str(), "r");
        if(pOut == NULL)
        {
            std::cerr << "Error: problem with access, file type, or file open of file: "
                      << m_sFileName << std::endl;
            return false;
        }

        char *pLine = NULL;
        size_t len = 0;
        while(getline(&pLine, &len, pOut) != -1)
        {
            script += pLine;
        }
        if(pLine != NULL)
            ::free(pLine);
        fclose(pOut);
    }
    else
    {
        script = pScript;
    }

    std::string::size_type nPos = script.find("\\[");
    while(nPos != std::string::npos)
    {
    	nPos += 2;
    	while(nPos < script.size())
    	{
    		if(']' == script[nPos])
    		{
    			if('\\' != script[nPos - 1])
    			{
    				script.insert(nPos, "\\");
    				nPos++;
    			}
    			break;
    		}
    		nPos++;
    	}
    	nPos = script.find("\\[", nPos + 2);
    }

    if(m_pInterp == NULL)
    {
        m_pInterp = Tcl_CreateInterp();
        if(Tcl_Init(m_pInterp) == TCL_ERROR)
        {
            std::cerr << "Error: Can not Initialize Tcl: " << m_pInterp->result << std::endl;
            return false;
        }
    }
    if(pScript == NULL)
    {
    	if(rcsManager_T::getInstance()->isRuleFileHasPrefix())
    	{
    		(*m_oPVRSStream) << "#!trs\n";
    	}
    }

    if(rcsManager_T::getInstance()->isTvfFile() && !rcsManager_T::getInstance()->isTvfFunction())
    {
    	(*m_oPVRSStream) << "namespace import trs::* \n";
    }
    m_pScript = script.c_str();
    if(!parseScript(m_pInterp, script.c_str(), script.size()))
        return false;
    return true;
}

bool
rcsTvfConvertor::isTVFRule(const char* pFilename)
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
            if(i < len && (strncasecmp( line + i, "tvf", 3 ) == 0 || strncasecmp( line + i, "trs", 3 ) == 0))
            {
                bTVF = true;
            }
        }
        else if( i < len && (strncasecmp( line+i, "tvf::", 5) == 0 || strncasecmp( line+i, "trs::", 5 ) == 0))
        {
        	bTVF = true;
        }
    }
    else
    {
        printf("Error: file %s is empty\n", pFilename);
        
        
    }

    fclose(fp);

    return bTVF;
}

#if 0


static int
ParseTokens
(
    register const char *src,   
    register int numBytes,  
    int mask,           
    Tcl_Parse *parsePtr    
)
{
    char type;
    int originalTokens, varToken;
    Tcl_Token *tokenPtr;
    Tcl_Parse nested;

    

    originalTokens = parsePtr->numTokens;
    while (numBytes && !((type = CHAR_TYPE(*src)) & mask)) {
    if (parsePtr->numTokens == parsePtr->tokensAvailable) {
        TclExpandTokenArray(parsePtr);
    }
    tokenPtr = &parsePtr->tokenPtr[parsePtr->numTokens];
    tokenPtr->start = src;
    tokenPtr->numComponents = 0;

    if ((type & TYPE_SUBS) == 0) {
        
        if (*src == '\n') {
        parsePtr->lineCnt++;
        }

        while ((++src, --numBytes)
            && !(CHAR_TYPE(*src) & (mask | TYPE_SUBS))) {
            if (*src == '\n') {
            parsePtr->lineCnt++;
            }
        }
        tokenPtr->type = TCL_TOKEN_TEXT;
        tokenPtr->size = src - tokenPtr->start;
        parsePtr->numTokens++;
    } else if (*src == '$') {
        

        varToken = parsePtr->numTokens;
        if (Tcl_ParseVarName(parsePtr->interp, src, numBytes,
            parsePtr, 1) != TCL_OK) {
        return TCL_ERROR;
        }
        src += parsePtr->tokenPtr[varToken].size;
        numBytes -= parsePtr->tokenPtr[varToken].size;
    } else if (*src == '[') {
        

        src++; numBytes--;
        while (1) {
        if (Tcl_ParseCommand(parsePtr->interp, src,
            numBytes, 1, &nested) != TCL_OK) {
            parsePtr->errorType = nested.errorType;
            parsePtr->term = nested.term;
            parsePtr->incomplete = nested.incomplete;
            return TCL_ERROR;
        }
        parsePtr->lineCnt += nested.lineCnt;
        src = nested.commandStart + nested.commandSize;
        numBytes = parsePtr->end - src;

        

        if (nested.tokenPtr != nested.staticTokens) {
            ckfree((char *) nested.tokenPtr);
        }

        

        if ((nested.term < parsePtr->end) && (*nested.term == ']')
            && !nested.incomplete) {
            break;
        }
        if (numBytes == 0) {
            if (parsePtr->interp != NULL) {
            Tcl_SetResult(parsePtr->interp,
                "missing close-bracket", TCL_STATIC);
            }
            parsePtr->errorType = TCL_PARSE_MISSING_BRACKET;
            parsePtr->term = tokenPtr->start;
            parsePtr->incomplete = 1;
            return TCL_ERROR;
        }
        }
        tokenPtr->type = TCL_TOKEN_COMMAND;
        tokenPtr->size = src - tokenPtr->start;
        parsePtr->numTokens++;
    } else if (*src == '\\') {
        
        TclParseBackslash(src, numBytes, &tokenPtr->size, NULL, parsePtr);

        if (tokenPtr->size == 1) {
        
        tokenPtr->type = TCL_TOKEN_TEXT;
        parsePtr->numTokens++;
        src++; numBytes--;
        continue;
        }

        if (src[1] == '\n') {
        parsePtr->lineCnt++;
        if (numBytes == 2) {
            parsePtr->incomplete = 1;
        }

        

        if (mask & TYPE_SPACE) {
            if (parsePtr->numTokens == originalTokens) {
            goto finishToken;
            }
            break;
        }
        }

        tokenPtr->type = TCL_TOKEN_BS;
        parsePtr->numTokens++;
        src += tokenPtr->size;
        numBytes -= tokenPtr->size;
    } else if (*src == 0) {
        tokenPtr->type = TCL_TOKEN_TEXT;
        tokenPtr->size = 1;
        parsePtr->numTokens++;
        src++; numBytes--;
    } else {
        panic("ParseTokens encountered unknown character");
    }
    }
    if (parsePtr->numTokens == originalTokens) {
    
    if (parsePtr->numTokens == parsePtr->tokensAvailable) {
        TclExpandTokenArray(parsePtr);
    }
    tokenPtr = &parsePtr->tokenPtr[parsePtr->numTokens];
    tokenPtr->start = src;
    tokenPtr->numComponents = 0;

    finishToken:
    tokenPtr->type = TCL_TOKEN_TEXT;
    tokenPtr->size = 0;
    parsePtr->numTokens++;
    }
    parsePtr->term = src;
    return TCL_OK;
}



int
TclCompileScript(Tcl_Interp *interp, const char *script, int numBytes, int nested, CompileEnv *envPtr)
#if 0
    Tcl_Interp *interp;
    
    CONST char *script;     
    int numBytes;       
    int nested;         
    CompileEnv *envPtr;     
#endif
{
    Interp *iPtr = (Interp *) interp;
    Tcl_Parse parse;
    int lastTopLevelCmdIndex = -1;
                    
    int startCodeOffset = -1;   
    unsigned char *entryCodeNext = envPtr->codeNext;
    const char *p, *next;
    Namespace *cmdNsPtr;
    Command *cmdPtr;
    Tcl_Token *tokenPtr;
    int bytesLeft, isFirstCmd, gotParse, wordIdx, currCmdIndex;
    int commandLength, objIndex, code;
    Tcl_DString ds;

    Tcl_DStringInit(&ds);

    if (numBytes < 0) {
    numBytes = strlen(script);
    }
    Tcl_ResetResult(interp);
    isFirstCmd = 1;

    

    p = script;
    bytesLeft = numBytes;
    gotParse = 0;
    do {
    envPtr->startLine = iPtr->lineNum;
    if (Tcl_ParseCommand(interp, p, bytesLeft, nested, &parse) != TCL_OK) {
        code = TCL_ERROR;
        goto error;
    }
    parse.lineNum = iPtr->lineNum; 
    gotParse = 1;
    if (nested) {
        

        if (parse.term == (script + numBytes)) {
        

        Tcl_SetObjResult(interp,
            Tcl_NewStringObj("missing close-bracket", -1));
        code = TCL_ERROR;
        goto error;
        }
    }
    if (parse.numWords > 0) {
        

        if (!isFirstCmd) {
        TclEmitOpcode(INST_POP, envPtr);
        if (!nested) {
            envPtr->cmdMapPtr[lastTopLevelCmdIndex].numCodeBytes =
               (envPtr->codeNext - envPtr->codeStart)
               - startCodeOffset;
        }
        }

        

        commandLength = parse.commandSize;
        if (parse.term == parse.commandStart + commandLength - 1) {
        

        commandLength -= 1;
        }

#ifdef TCL_COMPILE_DEBUG
        

        if ((tclTraceCompile >= 1)
            && !nested && (envPtr->procPtr == NULL)) {
        fprintf(stdout, "  Compiling: ");
        TclPrintSource(stdout, parse.commandStart,
            TclMin(commandLength, 55));
        fprintf(stdout, "\n");
        }
#endif
        

        envPtr->numCommands++;
        currCmdIndex = (envPtr->numCommands - 1);
        if (!nested) {
        lastTopLevelCmdIndex = currCmdIndex;
        }
        startCodeOffset = (envPtr->codeNext - envPtr->codeStart);
        EnterCmdStartData(envPtr, currCmdIndex,
                (parse.commandStart - envPtr->source), startCodeOffset,
                iPtr);

        for (wordIdx = 0, tokenPtr = parse.tokenPtr;
            wordIdx < parse.numWords;
            wordIdx++, tokenPtr += (tokenPtr->numComponents + 1)) {
        if (tokenPtr->type == TCL_TOKEN_SIMPLE_WORD) {
            

            if (wordIdx == 0) {
            if (envPtr->procPtr != NULL) {
                cmdNsPtr = envPtr->procPtr->cmdPtr->nsPtr;
            } else {
                cmdNsPtr = NULL; 
            }

            

            Tcl_DStringSetLength(&ds, 0);
            Tcl_DStringAppend(&ds, tokenPtr[1].start,
                tokenPtr[1].size);

            cmdPtr = (Command *) Tcl_FindCommand(interp,
                Tcl_DStringValue(&ds),
                    (Tcl_Namespace *) cmdNsPtr,  0);

            if ((cmdPtr != NULL)
                    && (cmdPtr->compileProc != NULL)
                    && !(cmdPtr->flags & CMD_HAS_EXEC_TRACES)
                    && !(iPtr->flags & DONT_COMPILE_CMDS_INLINE)) {
                int savedNumCmds = envPtr->numCommands;
                unsigned int savedCodeNext =
                    envPtr->codeNext - envPtr->codeStart;

                code = (*(cmdPtr->compileProc))(interp, &parse,
                        envPtr);
                if (code == TCL_OK) {
                goto finishCommand;
                } else if (code == TCL_OUT_LINE_COMPILE) {
                
                envPtr->numCommands = savedNumCmds;
                envPtr->codeNext = envPtr->codeStart + savedCodeNext;
                } else { 
                
                envPtr->numCommands--;
                goto log;
                }
            }

            

            objIndex = TclRegisterNewLiteral(envPtr,
                tokenPtr[1].start, tokenPtr[1].size);
            if (cmdPtr != NULL) {
                TclSetCmdNameObj(interp,
                       envPtr->literalArrayPtr[objIndex].objPtr,
                   cmdPtr);
            }
            } else {
            objIndex = TclRegisterNewLiteral(envPtr,
                tokenPtr[1].start, tokenPtr[1].size);
            }
            TclEmitPush(objIndex, envPtr);
        } else {
            

            code = TclCompileTokens(interp, tokenPtr+1,
                tokenPtr->numComponents, envPtr);
            if (code != TCL_OK) {
            goto log;
            }
        }
        }

        

        if (wordIdx > 0) {
        if (wordIdx <= 255) {
            TclEmitInstInt1(INST_INVOKE_STK1, wordIdx, envPtr);
        } else {
            TclEmitInstInt4(INST_INVOKE_STK4, wordIdx, envPtr);
        }
        }

        

        finishCommand:
        EnterCmdExtentData(envPtr, currCmdIndex, commandLength,
            (envPtr->codeNext-envPtr->codeStart) - startCodeOffset);
        isFirstCmd = 0;
    } 

    

    next = parse.commandStart + parse.commandSize;
    bytesLeft -= (next - p);
    p = next;
    if (iPtr != NULL) {
        iPtr->lineNum = parse.lineNum + parse.lineCnt;
    }
    Tcl_FreeParse(&parse);
    gotParse = 0;
    if (nested && (*parse.term == ']')) {
        

        break;
    }
    } while (bytesLeft > 0);

    

    if (envPtr->codeNext == entryCodeNext) {
    TclEmitPush(TclRegisterLiteral(envPtr, "", 0,  0),
            envPtr);
    }

    if (nested) {
    

    iPtr->termOffset = (p - 1) - script;
    } else {
    iPtr->termOffset = (p - script);
    }
    Tcl_DStringFree(&ds);
    return TCL_OK;

    error:
    

    commandLength = parse.commandSize;
    if (parse.term == parse.commandStart + commandLength - 1) {
    

    commandLength -= 1;
    }

    log:
    LogCompilationInfo(interp, script, parse.commandStart, commandLength);
    if (gotParse) {
    Tcl_FreeParse(&parse);
    }
    iPtr->termOffset = (p - script);
    Tcl_DStringFree(&ds);
    return code;
}



int
TclCompileTokens(interp, tokenPtr, count, envPtr)
    Tcl_Interp *interp;     
    Tcl_Token *tokenPtr;    
    int count;          
    CompileEnv *envPtr;     
{
    Tcl_DString textBuffer; 
    char buffer[TCL_UTF_MAX];
    CONST char *name, *p;
    int numObjsToConcat, nameBytes, localVarName, localVar;
    int length, i, code;
    unsigned char *entryCodeNext = envPtr->codeNext;

    Tcl_DStringInit(&textBuffer);
    numObjsToConcat = 0;
    for ( ;  count > 0;  count--, tokenPtr++) {
    switch (tokenPtr->type) {
        case TCL_TOKEN_TEXT:
        Tcl_DStringAppend(&textBuffer, tokenPtr->start,
            tokenPtr->size);
        break;

        case TCL_TOKEN_BS:
        length = Tcl_UtfBackslash(tokenPtr->start, (int *) NULL,
            buffer);
        Tcl_DStringAppend(&textBuffer, buffer, length);
        break;

        case TCL_TOKEN_COMMAND:
        

        if (Tcl_DStringLength(&textBuffer) > 0) {
            int literal;

            literal = TclRegisterLiteral(envPtr,
                Tcl_DStringValue(&textBuffer),
                Tcl_DStringLength(&textBuffer),  0);
            TclEmitPush(literal, envPtr);
            numObjsToConcat++;
            Tcl_DStringFree(&textBuffer);
        }

        code = TclCompileScript(interp, tokenPtr->start+1,
            tokenPtr->size-2,  0, envPtr);
        if (code != TCL_OK) {
            goto error;
        }
        numObjsToConcat++;
        break;

        case TCL_TOKEN_VARIABLE:
        

        if (Tcl_DStringLength(&textBuffer) > 0) {
            int literal;

            literal = TclRegisterLiteral(envPtr,
                Tcl_DStringValue(&textBuffer),
                Tcl_DStringLength(&textBuffer),  0);
            TclEmitPush(literal, envPtr);
            numObjsToConcat++;
            Tcl_DStringFree(&textBuffer);
        }

        

        name = tokenPtr[1].start;
        nameBytes = tokenPtr[1].size;
        localVarName = -1;
        if (envPtr->procPtr != NULL) {
            localVarName = 1;
            for (i = 0, p = name;  i < nameBytes;  i++, p++) {
            if ((*p == ':') && (i < (nameBytes-1))
                    && (*(p+1) == ':')) {
                localVarName = -1;
                break;
            } else if ((*p == '(')
                    && (tokenPtr->numComponents == 1)
                && (*(name + nameBytes - 1) == ')')) {
                localVarName = 0;
                break;
            }
            }
        }

        

        localVar = -1;
        if (localVarName != -1) {
            localVar = TclFindCompiledLocal(name, nameBytes,
                    localVarName,  0, envPtr->procPtr);
        }
        if (localVar < 0) {
            TclEmitPush(TclRegisterNewLiteral(envPtr, name, nameBytes),
                envPtr);
        }

        

        if (tokenPtr->numComponents == 1) {
            if (localVar < 0) {
            TclEmitOpcode(INST_LOAD_SCALAR_STK, envPtr);
            } else if (localVar <= 255) {
            TclEmitInstInt1(INST_LOAD_SCALAR1, localVar,
                    envPtr);
            } else {
            TclEmitInstInt4(INST_LOAD_SCALAR4, localVar,
                envPtr);
            }
        } else {
            code = TclCompileTokens(interp, tokenPtr+2,
                tokenPtr->numComponents-1, envPtr);
            if (code != TCL_OK) {
            char errorBuffer[150];
            sprintf(errorBuffer,
                    "\n    (parsing index for array \"%.*s\")",
                ((nameBytes > 100)? 100 : nameBytes), name);
            Tcl_AddObjErrorInfo(interp, errorBuffer, -1);
            goto error;
            }
            if (localVar < 0) {
            TclEmitOpcode(INST_LOAD_ARRAY_STK, envPtr);
            } else if (localVar <= 255) {
            TclEmitInstInt1(INST_LOAD_ARRAY1, localVar,
                    envPtr);
            } else {
            TclEmitInstInt4(INST_LOAD_ARRAY4, localVar,
                    envPtr);
            }
        }
        numObjsToConcat++;
        count -= tokenPtr->numComponents;
        tokenPtr += tokenPtr->numComponents;
        break;

        default:
        panic("Unexpected token type in TclCompileTokens");
    }
    }

    

    if (Tcl_DStringLength(&textBuffer) > 0) {
    int literal;

    literal = TclRegisterLiteral(envPtr, Tcl_DStringValue(&textBuffer),
            Tcl_DStringLength(&textBuffer),  0);
    TclEmitPush(literal, envPtr);
    numObjsToConcat++;
    }

    

    while (numObjsToConcat > 255) {
    TclEmitInstInt1(INST_CONCAT1, 255, envPtr);
    numObjsToConcat -= 254; 
    }
    if (numObjsToConcat > 1) {
    TclEmitInstInt1(INST_CONCAT1, numObjsToConcat, envPtr);
    }

    

    if (envPtr->codeNext == entryCodeNext) {
    TclEmitPush(TclRegisterLiteral(envPtr, "", 0,  0),
            envPtr);
    }
    Tcl_DStringFree(&textBuffer);
    return TCL_OK;

    error:
    Tcl_DStringFree(&textBuffer);
    return code;
}



int
TclCompileCmdWord(interp, tokenPtr, count, envPtr)
    Tcl_Interp *interp;     
    Tcl_Token *tokenPtr;    
    int count;          
    CompileEnv *envPtr;     
{
    int code;

    

    if ((count == 1) && (tokenPtr->type == TCL_TOKEN_TEXT)) {
    code = TclCompileScript(interp, tokenPtr->start, tokenPtr->size,
             0, envPtr);
    return code;
    }

    

    code = TclCompileTokens(interp, tokenPtr, count, envPtr);
    if (code != TCL_OK) {
    return code;
    }
    TclEmitOpcode(INST_EVAL_STK, envPtr);
    return TCL_OK;
}



int
TclCompileExprWords(interp, tokenPtr, numWords, envPtr)
    Tcl_Interp *interp;     
    Tcl_Token *tokenPtr;    
    int numWords;       
    CompileEnv *envPtr;     
{
    Tcl_Token *wordPtr;
    int numBytes, i, code;
    CONST char *script;

    code = TCL_OK;

    

    if ((numWords == 1) && (tokenPtr->type == TCL_TOKEN_SIMPLE_WORD)) {
    script = tokenPtr[1].start;
    numBytes = tokenPtr[1].size;
    code = TclCompileExpr(interp, script, numBytes, envPtr);
    return code;
    }

    

    wordPtr = tokenPtr;
    for (i = 0;  i < numWords;  i++) {
    code = TclCompileTokens(interp, wordPtr+1, wordPtr->numComponents,
                envPtr);
    if (code != TCL_OK) {
        break;
    }
    if (i < (numWords - 1)) {
        TclEmitPush(TclRegisterLiteral(envPtr, " ", 1,  0),
                envPtr);
    }
    wordPtr += (wordPtr->numComponents + 1);
    }
    if (code == TCL_OK) {
    int concatItems = 2*numWords - 1;
    while (concatItems > 255) {
        TclEmitInstInt1(INST_CONCAT1, 255, envPtr);
        concatItems -= 254;
    }
    if (concatItems > 1) {
        TclEmitInstInt1(INST_CONCAT1, concatItems, envPtr);
    }
    TclEmitOpcode(INST_EXPR_STK, envPtr);
    }

    return code;
}
#endif

