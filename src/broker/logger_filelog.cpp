#include "logger.hpp"
#include "errors.hpp"

#include <fstream>


namespace zbroker {

struct file_logger_t::file_logger_impl_t {
    file_logger_impl_t(const std::string& filename)
        : m_stream(filename.c_str(), std::ios_base::app)
    {
        if(!m_stream) {
            throw runtime_error_t("File (%s) opening failed", filename.c_str());
        }
    }

    void log(const log_message_t &msg) {
        m_stream << "[" << log_message_t::level_to_str(msg.level()) << "] ";
        m_stream << msg.value() << std::endl;
    }
private:
    std::ofstream m_stream;
};

file_logger_t::file_logger_t(const std::string &filename)
    : m_impl(new file_logger_impl_t(filename)) {

}

file_logger_t::~file_logger_t() {
}

void file_logger_t::log(const log_message_t &msg) {
    m_impl->log(msg);
}

} // zbroker
