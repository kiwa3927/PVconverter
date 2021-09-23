








#ifndef CPPTK_H_INCLUDED
#define CPPTK_H_INCLUDED

#include "base/cpptkbase.h"


namespace Tk
{





details::Expr bell();

details::Expr bindtags(std::string const &name,
     std::string const &tags = std::string());     

details::Expr button(std::string const &name);

details::Expr canvas(std::string const &name);

details::Expr clipboard(std::string const &option);

details::Expr clipboard(std::string const &option, std::string const &data);

details::Expr destroy(std::string const &name);

details::Expr entry(std::string const &name);

details::Expr fonts(std::string const &option,
     std::string const &name = std::string());

details::Expr grab(std::string const &option,
     std::string const &name = std::string());

details::Expr images(std::string const &option,
     std::string const &tn = std::string(),
     std::string const &name = std::string());

details::Expr label(std::string const &name);

details::Expr labelframe(std::string const &name);

details::Expr listbox(std::string const &name);

details::Expr menu(std::string const &name);

details::Expr menubutton(std::string const &name);

details::Expr message(std::string const &name);

details::Expr option(std::string const &todo,
     std::string const &s1 = std::string(),
     std::string const &s2 = std::string(),
     std::string const &s3 = std::string());

details::Expr pack(std::string const &w1,
     std::string const &w2 = std::string(),
     std::string const &w3 = std::string(),
     std::string const &w4 = std::string(),
     std::string const &w5 = std::string(),
     std::string const &w6 = std::string(),
     std::string const &w7 = std::string(),
     std::string const &w8 = std::string(),
     std::string const &w9 = std::string(),
     std::string const &w10 = std::string());

template <typename T>
details::Expr pack(std::string const &option,
     std::string const &w, T const &t)
{
     std::string str("pack ");
     str += option; str += " ";
     str += w; str += " ";
     str += details::toString(t);
     return details::Expr(str);
}

details::Expr panedwindow(std::string const &name);

details::Expr scale(std::string const &name);

details::Expr scrollbar(std::string const &name);

details::Expr spinbox(std::string const &name);

details::Expr textw(std::string const &name);

details::Expr tk_chooseColor();

details::Expr tk_chooseDirectory();

details::Expr tk_dialog(std::string const &window, std::string const &title,
     std::string const &text, std::string const &bitmap,
     std::string const &def, std::string const &but1,
     std::string const &but2 = std::string(),
     std::string const &but3 = std::string(),
     std::string const &but4 = std::string());

details::Expr tk_focusNext(std::string const &window);

details::Expr tk_focusPrev(std::string const &window);

details::Expr tk_getOpenFile();

details::Expr tk_getSaveFile();

details::Expr tk_menuSetFocus(std::string const &menu);

details::Expr tk_messageBox();

template <typename T, class InputIterator>
details::Expr tk_optionMenu(std::string const &butname, T &var,
     InputIterator b, InputIterator e)
{
     std::string str("tk_optionMenu ");
     str += butname; str += " ";
     str += details::addLinkVar(var);
     for (InputIterator i = b; i != e; ++i)
     {
          str += " \"";
          str += *i;
          str += '\"';
     }
     return details::Expr(str);
}

template <typename T1, typename T2>
details::Expr tk_popup(std::string const &menu, T1 const &x, T2 const &y)
{
     std::string str("tk_popup ");
     str += menu; str += " ";
     str += details::toString(x); str += " ";
     str += details::toString(y);
     return details::Expr(str);
}

template <typename T1, typename T2>
details::Expr tk_popup(std::string const &menu, T1 const &x, T2 const &y,
     int entry)
{
     std::string str("tk_popup ");
     str += menu; str += " ";
     str += details::toString(x); str += " ";
     str += details::toString(y); str += " ";
     str += details::toString(entry);
     return details::Expr(str);
}

details::Expr tk_setPalette(std::string const &color);

details::Expr tk_textCopy(std::string const &w);

details::Expr tk_textCut(std::string const &w);

details::Expr tk_textPaste(std::string const &w);

details::Expr tkwait(std::string const &option, std::string const &w);

details::Expr winfo(std::string const &option, std::string const &w);

template <typename T1, typename T2>
details::Expr winfo(std::string const &option, T1 const &val1,
     T2 const &val2)
{
     std::string str("winfo ");
     str += option;
     std::string postfix(" ");
     postfix += details::toString(val1);
     postfix += " ";
     postfix += details::toString(val2);
     return details::Expr(str, postfix);
}

details::Expr wm(std::string const &option, std::string const &w);

template <typename T>
details::Expr wm(std::string const &option, std::string const &w,
     T const &t)
{
     std::string str("wm ");
     str += option; str += " ";
     str += w; str += " \"";
     str += details::toString(t);
     str += '\"';
     return details::Expr(str);
}

template <typename T1, typename T2>
details::Expr wm(std::string const &option, std::string const &w,
     T1 const &t1, T2 const &t2)
{
     std::string str("wm ");
     str += option; str += " ";
     str += w; str += " \"";
     str += details::toString(t1); str += "\" \"";
     str += details::toString(t2); str += '\"';
     return details::Expr(str);
}

template <typename T1, typename T2, typename T3, typename T4>
details::Expr wm(std::string const &option, std::string const &w,
     T1 const &t1, T2 const &t2, T3 const &t3, T4 const &t4)
{
     std::string str("wm ");
     str += option; str += " ";
     str += w; str += " \"";
     str += details::toString(t1); str += "\" \"";
     str += details::toString(t2); str += "\" \"";
     str += details::toString(t3); str += "\" \"";
     str += details::toString(t4); str += '\"';
     return details::Expr(str);
}

details::Expr wmprotocol(std::string const &w,
     std::string const &proto = std::string());

template <class Functor>
details::Expr wmprotocol(std::string const &w,
     std::string const &proto, Functor f)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback0<Functor>(f)));

     std::string str("wm protocol ");
     str += w;          str += " ";
     str += proto;      str += " { ";
     str += newCmd;     str += " }";
     return details::Expr(str);
}



template <typename T>
details::Expr activate(T const &t)
{
     std::string str("activate ");
     str += details::toString(t);
     return details::Expr(str);
}

details::Expr addtag(std::string const &tag, std::string const &spec);

template <typename T>
details::Expr addtag(std::string const &tag, std::string const &spec,
     T const &arg)
{
     std::string str("addtag ");
     str += tag;    str += " ";
     str += spec;   str += " ";
     str += details::toString(arg);
     return details::Expr(str);
}

template <typename T1, typename T2>
details::Expr addtag(std::string const &tag, std::string const &spec,
     T1 const &arg1, T2 const &arg2)
{
     std::string str("addtag ");
     str += tag;    str += " ";
     str += spec;   str += " ";
     str += details::toString(arg1); str += " ";
     str += details::toString(arg2);
     return details::Expr(str);
}

template <typename T1, typename T2, typename T3>
details::Expr addtag(std::string const &tag, std::string const &spec,
     T1 const &arg1, T2 const &arg2, T3 const &arg3)
{
     std::string str("addtag ");
     str += tag;    str += " ";
     str += spec;   str += " ";
     str += details::toString(arg1);    str += " ";
     str += details::toString(arg2);    str += " ";
     str += details::toString(arg3);
     return details::Expr(str);
}

template <typename T1, typename T2, typename T3, typename T4>
details::Expr addtag(std::string const &tag, std::string const &spec,
     T1 const &arg1, T2 const &arg2, T3 const &arg3, T4 const &arg4)
{
     std::string str("addtag ");
     str += tag;    str += " ";
     str += spec;   str += " ";
     str += details::toString(arg1);    str += " ";
     str += details::toString(arg2);    str += " ";
     str += details::toString(arg3);    str += " ";
     str += details::toString(arg4);
     return details::Expr(str);
}

details::Expr blank();

template <typename T>
details::Expr canvasx(T const &x)
{
     std::string str("canvasx ");
     str += details::toString(x);
     return details::Expr(str);
}

template <typename T1, typename T2>
details::Expr canvasx(T1 const &x, T2 const &g)
{
     std::string str("canvasx ");
     str += details::toString(x); str += " ";
     str += details::toString(g);
     return details::Expr(str);
}

template <typename T>
details::Expr canvasy(T const &y)
{
     std::string str("canvasy ");
     str += details::toString(y);
     return details::Expr(str);
}

template <typename T1, typename T2>
details::Expr canvasy(T1 const &y, T2 const &g)
{
     std::string str("canvasy ");
     str += details::toString(y); str += " ";
     str += details::toString(g);
     return details::Expr(str);
}

details::Expr clone(std::string const &newpath,
     std::string const &type = std::string());

details::Expr compare(std::string const &indx1, std::string const &oper,
     std::string const &indx2);

details::Expr coords();

template <typename T>
details::Expr coords(T const &t)
{
     std::string str("coords ");
     str += details::toString(t);
     return details::Expr(str);
}

details::Expr coords(std::string const &item, int x, int y);

details::Expr coords(std::string const &item, Point const &p);

details::Expr coords(std::string const &item, int x1, int y1, int x2, int y2);

details::Expr coords(std::string const &item,
     Point const &p1, Point const &p2);

details::Expr coords(std::string const &item, Box const &b);

template <class InputIterator>
details::Expr coords(std::string const &item,
     InputIterator b, InputIterator e)
{
     std::string cmd("coords ");
     cmd += item;
     if (b == e)
     {
          throw TkError("A non-empty list of coordinates expected");
     }
     for (InputIterator i = b; i != e; ++i)
     {
          cmd += ' ';
          cmd += details::toString(*i);
     }
     
     return details::Expr(cmd);
}

details::Expr copy(std::string const &photo);

details::Expr curselection();

template <typename T>
details::Expr dchars(std::string const &item, T const &first)
{
     std::string str("dchars ");
     str += item;   str += " ";
     str += details::toString(first);
     return details::Expr(str);
}

template <typename T1, typename T2>
details::Expr dchars(std::string const &item,
     T1 const &first, T2 const &last)
{
     std::string str("dchars ");
     str += item;   str += " ";
     str += details::toString(first); str += " ";
     str += details::toString(last);
     return details::Expr(str);
}

details::Expr debug();
details::Expr debug(bool d);

template <typename T>
details::Expr deleteentry(T const &t)
{
     std::string str("delete ");
     str += details::toString(t);
     return details::Expr(str);
}

template <typename T1, typename T2>
details::Expr deleteentry(T1 const &t1, T2 const &t2)
{
     std::string str("delete ");
     str += details::toString(t1); str += " ";
     str += details::toString(t2);
     return details::Expr(str);
}

template <typename T>
details::Expr deleteitem(T const &t)
{
     std::string str("delete ");
     str += details::toString(t);
     return details::Expr(str);
}

template <class InputIterator>
details::Expr deleteitem(InputIterator b, InputIterator e)
{
     std::string cmd("delete");
     for (InputIterator i = b; i != e; ++i)
     {
          cmd += ' ';
          cmd += *i;
     }
     
     return details::Expr(cmd);
}

template <typename T>
details::Expr deletetext(T const &t)
{
     std::string str("delete ");
     str += details::toString(t);
     return details::Expr(str);
}

template <typename T1, typename T2>
details::Expr deletetext(T1 const &t1, T2 const &t2)
{
     std::string str("delete ");
     str += details::toString(t1); str += " ";
     str += details::toString(t2);
     return details::Expr(str);
}

template <typename T1, typename T2>
details::Expr delta(T1 const &t1, T2 const &t2)
{
     std::string str("delta ");
     str += details::toString(t1); str += " ";
     str += details::toString(t2);
     return details::Expr(str);
}

details::Expr deselect();

details::Expr dlineinfo(std::string const &indx);

details::Expr dtag(std::string const &tag,
     std::string const &todel = std::string());

details::Expr dump(std::string const &indx1,
     std::string const &indx2 = std::string());

details::Expr edit(std::string const &option);

template <typename T>
details::Expr edit(std::string const &option, T const &t)
{
     std::string str("edit ");
     str += option; str += " ";
     str += details::toString(t);
     return details::Expr(str);
}

template <typename T>
details::Expr entrycget(T const &index, std::string const &option)
{
     std::string str("entrycget ");
     str += details::toString(index); str += " -";
     str += option;
     return details::Expr(str);
}

template <typename T>
details::Expr entryconfigure(T const &index)
{
     std::string str("entryconfigure ");
     str += details::toString(index);
     return details::Expr(str);
}

details::Expr find(std::string const &spec);

template <typename T1>
details::Expr find(std::string const &spec, T1 const &arg1)
{
     std::string str("find ");
     str += spec;   str += " ";
     str += details::toString(arg1);
     return details::Expr(str);
}

template <typename T1, typename T2>
details::Expr find(std::string const &spec, T1 const &arg1, T2 const &arg2)
{
     std::string str("find ");
     str += spec;   str += " ";
     str += details::toString(arg1); str += " ";
     str += details::toString(arg2);
     return details::Expr(str);
}

template <typename T1, typename T2, typename T3>
details::Expr find(std::string const &spec,
     T1 const &arg1, T2 const &arg2, T3 const &arg3)
{
     std::string str("find ");
     str += spec;   str += " ";
     str += details::toString(arg1); str += " ";
     str += details::toString(arg2); str += " ";
     str += details::toString(arg3);
     return details::Expr(str);
}

template <typename T1, typename T2, typename T3, typename T4>
details::Expr find(std::string const &spec,
     T1 const &arg1, T2 const &arg2, T3 const &arg3, T4 const &arg4)
{
     std::string str("find ");
     str += spec;   str += " ";
     str += details::toString(arg1); str += " ";
     str += details::toString(arg2); str += " ";
     str += details::toString(arg3); str += " ";
     str += details::toString(arg4);
     return details::Expr(str);
}

details::Expr flash();

template <typename T1, typename T2>
details::Expr fraction(T1 const &t1, T2 const &t2)
{
     std::string str("fraction ");
     str += details::toString(t1); str += " ";
     str += details::toString(t2);
     return details::Expr(str);
}

details::Expr getsize();

details::Expr gettags(std::string const &item);

template <typename T>
details::Expr icursor(T const &index)
{
     std::string str("icursor ");
     str += details::toString(index);
     return details::Expr(str);
}

template <typename T>
details::Expr icursor(std::string const &item, T const &index)
{
     std::string str("icursor ");
     str += item;   str += " ";
     str += details::toString(index);
     return details::Expr(str);
}

template <typename T1, typename T2>
details::Expr identify(T1 const &x, T2 const &y)
{
     std::string str("identify ");
     str += details::toString(x); str += " ";
     str += details::toString(y);
     return details::Expr(str);
}

template <typename T>
details::Expr index(T const &index)
{
     std::string str("index ");
     str += details::toString(index);
     return details::Expr(str);
}

template <typename T>
details::Expr index(std::string const &item, T const &index)
{
     std::string str("index ");
     str += item;   str += " ";
     str += details::toString(index);
     return details::Expr(str);
}

template <typename T>
details::Expr insert(T const &index, std::string const &what)
{
     std::string str("insert ");
     str += details::toString(index); str += " \"";
     str += details::quote(what);   str += "\"";
     return details::Expr(str);
}

details::Expr insert(std::string const &index,
     std::string const &txt, std::string const &tag = std::string());

template <typename T>
details::Expr insert(std::string const &item, T const &index,
     std::string const &what)
{
     std::string str("insert ");
     str += item;   str += " ";
     str += details::toString(index); str += " \"";
     str += details::quote(what);   str += "\"";
     return details::Expr(str);
}

template <int N>
details::Expr insert(std::string const &indx, char const txt[N],
     std::string const &tag)
{
     std::string str("insert ");
     str += indx;   str += " \"";
     str += details::quote(txt); str += "\" ";
     str += tag;
     return details::Expr(str);
}

template <typename T, class InputIterator>
details::Expr insert(T const &index, InputIterator b, InputIterator e)
{
     std::string str("insert ");
     str += details::toString(index);
     for (InputIterator i = b; i != e; ++i)
     {
          str += " \"";
          str += details::toString(*i);
          str += '\"';
     }
     return details::Expr(str);
}

details::Expr invoke();

template <typename T>
details::Expr invoke(T const &index)
{
     std::string str("invoke ");
     str += details::toString(index);
     return details::Expr(str);
}

template <typename T>
details::Expr itemcget(T const &t, std::string const &name)
{
     std::string str("itemcget ");
     str += details::toString(t); str += " -";
     str += name;
     return details::Expr(str);
}

template <typename T>
details::Expr itemconfigure(T const &t)
{
     std::string str("itemconfigure ");
     str += details::toString(t);
     return details::Expr(str);
}

details::Expr move(std::string const &item, int x, int y);

template <typename T>
details::Expr nearest(T const &t)
{
     std::string str("nearest ");
     str += details::toString(t);
     return details::Expr(str);
}

details::Expr panecget(std::string const &w, std::string const &option);

details::Expr paneconfigure(std::string const &w);

details::Expr panes();

template <typename T1, typename T2>
details::Expr post(T1 const &x, T2 const &y)
{
     std::string str("post ");
     str += details::toString(x); str += " ";
     str += details::toString(y);
     return details::Expr(str);
}

template <typename T>
details::Expr postcascade(T const &index)
{
     std::string str("postcascade ");
     str += details::toString(index);
     return details::Expr(str);
}

details::Expr postscript();

details::Expr proxy(std::string const &option);

template <typename T1, typename T2>
details::Expr proxy(std::string const &option, T1 const &x, T2 const &y)
{
     std::string str("proxy ");
     str += option; str += " ";
     str += details::toString(x); str += " ";
     str += details::toString(y);
     return details::Expr(str);
}

details::Expr put(std::string const &color);

details::Expr read(std::string const &file);

details::Expr redither();

details::Expr sash(std::string const &option, int index);

template <typename T1, typename T2>
details::Expr sash(std::string const &option, int index,
     T1 const &x, T2 const &y)
{
     std::string str("sash ");
     str += option; str += " ";
     str += details::toString(index); str += " ";
     str += details::toString(x); str += " ";
     str += details::toString(y);
     return details::Expr(str);
}

template <typename T1, typename T2>
details::Expr scale(std::string const &item,
     T1 const &xorig, T2 const &yorig, double xscale, double yscale)
{
     std::string str("scale ");
     str += item;     str += " ";
     str += details::toString(xorig); str += " ";
     str += details::toString(yorig); str += " ";
     str += details::toString(xscale); str += " ";
     str += details::toString(yscale);
     return details::Expr(str);
}

template <typename T>
details::Expr scan(std::string const &option, T const &x)
{
     std::string str("scan ");
     str += option; str += " ";
     str += details::toString(x);
     return details::Expr(str);
}

template <typename T1, typename T2>
details::Expr scan(std::string const &option,
     T1 const &x, T2 const &y)
{
     std::string str("scan ");
     str += option; str += " ";
     str += details::toString(x); str += " ";
     str += details::toString(y);
     return details::Expr(str);
}

template <typename T1, typename T2>
details::Expr scan(std::string const &option,
     T1 const &x, T2 const &y, double gain)
{
     std::string str("scan ");
     str += option; str += " ";
     str += details::toString(x); str += " ";
     str += details::toString(y); str += " ";
     str += details::toString(gain);
     return details::Expr(str);
}

details::Expr search(std::string const &pattern,
     std::string const &indx1, std::string const &indx2 = std::string());

template <typename T>
details::Expr see(T const &t)
{
     std::string str("see ");
     str += details::toString(t);
     return details::Expr(str);
}

details::Expr select();
details::Expr select(std::string const &option);

template <typename T>
details::Expr select(std::string const &option,
     std::string const &item, T const &index)
{
     std::string str("select ");
     str += option; str += " ";
     str += item; str += " ";
     str += details::toString(index);
     return details::Expr(str);
}

details::Expr selection(std::string const &option);

template <typename T>
details::Expr selection(std::string const &option, T const &index)
{
     std::string str("selection ");
     str += option; str += " ";
     str += details::toString(index);
     return details::Expr(str);
}

template <typename T1, typename T2>
details::Expr selection(std::string const &option, T1 const &i1, T2 const &i2)
{
     std::string str("selection ");
     str += option; str += " ";
     str += details::toString(i1); str += " ";
     str += details::toString(i2);
     return details::Expr(str);
}

details::Expr tag(std::string const &option,
     std::string const &tagname = std::string());

details::Expr tag(std::string const &option, std::string const &tagname,
     std::string const &indx1, std::string const &indx2 = std::string());

details::Expr tag(std::string const &option, std::string const &tagname,
     std::string const &indx1, char const *indx2);

template <class Functor>
details::Expr tag(std::string const &option, std::string const &name,
     std::string const &seq, Functor f)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback0<Functor>(f)));

     std::string str("tag ");
     str += option;     str += " ";
     str += name;       str += " ";
     str += seq;        str += " { ";
     str += newCmd;     str += " }";
     return details::Expr(str);
}

template <class Functor, class EventAttr1>
details::Expr tag(std::string const &option, std::string const &name,
     std::string const &seq, Functor f,
     EventAttr1 const &ea1)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback1<Functor,
                    typename EventAttr1::attrType>(f)));
     
     std::string str("tag ");
     str += option;     str += " ";
     str += name;       str += " ";
     str += seq;        str += " { ";
     str += newCmd;     str += " ";
     str += ea1.get();  str += " }";
     return details::Expr(str);
}

template <class Functor, class EventAttr1, class EventAttr2>
details::Expr tag(std::string const &option, std::string const &name,
     std::string const &seq, Functor f,
     EventAttr1 const &ea1, EventAttr2 const &ea2)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback2<Functor,
                    typename EventAttr1::attrType,
                    typename EventAttr2::attrType>(f)));
     
     std::string str("tag ");
     str += option;     str += " ";
     str += name;       str += " ";
     str += seq;        str += " { ";
     str += newCmd;     str += " ";
     str += ea1.get();  str += " ";
     str += ea2.get();  str += " }";
     return details::Expr(str);
}

template <class Functor, class EventAttr1, class EventAttr2,
     class EventAttr3>
details::Expr tag(std::string const &option, std::string const &name,
     std::string const &seq, Functor f,
     EventAttr1 const &ea1, EventAttr2 const &ea2, EventAttr3 const &ea3)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback3<Functor,
                    typename EventAttr1::attrType,
                    typename EventAttr2::attrType,
                    typename EventAttr3::attrType>(f)));
     
     std::string str("tag ");
     str += option;     str += " ";
     str += name;       str += " ";
     str += seq;        str += " { ";
     str += newCmd;     str += " ";
     str += ea1.get();  str += " ";
     str += ea2.get();  str += " ";
     str += ea3.get();  str += " }";
     return details::Expr(str);
}

template <class Functor, class EventAttr1, class EventAttr2,
     class EventAttr3, class EventAttr4>
details::Expr tag(std::string const &option, std::string const &name,
     std::string const &seq, Functor f,
     EventAttr1 const &ea1, EventAttr2 const &ea2, EventAttr3 const &ea3,
     EventAttr4 const &ea4)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback4<Functor,
                    typename EventAttr1::attrType,
                    typename EventAttr2::attrType,
                    typename EventAttr3::attrType,
                    typename EventAttr4::attrType>(f)));
     
     std::string str("tag ");
     str += option;     str += " ";
     str += name;       str += " ";
     str += seq;        str += " { ";
     str += newCmd;     str += " ";
     str += ea1.get();  str += " ";
     str += ea2.get();  str += " ";
     str += ea3.get();  str += " ";
     str += ea4.get();  str += " }";
     return details::Expr(str);
}

template <class Functor, class EventAttr1, class EventAttr2,
     class EventAttr3, class EventAttr4, class EventAttr5>
details::Expr tag(std::string const &option, std::string const &name,
     std::string const &seq, Functor f,
     EventAttr1 const &ea1, EventAttr2 const &ea2, EventAttr3 const &ea3,
     EventAttr4 const &ea4, EventAttr5 const &ea5)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback5<Functor,
                    typename EventAttr1::attrType,
                    typename EventAttr2::attrType,
                    typename EventAttr3::attrType,
                    typename EventAttr4::attrType,
                    typename EventAttr5::attrType>(f)));
     
     std::string str("tag ");
     str += option;     str += " ";
     str += name;       str += " ";
     str += seq;        str += " { ";
     str += newCmd;     str += " ";
     str += ea1.get();  str += " ";
     str += ea2.get();  str += " ";
     str += ea3.get();  str += " ";
     str += ea4.get();  str += " ";
     str += ea5.get();  str += " }";
     return details::Expr(str);
}

template <class Functor, class EventAttr1, class EventAttr2,
     class EventAttr3, class EventAttr4, class EventAttr5, class EventAttr6>
details::Expr tag(std::string const &option, std::string const &name,
     std::string const &seq, Functor f,
     EventAttr1 const &ea1, EventAttr2 const &ea2, EventAttr3 const &ea3,
     EventAttr4 const &ea4, EventAttr5 const &ea5, EventAttr6 const &ea6)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback6<Functor,
                    typename EventAttr1::attrType,
                    typename EventAttr2::attrType,
                    typename EventAttr3::attrType,
                    typename EventAttr4::attrType,
                    typename EventAttr5::attrType,
                    typename EventAttr6::attrType>(f)));
     
     std::string str("tag ");
     str += option;     str += " ";
     str += name;       str += " ";
     str += seq;        str += " { ";
     str += newCmd;     str += " ";
     str += ea1.get();  str += " ";
     str += ea2.get();  str += " ";
     str += ea3.get();  str += " ";
     str += ea4.get();  str += " ";
     str += ea5.get();  str += " ";
     str += ea6.get();  str += " }";
     return details::Expr(str);
}

template <class Functor, class EventAttr1, class EventAttr2,
     class EventAttr3, class EventAttr4, class EventAttr5, class EventAttr6,
     class EventAttr7>
details::Expr tag(std::string const &option, std::string const &name,
     std::string const &seq, Functor f,
     EventAttr1 const &ea1, EventAttr2 const &ea2, EventAttr3 const &ea3,
     EventAttr4 const &ea4, EventAttr5 const &ea5, EventAttr6 const &ea6,
     EventAttr7 const &ea7)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback7<Functor,
                    typename EventAttr1::attrType,
                    typename EventAttr2::attrType,
                    typename EventAttr3::attrType,
                    typename EventAttr4::attrType,
                    typename EventAttr5::attrType,
                    typename EventAttr6::attrType,
                    typename EventAttr7::attrType>(f)));
     
     std::string str("tag ");
     str += option;     str += " ";
     str += name;       str += " ";
     str += seq;        str += " { ";
     str += newCmd;     str += " ";
     str += ea1.get();  str += " ";
     str += ea2.get();  str += " ";
     str += ea3.get();  str += " ";
     str += ea4.get();  str += " ";
     str += ea5.get();  str += " ";
     str += ea6.get();  str += " ";
     str += ea7.get();  str += " }";
     return details::Expr(str);
}

template <class Functor, class EventAttr1, class EventAttr2,
     class EventAttr3, class EventAttr4, class EventAttr5, class EventAttr6,
     class EventAttr7, class EventAttr8>
details::Expr tag(std::string const &option, std::string const &name,
     std::string const &seq, Functor f,
     EventAttr1 const &ea1, EventAttr2 const &ea2, EventAttr3 const &ea3,
     EventAttr4 const &ea4, EventAttr5 const &ea5, EventAttr6 const &ea6,
     EventAttr7 const &ea7, EventAttr8 const &ea8)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback8<Functor,
                    typename EventAttr1::attrType,
                    typename EventAttr2::attrType,
                    typename EventAttr3::attrType,
                    typename EventAttr4::attrType,
                    typename EventAttr5::attrType,
                    typename EventAttr6::attrType,
                    typename EventAttr7::attrType,
                    typename EventAttr8::attrType>(f)));
     
     std::string str("tag ");
     str += option;     str += " ";
     str += name;       str += " ";
     str += seq;        str += " { ";
     str += newCmd;     str += " ";
     str += ea1.get();  str += " ";
     str += ea2.get();  str += " ";
     str += ea3.get();  str += " ";
     str += ea4.get();  str += " ";
     str += ea5.get();  str += " ";
     str += ea6.get();  str += " ";
     str += ea7.get();  str += " ";
     str += ea8.get();  str += " }";
     return details::Expr(str);
}

template <class Functor, class EventAttr1, class EventAttr2,
     class EventAttr3, class EventAttr4, class EventAttr5, class EventAttr6,
     class EventAttr7, class EventAttr8, class EventAttr9>
details::Expr tag(std::string const &option, std::string const &name,
     std::string const &seq, Functor f,
     EventAttr1 const &ea1, EventAttr2 const &ea2, EventAttr3 const &ea3,
     EventAttr4 const &ea4, EventAttr5 const &ea5, EventAttr6 const &ea6,
     EventAttr7 const &ea7, EventAttr8 const &ea8, EventAttr9 const &ea9)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback9<Functor,
                    typename EventAttr1::attrType,
                    typename EventAttr2::attrType,
                    typename EventAttr3::attrType,
                    typename EventAttr4::attrType,
                    typename EventAttr5::attrType,
                    typename EventAttr6::attrType,
                    typename EventAttr7::attrType,
                    typename EventAttr8::attrType,
                    typename EventAttr9::attrType>(f)));
     
     std::string str("tag ");
     str += option;     str += " ";
     str += name;       str += " ";
     str += seq;        str += " { ";
     str += newCmd;     str += " ";
     str += ea1.get();  str += " ";
     str += ea2.get();  str += " ";
     str += ea3.get();  str += " ";
     str += ea4.get();  str += " ";
     str += ea5.get();  str += " ";
     str += ea6.get();  str += " ";
     str += ea7.get();  str += " ";
     str += ea8.get();  str += " ";
     str += ea9.get();  str += " }";
     return details::Expr(str);
}

template <class Functor, class EventAttr1, class EventAttr2,
     class EventAttr3, class EventAttr4, class EventAttr5, class EventAttr6,
     class EventAttr7, class EventAttr8, class EventAttr9, class EventAttr10>
details::Expr tag(std::string const &option, std::string const &name,
     std::string const &seq, Functor f,
     EventAttr1 const &ea1, EventAttr2 const &ea2, EventAttr3 const &ea3,
     EventAttr4 const &ea4, EventAttr5 const &ea5, EventAttr6 const &ea6,
     EventAttr7 const &ea7, EventAttr8 const &ea8, EventAttr9 const &ea9,
     EventAttr10 const &ea10)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback10<Functor,
                    typename EventAttr1::attrType,
                    typename EventAttr2::attrType,
                    typename EventAttr3::attrType,
                    typename EventAttr4::attrType,
                    typename EventAttr5::attrType,
                    typename EventAttr6::attrType,
                    typename EventAttr7::attrType,
                    typename EventAttr8::attrType,
                    typename EventAttr9::attrType,
                    typename EventAttr10::attrType>(f)));
     
     std::string str("tag ");
     str += option;     str += " ";
     str += name;       str += " ";
     str += seq;        str += " { ";
     str += newCmd;     str += " ";
     str += ea1.get();  str += " ";
     str += ea2.get();  str += " ";
     str += ea3.get();  str += " ";
     str += ea4.get();  str += " ";
     str += ea5.get();  str += " ";
     str += ea6.get();  str += " ";
     str += ea7.get();  str += " ";
     str += ea8.get();  str += " ";
     str += ea9.get();  str += " ";
     str += ea10.get(); str += " }";
     return details::Expr(str);
}

details::Expr toggle();

details::Expr transparency(std::string const &option, int x, int y);
details::Expr transparency(std::string const &option, int x, int y, bool tr);

details::Expr windows(std::string const &option,
     std::string const &indx = std::string(),
     std::string const &name = std::string());

details::Expr write(std::string const &file);

details::Expr xview();

template <typename T>
details::Expr xview(T const &t)
{
     std::string str("xview ");
     str += details::toString(t);
     return details::Expr(str);
}
details::Expr xview(std::string const &option, double fraction);
details::Expr xview(std::string const option,
     int number, std::string const &what);

template <typename T>
details::Expr yposition(T const &index)
{
     std::string str("yposition ");
     str += details::toString(index);
     return details::Expr(str);
}

details::Expr yview();
details::Expr yview(std::string const &option, double fraction);
details::Expr yview(std::string const option,
     int number, std::string const &what);



#define CPPTK_OPTION(name, quote) extern details::Option name;
#include "cpptkoptions.x"
#undef CPPTK_OPTION



template <typename T1, typename T2, typename T3>
details::Expr arrowshape(T1 const &t1, T2 const &t2, T3 const &t3)
{
     std::string str(" -arrowshape {");
     str += details::toString(t1); str += " ";
     str += details::toString(t2); str += " ";
     str += details::toString(t3); str += "}";
     return details::Expr(str, false);
}

details::Expr backwards();


details::Expr cliptype(std::string const &type);

details::Expr count(int &i);

details::Expr count(std::string const &name);

details::Expr defaultbutton(std::string const &but);

details::Expr defaultstate(std::string const &name);

details::Expr exact();

details::Expr filetypes(std::string const &types);

details::Expr forwards();

details::Expr grayscale();

template <class Functor> details::Expr invalidcommand(Functor f)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback0<Functor>(f)));
     
     std::string str(" -invalidcommand ");
     str += newCmd;
     return details::Expr(str, false);
}

details::Expr invalidcommand(char const *name);
details::Expr invalidcommand(std::string const &name);

class CallbackHandle;
details::Expr invalidcommand(CallbackHandle const &handle);

template <class Functor> details::Expr postcommand(Functor f)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback0<Functor>(f)));
     
     std::string str(" -postcommand ");
     str += newCmd;
     return details::Expr(str, false);
}

details::Expr menutype(std::string const &type);

details::Expr messagetext(std::string const &txt);

details::Expr messagetype(std::string const &type);

details::Expr multiple();

details::Expr nocase();

details::Expr postcommand(std::string const &name);

class CallbackHandle;
details::Expr postcommand(CallbackHandle const &handle);

details::Expr regexp();

template <typename T1, typename T2, typename T3, typename T4>
details::Expr scrollregion(T1 const &x1, T2 const &y1,
     T3 const &x2, T4 const y2)
{
     std::string str(" -scrollregion ");
     str += details::toString(x1); str += " ";
     str += details::toString(y1); str += " ";
     str += details::toString(x2); str += " ";
     str += details::toString(y2);
     return details::Expr(str, false);
}

details::Expr shrink();

details::Expr submenu(std::string const &menu);

details::Expr subsample(int x, int y);

details::Expr tags();

template <class InputIterator>
details::Expr tags(InputIterator b, InputIterator e)
{
     std::string cmd(" -tags {");
     for (InputIterator i = b; i != e; ++i)
     {
          cmd += ' ';
          cmd += *i;
     }
     cmd += "}";
     
     return details::Expr(cmd, false);
}

template <class Functor> details::Expr tearoffcommand(Functor f)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback0<Functor>(f)));
     
     std::string str(" -tearoffcommand ");
     str += newCmd;
     return details::Expr(str, false);
}

details::Expr tearoffcommand(std::string const &name);

class CallbackHandle;
details::Expr tearoffcommand(CallbackHandle const &handle);

template <typename T>
details::Expr textvariable(T &t)
{
     std::string str(" -textvariable ");
     str += details::addLinkVar(t);
     return details::Expr(str, false);
}

details::Expr textvariable(std::string const &name);

template <class Functor>
details::Expr validatecommand(Functor f)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback0<Functor>(f)));
     
     std::string str(" -validatecommand ");
     str += newCmd;
     return details::Expr(str, false);
}

template <class Functor, class ValidateAttr1>
details::Expr validatecommand(Functor f, ValidateAttr1 const &va1)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback1<Functor,
                    typename ValidateAttr1::validType>(f)));
     
     std::string str(" -validatecommand { ");
     str += newCmd;     str += " ";
     str += va1.get();  str += " }";
     return details::Expr(str, false);
}

template <class Functor, class ValidateAttr1, class ValidateAttr2>
details::Expr validatecommand(Functor f, ValidateAttr1 const &va1,
     ValidateAttr2 const &va2)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback2<Functor,
                    typename ValidateAttr1::validType,
                    typename ValidateAttr2::validType>(f)));
     
     std::string str(" -validatecommand { ");
     str += newCmd;     str += " ";
     str += va1.get();  str += " ";
     str += va2.get();  str += " }";
     return details::Expr(str, false);
}

template <class Functor, class ValidateAttr1, class ValidateAttr2,
     class ValidateAttr3>
details::Expr validatecommand(Functor f, ValidateAttr1 const &va1,
     ValidateAttr2 const &va2, ValidateAttr3 const &va3)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback3<Functor,
                    typename ValidateAttr1::validType,
                    typename ValidateAttr2::validType,
                    typename ValidateAttr3::validType>(f)));
     
     std::string str(" -validatecommand { ");
     str += newCmd;     str += " ";
     str += va1.get();  str += " ";
     str += va2.get();  str += " ";
     str += va3.get();  str += " }";
     return details::Expr(str, false);
}

template <class Functor, class ValidateAttr1, class ValidateAttr2,
     class ValidateAttr3, class ValidateAttr4>
details::Expr validatecommand(Functor f, ValidateAttr1 const &va1,
     ValidateAttr2 const &va2, ValidateAttr3 const &va3,
     ValidateAttr4 const &va4)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback4<Functor,
                    typename ValidateAttr1::validType,
                    typename ValidateAttr2::validType,
                    typename ValidateAttr3::validType,
                    typename ValidateAttr4::validType>(f)));
     
     std::string str(" -validatecommand { ");
     str += newCmd;     str += " ";
     str += va1.get();  str += " ";
     str += va2.get();  str += " ";
     str += va3.get();  str += " ";
     str += va4.get();  str += " }";
     return details::Expr(str, false);
}

template <class Functor, class ValidateAttr1, class ValidateAttr2,
     class ValidateAttr3, class ValidateAttr4, class ValidateAttr5>
details::Expr validatecommand(Functor f, ValidateAttr1 const &va1,
     ValidateAttr2 const &va2, ValidateAttr3 const &va3,
     ValidateAttr4 const &va4, ValidateAttr5 const &va5)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback5<Functor,
                    typename ValidateAttr1::validType,
                    typename ValidateAttr2::validType,
                    typename ValidateAttr3::validType,
                    typename ValidateAttr4::validType,
                    typename ValidateAttr5::validType>(f)));
     
     std::string str(" -validatecommand { ");
     str += newCmd;     str += " ";
     str += va1.get();  str += " ";
     str += va2.get();  str += " ";
     str += va3.get();  str += " ";
     str += va4.get();  str += " ";
     str += va5.get();  str += " }";
     return details::Expr(str, false);
}

template <class Functor, class ValidateAttr1, class ValidateAttr2,
     class ValidateAttr3, class ValidateAttr4, class ValidateAttr5,
     class ValidateAttr6>
details::Expr validatecommand(Functor f, ValidateAttr1 const &va1,
     ValidateAttr2 const &va2, ValidateAttr3 const &va3,
     ValidateAttr4 const &va4, ValidateAttr5 const &va5,
     ValidateAttr6 const &va6)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback6<Functor,
                    typename ValidateAttr1::validType,
                    typename ValidateAttr2::validType,
                    typename ValidateAttr3::validType,
                    typename ValidateAttr4::validType,
                    typename ValidateAttr5::validType,
                    typename ValidateAttr6::validType>(f)));
     
     std::string str(" -validatecommand { ");
     str += newCmd;     str += " ";
     str += va1.get();  str += " ";
     str += va2.get();  str += " ";
     str += va3.get();  str += " ";
     str += va4.get();  str += " ";
     str += va5.get();  str += " ";
     str += va6.get();  str += " }";
     return details::Expr(str, false);
}

template <class Functor, class ValidateAttr1, class ValidateAttr2,
     class ValidateAttr3, class ValidateAttr4, class ValidateAttr5,
     class ValidateAttr6, class ValidateAttr7>
details::Expr validatecommand(Functor f, ValidateAttr1 const &va1,
     ValidateAttr2 const &va2, ValidateAttr3 const &va3,
     ValidateAttr4 const &va4, ValidateAttr5 const &va5,
     ValidateAttr6 const &va6, ValidateAttr7 const &va7)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback7<Functor,
                    typename ValidateAttr1::validType,
                    typename ValidateAttr2::validType,
                    typename ValidateAttr3::validType,
                    typename ValidateAttr4::validType,
                    typename ValidateAttr5::validType,
                    typename ValidateAttr6::validType,
                    typename ValidateAttr7::validType>(f)));
     
     std::string str(" -validatecommand { ");
     str += newCmd;     str += " ";
     str += va1.get();  str += " ";
     str += va2.get();  str += " ";
     str += va3.get();  str += " ";
     str += va4.get();  str += " ";
     str += va5.get();  str += " ";
     str += va6.get();  str += " ";
     str += va7.get();  str += " }";
     return details::Expr(str, false);
}

template <class Functor, class ValidateAttr1, class ValidateAttr2,
     class ValidateAttr3, class ValidateAttr4, class ValidateAttr5,
     class ValidateAttr6, class ValidateAttr7, class ValidateAttr8>
details::Expr validatecommand(Functor f, ValidateAttr1 const &va1,
     ValidateAttr2 const &va2, ValidateAttr3 const &va3,
     ValidateAttr4 const &va4, ValidateAttr5 const &va5,
     ValidateAttr6 const &va6, ValidateAttr7 const &va7,
     ValidateAttr8 const &va8)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback8<Functor,
                    typename ValidateAttr1::validType,
                    typename ValidateAttr2::validType,
                    typename ValidateAttr3::validType,
                    typename ValidateAttr4::validType,
                    typename ValidateAttr5::validType,
                    typename ValidateAttr6::validType,
                    typename ValidateAttr7::validType,
                    typename ValidateAttr8::validType>(f)));
     
     std::string str(" -validatecommand { ");
     str += newCmd;     str += " ";
     str += va1.get();  str += " ";
     str += va2.get();  str += " ";
     str += va3.get();  str += " ";
     str += va4.get();  str += " ";
     str += va5.get();  str += " ";
     str += va6.get();  str += " ";
     str += va7.get();  str += " ";
     str += va8.get();  str += " }";
     return details::Expr(str, false);
}

template <typename T>
details::Expr variable(T &t)
{
     std::string str(" -variable ");
     str += details::addLinkVar(t);
     return details::Expr(str, false);
}

details::Expr variable(std::string const &name);

details::Expr zoom(double x, double y);



extern details::EventAttr<std::string> event_A; 
extern details::EventAttr<int>         event_b; 
extern details::EventAttr<int>         event_D; 
extern details::EventAttr<int>         event_f; 
extern details::EventAttr<int>         event_h; 
extern details::EventAttr<std::string> event_k; 
extern details::EventAttr<std::string> event_K; 
extern details::EventAttr<std::string> event_m; 
extern details::EventAttr<int>         event_N; 
extern details::EventAttr<std::string> event_s; 
extern details::EventAttr<std::string> event_T; 
extern details::EventAttr<int>         event_w; 
extern details::EventAttr<std::string> event_W; 
extern details::EventAttr<int>         event_x; 
extern details::EventAttr<int>         event_X; 
extern details::EventAttr<int>         event_y; 
extern details::EventAttr<int>         event_Y; 

extern details::ValidateAttr<int>         valid_d; 
extern details::ValidateAttr<int>         valid_i; 
extern details::ValidateAttr<std::string> valid_P; 
extern details::ValidateAttr<std::string> valid_s; 
extern details::ValidateAttr<std::string> valid_S; 
extern details::ValidateAttr<std::string> valid_v; 
extern details::ValidateAttr<std::string> valid_V; 
extern details::ValidateAttr<std::string> valid_W; 



#define CPPTK_CONSTANT(c) extern char const * c;
#include "cpptkconstants.x"
#undef CPPTK_CONSTANT



extern char const *deletefont; 
extern char const *deleteimg;  
extern char const *deletetag;  
extern char const *setglobal;
extern char const *wrapchar;   
extern char const *wrapword;   



template <class Functor>
details::Expr afteridle(Functor f)
{
     std::string newCmd = details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback0<Functor>(f)));

     std::string str("after idle ");
     str += newCmd;
     return details::Expr(str);
}

details::Expr afteridle(std::string const &cmd);

details::Expr update(std::string const &option = std::string());





namespace details
{

class BindToken : public BasicToken
{
public:
     BindToken();
     
     Expr operator()(std::string const &name,
          std::string const &seq) const;

     template <class Functor>
     details::Expr operator()(std::string const &name,
          std::string const &seq, Functor f) const
     {
          std::string newCmd = addCallback(
               std::shared_ptr<CallbackBase>(
                    new Callback0<Functor>(f)));

          std::string str("bind ");
          str += name;       str += " ";
          str += seq;        str += " { ";
          str += newCmd;     str += " }";
          return Expr(str);
     }

     template <class Functor, class EventAttr1>
     Expr operator()(std::string const &name,
          std::string const &seq, Functor f,
          EventAttr1 const &ea1) const
     {
          std::string newCmd = addCallback(
               std::shared_ptr<CallbackBase>(
                    new Callback1<Functor,
                         typename EventAttr1::attrType>(f)));
     
          std::string str("bind ");
          str += name;       str += " ";
          str += seq;        str += " { ";
          str += newCmd;     str += " ";
          str += ea1.get();  str += " }";
          return Expr(str);
     }

     template <class Functor, class EventAttr1, class EventAttr2>
     Expr operator()(std::string const &name,
          std::string const &seq, Functor f,
          EventAttr1 const &ea1, EventAttr2 const &ea2) const
     {
          std::string newCmd = addCallback(
               std::shared_ptr<CallbackBase>(
                    new Callback2<Functor,
                         typename EventAttr1::attrType,
                         typename EventAttr2::attrType>(f)));
     
          std::string str("bind ");
          str += name;       str += " ";
          str += seq;        str += " { ";
          str += newCmd;     str += " ";
          str += ea1.get();  str += " ";
          str += ea2.get();  str += " }";
          return Expr(str);
     }

     template <class Functor, class EventAttr1, class EventAttr2,
          class EventAttr3>
     Expr operator()(std::string const &name,
          std::string const &seq, Functor f,
          EventAttr1 const &ea1, EventAttr2 const &ea2,
          EventAttr3 const &ea3) const
     {
          std::string newCmd = addCallback(
               std::shared_ptr<CallbackBase>(
                    new Callback3<Functor,
                         typename EventAttr1::attrType,
                         typename EventAttr2::attrType,
                         typename EventAttr3::attrType>(f)));
     
          std::string str("bind ");
          str += name;       str += " ";
          str += seq;        str += " { ";
          str += newCmd;     str += " ";
          str += ea1.get();  str += " ";
          str += ea2.get();  str += " ";
          str += ea3.get();  str += " }";
          return Expr(str);
     }

     template <class Functor, class EventAttr1, class EventAttr2,
          class EventAttr3, class EventAttr4>
     Expr operator()(std::string const &name,
          std::string const &seq, Functor f,
          EventAttr1 const &ea1, EventAttr2 const &ea2, EventAttr3 const &ea3,
          EventAttr4 const &ea4) const
     {
          std::string newCmd = addCallback(
               std::shared_ptr<CallbackBase>(
                    new Callback4<Functor,
                         typename EventAttr1::attrType,
                         typename EventAttr2::attrType,
                         typename EventAttr3::attrType,
                         typename EventAttr4::attrType>(f)));
     
          std::string str("bind ");
          str += name;       str += " ";
          str += seq;        str += " { ";
          str += newCmd;     str += " ";
          str += ea1.get();  str += " ";
          str += ea2.get();  str += " ";
          str += ea3.get();  str += " ";
          str += ea4.get();  str += " }";
          return Expr(str);
     }

     template <class Functor, class EventAttr1, class EventAttr2,
          class EventAttr3, class EventAttr4, class EventAttr5>
     Expr operator()(std::string const &name,
          std::string const &seq, Functor f,
          EventAttr1 const &ea1, EventAttr2 const &ea2, EventAttr3 const &ea3,
          EventAttr4 const &ea4, EventAttr5 const &ea5) const
     {
          std::string newCmd = addCallback(
               std::shared_ptr<CallbackBase>(
                    new Callback5<Functor,
                         typename EventAttr1::attrType,
                         typename EventAttr2::attrType,
                         typename EventAttr3::attrType,
                         typename EventAttr4::attrType,
                         typename EventAttr5::attrType>(f)));
     
          std::string str("bind ");
          str += name;       str += " ";
          str += seq;        str += " { ";
          str += newCmd;     str += " ";
          str += ea1.get();  str += " ";
          str += ea2.get();  str += " ";
          str += ea3.get();  str += " ";
          str += ea4.get();  str += " ";
          str += ea5.get();  str += " }";
          return Expr(str);
     }

     template <class Functor, class EventAttr1, class EventAttr2,
          class EventAttr3, class EventAttr4, class EventAttr5,
          class EventAttr6>
     Expr operator()(std::string const &name,
          std::string const &seq, Functor f,
          EventAttr1 const &ea1, EventAttr2 const &ea2, EventAttr3 const &ea3,
          EventAttr4 const &ea4, EventAttr5 const &ea5,
          EventAttr6 const &ea6) const
     {
          std::string newCmd = addCallback(
               std::shared_ptr<CallbackBase>(
                    new Callback6<Functor,
                         typename EventAttr1::attrType,
                         typename EventAttr2::attrType,
                         typename EventAttr3::attrType,
                         typename EventAttr4::attrType,
                         typename EventAttr5::attrType,
                         typename EventAttr6::attrType>(f)));
     
          std::string str("bind ");
          str += name;       str += " ";
          str += seq;        str += " { ";
          str += newCmd;     str += " ";
          str += ea1.get();  str += " ";
          str += ea2.get();  str += " ";
          str += ea3.get();  str += " ";
          str += ea4.get();  str += " ";
          str += ea5.get();  str += " ";
          str += ea6.get();  str += " }";
          return Expr(str);
     }

     template <class Functor, class EventAttr1, class EventAttr2,
          class EventAttr3, class EventAttr4, class EventAttr5,
          class EventAttr6, class EventAttr7>
     Expr operator()(std::string const &name,
          std::string const &seq, Functor f,
          EventAttr1 const &ea1, EventAttr2 const &ea2, EventAttr3 const &ea3,
          EventAttr4 const &ea4, EventAttr5 const &ea5, EventAttr6 const &ea6,
          EventAttr7 const &ea7) const
     {
          std::string newCmd = addCallback(
               std::shared_ptr<CallbackBase>(
                    new Callback7<Functor,
                         typename EventAttr1::attrType,
                         typename EventAttr2::attrType,
                         typename EventAttr3::attrType,
                         typename EventAttr4::attrType,
                         typename EventAttr5::attrType,
                         typename EventAttr6::attrType,
                         typename EventAttr7::attrType>(f)));
     
          std::string str("bind ");
          str += name;       str += " ";
          str += seq;        str += " { ";
          str += newCmd;     str += " ";
          str += ea1.get();  str += " ";
          str += ea2.get();  str += " ";
          str += ea3.get();  str += " ";
          str += ea4.get();  str += " ";
          str += ea5.get();  str += " ";
          str += ea6.get();  str += " ";
          str += ea7.get();  str += " }";
          return Expr(str);
     }

     template <class Functor, class EventAttr1, class EventAttr2,
          class EventAttr3, class EventAttr4, class EventAttr5,
          class EventAttr6, class EventAttr7, class EventAttr8>
     Expr operator()(std::string const &name,
          std::string const &seq, Functor f,
          EventAttr1 const &ea1, EventAttr2 const &ea2, EventAttr3 const &ea3,
          EventAttr4 const &ea4, EventAttr5 const &ea5, EventAttr6 const &ea6,
          EventAttr7 const &ea7, EventAttr8 const &ea8) const
     {
          std::string newCmd = addCallback(
               std::shared_ptr<CallbackBase>(
                    new Callback8<Functor,
                         typename EventAttr1::attrType,
                         typename EventAttr2::attrType,
                         typename EventAttr3::attrType,
                         typename EventAttr4::attrType,
                         typename EventAttr5::attrType,
                         typename EventAttr6::attrType,
                         typename EventAttr7::attrType,
                         typename EventAttr8::attrType>(f)));
     
          std::string str("bind ");
          str += name;       str += " ";
          str += seq;        str += " { ";
          str += newCmd;     str += " ";
          str += ea1.get();  str += " ";
          str += ea2.get();  str += " ";
          str += ea3.get();  str += " ";
          str += ea4.get();  str += " ";
          str += ea5.get();  str += " ";
          str += ea6.get();  str += " ";
          str += ea7.get();  str += " ";
          str += ea8.get();  str += " }";
          return Expr(str);
     }

     template <class Functor, class EventAttr1, class EventAttr2,
          class EventAttr3, class EventAttr4, class EventAttr5,
          class EventAttr6, class EventAttr7, class EventAttr8,
          class EventAttr9>
     Expr operator()(std::string const &name,
          std::string const &seq, Functor f,
          EventAttr1 const &ea1, EventAttr2 const &ea2, EventAttr3 const &ea3,
          EventAttr4 const &ea4, EventAttr5 const &ea5, EventAttr6 const &ea6,
          EventAttr7 const &ea7, EventAttr8 const &ea8,
          EventAttr9 const &ea9) const
     {
          std::string newCmd = addCallback(
               std::shared_ptr<CallbackBase>(
                    new Callback9<Functor,
                         typename EventAttr1::attrType,
                         typename EventAttr2::attrType,
                         typename EventAttr3::attrType,
                         typename EventAttr4::attrType,
                         typename EventAttr5::attrType,
                         typename EventAttr6::attrType,
                         typename EventAttr7::attrType,
                         typename EventAttr8::attrType,
                         typename EventAttr9::attrType>(f)));
     
          std::string str("bind ");
          str += name;       str += " ";
          str += seq;        str += " { ";
          str += newCmd;     str += " ";
          str += ea1.get();  str += " ";
          str += ea2.get();  str += " ";
          str += ea3.get();  str += " ";
          str += ea4.get();  str += " ";
          str += ea5.get();  str += " ";
          str += ea6.get();  str += " ";
          str += ea7.get();  str += " ";
          str += ea8.get();  str += " ";
          str += ea9.get();  str += " }";
          return Expr(str);
     }

     template <class Functor, class EventAttr1, class EventAttr2,
          class EventAttr3, class EventAttr4, class EventAttr5,
          class EventAttr6, class EventAttr7, class EventAttr8,
          class EventAttr9, class EventAttr10>
     Expr operator()(std::string const &name,
          std::string const &seq, Functor f,
          EventAttr1 const &ea1, EventAttr2 const &ea2, EventAttr3 const &ea3,
          EventAttr4 const &ea4, EventAttr5 const &ea5, EventAttr6 const &ea6,
          EventAttr7 const &ea7, EventAttr8 const &ea8, EventAttr9 const &ea9,
          EventAttr10 const &ea10) const
     {
          std::string newCmd = addCallback(
               std::shared_ptr<CallbackBase>(
                    new Callback10<Functor,
                         typename EventAttr1::attrType,
                         typename EventAttr2::attrType,
                         typename EventAttr3::attrType,
                         typename EventAttr4::attrType,
                         typename EventAttr5::attrType,
                         typename EventAttr6::attrType,
                         typename EventAttr7::attrType,
                         typename EventAttr8::attrType,
                         typename EventAttr9::attrType,
                         typename EventAttr10::attrType>(f)));
     
          std::string str("bind ");
          str += name;       str += " ";
          str += seq;        str += " { ";
          str += newCmd;     str += " ";
          str += ea1.get();  str += " ";
          str += ea2.get();  str += " ";
          str += ea3.get();  str += " ";
          str += ea4.get();  str += " ";
          str += ea5.get();  str += " ";
          str += ea6.get();  str += " ";
          str += ea7.get();  str += " ";
          str += ea8.get();  str += " ";
          str += ea9.get();  str += " ";
          str += ea10.get(); str += " }";
          return Expr(str);
     }
};

class CheckButtonToken : public BasicToken
{
public:
     CheckButtonToken();
     Expr operator()(std::string const &name) const;
};

class FrameToken : public BasicToken
{
public:
     FrameToken();
     Expr operator()(std::string const &name) const;
};

class GridToken : public BasicToken
{
public:
     GridToken();
     Expr operator()(std::string const &option,
          std::string const &name) const;

     template <typename T>
     Expr operator()(std::string const &option,
          std::string const &name, T const &t) const
     {
          std::string str("grid ");
          str += option; str += " ";
          str += name; str += " ";
          str += toString(t);
          return Expr(str);
     }

     Expr operator()(std::string const &option,
          std::string const &name, int x, int y) const;
     Expr operator()(std::string const &option,
          std::string const &name,
          int col1, int row1, int col2, int row2) const;
};

class LowerToken : public BasicToken
{
public:
     LowerToken();
     Expr operator()(std::string const &name,
          std::string const &belowthis = std::string()) const;
};

class PlaceToken : public BasicToken
{
public:
     PlaceToken();
     Expr operator()(std::string const &w) const;
     Expr operator()(std::string const &option,
          std::string const &w) const;
};

class RadioButtonToken : public BasicToken
{
public:
     RadioButtonToken();
     Expr operator()(std::string const &name) const;
};

class RaiseToken : public BasicToken
{
public:
     RaiseToken();
     Expr operator()(std::string const &name,
          std::string const &abovethis = std::string()) const;
};

class ToplevelToken : public BasicToken
{
public:
     ToplevelToken();
     Expr operator()(std::string const &w) const;
};

class AddToken : public BasicToken
{
public:
     AddToken();
     Expr operator()(std::string const &tn) const;
};

class BboxToken : public BasicToken
{
public:
     BboxToken();
     
     template <typename T>
     Expr operator()(T const &t) const
     {
          std::string str("bbox ");
          str += toString(t);
          return Expr(str);
     }

     template <class InputIterator>
     Expr operator()(InputIterator b, InputIterator e) const
     {
          std::string cmd("bbox");
          if (b == e)
          {
               cmd += " {}";
          }
          for (InputIterator i = b; i != e; ++i)
          {
               cmd += ' ';
               cmd += *i;
          }
     
          return Expr(cmd);
     }
};

class CgetToken : public BasicToken
{
public:
     CgetToken();
     Expr operator()(std::string const &name) const;
};

class ConfigureToken : public BasicToken
{
public:
     ConfigureToken();
     Expr operator()() const;
};

class CreateToken : public BasicToken
{
public:
     CreateToken();
     
     Expr operator()(std::string const &type, int x, int y) const;

     Expr operator()(std::string const &type, Point const &p) const;

     Expr operator()(std::string const &type,
          int x1, int y1, int x2, int y2) const;

     Expr operator()(std::string const &type,
          Point const &p1, Point const &p2) const;

     Expr operator()(std::string const &type, Box const &b) const;

     template <class InputIterator>
     Expr operator()(std::string const &type,
          InputIterator b, InputIterator e) const
     {
          std::string cmd("create ");
          cmd += type;
          if (b == e)
          {
               throw TkError("A non-empty list of coordinates expected");
          }
          for (InputIterator i = b; i != e; ++i)
          {
               cmd += ' ';
               cmd += toString(*i);
          }
     
          return Expr(cmd);
     }
};

class FocusToken : public BasicToken
{
public:
     FocusToken();
     Expr operator()(std::string const &name = std::string()) const;
};

class ForgetToken : public BasicToken
{
public:
     ForgetToken();
     Expr operator()(std::string const &name) const;
};

class GetToken : public BasicToken
{
public:
     GetToken();
     Expr operator()() const;

     template <typename T>
     Expr operator()(T const &t) const
     {
          std::string str("get ");
          str += toString(t);
          return Expr(str);
     }

     template <typename T1, typename T2>
     Expr operator()(T1 const &t1, T2 const &t2) const
     {
          std::string str("get ");
          str += toString(t1); str += " ";
          str += toString(t2);
          return Expr(str);
     }
};

class MoveToToken : public BasicToken
{
public:
     MoveToToken();
     Expr operator()(double fraction) const;
};

class ScrollToken : public BasicToken
{
public:
     ScrollToken();
     Expr operator()(int n, std::string const &what) const;
};

class SetToken : public BasicToken
{
public:
     SetToken();

     Expr operator()() const;
     
     template <typename T>
     Expr operator()(T const &t) const
     {
          std::string str("set ");
          str += toString(t);
          return Expr(str);
     }
     
     Expr operator()(double first, double last) const;
};

class TypeToken : public BasicToken
{
public:
     TypeToken();
     
     template <typename T>
     Expr operator()(T const &item) const
     {
          std::string str("type ");
          str += toString(item);
          return Expr(str);
     }
};

class ValidateToken : public BasicToken
{
public:
     ValidateToken();
     Expr operator()() const;
     Expr operator()(std::string const &when) const;
};

class AllToken : public BasicToken
{
public:
     AllToken();
     Expr operator()() const;
};

class CommandToken : public BasicToken
{
public:
     CommandToken();
     
     template <class Functor> Expr operator()(Functor f) const
     {
          std::string newCmd = addCallback(
               std::shared_ptr<CallbackBase>(
                    new Callback0<Functor>(f)));
     
          std::string str(" -command ");
          str += newCmd;
          return Expr(str, false);
     }

     Expr operator()(std::string const &name) const;
     Expr operator()(CallbackHandle const &handle) const;
};

class ElideToken : public BasicToken
{
public:
     ElideToken();
     Expr operator()() const;
     Expr operator()(bool b) const;
};

class FromToken : public BasicToken
{
public:
     FromToken();
     Expr operator()(int v) const;
     Expr operator()(int x1, int y1, int x2, int y2) const;
};

class ImageToken : public BasicToken
{
public:
     ImageToken();
     Expr operator()() const;
     Expr operator()(std::string const &name) const;
};

class MarkToken : public BasicToken
{
public:
     MarkToken();
     Expr operator()() const;
     Expr operator()(std::string const &option,
          std::string const &markname = std::string(),
          std::string const &dir = std::string()) const;
};

class MenuLabelToken : public BasicToken
{
public:
     MenuLabelToken();
     Expr operator()(std::string const &label) const;
};

class TextToken : public BasicToken
{
public:
     TextToken();
     Expr operator()() const;
     Expr operator()(std::string const &t) const;
};

class ToToken : public BasicToken
{
public:
     ToToken();
     Expr operator()(int val) const;
     Expr operator()(int x, int y) const;
     Expr operator()(int x1, int y1, int x2, int y2) const;
};

class WindowToken : public BasicToken
{
public:
     WindowToken();
     Expr operator()(std::string const &name = std::string()) const;
};

class WndClassToken : public BasicToken
{
public:
     WndClassToken();
     Expr operator()(std::string const &name) const;
};

class AfterToken : public BasicToken
{
public:
     AfterToken();
     Expr operator()(int time) const;
     Expr operator()(std::string const &name) const;
     Expr operator()(int time, std::string const &name) const;
     Expr operator()(std::string const &option, std::string const &id) const;

     template <class Functor>
     Expr operator()(int t, Functor f) const
     {
          std::string newCmd = addCallback(
               std::shared_ptr<CallbackBase>(
                    new Callback0<Functor>(f)));

          std::string str("after ");
          str += toString(t); str += " ";
          str += newCmd;
          return Expr(str);
     }
};

class RGBToken : public BasicToken
{
public:
     RGBToken();
     std::string operator()(int r, int g, int b) const;
};

} 

extern details::BindToken bind;
extern details::CheckButtonToken checkbutton;
extern details::FrameToken frame;
extern details::GridToken grid;
extern details::LowerToken lower;
extern details::PlaceToken place;
extern details::RadioButtonToken radiobutton;
extern details::RaiseToken raise;
extern details::ToplevelToken toplevel;
extern details::AddToken add;
extern details::BboxToken bbox;
extern details::CgetToken cget;
extern details::ConfigureToken configure;
extern details::CreateToken create;
extern details::FocusToken focus;
extern details::ForgetToken forget;
extern details::GetToken get;
extern details::MoveToToken moveto;
extern details::ScrollToken scroll;
extern details::SetToken set;
extern details::TypeToken type;
extern details::ValidateToken validate;
extern details::AllToken all;
extern details::CommandToken command;
extern details::ElideToken elide;
extern details::FromToken from;
extern details::ImageToken image;
extern details::MarkToken mark;
extern details::MenuLabelToken menulabel;
extern details::TextToken text;
extern details::ToToken to;
extern details::WindowToken window;
extern details::WndClassToken wndclass;
extern details::AfterToken after;
extern details::RGBToken rgb;




template <typename T>
std::string at(T const &t)
{
     std::string str("@");
     str += details::toString(t);
     return str;
}


template <typename T1, typename T2>
std::string at(T1 const &t1, T2 const &t2)
{
     std::string str("@");
     str += details::toString(t1); str += ",";
     str += details::toString(t2);
     return str;
}


template <typename T1, typename T2>
std::string txt(T1 const &t1, T2 const &t2)
{
     std::string str;
     str += details::toString(t1); str += ".";
     str += details::toString(t2);
     return str;
}

} 

#endif 
