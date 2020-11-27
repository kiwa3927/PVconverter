



#ifndef RCSCHECKTEXTEXTRACTOR_H_
#define RCSCHECKTEXTEXTRACTOR_H_

class rcsCheckTextExtractor_T
{
public:
    rcsCheckTextExtractor_T(const char *pFile);

    void execute();

private:
    void addText(const std::string &sFileName);

private:
    const char *m_pFile;
};

#endif 
