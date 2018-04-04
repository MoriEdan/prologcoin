#include "task.hpp"
#include "connection.hpp"

namespace prologcoin { namespace node {

using namespace prologcoin::common;

out_task::out_task(const char *description, out_connection &out,
		   void (*fn)(out_task &task) )
    : description_(description), out_(&out), env_(&out.env()), fn_(fn),
      state_(IDLE), when_(utime::now())
{
}

void out_task::reschedule(utime t)
{ connection().reschedule(*this, t); }

void out_task::reschedule_last()
{ connection().reschedule_last(*this); }

bool out_task::is_connected() const
{ return connection().is_connected(); }

const ip_service & out_task::ip() const
{ return connection().ip(); }

self_node & out_task::self()
{ return connection().self(); }

void out_task::stop()
{ connection().stop(); }

term out_task::get_result()
{
    static const con_cell result_3("result",3);

    term r = get_term();
    if (r.tag() != tag_t::STR) {
	return term();
    }
    if (!env().functor(r) == result_3) {
	return term();
    }
    return env().arg(r, 0);
}

void out_task::error(const reason_t &reason)
{
    connection().error(reason, "");
}

}}


