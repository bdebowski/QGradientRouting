//
// Generated file, do not edit! Created by nedtool 4.6 from statusFrame.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "statusFrame_m.h"

USING_NAMESPACE


// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




// Template rule for outputting std::vector<T> types
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

Register_Class(StatusFrame);

StatusFrame::StatusFrame(const char *name, int kind) : ::cPacket(name,kind)
{
    this->senderId_var = 0;
    this->energyRemaining_var = 0;
    this->qEValue_var = 0;
    this->qBValue_var = 0;
    this->numTxToBaseStation_var = 0;
    this->numHopsToBaseStation_var = 0;
}

StatusFrame::StatusFrame(const StatusFrame& other) : ::cPacket(other)
{
    copy(other);
}

StatusFrame::~StatusFrame()
{
}

StatusFrame& StatusFrame::operator=(const StatusFrame& other)
{
    if (this==&other) return *this;
    ::cPacket::operator=(other);
    copy(other);
    return *this;
}

void StatusFrame::copy(const StatusFrame& other)
{
    this->senderId_var = other.senderId_var;
    this->energyRemaining_var = other.energyRemaining_var;
    this->qEValue_var = other.qEValue_var;
    this->qBValue_var = other.qBValue_var;
    this->numTxToBaseStation_var = other.numTxToBaseStation_var;
    this->numHopsToBaseStation_var = other.numHopsToBaseStation_var;
}

void StatusFrame::parsimPack(cCommBuffer *b)
{
    ::cPacket::parsimPack(b);
    doPacking(b,this->senderId_var);
    doPacking(b,this->energyRemaining_var);
    doPacking(b,this->qEValue_var);
    doPacking(b,this->qBValue_var);
    doPacking(b,this->numTxToBaseStation_var);
    doPacking(b,this->numHopsToBaseStation_var);
}

void StatusFrame::parsimUnpack(cCommBuffer *b)
{
    ::cPacket::parsimUnpack(b);
    doUnpacking(b,this->senderId_var);
    doUnpacking(b,this->energyRemaining_var);
    doUnpacking(b,this->qEValue_var);
    doUnpacking(b,this->qBValue_var);
    doUnpacking(b,this->numTxToBaseStation_var);
    doUnpacking(b,this->numHopsToBaseStation_var);
}

int StatusFrame::getSenderId() const
{
    return senderId_var;
}

void StatusFrame::setSenderId(int senderId)
{
    this->senderId_var = senderId;
}

double StatusFrame::getEnergyRemaining() const
{
    return energyRemaining_var;
}

void StatusFrame::setEnergyRemaining(double energyRemaining)
{
    this->energyRemaining_var = energyRemaining;
}

double StatusFrame::getQEValue() const
{
    return qEValue_var;
}

void StatusFrame::setQEValue(double qEValue)
{
    this->qEValue_var = qEValue;
}

double StatusFrame::getQBValue() const
{
    return qBValue_var;
}

void StatusFrame::setQBValue(double qBValue)
{
    this->qBValue_var = qBValue;
}

double StatusFrame::getNumTxToBaseStation() const
{
    return numTxToBaseStation_var;
}

void StatusFrame::setNumTxToBaseStation(double numTxToBaseStation)
{
    this->numTxToBaseStation_var = numTxToBaseStation;
}

int StatusFrame::getNumHopsToBaseStation() const
{
    return numHopsToBaseStation_var;
}

void StatusFrame::setNumHopsToBaseStation(int numHopsToBaseStation)
{
    this->numHopsToBaseStation_var = numHopsToBaseStation;
}

class StatusFrameDescriptor : public cClassDescriptor
{
  public:
    StatusFrameDescriptor();
    virtual ~StatusFrameDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(StatusFrameDescriptor);

StatusFrameDescriptor::StatusFrameDescriptor() : cClassDescriptor("StatusFrame", "cPacket")
{
}

StatusFrameDescriptor::~StatusFrameDescriptor()
{
}

bool StatusFrameDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<StatusFrame *>(obj)!=NULL;
}

const char *StatusFrameDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int StatusFrameDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 6+basedesc->getFieldCount(object) : 6;
}

unsigned int StatusFrameDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<6) ? fieldTypeFlags[field] : 0;
}

const char *StatusFrameDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "senderId",
        "energyRemaining",
        "qEValue",
        "qBValue",
        "numTxToBaseStation",
        "numHopsToBaseStation",
    };
    return (field>=0 && field<6) ? fieldNames[field] : NULL;
}

int StatusFrameDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderId")==0) return base+0;
    if (fieldName[0]=='e' && strcmp(fieldName, "energyRemaining")==0) return base+1;
    if (fieldName[0]=='q' && strcmp(fieldName, "qEValue")==0) return base+2;
    if (fieldName[0]=='q' && strcmp(fieldName, "qBValue")==0) return base+3;
    if (fieldName[0]=='n' && strcmp(fieldName, "numTxToBaseStation")==0) return base+4;
    if (fieldName[0]=='n' && strcmp(fieldName, "numHopsToBaseStation")==0) return base+5;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *StatusFrameDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "double",
        "double",
        "double",
        "double",
        "int",
    };
    return (field>=0 && field<6) ? fieldTypeStrings[field] : NULL;
}

const char *StatusFrameDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int StatusFrameDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    StatusFrame *pp = (StatusFrame *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string StatusFrameDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    StatusFrame *pp = (StatusFrame *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getSenderId());
        case 1: return double2string(pp->getEnergyRemaining());
        case 2: return double2string(pp->getQEValue());
        case 3: return double2string(pp->getQBValue());
        case 4: return double2string(pp->getNumTxToBaseStation());
        case 5: return long2string(pp->getNumHopsToBaseStation());
        default: return "";
    }
}

bool StatusFrameDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    StatusFrame *pp = (StatusFrame *)object; (void)pp;
    switch (field) {
        case 0: pp->setSenderId(string2long(value)); return true;
        case 1: pp->setEnergyRemaining(string2double(value)); return true;
        case 2: pp->setQEValue(string2double(value)); return true;
        case 3: pp->setQBValue(string2double(value)); return true;
        case 4: pp->setNumTxToBaseStation(string2double(value)); return true;
        case 5: pp->setNumHopsToBaseStation(string2long(value)); return true;
        default: return false;
    }
}

const char *StatusFrameDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *StatusFrameDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    StatusFrame *pp = (StatusFrame *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


