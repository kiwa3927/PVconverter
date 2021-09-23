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
#include "lexical/rcsLexParser.h"
#include "lexical/rcsMacroParser.h"
#include "syntax/rcsSynConvertor.h"
#include "tvf/rcsTvfCompiler.h"
#include "rcsSvrf2Pvrs.h"
#include "gui/convertorWin.h"

void PrintLicense()
{
	std::string strLicense = "\n\
************************************************************************************** \n\
***                              SVRF2PVRS LICENSE                                **** \n\
************************************************************************************** \n\
\n\
Redistribution and use in source and binary forms, with or without \n\
modification, are permitted provided that the following conditions are met: \n\
\n\
Redistributions of source code must retain the above copyright notice, this \n\
list of conditions and the following disclaimer. \n\
\n\
Redistributions in binary form must reproduce the above copyright notice, \n\
this list of conditions and the following disclaimer in the documentation \n\
and/or other materials provided with the distribution. \n\
\n\
Neither the name of the copyright holder nor the names of its \n\
contributors may be used to endorse or promote products derived from \n\
this software without specific prior written permission. \n\
\n\
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" \n\
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE \n\
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE \n\
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE \n\
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR \n\
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF \n\
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS \n\
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN \n\
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) \n\
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE \n\
POSSIBILITY OF SUCH DAMAGE. \n\
\n\
************************************************************************************** \n\
";
	std::cout << strLicense << endl;
}

void Usage(const char* exeName)
{
    std::cout << "Usage:\n\t" << exeName << " -i svrf_rule_file [-sf switch_file] [-full] [-oc] -o[l] pvrs_rule_file [-kt] [-trs]" << std::endl
    		  << "\t" << exeName << " -gui [-i svrf_rule_file -o[l] pvrs_rule_file]" << std::endl
    		  << "\t" << exeName << " -E preprocessed_file_from_rulefile rule_file_name \n"
			  << std::endl
			  << "Option:\n"
			  << "\t" << "[-i svrf_rule_file]" << std::endl
			  << "\t\t" <<" Specify the input SVRF ruledeck." << std::endl
			  << std::endl
			  << "\t" << "[-sf switch_file]" << std::endl
  			  << "\t\t" <<" Specify the input switches in this file. Switch file format:" << std::endl
  			  << "\t\t" <<" #define  METAL5" << std::endl
  			  << "\t\t" <<" #define  VOL  18" << std::endl
  			  << std::endl
  			  << "\t" << "[-full]" << std::endl
  			  << "\t\t" <<" This option instructs svrf2pvrs to convertor all switches in the ruledeck." << std::endl
  			  << "\t\t" <<" This option is not recommended because svrf2pvrs may fail to convertor the ruledeck " << std::endl
  			  << "\t\t" <<" for some situations which you can find in the readme.txt in the svrf2pvrs package." << std::endl
  			  << "\t\t" <<" If not use this option, svrf2pvrs only convert the active switches to pvrs ruledeck." << std::endl
  			  << std::endl
  			  << "\t" << "[-oc]" << std::endl
  			  << "\t\t" <<" Output the comments in the ruledeck to PVRS ruledeck." << std::endl
  			  << std::endl
  			  << "\t" << "-o[l] pvrs_rule_file" << std::endl
  			  << "\t\t" <<" Specify the output PVRS ruledeck." << std::endl
  			  << "\t\t" <<" If use the “-ol” option, svrf2pvrs will output the SVRF rules to the comments of PVRS ruledeck." << std::endl
  			  << std::endl
  			  << "\t" << "-gui" << std::endl
  			  << "\t\t" <<" Use GUI to setup and run svrf2pvrs." << std::endl
  			  << std::endl;

    exit(0);
}

int main(int argc,char* argv[])
{

    bool outSvrf  			= false;
    bool isGui    			= false;
    bool convertSwitch  	= false;
    bool flattenMacro   	= false;
    bool outComment     	= false;
    bool isNewPvrs      	= true;
    bool expandTmpLayer 	= true;
    bool convertIncludeFile = false;
    bool trsFlag 			= false;
    bool tvfPreprocessFlag 	= false;
    std::string sInFile;
    std::string sSwitchFile;
    std::string sOutFile;
    std::string sTmpLayerSuffix;

    PrintLicense();

    for(int i = 1; i < argc; ++i)
    {
        if(0 == strcasecmp(argv[i],"-i"))
        {
            if(!sInFile.empty() || ++i >= argc)
            {
                Usage(argv[0]);
            }

            sInFile = argv[i];
        }
        else if(0 == strcasecmp(argv[i], "-o"))
        {
            if(!sOutFile.empty() || ++i >= argc)
            {
                Usage(argv[0]);
            }

            sOutFile = argv[i];
        }
        else if(0 == strcasecmp(argv[i], "-tmp"))
        {
            if(!sTmpLayerSuffix.empty() || ++i >= argc)
            {
                Usage(argv[0]);
            }

            sTmpLayerSuffix = argv[i];
        }
        else if(0 == strcasecmp(argv[i], "-ol"))
        {
            if(!sOutFile.empty() || ++i >= argc)
            {
                Usage(argv[0]);
            }
            outSvrf  = true;
            sOutFile = argv[i];
        }
        else if(0 == strcasecmp(argv[i], "-oi"))
        {
        	if(!sOutFile.empty() || ++i >= argc)
			{
				Usage(argv[0]);
			}
        	convertIncludeFile = true;
        	convertSwitch = true;
        	sOutFile = argv[i];
        }
        else if(0 == strcasecmp(argv[i], "-full"))
        {
            if(convertSwitch)
            {
                Usage(argv[0]);
            }
            convertSwitch = true;

        }
        else if(0 == strcasecmp(argv[i], "-sf"))
        {
            if(!sSwitchFile.empty() || ++i >= argc || convertSwitch)
            {
                Usage(argv[0]);
            }

            sSwitchFile = argv[i];
        }








        else if(0 == strcasecmp(argv[i], "-oc"))
        {
            if(outComment)
            {
                Usage(argv[0]);
            }
            outComment = true;
        }
        else if(0 == strcasecmp(argv[i], "-gui"))
        {
            if(isGui)
            {
                Usage(argv[0]);
            }

            isGui = true;
        }
        else if(0 == strcasecmp(argv[i], "-new"))
        {
            if(!isNewPvrs)
            {
                Usage(argv[0]);
            }

            isNewPvrs = true;
        }
        else if(0 == strcasecmp(argv[i], "-old"))
        {
            isNewPvrs = false;
        }
        else if(0 == strcasecmp(argv[i], "-kt"))
        {
            expandTmpLayer = false;
        }
        else if(0 == strcasecmp(argv[i], "-trs"))
        {
        	trsFlag = true;
        }
        else if(0 == strcasecmp(argv[i], "-E"))
		{
        	i+=2;
            if(!sInFile.empty() || !sOutFile.empty() ||  i >= argc)
            {
                Usage(argv[0]);
            }

            sInFile = argv[i];
            sOutFile = argv[--i];
            tvfPreprocessFlag = true;
		}
    }

    if(false == isGui && (sInFile.empty() || sOutFile.empty()))
        Usage(argv[0]);

    rcsManager_T::getInstance()->setGui(isGui);

    if(tvfPreprocessFlag)
    {
        try
        {
            rcsSvrf2Pvrs convertor;
            convertor.executeTvfPreprocess(sInFile, sOutFile, std::cout);
        }
        catch(const bool &ExitFlag)
        {
            if(!ExitFlag)
                return -1;
        }



    }
    else if(!isGui)
    {
        try
        {
            rcsSvrf2Pvrs convertor;
            convertor.execute(sInFile, sSwitchFile, sOutFile, outSvrf, convertSwitch,
                              flattenMacro, outComment, std::cout, isNewPvrs, expandTmpLayer,
                              convertIncludeFile, trsFlag, sTmpLayerSuffix);
        }
        catch(const bool &ExitFlag)
        {
            if(!ExitFlag)
                return -1;
        }



    }
    else
    {
    	Tk::runGui(argv[0], sInFile.c_str(), (false == sOutFile.empty() ? sOutFile.c_str() : "_temp_rule.pvrs"), outSvrf);
    }

    return 0;
}
