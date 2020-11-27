#ifndef RCSCOMMENTUTIL_H
#define RCSCOMMENTUTIL_H

#include <string>
#include <vector>
#include "public/token/rcsToken.h"

namespace rcsCommentUtil
{

class token_cmp
{
public:
    token_cmp(TOKEN_TYPE type, const std::string &val)
        :m_type(type), m_val(val)
    {}
    bool operator ()(const rcsToken_T& token) const
    {
        return token.eType == m_type && token.sValue == m_val;
    }
private:
    const TOKEN_TYPE m_type;
    const std::string m_val;
};

class token_type_cmp
{
public:
    token_type_cmp(TOKEN_TYPE type)
        :m_type(type)
    {}
    bool operator ()(const rcsToken_T& token) const
    {
        return token.eType == m_type;
    }
private:
    const TOKEN_TYPE m_type;
};

int count_line(const std::string &str);

bool has_braces_match(const std::string &str);


bool maybe_rulecheck(const std::string &svrf);


bool maybe_in_buildin(const std::vector<rcsToken_T>& tokens, std::vector<rcsToken_T>::const_iterator iter);

bool maybe_rulecheck_comment(const std::vector<rcsToken_T>& tokens, std::vector<rcsToken_T>::const_iterator iter);

std::vector<rcsToken_T> getTokens(const std::string &svrf, bool *hasBadToken);

std::vector<std::pair<std::vector<rcsToken_T>, bool > > split_rule(const std::vector<rcsToken_T>& tokens, bool *hasCheck);

bool split_next_rule(std::vector<rcsToken_T>::const_iterator &iter,
                     const std::vector<rcsToken_T>::const_iterator &end,
                     std::vector<std::pair<std::vector<rcsToken_T>, bool > > &vec);

std::string replaceKeyword(const std::vector<rcsToken_T> &tokens);


bool getCommentContent(const std::string &comment, std::string::size_type &start, std::string::size_type &end);

bool replaceBlockComment(const std::string &comment, std::string &newStr);

}

#endif 
