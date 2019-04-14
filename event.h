#ifndef __MLHMZ_EVENT__H__
#define __MLHMZ_EVENT__H__

namespace Mlhmz{

enum class EventType{
    EVT_ACCEPT = 0,
    EVT_RECV = 1,
    EVT_SEND = 2
};

class EventBase{
public:
    explicit EventBase() = default;
    explicit EventBase(EventType et, int fd):evtType(et),fd_(fd)
    {
    };
    EventType type() const{
        return evtType;
    }
    int fd() { return fd_;};
    virtual ~EventBase() {}
private:
    EventType evtType;
    int fd_;
};

}

#endif