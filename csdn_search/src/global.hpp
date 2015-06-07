#ifndef __GLOBAL_HPP__
#define __GLOBAL_HPP__
// ==================== includes ====================

// standard cpp libraries
#include <map>
#include <set>
#include <list>
#include <stack>
#include <vector>
#include <string>
#include <utility>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <exception>

// standard c libraries
#include <ctime>
#include <cstdlib>
#include <cstring>

// system libraries
#include <sys/time.h>

// boost libraries
#include <boost/any.hpp>
#include <boost/format.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>    


// zlog library
#include <zlog.h>

// event
#include <event.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>

// mysqlcppconn
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>


// linux standard libraries
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>

// json cpp
#include <json/json.h>


#include "define.hpp"

#endif	// __GLOBAL_HPP__
