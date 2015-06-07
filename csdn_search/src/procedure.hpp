#ifndef __PROCEDURE_HPP__
#define __PROCEDURE_HPP__
#include "global.hpp"

void any_cb(evhttp_request* req, void* arg);

void search_cb(evhttp_request* req, void* arg);

void suggest_cb(evhttp_request* req, void* arg);

#endif//__PROCEDURE_HPP__
