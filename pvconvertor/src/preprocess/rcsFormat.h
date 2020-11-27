

#ifndef RCSFORMAT_H_
#define RCSFORMAT_H_

#include <string>

class rcsFormat
{
public:
	rcsFormat(const char *pFileName);
	bool execute();

private:
	void formatTvfFunWithoutComment(std::string &sLineBuf);
	void formatTvfFunForSetLayer(std::string &sLineBuf);

    const char           *m_pFileName;
};

#endif 
