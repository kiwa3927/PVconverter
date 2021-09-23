#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>
#include <cassert>
#include <string.h>
#include <unistd.h>
using namespace std;

extern const char *g_viva_mapping;


namespace viva {





const string g_splite_key = "|#|";

namespace viva_util {
void
trimBegin(std::string &str, const char *pTrim = " \t\r\n")
{
    std::string::size_type nIndex = str.find_first_not_of(pTrim, 0);
    if(nIndex == std::string::npos)
    {
        str.clear();
    }
    else
    {
        str.erase(0, nIndex);
    }
}

void
trimEnd(std::string &str, const char *pTrim = " \t\r\n")
{
    std::string::size_type nIndex = str.find_last_not_of(pTrim);
    if(nIndex != std::string::npos)
    {
        str.erase(nIndex + 1, str.length());
    }
}

inline void
trim(std::string &str, const char *pTrim = " \t\r\n")
{
    trimBegin(str, pTrim);
    trimEnd(str, pTrim);
}

inline void
toUpper(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

template<class Container>
void tokenize(Container &v, const std::string &str,
                     const char *pToken = " \t\r\n", const bool addEmpty = false,
                     const bool needTrim = true)
{
    if(str.empty())
    {
        return;
    }

    std::string::size_type pre_index = 0, index = 0;
    while((index = str.find_first_of(pToken, pre_index)) != std::string::npos)
    {
        if(index != 0)
        {
            std::string element = str.substr(pre_index, index - pre_index);
            if (needTrim) trim(element);

            if((!addEmpty && !element.empty()) || addEmpty)
                v.push_back(str.substr(pre_index, index - pre_index));
        }
        pre_index = index + 1;
    }

    std::string element = str.substr(pre_index);
    if((!addEmpty && !element.empty()) || addEmpty)
        v.push_back(element);
}

bool parseFileName(const std::string &sFileName, std::string &sValue)
{
    std::vector<std::string> v;
    if(sFileName[0] == '/')
    {
        sValue += "/";
    }
    tokenize(v, sFileName, "/", false, false);
    for(std::vector<std::string>::iterator iter = v.begin();
        iter != v.end(); ++iter)
    {
        if(iter == v.begin() && sFileName[0] == '~' && v.size() > 1 &&
           !(iter->empty()) && (*iter)[0] == '~')
        {
            if(*iter == "~")
            {
                const char *pValue = getenv("HOME");
                assert(pValue != NULL);
                if(pValue != NULL)
                    sValue += pValue;
            }
            else
            {
                sValue += "/home/";
                sValue += iter->substr(1);
            }
        }
        else if((*iter)[0] != '$')
        {
            sValue += *iter;
        }
        else
        {
            std::string sVarName;
            sVarName = iter->substr(1);
            const char *pValue = getenv(sVarName.c_str());
            if(pValue == NULL || strlen(pValue) == 0)
            {
                sValue = sVarName;
                return false;
            }
            sValue += pValue;
        }
        sValue += "/";
    }
    sValue.erase(--sValue.end());
    return true;
}

bool isEmptyLine(const string &line)
{
    for (string::const_iterator it=line.begin(); it!=line.end(); ++it)
    {
        if (!std::isspace(*it))
            return false;
    }
    return true;
}

inline
void skipWS(const char * &p)
{
    while (*p != '\0' && std::isspace(*p))
        ++p;
}

bool readAllFile(vector<string> &vec, const char *file)
{
    ifstream in(file);
    if (!in)
    {
        cerr << "cannot open file!" <<endl;
        return false;
    }

    string line;
    while (getline(in, line))
    {
        vec.push_back(line);
    }
    return true;
}

}

using namespace viva_util;

namespace viva_enc {
}

using namespace viva_enc;

namespace viva_pattern {

struct MapTable
{
    string m_svrf;
    string m_pvrs;

    void prehandle()
    {
        trim(m_svrf);
        trim(m_pvrs);
        toUpper(m_svrf);
    }
};

bool spliteMapping(vector<MapTable> &pv, const vector<string> &vec)
{
    for(vector<string>::const_iterator it=vec.begin(); it!=vec.end(); ++it)
    {

        const string &str = *it;
        if (isEmptyLine(str))
            continue;

        string::size_type index = str.find(',');
        if (index == string::npos)
        {
            cerr << "can not find \',\' in " << str <<endl;
            return false;
        }

        MapTable mt;
        mt.m_svrf.assign(str.c_str(), str.c_str() + index);
        mt.m_pvrs.assign(str.c_str() + index + 1, str.c_str() + str.size());
        mt.prehandle();
        pv.push_back(mt);
    }
    return true;
}

struct Patterner
{
    vector<string> m_words;
    string m_pvrs;

    bool set(const MapTable &mt)
    {
        tokenize(m_words, mt.m_svrf);
        m_pvrs = mt.m_pvrs;

        vector<string> v;
        tokenize(v, m_pvrs);
        if (v.size() != 1)
            return false;

        return true;
    }

    string toString() const
    {
        string line;
        for (vector<string>::const_iterator it=m_words.begin(); it!=m_words.end(); )
        {
            line += *it;
            if (++it != m_words.end())
                line += ' ';
            else
                break;
        }
        line += ',';
        line += m_pvrs;
        return line;
    }

    bool equal(const Patterner &p) const
    {
        return m_words == p.m_words && m_pvrs == p.m_pvrs;
    }
    bool less(const Patterner &p) const
    {
        return m_words < p.m_words;
    }
};

bool operator <(const Patterner &p1, const Patterner &p2)
{
    return p1.less(p2);
}

bool isEqualPattern(const vector<Patterner> &v1, const vector<Patterner> &v2)
{
    if (v1.size() != v2.size())
    {
        cerr << "size not equal" <<endl;
        return false;
    }

    for (size_t i=0; i<v1.size(); ++i)
    {
        const Patterner &p1 = v1[i];
        const Patterner &p2 = v2[i];
        if (!p1.equal(p2))
        {
            cerr << p1.toString() << " #!=# "<<p2.toString() <<endl;
            return false;
        }
    }
    return true;
}

bool getPatternFromVec(vector<Patterner> &pvec, const vector<string> &svec)
{
    vector<MapTable> mp;
    if (false == spliteMapping(mp, svec))
        return false;

    for (vector<MapTable>::const_iterator it=mp.begin(); it!=mp.end(); ++it)
    {
        Patterner p;
        if (false == p.set(*it))
        {
            cerr << "pattern error : " << p.m_pvrs << endl;
            return false;
        }
        pvec.push_back(p);
    }
    return true;
}

string patternVec2String(const vector<Patterner> &vec)
{
    string str;
    for (vector<Patterner>::const_iterator it=vec.begin(); it!=vec.end(); ++it)
    {
        const Patterner &p = *it;
        str += p.toString() + g_splite_key;
    }
    return str;
}

bool string2PatternVec(vector<Patterner> &vec, const string &buffer)
{
    vector<string> lineVec;
    string::size_type beg = 0;
    do
    {
        string::size_type end = buffer.find(g_splite_key, beg);
        if (end == string::npos)
            break;

        lineVec.push_back(buffer.substr(beg, end - beg));
        beg = end + g_splite_key.size();
    }while (true);

    return getPatternFromVec(vec, lineVec);
}

}

using namespace viva_pattern;

namespace viva_matcher {

struct OneWord
{
    bool m_bSpace;
    string m_cnt;
    OneWord() : m_bSpace(true) {}
};

typedef list<OneWord> Words_Stream;

string merge_vector(const vector<string> &vec)
{
    string str;
    vector<string>::const_iterator it = vec.begin();
    while (it != vec.end())
    {
        str += *it;
        ++it;
        if (it != vec.end())
            str += ' ';
    }

    return str;
}

Words_Stream splite_line(const string &line)
{
    Words_Stream words;

    const char *pStart = line.c_str();
    const char *pEnd = pStart + line.size();

    while (pStart != pEnd)
    {
        const char *pBeg = pStart;
        bool isSpace = std::isspace(*pStart);
        while (++pStart != pEnd &&
               isSpace == static_cast<bool>(std::isspace(*pStart)))
            ;

        OneWord t;
        t.m_bSpace = isSpace;
        t.m_cnt.assign(pBeg, pStart);
        words.push_back(t);
    }

    return words;
}

string merge_token(const Words_Stream &words)
{
    string line;
    for (Words_Stream::const_iterator it=words.begin(); it!=words.end(); ++it)
    {
        const OneWord &t = *it;
        line += t.m_cnt;
    }
    return line;
}

void handleFirstSpace(string &spaceHead, Words_Stream &all)
{
    if (all.empty())
        return;

    Words_Stream::iterator spaceIter = all.begin();

    while (spaceIter != all.end())
    {
        const OneWord &t = *spaceIter;
        if (false == t.m_bSpace)
            break;

        spaceHead += t.m_cnt;
        ++spaceIter;
    }

    all.erase(all.begin(), spaceIter);
}

void skipSpaceWord(Words_Stream &head, Words_Stream &tail)
{
    if (tail.empty())
        return;

    Words_Stream::iterator it = tail.begin();
    while (it != tail.end())
    {
        const OneWord &t = *it;
        if (t.m_bSpace)
            ++it;
        else
            break;
    }
    head.splice(head.end(), tail, tail.begin(), it);
}

void backSkipSpaceWord(Words_Stream &head, Words_Stream &tail)
{
    if (head.empty())
        return;

    Words_Stream::reverse_iterator rit = head.rbegin();
    while (rit != head.rend())
    {
        const OneWord &t = *rit;
        if (t.m_bSpace)
            ++rit;
        else
            break;
    }
    tail.splice(tail.begin(), head, rit.base(), head.end());
}

bool lessOneWord(Words_Stream &head, Words_Stream &tail, vector<string> &words)
{
    if (words.empty())
        return false;

    words.pop_back();

    assert(false == head.empty());
    backSkipSpaceWord(head, tail);

    if (head.empty())
    {
        assert(false);
        return false;
    }

    Words_Stream::iterator it = --head.end();
    assert(it->m_bSpace == false);
    tail.splice(tail.begin(), head, it);

    backSkipSpaceWord(head, tail);
    return true;
}

bool moreOneWord(Words_Stream &header, Words_Stream &tail, vector<string> &words)
{
    skipSpaceWord(header, tail);

    if (tail.empty())
        return false;

    Words_Stream::iterator it = tail.begin();
    assert(it->m_bSpace == false);

    string w = it->m_cnt;
    toUpper(w);
    words.push_back(w);
    header.splice(header.end(), tail, it);

    return true;
}



class RuleMapper_MaxWords
{
    typedef map<vector<string>, const Patterner*> MAP;

public:
    bool setPatternVec(const vector<Patterner> &vec)
    {
        m_vPatterns = vec;
        for (vector<Patterner>::const_iterator it=m_vPatterns.begin(); it!=m_vPatterns.end(); ++it)
        {
            const Patterner &pattern = *it;
            string svrf = merge_vector(pattern.m_words);
            bool b1 = m_prvs2svrf.insert(make_pair(pattern.m_pvrs, svrf)).second;
            if (!b1)
            {
                cerr << "value conflict : " << pattern.m_pvrs <<endl;
                return false;
            }

            size_t wordSize = pattern.m_words.size();
            assert(wordSize >= 1);
            if (m_mapping.size() < wordSize)
                m_mapping.resize(wordSize);

            MAP &s = m_mapping[wordSize - 1];
            bool b2 = s.insert(make_pair(pattern.m_words, &pattern)).second;
            if (!b2)
            {
                cerr << "key conflict : " << svrf <<endl;
                return false;
            }
        }

        if (m_mapping.empty())
            return false;

        return true;
    }

    string recover(const string &line) const
    {
        Words_Stream words = splite_line(line);

        bool hasRecover = false;
        for (Words_Stream::iterator it=words.begin(); it!= words.end(); ++ it)
        {
            OneWord &t = *it;
            if (t.m_bSpace) continue;

            string &s = t.m_cnt;

            map<string, string>::const_iterator mIt = m_prvs2svrf.find(s);
            if (mIt != m_prvs2svrf.end())
            {
                s = mIt->second;
                hasRecover = true;
            }
        }

        if (hasRecover)
            return merge_token(words);
        else
            return line;
    }

    string mapping(const string &line) const
    {
        Words_Stream words = splite_line(line);
        return mapping_r(words);
    }
private:
    string mapping_r(Words_Stream &tail) const
    {
        string spaceHead;
        handleFirstSpace(spaceHead, tail);

        if (tail.empty())
            return spaceHead;

        Words_Stream head;
        vector<string> matchWords;

        Words_Stream::iterator itNextStart = getMaxForFirst(head, tail, matchWords);

        assert(false == matchWords.empty());
        assert(head.size() >= matchWords.size());

        while (true)
        {
            assert(matchWords.size() - 1 < m_mapping.size());
            const MAP &sp = m_mapping[matchWords.size() - 1];
            MAP::const_iterator sit = sp.find(matchWords);
            if (sit != sp.end())
            {
                const Patterner *p = sit->second;
                return spaceHead + p->m_pvrs + mapping_r(tail);
            }
            else
            {
                if (false == lessOneWord(head, tail, matchWords))
                {
                    assert(matchWords.empty());
                    break;
                }
            }
        }


        assert(head.empty());
        head.splice(head.begin(), tail, tail.begin(), itNextStart);

        return spaceHead + merge_token(head) + mapping_r(tail);
    }

private:
    Words_Stream::iterator getMaxForFirst(Words_Stream &header, Words_Stream &all, vector<string> &vec) const
    {
        Words_Stream::iterator it = all.begin();

        while (it != all.end())
        {
            OneWord &t = *it;
            header.splice(header.end(), all, it);
            if (false == t.m_bSpace)
            {
                string s = t.m_cnt;
                toUpper(s);
                vec.push_back(s);
                if (vec.size() >= m_mapping.size())
                    break;
            }
            it = all.begin();
        }

        return it;
    }
private:
    vector<Patterner> m_vPatterns;
    map<string, string> m_prvs2svrf;
    vector<MAP> m_mapping;
};

class RuleMapper_Left2Right
{
    struct _HeadMatcher
    {
        const Patterner *m_pattern;
        bool m_hasMore;
        _HeadMatcher() : m_pattern(NULL), m_hasMore(false) {}
    };
    typedef map<vector<string>, _HeadMatcher > MAP;
    static RuleMapper_Left2Right g_RM;
public:
    static const RuleMapper_Left2Right &instance();
private:
    RuleMapper_Left2Right(const RuleMapper_Left2Right &);
    RuleMapper_Left2Right() : m_bOK(false) {}

    bool isOK() const { return m_bOK; }

    bool setPatternVec(const vector<Patterner> &vec)
    {
        m_vPatterns = vec;
        for (vector<Patterner>::const_iterator it=m_vPatterns.begin(); it!=m_vPatterns.end(); ++it)
        {
            const Patterner &pattern = *it;
            string svrf = merge_vector(pattern.m_words);
            bool b1 = m_prvs2svrf.insert(make_pair(pattern.m_pvrs, svrf)).second;
            if (!b1)
            {
                cerr << "value conflict : " << pattern.m_pvrs <<endl;
                return false;
            }

            vector<string> words = pattern.m_words;
            assert(!words.empty());
            MAP::iterator mIt = m_map.find(words);
            if (mIt != m_map.end())
            {
                _HeadMatcher &hm = mIt->second;
                if (hm.m_pattern != NULL)
                {
                    cerr << "value conflict : " << pattern.m_pvrs <<endl;
                    return false;
                }
                else
                {
                    hm.m_pattern = &pattern;
                }
            }
            else
            {

                _HeadMatcher &hm = m_map[words];
                hm.m_pattern = &pattern;;
            }
            words.pop_back();

            while (!words.empty())
            {
                _HeadMatcher &hm = m_map[words];
                hm.m_hasMore = true;
                words.pop_back();
            }
        }

        selfCheck();

        m_bOK = true;
        return true;
    }
public:
    string recover(const string &line) const
    {
        Words_Stream words = splite_line(line);

        bool hasRecover = false;
        for (Words_Stream::iterator it=words.begin(); it!= words.end(); ++ it)
        {
            OneWord &t = *it;
            if (t.m_bSpace) continue;

            string &s = t.m_cnt;

            map<string, string>::const_iterator mIt = m_prvs2svrf.find(s);
            if (mIt != m_prvs2svrf.end())
            {
                s = mIt->second;
                hasRecover = true;
            }
        }

        if (hasRecover)
            return merge_token(words);
        else
            return line;
    }
    string mapping(const string &line) const
    {
        Words_Stream words = splite_line(line);
        return mapping_r(words);
    }
private:
    string mapping_r(Words_Stream &tail) const
    {
        string spaceHead;
        handleFirstSpace(spaceHead, tail);

        if (tail.empty())
            return spaceHead;

        Words_Stream head;
        vector<string> matchWords;

        MAP::const_iterator lastFind = m_map.end();
        size_t nFindCount = 0;

        while (true)
        {
            if (moreOneWord(head, tail, matchWords))
            {
                MAP::const_iterator it = m_map.find(matchWords);
                if (it == m_map.end())
                {
                    if (lastFind == m_map.end())
                    {
                        Words_Stream::iterator it = head.begin();
                        assert(it != head.end());
                        assert(it->m_bSpace == false);

                        ++it;
                        tail.splice(tail.begin(), head, it, head.end());
                    }
                    break;
                }

                const _HeadMatcher &hm = it->second;
                if (hm.m_hasMore)
                {
                    if (hm.m_pattern)
                    {
                        lastFind = it;
                        nFindCount = matchWords.size();
                    }
                    continue;
                }
                else
                {

                    assert(hm.m_pattern);
                    return spaceHead + hm.m_pattern->m_pvrs + mapping_r(tail);
                }

            }
            else
            {
                break;
            }
        }

        if (lastFind != m_map.end())
        {
            const _HeadMatcher &hm = lastFind->second;

            assert(matchWords.size() >= nFindCount);
            while (matchWords.size() > nFindCount)
            {
                bool b = lessOneWord(head, tail, matchWords);
                assert(b);
            }
            assert(hm.m_pattern);
            return spaceHead + hm.m_pattern->m_pvrs + mapping_r(tail);
        }
        else
        {
            return spaceHead + merge_token(head) + mapping_r(tail);
        }
    }

private:
    void selfCheck() const
    {

        cout << "match size: " << m_map.size() <<endl;

        for (MAP::const_iterator it=m_map.begin(); it!=m_map.end(); ++it)
        {
            const vector<string> &vec = it->first;
            const _HeadMatcher &hm = it->second;

            if (hm.m_pattern == NULL)
            {
                assert(hm.m_hasMore == true);
            }
        }
    }
private:
    bool m_bOK;
    vector<Patterner> m_vPatterns;
    map<string, string> m_prvs2svrf;
    MAP m_map;
};

}

using namespace viva_matcher;

namespace viva_rule {

const string g_encrypt = "#encrypt";
const string g_endcrypt= "#endcrypt";


const string g_compatible_string= "COMPATIBLE_PVRS_2012";
const string g_begin_compatible = "#ifdef COMPATIBLE_PVRS_2012";
const string g_end_compatible = "#endif  // COMPATIBLE_PVRS_2012";
const string g_begin_trans_file = "// rc_trans_file: ";
const string g_end_block = "// rc_trans_end_block";

enum COND_TYPE
{
    COND_INVALID,
    COND_INCLUDE,
    COND_ENCRYPT,
    COND_DECRYPT,
    COND_ENDCRYPT,
    COND_IFDEF,
    COND_ENDIF
};

COND_TYPE
judgeFirstDirective(const string &line)
{
    const char *p= line.c_str();
    skipWS(p);
    if (p == line.c_str() + line.size())
        return COND_INVALID;

    if('#' == *p)
    {
        if(0 == strncasecmp(p, "#encrypt", 8))
        {
            return COND_ENCRYPT;
        }
        if(0 == strncasecmp(p, "#decrypt", 8))
        {
            return COND_DECRYPT;
        }
        else if(0 == strncasecmp(p, "#endcrypt", 9))
        {
            return COND_ENDCRYPT;
        }
        else if (0 == strncasecmp(p, "#ifdef", 6))
        {
            return COND_IFDEF;
        }
        else if (0 == strncasecmp(p, "#endif", 6))
        {
            return COND_ENDIF;
        }
    }
    else if(0 == strncasecmp(p, "include", 7))
    {
        return COND_INCLUDE;
    }

    return COND_INVALID;
}

static void
parseOperands(std::string &str, std::vector<std::string> &vOperands)
{
    trim(str);
    if(str.empty())
    {
        return;
    }
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
        {
            return;
        }
        else if(iComment < iRight)
        {
            vOperands.push_back(str.substr(0, iComment));
            return;
        }
        if(iRight == std::string::npos)
        {
            assert(iComment == std::string::npos);

            vOperands.push_back(str);
            return;
        }
        vOperands.push_back(str.substr(0, iRight));
        std::string remainder = str.substr(iRight);
        parseOperands(remainder, vOperands);
    }
}

bool getIncPath(string &path, const string &line)
{
    const char *pPath = strcasestr(line.c_str(), "include");
    string str = pPath + 7;

    std::vector<std::string> vOperands;
    parseOperands(str, vOperands);

    if (vOperands.empty())
        return false;

    str = vOperands[0];
    if (false == parseFileName(str, path))
    {
        cerr << "include line fail!: "<< line <<endl;
        return false;
    }

    return true;
}

struct MappingBlock
{


    vector<string> m_cnts;

    vector<string> toMappingBlock(const RuleMapper_Left2Right &rm) const
    {
        vector<string> vec;
        vec.push_back(g_encrypt);


        for(vector<string>::const_iterator sit=m_cnts.begin(); sit!=m_cnts.end(); ++sit)
        {
            string s = rm.mapping(*sit);
            vec.push_back(s);
        }


        vec.push_back(g_endcrypt);
        return vec;
    }
    void recover(const RuleMapper_Left2Right &rm)
    {
        for (size_t i=0; i<m_cnts.size(); ++i)
        {
            m_cnts[i] = rm.recover(m_cnts[i]);
        }
    }
};


class EncReader
{
public:
    static bool s_gInEnc;
    EncReader(istream &in, ostream &out) : m_in(in), m_out(out) {}
    bool process()
    {
        string line;
        while (getline(m_in, line))
        {
            COND_TYPE t = judgeFirstDirective(line);
            switch (t)
            {
                case COND_INCLUDE:
                {
                    string path;
                    if (false == getIncPath(path, line))
                    {
                        cerr << "include line fail!: "<< line <<endl;
                        return false;
                    }

                    ifstream in(path.c_str());
                    if (!in)
                    {
                        cerr << "open file fail!: " << path <<endl;
                        return false;
                    }
                    EncReader reader(in, m_out);
                    if (false == reader.process())
                        return false;

                    break;
                }
                case COND_ENCRYPT:
                {
                    if (s_gInEnc)
                    {
                        cerr << "nest encrypt!" <<endl;
                        return false;
                    }
                    s_gInEnc = true;
                    m_out << "#encrypt" <<endl;
                    break;
                }
                case COND_ENDCRYPT:
                {
                    if (!s_gInEnc)
                    {
                        cerr << "unmatch endcrypt!"<<endl;
                        return false;
                    }

                    s_gInEnc = false;
                    m_out << "#endcrypt\n" <<endl;
                    break;
                }
                case COND_DECRYPT:
                {
                    cerr<< "cannot mapping rule has #decrypt!"<<endl;
                    return false;
                }
                default:
                {
                    if (s_gInEnc)
                    {
                        const RuleMapper_Left2Right &rm = RuleMapper_Left2Right::instance();
                        m_out << rm.mapping(line) <<endl;
                    }
                    break;
                }

            }
        }
        return true;
    }
private:
    istream &m_in;
    ostream &m_out;
};

bool EncReader::s_gInEnc = false;

string getFileName(const string &pathName)
{
    size_t index = pathName.rfind('/');
    if (index == string::npos)
        return pathName;
    else
        return pathName.substr(index + 1);
}

bool readOneRule_mapping(const string &filePath, list<MappingBlock> &lst, set<string> &incFile)
{
    ifstream in(filePath.c_str());
    if (!in)
    {
        cerr << "cannot open in file: " << filePath <<endl;
        return false;
    }

    string fileName = getFileName(filePath);

    string line;

    bool bInEncrypt = false;
    int nBlock = 0;

    while (getline(in, line))
    {
        COND_TYPE t = judgeFirstDirective(line);

        switch (t) {
            case COND_ENCRYPT:
            {
                if (bInEncrypt)
                {
                    cerr << "nest encrpyt!" <<endl;
                    return false;
                }

                bInEncrypt = true;
                ++nBlock;
                MappingBlock mb;


                lst.push_back(mb);
                break;
            }

            case COND_ENDCRYPT:
            {
                if (false == bInEncrypt)
                {
                    cerr << "endcrypt not match!" <<endl;
                    return false;
                }

                bInEncrypt = false;
                break;
            }
            case COND_INCLUDE:
            {
                string path;
                if (false == getIncPath(path, line))
                {
                    cerr << "include line fail!: "<< line <<endl;
                    return false;
                }
                incFile.insert(path);


            }
            default:
            case COND_INVALID:
            {
                if (bInEncrypt)
                {
                    assert(!lst.empty());
                    MappingBlock &mb = lst.back();
                    mb.m_cnts.push_back(line);
                }
                break;
            }
        }
    }

    return true;
}

bool readRule_mapping(const char *pInFile, list<MappingBlock> &lst)
{

    set<string> pathSet;
    set<string> nameSet;

    string mainFile = pInFile;

    pathSet.insert(mainFile);

    while (false == pathSet.empty())
    {
        set<string>::iterator it = pathSet.begin();
        string path = *it;
        pathSet.erase(it);
        string name = getFileName(path);

        if (nameSet.find(name) != nameSet.end())
        {

            continue;
        }
        else
        {
            nameSet.insert(name);
            if (false == readOneRule_mapping(path, lst, pathSet))
            {
                return false;
            }
        }
    }
    return true;
}


}

using namespace viva_rule;

}


using namespace viva;

string encryptPatternVec(const vector<Patterner> &vec)
{
    string str = patternVec2String(vec);

    return str;



}

bool decryptPatternVec(vector<Patterner> &vec, const string &enc)
{
    string str;

    str = enc;
    return string2PatternVec(vec, str);
}

bool getPatternFromFile(vector<Patterner> &pvec, const char *file)
{
    vector<string> fileBuffer;
    if (false == readAllFile(fileBuffer, file))
        return false;

    return getPatternFromVec(pvec, fileBuffer);
}

bool outputPatternVec(const vector<Patterner> &pvec, const char *outFile)
{
    ofstream out(outFile);
    if (!out)
    {
        cerr << "cannot open file: " << outFile <<endl;
        return false;
    }

    for (vector<Patterner>::const_iterator it=pvec.begin(); it!=pvec.end(); )
    {
        const Patterner &p = *it;
        out << p.toString();
        if (++it != pvec.end())
            out << "\n";
        else
            break;
    }
    return true;
}

bool processMappingFile(const char * inFile, const char * outFile)
{
    vector<Patterner> pvec;
    if (false == getPatternFromFile(pvec, inFile))
        return false;

    outputPatternVec(pvec, outFile);

    return true;
}

bool genCppFile(const char *inFile, const char *cppFile)
{
    vector<Patterner> pvec;
    if (false == getPatternFromFile(pvec, inFile))
        return false;

    RuleMapper_MaxWords rm;
    if (false == rm.setPatternVec(pvec))
    {
        cerr << "not one-2-one mapping" <<endl;
        return false;
    }

    ofstream out(cppFile);
    if (!out)
    {
        cerr << "cannot open file: " << cppFile <<endl;
        return false;
    }

    string encStr = encryptPatternVec(pvec);


    out << "const char *g_viva_mapping = ";
    out << "\"";
    for (size_t i=0; i<encStr.size(); ++i)
    {
        char c = encStr[i];
        if (c == '\n')
            out << "\\n";
        else
            out << c;
    }
    out << "\";";

    return true;
}

bool get_inner_PatternVec(vector<Patterner> &vec)
{
    string encString = g_viva_mapping;
    bool b = decryptPatternVec(vec, encString);
    assert(b);
    return b;
}

RuleMapper_Left2Right RuleMapper_Left2Right::g_RM;

const RuleMapper_Left2Right &RuleMapper_Left2Right::instance()
{
    if (!g_RM.isOK())
    {
        vector<Patterner> vp;
        if (false == get_inner_PatternVec(vp))
        {
            exit(-1);
        }

        if (false == g_RM.setPatternVec(vp))
        {
            cerr << "inner mapping err!" <<endl;
            exit(-1);
        }
    }

    assert(g_RM.isOK());
    return g_RM;
}

static bool
case_equl(const string &s, const char *p)
{
    return strcasecmp(s.c_str(), p) == 0;
}

static bool hasOtherChar(string s)
{

    if (s.size() > 2 && s[s.size()-2] == '4' && s[s.size()-1] == '5')
    {
        s.resize(s.size()-2);
    }
    else if (s.size() >= 2 && (s[s.size()-1] == '0' ||
                               s[s.size()-1] == '1' ||
                               s[s.size()-1] == '2' ||
                               s[s.size()-1] == '3' ||
                               s[s.size()-1] == '4' )
             )
    {
        s.resize(s.size() - 1);
    }
    for (size_t i=0; i<s.size(); ++i)
    {
        const char &c = s[i];
        if (('a' <= c && c <='z') || ('A' <= c && c <='Z') ||
                c == ' ' || c == ':' || c == '_' || c == '~' )
        {
            continue;
        }
        else
        {
            return true;
        }
    }

    return false;
}

bool checkOtherChar(const vector<Patterner> &inner)
{
    vector<string> errorLines;
    for (vector<Patterner>::const_iterator it=inner.begin(); it!=inner.end(); ++it)
    {
        const Patterner &p = *it;
        vector<string> vec = p.m_words;
        vec.push_back(p.m_pvrs);

        for (vector<string>::const_iterator it=vec.begin(); it!=vec.end(); ++it)
        {
            if (hasOtherChar(*it))
            {
                string s = p.toString();
                errorLines.push_back(s);
                cerr << "error line: " << s << endl;
                break;
            }
        }
    }
    return errorLines.empty();
}

bool checkCpp(const char *inFile, const char *cppFile)
{
    vector<Patterner> inner;
    bool b1 = get_inner_PatternVec(inner);
    if (false == b1)
    {
        cerr << "inner mapper error!" <<endl;
        return false;
    }

    outputPatternVec(inner, cppFile);
    cout << "inner mapping wirte to file : " << cppFile <<endl;

    if (false == checkOtherChar(inner))
    {
        return false;
    }

    RuleMapper_MaxWords rm;
    if (false == rm.setPatternVec(inner))
    {
        cerr << "inner mapping err!" <<endl;
        return false;
    }

    vector<Patterner> outter;
    bool b2 = getPatternFromFile(outter, inFile);
    if (false == b2)
    {
        return false;
    }

    if (!isEqualPattern(inner, outter))
    {
        cerr << "inner mapping not equal outter!" <<endl;
        return false;
    }

    cout << "inner mapping is equal file : " << inFile <<endl;

    return true;
}

bool mappingFile(const char *pInFile, const char *pOutFile, bool isMapping)
{
    vector<Patterner> vp;
    if (false == get_inner_PatternVec(vp))
        return false;

    RuleMapper_MaxWords rm;
    if (false == rm.setPatternVec(vp))
    {
        cerr << "inner mapping err!" <<endl;
        return false;
    }

    ifstream in(pInFile);
    if (!in)
    {
        cerr << "cannot open in file: " << pInFile <<endl;
        return false;
    }
    ofstream out(pOutFile);
    if (!out)
    {
        cerr << "cannot open out file: " << pOutFile <<endl;
        return false;
    }

    string line;
    while (getline(in, line))
    {
        if (isMapping)
            line = rm.mapping(line);
        else
            line = rm.recover(line);

        out << line << "\n";
    }

    return true;
}

bool mappingFile_v2(const char *pInFile, const char *pOutFile, bool isMapping)
{
    const RuleMapper_Left2Right &rm = RuleMapper_Left2Right::instance();

    ifstream in(pInFile);
    if (!in)
    {
        cerr << "cannot open in file: " << pInFile <<endl;
        return false;
    }
    ofstream out(pOutFile);
    if (!out)
    {
        cerr << "cannot open out file: " << pOutFile <<endl;
        return false;
    }

    string line;
    while (getline(in, line))
    {
        if (isMapping)
            line = rm.mapping(line);
        else
            line = rm.recover(line);

        out << line << "\n";
    }

    return true;
}

bool mappingRule(const char *pInFile, const char *pOutFile)
{
    const RuleMapper_Left2Right &rm = RuleMapper_Left2Right::instance();

    list<MappingBlock> lst;
    if (false == readRule_mapping(pInFile, lst))
    {
        return false;
    }

    ofstream out(pOutFile, fstream::app);
    if (!out)
    {
        cerr << "cannot open out file: " << pOutFile <<endl;
        return false;
    }

    out << "\n" ;


    out << g_begin_compatible << "\n\n";


    for (list<MappingBlock>::const_iterator it=lst.begin(); it!=lst.end(); ++it)
    {
        const MappingBlock &mb = *it;
        vector<string> vec = mb.toMappingBlock(rm);
        for(vector<string>::const_iterator sit=vec.begin(); sit!=vec.end(); ++sit)
        {
            out << *sit << endl;
        }
        out << endl;
    }

    out << g_end_compatible << "\n";
    return true;
}

bool mappingRule_v2(const char *pInFile, const char *pOutFile)
{
    ifstream in(pInFile);
    if (!in)
    {
        cerr << "cannot open file: " << pInFile <<endl;
        return false;
    }

    ofstream out(pOutFile, ios::app);
    if (!out)
    {
        cerr << "cannot open file: " << pOutFile <<endl;
        return false;
    }

    out << "\n";
    out << g_begin_compatible << "\n\n";

    EncReader r(in, out);
    if (false == r.process())
        return false;

    out << g_end_compatible << "\n";
    return true;
}

bool readRecoverEncBlock(const char *pInFile, list<vector<string> > &blockStrings)
{
    ifstream in(pInFile);
    if (!in)
    {
        cerr<< "cannot open file: " <<pInFile <<endl;
        return false;
    }

    bool hasFind = false;
    string line;
    while (getline(in, line))
    {
        COND_TYPE t = judgeFirstDirective(line);
        if (t == COND_IFDEF && line.find(g_compatible_string) != string::npos)
        {
            hasFind = true;
            break;
        }
    }

    if (hasFind == false)
    {
        cerr << "cannot find: " << g_compatible_string <<endl;
        return false;
    }


    bool inEnc = false;
    while (getline(in, line))
    {
        COND_TYPE t = judgeFirstDirective(line);
        switch (t)
        {
            case COND_DECRYPT:
            {
                if (inEnc)
                {
                    cerr << "cannot match #endcrypt!" <<endl;
                    return false;
                }
                inEnc = true;
                blockStrings.resize(blockStrings.size()+1);
                break;
            }
            case COND_ENDCRYPT:
            {
                if (!inEnc)
                {
                    cerr << "cannot match #endcrypt!" <<endl;
                    return false;
                }
                inEnc = false;
                break;
            }

            case COND_ENDIF:
                break;

            case COND_INVALID:
            default:
            {
                if (inEnc)
                {
                    assert(!blockStrings.empty());
                    vector<string> &vec = blockStrings.back();
                    vec.push_back(line);
                }
                break;
            }
        }
    }

    if (inEnc)
    {
        cerr << "cannot match #endcrypt!" <<endl;
        return false;
    }
    else
        return true;
}
int main(int argc, char **argv)
{


    const char *pInFile = NULL;
    const char *pEncFile = NULL;
    const char *pOutFile = NULL;

    if (argc < 2)
    {
        cerr << "param number err!" <<endl;
        return -1;
    }

    const char *pMode = argv[1];

    for (int i=2; i<argc; ++i)
    {
        if (0 == strncmp(argv[i], "-i", 2) )
        {
            if (++i == argc)
                return -1;

            pInFile = argv[i];
        }

        else if (0 == strncmp(argv[i], "-o", 2) )
        {
            if (++i == argc)
                return -1;

            pOutFile = argv[i];
        }
        else if (0 == strncmp(argv[i], "-s", 2) )
        {
            if (++i == argc)
                return -1;

            pEncFile = argv[i];
        }
    }

    bool bRet = false;
    if (0 == strcasecmp(pMode, "proc"))
    {
        bRet = processMappingFile(pInFile, pOutFile);
    }
    else if (0 == strcasecmp(pMode, "gencpp"))
    {
        bRet = genCppFile(pInFile, pOutFile);
    }
    else if (0 == strcasecmp(pMode, "check"))
    {
        bRet = checkCpp(pInFile, pOutFile);
    }
    else if ( 0 == strcasecmp(pMode, "map"))
    {
        bRet = mappingFile_v2(pInFile, pOutFile, true);
    }
    else if ( 0 == strcasecmp(pMode, "recover"))
    {
        bRet = mappingFile_v2(pInFile, pOutFile, false);
    }
    else if ( 0 == strcasecmp(pMode, "append"))
    {
        bRet = mappingRule_v2(pInFile, pOutFile);
    }
    else if ( 0 == strcasecmp(pMode, "replace"))
    {



    }
    else
    {
        cerr << "unkown mode!" <<endl;
        return -1;
    }
    if (!bRet)
        return -1;

    return 0;
}
