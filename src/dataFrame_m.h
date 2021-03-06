//
// Generated file, do not edit! Created by nedtool 4.6 from dataFrame.msg.
//

#ifndef _DATAFRAME_M_H_
#define _DATAFRAME_M_H_

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0406
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>dataFrame.msg:19</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * packet DataFrame
 * {
 *     int destinationId;	// ultimate destination
 *     int nextHopId;		// next stop
 *     int sourceId;		// ultimate origin
 *     int senderId;		// node last visited
 * 
 *     int protocol;		// Routing protocol to use
 *     int txAttemptsAllowedPerHop;
 *     int txAttemptsMadeThisHop;
 * }
 * </pre>
 */
class DataFrame : public ::cPacket
{
  protected:
    int destinationId_var;
    int nextHopId_var;
    int sourceId_var;
    int senderId_var;
    int protocol_var;
    int txAttemptsAllowedPerHop_var;
    int txAttemptsMadeThisHop_var;

  private:
    void copy(const DataFrame& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const DataFrame&);

  public:
    DataFrame(const char *name=NULL, int kind=0);
    DataFrame(const DataFrame& other);
    virtual ~DataFrame();
    DataFrame& operator=(const DataFrame& other);
    virtual DataFrame *dup() const {return new DataFrame(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getDestinationId() const;
    virtual void setDestinationId(int destinationId);
    virtual int getNextHopId() const;
    virtual void setNextHopId(int nextHopId);
    virtual int getSourceId() const;
    virtual void setSourceId(int sourceId);
    virtual int getSenderId() const;
    virtual void setSenderId(int senderId);
    virtual int getProtocol() const;
    virtual void setProtocol(int protocol);
    virtual int getTxAttemptsAllowedPerHop() const;
    virtual void setTxAttemptsAllowedPerHop(int txAttemptsAllowedPerHop);
    virtual int getTxAttemptsMadeThisHop() const;
    virtual void setTxAttemptsMadeThisHop(int txAttemptsMadeThisHop);
};

inline void doPacking(cCommBuffer *b, DataFrame& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, DataFrame& obj) {obj.parsimUnpack(b);}


#endif // ifndef _DATAFRAME_M_H_

