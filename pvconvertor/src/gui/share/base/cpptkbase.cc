








#include "cpptkbase.h"
#include <tcl.h>
#include <tk.h>
#include <map>
#include <ostream>
#include <iostream>
#include <sstream>
#include <boost/scoped_ptr.hpp>

using namespace Tk;
using namespace Tk::details;
using namespace boost;
using namespace std;

namespace { 

class Interp
{
public:
     Interp()
     {
          interp_ = Tcl_CreateInterp();
	
          int cc = Tcl_Init(interp_);
          if (cc != TCL_OK)
          {
               throw TkError(interp_->result);
          }

          cc = Tk_Init(interp_);
          if (cc != TCL_OK)
          {
               throw TkError(interp_->result);
          }
          
          cc = Tcl_Eval(interp_, "namespace eval CppTk {}");
          if (cc != TCL_OK)
          {
               throw TkError(interp_->result);
          }
     }
     
     ~Interp()
     {
          
          
          
     }
     
     Tcl_Interp * get() const { return interp_; }

private:
     Tcl_Interp * interp_;
};


Tcl_Interp * getInterp()
{
     static Interp interp;
     return interp.get();
}



ostream *dumpstream = &cerr;

void do_eval(string const &str)
{
#ifdef CPPTK_DUMP_COMMANDS
     *dumpstream << str << '\n';
#endif 

#ifndef CPPTK_DONT_EVALUATE
     int cc = Tcl_Eval(getInterp(), (char*)str.c_str());
     if (cc != TCL_OK)
     {
          throw TkError(getInterp()->result);
     }
#endif
}


typedef map<int, std::shared_ptr<CallbackBase> > CallbacksMap;
CallbacksMap callbacks;


int callbackId = 0;

char const *callbackPrefix = "CppTk::callback";

typedef map<int *,    string> IntLinks;
typedef map<double *, string> DoubleLinks;
typedef map<string *, string> StringLinks;

IntLinks intLinks;
DoubleLinks doubleLinks;
StringLinks stringLinks;

typedef map<string *, char *> StringLinkBuffers;
StringLinkBuffers stringLinkBuffers;

int linkId = 0;

char const *linkVarPrefix = "CppTk::variable";


void linkCpptoTcl()
{
     
     
     
     
     for (StringLinks::iterator it = stringLinks.begin();
          it != stringLinks.end(); ++it)
     {
          string *ps = it->first; 

          StringLinkBuffers::iterator itb = stringLinkBuffers.find(ps);
          char *&pb = itb->second; 
          
          if (pb != NULL)
          {
               Tcl_Free(pb);
          }
          pb = Tcl_Alloc(static_cast<unsigned int>(ps->size()) + 1);
          copy(ps->begin(), ps->end(), pb);
          pb[ps->size()] = '\0';
          
          Tcl_UpdateLinkedVar(getInterp(), (char*)it->second.c_str());
     }
     
     
     for (IntLinks::iterator it = intLinks.begin();
          it != intLinks.end(); ++it)
     {
          Tcl_UpdateLinkedVar(getInterp(), (char*)it->second.c_str());
     }
     for (DoubleLinks::iterator it = doubleLinks.begin();
          it != doubleLinks.end(); ++it)
     {
          Tcl_UpdateLinkedVar(getInterp(), (char*)it->second.c_str());
     }
}


void linkTcltoCpp()
{
     
     for (StringLinks::iterator it = stringLinks.begin();
          it != stringLinks.end(); ++it)
     {
          string *ps = it->first; 
          
          StringLinkBuffers::iterator itb = stringLinkBuffers.find(ps);
          char *pb = itb->second; 
          
          if (pb != NULL)
          {
               ps->assign(pb);
          }
          else
          {
               ps->clear();
          }
     }
}

} 



bool Tk::TkError::inTkError = false;




extern "C"
int callbackHandler(ClientData cd, Tcl_Interp *interp,
     int objc, Tcl_Obj *CONST objv[])
{
     int slot = static_cast<int>(reinterpret_cast<size_t>(cd));
     
     CallbacksMap::iterator it = callbacks.find(slot);
     if (it == callbacks.end())
     {
          Tcl_SetResult(interp,
               "Trying to invoke non-existent callback", TCL_STATIC);
          return TCL_ERROR;
     }
     
     try
     {
          
          linkTcltoCpp();
          
          Params p(objc, reinterpret_cast<void*>(
               const_cast<Tcl_Obj **>(objv)));
          it->second->invoke(p);
          
          
          linkCpptoTcl();
     }
     catch (std::exception const &e)
     {
          Tcl_SetResult(interp, const_cast<char*>(e.what()), TCL_VOLATILE);
          return TCL_ERROR;
     }
     return TCL_OK;
}



extern "C"
void callbackDeleter(ClientData cd)
{
     int slot = static_cast<int>(reinterpret_cast<size_t>(cd));
     callbacks.erase(slot);
}

string Tk::details::addCallback(std::shared_ptr<CallbackBase> cb)
{
     int newSlot = callbackId++;
     callbacks[newSlot] = cb;
     
     string newCmd(callbackPrefix);
     newCmd += lexical_cast<string>(newSlot);
     
     Tcl_CreateObjCommand(getInterp(), (char*)newCmd.c_str(),
          callbackHandler, reinterpret_cast<ClientData>(
			static_cast<size_t>(newSlot)),
          callbackDeleter);
     
     return newCmd;
}

string Tk::details::addLinkVar(int &i)
{
     int newLink = linkId++;
     string newLinkVar(linkVarPrefix);
     newLinkVar += lexical_cast<string>(newLink);
     
     int cc = Tcl_LinkVar(getInterp(), (char*)newLinkVar.c_str(),
          reinterpret_cast<char*>(&i), TCL_LINK_INT);
     if (cc != TCL_OK)
     {
          throw TkError(getInterp()->result);
     }
     
     intLinks[&i] = newLinkVar;
     return newLinkVar;
}

string Tk::details::addLinkVar(double &d)
{
     int newLink = linkId++;
     string newLinkVar(linkVarPrefix);
     newLinkVar += lexical_cast<string>(newLink);
     
     int cc = Tcl_LinkVar(getInterp(), (char*)newLinkVar.c_str(),
          reinterpret_cast<char*>(&d), TCL_LINK_DOUBLE);
     if (cc != TCL_OK)
     {
          throw TkError(getInterp()->result);
     }
     
     doubleLinks[&d] = newLinkVar;
     return newLinkVar;
}

string Tk::details::addLinkVar(string &s)
{
     int newLink = linkId++;
     string newLinkVar(linkVarPrefix);
     newLinkVar += lexical_cast<string>(newLink);
     
     pair<map<string *, char *>::iterator, bool> it =
          stringLinkBuffers.insert(make_pair(&s, static_cast<char*>(NULL)));
     
     char *&pb = it.first->second;
     pb = Tcl_Alloc(static_cast<unsigned int>(s.size()) + 1);
     copy(s.begin(), s.end(), pb);
     pb[s.size()] = '\0';
     
     int cc = Tcl_LinkVar(getInterp(), (char*)newLinkVar.c_str(),
          reinterpret_cast<char*>(&it.first->second), TCL_LINK_STRING);
     if (cc != TCL_OK)
     {
          throw TkError(getInterp()->result);
     }
     
     stringLinks[&s] = newLinkVar;
     return newLinkVar;
}

void Tk::details::deleteLinkVar(int &i)
{
     IntLinks::iterator it = intLinks.find(&i);
     if (it == intLinks.end())
     {
          return;
     }
     
     Tcl_UnlinkVar(getInterp(), (char*)it->second.c_str());
     intLinks.erase(it);
}

void Tk::details::deleteLinkVar(double &d)
{
     DoubleLinks::iterator it = doubleLinks.find(&d);
     if (it == doubleLinks.end())
     {
          return;
     }
     
     Tcl_UnlinkVar(getInterp(), (char*)it->second.c_str());
     doubleLinks.erase(it);
}

void Tk::details::deleteLinkVar(string &s)
{
     StringLinks::iterator it = stringLinks.find(&s);
     if (it == stringLinks.end())
     {
          return;
     }
     
     Tcl_UnlinkVar(getInterp(), (char*)it->second.c_str());
     stringLinks.erase(it);
     
     StringLinkBuffers::iterator itb = stringLinkBuffers.find(&s);
     char *pb = itb->second;
     if (pb != NULL)
     {
          Tcl_Free(pb);
     }
     
     stringLinkBuffers.erase(itb);
}

void details::setResult(bool b)
{
     Tcl_SetObjResult(getInterp(), Tcl_NewBooleanObj(b));
}

void details::setResult(long i)
{
     Tcl_SetObjResult(getInterp(), Tcl_NewLongObj(i));
}

void details::setResult(double d)
{
     Tcl_SetObjResult(getInterp(), Tcl_NewDoubleObj(d));
}

void details::setResult(string const &s)
{
     Tcl_SetObjResult(getInterp(),
          Tcl_NewStringObj(s.data(), static_cast<int>(s.size())));
}

int details::getResultLen()
{
     Tcl_Interp *interp = getInterp();
     
     Tcl_Obj *list = Tcl_GetObjResult(interp);
     int len, cc;
     
     cc = Tcl_ListObjLength(interp, list, &len);
     if (cc != TCL_OK)
     {
          throw TkError(interp->result);
     }
     
     return len;
}

template <>
int details::getResultElem<int>(int indx)
{
     Tcl_Interp *interp = getInterp();

     Tcl_Obj *list = Tcl_GetObjResult(interp);
     Tcl_Obj *obj;
     
     int cc = Tcl_ListObjIndex(interp, list, indx, &obj);
     if (cc != TCL_OK)
     {
          throw TkError(interp->result);
     }
     
     int val;
     cc = Tcl_GetIntFromObj(interp, obj, &val);
     if (cc != TCL_OK)
     {
          throw TkError(interp->result);
     }
     
     return val;
}

template <>
double details::getResultElem<double>(int indx)
{
     Tcl_Interp *interp = getInterp();

     Tcl_Obj *list = Tcl_GetObjResult(interp);
     Tcl_Obj *obj;
     
     int cc = Tcl_ListObjIndex(interp, list, indx, &obj);
     if (cc != TCL_OK)
     {
          throw TkError(interp->result);
     }
     
     double val;
     cc = Tcl_GetDoubleFromObj(interp, obj, &val);
     if (cc != TCL_OK)
     {
          throw TkError(interp->result);
     }
     
     return val;
}

template <>
string details::getResultElem<string>(int indx)
{
     Tcl_Interp *interp = getInterp();

     Tcl_Obj *list = Tcl_GetObjResult(interp);
     Tcl_Obj *obj;
     
     int cc = Tcl_ListObjIndex(interp, list, indx, &obj);
     if (cc != TCL_OK)
     {
          throw TkError(interp->result);
     }
     
     return Tcl_GetString(obj);
}

details::Command::Command(std::string const &str, std::string const &postfix)
     : invoked_(false), str_(str), postfix_(postfix)
{
}

details::Command::~Command()
{
     if (!TkError::inTkError)
     {
          invokeOnce();
     }
}

string details::Command::invoke() const
{
     invokeOnce();
     return getInterp()->result;
}

void details::Command::invokeOnce() const
{
     if (invoked_ == false)
     {
          invoked_ = true;
          
          string cmd(str_);
          cmd += postfix_;
          
          do_eval(cmd);
     }
}

details::Expr::Expr(string const &str, bool starter)
{
     if (starter)
     {
          cmd_.reset(new Command(str));
     }
     else
     {
          str_ = str;
     }
}

details::Expr::Expr(string const &str, string const &postfix)
{
     cmd_.reset(new Command(str, postfix));
}

string details::Expr::getValue() const
{
     if (!str_.empty())
     {
          return str_;
     }
     else
     {
          return cmd_->getValue();
     }
}

details::Expr::operator string() const
{
     return cmd_->invoke();
}

details::Expr::operator int() const
{
     cmd_->invokeOnce();
     
     Tcl_Interp *interp = getInterp();
     Tcl_Obj *obj = Tcl_GetObjResult(interp);

     int val, cc;
     cc = Tcl_GetIntFromObj(interp, obj, &val);
     if (cc != TCL_OK)
     {
          throw TkError(interp->result);
     }
     
     return val;
}

details::Expr::operator double() const
{
     cmd_->invokeOnce();
     
     Tcl_Interp *interp = getInterp();
     Tcl_Obj *obj = Tcl_GetObjResult(interp);
     
     double val;
     int cc = Tcl_GetDoubleFromObj(interp, obj, &val);
     if (cc != TCL_OK)
     {
          throw TkError(interp->result);
     }
     
     return val;
}

details::Expr::operator Tk::Point() const
{
     string ret(cmd_->invoke());
     if (ret.empty())
     {
          return Tk::Point(0, 0);
     }

     int len = getResultLen();
     if (len < 2)
     {
          throw TkError("Cannot convert the result list to Point\n");
     }
     
     int x = getResultElem<int>(0);
     int y = getResultElem<int>(1);
     
     return Point(x, y);
}

details::Expr::operator Tk::Box() const
{
     string ret(cmd_->invoke());
     if (ret.empty())
     {
          return Tk::Box(0, 0, 0, 0);
     }
     
     int len = getResultLen();
     if (len < 4)
     {
          throw TkError("Cannot convert the result list to Box\n");
     }
     
     int x1 = getResultElem<int>(0);
     int y1 = getResultElem<int>(1);
     int x2 = getResultElem<int>(2);
     int y2 = getResultElem<int>(3);
     
     return Box(x1, y1, x2, y2);
}




template <>
int details::Params::get<int>(int argno) const
{
     if (argno < 1 || argno >= argc_)
     {
          throw TkError("Parameter number out of valid range");
     }
     
     Tcl_Obj *CONST *objv = reinterpret_cast<Tcl_Obj *CONST *>(objv_);
     
     int res, cc;
     cc = Tcl_GetIntFromObj(getInterp(), objv[argno], &res);
     if (cc != TCL_OK)
     {
          throw TkError(getInterp()->result);
     }
     
     return res;
}

template <>
string details::Params::get<string>(int argno) const
{
     if (argno < 1 || argno >= argc_)
     {
          throw TkError("Parameter number out of valid range");
     }
     
     Tcl_Obj *CONST *objv = reinterpret_cast<Tcl_Obj *CONST *>(objv_);
     
     string res = Tcl_GetString(objv[argno]);
     return res;
}

ostream & details::operator<<(ostream &os, BasicToken const &token)
{
     return os << static_cast<string>(token);
}

namespace { 

void doSingleQuote(string &s, char c)
{
     string::size_type pos = 0;
     while ((pos = s.find(c, pos)) != string::npos)
     {
          s.insert(pos, "\\");
          pos += 2;
     }
}

} 



string details::quote(string const &s)
{
     string ret(s);
     doSingleQuote(ret, '\\');
     doSingleQuote(ret, '\"');
     doSingleQuote(ret, '$');
     doSingleQuote(ret, '[');
     doSingleQuote(ret, ']');
     
     return ret;
}



Expr Tk::operator-(Expr const &lhs, Expr const &rhs)
{
     std::shared_ptr<Command> cmd(lhs.getCmd());
     cmd->append(rhs.getValue());
     
     return Expr(cmd);
}

Expr Tk::operator<<(string const &w, Expr const &rhs)
{
     std::shared_ptr<Command> cmd(rhs.getCmd());
     cmd->prepend(" ");
     cmd->prepend(w);

     return Expr(cmd);
}



void Tk::deleteCallback(string const &name)
{
     string::size_type pos = name.find_first_not_of(callbackPrefix);
     if (pos == string::npos) return;
     
     int slot = lexical_cast<int>(name.substr(pos, name.size()));
     callbacks.erase(slot);
     
     int cc = Tcl_DeleteCommand(getInterp(), (char*)name.c_str());
     if (cc != TCL_OK)
     {
          throw TkError(getInterp()->result);
     }
}

Tk::CallbackHandle::CallbackHandle(string const &name) : name_(name) {}

Tk::CallbackHandle::~CallbackHandle() { deleteCallback(name_); }

Expr Tk::eval(string const &str)
{
     return Expr(str);
}

void Tk::init(char *argv0)
{
	Tcl_FindExecutable(argv0);
}

void Tk::runEventLoop()
{
     
     linkCpptoTcl();
     
     Tk_MainLoop();
}

void Tk::setDumpStream(ostream &os)
{
	dumpstream = &os;
}
