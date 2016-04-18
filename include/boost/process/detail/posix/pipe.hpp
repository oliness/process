// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_POSIX_PIPE_HPP
#define BOOST_PROCESS_POSIX_PIPE_HPP


#include <boost/filesystem.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <system_error>
#include <array>
#include <unistd.h>


namespace boost { namespace process { namespace detail { namespace posix {



class pipe
{
    int _source;
    int _sink;
    std::string _pipe_name;
protected:
    pipe(int source, int sink) : _source(source), _sink(sink) {}
    pipe(int source, int sink, const std::string & name) : _source(source), _sink(sink), _pipe_name(name) {}
public:
    pipe() : pipe(create()) {}
    pipe(const pipe& ) = delete;
    pipe(pipe&& lhs)  : _source(lhs._source), _sink(lhs._sink), _pipe_name(std::move(lhs._pipe_name))
    {
        lhs._source = 0;
        lhs._sink = 0;
        lhs._pipe_name.clear();
    }
    pipe& operator=(const pipe& ) = delete;
    pipe& operator=(pipe&& lhs)
    {
        _source = 0;
        _sink   = 0;
        _pipe_name.clear();

        lhs._source = _source;
        lhs._sink   = _sink;
        lhs._pipe_name.clear();

        return *this;
    }
    ~pipe()
    {
        if (_sink   != 0)
            ::close(_sink);
        if (_source != 0)
            ::close(_source);
        if (!_pipe_name.empty())
            ::unlink(_pipe_name.c_str());
    }
    int source() const {return _source;}
    int sink  () const {return _sink;}

    static pipe create()
    {
        int fds[2];
        if (::pipe(fds) == -1)
            boost::process::detail::throw_last_error("pipe(2) failed");

        return pipe(fds[0], fds[1]);
    }

    static pipe create(std::error_code &ec)
    {
        int fds[2];
        if (::pipe(fds) == -1)
            ec = boost::process::detail::get_last_error();

        return pipe(fds[0], fds[1]);

    }

    static std::string make_pipe_name();

    inline static pipe create_named(const std::string & name = make_pipe_name());
    inline static pipe create_named(const std::string & name, std::error_code & ec);
    inline static pipe create_named(std::error_code & ec)
    {
        return create_named(make_pipe_name(), ec);
    }

    inline static pipe create_async()                     {return create();  }
    inline static pipe create_async(std::error_code & ec) {return create(ec);}

};


std::string pipe::make_pipe_name()
{
    std::string name = "/tmp/boost_process_auto_pipe_";

    namespace fs = boost::filesystem;
    fs::path p;

    do
    {
        static unsigned long long int i = 0;
        p = name + std::to_string(i);
        i++;
    }
    while (fs::exists(p)); //so it limits it to 2^31 pipes. should suffice.
    return p.string();
}


pipe pipe::create_named(const std::string & name)
{
    auto fifo = mkfifo(name.c_str(), 0666 );
            
    if (fifo != 0) 
        boost::process::detail::throw_last_error("mkfifo() failed");

    
    int  read_fd = open(name.c_str(), O_RDWR);
        
    if (read_fd == -1)
        boost::process::detail::throw_last_error();
    
    int write_fd = dup(read_fd);
    
    if (write_fd == -1)
        boost::process::detail::throw_last_error();

    return pipe(read_fd, write_fd, name);

}

pipe pipe::create_named(const std::string & name, std::error_code & ec)
{
    auto fifo = mkfifo(name.c_str(), 0666 );
            
    if (fifo != 0) 
        ec = boost::process::detail::get_last_error();


    int  read_fd = open(name.c_str(), O_RDONLY);
    
    if (read_fd == -1)
        ec = boost::process::detail::get_last_error();

    int write_fd = dup(read_fd);

    if (write_fd == -1)
        ec = boost::process::detail::get_last_error();
    
    return pipe(read_fd, write_fd, name);
}

typedef boost::asio::posix::stream_descriptor async_pipe_handle;


}}}}

#endif
