#ifndef OUTBREAK_UTIL_MULTIPLE_DISPATCH_H
#define OUTBREAK_UTIL_MULTIPLE_DISPATCH_H

#include <cassert>
#include <cstdint>
#include <typeinfo>

#include <QHash>

#define IMPLEMENT_DOUBLE_DISPATCH(type,parent)\
    static Util::DispatcherId Dispatcher_StaticId()\
    {\
        return (Util::DispatcherId)&typeid(type);\
    }\
    inline virtual Util::DispatcherId Dispatcher_Id(int depth=0)\
    {\
        assert(depth >= 0);\
        assert(depth > 0 || (typeid(*this) == typeid(type)));\
        return (depth == 0) ? Dispatcher_StaticId() :\
               (depth > 0)  ? parent::Dispatcher_Id(depth - 1) :\
                              NULL;\
    } enum { Dispatcher_##type##dummy1 }

#define IMPLEMENT_DOUBLE_DISPATCH_BASE(type)\
    static Util::DispatcherId Dispatcher_StaticId()\
    {\
        return (Util::DispatcherId)&typeid(type);\
    }\
    inline virtual Util::DispatcherId Dispatcher_Id(int depth=0)\
    {\
        return depth ? NULL : Dispatcher_StaticId();\
    } enum { Dispatcher_##type##dummy2 } 

namespace Util
{

typedef uintptr_t DispatcherId;

template< class BaseLhs, 
          class BaseRhs = BaseLhs, 
          typename ResultType = void
        >
class Dispatcher
{
public:
    typedef DispatcherId Id;

private:
    typedef ResultType (*CallbackType)(BaseLhs&, BaseRhs&);
    typedef QHash<Id, CallbackType> Row;
    typedef QHash<Id, Row> Matrix; 

public:

    template <class Lhs, class Rhs, ResultType (*callback)(Lhs&, Rhs&)>
    void Add()
    {
        struct Local 
        {
            static ResultType Reverse(BaseRhs& rhs, BaseLhs& lhs)
            {
                return callback(static_cast<Lhs&>(lhs), static_cast<Rhs&>(rhs));
            }
        };

        Id lhs_id = GetId<Lhs>();
        Id rhs_id = GetId<Rhs>();

        assert(m_callbacks[lhs_id][rhs_id] == 0);

        m_callbacks[lhs_id][rhs_id] = (CallbackType)callback;

        if (lhs_id != rhs_id)
        {
            m_callbacks[rhs_id][lhs_id] = &Local::Reverse;
        }
    }

    ResultType Dispatch(BaseLhs& lhs, BaseRhs& rhs) const
    {
        Id lhs_id = -1;
        Id rhs_id = -1;
        int d1 = 0;

        do 
        {
            lhs_id = lhs.Dispatcher_Id(d1++);
            int d2 = 0;
            do
            {
                rhs_id = rhs.Dispatcher_Id(d2++);

                typename Matrix::const_iterator it = m_callbacks.find(lhs_id);
                
                if (it != m_callbacks.end())
                {
                    typename Row::const_iterator jt = it->find(rhs_id);

                    if (jt != it->end())
                    {
                        return jt.value()(lhs, rhs);
                    }
                }

                CallbackType cb_p( m_callbacks[lhs_id][rhs_id] );
                if (cb_p != NULL)
                {
                    return cb_p(lhs, rhs);
                }
            } while (rhs_id != 0);
        } while (lhs_id != 0);

        return ResultType();
    }

private:
    template<class T>
    Id GetId() const
    {
        return (Id)&typeid(T);
    }

    Matrix m_callbacks;
};

} // namespace Util

#endif // OUTBREAK_UTIL_MULTIPLE_DISPATCH_H
