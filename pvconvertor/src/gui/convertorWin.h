



#ifndef CONVERTORWIN_H_
#define CONVERTORWIN_H_

#include "share/cpptk.h"
#include <iostream>
#include <fstream>

#include "main/rcsSvrf2Pvrs.h"

using namespace std;

namespace Tk
{

string strSvrf;
string strSwitch;
string strPvrs;
string strIsAddComment;
int nIsConvertOneSwitch;
int nIsConvertAllSwitch;
string strIsOutputComment;

void onBrowseSvrfFile()
{
     string fileName(tk_getOpenFile());
     strSvrf = fileName;
}

void onBrowsePvrsFile()
{
     string fileName(tk_getSaveFile());
     strPvrs = fileName;
}

void onBrowseSwitchFile()
{
    string fileName(tk_getOpenFile());
    strSwitch = fileName;
}

void onConvertOneSwitch()
{
	nIsConvertAllSwitch = 0;
}

void onConvertAllSwitch()
{
	nIsConvertOneSwitch = 0;
}

void onConvert()
{
	
	if(strSvrf.empty())
	{
		".mes" << deletetext(txt(1,0), end);
		".mes" << insert(end, "Svrf rule file should be specified.\n");
		return;
	}
	if(strPvrs.empty())
	{
		".mes" << deletetext(txt(1,0), end);
		".mes" << insert(end, "Pvrs rule file should be specified.\n");
		return;
	}

	
    std::string sMsg;
    std::ostringstream oMsg(sMsg);
    s_errManager.clear();
    rcsManager_T::deleteInstance();
    rcsManager_T::getInstance()->setGui(true);

    try
    {
        rcsSvrf2Pvrs convertor;
        convertor.execute(strSvrf,
        				  (nIsConvertOneSwitch == 1 ? strSwitch : ""),
        				  strPvrs,
        				  (strcmp(strIsAddComment.c_str(), "1") == 0 ? true : false),
        				  (nIsConvertAllSwitch == 1 ? true : false),
        				  false,
        				  (strcmp(strIsOutputComment.c_str(), "1") == 0 ? true : false),
        				  oMsg,
        				  true);
    }
    catch(const bool &ExitFlag)
    {
        if(!ExitFlag)
            s_errManager.Report(oMsg);
    }

    ".mes" << deletetext(txt(1,0), end);
    ".mes" << insert(end, oMsg.str());

}

void runGui(char* pExeName, const char* pSvrfFile, const char* pPvrsFile,
			bool bAddComment)
{
    try
    {
         init(pExeName);
         wm("title .", pExeName);

         
         frame(".f_svrf");
         pack(".f_svrf") -Tk::fill(x) -side(top);
         label(".f_svrf.lab") -text("Svrf Rule:");
         entry(".f_svrf.ent") -textvariable(strSvrf);
         button(".f_svrf.but") -text("Browse") -command(onBrowseSvrfFile);
         pack(".f_svrf.lab", ".f_svrf.ent", ".f_svrf.but") -side(Tk::left);
         pack(".f_svrf.ent") -Tk::fill(x) -expand(true);
         strSvrf = pSvrfFile;

         frame(".f_pvrs");
         pack(".f_pvrs") -Tk::fill(x) -side(top);
         label(".f_pvrs.lab") -text("Pvrs Rule:");
         entry(".f_pvrs.ent") -textvariable(strPvrs);
         button(".f_pvrs.but") -text("Browse") -command(onBrowsePvrsFile);
         pack(".f_pvrs.lab", ".f_pvrs.ent", ".f_pvrs.but") -side(Tk::left);
         pack(".f_pvrs.ent") -Tk::fill(x) -expand(true);
         strPvrs = pPvrsFile;

         frame(".f_option1");
         pack(".f_option1") -Tk::fill(x) -side(top);
         checkbutton(".f_option1.cb_svrf_log") -text("Add the svrf contents in the pvrs rule as the comments.") -variable(strIsAddComment);
         pack(".f_option1.cb_svrf_log") -side(Tk::left);
         strIsAddComment = bAddComment ? "1" : "0";

         frame(".f_option2");
         pack(".f_option2") -Tk::fill(x) -side(top);
         radiobutton(".f_option2.rb_convert_switch") -text("Convert Current Switch. Switch File:") -variable(nIsConvertOneSwitch) -value(1) -command(onConvertOneSwitch);
         entry(".f_option2.ent_switch") -textvariable(strSwitch);
         button(".f_option2.but_switch") -text("Browse") -command(onBrowseSwitchFile);
         pack(".f_option2.rb_convert_switch", ".f_option2.ent_switch", ".f_option2.but_switch") -side(Tk::left);
         pack(".f_option2.ent_switch") -Tk::fill(x) -expand(true);
         nIsConvertOneSwitch = 1;
         strSwitch = "";

         frame(".f_option3");
         pack(".f_option3") -Tk::fill(x) -side(top);
         radiobutton(".f_option3.rb_convert_all_switch") -text("Convert All Switch.") -variable(nIsConvertAllSwitch) -value(1) -command(onConvertAllSwitch);
         pack(".f_option3.rb_convert_all_switch") -side(Tk::left);
         nIsConvertAllSwitch = 0;

         frame(".f_option4");
         pack(".f_option4") -Tk::fill(x) -side(top);
         checkbutton(".f_option4.cb_output_comment") -text("Convert with the Comments in the Svrf Rule.") -variable(strIsOutputComment);
         pack(".f_option4.cb_output_comment") -side(Tk::left);
         strIsOutputComment = "0";

         frame(".f_conv");
         pack(".f_conv") -Tk::fill(x) -side(top);
         button(".f_conv.but_convert") -text("Convert") -command(onConvert);
         pack(".f_conv.but_convert") -side(Tk::right);

         textw(".mes") -background("white") -yscrollcommand(".scr set");
         scrollbar(".scr") -command(string(".mes yview"));
         pack(".scr") -side(Tk::right) -Tk::fill(y);
         pack(".mes") -expand(true) -Tk::fill(both);

         runEventLoop();
    }
    catch (exception const &e)
    {
         cerr << "Error: " << e.what() << '\n';
    }

}

}

#endif 
