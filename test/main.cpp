//

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>

#include "gtest/gtest.h"

void initLog()
{
    boost::log::add_common_attributes();

    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);

    boost::log::add_console_log(std::cout,
                                boost::log::keywords::format =
                                    (boost::log::expressions::stream
                                     << boost::log::expressions::attr<unsigned int>("LineID") << ": {"
                                     << boost::log::trivial::severity << ","
                                     << boost::log::expressions::attr<std::string>("Channel") << "}"
                                     << " --- " << boost::log::expressions::smessage));
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    initLog();
    return RUN_ALL_TESTS();
}
