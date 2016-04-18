// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_POSIX_NULL_IN_HPP
#define BOOST_PROCESS_DETAIL_POSIX_NULL_IN_HPP

#include <boost/process/pipe.hpp>
#include <boost/process/detail/posix/handler.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <unistd.h>

namespace boost { namespace process { namespace detail { namespace posix {

struct null_in : handler_base_ext
{
    boost::iostreams::file_descriptor_source source{"/dev/null"};

public:
    template <class Executor>
    void on_exec_setup(Executor &e) const
    {
        ::dup2(source.handle(), STDIN_FILENO);
    }
};

}}}}

#endif
