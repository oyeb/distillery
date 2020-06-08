#include <utility>

/**
 * @file 		profiler.hh
 * @author		Ajeesh Kumar K (ajeeshmsd@gmail.com)
 * @brief 		Utility class for profiling
 * @version 	0.1
 * @date 		2019-10-01
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef RTL_TRANSLATION_VALIDATION_PROFILER_HH
#define RTL_TRANSLATION_VALIDATION_PROFILER_HH

#include <chrono>
#include <string>
#include <iomanip>

#include <utils/logger.hh>

namespace distillery{
    /**
     * @brief Utility profiler.
     *        Can be use as scope based profiler.
     *        Create a scope for the operations to be profiled and create a profiler object in the scope.
     *        Will get the time info when the scope ends and the destructor is called.
     *        Can also use print_elapsed_time to get the elapsed time from the creation of the object.
     *          
     *        Source : https://www.youtube.com/watch?v=YG4jexlSAjc
     */
    class Profiler
    {
    private:
        // Logger to be used for this profiler.
        distillery::Logger &m_logger;

        // Time at which the Profiler object was created.
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time_point;

        // Start time in long long
        long long m_start_time;

        // Message to be printed when the profiler is destructed.
        std::string m_message;

        // Log level of the profiler.
        LogLevel m_log_level;

    public:
        /**
         * @brief Construct a new Profiler object
         * 
         * @param p_logger 
         * @param p_message 
         * @param p_log_level 
         */
        explicit  Profiler(Logger &p_logger, std::string p_message = "", LogLevel p_log_level = DEBUG)
            :m_logger(p_logger),
             m_start_time_point(std::chrono::high_resolution_clock::now()),
             m_start_time(std::chrono::time_point_cast<std::chrono::microseconds>(m_start_time_point).time_since_epoch().count()),
             m_message(std::move(p_message)),
             m_log_level(p_log_level)
        {
        }

        /**
         * @brief Print the time from the creation of the Profiler object.
         * 
         * @param p_message Message to be printed
         */
        void print_elapsed_time(const std::string& p_message)
        {
            auto current_time_point = std::chrono::high_resolution_clock::now();
            auto current_time = std::chrono::time_point_cast<std::chrono::microseconds>(current_time_point).time_since_epoch().count();
            
            m_logger.LOG(m_log_level) << "[ " << std::setw(10) << current_time - m_start_time << " us ] " << p_message << std::endl;
        }

        /**
         * @brief Destroy the Profiler object
         *        Log the time before destruction
         */
        ~Profiler()
        {
            print_elapsed_time(m_message);
        }
    };
    
} // namespace distillery::utils

#endif // RTL_TRANSLATION_VALIDATION_PROFILER_HH