








#ifndef CPPTKBASE_H_INCLUDED
#define CPPTKBASE_H_INCLUDED

# if defined _MSC_VER
#   if (_MSC_VER >= 1300)
#     pragma warning (disable : 4127 4511 4512 4701)
#   endif
# endif

#include <stdexcept>
#include <string>
#include <utility>
#include <sstream>
#include <vector>
#include <memory>
#include <boost/lexical_cast.hpp>
#include <iosfwd>

namespace Tk
{



class TkError : public std::runtime_error
{
public:
     explicit TkError(std::string const &msg)
          : std::runtime_error(msg)
     {
          inTkError = true;
     }

     ~TkError() throw()
     {
          inTkError = false;
     }

     static bool inTkError;
};


struct Point
{
     Point(int a, int b) : x(a), y(b) {}
     int x, y;
};

struct Box
{
     Box(int a, int b, int c, int d) : x1(a), y1(b), x2(c), y2(d) {}
     int x1, y1, x2, y2;
};






template <class Functor>
struct CallbackTraits
{
     typedef typename Functor::result_type result_type;
};



template <typename R>
struct CallbackTraits<R (*)()>
{
     typedef R result_type;
};

template <typename R, typename T1>
struct CallbackTraits<R (*)(T1)>
{
     typedef R result_type;
};

template <typename R, typename T1, typename T2>
struct CallbackTraits<R (*)(T1, T2)>
{
     typedef R result_type;
};

template <typename R, typename T1, typename T2, typename T3>
struct CallbackTraits<R (*)(T1, T2, T3)>
{
     typedef R result_type;
};

template <typename R, typename T1, typename T2, typename T3, typename T4>
struct CallbackTraits<R (*)(T1, T2, T3, T4)>
{
     typedef R result_type;
};

template <typename R, typename T1, typename T2, typename T3, typename T4,
     typename T5>
struct CallbackTraits<R (*)(T1, T2, T3, T4, T5)>
{
     typedef R result_type;
};

template <typename R, typename T1, typename T2, typename T3, typename T4,
     typename T5, typename T6>
struct CallbackTraits<R (*)(T1, T2, T3, T4, T5, T6)>
{
     typedef R result_type;
};

template <typename R, typename T1, typename T2, typename T3, typename T4,
     typename T5, typename T6, typename T7>
struct CallbackTraits<R (*)(T1, T2, T3, T4, T5, T6, T7)>
{
     typedef R result_type;
};

template <typename R, typename T1, typename T2, typename T3, typename T4,
     typename T5, typename T6, typename T7, typename T8>
struct CallbackTraits<R (*)(T1, T2, T3, T4, T5, T6, T7, T8)>
{
     typedef R result_type;
};

template <typename R, typename T1, typename T2, typename T3, typename T4,
     typename T5, typename T6, typename T7, typename T8, typename T9>
struct CallbackTraits<R (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9)>
{
     typedef R result_type;
};

template <typename R, typename T1, typename T2, typename T3, typename T4,
     typename T5, typename T6, typename T7, typename T8, typename T9,
     typename T10>
struct CallbackTraits<R (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)>
{
     typedef R result_type;
};

namespace details
{






class Command
{
public:
     explicit Command(std::string const &str,
          std::string const &posfix = std::string());
     ~Command();
     
     std::string invoke() const;
     void append(std::string const &str) { str_ += str; }
     void prepend(std::string const &str) { str_.insert(0, str); }
     std::string getValue() const { return str_; }
     
     void invokeOnce() const;

private:
     
     mutable bool invoked_;
     std::string str_;
     std::string postfix_;
};


int getResultLen();


template <typename T> T getResultElem(int indx);


template <> int         getResultElem<int>(int indx);
template <> double      getResultElem<double>(int indx);
template <> std::string getResultElem<std::string>(int indx);







class Expr
{
public:
     explicit Expr(std::string const &str, bool starter = true);
     Expr(std::string const &str, std::string const &postfix);
     Expr(std::shared_ptr<Command> const &cmd) : cmd_(cmd) {}
     
     std::shared_ptr<Command> getCmd() const { return cmd_; }
     std::string getValue() const;
     
     operator std::string() const;
     operator int() const;
     operator double() const;
     operator Tk::Point() const;
     operator Tk::Box() const;
     
     template <typename T1, typename T2>
     operator std::pair<T1, T2>() const
     {
          std::string ret(cmd_->invoke());
          if (ret.empty())
          {
               return std::make_pair(T1(), T2());
          }

          int len = getResultLen();
          if (len < 2)
          {
               throw TkError("Cannot convert the result list into pair\n");
          }
          
          return std::make_pair(getResultElem<T1>(0), getResultElem<T2>(1));
     }
     
     template <typename T>
     operator std::vector<T>() const
     {
          std::string ret(cmd_->invoke());
          if (ret.empty())
          {
               return std::vector<T>();
          }
          
          int len = getResultLen();
          std::vector<T> v;
          v.reserve(len);
          for(int i = 0; i != len; ++i)
          {
               v.push_back(getResultElem<T>(i));
          }
          
          return v;
     }
     
private:
     std::string str_;
     std::shared_ptr<Command> cmd_;
};





class Params
{
public:
     Params(int argc, void *objv) : argc_(argc), objv_(objv) {}
     
     template <typename T> T get(int argno) const;

private:
     int argc_;
     void *objv_;     
};


template <> int         Params::get<int>(int argno) const;
template <> std::string Params::get<std::string>(int argno) const;





class CallbackBase
{
public:
     virtual ~CallbackBase() {}
     virtual void invoke(Params const &) = 0;
};

std::string addCallback(std::shared_ptr<CallbackBase> cb);


void setResult(bool b);
void setResult(long i);
void setResult(double d);
void setResult(std::string const &s);






template <typename R, class Functor>
struct Dispatch0
{
     static void doDispatch(Functor f)
     {
          R result = f();
          setResult(result);
     }
};


template <class Functor>
struct Dispatch0<void, Functor>
{
     static void doDispatch(Functor f)
     {
          f();
     }
};

template <typename R, class Functor, typename T1>
struct Dispatch1
{
     static void doDispatch(Functor f, T1 const &t1)
     {
          R result = f(t1);
          setResult(result);
     }
};


template <class Functor, typename T1>
struct Dispatch1<void, Functor, T1>
{
     static void doDispatch(Functor f, T1 const &t1)
     {
          f(t1);
     }
};

template <typename R, class Functor, typename T1, typename T2>
struct Dispatch2
{
     static void doDispatch(Functor f, T1 const &t1, T2 const &t2)
     {
          R result = f(t1, t2);
          setResult(result);
     }
};


template <class Functor, typename T1, typename T2>
struct Dispatch2<void, Functor, T1, T2>
{
     static void doDispatch(Functor f, T1 const &t1, T2 const &t2)
     {
          f(t1, t2);
     }
};

template <typename R, class Functor, typename T1, typename T2, typename T3>
struct Dispatch3
{
     static void doDispatch(Functor f, T1 const &t1, T2 const &t2,
          T3 const &t3)
     {
          R result = f(t1, t2, t3);
          setResult(result);
     }
};


template <class Functor, typename T1, typename T2, typename T3>
struct Dispatch3<void, Functor, T1, T2, T3>
{
     static void doDispatch(Functor f, T1 const &t1, T2 const &t2,
          T3 const &t3)
     {
          f(t1, t2, t3);
     }
};

template <typename R, class Functor, typename T1, typename T2, typename T3,
     typename T4>
struct Dispatch4
{
     static void doDispatch(Functor f, T1 const &t1, T2 const &t2,
          T3 const &t3, T4 const &t4)
     {
          R result = f(t1, t2, t3, t4);
          setResult(result);
     }
};


template <class Functor, typename T1, typename T2, typename T3, typename T4>
struct Dispatch4<void, Functor, T1, T2, T3, T4>
{
     static void doDispatch(Functor f, T1 const &t1, T2 const &t2,
          T3 const &t3, T4 const &t4)
     {
          f(t1, t2, t3, t4);
     }
};

template <typename R, class Functor, typename T1, typename T2, typename T3,
     typename T4, typename T5>
struct Dispatch5
{
     static void doDispatch(Functor f, T1 const &t1, T2 const &t2,
          T3 const &t3, T4 const &t4, T5 const &t5)
     {
          R result = f(t1, t2, t3, t4, t5);
          setResult(result);
     }
};


template <class Functor, typename T1, typename T2, typename T3, typename T4,
     typename T5>
struct Dispatch5<void, Functor, T1, T2, T3, T4, T5>
{
     static void doDispatch(Functor f, T1 const &t1, T2 const &t2,
          T3 const &t3, T4 const &t4, T5 const &t5)
     {
          f(t1, t2, t3, t4, t5);
     }
};

template <typename R, class Functor, typename T1, typename T2, typename T3,
     typename T4, typename T5, typename T6>
struct Dispatch6
{
     static void doDispatch(Functor f, T1 const &t1, T2 const &t2,
          T3 const &t3, T4 const &t4, T5 const &t5, T6 const &t6)
     {
          R result = f(t1, t2, t3, t4, t5, t6);
          setResult(result);
     }
};


template <class Functor, typename T1, typename T2, typename T3, typename T4,
     typename T5, typename T6>
struct Dispatch6<void, Functor, T1, T2, T3, T4, T5, T6>
{
     static void doDispatch(Functor f, T1 const &t1, T2 const &t2,
          T3 const &t3, T4 const &t4, T5 const &t5, T6 const &t6)
     {
          f(t1, t2, t3, t4, t5, t6);
     }
};

template <typename R, class Functor, typename T1, typename T2, typename T3,
     typename T4, typename T5, typename T6, typename T7>
struct Dispatch7
{
     static void doDispatch(Functor f, T1 const &t1, T2 const &t2,
          T3 const &t3, T4 const &t4, T5 const &t5, T6 const &t6,
          T7 const &t7)
     {
          R result = f(t1, t2, t3, t4, t5, t6, t7);
          setResult(result);
     }
};


template <class Functor, typename T1, typename T2, typename T3, typename T4,
     typename T5, typename T6, typename T7>
struct Dispatch7<void, Functor, T1, T2, T3, T4, T5, T6, T7>
{
     static void doDispatch(Functor f, T1 const &t1, T2 const &t2,
          T3 const &t3, T4 const &t4, T5 const &t5, T6 const &t6,
          T7 const &t7)
     {
          f(t1, t2, t3, t4, t5, t6, t7);
     }
};

template <typename R, class Functor, typename T1, typename T2, typename T3,
     typename T4, typename T5, typename T6, typename T7, typename T8>
struct Dispatch8
{
     static void doDispatch(Functor f, T1 const &t1, T2 const &t2,
          T3 const &t3, T4 const &t4, T5 const &t5, T6 const &t6,
          T7 const &t7, T8 const &t8)
     {
          R result = f(t1, t2, t3, t4, t5, t6, t7, t8);
          setResult(result);
     }
};


template <class Functor, typename T1, typename T2, typename T3, typename T4,
     typename T5, typename T6, typename T7, typename T8>
struct Dispatch8<void, Functor, T1, T2, T3, T4, T5, T6, T7, T8>
{
     static void doDispatch(Functor f, T1 const &t1, T2 const &t2,
          T3 const &t3, T4 const &t4, T5 const &t5, T6 const &t6,
          T7 const &t7, T8 const &t8)
     {
          f(t1, t2, t3, t4, t5, t6, t7, t8);
     }
};

template <typename R, class Functor, typename T1, typename T2, typename T3,
     typename T4, typename T5, typename T6, typename T7, typename T8,
     typename T9>
struct Dispatch9
{
     static void doDispatch(Functor f, T1 const &t1, T2 const &t2,
          T3 const &t3, T4 const &t4, T5 const &t5, T6 const &t6,
          T7 const &t7, T8 const &t8, T9 const &t9)
     {
          R result = f(t1, t2, t3, t4, t5, t6, t7, t8, t9);
          setResult(result);
     }
};


template <class Functor, typename T1, typename T2, typename T3, typename T4,
     typename T5, typename T6, typename T7, typename T8, typename T9>
struct Dispatch9<void, Functor, T1, T2, T3, T4, T5, T6, T7, T8, T9>
{
     static void doDispatch(Functor f, T1 const &t1, T2 const &t2,
          T3 const &t3, T4 const &t4, T5 const &t5, T6 const &t6,
          T7 const &t7, T8 const &t8, T9 const &t9)
     {
          f(t1, t2, t3, t4, t5, t6, t7, t8, t9);
     }
};

template <typename R, class Functor, typename T1, typename T2, typename T3,
     typename T4, typename T5, typename T6, typename T7, typename T8,
     typename T9, typename T10>
struct Dispatch10
{
     static void doDispatch(Functor f, T1 const &t1, T2 const &t2,
          T3 const &t3, T4 const &t4, T5 const &t5, T6 const &t6,
          T7 const &t7, T8 const &t8, T9 const &t9, T10 const &t10)
     {
          R result = f(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10);
          setResult(result);
     }
};


template <class Functor, typename T1, typename T2, typename T3, typename T4,
     typename T5, typename T6, typename T7, typename T8, typename T9,
     typename T10>
struct Dispatch10<void, Functor, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>
{
     static void doDispatch(Functor f, T1 const &t1, T2 const &t2,
          T3 const &t3, T4 const &t4, T5 const &t5, T6 const &t6,
          T7 const &t7, T8 const &t8, T9 const &t9, T10 const &t10)
     {
          f(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10);
     }
};





template <class Functor>
class Callback0 : public CallbackBase
{
public:
     Callback0(Functor f) : f_(f) {}
     
     virtual void invoke(Params const &)
     {
          Dispatch0<result_type, Functor>::doDispatch(f_);
     }

private:
     typedef typename CallbackTraits<Functor>::result_type result_type;
     Functor f_;
};

template <class Functor, typename T1>
class Callback1 : public CallbackBase
{
public:
     Callback1(Functor f) : f_(f) {}
     
     virtual void invoke(Params const &p)
     {
          Dispatch1<result_type, Functor, T1>
               ::doDispatch(f_, p.template get<T1>(1));
     }

private:
     typedef typename CallbackTraits<Functor>::result_type result_type;
     Functor f_;
};

template <class Functor, typename T1, typename T2>
class Callback2 : public CallbackBase
{
public:
     Callback2(Functor f) : f_(f) {}
     
     virtual void invoke(Params const &p)
     {
          Dispatch2<result_type, Functor, T1, T2>
               ::doDispatch(f_,
                    p.template get<T1>(1),
                    p.template get<T2>(2));
     }

private:
     typedef typename CallbackTraits<Functor>::result_type result_type;
     Functor f_;
};

template <class Functor, typename T1, typename T2, typename T3>
class Callback3 : public CallbackBase
{
public:
     Callback3(Functor f) : f_(f) {}
     
     virtual void invoke(Params const &p)
     {
          Dispatch3<result_type, Functor, T1, T2, T3>
               ::doDispatch(f_,
                    p.template get<T1>(1),
                    p.template get<T2>(2),
                    p.template get<T3>(3));
     }

private:
     typedef typename CallbackTraits<Functor>::result_type result_type;
     Functor f_;
};

template <class Functor, typename T1, typename T2, typename T3, typename T4>
class Callback4 : public CallbackBase
{
public:
     Callback4(Functor f) : f_(f) {}
     
     virtual void invoke(Params const &p)
     {
          Dispatch4<result_type, Functor, T1, T2, T3, T4>
               ::doDispatch(f_,
                    p.template get<T1>(1),
                    p.template get<T2>(2),
                    p.template get<T3>(3),
                    p.template get<T4>(4));
     }

private:
     typedef typename CallbackTraits<Functor>::result_type result_type;
     Functor f_;
};

template <class Functor, typename T1, typename T2, typename T3, typename T4,
     typename T5>
class Callback5 : public CallbackBase
{
public:
     Callback5(Functor f) : f_(f) {}
     
     virtual void invoke(Params const &p)
     {
          Dispatch5<result_type, Functor, T1, T2, T3, T4, T5>
               ::doDispatch(f_,
                    p.template get<T1>(1),
                    p.template get<T2>(2),
                    p.template get<T3>(3),
                    p.template get<T4>(4),
                    p.template get<T5>(5));
     }

private:
     typedef typename CallbackTraits<Functor>::result_type result_type;
     Functor f_;
};

template <class Functor, typename T1, typename T2, typename T3, typename T4,
     typename T5, typename T6>
class Callback6 : public CallbackBase
{
public:
     Callback6(Functor f) : f_(f) {}
     
     virtual void invoke(Params const &p)
     {
          Dispatch6<result_type, Functor, T1, T2, T3, T4, T5, T6>
               ::doDispatch(f_,
                    p.template get<T1>(1),
                    p.template get<T2>(2),
                    p.template get<T3>(3),
                    p.template get<T4>(4),
                    p.template get<T5>(5),
                    p.template get<T6>(6));
     }

private:
     typedef typename CallbackTraits<Functor>::result_type result_type;
     Functor f_;
};

template <class Functor, typename T1, typename T2, typename T3, typename T4,
     typename T5, typename T6, typename T7>
class Callback7 : public CallbackBase
{
public:
     Callback7(Functor f) : f_(f) {}
     
     virtual void invoke(Params const &p)
     {
          Dispatch7<result_type, Functor, T1, T2, T3, T4, T5, T6, T7>
               ::doDispatch(f_,
                    p.template get<T1>(1),
                    p.template get<T2>(2),
                    p.template get<T3>(3),
                    p.template get<T4>(4),
                    p.template get<T5>(5),
                    p.template get<T6>(6),
                    p.template get<T7>(7));
     }

private:
     typedef typename CallbackTraits<Functor>::result_type result_type;
     Functor f_;
};

template <class Functor, typename T1, typename T2, typename T3, typename T4,
     typename T5, typename T6, typename T7, typename T8>
class Callback8 : public CallbackBase
{
public:
     Callback8(Functor f) : f_(f) {}
     
     virtual void invoke(Params const &p)
     {
          Dispatch8<result_type, Functor, T1, T2, T3, T4, T5, T6, T7, T8>
               ::doDispatch(f_,
                    p.template get<T1>(1),
                    p.template get<T2>(2),
                    p.template get<T3>(3),
                    p.template get<T4>(4),
                    p.template get<T5>(5),
                    p.template get<T6>(6),
                    p.template get<T7>(7),
                    p.template get<T8>(8));
     }

private:
     typedef typename CallbackTraits<Functor>::result_type result_type;
     Functor f_;
};

template <class Functor, typename T1, typename T2, typename T3, typename T4,
     typename T5, typename T6, typename T7, typename T8, typename T9>
class Callback9 : public CallbackBase
{
public:
     Callback9(Functor f) : f_(f) {}
     
     virtual void invoke(Params const &p)
     {
          Dispatch9<result_type, Functor, T1, T2, T3, T4, T5, T6, T7, T8, T9>
               ::doDispatch(f_,
                    p.template get<T1>(1),
                    p.template get<T2>(2),
                    p.template get<T3>(3),
                    p.template get<T4>(4),
                    p.template get<T5>(5),
                    p.template get<T6>(6),
                    p.template get<T7>(7),
                    p.template get<T8>(8),
                    p.template get<T9>(9));
     }

private:
     typedef typename CallbackTraits<Functor>::result_type result_type;
     Functor f_;
};

template <class Functor, typename T1, typename T2, typename T3, typename T4,
     typename T5, typename T6, typename T7, typename T8, typename T9,
     typename T10>
class Callback10 : public CallbackBase
{
public:
     Callback10(Functor f) : f_(f) {}
     
     virtual void invoke(Params const &p)
     {
          Dispatch10<result_type, Functor,
               T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>
               ::doDispatch(f_,
                    p.template get<T1>(1),
                    p.template get<T2>(2),
                    p.template get<T3>(3),
                    p.template get<T4>(4),
                    p.template get<T5>(5),
                    p.template get<T6>(6),
                    p.template get<T7>(7),
                    p.template get<T8>(8),
                    p.template get<T9>(9),
                    p.template get<T10>(10));
     }

private:
     typedef typename CallbackTraits<Functor>::result_type result_type;
     Functor f_;
};

std::string addLinkVar(int &i);
std::string addLinkVar(double &d);
std::string addLinkVar(std::string &s);
void deleteLinkVar(int &i);
void deleteLinkVar(double &d);
void deleteLinkVar(std::string &s);



template <typename T>
inline std::string toString(T const &t)
{ return boost::lexical_cast<std::string>(t); }

inline std::string toString(std::string const &str) { return str; }
inline std::string toString(char const *str) { return str; }



std::string quote(std::string const &s);

class BasicToken
{
public:
     BasicToken(std::string const &n) : name_(n) {}
     operator std::string() const { return name_; }
     
protected:
     std::string name_;
};

std::ostream & operator<<(std::ostream &os, BasicToken const &token);





class Option : public details::BasicToken
{
public:
     explicit Option(char const *name, bool quote = false)
          : BasicToken(name), quote_(quote) {}
     
     template <typename T>
     Expr operator()(T const &t) const
     {
          std::string str(" -");
          str += name_;  str += " ";
          str += (quote_ ? "\"" : "");
          str += toString(t);
          str += (quote_ ? "\"" : "");
          return Expr(str, false);
     }
     
private:
     bool quote_;
};



template <typename T>
class SubstAttr
{
public:
     SubstAttr(std::string const &spec) : spec_(spec) {}
     
     std::string get() const { return spec_; }

private:
     std::string spec_;
};

template <typename T>
class EventAttr : public SubstAttr<T>
{
public:
     typedef T attrType;
     
     EventAttr(std::string const &spec) : SubstAttr<T>(spec) {}
};

template <typename T>
class ValidateAttr : public SubstAttr<T>
{
public:
     typedef T validType;
     
     ValidateAttr(std::string const &spec) : SubstAttr<T>(spec) {}
};

} 



details::Expr operator-(details::Expr const &lhs, details::Expr const &rhs);
details::Expr operator<<(std::string const &w, details::Expr const &rhs);


template <class Functor> std::string callback(Functor f)
{
     return details::addCallback(
          std::shared_ptr<details::CallbackBase>(
               new details::Callback0<Functor>(f)));
}


void deleteCallback(std::string const &name);


class CallbackHandle
{
public:
     explicit CallbackHandle(std::string const &name);
     ~CallbackHandle();
     
     std::string const & get() const { return name_; }
     
private:
     std::string name_;
};


template <typename T> std::string linkVar(T &t)
{
     return details::addLinkVar(t);
}


template <typename T> void unLinkVar(T &t) { details::deleteLinkVar(t); }


template <typename T>
class LinkHandle
{
public:
     explicit LinkHandle(T &t) :t_(t) { var_ = details::addLinkVar(t); }
     ~LinkHandle() { details::deleteLinkVar(t_); }
     
     std::string const & get() const { return var_; }
     
private:
     T &t_;
     std::string var_;
};


details::Expr eval(std::string const &str);


void init(char *argv0);


void runEventLoop();


void setDumpStream(std::ostream &os);

} 

#endif 
