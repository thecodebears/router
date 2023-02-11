#include "HTTP.hpp"

#include <string>
#include <sstream>

#ifndef ONLYMETHODANDURI
HTTP::Request::Request(HTTP::REQTYPE rtype, HTTP::CONTYPE contype, std::string route, size_t sz, std::string req)
{
	m_reqtype = rtype;
	m_contype = contype;
	m_route = route;
	m_length = sz;
	m_request = req;
}
#endif
#ifdef ONLYMETHODANDURI
HTTP::Request::Request(HTTP::REQTYPE rtype, std::string route)
{
	m_reqtype = rtype;
	m_route = route;
}
#endif

HTTP::Request::Request(){}

HTTP::REQTYPE HTTP::Request::getRequestType()
{
	return m_reqtype;
}

std::string HTTP::Request::getRoute()
{
	return m_route;
}

std::string HTTP::Request::getRequestText()
{
	return m_raw_request;
}

#ifndef ONLYMETHODANDURI
HTTP::CONTYPE HTTP::Request::getContentType()
{
	return m_contype;
}

size_t HTTP::Request::getRequestLength()
{
	return m_length;
}

std::string HTTP::Request::getRequest()
{
	return m_request;
}
#endif

void HTTP::Request::parse(std::string request)
{
	m_raw_request = request;
	parse();
} 

void HTTP::Request::parse()
{
	// Request type parsing
	if(m_raw_request.find("GET") != std::string::npos)
	{
		m_reqtype = HTTP::REQTYPE::GET;
	}
	else if(m_raw_request.find("POST") != std::string::npos)
	{
		m_reqtype = HTTP::REQTYPE::POST;
	}
	else if(m_raw_request.find("PUT") != std::string::npos)
	{
		m_reqtype = HTTP::REQTYPE::PUT;
	}

	#ifndef ONLYMETHODANDURI
	// Content type parsing
	if(m_raw_request.find("application/json") != std::string::npos)
	{
		m_contype = HTTP::CONTYPE::JSON;
	}

	size_t fpos = m_raw_request.find("Content-Length:") + 16;
	if(fpos <= 16)
	{
		fpos = m_raw_request.find("Content-length:") + 16;
	}

	m_length = std::atoi(m_raw_request.substr(fpos, fpos - m_raw_request.find("\n\r", fpos)).c_str());
	m_request = m_raw_request.substr(m_raw_request.find("{"));
	#endif

	std::istringstream _s_in(m_raw_request);
	for(std::uint8_t i = 0; i < 2; i++)
	{
		std::getline(_s_in, m_route, ' ');
	}
}
