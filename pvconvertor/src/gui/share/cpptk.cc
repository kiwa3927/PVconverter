








#include "cpptk.h"
#include <iomanip>


using namespace Tk;
using namespace Tk::details;
using namespace boost;
using namespace std;





Expr Tk::bell() { return Expr("bell"); }

Expr Tk::bindtags(string const &name, string const &tags)
{
     string str("bindtags ");
     str += name;
     if (tags.empty())
     {
          return Expr(str);
     }
     else
     {
          str += " { ";  str += tags;   str += " }";
          return Expr(str);
     }
}

Expr Tk::button(string const &name)
{
     string str("button ");
     str += name;
     return Expr(str);
}

Expr Tk::canvas(string const &name)
{
     string str("canvas ");
     str += name;
     return Expr(str);
}

Expr Tk::clipboard(string const &option)
{
     string str("clipboard ");
     str += option;
     return Expr(str);
}

Expr Tk::clipboard(string const &option, string const &data)
{
     string str("clipboard ");
     str += option;
     string postfix(" -- \"");
     postfix += quote(data);
     postfix += "\"";
     return Expr(str, postfix);
}

Expr Tk::destroy(string const &name)
{
     string str("destroy ");
     str += name;
     return Expr(str);
}

Expr Tk::entry(string const &name)
{
     string str("entry ");
     str += name;
     return Expr(str);
}

Expr Tk::fonts(string const &option, string const &name)
{
     string str("font ");
     str += option;
     if (name.empty())
     {
          return Expr(str);
     }
     else
     {
          str += " ";
          str += name;
          return Expr(str);
     }
}

Expr Tk::grab(string const &option, string const &name)
{
     string str("grab ");
     str += option;
     if (name.empty())
     {
          return Expr(str);
     }
     else
     {
          str += " ";
          str += name;
          return Expr(str);
     }
}

Expr Tk::images(string const &option, string const &tn, string const &name)
{
     string str("image ");
     str += option;
     if (tn.empty())
     {
          return Expr(str);
     }
     str += " "; str += tn;
     if (name.empty())
     {
          return Expr(str);
     }
     str += " ";
     if (option == "cget")
     {
          str += '-';
     }
     str += name;
     return Expr(str);
}

Expr Tk::label(string const &name)
{
     string str("label ");
     str += name;
     return Expr(str);
}

Expr Tk::labelframe(string const &name)
{
     string str("labelframe ");
     str += name;
     return Expr(str);
}

Expr Tk::listbox(string const &name)
{
     string str("listbox ");
     str += name;
     return Expr(str);
}

Expr Tk::menu(string const &name)
{
     string str("menu ");
     str += name;
     return Expr(str);
}

Expr Tk::menubutton(string const &name)
{
     string str("menubutton ");
     str += name;
     return Expr(str);
}

Expr Tk::message(string const &name)
{
     string str("message ");
     str += name;
     return Expr(str);
}

Expr Tk::option(string const &todo, string const &s1,
     string const &s2, string const &s3)
{
     string str("option ");
     str += todo;
     if (s1.empty())
     {
          return Expr(str);
     }
     str += " \""; str += s1; str += '\"';
     if (s2.empty())
     {
          return Expr(str);
     }
     str += " \""; str += s2; str += '\"';
     if (s3.empty())
     {
          return Expr(str);
     }
     str += " "; str += s3;
     return Expr(str);
}

Expr Tk::pack(string const &w1,
     string const &w2,
     string const &w3,
     string const &w4,
     string const &w5,
     string const &w6,
     string const &w7,
     string const &w8,
     string const &w9,
     string const &w10)
{
     string str("pack ");
     str += w1;
     if (!w2.empty()) { str += " "; str += w2; }
     if (!w3.empty()) { str += " "; str += w3; }
     if (!w4.empty()) { str += " "; str += w4; }
     if (!w5.empty()) { str += " "; str += w5; }
     if (!w6.empty()) { str += " "; str += w6; }
     if (!w7.empty()) { str += " "; str += w7; }
     if (!w8.empty()) { str += " "; str += w8; }
     if (!w9.empty()) { str += " "; str += w9; }
     if (!w10.empty()) { str += " "; str += w10; }
     
     return Expr(str);
}

Expr Tk::panedwindow(string const &name)
{
     string str("panedwindow ");
     str += name;
     return Expr(str);
}

Expr Tk::scale(string const &name)
{
     string str("scale ");
     str += name;
     return Expr(str);
}

Expr Tk::scrollbar(string const &name)
{
     string str("scrollbar ");
     str += name;
     return Expr(str);
}

Expr Tk::spinbox(string const &name)
{
     string str("spinbox ");
     str += name;
     return Expr(str);
}

Expr Tk::textw(string const &name)
{
     string str("text ");
     str += name;
     return Expr(str);
}

Expr Tk::tk_chooseColor()
{
     return Expr("tk_chooseColor");
}

Expr Tk::tk_chooseDirectory()
{
     return Expr("tk_chooseDirectory");
}

Expr Tk::tk_dialog(string const &window, string const &title,
     string const &text, string const &bitmap, string const &def,
     string const &but1, string const &but2, string const &but3,
     string const &but4)
{
     string str("tk_dialog ");
     str += window; str += " \"";
     str += title; str += "\" \"";
     str += quote(text); str += "\" ";
     str += bitmap; str += " \"";
     str += def; str += "\" \"";
     str += but1; str += "\"";
     if (but2.empty())
     {
          return Expr(str);
     }
     str += " \"";
     str += but2; str += "\"";
     if (but3.empty())
     {
          return Expr(str);
     }
     str += " \"";
     str += but3; str += "\"";
     if (but4.empty())
     {
          return Expr(str);
     }
     str += " \"";
     str += but4; str += "\"";
     return Expr(str);
}

Expr Tk::tk_focusNext(string const &window)
{
     string str("tk_focusNext ");
     str += window;
     return Expr(str);
}

Expr Tk::tk_focusPrev(string const &window)
{
     string str("tk_focusPrev ");
     str += window;
     return Expr(str);
}

Expr Tk::tk_getOpenFile()
{
     return Expr("tk_getOpenFile");
}

Expr Tk::tk_getSaveFile()
{
     return Expr("tk_getSaveFile");
}

Expr Tk::tk_menuSetFocus(string const &menu)
{
     string str("tk_menuSetFocus ");
     str += menu;
     return Expr(str);
}

Expr Tk::tk_messageBox()
{
     return Expr("tk_messageBox");
}

Expr Tk::tk_setPalette(string const &color)
{
     string str("tk_setPalette ");
     str += color;
     return Expr(str);
}

Expr Tk::tk_textCopy(string const &w)
{
     string str("tk_textCopy ");
     str += w;
     return Expr(str);
}

Expr Tk::tk_textCut(string const &w)
{
     string str("tk_textCut ");
     str += w;
     return Expr(str);
}

Expr Tk::tk_textPaste(string const &w)
{
     string str("tk_textPaste ");
     str += w;
     return Expr(str);
}

Expr Tk::tkwait(string const &option, string const &w)
{
     string str("tkwait ");
     str += option; str += " ";
     str += w;
     return Expr(str);
}

Expr Tk::winfo(string const &option, string const &w)
{
     string str("winfo ");
     str += option;
     string postfix(" ");
     postfix += w;
     return Expr(str, postfix);
}

Expr Tk::wm(string const &option, string const &w)
{
     string str("wm ");
     str += option; str += " ";
     str += w;
     return Expr(str);
}

Expr Tk::wmprotocol(string const &w, string const &proto)
{
     string str("wm protocol ");
     str += w;
     if (proto.empty())
     {
          return Expr(str);
     }
     str += " ";
     str += proto;
     return Expr(str);
}



Expr Tk::addtag(string const &tag, string const &spec)
{
     string str("addtag ");
     str += tag;    str += " ";
     str += spec;
     return Expr(str);
}

Expr Tk::blank()
{
     return Expr("blank");
}

Expr Tk::clone(string const &newpath, string const &type)
{
     string str("clone ");
     str += newpath;
     if (type.empty())
     {
          return Expr(str);
     }
     str += " ";    str += type;
     return Expr(str);
}

Expr Tk::compare(string const &indx1, string const &oper, string const &indx2)
{
     string str("compare ");
     str += indx1; str += " ";
     str += oper; str += " ";
     str += indx2;
     return Expr(str);
}

Expr Tk::coords()
{
     return Expr("coords");
}

Expr Tk::coords(string const &item, int x, int y)
{
     string str("coords ");
     str += item;   str += " ";
     str += toString(x); str += " ";
     str += toString(y);
     return Expr(str);
}

Expr Tk::coords(string const &item, Point const &p)
{
     return coords(item, p.x, p.y);
}

Expr Tk::coords(string const &item, int x1, int y1, int x2, int y2)
{
     string str("coords ");
     str += item;   str += " ";
     str += toString(x1);     str += " ";
     str += toString(y1);     str += " ";
     str += toString(x2);     str += " ";
     str += toString(y2);
     return Expr(str);
}

Expr Tk::coords(string const &item, Point const &p1, Point const &p2)
{
     return coords(item, p1.x, p1.y, p2.x, p2.y);
}

Expr Tk::coords(string const &item, Box const &b)
{
     return coords(item, b.x1, b.y1, b.x2, b.y2);
}

Expr Tk::copy(string const &photo)
{
     string str("copy ");
     str += photo;
     return Expr(str);
}

Expr Tk::curselection()
{
     return Expr("curselection");
}

Expr Tk::debug()
{
     return Expr("debug");
}

Expr Tk::debug(bool d)
{
     string str("debug ");
     str += toString(d);
     return Expr(str);
}

Expr Tk::deselect()
{
     return Expr("deselect");
}

Expr Tk::dlineinfo(string const &indx)
{
     string str("dlineinfo ");
     str += indx;
     return Expr(str);
}

Expr Tk::dtag(string const &tag, string const &todel)
{
     string str("dtag ");
     str += tag;
     if (todel.empty())
     {
          return Expr(str);
     }
     else
     {
          str += " ";    str += todel;
          return Expr(str);
     }
}

Expr Tk::dump(string const &indx1, string const &indx2)
{
     string str("dump");
     string postfix(" ");
     postfix += indx1;
     if (indx2.empty() == false)
     {
          postfix += " ";
          postfix += indx2;
     }
     return Expr("dump", postfix);
}

Expr Tk::edit(string const &option)
{
     string str("edit ");
     str += option;
     return Expr(str);
}

Expr Tk::find(string const &spec)
{
     string str("find ");
     str += spec;
     return Expr(str);
}

Expr Tk::flash()
{
     return Expr("flash");
}

Expr Tk::getsize()
{
     return Expr("size");
}

Expr Tk::gettags(string const &item)
{
     string str("gettags ");
     str += item;
     return Expr(str);
}

Expr Tk::insert(string const &indx, string const &txt, string const &tag)
{
     string str("insert ");
     str += indx; str += " \"";
     str += quote(txt); str += "\"";
     if (tag.empty())
     {
          return Expr(str);
     }
     str += " ";
     str += tag;
     return Expr(str);
}

Expr Tk::invoke()
{
     return Expr("invoke");
}

Expr Tk::move(string const &item, int x, int y)
{
     string str("move ");
     str += item;   str += " ";
     str += toString(x); str += " ";
     str += toString(y);
     return Expr(str);
}

Expr Tk::panecget(string const &w, string const &option)
{
     string str("panecget ");
     str += w; str += " -";
     str += option;
     return Expr(str);
}

Expr Tk::paneconfigure(string const &w)
{
     string str("paneconfigure ");
     str += w;
     return Expr(str);
}

Expr Tk::panes()
{
     return Expr("panes");
}

Expr Tk::postscript()
{
     return Expr("postscript");
}

Expr Tk::proxy(string const &option)
{
     string str("proxy ");
     str += option;
     return Expr(str);
}

Expr Tk::put(string const &color)
{
     string str("put ");
     str += color;
     return Expr(str);
}

Expr Tk::read(string const &file)
{
     string str("read \"");
     str += file; str += '\"';
     return Expr(str);
}

Expr Tk::redither()
{
     return Expr("redither");
}

Expr Tk::sash(string const &option, int index)
{
     string str("sash ");
     str += option; str += " ";
     str += toString(index);
     return Expr(str);
}

Expr Tk::search(string const &pattern,
     string const &indx1, string const &indx2)
{
     string str("search \"");
     str += quote(pattern); str += '\"';
     string postfix(" -- ");
     postfix += indx1;
     if (indx2.empty())
     {
          return Expr(str, postfix);
     }
     postfix += " ";
     postfix += indx2;
     return Expr(str, postfix);
}

Expr Tk::select()
{
     return Expr("select");
}

Expr Tk::select(string const &option)
{
     string str("select ");
     str += option;
     return Expr(str);
}

Expr Tk::selection(string const &option)
{
     string str("selection ");
     str += option;
     return Expr(str);
}

Expr Tk::tag(string const &option, string const &tagname)
{
     string str("tag ");
     str += option;
     if (tagname.empty())
     {
          return Expr(str);
     }
     str += " ";
     str += tagname;
     return Expr(str);
}

Expr Tk::tag(string const &option, string const &tagname,
     string const &indx1, string const &indx2)
{
     string str("tag ");
     str += option; str += " ";
     str += tagname; str += " ";
     if (option == "cget")
     {
          str += '-';
          str += indx1;
          return Expr(str);
     }
     str += indx1;
     if (indx2.empty())
     {
          return Expr(str);
     }
     str += " ";
     str += indx2;
     return Expr(str);
}

Expr Tk::tag(string const &option, string const &tagname,
     string const &indx1, char const *indx2)
{
     return Tk::tag(option, tagname, indx1, string(indx2));
}

Expr Tk::toggle()
{
     return Expr("toggle");
}

Expr Tk::transparency(string const &option, int x, int y)
{
     string str("transparency ");
     str += option; str += " ";
     str += toString(x); str += " ";
     str += toString(y);
     return Expr(str);
}

Expr Tk::transparency(string const &option, int x, int y, bool tr)
{
     string str("transparency ");
     str += option; str += " ";
     str += toString(x); str += " ";
     str += toString(y); str += " ";
     str += toString(tr);
     return Expr(str);
}

Expr Tk::windows(string const &option, string const &indx, string const &name)
{
     string str("window ");
     str += option;
     if (indx.empty())
     {
          return Expr(str);
     }
     str += " ";
     str += indx;
     if (name.empty())
     {
          return Expr(str);
     }
     str += " ";
     str += name;
     return Expr(str);
}

Expr Tk::write(string const &file)
{
     string str("write \"");
     str += file; str += '\"';
     return Expr(str);
}

Expr Tk::xview()
{
     return Expr("xview");
}

Expr Tk::xview(string const &option, double fraction)
{
     string str("xview ");
     str += option; str += " ";
     str += toString(fraction);
     return Expr(str);
}

Expr Tk::xview(string const option, int number, string const &what)
{
     string str("xview ");
     str += option; str += " ";
     str += toString(number); str += " ";
     str += what;
     return Expr(str);
}

Expr Tk::yview()
{
     return Expr("yview");
}

Expr Tk::yview(string const &option, double fraction)
{
     string str("yview ");
     str += option; str += " ";
     str += toString(fraction);
     return Expr(str);
}

Expr Tk::yview(string const option, int number, string const &what)
{
     string str("yview ");
     str += option; str += " ";
     str += toString(number); str += " ";
     str += what;
     return Expr(str);
}



#define CPPTK_OPTION(name, quote) Option Tk::name(#name, quote);
#include "cpptkoptions.x"
#undef CPPTK_OPTION



Expr Tk::backwards()
{
     return Expr(" -backwards", false);
}

Expr Tk::cliptype(string const &type)
{
     string str(" -type ");
     str += type;
     return Expr(str, false);
}

Expr Tk::count(int &i)
{
     std::string str(" -count ");
     str += details::addLinkVar(i);
     return details::Expr(str, false);
}

Expr Tk::count(string const &name)
{
     string str(" -count ");
     str += name;
     return Expr(str, false);
}

Expr Tk::defaultbutton(string const &but)
{
     string str(" -default \"");
     str += but; str += '\"';
     return Expr(str, false);
}

Expr Tk::defaultstate(string const &name)
{
     string str(" -default ");
     str += name;
     return Expr(str, false);
}

Expr Tk::exact()
{
     return Expr(" -exact", false);
}

Expr Tk::filetypes(string const &types)
{
     string str(" -filetypes {");
     str += types;
     str += "}";
     return Expr(str, false);
}

Expr Tk::forwards()
{
     return Expr(" -forwards", false);
}

Expr Tk::grayscale()
{
     return Expr(" -grayscale", false);
}

Expr Tk::invalidcommand(char const *name)
{
     string str(" -invalidcommand { ");
     str += name; str += " }";
     return Expr(str, false);
}

Expr Tk::invalidcommand(string const &name)
{
     string str(" -invalidcommand { ");
     str += name; str += " }";
     return Expr(str, false);
}

Expr Tk::invalidcommand(CallbackHandle const &handle)
{
     string str(" -invalidcommand { ");
     str += handle.get(); str += " }";
     return Expr(str, false);
}

Expr Tk::menutype(string const &type)
{
     string str(" -type ");
     str += type;
     return Expr(str, false);
}

Expr Tk::messagetext(string const &txt)
{
     string str(" -message \"");
     str += txt; str += '\"';
     return Expr(str, false);
}

Expr Tk::messagetype(string const &type)
{
     string str(" -type ");
     str += type;
     return Expr(str, false);
}

Expr Tk::multiple()
{
     return Expr(" -multiple", false);
}

Expr Tk::nocase()
{
     return Expr(" -nocase", false);
}

Expr Tk::postcommand(string const &name)
{
     string str(" -postcommand { ");
     str += name; str += " }";
     return Expr(str, false);
}

Expr Tk::postcommand(CallbackHandle const &handle)
{
     string str(" -postcommand { ");
     str += handle.get(); str += " }";
     return Expr(str, false);
}

Expr Tk::regexp()
{
     return Expr(" -regexp", false);
}

Expr Tk::shrink()
{
     return Expr(" -shrink", false);
}

Expr Tk::submenu(string const &menu)
{
     string str(" -menu ");
     str += menu;
     return Expr(str, false);
}

Expr Tk::subsample(int x, int y)
{
     string str(" -subsample ");
     str += toString(x); str += " ";
     str += toString(y);
     return Expr(str, false);
}

Expr Tk::tags()
{
     return Expr(" -tag", false);
}

Expr Tk::tearoffcommand(string const &name)
{
     string str(" -tearoffcommand { ");
     str += name; str += " }";
     return Expr(str, false);
}

Expr Tk::tearoffcommand(CallbackHandle const &handle)
{
     string str(" -tearoffcommand { ");
     str += handle.get(); str += " }";
     return Expr(str, false);
}

Expr Tk::textvariable(string const &name)
{
     string str(" -textvariable ");
     str += name;
     return Expr(str, false);
}

Expr Tk::variable(string const &name)
{
     string str(" -variable ");
     str += name;
     return Expr(str, false);
}

Expr Tk::zoom(double x, double y)
{
     string str(" -zoom ");
     str += toString(x); str += " ";
     str += toString(y);
     return Expr(str, false);
}



EventAttr<string> Tk::event_A("%A"); 
EventAttr<int>    Tk::event_b("%b"); 
EventAttr<int>    Tk::event_D("%D"); 
EventAttr<int>    Tk::event_f("%f"); 
EventAttr<int>    Tk::event_h("%h"); 
EventAttr<string> Tk::event_k("%k"); 
EventAttr<string> Tk::event_K("%K"); 
EventAttr<string> Tk::event_m("%m"); 
EventAttr<int>    Tk::event_N("%N"); 
EventAttr<string> Tk::event_s("%s"); 
EventAttr<string> Tk::event_T("%T"); 
EventAttr<int>    Tk::event_w("%w"); 
EventAttr<string> Tk::event_W("%W"); 
EventAttr<int>    Tk::event_x("%x"); 
EventAttr<int>    Tk::event_X("%X"); 
EventAttr<int>    Tk::event_y("%y"); 
EventAttr<int>    Tk::event_Y("%Y"); 

ValidateAttr<int>    Tk::valid_d("%d"); 
ValidateAttr<int>    Tk::valid_i("%i"); 
ValidateAttr<string> Tk::valid_P("%P"); 
ValidateAttr<string> Tk::valid_s("%s"); 
ValidateAttr<string> Tk::valid_S("%S"); 
ValidateAttr<string> Tk::valid_v("%v"); 
ValidateAttr<string> Tk::valid_V("%V"); 
ValidateAttr<string> Tk::valid_W("%W"); 




#define CPPTK_CONSTANT(c) char const * Tk::c = #c;
#include "cpptkconstants.x"
#undef CPPTK_CONSTANT



char const * Tk::deletefont = "delete"; 
char const * Tk::deleteimg  = "delete"; 
char const * Tk::deletetag  = "delete"; 
char const * Tk::setglobal  = "set -global";
char const * Tk::wrapchar   = "char";   
char const * Tk::wrapword   = "word";   



Expr Tk::afteridle(string const &cmd)
{
     string str("after idle { ");
     str += cmd; str += " }";
     return Expr(str);
}

Expr Tk::update(string const &option)
{
     string str("update");
     if (option.empty())
     {
          return Expr(str);
     }
     str += " ";
     str += option;
     return Expr(str);
}



Tk::details::BindToken::BindToken() : BasicToken("bind") {}

Expr Tk::details::BindToken::operator()(string const &name,
     string const &seq) const
{
     string str("bind ");
     str += name;   str += " ";
     str += seq;    str += " {}";
     return Expr(str);
}

BindToken Tk::bind;

Tk::details::CheckButtonToken::CheckButtonToken()
     : BasicToken("checkbutton") {}

Expr Tk::details::CheckButtonToken::operator()(string const &name) const
{
     string str("checkbutton ");
     str += name;
     return Expr(str);
}

CheckButtonToken Tk::checkbutton;

Tk::details::FrameToken::FrameToken() : BasicToken("frame") {}

Expr Tk::details::FrameToken::operator()(string const &name) const
{
     string str("frame ");
     str += name;
     return Expr(str);
}

FrameToken Tk::frame;

Tk::details::GridToken::GridToken() : BasicToken("grid") {}

Expr Tk::details::GridToken::operator()(string const &option,
     string const &name) const
{
     string str("grid ");
     str += option; str += " ";
     str += name;
     return Expr(str);
}

Expr Tk::details::GridToken::operator()(string const &option,
     string const &name, int x, int y) const
{
     string str("grid ");
     str += option; str += " ";
     str += name; str += " ";
     str += toString(x); str += " ";
     str += toString(y);
     return Expr(str);
}

Expr Tk::details::GridToken::operator()(string const &option,
     string const &name, int col1, int row1, int col2, int row2) const
{
     string str("grid ");
     str += option; str += " ";
     str += name; str += " ";
     str += toString(col1); str += " ";
     str += toString(row1); str += " ";
     str += toString(col2); str += " ";
     str += toString(row2);
     return Expr(str);
}

GridToken Tk::grid;

Tk::details::LowerToken::LowerToken() : BasicToken("lower") {}

Expr Tk::details::LowerToken::operator()(string const &name,
     string const &belowthis) const
{
     string str("lower ");
     str += name;
     if (belowthis.empty())
     {
          return Expr(str);
     }
     str += " "; str += belowthis;
     return Expr(str);    
}

LowerToken Tk::lower;

Tk::details::PlaceToken::PlaceToken() : BasicToken("place") {}

Expr Tk::details::PlaceToken::operator()(string const &w) const
{
     string str("place ");
     str += w;
     return Expr(str);
}

Expr Tk::details::PlaceToken::operator()(string const &option,
     string const &w) const
{
     string str("place ");
     str += option; str += " ";
     str += w;
     return Expr(str);
}

PlaceToken Tk::place;

Tk::details::RadioButtonToken::RadioButtonToken()
     : BasicToken("radiobutton") {}

Expr Tk::details::RadioButtonToken::operator()(string const &name) const
{
     string str("radiobutton ");
     str += name;
     return Expr(str);
}

RadioButtonToken Tk::radiobutton;

Tk::details::RaiseToken::RaiseToken() : BasicToken("raise") {}

Expr Tk::details::RaiseToken::operator()(string const &name,
     string const &abovethis) const
{
     string str("raise ");
     str += name;
     if (abovethis.empty())
     {
          return Expr(str);
     }
     str += " "; str += abovethis;
     return Expr(str);    
}

RaiseToken Tk::raise;

Tk::details::ToplevelToken::ToplevelToken() : BasicToken("toplevel") {}

Expr Tk::details::ToplevelToken::operator()(string const &w) const
{
     string str("toplevel ");
     str += w;
     return Expr(str);
}

ToplevelToken Tk::toplevel;

Tk::details::AddToken::AddToken() : BasicToken("add") {}

Expr Tk::details::AddToken::operator()(string const &tn) const
{
     string str("add ");
     str += tn;
     return Expr(str);
}

AddToken Tk::add;

Tk::details::BboxToken::BboxToken() : BasicToken("bbox") {}

BboxToken Tk::bbox;

Tk::details::CgetToken::CgetToken() : BasicToken("cget") {}

Expr Tk::details::CgetToken::operator()(string const &name) const
{
     string str("cget -");
     str += name;
     return Expr(str);
}

CgetToken Tk::cget;

Tk::details::ConfigureToken::ConfigureToken() : BasicToken("configure") {}

Expr Tk::details::ConfigureToken::operator()() const
{
     return Expr("configure");
}

ConfigureToken Tk::configure;

Tk::details::CreateToken::CreateToken() : BasicToken("create") {}

Expr Tk::details::CreateToken::operator()(string const &type,
     int x, int y) const
{
     string str("create ");
     str += type;   str += " ";
     str += toString(x); str += " ";
     str += toString(y);
     return Expr(str);
}

Expr Tk::details::CreateToken::operator()(string const &type,
     Point const &p) const
{
     return create(type, p.x, p.y);
}

Expr Tk::details::CreateToken::operator()(string const &type,
     int x1, int y1, int x2, int y2) const
{
     string str("create ");
     str += type;   str += " ";
     str += toString(x1); str += " ";
     str += toString(y1); str += " ";
     str += toString(x2); str += " ";
     str += toString(y2);
     return Expr(str);
}

Expr Tk::details::CreateToken::operator()(string const &type,
     Point const &p1, Point const &p2) const
{
     return create(type, p1.x, p1.y, p2.x, p2.y);
}

Expr Tk::details::CreateToken::operator()(string const &type,
     Box const &b) const
{
     return create(type, b.x1, b.y1, b.x2, b.y2);
}

CreateToken Tk::create;

Tk::details::FocusToken::FocusToken() : BasicToken("focus") {}

Expr Tk::details::FocusToken::operator()(string const &name) const
{
     string str("focus");
     if (name.empty())
     {
          return Expr(str);
     }
     else
     {
          str += " ";    str += name;
          return Expr(str);
     }
}

FocusToken Tk::focus;

Tk::details::ForgetToken::ForgetToken() : BasicToken("forget") {}

Expr Tk::details::ForgetToken::operator()(string const &name) const
{
     string str("forget ");
     str += name;
     return Expr(str);
}

ForgetToken Tk::forget;

Tk::details::GetToken::GetToken() : BasicToken("get") {}

Expr Tk::details::GetToken::operator()() const
{
     return Expr("get");
}

GetToken Tk::get;

Tk::details::MoveToToken::MoveToToken() : BasicToken("moveto") {}

Expr Tk::details::MoveToToken::operator()(double fraction) const
{
     string str("moveto ");
     str += toString(fraction);
     return Expr(str);
}

MoveToToken Tk::moveto;

Tk::details::ScrollToken::ScrollToken() : BasicToken("scroll") {}

Expr Tk::details::ScrollToken::operator()(int n, string const &what) const
{
     string str("scroll ");
     str += toString(n); str += " ";
     str += what;
     return Expr(str);
}

ScrollToken Tk::scroll;

Tk::details::SetToken::SetToken() : BasicToken("set") {}

Expr Tk::details::SetToken::operator()() const
{
     return Expr("set");
}

Expr Tk::details::SetToken::operator()(double first, double last) const
{
     string str("set ");
     str += toString(first); str += " ";
     str += toString(last);
     return Expr(str);
}

SetToken Tk::set;

Tk::details::TypeToken::TypeToken() : BasicToken("type") {}

TypeToken Tk::type;

Tk::details::ValidateToken::ValidateToken() : BasicToken("validate") {}

Expr Tk::details::ValidateToken::operator()() const
{
     return Expr("validate");
}

Expr Tk::details::ValidateToken::operator()(string const &when) const
{
     string str(" -validate ");
     str += when;
     return Expr(str, false);
}

ValidateToken Tk::validate;

Tk::details::AllToken::AllToken() : BasicToken("all") {}

Expr Tk::details::AllToken::operator()() const
{
     return Expr(" -all", false);
}

AllToken Tk::all;

Tk::details::CommandToken::CommandToken() : BasicToken("command") {}

Expr Tk::details::CommandToken::operator()(string const &name) const
{
     string str(" -command { ");
     str += name; str += " }";
     return Expr(str, false);
}

Expr Tk::details::CommandToken::operator()(CallbackHandle const &handle) const
{
     string str(" -command { ");
     str += handle.get(); str += " }";
     return Expr(str, false);
}

CommandToken Tk::command;

Tk::details::ElideToken::ElideToken() : BasicToken("elide") {}

Expr Tk::details::ElideToken::operator()() const
{
     return Expr(" -elide", false);
}

Expr Tk::details::ElideToken::operator()(bool b) const
{
     string str(" -elide ");
     str += toString(b);
     return Expr(str, false);
}

ElideToken Tk::elide;

Tk::details::FromToken::FromToken() : BasicToken("from") {}

Expr Tk::details::FromToken::operator()(int val) const
{
     string str(" -from ");
     str += toString(val);
     return Expr(str, false);
}

Expr Tk::details::FromToken::operator()(int x1, int y1, int x2, int y2) const
{
     string str(" -from ");
     str += toString(x1); str += " ";
     str += toString(y1); str += " ";
     str += toString(x2); str += " ";
     str += toString(y2);
     return Expr(str, false);
}

FromToken Tk::from;

Tk::details::ImageToken::ImageToken() : BasicToken("image") {}

Expr Tk::details::ImageToken::operator()() const
{
     return Expr(" -image", false);
}

Expr Tk::details::ImageToken::operator()(string const &name) const
{
     string str(" -image ");
     str += name;
     return Expr(str, false);
}

ImageToken Tk::image;

Tk::details::MarkToken::MarkToken() : BasicToken("mark") {}

Expr Tk::details::MarkToken::operator()() const
{
     return Expr(" -mark", false);
}

Expr Tk::details::MarkToken::operator()(string const &option,
     string const &markname, string const &dir) const
{
     string str("mark ");
     str += option;
     if (markname.empty())
     {
          return Expr(str);
     }
     str += " ";
     str += markname;
     if (dir.empty())
     {
          return Expr(str);
     }
     str += " ";
     str += dir;
     return Expr(str);
}

MarkToken Tk::mark;

Tk::details::MenuLabelToken::MenuLabelToken() : BasicToken("label") {}

Expr Tk::details::MenuLabelToken::operator()(string const &label) const
{
     string str(" -label \"");
     str += label; str += '\"';
     return Expr(str, false);
}

MenuLabelToken Tk::menulabel;

Tk::details::TextToken::TextToken() : BasicToken("text") {}

Expr Tk::details::TextToken::operator()() const
{
     return Expr(" -text", false);
}

Expr Tk::details::TextToken::operator()(string const &t) const
{
     string str(" -text \"");
     str += quote(t); str += '\"';
     return Expr(str, false);
}

TextToken Tk::text;

Tk::details::ToToken::ToToken() : BasicToken("to") {}

Expr Tk::details::ToToken::operator()(int val) const
{
     string str(" -to ");
     str += toString(val);
     return Expr(str, false);
}

Expr Tk::details::ToToken::operator()(int x, int y) const
{
     string str(" -to ");
     str += toString(x); str += " ";
     str += toString(y);
     return Expr(str, false);
}

Expr Tk::details::ToToken::operator()(int x1, int y1, int x2, int y2) const
{
     string str(" -to ");
     str += toString(x1); str += " ";
     str += toString(y1); str += " ";
     str += toString(x2); str += " ";
     str += toString(y2);
     return Expr(str, false);
}

ToToken Tk::to;

Tk::details::WindowToken::WindowToken() : BasicToken("window") {}

Expr Tk::details::WindowToken::operator()(string const &name) const
{
     string str(" -window");
     if (name.empty())
     {
          return Expr(str, false);
     }
     str += " "; str += name;
     return Expr(str, false);
}

WindowToken Tk::window;

Tk::details::WndClassToken::WndClassToken() : BasicToken("class") {}

Expr Tk::details::WndClassToken::operator()(string const &name) const
{
     string str(" -class ");
     str += name;
     return Expr(str, false);
}

WndClassToken Tk::wndclass;

Tk::details::AfterToken::AfterToken() : BasicToken("after") {}

Expr Tk::details::AfterToken::operator()(int time) const
{
     string str("after ");
     str += toString(time);
     return Expr(str);
}

Expr Tk::details::AfterToken::operator()(string const &name) const
{
     string str(" -after ");
     str += name;
     return Expr(str, false);
}

Expr Tk::details::AfterToken::operator()(int time, string const &name) const
{
     string str("after ");
     str += toString(time); str += " ";
     str += name;
     return Expr(str);
}

Expr Tk::details::AfterToken::operator()(string const &option,
     string const &id) const
{
     string str("after ");
     str += option; str += " ";
     str += id;
     return Expr(str);
}

AfterToken Tk::after;

Tk::details::RGBToken::RGBToken() : BasicToken("rgb") {}

string Tk::details::RGBToken::operator()(int r, int g, int b) const
{
     if (r < 0)   r = 0;
     if (r > 255) r = 255;
     if (g < 0)   g = 0;
     if (g > 255) g = 255;
     if (b < 0)   b = 0;
     if (b > 255) b = 255;
     ostringstream ss;
     ss << '#' << setw(2) << setfill('0') << hex << r
           << setw(2) << setfill('0') << hex << g
           << setw(2) << setfill('0') << hex << b;
     return ss.str();
}

RGBToken Tk::rgb;

