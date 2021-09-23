



#ifndef SRC_PUBLIC_MANAGER_RCSLITHOFILESPEC_H_
#define SRC_PUBLIC_MANAGER_RCSLITHOFILESPEC_H_

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include "public/rcsTypes.h"

struct rcsLithoFileAction
{
    rcsLithoFileAction():m_hasStitchMode(false), m_iPriority(0), m_bSame(false)
    {}

    std::string       m_actionLayer;
    bool              m_hasStitchMode;
    hvInt32           m_iPriority;
    bool              m_bSame;
};

struct rcsLithoFileAnchor
{
    rcsLithoFileAnchor():m_iPriority(0), m_isMask0(true)
    {}

    std::string     m_anchorLayer;
    hvInt32         m_iPriority;
    bool            m_isMask0;
};

struct rcsLithoFileSpec
{
    rcsLithoFileSpec() : m_isResolve(false), m_isOutputAllConflict(false), m_isVerbose(false)
    {
    }
    std::string m_sLithoFileName;
    bool                                m_isResolve;
    bool                                m_isOutputAllConflict;
    bool                                m_isVerbose;
    std::vector<rcsLithoFileAction>     m_stAction;
    std::vector<rcsLithoFileAnchor>     m_stAnchor;
    std::string                         m_anchorForMP[4];
    std::vector<std::string>            m_stSeparatorLayers;
    std::vector<std::string>            m_stTargetLayers;
};
#endif
