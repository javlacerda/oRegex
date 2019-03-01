#ifndef _O_REGEX_H
#define _O_REGEX_H

#define LIB_RES_NAME 1000
#define RES_CONST_PREFIX 19999
#define RES_CONST_START 20000
#define RES_CONST_END 20009

const qshort cObject_Regex = 1;

const qlong cSetPatternFunction = 1;
const qlong cSetFlagsFunction = 2;
const qlong cMatchFunction = 3;
const qlong cMatchResultsFunction = 4;
const qlong cSearchFunction = 5;
const qlong cReplaceFunction = 6;
const qlong cSplitFunction = 7;
const qlong cGetErrorMessageFunction = 18;

const qlong kRegexFlagECMAScript = 0;
const qlong kRegexFlagBasic = 1;
const qlong kRegexFlagExtended = 2;
const qlong kRegexFlagAwk = 4;
const qlong kRegexFlagGrep = 8;
const qlong kRegexFlagEgrep = 16;
const qlong kRegexFlagIcase = 32;
const qlong kRegexFlagNoSubs = 64;
const qlong kRegexFlagOptimize = 128;
const qlong kRegexFlagCollate = 256;

bool getParamString(EXTCompInfo *pEci, long paramID, std::wstring &valor);
bool getParamInt(EXTCompInfo *eci, long paramID, int &valor);
errno_t char2wstring(const char *source, std::wstring &destination);

class ORegex
{
private:
    std::wstring errorMessage;
    std::wstring pattern;
    int flags;

public:
    qobjinst mObjPtr;

public:
    ORegex(qobjinst pObjPtr);
    ORegex(qobjinst pObjPtr, const ORegex *src);
    ~ORegex();
    void setObject(qobjinst pObjPtr, const ORegex *src);

    qbool methodCall(EXTCompInfo *pEci);
    qbool setPattern(EXTCompInfo *pEci);
    qbool setFlags(EXTCompInfo *pEci);
    void getErrorMessage(EXTCompInfo *pEci);
    qbool match(EXTCompInfo *pEci);
    qbool matchResults(EXTCompInfo *pEci);
    qbool search(EXTCompInfo *pEci);
    qbool replace(EXTCompInfo *pEci);
    qbool split(EXTCompInfo *pEci);

private:
    std::regex:: flag_type buildFlagType();
};

#endif