/**
 * @file 		logger.hh
 * @author		Ajeesh Kumar K (ajeeshmsd@gmail.com)
 * @brief 		Utility logger
 * @version 	0.1
 * @date 		2019-10-01
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef RTL_TRANSLATION_VALIDATION_LOGGER_HH
#define RTL_TRANSLATION_VALIDATION_LOGGER_HH

#include <ostream>
#include <fstream>

namespace distillery {

	/**
	 * @brief	A stream Buffer that simply ignores everything that is given to it.
	 * 			Source : https://stackoverflow.com/questions/11826554/standard-no-op-output-stream/11826666#11826666
	 */

    class NullStream : public std::ostream
    {
        class NullBuffer : public std::streambuf
        {
        public:
            int overflow(int c) final
            {
                return c;
            }
        }m_buffer;

    public:
        NullStream()
            :std::ostream(&m_buffer)
        {
        }
    };


	/**
	 * @brief Log levels in order of priority
	 */
    enum LogLevel 
	{
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        OUTPUT,
		CNT
    };

	/**
	 * @brief Simple logger
	 */
    class Logger
	{
	private:
		// Stream to log
        std::ostream *m_log_stream;
		
		// (/dev/null)
		NullStream m_null_stream;
        
		// Minimum level. Anything log info below will be logged by null logger
		const LogLevel m_min_log_level;
		// Current level. If log level CNT is used, earlier log level is continued
        LogLevel m_current_log_level;

		// Will print the log level along with the messages
        const bool m_should_print_headers;

		// Denotes if the logger is instantiated using a file. Need to close the file.
		const bool m_is_file;

        /**
         * @brief Get string representation of the log level
         *
         * @param log_level
         * @return std::string
         */
        static std::string get_as_string(LogLevel log_level)
        {
            switch(log_level) {
                case TRACE: return  "[TRACE ] ";
                case DEBUG: return  "[DEBUG ] ";
                case INFO:  return  "[INFO  ] ";
                case WARN:  return  "[WARN  ] ";
                case ERROR: return  "[ERROR ] ";
                case OUTPUT: return "[OUTPUT] ";
                case CNT: return "";
                default: return "";
            }
        }

    public:

        /**
		 * @brief Construct a new Logger object using a stream object
		 * 
		 * @param p_output_stream 
		 * @param p_min_log_level 
		 * @param p_print_headers 
		 */
        Logger(std::ostream& p_output_stream, LogLevel p_min_log_level, bool p_print_headers)
			:m_log_stream(&p_output_stream),
			 m_min_log_level(p_min_log_level),
			 m_current_log_level(p_min_log_level),
			 m_should_print_headers(p_print_headers),
			 m_is_file(false)
		{
        }

		/**
		 * @brief Construct a new Logger object using file name
		 * 
		 * @param p_file_name 
		 * @param p_min_log_level 
		 * @param p_print_headers 
		 */
		Logger(const std::string&  p_file_name, LogLevel p_min_log_level, bool p_print_headers)
		   :m_log_stream(new std::ofstream(p_file_name.c_str(), std::ios::out)),
			m_min_log_level(p_min_log_level),
			m_current_log_level(p_min_log_level),
			m_should_print_headers(p_print_headers),
			m_is_file(true)
		{
		}
		
		/**
		 * @brief Destroy the Logger object
		 * 		  Close the file if the logger was constructed by file name		
		 */
        ~Logger() 
		{
			if(m_is_file)
			{
			    // cannot dynamic_cast because of no-rtti flag
			    // static cast is OK as we are making sure that we can cast.
                static_cast<std::ofstream*>(m_log_stream)->close(); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
			}
		}

		/**
		 * @brief get the output stream of the logger
		 * 
		 * @param log_level - Log level of the message. if CNT, continue earlier level
		 * @return std::ostream& - m_log_stream : log_level is at least minimum log level
		 * 						 - m_null_stream : other wise
		 */
        std::ostream& LOG(LogLevel log_level = CNT)
		{
            m_current_log_level = log_level == CNT ? m_current_log_level : log_level;

            if(m_current_log_level < m_min_log_level)
            {
                return m_null_stream;
            }
            
			if(m_should_print_headers && log_level != CNT)
			{
				*m_log_stream << get_as_string(log_level);
			}
			
			return *m_log_stream;
        }
    };
}

#endif  // RTL_TRANSLATION_VALIDATION_LOGGER_HH