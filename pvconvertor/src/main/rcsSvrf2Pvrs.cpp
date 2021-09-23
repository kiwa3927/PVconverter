#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>

#include "public/synnode/rcsSynRootNode.h"
#include "public/manager/rcErrorManager.h"
#include "public/manager/rcsManager.h"
#include "preprocess/rcsPreprocessor.h"
#include "preprocess/rcsCheckTextExtractor.h"
#include "preprocess/rcsFormat.h"
#include "lexical/rcsLexParser.h"
#include "lexical/rcsMacroParser.h"
#include "syntax/rcsSynConvertor.h"
#include "tvf/rcsTvfCompiler.h"
#include "tvf/rcpTrsCompiler.h"
#include "tvf/rcsTvfConvertor.h"

#include "rcsSvrf2Pvrs.h"

hvUInt32 rcsSvrf2Pvrs::mCurLineNo = 0;
hvUInt32 rcsSvrf2Pvrs::mStrSearchBegin = 0;
std::string rcsSvrf2Pvrs::mCompleteStr = "";

bool
rcsSvrf2Pvrs::isTVFRule(const char* pFilename, bool* isTVF, bool* isTRS)
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

rcsSvrf2Pvrs::~rcsSvrf2Pvrs()
{
    remove(rcsManager_T::getInstance()->getTmpFileName());
    remove(".svrf2pvrs.switch.tmp");
}

bool
rcsSvrf2Pvrs::executeForTvfCommand(std::string &script)
{
    try
    {
        static const char *pSvrfGenFromTvf = ".svrfgenformtvf";
        FILE *pFile = fopen(pSvrfGenFromTvf, "w");
        fputs(script.c_str(), pFile);
        fclose(pFile);

        std::ofstream outFile(rcsManager_T::getInstance()->getTmpFileName());
        rcsPreProcessor_T processor(pSvrfGenFromTvf, true, outFile);
        processor.initial();
        processor.execute();
        outFile.flush();
        outFile.close();

        rcsCheckTextExtractor_T extractor(rcsManager_T::getInstance()->getTmpFileName());
        extractor.execute();
        std::list<rcsToken_T> tokens;
        std::set<hvUInt32> vUnstoredCommentLineNumbers;
        rcsLexParser_T lexparser(tokens, vUnstoredCommentLineNumbers);
        lexparser.parse(rcsManager_T::getInstance()->getTmpFileName());
        std::map<hvUInt32, std::pair<hvUInt32, bool> > blankLinesBefore;
        rcsLexMacroParser_T macroparser(tokens, blankLinesBefore, vUnstoredCommentLineNumbers, false);
        macroparser.parse();

        bool bIsRunTVF = false;
		if(rcsManager_T::getInstance()->isTrsFlag())
		{
			bIsRunTVF = false;
		}
		else
		{
			bIsRunTVF = true;
		}

        rcsSynConvertor convertor(tokens,bIsRunTVF);
        rcsSynRootNode_T *pNode = convertor.execute(blankLinesBefore);

        delete pNode;

        remove(rcsManager_T::getInstance()->getTmpFileName());
        remove(pSvrfGenFromTvf);
    }
    catch(const rcErrorNode_T &error)
    {
        s_errManager.addError(error, std::cout);
        return false;
    }
    return true;
}

void
rcsSvrf2Pvrs::processIncludeFileRespectively(rcsPreProcessor_T &preProcessor)
{
    
    if(preProcessor.getIncludeFileVectorSize())
    {
        rcsSvrf2Pvrs convertor;
        std::string switchFile;

        for(hvUInt32 i = 0; i < preProcessor.getIncludeFileVectorSize(); i++)
        {
            convertor.setLastGlobalLine(rcsPreProcessor_T::lastGlobalLine());
        	std::string includeFileName = preProcessor.getIncludeFile(i);
        	size_t it;
        	std::string inputFileName;
        	if((it = includeFileName.find(".trs")) != std::string::npos)
        	{
        		std::string tmp = includeFileName;
        		inputFileName = tmp.erase(it, 4);
        	}
        	else if((it = includeFileName.find(".pvrs")) != std::string::npos)
        	{
        		std::string tmp = includeFileName;
        		inputFileName = tmp.erase(it, 5);
        	}

        	rcsManager_T::getInstance()->setFirstTime(false);

        	convertor.execute(inputFileName, switchFile, includeFileName,
							   rcsManager_T::getInstance()->isSvrfComment(),
							   rcsManager_T::getInstance()->isConvertSwitch(),
							   rcsManager_T::getInstance()->isFlattenMacro(),
							   rcsManager_T::getInstance()->outputComment(),
							   std::cout,
							   rcsManager_T::getInstance()->isNewPvrs(),
							   rcsManager_T::getInstance()->needExpandTmpLayer(),
							   rcsManager_T::getInstance()->isConvertIncludeFile(),
							   rcsManager_T::getInstance()->isTvf2Trs());
        }
    }
}

bool
rcsSvrf2Pvrs::execute(std::string sInFile, const std::string &sSwitchFile,
                      const std::string &sOutFile, bool outSvrf,
                      bool convertSwitch, bool flattenMacro, bool outComment,
                      std::ostream &out, bool isNewPvrs, bool needExpandTmpLayer,
                      bool convertIncludeFile, bool trsFlag,
                      const std::string &sTmpLayerSuffix)
{
    try
    {
        rcsManager_T::getInstance()->setPVRSOutFile(sOutFile);
        bool bOpen = rcsManager_T::getInstance()->beginWrite();
        if (!bOpen)
        {
            s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, OPEN1, 0, sOutFile), out);
            s_errManager.Report(out);
            rcsManager_T::getInstance()->endWrite();
            return false;
        }
        rcsManager_T::getInstance()->setOutputComment(outComment);
        rcsManager_T::getInstance()->setOutputSvrfComment(outSvrf);
        rcsManager_T::getInstance()->setExpandTmpLayer(needExpandTmpLayer);
        rcsManager_T::getInstance()->setNewPvrs(isNewPvrs);
        rcsManager_T::getInstance()->setFlattenMacro(flattenMacro);
        rcsManager_T::getInstance()->setConvertSwitch(convertSwitch);
        rcsManager_T::getInstance()->setConvertIncludeFile(convertIncludeFile);
        rcsManager_T::getInstance()->setTvf2Trs(trsFlag);
        rcsManager_T::getInstance()->setTrsFlag(trsFlag);
        rcsManager_T::getInstance()->setTmpLayerSuffix(sTmpLayerSuffix);

        if(!sSwitchFile.empty())
            convertSwitch = false;

        bool isTvf = false;
        std::string fileType = "";
        std::string includeFlag = (rcsManager_T::getInstance()->isConvertIncludeFile() &&
								   !rcsManager_T::getInstance()->isFirstTime()) ? "INCLUDE" : "";
        isTvf = isTVFRule(sInFile.c_str()) ? true : false;
        fileType = isTvf ? " TVF " : " SVRF ";
        rcsManager_T::getInstance()->setTvfFile(isTvf);

        if(!isTvf && trsFlag)
        {
        	out << "\n";
        	out << "Run Error:Rule File is not TVF format, can't use \"-trs\" option, please delete it.\n";
        	out << "\n";
        	return false;
        }

        out << "\n";
		out << "------        " << includeFlag << fileType << "RULE FILE CONVERTOR BEGIN           ------\n";
		out << "\n";

#ifdef DEBUG
		out << "inputFile:" << sInFile << std::endl;
		out << "outputFile:" << sOutFile << std::endl;
#endif

        if(access(sInFile.c_str(), F_OK|R_OK) != 0)
        {
            s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, OPEN1, 0, sInFile), out);
            s_errManager.Report(out);
            rcsManager_T::getInstance()->endWrite();
            return false;
        }

        struct stat filestat;
        stat(sInFile.c_str(), &filestat);
        if(!S_ISREG(filestat.st_mode))
        {
            s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, OPEN1, 0, sInFile), out);
            s_errManager.Report(out);
            rcsManager_T::getInstance()->endWrite();

            return false;
        }

        if(isTvf && rcsManager_T::getInstance()->isTvf2Trs())
        {
            rcsTvfConvertor convertor(sInFile);
            convertor.execute();
            rcsManager_T::getInstance()->endWrite();

            out << "\n";
            out << "------        " << includeFlag << fileType << "RULE FILE CONVERTOR FINISHED        ------\n";
            out << "\n";



            return true;
        }
        else if(isTvf && !rcsManager_T::getInstance()->isTvf2Trs())
        {
            rcsTVFCompiler_T tvf2svrf(sInFile);

            int n = tvf2svrf.tvf_compiler();
            if(n != 0)
            {
                s_errManager.Report(out);
                rcsManager_T::getInstance()->endWrite();

                out << "\n";
                out << "------        " << includeFlag << fileType << "RULE FILE CONVERTOR FAILED          ------\n\n";
                out << "\n";
                return false;
            }

            sInFile = tvf2svrf.getsvrffilename();
        }

        if(!sSwitchFile.empty())
        {
            convertSwitch = false;

            std::string sTmpFile = ".svrf2pvrs.switch.tmp";
            std::ofstream writer(sTmpFile.c_str());
            writer << "INCLUDE \"" << sSwitchFile << "\"" << std::endl;
            writer << "INCLUDE \"" << sInFile << "\"" << std::endl;
            writer.close();

            sInFile = sTmpFile;
        }

        if(!convertSwitch)
            flattenMacro  = true;

        std::ofstream outFile(rcsManager_T::getInstance()->getTmpFileName());
        if (!outFile)
        {
            s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, OPEN1, 0, rcsManager_T::getInstance()->getTmpFileName()), out);
            s_errManager.Report(out);
            rcsManager_T::getInstance()->endWrite();
            return false;
        }
        rcsPreProcessor_T processor(sInFile.c_str(), convertSwitch, outFile);
        processor.initial(this->getLastGlobalLine());
        processor.execute();
        outFile.flush();
        outFile.close();

#ifdef __DEBUG__
        {
            std::cout << "-------------- AFTER PREPROCESS ---------------" << std::endl;
            std::ifstream reader(rcsManager_T::getInstance()->getTmpFileName());
            std::string line;
            while (getline(reader, line))
            {
                std::cout << line << std::endl;
            }
            std::cout << std::endl;
        }
#endif



        if(isTvf)
        {
        	rcsFormat format(rcsManager_T::getInstance()->getTmpFileName());
        	format.execute();
        }

		rcsCheckTextExtractor_T extractor(rcsManager_T::getInstance()->getTmpFileName());
		extractor.execute();
		std::list<rcsToken_T> tokens;
		std::set<hvUInt32> vUnstoredCommentLineNumbers;
		rcsLexParser_T lexparser(tokens, vUnstoredCommentLineNumbers);
		lexparser.parse(rcsManager_T::getInstance()->getTmpFileName());

#ifdef __DEBUG__
        {
		    std::cout << "\n----------- AFTER LEX PARSER ----------------\n";
		    printTokenStream(tokens.begin(), tokens.end());
		    std::cout << std::endl;
        }
#endif

        std::map<hvUInt32, std::pair<hvUInt32, bool> > blankLinesBefore;
		rcsLexMacroParser_T macroparser(tokens, blankLinesBefore, vUnstoredCommentLineNumbers, flattenMacro);
		macroparser.parse();

        transGlobalLineForIncFile(tokens);

#ifdef __DEBUG__
        {
		    std::cout << "\n----------- AFTER MACRO PARSER ----------------\n";
		    printTokenStream(tokens.begin(), tokens.end());
		    std::cout << std::endl;
        }
#endif

		rcsSynConvertor convertor(tokens);

        if (!flattenMacro)
            convertor.setMacroParaMap(&macroparser.m_macrosParaSizeMap);

		rcsSynRootNode_T *pNode = convertor.execute(blankLinesBefore);

		delete pNode;


        if(isTvf)
            remove(rcsTVFCompiler_T::getsvrffilename());

        remove(".svrf2pvrs.switch.tmp");
        remove(rcsManager_T::getInstance()->getTmpFileName());

        out << "\n";
        out << "------        " << includeFlag << fileType << "RULE FILE CONVERTOR FINISHED        ------\n\n";
        out << "\n";

        s_errManager.Report(out);
        rcsManager_T::getInstance()->endWrite();
        processIncludeFileRespectively(processor);

        return true;
    }
    catch(const rcErrorNode_T &error)
    {
        s_errManager.addError(error, out);
    }
    return false;
}

bool
rcsSvrf2Pvrs::executeIncludeFile(std::string sInFile, const std::string &sSwitchFile,
							const std::string &sOutFile, std::ostream &out)
{
    try
    {
        rcsManager_T::getInstance()->setPVRSOutFile(sOutFile);
        bool bOpen = rcsManager_T::getInstance()->beginWrite();
        if (!bOpen)
        {
            s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, OPEN1, 0, sOutFile), out);
            s_errManager.Report(out);
            rcsManager_T::getInstance()->endWrite();
            return false;
        }
        bool convertSwitch 		= rcsManager_T::getInstance()->isConvertSwitch();
        bool flattenMacro 		= rcsManager_T::getInstance()->isFlattenMacro();

        if(!sSwitchFile.empty())
            convertSwitch = false;

        bool isTvf = false;
        std::string fileType = "";
        std::string includeFlag = "INCLUDE";
        isTvf = isTVFRule(sInFile.c_str()) ? true : false;
        fileType = isTvf ? " TVF " : " SVRF ";

        out << "\n";
		out << "------        " << includeFlag << fileType << "RULE FILE CONVERTOR BEGIN           ------\n";
		out << "\n";

        if(access(sInFile.c_str(), F_OK|R_OK) != 0)
        {
            s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, OPEN1, 0, sInFile), out);
            s_errManager.Report(out);
            rcsManager_T::getInstance()->endWrite();
            return false;
        }

        struct stat filestat;
        stat(sInFile.c_str(), &filestat);
        if(!S_ISREG(filestat.st_mode))
        {
            s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, OPEN1, 0, sInFile), out);
            s_errManager.Report(out);
            rcsManager_T::getInstance()->endWrite();
            return false;
        }

        if(isTvf && rcsManager_T::getInstance()->isTvf2Trs())
        {
            rcsTvfConvertor convertor(sInFile);
            convertor.execute();

            if(!rcsManager_T::getInstance()->isConvertIncludeFile())
            {
            	rcsManager_T::getInstance()->endWrite();
            }

            out << "\n";
            out << "------        " <<  includeFlag << fileType << "RULE FILE CONVERTOR FINISHED        ------\n";
            out << "\n";

            return true;
        }
        else if(isTvf && !rcsManager_T::getInstance()->isTvf2Trs())
        {
            rcsTVFCompiler_T tvf2svrf(sInFile);

            int n = tvf2svrf.tvf_compiler();
            if(n != 0)
            {
                s_errManager.Report(out);
                rcsManager_T::getInstance()->endWrite();

                out << "\n";
                out << "------        " << includeFlag << fileType << "RULE FILE CONVERTOR FAILED          ------\n\n";
                out << "\n";
                return false;
            }

            sInFile = tvf2svrf.getsvrffilename();
        }

        if(!sSwitchFile.empty())
        {
            convertSwitch = false;

            std::string sTmpFile = ".svrf2pvrs.switch.tmp";
            std::ofstream writer(sTmpFile.c_str());
            writer << "INCLUDE \"" << sSwitchFile << "\"" << std::endl;
            writer << "INCLUDE \"" << sInFile << "\"" << std::endl;
            writer.close();

            sInFile = sTmpFile;
        }

        if(!convertSwitch)
            flattenMacro  = true;

        std::ofstream outFile(rcsManager_T::getInstance()->getTmpFileName());
        rcsPreProcessor_T processor(sInFile.c_str(), convertSwitch, outFile);
        processor.initial();
        processor.execute();
        outFile.flush();
        outFile.close();

        rcsFormat format(rcsManager_T::getInstance()->getTmpFileName());
        format.execute();

        rcsCheckTextExtractor_T extractor(rcsManager_T::getInstance()->getTmpFileName());
        extractor.execute();
        std::list<rcsToken_T> tokens;
        std::set<hvUInt32> vUnstoredCommentLineNumbers;
        rcsLexParser_T lexparser(tokens, vUnstoredCommentLineNumbers);
        lexparser.parse(rcsManager_T::getInstance()->getTmpFileName());
        std::map<hvUInt32, std::pair<hvUInt32, bool> > blankLinesBefore;
        rcsLexMacroParser_T macroparser(tokens, blankLinesBefore, vUnstoredCommentLineNumbers, flattenMacro);
        macroparser.parse();
        rcsSynConvertor convertor(tokens);
        rcsSynRootNode_T *pNode = convertor.execute(blankLinesBefore);

        delete pNode;

        if(isTvf)
            remove(rcsTVFCompiler_T::getsvrffilename());

        remove(".svrf2pvrs.switch.tmp");
        remove(rcsManager_T::getInstance()->getTmpFileName());

        out << "\n";
        out << "------        " <<  includeFlag << fileType << "RULE FILE CONVERTOR FINISHED        ------\n\n";
        out << "\n";

        

        if(!rcsManager_T::getInstance()->isConvertIncludeFile())
        {
        	rcsManager_T::getInstance()->endWrite();
        }

        return true;
    }
    catch(const rcErrorNode_T &error)
    {
        s_errManager.addError(error, out);
    }
    return false;
}

bool
rcsSvrf2Pvrs::executeTvfPreprocess(std::string sInFile, const std::string &sOutFile, std::ostream &out)
{
    std::string fileType = "";
    bool isTRS = false;
    bool isTVF = false;
    isTVFRule(sInFile.c_str(), &isTVF, &isTRS);

    if(isTVF)
    {
    	fileType = " TVF ";
    	out << "\n";
		out << "------        " << fileType << "RULE FILE PPEPROCESSOR BEGIN           ------\n\n";
		out << "\n";

        rcsTVFCompiler_T tvf2svrf(sInFile, sOutFile);
	    int n = tvf2svrf.tvf_compiler();
	    if(n != 0)
	    {
	        s_errManager.Report(out);

	        out << "\n";
	        out << "------        " << fileType << "RULE FILE PPEPROCESSOR FAILED          ------\n\n";
	        out << "\n";
	        return false;
	    }
	    else
	    {
	        out << "\n";
	    	out << "------        " << fileType << "RULE FILE PPEPROCESSOR FINISHED        ------\n\n";
	    	out << "\n";
	    	return true;
	    }
    }
    else if(isTRS)
    {
    	fileType = " TRS ";
    	out << "\n";
		out << "------        " << fileType << "RULE FILE PPEPROCESSOR BEGIN           ------\n\n";
		out << "\n";

        rcpTRSCompiler trs2pvrs(sInFile.c_str(), sOutFile.c_str());
	    int n = trs2pvrs.trs_compiler();
	    if(n != 0)
	    {
	        s_errManager.Report(out);

	        out << "\n";
	        out << "------        " << fileType << "RULE FILE PPEPROCESSOR FAILED        ------\n\n";
	        out << "\n";
	        return false;
	    }
	    else
	    {
	        out << "\n";
	    	out << "------        " << fileType << "RULE FILE PPEPROCESSOR FINISHED        ------\n\n";
	    	out << "\n";
	    	return true;
	    }
    }
    else
    {
    	out << "Error: The rule file is not TVF or TRS format! \n";
    	return false;
    }
}

hvUInt32
rcsSvrf2Pvrs::increaseCurLineNo(hvUInt32 step)
{
	return rcsSvrf2Pvrs::mCurLineNo += step;
}

hvUInt32
rcsSvrf2Pvrs::decreaseCurLineNo()
{
	return --rcsSvrf2Pvrs::mCurLineNo;
}

hvUInt32
rcsSvrf2Pvrs::setCurLineNo(hvUInt32 lineNo)
{
	rcsSvrf2Pvrs::mCurLineNo = lineNo;
	return rcsSvrf2Pvrs::mCurLineNo;
}

hvUInt32
rcsSvrf2Pvrs::getCurLineNo()
{
	return rcsSvrf2Pvrs::mCurLineNo;
}

void
rcsSvrf2Pvrs::setCompleteStr(std::string& str)
{
	mCompleteStr = str;
}

void
rcsSvrf2Pvrs::setStrSearchBegin(hvUInt32 pos)
{
	mStrSearchBegin = pos;
}

hvUInt32
rcsSvrf2Pvrs::findLineNumInTvfFun(std::string command)
{
	hvUInt32 nLines = 0;
	std::string::size_type found;
	if(mCompleteStr != "")
	{
		found = mCompleteStr.find(command, mStrSearchBegin);
		std::string substr = mCompleteStr.substr(mStrSearchBegin, found - mStrSearchBegin);
		for(hvUInt32 i = 0; i < substr.size(); i++)
		{
			if(substr[i] == '\n')
				nLines++;
		}
		mStrSearchBegin = found + command.size();
	}
    return nLines;
}

#ifdef DEBUG
void
printTokenStream(std::list<rcsToken_T>::iterator first, std::list<rcsToken_T>::iterator last)
{
    hvUInt32 nLineNo = first->nLineNo;
    while (first != last)
    {
        if (nLineNo != first->nLineNo)
        {
            std::cout << "\n";
            nLineNo = first->nLineNo;
        }
        std::cout << first->sValue << "(" << first->eType << "," << first->eKeyType << ") ";
        first++;
        std::cout<< "\n=#=";
    }
    std::cout << std::endl;
}
#endif
