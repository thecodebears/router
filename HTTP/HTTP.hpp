#pragma once

#include <string>

namespace HTTP
{
	enum REQTYPE
	{
		POST = 1,
		GET,
		PUT
	};

	#ifndef ONLYMETHODANDURI
	enum CONTYPE
	{
		JSON = 1,
	};
	#endif

	class Request
	{
		public:
			Request();
			#ifndef ONLYMETHODANDURI 
			Request(REQTYPE rtype, CONTYPE contype, std::string route, size_t sz, std::string req);
			#endif
			#ifdef ONLYMETHODANDURI
			Request(REQTYPE rtype, std::string route);
			#endif

			REQTYPE 	getRequestType();
			#ifndef ONLYMETHODANDURI
			CONTYPE 	getContentType();			
			size_t 		getRequestLength();
			std::string getRequest();
			#endif
			std::string getRoute();
			std::string getRequestText();

			void 		parse();
			void		parse(std::string request);

		private:
			REQTYPE 	m_reqtype;
			std::string m_raw_request;
			std::string m_route;

			#ifndef ONLYMETHODANDURI
			size_t 		m_length;
			CONTYPE 	m_contype;
			std::string m_request;
			#endif
	};
}
