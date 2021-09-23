





#ifndef _H_ERR_MANAGER_H_
#define _H_ERR_MANAGER_H_

#include <vector>
#include <string>
#include <iostream>
#include <map>

using std::vector;
using std::map;
using std::string;

#include "public/rcsTypes.h"
#include "public/util/rcsStringUtil.hpp"

struct rcErrorNode_T
{
	enum ErrLevelEnum
	{
	    FATAL,
	    ERROR,
	    WARNING,
	    INFO,
	}           m_eErrLevel;
    hvInt32		m_nType;
    hvUInt32    m_nLineNo;
    std::string m_eKeyValue;

    rcErrorNode_T(rcErrorNode_T::ErrLevelEnum eLevel, hvInt32 nType,
                  hvUInt32 nLineNo, std::string keyValue = "") :
    m_eErrLevel(eLevel),m_nType(nType), m_nLineNo(nLineNo), m_eKeyValue(keyValue)
    {
    }
};

struct rcErrorMsg_T
{
    const char *pType;
    const char *pMsg;
};



class rcErrManager_T
{
public:
    static rcErrManager_T& Singleton();
	static void initial();

    typedef std::map<hvUInt32, std::pair<std::string, hvUInt32> > LINE_INDEX_MAP;

    void addError(const rcErrorNode_T& error, std::ostream &out = std::cerr);

    void MapLineIndex(const hvUInt32 &nGlobalLineNo, const hvUInt32 &nLocalLineNo,
                      const string &sFileName);

    void Report(std::ostream &out = std::cerr);

    void clear();

    bool getErrors(std::vector<std::string> &vErrMsgs);

#ifdef DEBUG
    void printMap();
#endif

private:
    rcErrManager_T();
    void reportError(const rcErrorNode_T& error, std::ostream &out);

private:
    LINE_INDEX_MAP				    m_mapLineIndex;
    std::vector<rcErrorNode_T>  	m_vErrorList;
	static const rcErrorMsg_T*		m_pvErrMsg;
};




enum ERROR_SVRF
{
	OPEN1=0,
    DEF1,DEF2,DEF3,DEF4,DEF5,
	INCL1,INCL2,INCL3,INCL4,
	LOC1,LOC2,LOC3,LOC4,
    SYN1,SYN2,SYN3,SYN4,SYN5,SYN6,SYN7,SYN8,
	MAC1,MAC2,MAC3,MAC4,MAC5,MAC6,MAC7,MAC8,MAC9,MAC10,
	ENV1,ENV2,
	LAY1,LAY2,LAY3,LAY4,LAY5,LAY6,LAY7,
	OLS1,OLS2,OLS3,OLS4,
	OUT1,OUT2,
	SPC1,SPC2,SPC4,SPC5,SPC7,SPC9,SPC10,SPC11,SPC12,
	SPC13,SPC14,SPC15,SPC16,SPC17,SPC18,SPC19,
	VAR1,VAR2,VAR3,
	RES1,RES2,RES3,RES4,RES5,RES8,
    PRAGMA6,PRAGMA8,PRAGMA9,
	INP1,INP2,INP3,INP4,INP5,INP6,INP7,INP8,INP9,INP10,INP11,INP12,INP13,INP14,
    INP15,INP16,INP17,INP18,INP20,INP21,INP22,INP23,INP24,INP28,INP32,INP33,INP34,INP35,INP36,INP37,INP38,
	NUM1,NUM2,NUM3,NUM4,NUM5,NUM6,NUM7,NUM8,NUM11,NUM12,NUM13,
	KEY1,
	CNS1,CNS2,CNS3,CNS4,CNS5,CNS6,CNS7,CNS8,
	GRP1,GRP2,GRP3,GRP4,GRP5,
	DCM1,DCM2,DCM3,DCM4,DCM5,DCM6,DCM7,DCM8,DCM9,DCM10,DCM11,DCM12,DCM13,DCM14,
	DCM15,DCM16,DCM17,DCM18,DCM19,
	TYP1,TYP2,TYP3,TYP4,TYP5,TYP6,TYP7,
	NAR1,NAR2,NAR3,NAR4,NAR5,NAR6,NAR7,NAR8,NAR9,NAR10,NAR11,NAR12,NAR13,NAR14,
	EXT1,EXT2,EXT3,EXT4,EXT5,EXT6,EXT7,EXT8,EXT9,EXT10,EXT11,EXT12,EXT13,EXT14,EXT15,EXT16,EXT17,
	LCL1,LCL2,LCL3,LCL4,LCL5,LCL6,
	DVL1,DVL2,DVL3,DVL4,
    CEL1,CEL2,CEL3,CEL4,CEL5,CEL6,
    DTP1,DTP2,DTP3,DTP4,DTP5,DTP6,DTP7,DTP8,DTP9,
    FIL1,FIL2,FIL3,FIL4,FIL5,FIL6,FIL7,FIL8,FIL9,FIL10,FIL11,FIL12,
    MDV1,MDV2,MDV3,MDV4,MDV5,MDV6,MDV7,MDV8,MDV9,MDV10,MDV11,MDV12,MDV13,
    LPI1,LPI2,LPI3,LPI4,LPI5,LPI6,LPI7,
    LPM1,LPM2,LPM3,LPM4,LPM5,LPM6,LPM7,LPM8,LPM9,LPM10,LPM11,LPM12,
    RGT1,RGT2,RGT3,RGT4,RGT5,RGT6,RGT7,RGT8,RGT9,RGT10,
    LRD1,LRD2,LRD3,LRD4,LRD5,LRD6,LRD7,LRD8,LRD9,LRD10,
    LRD11,LRD12,LRD13,LRD14,LRD15,LRD16,LRD17,LRD18,LRD19,LRD20,
    LRD21,LRD22,LRD23,LRD24,LRD25,LRD26,LRD27,LRD28,LRD29,LRD30,
    SGR1,SGR2,SGR3,SGR4,SGR5,SGR6,
    SPI1,SPI2,SPI3,SPI4,SPI5,
    TRP1,TRP2,TRP3,TRP4,TRP5,TRP6,TRP7,TRP8,TRP9,TRP10,
    TRP11,TRP12,TRP13,TRP14,TRP15,TRP16,TRP17,TRP18,TRP19,TRP20,
    TRP21,TRP22,TRP23,
    DPR1,DPR2,DPR3,DPR4,DPR5,DPR6,DPR7,DPR8,DPR9,DPR10,
    DPR11,DPR12,DPR13,DPR14,DPR15,DPR16,DPR17,DPR18,DPR19,DPR20,
    DPR21,DPR22,DPR23,DPR24,DPR25,DPR26,DPR27,DPR28,DPR29,DPR30,
    DPR31,DPR32,DPR33,DPR34,DPR35,DPR36,DPR37,DPR38,DPR39,DPR40,
    DPR41,DPR42,DPR43,DPR44,DPR45,DPR46,DPR47,DPR48,DPR49,DPR50,
    DPR51,DPR52,DPR53,DPR54,DPR55,DPR56,DPR57,DPR58,DPR59,DPR60,
    DPR61,DPR62,DPR63,

    ERC1,
    DEV1,DEV2,DEV3,DEV4,DEV5,DEV6,DEV7,DEV8,DEV9,DEV10,
    DEV11,DEV12,DEV13,DEV14,DEV15,DEV16,DEV17,DEV18,DEV19,DEV20,
    DEV21,DEV22,DEV23,DEV24,DEV25,DEV26,DEV27,DEV28,DEV29,DEV30,
    DEV31,DEV32,DEV33,DEV34,DEV35,DEV36,DEV37,
    USER1,ARG1,
    TRT1,TRT2, TRT5,
    RDBG1,RDBG2,RDBG3,RDBG4,

    CON1,CON2,CON3,

    ERCTVF1,ERCTVF2,ERCTVF3,ERCTVF4,
};



static const
char* vConflictSvrfNames[] =
{
    "abut",
    "acute",
    "angled",
    "by",
    "connected",
    "corner",
    "direct",
    "extend",
    "fracture",
    "intersecting",
    "layout",
    "mask",
    "notch",
    "obtuse",
    "opposite",
    "overlap",
    "para",
    "parallel",
    "perp",
    "perpendicular",
    "proj",
    "ports",
    "projecting",
    "region",
    "ret",
    "singular",
    "source",
    "space",
    "square",
    "step",
    "include",
    "capacitance"
    "coin",
    "coincident",
    "convex",
    "dfm",
    "drawn",
    "drc",
    "erc",
    "exclude",
    "expand",
    "extend",
    "flag",
    "INDUCTANCE",
    "LABEL",
    "LVS",
    "MEASURE",
    "PARASITIC",
    "PATH",
    "PEX",
    "PORT",
    "RESISTANCE",
    "SVRF",
    "TRACE",
    "UNIT",
    "VIRTUAL",
    "MDP",
    "WITH",
};

#define INVALID_NAME_COUNT 57

static inline bool
isValidSvrfName(const std::string &sName)
{
    std::vector<std::string> vString;
    tokenize(vString, sName);
    if(vString.size() > 1)
    {
        return false;
    }
    for(hvUInt32 index = 0; index < INVALID_NAME_COUNT; ++index)
    {
        std::string prefix = vConflictSvrfNames[index];
        hvUInt32 i = 0;
        while(i != vString.size())
        {
            if(0 == strcasecmp(vString[i].c_str(), prefix.c_str()))
            {
                return false;
            }
            ++i;
        }
    }
    return true;
}

static const
std::string rcsDFMCmd[] =
{
    "AREA",
    "PERIM",
    "PERIMeter",
    "PERIMX",
    "PERIMeterX",
    "PERIMXP",
    "PERIMeterXP",
    "PERIMY",
    "PERIMeterY",
    "PERIMYP",
    "PERIMeterYP",
    "LENGTH",
    "LENGTHX",
    "LENGTHXP",
    "LENGTHY",
    "LENGTHYP",
    "COUNT",
    "ANGLE",
    "EC",
    "ECMAX",
    "ECMIN",
    "ECBEGIN",
    "ECEND",
    "ECX",
    "ECXP",
    "ECY",
    "ECYP",
    "EW",
    "EWP",
    "EWX",
    "EWXP",
    "EWY",
    "EWYP",
    "NETID",
    "VNETID",
    "PSNETID",
    "VECTOR",
    "GLOBALNETID",
    "PROPERTY",
    "VPROPERTY",
    "NETPROPERTY",
    "NETVPROPERTY",
    "PROPERTY_REF",
    "SPROPERTY_REF",
    "PSPROPERTY_REF",
    "VPROPERTY_REF",
    "SVPROPERTY_REF",
    "NETPROPERTY_REF",
    "NETVPROPERTY_REF",
    "ABS",
    "ABSolute",
    "ACOS",
    "ACOSH",
    "ASIN",
    "ASINH",
    "ATAN",
    "ATAN2",
    "ATANH",
    "CBRT",
    "CEIL",
    "COPYSIGN",
    "COS",
    "COSH",
    "E",
    "ERF",
    "ERFC",
    "EXP",
    "EXP2",
    "EXPM1",
    "FDIM",
    "FLOOR",
    "FMAX",
    "FMIN",
    "FMOD",
    "HYPOT",
    "LGAMMA",
    "LOG",
    "LOG10",
    "LOG1P",
    "LOG2",
    "PI",
    "POISSON",
    "POW",
    "REMAINDER",
    "RINT",
    "ROUND",
    "SIN",
    "SINH",
    "SQRT",
    "TAN",
    "TANH",
    "TRUNC",
    "TRUNCate",
    "MAX",
    "MIN",
    "SPROPERTY",
    "PSPROPERTY",
    "NARAC",
    "PROD",
    "PRODuct",
    "SUM",
    "Y0",
    "Y1",
    "YN",
    "J0",
    "J1",
    "JN",
};

#define INVALID_FUNCTION_NAME_COUNT 99

static inline bool
isDFMFuncName(const std::string &sName)
{
    for(hvUInt32 index = 0; index < INVALID_FUNCTION_NAME_COUNT; ++index)
    {
        std::string prefix = rcsDFMCmd[index];
        if(0 == strcasecmp(sName.c_str(), prefix.c_str()))
        {
            return true;
        }
    }
    return false;
}

#define s_errManager rcErrManager_T::Singleton()

#endif
