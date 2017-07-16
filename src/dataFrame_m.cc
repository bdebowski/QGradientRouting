//
// Generated file, do not edit! Created by nedtool 4.6 from dataFrame.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "dataFrame_m.h"

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

Register_Class(DataFrame);

DataFrame::DataFrame(const char *name, int kind) : ::cPacket(name,kind)
{
    this->destinationId_var = 0;
    this->nextHopId_var = 0;
    this->sourceId_var = 0;
    this->senderId_var = 0;
    this->protocol_var = 0;
    this->txAttemptsAllowedPerHop_var = 0;
    this->txAttemptsMadeThisHop_var = 0;
}

DataFrame::DataFrame(const DataFrame& other) : ::cPacket(other)
{
    copy(other);
}

DataFrame::~DataFrame()
{
}

DataFrame& DataFrame::operator=(const DataFrame& other)
{
    if (this==&other) return *this;
    ::cPacket::operator=(other);
    copy(other);
    return *this;
}

void DataFrame::copy(const DataFrame& other)
{
    this->destinationId_var = other.destinationId_var;
    this->nextHopId_var = other.nextHopId_var;
    this->sourceId_var = other.sourceId_var;
    this->senderId_var = other.senderId_var;
    this->protocol_var = other.protocol_var;
    this->txAttemptsAllowedPerHop_var = other.txAttemptsAllowedPerHop_var;
    this->txAttemptsMadeThisHop_var = other.txAttemptsMadeThisHop_var;
}

void DataFrame::parsimPack(cCommBuffer *b)
{
    ::cPacket::parsimPack(b);
    doPacking(b,this->destinationId_var);
    doPacking(b,this->nextHopId_var);
    doPacking(b,this->sourceId_var);
    doPacking(b,this->senderId_var);
    doPacking(b,this->protocol_var);
    doPacking(b,this->txAttemptsAllowedPerHop_var);
    doPacking(b,this->txAttemptsMadeThisHop_var);
}

void DataFrame::parsimUnpack(cCommBuffer *b)
{
    ::cPacket::parsimUnpack(b);
    doUnpacking(b,this->destinationId_var);
    doUnpacking(b,this->nextHopId_var);
    doUnpacking(b,this->sourceId_var);
    doUnpacking(b,this->senderId_var);
    doUnpacking(b,this->protocol_var);
    doUnpacking(b,this->txAttemptsAllowedPerHop_var);
    doUnpacking(b,this->txAttemptsMadeThisHop_var);
}

int DataFrame::getDestinationId() const
{
    return destinationId_var;
}

void DataFrame::setDestinationId(int destinationId)
{
    this->destinationId_var = destinationId;
}

int DataFrame::getNextHopId() const
{
    return nextHopId_var;
}

void DataFrame::setNextHopId(int nextHopId)
{
    this->nextHopId_var = nextHopId;
}

int DataFrame::getSourceId() const
{
    return sourceId_var;
}

void DataFrame::setSourceId(int sourceId)
{
    this->sourceId_var = sourceId;
}

int DataFrame::getSenderId() const
{
    return senderId_var;
}

void DataFrame::setSenderId(int senderId)
{
    this->senderId_var = senderId;
}

int DataFrame::getProtocol() const
{
    return protocol_var;
}

void DataFrame::setProtocol(int protocol)
{
    this->protocol_var = protocol;
}

int DataFrame::getTxAttemptsAllowedPerHop() const
{
    return txAttemptsAllowedPerHop_var;
}

void DataFrame::setTxAttemptsAllowedPerHop(int txAttemptsAllowedPerHop)
{
    this->txAttemptsAllowedPerHop_var = txAttemptsAllowedPerHop;
}

int DataFrame::getTxAttemptsMadeThisHop() const
{
    return txAttemptsMadeThisHop_var;
}

void DataFrame::setTxAttemptsMadeThisHop(int txAttemptsMadeThisHop)
{
    this->txAttemptsMadeThisHop_var = txAttemptsMadeThisHop;
}

class DataFrameDescriptor : public cClassDescriptor
{
  public:
    DataFrameDescriptor();
    virtual ~DataFrameDescriptor();

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

Register_ClassDescriptor(DataFrameDescriptor);

DataFrameDescriptor::DataFrameDescriptor() : cClassDescriptor("DataFrame", "cPacket")
{
}

DataFrameDescriptor::~DataFrameDescriptor()
{
}

bool DataFrameDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<DataFrame *>(obj)!=NULL;
}

const char *DataFrameDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int DataFrameDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 7+basedesc->getFieldCount(object) : 7;
}

unsigned int DataFrameDescriptor::getFieldTypeFlags(void *object, int field) const
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
        FD_ISEDITABLE,
    };
    return (field>=0 && field<7) ? fieldTypeFlags[field] : 0;
}

const char *DataFrameDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "destinationId",
        "nextHopId",
        "sourceId",
        "senderId",
        "protocol",
        "txAttemptsAllowedPerHop",
        "txAttemptsMadeThisHop",
    };
    return (field>=0 && field<7) ? fieldNames[field] : NULL;
}

int DataFrameDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destinationId")==0) return base+0;
    if (fieldName[0]=='n' && strcmp(fieldName, "nextHopId")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "sourceId")==0) return base+2;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderId")==0) return base+3;
    if (fieldName[0]=='p' && strcmp(fieldName, "protocol")==0) return base+4;
    if (fieldName[0]=='t' && strcmp(fieldName, "txAttemptsAllowedPerHop")==0) return base+5;
    if (fieldName[0]=='t' && strcmp(fieldName, "txAttemptsMadeThisHop")==0) return base+6;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *DataFrameDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
    };
    return (field>=0 && field<7) ? fieldTypeStrings[field] : NULL;
}

const char *DataFrameDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int DataFrameDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    DataFrame *pp = (DataFrame *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string DataFrameDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    DataFrame *pp = (DataFrame *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getDestinationId());
        case 1: return long2string(pp->getNextHopId());
        case 2: return long2string(pp->getSourceId());
        case 3: return long2string(pp->getSenderId());
        case 4: return long2string(pp->getProtocol());
        case 5: return long2string(pp->getTxAttemptsAllowedPerHop());
        case 6: return long2string(pp->getTxAttemptsMadeThisHop());
        default: return "";
    }
}

bool DataFrameDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    DataFrame *pp = (DataFrame *)object; (void)pp;
    switch (field) {
        case 0: pp->setDestinationId(string2long(value)); return true;
        case 1: pp->setNextHopId(string2long(value)); return true;
        case 2: pp->setSourceId(string2long(value)); return true;
        case 3: pp->setSenderId(string2long(value)); return true;
        case 4: pp->setProtocol(string2long(value)); return true;
        case 5: pp->setTxAttemptsAllowedPerHop(string2long(value)); return true;
        case 6: pp->setTxAttemptsMadeThisHop(string2long(value)); return true;
        default: return false;
    }
}

const char *DataFrameDescriptor::getFieldStructName(void *object, int field) const
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

void *DataFrameDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    DataFrame *pp = (DataFrame *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


