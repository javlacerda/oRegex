//--------------------------------------------------------------------------------
// Copyright 2018 Jos� Lacerda
//
// Permission is hereby granted, free of charge, to any person obtaining 
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
// OTHER DEALINGS IN THE SOFTWARE.
//--------------------------------------------------------------------------------

#include <string>
#include <memory>
#include <regex>

#include <basics.h>
#include <dmconst.he>
#include <extcomp.he>

#include "oRegex.h"

#define VERSION_MAJOR 1
#define VERSION_MINOR 2

//--------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------

ECOobject oRegexObjects[] =
{
    {cObject_Regex, 2000, 0, 0}
};

ECOparam paramsSetPattern[] =
{
    {7000, fftCharacter, 0, 0}
};

ECOparam paramsSetFlags[] =
{
    {7001, fftInteger, 0, 0}
};

ECOparam paramsMatch[] =
{
    {7002, fftCharacter, 0, 0},
    {7003, fftBoolean,   0, 0}
};

ECOparam paramsMatchResults[] =
{
    {7002, fftCharacter, 0, 0},
    {7003, fftBoolean,   0, 0},
    {7004, fftList,      0, 0}
};

ECOparam paramsSearchResults[] =
{
    {7002, fftCharacter, 0, 0},
    {7004, fftList,      0, 0}
};

ECOparam paramsReplace[] =
{
    {7002, fftCharacter, 0, 0},
    {7006, fftCharacter, 0, 0},
    {7007, fftCharacter, 0, 0}
};

ECOparam paramsSplit[] =
{
    {7002, fftCharacter, 0, 0},
    {7004, fftList,      0, 0}
};

ECOmethodEvent oRegexFunctions[] =
{
    {cSetPatternFunction,      6000, fftBoolean,   sizeof(paramsSetPattern) / sizeof(ECOparam),    paramsSetPattern,    0, 0},
    {cSetFlagsFunction,        6001, fftBoolean,   sizeof(paramsSetFlags) / sizeof(ECOparam),      paramsSetFlags,      0, 0},
    {cMatchFunction,           6002, fftBoolean,   sizeof(paramsMatch) / sizeof(ECOparam),         paramsMatch,         0, 0},
    {cMatchResultsFunction,    6003, fftBoolean,   sizeof(paramsMatchResults) / sizeof(ECOparam),  paramsMatchResults,  0, 0},
    {cSearchFunction,          6004, fftBoolean,   sizeof(paramsSearchResults) / sizeof(ECOparam), paramsSearchResults, 0, 0},
    {cReplaceFunction,         6005, fftBoolean,   sizeof(paramsReplace) / sizeof(ECOparam),       paramsReplace,       0, 0},
    {cSplitFunction,           6006, fftBoolean,   sizeof(paramsSplit) / sizeof(ECOparam),         paramsSplit,         0, 0},
    {cGetErrorMessageFunction, 6017, fftCharacter, 0,                                              0,                   0, 0}
};

#define cFunctionsCount (sizeof(oRegexFunctions) / sizeof(ECOmethodEvent))
#define cObjectsCount (sizeof(oRegexObjects) / sizeof(ECOobject))

//--------------------------------------------------------------------------------
// For Omnis 5
// extern "C" qlong OMNISWNDPROC oRegexWndProc(HWND hwnd, LPARAM Msg, WPARAM wParam, LPARAM lParam, EXTCompInfo *eci)
//--------------------------------------------------------------------------------
extern "C" LRESULT OMNISWNDPROC oRegexWndProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam, EXTCompInfo *eci)
{
	ECOsetupCallbacks(hwnd, eci);

	switch (Msg) {
        case ECM_CONNECT:
            return EXT_FLAG_LOADED | EXT_FLAG_ALWAYS_USABLE | EXT_FLAG_REMAINLOADED | EXT_FLAG_NVOBJECTS;

        case ECM_DISCONNECT:
            break;

        case ECM_GETOBJECT:
		    return ECOreturnObjects(gInstLib, eci, &oRegexObjects[0], cObjectsCount);

        case ECM_OBJCONSTRUCT:
            if (eci->mCompId == cObject_Regex) {
                ORegex *taskDialog = reinterpret_cast<ORegex *>(ECOfindNVObject(eci->mOmnisInstance, lParam));
                if (taskDialog == nullptr) {
                    ORegex *obj = new ORegex(reinterpret_cast<qobjinst>(lParam));
                    ECOinsertNVObject(eci->mOmnisInstance, lParam, reinterpret_cast<void *>(obj));
                }
                return qtrue;
            }
            return qfalse;

        case ECM_OBJDESTRUCT:
            if (eci->mCompId == cObject_Regex && wParam == ECM_WPARAM_OBJINFO) {
                void *object = ECOremoveNVObject(eci->mOmnisInstance, lParam);
                if (object) {
                    ORegex *obj = reinterpret_cast<ORegex *>(object);
                    delete obj;
                    obj = nullptr;
                }
            }
            return qtrue;

        case ECM_OBJECT_COPY: {
            objCopyInfo *copyInfo = reinterpret_cast<objCopyInfo *>(lParam);
            ORegex *srcObject = reinterpret_cast<ORegex *>(ECOfindNVObject(eci->mOmnisInstance, copyInfo->mSourceObject));
            if (srcObject) {
                ORegex *destObj = reinterpret_cast<ORegex *>(ECOfindNVObject(eci->mOmnisInstance, copyInfo->mDestinationObject));
                if (destObj == nullptr) {
                    destObj = new ORegex(reinterpret_cast<qobjinst>(copyInfo->mDestinationObject), srcObject);
                    ECOinsertNVObject(eci->mOmnisInstance, copyInfo->mDestinationObject, reinterpret_cast<void *>(destObj));
                }
                else {
                    destObj->setObject(reinterpret_cast<qobjinst>(copyInfo->mDestinationObject), srcObject);
                }
            }
            break;
        }

        case ECM_GETMETHODNAME:
            if (eci->mCompId == cObject_Regex) {
                return ECOreturnMethods(gInstLib, eci, &oRegexFunctions[0], cFunctionsCount);
            }
			return qtrue;	

        case ECM_METHODCALL: {
            void *object = ECOfindNVObject(eci->mOmnisInstance, lParam);
            if (object) {
                if (eci->mCompId == cObject_Regex) {
                    ORegex *taskDialog = reinterpret_cast<ORegex *>(object);
                    return taskDialog->methodCall(eci);
                }
            }
            return qfalse;
        }

        case ECM_CONSTPREFIX: {
            EXTfldval exfldval;
            EXTParamInfo *newparam = ECOaddParam(eci, &exfldval);
            str80 conPrefix;
            RESloadString(gInstLib, RES_CONST_PREFIX, conPrefix);
            exfldval.setChar(conPrefix);
            return qtrue;
        }

		case ECM_GETCONSTNAME:
		    return ECOreturnConstants(gInstLib, eci, RES_CONST_START, RES_CONST_END);

        case ECM_GETCOMPLIBINFO:
			return ECOreturnCompInfo(gInstLib, eci, LIB_RES_NAME, 0);

		case ECM_ISUNICODE:
			return qtrue;

        case ECM_GETVERSION:
            return ECOreturnVersion(VERSION_MAJOR, VERSION_MINOR);
    }

	return WNDdefWindowProc(hwnd, Msg, wParam, lParam, eci);
}

//--------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------
ORegex::ORegex(qobjinst pObjPtr):
    mObjPtr(pObjPtr),
    flags(0)
{
};

//--------------------------------------------------------------------------------
// Copy constructor (sort of)
//--------------------------------------------------------------------------------
ORegex::ORegex(qobjinst pObjPtr, const ORegex *sourceObject)
{
    setObject(pObjPtr, sourceObject);
};

//--------------------------------------------------------------------------------
// Copy object
//--------------------------------------------------------------------------------
void ORegex::setObject(qobjinst pObjPtr, const ORegex *sourceObject)
{
    mObjPtr = sourceObject->mObjPtr;
    flags = sourceObject->flags;
    pattern = sourceObject->pattern;
    errorMessage = sourceObject->errorMessage;
};

//--------------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------------
ORegex::~ORegex()
{
};

//--------------------------------------------------------------------------------
// Entry method for calling object's methods
//--------------------------------------------------------------------------------
qbool ORegex::methodCall(EXTCompInfo *pEci)
{
    qlong funcId = ECOgetId(pEci);

    if (funcId == cGetErrorMessageFunction) {
        getErrorMessage(pEci);
        return qtrue;
    }

    EXTfldval rtnVal;
    qbool rtnCode = qfalse;
    errorMessage = L"";

    switch (funcId) {
        case cSetPatternFunction:
            rtnCode = setPattern(pEci);
            break;
        case cSetFlagsFunction:
            rtnCode = setFlags(pEci);
            break;
        case cMatchFunction:
            rtnCode = match(pEci);
            break;
        case cMatchResultsFunction:
            rtnCode = matchResults(pEci);
            break;
        case cSearchFunction:
            rtnCode = search(pEci);
            break;
        case cReplaceFunction:
            rtnCode = replace(pEci);
            break;
        case cSplitFunction:
            rtnCode = split(pEci);
            break;
    }

    rtnVal.setBool((rtnCode == qtrue) ? preBoolTrue : preBoolFalse);
    ECOaddParam(pEci, &rtnVal);
    return rtnCode;
}

//--------------------------------------------------------------------------------
// Returns the error message generated in the last function called.
//--------------------------------------------------------------------------------
void ORegex::getErrorMessage(EXTCompInfo *pEci)
{
    EXTfldval result;
    CHRconvFromOs cConv(reinterpret_cast<qoschar *>(const_cast<wchar_t *>(errorMessage.c_str())));
    result.setChar(cConv.dataPtr(), cConv.len());
    ECOaddParam(pEci, &result);
}

//--------------------------------------------------------------------------------
// Set the pattern of the regex object.
//--------------------------------------------------------------------------------
qbool ORegex::setPattern(EXTCompInfo *pEci)
{
    if (!getParamString(pEci, 1, pattern)) {
        errorMessage = L"Incorrect number of parameters.";
        return qfalse;
    }
    return qtrue;
}

//--------------------------------------------------------------------------------
// Set the flags of the regex object.
//--------------------------------------------------------------------------------
qbool ORegex::setFlags(EXTCompInfo *pEci)
{
    if (!getParamInt(pEci, 1, flags)) {
        errorMessage = L"Incorrect number of parameters.";
        return qfalse;
    }
    return qtrue;
}

//--------------------------------------------------------------------------------
// Calls the regex_match algorithm
//--------------------------------------------------------------------------------
qbool ORegex::match(EXTCompInfo *pEci)
{
    std::wstring str;
    std::wregex rgx;

    if (!getParamString(pEci, 1, str)) {
        errorMessage = L"Incorrect number of parameters.";
        return qfalse;
    }

    EXTParamInfo *param = ECOfindParamNum(pEci, 2);
	if (!param) {
        errorMessage = L"Incorrect number of parameters.";
        return qfalse;
	}

    std::regex::flag_type flgs = std::regex::ECMAScript;
    if (flags & kRegexFlagBasic)
        flgs |= std::regex::basic;
    if (flags & kRegexFlagExtended)
        flgs |= std::regex::extended;
    if (flags & kRegexFlagIcase)
        flgs |= std::regex::icase;

    try {
        rgx.assign(pattern, flgs);
    }
    catch (const std::regex_error &ex) {
        char2wstring(ex.what(), errorMessage);
        return qfalse;
    }

    bool rc = std::regex_match(str, rgx);

    EXTfldval returnField(reinterpret_cast<qfldval>(param->mData));
    returnField.setBool((rc) ? preBoolTrue : preBoolFalse);
	ECOsetParameterChanged(pEci, 2);

    return qtrue;
}

//--------------------------------------------------------------------------------
// Calls the regex_match algorithm
//--------------------------------------------------------------------------------
qbool ORegex::matchResults(EXTCompInfo *pEci)
{
    std::wstring str;
    std::wregex rgx;
    std::wsmatch mtch;

    if (!getParamString(pEci, 1, str)) {
        errorMessage = L"Incorrect number of parameters.";
        return qfalse;
    }

    EXTParamInfo *paramRet = ECOfindParamNum(pEci, 2);
	if (!paramRet) {
        errorMessage = L"Incorrect number of parameters.";
        return qfalse;
	}

    EXTParamInfo *paramRetList = ECOfindParamNum(pEci, 3);
    if (!paramRetList) {
        errorMessage = L"Incorrect number of parameters.";
        return qfalse;
    }

    EXTfldval fval(reinterpret_cast<qfldval>(paramRetList->mData));
    std::unique_ptr<EXTqlist> retList(fval.getList(qfalse));
    if (!retList.get()) {
        errorMessage = L"Param number 3 is not a list.";
        return qfalse;
    }

    retList->clear(listVlen);
    retList->addCol(fftCharacter, dpFcharacter, 0, &str255(L"expression"));

    try {
        rgx.assign(pattern, buildFlagType());
    }
    catch (const std::regex_error &ex) {
        char2wstring(ex.what(), errorMessage);
        return qfalse;
    }

    EXTfldval column;
    bool rc = std::regex_match(str, mtch, rgx);
    if (rc) {
        for (size_t i = 0; i < mtch.size(); ++i) {
            //std::wssub_match sub_match = mtch[i];
            qlong lineNumber = retList->insertRow();
            retList->getColValRef(lineNumber, 1, column, qtrue);

            str255 stringTempOmnis(mtch[i].str().c_str());
            column.setChar(stringTempOmnis);
        }
    }

    EXTfldval returnField(reinterpret_cast<qfldval>(paramRet->mData));
    returnField.setBool((rc) ? preBoolTrue : preBoolFalse);
	ECOsetParameterChanged(pEci, 2);
    ECOsetParameterChanged(pEci, 3);
    return qtrue;
}

//--------------------------------------------------------------------------------
// Calls the regex_search algorithm
//--------------------------------------------------------------------------------
qbool ORegex::search(EXTCompInfo *pEci)
{
    std::wstring str;
    std::wregex rgx;
    std::wsmatch mtch;

    if (!getParamString(pEci, 1, str)) {
        errorMessage = L"Incorrect number of parameters.";
        return qfalse;
    }

    EXTParamInfo *paramRetList = ECOfindParamNum(pEci, 2);
    if (!paramRetList) {
        errorMessage = L"Incorrect number of parameters.";
        return qfalse;
    }

    EXTfldval fval(reinterpret_cast<qfldval>(paramRetList->mData));
    std::unique_ptr<EXTqlist> retList(fval.getList(qfalse));
    if (!retList.get()) {
        errorMessage = L"Param number 2 is not a list.";
        return qfalse;
    }

    retList->clear(listVlen);
    retList->addCol(fftCharacter, dpFcharacter, 0, &str255(L"expression"));
    retList->addCol(fftNumber,    dpFsinteger,  0, &str255(L"position"));

    try {
        rgx.assign(pattern, buildFlagType());
    }
    catch (const std::regex_error &ex) {
        char2wstring(ex.what(), errorMessage);
        return qfalse;
    }

    EXTfldval column;
    int pos = 0;
    while (std::regex_search(str, mtch, rgx)) {
        qlong lineNumber = retList->insertRow();

        retList->getColValRef(lineNumber, 1, column, qtrue);
        str255 stringTempOmnis(mtch[0].str().c_str());
        column.setChar(stringTempOmnis);

        pos += mtch.prefix().length();
        retList->getColValRef(lineNumber, 2, column, qtrue);
        column.setLong(pos + 1);
        pos += mtch[0].length();

        str = mtch.suffix();
    }

	ECOsetParameterChanged(pEci, 2);
    return qtrue;
}

//--------------------------------------------------------------------------------
// Calls the regex_replace algorithm
//--------------------------------------------------------------------------------
qbool ORegex::replace(EXTCompInfo *pEci)
{
    std::wstring sourceStr;
    std::wstring replacementStr;
    std::wregex rgx;

    if (!getParamString(pEci, 1, sourceStr)) {
        errorMessage = L"Incorrect number of parameters.";
        return qfalse;
    }

    if (!getParamString(pEci, 2, replacementStr)) {
        errorMessage = L"Incorrect number of parameters.";
        return qfalse;
    }

    EXTParamInfo *paramRetField = ECOfindParamNum(pEci, 3);
	if (!paramRetField) {
        errorMessage = L"Incorrect number of parameters.";
        return qfalse;
	}

    try {
        rgx.assign(pattern, buildFlagType());
    }
    catch (const std::regex_error &ex) {
        char2wstring(ex.what(), errorMessage);
        return qfalse;
    }

    std::wstring destinationStr = std::regex_replace(sourceStr, rgx, replacementStr);

    EXTfldval retField(reinterpret_cast<qfldval>(paramRetField->mData));
    CHRconvFromOs cConv(reinterpret_cast<qoschar *>(const_cast<wchar_t *>(destinationStr.c_str())));
    retField.setChar(cConv.dataPtr(), cConv.len());
	ECOsetParameterChanged(pEci, 3);
    return qtrue;
}

//--------------------------------------------------------------------------------
// Splits a string using the regex_token_iterator
//--------------------------------------------------------------------------------
qbool ORegex::split(EXTCompInfo *pEci)
{
    std::wstring str;
    std::wregex rgx;

    if (!getParamString(pEci, 1, str)) {
        errorMessage = L"Incorrect number of parameters.";
        return qfalse;
    }

    EXTParamInfo *paramRetList = ECOfindParamNum(pEci, 2);
    if (!paramRetList) {
        errorMessage = L"Incorrect number of parameters.";
        return qfalse;
    }

    EXTfldval fval(reinterpret_cast<qfldval>(paramRetList->mData));
    std::unique_ptr<EXTqlist> retList(fval.getList(qfalse));
    if (!retList.get()) {
        errorMessage = L"Param number 2 is not a list.";
        return qfalse;
    }

    retList->clear(listVlen);
    retList->addCol(fftCharacter, dpFcharacter, 0, &str255(L"item"));

    try {
        rgx.assign(pattern, buildFlagType());
    }
    catch (const std::regex_error &ex) {
        char2wstring(ex.what(), errorMessage);
        return qfalse;
    }

    std::wsregex_token_iterator it(str.begin(), str.end(), rgx, -1);
    std::wsregex_token_iterator end;
    EXTfldval column;

    while (it != end) {
        qlong lineNumber = retList->insertRow();
        retList->getColValRef(lineNumber, 1, column, qtrue);
        str255 stringTempOmnis(it->str().c_str());
        column.setChar(stringTempOmnis);
        ++it;
    }

	ECOsetParameterChanged(pEci, 2);
    return qtrue;
}

//--------------------------------------------------------------------------------
// Build the value for the flag parameter.
//--------------------------------------------------------------------------------

std::regex::flag_type ORegex::buildFlagType()
{
    std::regex::flag_type flgs = std::regex::ECMAScript;

    if (flags & kRegexFlagBasic)
        flgs |= std::regex::basic;
    if (flags & kRegexFlagExtended)
        flgs |= std::regex::extended;
    if (flags & kRegexFlagIcase)
        flgs |= std::regex::icase;

    return flgs;
}

//--------------------------------------------------------------------------------
// Utility functions
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// Reads one parameter of type wstring from an EXTCompInfo structure
//--------------------------------------------------------------------------------
bool getParamString(EXTCompInfo *pEci, long paramID, std::wstring &valor)
{
    EXTParamInfo *param = ECOfindParamNum(pEci, paramID);
    if (!param)
        return false;

    EXTfldval paramTexto(reinterpret_cast<qfldval>(param->mData));
    if (paramTexto.isNull())
        return false;

    std::unique_ptr<qchar[]> textoTmp(new qchar[paramTexto.getCharLen() + sizeof(qchar)]);
    qlong lenTransf;

    paramTexto.getChar(paramTexto.getCharLen(), textoTmp.get(), lenTransf);
    textoTmp[paramTexto.getCharLen()] = 0;

    CHRconvToOs cConv(textoTmp.get());
    valor = reinterpret_cast<wchar_t *>(cConv.dataPtr());
    return true;
}

//--------------------------------------------------------------------------------
// Reads one parameter of type int from an EXTCompInfo structure
//--------------------------------------------------------------------------------
bool getParamInt(EXTCompInfo *eci, long paramID, int &valor)
{
    EXTParamInfo *param = ECOfindParamNum(eci, paramID);
	if (!param) 
        return false;

    EXTfldval paramInt(reinterpret_cast<qfldval>(param->mData));
    valor = paramInt.getLong();
    return true;
}

//--------------------------------------------------------------------------------
// Converts from std::string to std::wstring
//
// TODO: Use a std::wstringstream object instead.
//--------------------------------------------------------------------------------
errno_t char2wstring(const char *source, std::wstring &destination)
{
    size_t sourceSize = strlen(source);

    std::unique_ptr<wchar_t[]> buffer(new wchar_t[sourceSize + 1]);
    size_t convertedChars = 0;
    errno_t rc = mbstowcs_s(&convertedChars, buffer.get(), sourceSize + 1, source, _TRUNCATE);

    destination = buffer.get();
    return rc;

    //std::wstringstream sstr;
    //sstr << source;
    //destination = sstr.str();
    //return 0;
}

//--------------------------------------------------------------------------------
// Copiar uma vari�vel do tipo wstring para a estrutura EXTCompInfo
//--------------------------------------------------------------------------------
/*
bool setChar(EXTCompInfo *pEci, const qshort paramID, const std::wstring &orig)
{
    EXTParamInfo *param = ECOfindParamNum(pEci, paramID);
    if (!param)
        return false;

    EXTfldval paramTexto(reinterpret_cast<qfldval>(param->mData));

    CHRconvFromOs cConv(reinterpret_cast<qoschar *>(const_cast<wchar_t *>(orig.c_str())));
    paramTexto.setChar(cConv.dataPtr(), cConv.len());

    ECOsetParameterChanged(pEci, paramID);
    return true;
}
*/