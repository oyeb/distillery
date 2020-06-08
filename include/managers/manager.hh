/**
 * @file    manager.hh
 * @author  Ajeesh Kumar K (ajeeshmsd@gmail.com)
 * @brief   Manger that manages all the resources.
 * @version 0.1
 * @date    2019-10-08
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef RTL_TRANSLATION_VALIDATION_MANAGER_HH
#define RTL_TRANSLATION_VALIDATION_MANAGER_HH

#include <pre-compiled-header.hh>

#include <memory>

#include <utils/logger.hh>
#include <utils/profiler.hh>

#include <managers/pass-manager.hh>

namespace distillery
{
    /**
     * @brief 
     * 
     */
    class Manager
    {
    private:
        Logger m_profiler_logger;

        Logger m_logger;

        Profiler m_manager_profiler;

        PassManager m_pass_manager;

        static std::unique_ptr<Manager> m_instance;
    public:

        Manager();

        Logger& get_profiler_logger();

        Logger& get_logger();

        PassManager& get_pass_manager();

        static Manager& get_instance();
    };
    
} // namespace distillery::managers

#define TV_RTL_MANAGER tv_rtl::Manager::get_instance()
#define TV_RTL_PROFILER_LOGGER TV_RTL_MANAGER.get_profiler_logger()

/**
 * @brief - Used to profile the current scope.
 *        - The time will be calculated from the position of this statement to the end of scope.
 *        - Can only use this once in a scope.
 *
 */
#define TV_RTL_PROFILE_THIS_SCOPE(msg) tv_rtl::Profiler tv_rtl_scope_profiler##__LINE__(TV_RTL_PROFILER_LOGGER, msg)
#define TV_RTL_PROFILER(name, msg) tv_rtl::Profiler name (TV_RTL_PROFILER_LOGGER, msg)


#define  TV_RTL_LOGGER TV_RTL_MANAGER.get_logger()

#endif  // RTL_TRANSLATION_VALIDATION_MANAGER_HH