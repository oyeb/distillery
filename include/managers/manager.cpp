#include <managers/manager.hh>

#include <memory>

#include <utils/logger.hh>
#include <utils/profiler.hh>


using namespace distillery;

Manager::Manager()
            : m_profiler_logger("profiling.log", DEBUG, false),
              m_logger("rtl_tv.log", TRACE, true),
              m_manager_profiler(m_profiler_logger, "Lifetime of Manager")
        {
        }

distillery::Logger& Manager::get_profiler_logger()
{
    return m_profiler_logger;
}

distillery::Logger& Manager::get_logger()
{
    return m_logger;
}

Manager& Manager::get_instance()
{
    if(!m_instance) m_instance = std::unique_ptr<Manager>(new Manager);

    return *m_instance;
}

PassManager& Manager::get_pass_manager()
{
    return m_pass_manager;
}


std::unique_ptr<distillery::Manager> distillery::Manager::m_instance = nullptr;