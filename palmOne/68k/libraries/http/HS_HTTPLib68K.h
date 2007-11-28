/*
	HS_HTTPLib68K.h
	Copyright(c) 1996-2002 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/
/**
 * @defgroup HTTP HTTP Library
 *
 * @{
 * @}
 */
/**
 *
 * @ingroup HTTP
 *
*/

/**
 * @file 	HS_HTTPLib68K.h
 * @version 2.0
 * @date 	
 *
 * @brief Public 68k include file for the HTTP Library.
 * 
 *
 */



#ifndef HS_HTTPLIB68K_H__
#define HS_HTTPLIB68K_H__

#include <PalmTypes.h>
#include <LibTraps.h>

/**
 * Make sure to move the HS_inc folder in front of the following headers when merging
 * new Access code drops.
 **/
#include <Common/Libraries/HTTP/HS_HTTPLibApp.h>
#include <Common/Libraries/HTTP/HS_HTTPLibConst.h>

/* struct */

/**
 * structure for glueing
 **/

struct HS_HTTPLibSplit_ {
	Char *s;		/**<		*/
	Int32 len;		/**<		*/
};
typedef struct HS_HTTPLibSplit_ HS_HTTPLibSplit;

struct HS_HTTPLibTime_ {
	Int32 day;		/**< days (either backward or forward) since 01 January, 1970 UTC */
	Int32 msec;		/**< milliseconds since midnight [0-86399999] */
};
typedef struct HS_HTTPLibTime_ HS_HTTPLibTime;

typedef void *HS_HTTPLibDataHandle;	/**<		*/
typedef void *HS_HTTPLibVHandle;	/**<		*/
typedef void *HS_HTTPLibString;		/**<		*/
typedef void *HS_HTTPLibFixedString;	/**<		*/
typedef void *HS_HTTPLibURLString;	/**<		*/

/**
 *
 **/
struct HS_HTTPLibAuthInfo_ {
	HS_HTTPLibString fUser;		/**<		*/
	HS_HTTPLibString fPass;		/**<		*/
	HS_HTTPLibString fRealm;	/**<		*/
	HS_HTTPLibString fChallenge;	/**<		*/
	HS_HTTPLibString fNextNonce;	/**<		*/
	Int32 fNC;			/**<		*/
};
typedef struct HS_HTTPLibAuthInfo_ HS_HTTPLibAuthInfo;

/**
 * This sets the rules for HttpLib to use to select useragent for each request.
 * The rule should be set as little as possible so that it does not slow down
 * browser performance.
 **/
enum {
	httpUASelectTypeHost,		/**<		*/
	httpUASelectTypePath		/**<		*/
};

/**
 *
 **/
typedef struct {
	Int32 fType;		/**<		*/
	Char *fMatchString; 	/**< substring to be matched */
	Char *fUserAgent;	/**<		*/
} HttpUASelectionRule;


typedef void *HS_HTTPLibStream;			/**<		*/
typedef void *HS_HTTPLibSSL;			/**<		*/
typedef void *HS_HTTPLibCookies;		/**<		*/
typedef void *HS_HTTPLibCertPtr;		/**<		*/
typedef void *HS_HTTPLibCertList;		/**<		*/
typedef void *HS_HTTPLibClientCertList;		/**<		*/
typedef void *HS_HTTPLibSSLClassPtr;		/**<		*/

typedef Int32 (*HS_HTTPLibCookieConfirmProc)(HS_HTTPLibCookies in_cookie, HS_HTTPLibURLString in_url, Char *in_header, Int32 in_header_len, HS_HTTPLibOpaque in_aux);	/**<		*/
typedef Int32 (*HS_HTTPLibServerCertConfirmProc)(HS_HTTPLibSSL in_ssl, Int32 in_verify_result, HS_HTTPLibCertList in_list, HS_HTTPLibOpaque in_aux);	/**<		*/
typedef Int32 (*HS_HTTPLibClientCertSelectProc)(HS_HTTPLibSSL in_ssl, HS_HTTPLibClientCertList in_list, HS_HTTPLibOpaque in_aux);	/**<		*/
typedef Int32 (*HS_HTTPLibTunnelingCallbackProc)(HS_HTTPLibStream stream, Char* buf, Int32 len, void *in_aux);		/**<		*/
typedef void (*HS_HTTPLibWakeUpCallbackProc)(HS_HTTPLibOpaque in_aux);		/**<		*/

/* const */

/* API */

/**
 *  @brief This function opens the HTTP library and increments the open count. This
 *	   function will create and initialize the library’s common data at first opening.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @return Error code.
 **/
Err HS_HTTPLibOpen(UInt16 refnum)
		SYS_TRAP(kHTTPLibTrapOpen);

/**
 *  @brief This function decrements open count and finalizes the library’s common data at
 * 	   the last close.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param usecountP:	OUT: Open count
 *  @return Error code.
 **/
Err HS_HTTPLibClose(UInt16 refnum, UInt16* usecountP)
		SYS_TRAP(kHTTPLibTrapClose);

/**
 *  @brief Standard library sleep function.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @return Error code.
 **/
Err HS_HTTPLibSleep(UInt16 refnum)
		SYS_TRAP(kHTTPLibTrapSleep);

/**
 *  @brief Standard library wake function.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @return Error code.
 **/
Err HS_HTTPLibWake(UInt16 refnum)
		SYS_TRAP(kHTTPLibTrapWake);

/**
 *  @brief This function returns the open count of this library.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @return Open count.
 **/
UInt16 HS_HTTPLibOpenCount(UInt16 refnum)
		SYS_TRAP(kHTTPLibTrapOpenCount);

/**
 *  @brief This function initializes the library instance and sets application dependent
 *    	   information in the global variables. The return value is the handle of library
 *	   instance. If an error occurs during the initialization, the return value is NULL.
 *
 *  This call should be made after the application has called HS_HTTPLibOpen and
 *  assigned the HS_HTTPLibAppInfo, HS_HTTPLibNetLibInfo, and
 *  HS_HTTPLibPeer global variables with the application’s parameters. A reciprocal
 *  call to HS_HTTPLibFinalize should be made when the application is finished
 *  using the library.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param *appInfoP:	IN:
 *  @param *netLibInfoP: 	IN:
 *  @param *peerP: 	IN:
 *  @retval HS_HTTPLibHandle
 *	    non NULL if successful. NULL if failed
 **/
HS_HTTPLibHandle HS_HTTPLibInitialize(UInt16 refnum, HS_HTTPLibAppInfo *appInfoP, HS_HTTPLibNetLibInfo *netLibInfoP, HS_HTTPLibPeer *peerP)
		SYS_TRAP(kHTTPLibTrapInitialize);

/**
 *  @brief This function finalizes the library instance.
 *
 *  This call should be made when the application is finished using the library.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH:	IN:  The handle of HTTP library instance
 *  @return None.
 **/
void HS_HTTPLibFinalize(UInt16 refnum, HS_HTTPLibHandle libH)
		SYS_TRAP(kHTTPLibTrapFinalize);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param libH:	IN:
 *  @retval Err error code.
 **/
HS_HTTPLibSSLClassPtr HS_HTTPLibSSLClass(UInt16 refnum, HS_HTTPLibHandle libH)
		SYS_TRAP(kHTTPLibTrapSSLClass);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param libH:	IN:
 *  @param sslClassP:	IN:
 *  @retval Err error code.
 **/
void HS_HTTPLibRegisterSSLClass(UInt16 refnum, HS_HTTPLibHandle libH, HS_HTTPLibSSLClassPtr sslClassP)
		SYS_TRAP(kHTTPLibTrapRegisterSSLClass);

/**
 *  @brief This function clears the DNS cache information.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH:	IN:  The handle of HTTP library instance
 *  @return None.
 **/
void HS_HTTPLibClearDNSCache(UInt16 refnum, HS_HTTPLibHandle libH)
		SYS_TRAP(kHTTPLibTrapClearDNSCache);

/**
 *  @brief This function closes the all idle keep alive sockets relating this application. Nonidle
 *	   sockets are not closed by this function.
 *
 *  This call should be made at library finalization if keep-alive sockets are active.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH:	IN:  The handle of HTTP library instance
 *  @return None.
 **/
void HS_HTTPLibCloseAllKeepAlive(UInt16 refnum, HS_HTTPLibHandle libH)
		SYS_TRAP(kHTTPLibTrapCloseAllKeepAlive);

/**
 *  @brief Set the proxy information of this application to out_proxy_host,
 *	   out_proxy_http_port, out_proxy_https_port, out_no_proxy_hosts.
 *	   If out_proxy_host is NULL, the value of *out_proxy_http_port,
 *	   *out_proxy_https_port, *out_no_proxy_hosts are not set, otherwise the values of
 *	   *out_proxy_http_port, *out_proxy_https are set.
 *	   If out_no_proxy_hosts is NULL, *out_no_proxy_hosts is not set.
 *	   The format of out_no_proxy_hosts is the list separated by “,”.
 *	   The responsibility of delete *out_proxy_host, *out_no_proxy_hosts is caller.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH:	IN:  The handle of HTTP library instance
 *  @param *out_proxy_host:	OUT:  The pointer to handle of proxy host string
 *  @param *out_proxy_http_port:	OUT:  The pointer to HTTP port number
 *  @param *out_proxy_https_port:	OUT:  The pointer to HTTPS port number
 *  @param *out_no_proxy_hosts:	OUT:  The pointer to handle of the list string of hosts connect without using proxy server.
 *  @return None.
 **/
void HS_HTTPLibMakeProxy(UInt16 refnum, HS_HTTPLibHandle libH, HS_HTTPLibFixedString *out_proxy_host, Int32 *out_proxy_http_port, Int32 *out_proxy_https_port, HS_HTTPLibString *out_no_proxy_hosts)
		SYS_TRAP(kHTTPLibTrapMakeProxy);

/**
 *  @brief This function sets the default HTTP port number. If there is no indication in URL,
 *	   this port number is used for TCP connection.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH:	IN:  The handle of HTTP library instance
 *  @param in_port: 	IN:  The HTTP port number
 *  @return None.
 **/
void HS_HTTPLibSetHTTPPort(UInt16 refnum, HS_HTTPLibHandle libH, Int32 in_port)
		SYS_TRAP(kHTTPLibTrapSetHTTPPort);

/**
 *  @brief This function sets the default HTTPS port number. If there is no indication in https
 *	   URL, this port number is used for TCP connection.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH:	IN:  The handle of HTTP library instance
 *  @param in_port: 	IN:  The HTTPS port number
 *  @return None.
 **/
void HS_HTTPLibSetHTTPSPort(UInt16 refnum, HS_HTTPLibHandle libH, Int32 in_port)
		SYS_TRAP(kHTTPLibTrapSetHTTPSPort);

/**
 *  @brief This function sets the maximal size of Request-Line. If in_size is -1, the maximal
 *	   size is unlimited. Note that this function does not set the maximal size of Request-
 *	   Header. See RFC2616 for the definitions of Request-Line and Request-Header.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH:	IN:  The handle of HTTP library instance
 *  @param in_size: 	IN:  The maximal size (byte)
 *  @return None.
 **/
void HS_HTTPLibSetMaxRequestHeader(UInt16 refnum, HS_HTTPLibHandle libH, Int32 in_size)
		SYS_TRAP(kHTTPLibTrapSetMaxRequestHeader);

/**
 *  @brief This function sets the maximal size of Request-Body. If in_size is -1, the maximal
 *	   size is unlimited.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH:	IN:  The handle of HTTP library instance
 *  @param in_size: 	IN:  The maximal size (byte)
 *  @return None.
 **/
void HS_HTTPLibSetMaxRequestBody(UInt16 refnum, HS_HTTPLibHandle libH, Int32 in_size)
		SYS_TRAP(kHTTPLibTrapSetMaxRequestBody);

/**
 *  @brief This function sets the proxy server setting for this application. If in_proxy_host is
 *	   NULL, in_proxy_host, in_proxy_http_port, in_proxy_https_port are not set.
 *	   If in_no_proxy_hosts is NULL, in_no_proxy_hosts is not set. The format of
 *	   in_no_proxy_hosts is comma (“,”) separated list.
 *
 *  This call can be grouped with other proxy setting calls if the application is making
 *  the same type of request for all the requests.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH:	IN:  The handle of HTTP library instance
 *  @param *in_proxy_host: 	IN:  The proxy host name
 *  @param in_proxy_host_len:	IN:  The length of in_proxy_host string
 *  @param in_proxy_http_port:	IN:  The HTTP port of proxy server
 *  @param in_proxy_https_port:	IN:  The HTTPS port of proxy server
 *  @param *in_no_proxy_hosts:	IN:  The list of hosts not using proxy
 *  @param in_no_proxy_hosts_len:	IN:  The length of in_no_proxy_hosts
 *  @return
 *	    True if setting is succeed.
 *	    False if failed.(the main reason is memory shortage)
 **/
Boolean HS_HTTPLibSetProxy(UInt16 refnum, HS_HTTPLibHandle libH, Char *in_proxy_host, Int32 in_proxy_host_len, Int32 in_proxy_http_port, Int32 in_proxy_https_port, Char *in_no_proxy_hosts, Int32 in_no_proxy_hosts_len)
		SYS_TRAP(kHTTPLibTrapSetProxy);

/**
 *  @brief This function sets the UserAgent string for the application. If in_useragent is
 *	   NULL, setting is cleared.
 *
 *  This call allows the application to set the user agent identfication in the requests.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH:	IN:  The handle of HTTP library instance
 *  @param *in_useragent:	IN:  The string of User Agent
 *  @param in_useragent_len:	IN:  The length of in_useragent
 *  @return
 *	    True if setting is successful.
 *	    False if failed.(The main reason is memory shortage.)
 **/
Boolean HS_HTTPLibSetUserAgent(UInt16 refnum, HS_HTTPLibHandle libH, Char *in_useragent, Int32 in_useragent_len)
		SYS_TRAP(kHTTPLibTrapSetUserAgent);

/**
 *  @brief This function sets the default header, i.e. this string is added to the Request-header
 * 	   of the Request created at HS_HTTPLibStreamCreateRequest. If in_default_headers
 *	   is NULL, the setting is cleared.
 *
 *  This call allows the application to set the application defined standard headers
 *  without having the populate each request with HS_HTTPLibStreamAddHeader.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH:	IN:  The handle of HTTP library instance
 *  @param *in_default_headers: 	IN:  The string of default header
 *  @param in_default_headers_len:	IN:  The length of in_default_headers
 *  @return
 *	    True if setting is successful.
 *	    False if failed. (The main reason is memory shortage.)
 **/
Boolean HS_HTTPLibSetDefaultHeaders(UInt16 refnum, HS_HTTPLibHandle libH, Char *in_default_headers, Int32 in_default_headers_len)
		SYS_TRAP(kHTTPLibTrapSetDefaultHeaders);

/**
 *  @brief This function sets whether this application uses proxy server.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH:	IN:  The handle of HTTP library instance
 *  @param in_bool: 	IN:  True if using proxy server.
 *			     False if not using proxy server.
 *  @return None.
 **/
void HS_HTTPLibSetUseProxy(UInt16 refnum, HS_HTTPLibHandle libH, Boolean in_bool)
		SYS_TRAP(kHTTPLibTrapSetUseProxy);

/**
 *  @brief This function sets whether this application permits HTTP1.1 over proxy server. If
 *	   false is set, the application use HTTP/1.0.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH: 	IN:  The handle of HTTP library instance
 *  @param in_bool: 	IN:  True if permitting HTTP/1.1 over proxy server
 *			     False if not permitting HTTP/1.1 over proxy server
 *  @return None.
 **/
void HS_HTTPLibSetHTTP11OverProxy(UInt16 refnum, HS_HTTPLibHandle libH, Boolean in_bool)
		SYS_TRAP(kHTTPLibTrapSetHTTP11OverProxy);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param libH: 	IN:
 *  @param in_bool: 	IN:
 *  @retval Err error code.
 **/
void HS_HTTPLibSetSendReferer(UInt16 refnum, HS_HTTPLibHandle libH, Boolean in_bool)
		SYS_TRAP(kHTTPLibTrapSetSendReferer);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param libH: 	IN:
 *  @param in_bool: 	IN:
 *  @retval Err error code.
 **/
void HS_HTTPLibSetSendProxyKeepAlive(UInt16 refnum, HS_HTTPLibHandle libH, Boolean in_bool)
		SYS_TRAP(kHTTPLibTrapSetSendProxyKeepAlive);

/**
 *  @brief This function sets whether this application sends Cookie.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH: 	IN:  The handle of HTTP library instance
 *  @param in_bool: 	IN:  True if sending Cookie
 *			     False if not sending Cookie
 *  @return None.
 **/
void HS_HTTPLibSetSendCookie(UInt16 refnum, HS_HTTPLibHandle libH, Boolean in_bool)
		SYS_TRAP(kHTTPLibTrapSetSendCookie);

/**
 *  @brief This function sets the limit of Connection timeout. If in_msec is -1, no timeout is
 *	   set. The condition of timeout is in_msec has passed from TCP connect without
 *	   HTTP connection established.
 *
 *  With this function the different connection timeouts can be set at library
 *  initialization.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH:	IN:  The handle of HTTP library instance
 *  @param in_msec: 	IN:  Time Limit (msec)
 *  @retval Nothing
 **/
void HS_HTTPLibSetConnectTimeOut(UInt16 refnum, HS_HTTPLibHandle libH, Int32 in_msec)
		SYS_TRAP(kHTTPLibTrapSetConnectTimeOut);

/**
 *  @brief This function sets the limit of sending Request timeout. If in_msec is -1, no timeout
 *	   is set. The condition of timeout is in_msec has passed from the start of Request
 *	   sending without sending done.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH:	IN:  The handle of HTTP library instance
 *  @param in_msec: 	IN:  Time Limit (msec)
 *  @return None.
 **/
void HS_HTTPLibSetReqTimeOut(UInt16 refnum, HS_HTTPLibHandle libH, Int32 in_msec)
		SYS_TRAP(kHTTPLibTrapSetReqTimeOut);

/**
 *  @brief This function sets the limit of receiving Response timeout. If in_msec is -1, no
 *	   timeout is set. The condition of timeout is in_msec has passed from the end of
 *	   sending request without receiving Response. Or in_msec has passed from the last
 *	   receiving a part of Response without receiving any part of Response.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH:	IN:  The handle of HTTP library instance
 *  @param in_msec: 	IN:  Time Limit (msec)
 *  @retval Nothing
 **/
void HS_HTTPLibSetRspTimeOut(UInt16 refnum, HS_HTTPLibHandle libH, Int32 in_msec)
		SYS_TRAP(kHTTPLibTrapSetRspTimeOut);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param libH:	IN:
 *  @param in_cb: 	IN:
 *  @param *in_aux:	IN:
 *  @retval Err error code.
 **/
void HS_HTTPLibSetTunnelingCallback(UInt16 refnum, HS_HTTPLibHandle libH, HS_HTTPLibTunnelingCallbackProc in_cb, void *in_aux)
        SYS_TRAP(kHTTPLibTrapSetTunnelingCallback);

/**
 *  @brief This function returns whether this application uses proxy.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH:	IN:  The handle of HTTP library instance
 *  @return
 *	    True if this application uses proxy.
 *	    False if this application does not uses proxy.
 **/
Boolean HS_HTTPLibIsUseProxy(UInt16 refnum, HS_HTTPLibHandle libH)
		SYS_TRAP(kHTTPLibTrapIsUseProxy);

/**
 *  @brief This function returns whether this application permits HTTP/1.1 over proxy.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH:	IN:  The handle of HTTP library instance
 *  @return
 *	    True if this application permits HTTP/1.1 over proxy.
 *	    False if this application does not permit HTTP/1.1 over proxy.
 **/
Boolean HS_HTTPLibIsHTTP11OverProxy(UInt16 refnum, HS_HTTPLibHandle libH)
		SYS_TRAP(kHTTPLibTrapIsHTTP11OverProxy);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param libH:	IN:
 *  @retval Err error code.
 **/
Boolean HS_HTTPLibIsSendReferer(UInt16 refnum, HS_HTTPLibHandle libH)
		SYS_TRAP(kHTTPLibTrapIsSendReferer);

/**
 *  @brief This function returns whether this application sends Cookie.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH:	IN:  The handle of HTTP library instance
 *  @return
 *	    True if this application sends Cookie.
 *	    False if this application does not send Cookie.
 **/
Boolean HS_HTTPLibIsSendCookie(UInt16 refnum, HS_HTTPLibHandle libH)
		SYS_TRAP(kHTTPLibTrapIsSendCookie);
/**
 *  @brief This function creates a new stream and returns the handle of stream instance. At
 *	   this time, TCP/SSL socket is not opened.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param libH:	IN:  The handle of HTTP library instance
 *  @return
 *	    non NULL if successful.
 *	    NULL if failed (the main reason is memory shortage).
 **/
HS_HTTPLibStream HS_HTTPLibStreamNew(UInt16 refnum, HS_HTTPLibHandle libH)
		SYS_TRAP(kHTTPLibTrapStreamNew);

/**
 *  @brief This function deletes the stream instance. If this function is called without calling
 *	   HS_HTTPLibStreamClose, this function closes the TCP/SSL socket by calling
 *	   HS_HTTPLibStreamClose, however if the socket is in keep-alive, it is not closed.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param stream:	IN:  The handle of stream instance.
 *  @return None.
 **/
void HS_HTTPLibStreamDelete(UInt16 refnum, HS_HTTPLibStream stream)
		SYS_TRAP(kHTTPLibTrapStreamDelete);

/**
 *  @brief This function creates a request. If using proxy and SSL connection, CONNECT
 *	   request message is made. If there are cached authenticate informations, add
 *	   authenticate header. Add default headers and in_header to Request-Header.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param stream:	IN:  The handle of stream instance.
 *  @param in_method:	IN:  The HTTP method
 *			     httpMethodGET GET
 *			     httpMethodPOST POST
 *  @param *in_url:	IN:  The URL string.
 *  @param in_url_len:	IN:  The length of in_url.
 *  @param *in_header:	IN:  The header string to add the Request-Header.
 *			     Each line is terminated by <CR><LF>.
 *  @param in_header_len:	IN:  The length of in_header.
 *  @param *in_referer:	IN:  The referer string.
 *  @param in_referer_len:	IN:  The length of in_referer.
 *  @param in_version:	IN:  The version
 *			     httpVersion_0_9 HTTP/0.9
 *			     httpVersion_1_0 HTTP/1.0
 *		   	     httpVersion_1_1 HTTP/1.1
 *  @param in_no_cache:	IN:  True if no-cache is added to the header
 *			     False if no-cache is not added to the header
 *  @param in_pipeline:	IN:  True if doing pipeline
 *			     False if not doing pipeline
 *  @param in_ssl_flag:	IN:  The flag sent to SSL instance.
 *			     httpSSLFlagConnV2 connect by v2 protocol
 *			     httpSSLFlagConnV3 connect by v3 protocol
 *			     httpSSLFlagConnTLS connect by TLS(v3.1) protocol
 *			     httpSSLFlagConnV2V3 autoselect within v2,v3
 *			     httpSSLFlagConnV2TLS autoselect within v2,tls
 *			     httpSSLFlagConnV3TLS autoselect within v3,tls
 *			     httpSSLFlagConnV2V3TLS autoselect within v2,v3,tls
 *  @return
 *	    httpErrorOK succeed
 *		httpErrorNoMem memory shortage
 *		httpErrorReqHeaderSizeOver if Request header size is over the maximal
 *		setting. If the Request-Line size is set to unlimited, httpErrorReqHeaderSizeOver
 *		is not returned.
 **/
Int32 HS_HTTPLibStreamCreateRequest(UInt16 refnum, HS_HTTPLibStream stream, Int32 in_method, Char *in_url, UInt32 in_url_len,Char *in_header, UInt32 in_header_len,Char *in_referer, UInt32 in_referer_len, Int32 in_version, Boolean in_no_cache,Boolean in_pipeline, Int32 in_ssl_flag)
		SYS_TRAP(kHTTPLibTrapStreamCreateRequest);

/**
 *  @brief This function creates request for chunk-encoding post. If using proxy and SSL
 *	   connection, CONNECT request message is made. If there are cached authenticate
 *	   informations, add authenticate header. Add default headers and in_header to
 *	   Request-Header.
 *	   Do not use this API unless you want to create chunk-encoding request. Transfer-
 *	   Encoding: chunked is added to the request header.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param stream:	IN:  The handle of stream instance.
 *  @param in_method:	IN:  The HTTP method
 *			     httpMethodGET GET
 *			     httpMethodPOST POST
 *  @param *in_url:	IN:  The URL string.
 *  @param in_url_len:	IN:  The length of in_url.
 *  @param *in_header:	IN:  The header string to add the Request-Header.
 *			     Each line is terminated by <CR><LF>.
 *  @param in_header_len:	IN:  The length of in_header.
 *  @param *in_referer:	IN:  The referer string.
 *  @param in_referer_len:	IN:  The length of in_referer.
 *  @param in_version:	IN:  The version
 *			     httpVersion_0_9 HTTP/0.9
 *			     httpVersion_1_0 HTTP/1.0
 *			     httpVersion_1_1 HTTP/1.1
 *  @param in_no_cache:	IN:  True if no-cache is added to the header
 *			     False if no-cache is not added to the header
 *  @param in_pipeline:	IN:  True if doing pipeline
 *			     False if not doing pipeline
 *  @param in_ssl_flag:	IN:  The flag sent to SSL instance.
 *			     httpSSLFlagConnV2 connect by v2 protocol
 *			     httpSSLFlagConnV3 connect by v3 protocol
 *			     httpSSLFlagConnTLS connect by TLS(v3.1) protocol
 *			     httpSSLFlagConnV2V3 autoselect within v2,v3
 *			     httpSSLFlagConnV2TLS autoselect within v2,tls
 *			     httpSSLFlagConnV3TLS autoselect within v3,tls
 *			     httpSSLFlagConnV2V3TLS autoselect within v2,v3,tls
 *  @return
 *		httpErrorOK succeed
 *		httpErrorNoMem memory shortage
 *		httpErrorReqHeaderSizeOver if the Request header size is over the maximal
 *		setting. If the Request-Line size is set to unlimited, httpErrorReqHeaderSizeOver
 *		is not returned.
 **/
Int32 HS_HTTPLibStreamCreateRequestWithChunkEncoding(UInt16 refnum, HS_HTTPLibStream stream, Int32 in_method, Char *in_url, UInt32 in_url_len,Char *in_header, UInt32 in_header_len,Char *in_referer, UInt32 in_referer_len, Int32 in_version, Boolean in_no_cache,Boolean in_pipeline, Int32 in_ssl_flag)
		SYS_TRAP(kHTTPLibTrapStreamCreateRequestWithChunkEncoding);

/**
 *  @brief This function sends the request. This call opens and connects a TCP connection if
 *	   not already established. It automatically retries at where pipeline stalled if pipeline
 *	   is enable.
 *
 *  Because the data streaming could block and resume at where the pipeline was
 *  stalled, the application can call this function in a loop until the sending is finished.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param stream:	IN:  The handle of stream instance.
 *  @param *out_sleep:	OUT:  The time of the application can sleep (msec).
 *			     This value is not used for application.
 *  @return
 *	    httpErrorOK sending done
 *	    httpErrorWouldBlock application can call this function consecutively
 *	    httpErrorIOSleep application can sleep
 *	    httpErrorNoMem memory shortage
 *  	    httpErrorDNSInval DNS message invalid
 *	    httpErrorDNSNoServer DNS server is not found
 *	    httpErrorDNSTimedout Timeout at DNS resolve
 *	    httpErrorDNSNotFound DNS server can not find the name
 *	    httpErrorTCPOpen cannot open TCP
 *	    httpErrorTCPConnec cannot connect TCP
 *	    httpErrorTCPRead cannot read TCP
 *	    httpErrorTCPWrite cannot write TCP
 *	    httpErrorSSLConnect cannot Proxy Tunneling
 *	    httpErrorSSLHandShake cannot SSL handshake
 *	    httpErrorSSLRead cannot SSL read
 *	    httpErrorSSLWrite cannot SSL write
 *	    httpErrorReqTimedout request timeout
 *	    httpErrorRspTimedout response timeout at Proxy Tunneling (SSL only)
 **/
Int32 HS_HTTPLibStreamSendRequest(UInt16 refnum, HS_HTTPLibStream stream,Int32 *out_sleep)
		SYS_TRAP(kHTTPLibTrapStreamSendRequest);

/**
 *  @brief This function sends the POST request. This call opens and connects a TCP
 *	   connection if not already established. It automatically retries at where pipeline
 *	   stalled if pipeline is enable.
 *
 *  Because the data streaming could block and resume at where the pipeline was
 *  stalled, the application can call this function in a loop until the sending is finished.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param stream:	IN:  The handle of stream instance.
 *  @param *in_buf:	IN:  The pointer to POST data.
 *  @param in_len:	IN:  The length of in_buf.
 *  @param *out_len: 	OUT:  The size of sending data at this time (byte).
 *  @param *out_sleep:	OUT:  The time of the application can sleep (msec)
 *			     This value is not used for application.
 *  @return
 *	    httpErrorOK sending done
 *	    httpErrorWouldBlock application can call this function consecutively
 *	    httpErrorIOSleep application can sleep
 *	    httpErrorNoMem memory shortage
 *	    httpErrorDNSInval DNS message invalid
 **/
Int32 HS_HTTPLibStreamSendRequestWithPostData(UInt16 refnum, HS_HTTPLibStream stream, Char *in_buf, Int32 in_len, Int32 *out_len, Int32 *out_sleep)
		SYS_TRAP(kHTTPLibTrapStreamSendRequestWithPostData);

/**
 *  @brief This function sends chunk encoding data. This call opens and connects a TCP
 *	   connection if not already established. It automatically retries at where pipeline
 *	   stalled if pipeline is enable.
 *	   The sequence of chunked data sending is
 *	   1. call HS_HTTPLibStreamCreateRequestWithChunkEncoding()
 *	   2. call HS_HTTPLibStreamSendRequestWithChunkEncoding() while return
 *	   value is not httpErrorOK or error.
 *	   3. If there remains data, goto 2.
 *	   4. end
 *	   If only one chunked data is sent, this API returns httpErrorOK.
 *	   The address of data for one chunk is in_buf + in_offset, this value should not be
 *	   changed while sending only one chunked data.
 *	   isFinish is used to indicate the chunked data is the last chunk.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param stream:	IN:  The handle of stream instance.
 *  @param *in_buf: 	IN:  The pointer to POST data.
 *  @param in_offset:	IN:  The offset to POST data.
 *  @param in_len: 	IN:  The length of in_buf.
 *  @param isFinish: 	IN:  True if the send data is the last chunk.
 *			     False if send data is not the last chunk.
 *  @param *out_len: 	OUT:  The size of sending data at this time (byte).
 *  @param *out_sleep: 	OUT:  The time of the application can sleep (msec)
 *			     This value is not used for application.
 *  @return
 *	    httpErrorOK sending done
 *	    httpErrorWouldBlock application can call this function consecutively
 *	    httpErrorIOSleep application can sleep
 *	    httpErrorNoMem memory shortage
 *	    httpErrorDNSInval DNS message invalid
 *	    httpErrorDNSNoServer DNS server is not found
 *	    httpErrorDNSTimedout Timeout at DNS resolve
 *	    httpErrorDNSNotFound DNS server can not find the name
 *	    httpErrorTCPOpen cannot open TCP
 *	    httpErrorTCPConnec cannot connect TCP
 *	    httpErrorTCPRead cannot read TCP
 *	    httpErrorTCPWrite cannot write TCP
 *	    httpErrorSSLConnect cannot Proxy Tunneling
 *	    httpErrorSSLHandShake cannot SSL handshake
 * 	    httpErrorSSLRead cannot read SSL
 *	    httpErrorSSLWrite cannot write SSL
 *	    httpErrorReqTimedout request timeout
 *	    httpErrorRspTimedout response timeout at Proxy Tunneling (SSL only)
 **/
Int32 HS_HTTPLibStreamSendRequestWithChunkEncoding(UInt16 refnum, HS_HTTPLibStream stream, Char *in_buf, Int32 in_offset, Int32 in_len, Boolean isFinish, Int32 *out_len, Int32 *out_sleep)
		SYS_TRAP(kHTTPLibTrapStreamSendRequestWithChunkEncoding);

/**
 *  @brief This function receives response. It automatically retries at where pipeline stalled if
 *	   pipeline is enable.
 *
 *  Because the data streaming could block and resume at where the pipeline was
 *  stalled, the application can call this function in a loop until the receiving is finished.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param stream:	IN:  The handle of stream instance.
 *  @param *out_buf: 	OUT:  The pointer to the receive buffer.
 *  @param in_len:	IN:  The size of the receive buffer (byte).
 *  @param *out_len: 	OUT:  The received size (byte).
 *  @param *out_sleep: 	OUT:  The time of the application can sleep (msec)
 *			      This value is not used for application.
 *  @return
 *	    httpErrorOK receiving done
 *	    httpErrorWouldBlock application can call this function
 *	    consecutively
 *	    httpErrorIOSleep application can sleep
 *	    httpErrorNoMem memory shortage
 *	    httpErrorTCPRead cannot read TCP
 *	    httpErrorTCPWrite cannot write TCP
 *	    httpErrorSSLHandShake cannot SSL handshake
 *	    httpErrorSSLRead cannot read SSL
 *	    httpErrorSSLWrite cannot write SSL
 *	    httpErrorReqTimedout request timeout
 *	    httpErrorRspTimedout response timeout
 *	    httpErrorAuthUnknown unknown authentication type
 *	    httpErrorAuthNoHeader authentication request without
 * 	    authentication header
 *	    httpErrorAuthFormat wrong format of authentication header
 *	    httpErrorRedirectFormat wrong format of redirect header
 *	    httpErrorRedirectCanceled redirect is canceled by user
 *	    httpErrorRspHeaderSizeOver size over of response header
 *	    httpErrorReqTooManyContinue it exceeds the limit times of Continue
 **/
Int32 HS_HTTPLibStreamReceiveResponse(UInt16 refnum, HS_HTTPLibStream stream, Char *out_buf, Int32 in_len, Int32 *out_len, Int32 *out_sleep)
		SYS_TRAP(kHTTPLibTrapStreamReceiveResponse);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param stream:	IN:
 *  @retval Err error code.
 **/
Int32 HS_HTTPLibStreamGetFlag(UInt16 refnum, HS_HTTPLibStream stream)
		SYS_TRAP(kHTTPLibTrapStreamGetFlag);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param stream:	IN:
 *  @retval Err error code.
 **/
void HS_HTTPLibStreamSuspendTimer(UInt16 refnum, HS_HTTPLibStream stream)
		SYS_TRAP(kHTTPLibTrapStreamSuspendTimer);
/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param stream:	IN:
 *  @retval Err error code.
 **/
void HS_HTTPLibStreamResumeTimer(UInt16 refnum, HS_HTTPLibStream stream)
		SYS_TRAP(kHTTPLibTrapStreamResumeTimer);

/**
 *  @brief This function closes the HTTP stream. If the TCP/SSL socket is in keep-alive, it is
 *	   not closed.
 *
 *  @param refnum: 	IN:  Library reference number
 *  @param stream:	IN:  The handle of stream instance.
 *  @return None.
 **/
void HS_HTTPLibStreamClose(UInt16 refnum, HS_HTTPLibStream stream)
		SYS_TRAP(kHTTPLibTrapStreamClose);

/**
 *  @brief This function returns whether the response header is completely received.
 *
 *  This function is typically called after HS_HTTPLibStreamReceiveResponse when
 *  receiving the response header.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param stream:	IN:  The handle of stream instance.
 *  @return
 *	    True if header is completely received.
 *	    False if header is not received.
 **/
Boolean HS_HTTPLibStreamIsHeaderReceived(UInt16 refnum, HS_HTTPLibStream stream)
		SYS_TRAP(kHTTPLibTrapStreamIsHeaderReceived);

/**
 *  @brief This function sets the POST data.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param stream:	IN:  The handle of stream instance.
 *  @param in_encoding:	IN:  The type of encoding.
 *			     httpPostURLENCODED URL encoding
 *			     httpPostPLAINTEXT plain text (non encoding)
 *			     httpPostMULTIPART multi-part encoding
 *  @param *in_data: 	IN:  The pointer to the POST data.
 *  @param in_len: 	IN:  The length of in_data.
 *  @return
 *		httpErrorOK setting done
 *		httpErrorNoMem memory shortage
 *		httpErrorReqBodySizeOver the size of request body exceeds limit. If the
 *		application does not set the limit by
 *		HS_HTTPLibSetMaxRequestBody,
 *		httpErrorReqBodySizeOver is not returned.
 **/
Int32 HS_HTTPLibStreamSetPostData(UInt16 refnum, HS_HTTPLibStream stream, Int32 in_encoding, Char *in_data, Int32 in_len)
		SYS_TRAP(kHTTPLibTrapStreamSetPostData);

/**
 *  @brief This function returns the scheme given at the HS_HTTPLibStreamCreateRequest.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param stream:	IN:  The handle of stream instance.
 *  @return
 *	    httpSchemeHTTP HTTP
 *	    httpSchemeHTTPS HTTPS
 **/
Int32 HS_HTTPLibStreamGetScheme(UInt16 refnum, HS_HTTPLibStream stream)
		SYS_TRAP(kHTTPLibTrapStreamGetScheme);

/**
 *  @brief This function returns the state of the stream. The state changes follow the below
 *	   sequence (but skipping the states that are not relevant to the current request).
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param stream:	IN:  The handle of stream instance.
 *  @return
 *	    httpStreamStateNew The instance is created
 *	    httpStreamStateDormant The instance is dormant
 *	    httpStreamStateReqCreated The request is created
 *	    httpStreamStateReqResolve Name resolve begins
 *	    httpStreamStateReqResolving Name resolving
 *	    httpStreamStateReqOpen TCP/SSL open
 *	    httpStreamStateReqConnect TCP connect begins
 *	    httpStreamStateReqConnecting TCP connecting
 *	    httpStreamStateReqProxyConnect Proxy Tunneling begins
 * 	    httpStreamStateReqProxyConnecting Proxy Tunneling
 *	    httpStreamStateReqSSLHandShaking SSL HandShaking
 *	    httpStreamStateReqSSLHandShaked SSL handShake done
 *	    httpStreamStateReqSending Sending the request
 *	    httpStreamStateReqSent Sending the request done
 *	    httpStreamStateRspHeaderWaiting Waiting the response header
 *	    httpStreamStateRspContinue Waiting response receiving done for
 *	    Status-Code 100
 *	    httpStreamStateRspHeaderReceiving Receiving the response header
 *	    httpStreamStateRspHeaderReceived Receiving the response header done
 *	    httpStreamStateRspEntityReceiving Receiving the response body
 **/
Int32 HS_HTTPLibStreamGetState(UInt16 refnum, HS_HTTPLibStream stream)
		SYS_TRAP(kHTTPLibTrapStreamGetState);

/**
 *  @brief This function returns the Status Code.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param stream:	IN:  The handle of stream instance.
 *  @return
 *	    httpStatusCodeContinue 100
 *	    httpStatusCodeSwitchingProtocols 101
 *	    httpStatusCodeOK 200
 *	    httpStatusCodeCreated 201
 *	    httpStatusCodeAccepted 202
 *	    httpStatusCodeNonAuthoritativeInformation 203
 *	    httpStatusCodeNoContent 204
 *	    httpStatusCodeResetContent 205
 *	    httpStatusCodePartialContent 206
 *	    httpStatusCodeMultipleChoices 300
 *	    httpStatusCodeMovedPermanently 301
 *	    httpStatusCodeFound 302
 *	    httpStatusCodeSeeOther 303
 *	    httpStatusCodeNotModified 304
 *	    httpStatusCodeUseProxy 305
 *	    httpStatusCodeTemporaryRedirect 307
 *	    httpStatusCodeBadRequest 400
 **/
Int32 HS_HTTPLibStreamGetStatusCode(UInt16 refnum, HS_HTTPLibStream stream)
		SYS_TRAP(kHTTPLibTrapStreamGetStatusCode);

/**
 *  @brief This function returns the size of the response header.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param stream:	IN:  The handle of stream instance.
 *  @return
 *	    header size (byte)
 *	    -1 if not received
 **/
Int32 HS_HTTPLibStreamGetResponseHeaderLength(UInt16 refnum, HS_HTTPLibStream stream)
		SYS_TRAP(kHTTPLibTrapStreamGetResponseHeaderLength);

/**
 *  @brief This function returns the handle of the response header. The responsibility to
 *	   delete the handle is library, the application can not delete this return value.
 *	   To lock/unlock the handle, the application should use the APIs,
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param stream:	IN:  The handle of stream instance.
 *  @return
 *	    non NULL the handle of header
 *	    NULL if not received
 **/
HS_HTTPLibVHandle HS_HTTPLibStreamGetResponseHeader(UInt16 refnum, HS_HTTPLibStream stream)
		SYS_TRAP(kHTTPLibTrapStreamGetResponseHeader);

/**
 *  @brief This function returns the content’s size (byte).
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param stream:	IN:  The handle of stream instance.
 *  @return
 *	    The Content-Length (byte)
 *	    -1 if not received
 **/
Int32 HS_HTTPLibStreamGetContentLength(UInt16 refnum, HS_HTTPLibStream stream)
		SYS_TRAP(kHTTPLibTrapStreamGetContentLength);

/**
 *  @brief This function returns the size of response body (byte).
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param stream:	IN:  The handle of stream instance.
 *  @return
 *	    The size of response body (byte)
 *	    -1 if not received
 **/
Int32 HS_HTTPLibStreamGetEntityLength(UInt16 refnum, HS_HTTPLibStream stream)
		SYS_TRAP(kHTTPLibTrapStreamGetEntityLength);

/**
 *  @brief This function adds the line to the Request header. If there is already the same kind
 *	        of header line, old one is deleted.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param stream:	IN:  The handle of stream instance.
 *  @param *in_str:	IN:  The pointer to the header strong to add.
 *  @param in_len:	IN:  The length of in_str.
 *  @return
 *	    True if successful.
 *	    False failed (memory shortage)
 **/
Boolean HS_HTTPLibStreamAddHeaderLine(UInt16 refnum, HS_HTTPLibStream stream, Char *in_str, Int32 in_len)
		SYS_TRAP(kHTTPLibTrapStreamAddHeaderLine);

/**
 *  @brief This function sets the value of corresponding header. If there is already the same
 *  	   kind of header line, old one is deleted.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param stream:	IN:  The handle of stream instance.
 *  @param in_type:	IN:  The type of header (See httpHeaderIDEnum).
 *  @param *in_str:	IN:  The pointer to the header value string.
 *  @param in_len:	IN:  The length of in_str.
 *  @return
 *	    True if successful
 *	    False failed (memory shortage)
 **/
Boolean HS_HTTPLibStreamAddHeader(UInt16 refnum, HS_HTTPLibStream stream, Int32 in_type, Char *in_str, Int32 in_len)
		SYS_TRAP(kHTTPLibTrapStreamAddHeader);

/**
 *  @brief This function returns the offset and length of corresponding header value.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param stream:	IN:  The handle of stream instance.
 *  @param in_type:	IN:  The type of header (See httpHeaderIDEnum).
 *  @param *out_off:	IN:  The offset to the header value.
 *  @param *out_len:	IN:  The length of corresponding header value.
 *  @return
 *	    True if successful
 *	    False there is no corresponding header
 **/
Boolean HS_HTTPLibStreamGetHeader(UInt16 refnum, HS_HTTPLibStream stream, Int32 in_type, Int32 *out_off, Int32 *out_len)
		SYS_TRAP(kHTTPLibTrapStreamGetHeader);

/**
 *  @brief This function returns the Server certification used for SSL. The responsibility to
 *	   delete the return handle is the application. To delete the return value, the
 *	   application should use HS_HTTPLibCertListDelete.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param stream:	IN:  The handle of stream instance.
 *  @return
 *	    The handle of SSL server certification chain.
 *	    NULL if no server certification or memory shortage.
 **/
HS_HTTPLibCertList HS_HTTPLibMakeCertList(UInt16 refnum, HS_HTTPLibStream stream)
		SYS_TRAP(kHTTPLibTrapMakeCertList);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param *inout_ss:	IN:
 *  @param in_id: 	IN:
 *  @param *in_cs: 	IN:
 *  @retval Err error code.
 **/
Boolean HS_HTTPLibHeaderSplitByX(UInt16 refnum, HS_HTTPLibSplit *inout_ss, Int32 in_id, Char *in_cs)
		SYS_TRAP(kHTTPLibTrapHeaderSplitByX);

/**
 *  @brief This function initialize/finalize the HS_HTTPLibAuthInfo struct. The application
 *	    should call this function setting in_new = true before using HS_HTTPLibAuthInfo.
 *	    The application should call this function setting in_new = false after using
 *	    HS_HTTPLibAuthInfo.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param *in_info:	IN:  The pointer to the authenticate information struct.
 *  @param in_new: 	IN:  True if initialize the member.
 *			     False if initialize the member after deleting.
 *  @return None.
 **/
void HS_HTTPLibAuthInfoTidy(UInt16 refnum, HS_HTTPLibAuthInfo *in_info, Boolean in_new)
		SYS_TRAP(kHTTPLibTrapAuthInfoTidy);

/**
 *  @brief This function sets the authentication information.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param libH:	IN:  The handle of HTTP library instance.
 *  @param in_type:	IN:  The authentication type.
 *			     httpAuthTypeBasic 		Basic authentication
 *	        	     httpAuthTypeDigest 	Digest Authentication
 *  @param in_target:	IN:  The authentication target.
 *			     httpAuthTargetPage 	Authentication Information for Page
 *			     httpAuthTargetProxy 	Authentication Information for Proxy
 *  @param *in_challenge:	IN:  The challenge.
 *  @param *in_realm:	IN:  The realm.
 *  @param *in_user:	IN:  The pointer to the User ID.
 *  @param in_user_len:	IN:  The length of in_user.
 *  @param *in_pass:	IN:  The pointer to the Password.
 *  @param in_pass_len:	IN:  The length of in_pass.
 *  @param *in_url:	IN:  The pointer to the URL (httpAuthTargetPage only).
 *  @param in_url_len:	IN:  The length of in_url.
 *  @param *in_host:	IN:  The pointer to the proxy host name (httpAuthTargetProxy only).
 *  @param in_host_len:	IN:  The length of in_host.
 *  @param in_port:	IN:  The port of proxy (httpAuthTargetProxy only).
 *  @return
 *	    True if setting successful.
 *	    False if failed (memory shortage).
 **/
Boolean HS_HTTPLibSetUserPass(UInt16 refnum, HS_HTTPLibHandle libH, Int32 in_type, Int32 in_target, HS_HTTPLibSplit *in_challenge, HS_HTTPLibSplit *in_realm, Char *in_user, Int32 in_user_len, Char *in_pass, Int32 in_pass_len, Char *in_url, Int32 in_url_len, Char *in_host, Int32 in_host_len, Int32 in_port)
		SYS_TRAP(kHTTPLibTrapSetUserPass);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param libH:	IN:
 *  @param in_target: 	IN:
 *  @param *in_url:	IN:
 *  @param in_url_len:	IN:
 *  @param *in_host:	IN:
 *  @param in_host_len:	IN:
 *  @param in_port:	IN:
 *  @param *in_realm:	IN:
 *  @param *out_info:	IN:
 *  @retval Err error code.
 **/
Boolean HS_HTTPLibFindAuthInfo(UInt16 refnum, HS_HTTPLibHandle libH, Int32 in_target, Char *in_url, Int32 in_url_len, Char *in_host, Int32 in_host_len, Int32 in_port, HS_HTTPLibSplit *in_realm, HS_HTTPLibAuthInfo *out_info)
		SYS_TRAP(kHTTPLibTrapFindAuthInfo);

/**
 *  @brief This function returns the authentication information to the out_info. Before calling
 *	   this API, the application should intialize HS_HTTPLibAuthInfo by
 *	   HS_HTTPLibAuthInfoTidy(out_info, true).
 *
 *	   After using the authentication information, the application should finalize
 *	   HS_HTTPLibAuthInfoTidy by HS_HTTPLibAuthInfoTidy(out_info, false).
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param libH:	IN:  The handle of HTTP library instance.
 *  @param in_target:	IN:  The authentication target.
 *			     httpAuthTargetPage   	Authentication Information for Page
 *	 		     httpAuthTargetProxy 	Authentication Information for Proxy
 *  @param *in_url:	IN:  The pointer to the URL (httpAuthTargetPage only).
 *  @param in_url_len:	IN:  The length of in_url.
 *  @param *in_host:	IN:  The pointer to the proxy host name (httpAuthTargetProxy only).
 *  @param in_host_len:	IN:  The length of in_host.
 *  @param in_port:	IN:  The port of proxy (httpAuthTargetProxy only).
 *  @param *in_realm:	IN:  The realm.
 *  @param *out_info:	OUT:  The pointer to the authentication information.
 *  @return
 *	    True if setting successful.
 *	    False if failed (no information in cache or memory shortage).
 **/
Boolean HS_HTTPLibMakeAuthInfo(UInt16 refnum, HS_HTTPLibHandle libH, Int32 in_target, Char *in_url, Int32 in_url_len, Char *in_host, Int32 in_host_len, Int32 in_port, HS_HTTPLibSplit *in_realm, HS_HTTPLibAuthInfo *out_info)
		SYS_TRAP(kHTTPLibTrapMakeAuthInfo);

/**
 *  @brief This function returns the User Name. The application should not delete this return value.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param *in_info:	IN:  The pointer to the authentication information.
 *  @return The handle of User Name.
 **/
HS_HTTPLibString HS_HTTPLibAuthInfoUser(UInt16 refnum, HS_HTTPLibAuthInfo *in_info)
		SYS_TRAP(kHTTPLibTrapAuthInfoUser);

/**
 *  @brief This function returns the Password. The application should not delete this return value.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param *in_info:	IN:  The pointer to the authentication information.
 *  @return The handle of password.
 **/
HS_HTTPLibString HS_HTTPLibAuthInfoPass(UInt16 refnum, HS_HTTPLibAuthInfo *in_info)
		SYS_TRAP(kHTTPLibTrapAuthInfoPass);

/**
 *  @brief This function returns the Realm. The application should not delete this return value.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param *in_info:	IN:  The pointer to the authentication information.
 *  @return
 *	    The handle of Realm.
 **/
HS_HTTPLibString HS_HTTPLibAuthInfoRealm(UInt16 refnum, HS_HTTPLibAuthInfo *in_info)
		SYS_TRAP(kHTTPLibTrapAuthInfoRealm);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param *in_challenge_ss:	IN:
 *  @param *in_token: 	IN:
 *  @param *out_ss:	IN:
 *  @retval Err error code.
 **/
Boolean HS_HTTPLibAuthCacheGetChallengeTokenSS(UInt16 refnum, HS_HTTPLibSplit *in_challenge_ss, Char *in_token, HS_HTTPLibSplit *out_ss)
		SYS_TRAP(kHTTPLibTrapAuthCacheGetChallengeTokenSS);

/**
 *  @brief This function sets the Cookie mode.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param libH:	IN:  The handle of HTTP library instance.
 *  @param in_mode: 	IN:  The mode of receiving Cookie.
 *	  		     httpCookieNotifyBeforeSet 	Confirm before Cookie set
 *	 		     httpCookieAlwaysSet 	Cookie is always set
 *	 		     httpCookieNeverSet 	Cookie is not set
 *  @param in_cb:	IN:  The confirm callback function.
 *  @param in_opaque:: 	IN:  The opaque which is given to the callback function.
 *  @return None.
 **/
void HS_HTTPLibSetCookieMode(UInt16 refnum, HS_HTTPLibHandle libH, Int32 in_mode, HS_HTTPLibCookieConfirmProc in_cb, HS_HTTPLibOpaque in_opaque)
		SYS_TRAP(kHTTPLibTrapSetCookieMode);

/**
 *  @brief This function sets the max size of Cookie entries.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param libH:	IN:  The handle of HTTP library instance.
 *  @param in_max_total_cookies: 	IN:  The size of max Cookie entries.
 *  @param in_max_cookies_per_domain:	IN:  The size of Cookie entries per one domain.
 *  @param in_max_len_per_cookie:	IN:  The max size of one Cookie.
 *  @return None.
 **/
void HS_HTTPLibSetCookieMax(UInt16 refnum, HS_HTTPLibHandle libH, Int32 in_max_total_cookies, Int32 in_max_cookies_per_domain, Int32 in_max_len_per_cookie)
		SYS_TRAP(kHTTPLibTrapSetCookieMax);

/**
 *  @brief This function sets the Cookie abiding with the confirmation mode set by
 *	   HS_HTTPLibSetCookieMode. The application should not use this API in callback
 *	   function. The API HS_HTTPLibSetCookieX is designed for use in callback.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param libH:	IN:  The handle of HTTP library instance.
 *  @param in_url: 	IN:  The handle of URL.
 *  @param *in_header:	IN:  The pointer to the header string.
 *  @param in_header_len:	IN:  The length of in_header.
 *  @return
 *	    httpErrorOK 	Cookie is set.
 *	    httpErrorNoMem 	memory shortage
 *	    httpErrorInval 	failed by other reason
 **/
Int32 HS_HTTPLibSetCookie(UInt16 refnum, HS_HTTPLibHandle libH, HS_HTTPLibURLString in_url, Char *in_header, Int32 in_header_len)
		SYS_TRAP(kHTTPLibTrapSetCookie);

/**
 *  @brief This function sets the Cookie. This function is designed for callback confirm function.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param libH:	IN:  The handle of HTTP library instance.
 *  @param in_url: 	IN:  The handle of URL.
 *  @param *in_header:	IN:  The pointer to the header string.
 *  @param in_header_len:	IN:  The length of in_header.
 *  @return
 *	    httpErrorOK 	Cookie is set.
 *	    httpErrorNoMem 	memory shortage
 *	    httpErrorInval 	failed by other reason
 **/
Int32 HS_HTTPLibSetCookieX(UInt16 refnum, HS_HTTPLibHandle libH, HS_HTTPLibURLString in_url, Char *in_header, Int32 in_header_len)
		SYS_TRAP(kHTTPLibTrapSetCookieX);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param libH:	IN:
 *  @param *in_url: 	IN:
 *  @param in_urllen:	IN:
 *  @param in_secure:	IN:
 *  @param *out_len:	IN:
 *  @retval Err error code.
 **/
HS_HTTPLibString HS_HTTPLibMakeCookiesString(UInt16 refnum, HS_HTTPLibHandle libH, Char *in_url, Int32 in_urllen, Boolean in_secure, Int32 *out_len)
		SYS_TRAP(kHTTPLibTrapMakeCookiesString);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param libH:	IN:
 *  @param *out_len: 	IN:
 *  @retval Err error code.
 **/
HS_HTTPLibString HS_HTTPLibSaveCookiesEntries(UInt16 refnum, HS_HTTPLibHandle libH, Int32 *out_len)
		SYS_TRAP(kHTTPLibTrapSaveCookiesEntries);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param libH:	IN:
 *  @param *in_s: 	IN:
 *  @param in_slen:	IN:
 *  @retval Err error code.
 **/
Boolean HS_HTTPLibLoadCookiesEntries(UInt16 refnum, HS_HTTPLibHandle libH, Char *in_s, Int32 in_slen)
		SYS_TRAP(kHTTPLibTrapLoadCookiesEntries);

/**
 *  @brief This function clears Cookies.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param libH:	IN:  The handle of HTTP library instance.
 *  @param in_flag: 	IN:  httpCookieClearAll 		clear all Cookies
 *			     httpCookieClearSessionCookiesOnly 	clear session Cookies only
 *  @return None.
 **/
void HS_HTTPLibCookieMakeEmpty(UInt16 refnum, HS_HTTPLibHandle libH, Int32 in_flag)
		SYS_TRAP(kHTTPLibTrapCookieMakeEmpty);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param libH:	IN:
 *  @param in_retry: 	IN:
 *  @retval Err error code.
 **/
void HS_HTTPLibSetDNSMaxRetry(UInt16 refnum, HS_HTTPLibHandle libH, Int32 in_retry)
		SYS_TRAP(kHTTPLibTrapSetDNSMaxRetry);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param libH:	IN:
 *  @param in_interval: 	IN:
 *  @retval Err error code.
 **/
void HS_HTTPLibSetDNSRetryInterval(UInt16 refnum, HS_HTTPLibHandle libH, Int32 in_interval)
		SYS_TRAP(kHTTPLibTrapSetDNSRetryInterval);

/**
 *  @brief Set the SSL version.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param libH: 	IN:  The handle of HTTP library instance.
 *  @param in_flag: IN:  -
 *  @param in_on:	IN:  -
 *  @return None.
 **/
void HS_HTTPLibSetSSLVersionFlag(UInt16 refnum, HS_HTTPLibHandle libH, Int32 in_flag, Boolean in_on)
		SYS_TRAP(kHTTPLibTrapSetSSLVersionFlag);

/**
 *  @brief Set the timeout period for SSL use.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param libH:	IN:  The handle of HTTP library instance.
 *  @return Timeout.
 **/
Int32 HS_HTTPLibSSLTimeout(UInt16 refnum, HS_HTTPLibHandle libH)
		SYS_TRAP(kHTTPLibTrapSSLTimeout);

/**
 *  @brief TBD
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param libH:	IN:  The handle of HTTP library instance.
 *  @param *out_list: 	OUT:  -
 *  @retval Int32
 **/
Int32 HS_HTTPLibSSLOpenCertDB(UInt16 refnum, HS_HTTPLibHandle libH, HS_HTTPLibCertList *out_list)
		SYS_TRAP(kHTTPLibTrapSSLOpenCertDB);

/**
 *  @brief TBD
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param libH:	IN:  The handle of HTTP library instance.
 *  @return None.
 **/
void HS_HTTPLibSSLCloseCertDB(UInt16 refnum, HS_HTTPLibHandle libH)
		SYS_TRAP(kHTTPLibTrapSSLCloseCertDB);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param in_type:	IN:
 *  @return
 **/
HS_HTTPLibCertList HS_HTTPLibCertListNew(UInt16 refnum, Int32 in_type)
		SYS_TRAP(kHTTPLibTrapCertListNew);

/**
 *  @brief TBD
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_list:	IN:  The handle of the server’s certification list.
 *  @return None.
 **/
void HS_HTTPLibCertListDelete(UInt16 refnum, HS_HTTPLibCertList in_list)
		SYS_TRAP(kHTTPLibTrapCertListDelete);

/**
 *  @brief TBD
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_list:	IN:  The handle of the server’s certification list.
 *  @return Handle to the clone of the certification list.
 **/
HS_HTTPLibCertList HS_HTTPLibCertListClone(UInt16 refnum, HS_HTTPLibCertList in_list)
		SYS_TRAP(kHTTPLibTrapCertListClone);

/**
 *  @brief Lock the certificate list and return its pointer.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_list:	IN:  The handle of the server's certification list.
 *  @return Pointer to the certificate list.
 **/
void *HS_HTTPLibCertListLock(UInt16 refnum, HS_HTTPLibCertList in_list)
		SYS_TRAP(kHTTPLibTrapCertListLock);

/**
 *  @brief Unlock the certificate list
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_list:	IN:  The handle of the server’s certification list.
 *  @return None.
 **/
void HS_HTTPLibCertListUnlock(UInt16 refnum, HS_HTTPLibCertList in_list)
		SYS_TRAP(kHTTPLibTrapCertListUnlock);

/**
 *  @brief TBD
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_list:	IN:  The handle of the server’s certification list.
 *  @retval Int32
 **/
Int32 HS_HTTPLibCertListType(UInt16 refnum, HS_HTTPLibCertList in_list)
		SYS_TRAP(kHTTPLibTrapCertListType);

/**
 *  @brief TBD
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_list:	IN:  The handle of the server’s certification list.
 *  @retval Int32
 **/
Int32 HS_HTTPLibCertListLength(UInt16 refnum, HS_HTTPLibCertList in_list)
		SYS_TRAP(kHTTPLibTrapCertListLength);

/**
 *  @brief TBD
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_list:	IN:  The handle of the server’s certification list.
 *  @param in_index: 	IN:  -
 *  @param *out_cert:	OUT: -
 *  @retval Int32
 **/
Int32 HS_HTTPLibCertListBeginDecode(UInt16 refnum, HS_HTTPLibCertList in_list, Int32 in_index, HS_HTTPLibCertPtr *out_cert)
		SYS_TRAP(kHTTPLibTrapCertListBeginDecode);

/**
 *  @brief TBD
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_list:	IN:  The handle of the server’s certification list.
 *  @param in_cert: 	IN:  -
 *  @return None.
 **/
void HS_HTTPLibCertListEndDecode(UInt16 refnum, HS_HTTPLibCertList in_list, HS_HTTPLibCertPtr in_cert)
		SYS_TRAP(kHTTPLibTrapCertListEndDecode);

/**
 *  @brief TBD
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_list:	IN:  The handle of the server’s certification list.
 *  @param *in_info: 	IN:  -
 *  @param in_over_write: 	IN:  -
 *  @retval Int32
 **/
Int32 HS_HTTPLibCertListImportCert(UInt16 refnum, HS_HTTPLibCertList in_list, void *in_info, Boolean in_over_write)
		SYS_TRAP(kHTTPLibTrapCertListImportCert);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param in_cert:	IN:
 *  @retval Err error code.
 **/
Int32 HS_HTTPLibCertPeekVersion(UInt16 refnum, HS_HTTPLibCertPtr in_cert)
		SYS_TRAP(kHTTPLibTrapCertPeekVersion);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param in_cert:	IN:
 *  @param **out_str: 	IN:
 *  @retval Err error code.
 **/
Int32 HS_HTTPLibCertPeekSerialNumber(UInt16 refnum, HS_HTTPLibCertPtr in_cert, Char **out_str)
		SYS_TRAP(kHTTPLibTrapCertPeekSerialNumber);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param in_cert:	IN:
 *  @retval Err error code.
 **/
Int32 HS_HTTPLibCertPeekSignAlgo(UInt16 refnum, HS_HTTPLibCertPtr in_cert)
		SYS_TRAP(kHTTPLibTrapCertPeekSignAlgo);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param in_cert:	IN:
 *  @param in_attr: 	IN:
 *  @retval Err error code.
 **/
Boolean HS_HTTPLibCertBeginPeekIssuerRDN(UInt16 refnum, HS_HTTPLibCertPtr in_cert, Int32 in_attr)
		SYS_TRAP(kHTTPLibTrapCertBeginPeekIssuerRDN);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param in_cert:	IN:
 *  @param **out_str: 	IN:
 *  @retval Err error code.
 **/
Int32 HS_HTTPLibCertPeekIssuerRDN(UInt16 refnum, HS_HTTPLibCertPtr in_cert, Char **out_str)
		SYS_TRAP(kHTTPLibTrapCertPeekIssuerRDN);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param in_cert:	IN:
 *  @retval Err error code.
 **/
Int32 HS_HTTPLibCertPeekIssuerRDNStrType(UInt16 refnum, HS_HTTPLibCertPtr in_cert)
		SYS_TRAP(kHTTPLibTrapCertPeekIssuerRDNStrType);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param in_cert:	IN:
 *  @retval Err error code.
 **/
void HS_HTTPLibCertEndPeekIssuerRDN(UInt16 refnum, HS_HTTPLibCertPtr in_cert)
		SYS_TRAP(kHTTPLibTrapCert_EndPeekIssuerRDN);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param in_cert:	IN:
 *  @param in_attr: 	IN:
 *  @retval Err error code.
 **/
Boolean HS_HTTPLibCertBeginPeekSubjectRDN(UInt16 refnum, HS_HTTPLibCertPtr in_cert, Int32 in_attr)
		SYS_TRAP(kHTTPLibTrapCertBeginPeekSubjectRDN);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param in_cert:	IN:
 *  @param **out_str:	IN:
 *  @retval Err error code.
 **/
Int32 HS_HTTPLibCertPeekSubjectRDN(UInt16 refnum, HS_HTTPLibCertPtr in_cert, Char **out_str)
		SYS_TRAP(kHTTPLibTrapCertPeekSubjectRDN);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param in_cert:	IN:
 *  @retval Err error code.
 **/
Int32 HS_HTTPLibCertPeekSubjectRDNStrType(UInt16 refnum, HS_HTTPLibCertPtr in_cert)
		SYS_TRAP(kHTTPLibTrapCertPeekSubjectRDNStrType);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param in_cert:	IN:
 *  @retval Err error code.
 **/
void HS_HTTPLibCertEndPeekSubjectRDN(UInt16 refnum, HS_HTTPLibCertPtr in_cert)
		SYS_TRAP(kHTTPLibTrapCert_EndPeekSubjectRDN);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param in_cert:	IN:
 *  @param *in_time: 	IN:
 *  @retval Err error code.
 **/
void HS_HTTPLibCertPeekValidityStart(UInt16 refnum, HS_HTTPLibCertPtr in_cert, HS_HTTPLibTime *in_time)
		SYS_TRAP(kHTTPLibTrapCert_PeekValidityStart);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param in_cert:	IN:
 *  @param *in_time: 	IN:
 *  @retval Err error code.
 **/
void HS_HTTPLibCertPeekValidityEnd(UInt16 refnum, HS_HTTPLibCertPtr in_cert, HS_HTTPLibTime *in_time)
		SYS_TRAP(kHTTPLibTrapCert_PeekValidityEnd);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param in_cert:	IN:
 *  @retval Err error code.
 **/
Int32 HS_HTTPLibCertPeekRSAPublicKeyBits(UInt16 refnum, HS_HTTPLibCertPtr in_cert)
		SYS_TRAP(kHTTPLibTrapCert_PeekRSAPublicKeyBits);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @retval Err error code.
 **/
HS_HTTPLibClientCertList HS_HTTPLibClientCertListNew(UInt16 refnum)
		SYS_TRAP(kHTTPLibTrapClientCertListNew);

/**
 *  @brief TBD
 *
 *  @param refnum: 	IN:
 *  @param in_list:	IN:
 *  @retval HS_HTTPLibClientCertList
 **/
HS_HTTPLibClientCertList HS_HTTPLibClientCertListClone(UInt16 refnum, HS_HTTPLibClientCertList in_list)
		SYS_TRAP(kHTTPLibTrapClientCertListClone);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param in_list:	IN:
 *  @retval nothing
 **/
void HS_HTTPLibClientCertListDelete(UInt16 refnum, HS_HTTPLibClientCertList in_list)
		SYS_TRAP(kHTTPLibTrapClientCertListDelete);

/**
 *  @brief TBD
 *
 *  @param refnum: 	IN:
 *  @param in_list:	IN:
 *  @retval Nothing
 **/
Int32 HS_HTTPLibClientCertListLength(UInt16 refnum, HS_HTTPLibClientCertList in_list)
		SYS_TRAP(kHTTPLibTrapClientCertListLength);

/**
 *  @brief TBD
 *
 *  @param refnum: 	IN:
 *  @param in_list:	IN:
 *  @param in_index:	IN:
 *  @retval Boolean
 **/
Boolean HS_HTTPLibClientCertListIsValid(UInt16 refnum, HS_HTTPLibClientCertList in_list, Int32 in_index)
		SYS_TRAP(kHTTPLibTrapClientCertListIsValid);

/**
 *  @brief TBD
 *
 *  @param refnum: 	IN:
 *  @param in_list:	IN:
 *  @param in_index:	IN:
 *  @param in_valid:	IN:
 *  @retval Nothing
 **/
void HS_HTTPLibClientCertListSetValid(UInt16 refnum, HS_HTTPLibClientCertList in_list, Int32 in_index, Boolean in_valid)
		SYS_TRAP(kHTTPLibTrapClientCertListSetValid);

/**
 *  @brief This function sets the server certificate callback function for this application.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param libH:	IN:  -
 *  @param in_cb:	IN:  -
 *  @param in_opaque:	IN:  -
 *  @return Nothing
 **/
void HS_HTTPLibSetSSLServerCertConfirmProc(UInt16 refnum, HS_HTTPLibHandle libH, HS_HTTPLibServerCertConfirmProc in_cb, HS_HTTPLibOpaque in_opaque)
		SYS_TRAP(kHTTPLibTrapSetSSLServerCertConfirmProc);

/**
 *  @brief This function sets the result of confirmation. This function should be called in the
 *	   callback function.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_ssl:	IN:  -
 *  @param in_verify_result: 	IN:  -
 *  @param in_confirm: 	IN:  -
 *  @return None.
 **/
void HS_HTTPLibServerCertConfirm(UInt16 refnum, HS_HTTPLibSSL in_ssl, Int32 in_verify_result, Boolean in_confirm)
		SYS_TRAP(kHTTPLibTrapServerCertConfirm);

/**
 *  @brief This function sets the client certification select callback function.For now, there is
 *	   no corresponding function in SSL library, the callback will not be called.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param libH:	IN:  -
 *  @param in_cb: 	IN:  -
 *  @param in_opaque:	IN:  -
 *  @return None.
 **/
void HS_HTTPLibSetSSLClientCertSelectProc(UInt16 refnum, HS_HTTPLibHandle libH, HS_HTTPLibClientCertSelectProc in_cb, HS_HTTPLibOpaque in_opaque)
		SYS_TRAP(kHTTPLibTrapSetSSLClientCertSelectProc);

/**
 *  @brief This function sets the selected certification. For now, there is no corresponding
 *	   function in SSL library, the callback will not be called.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_ssl:	IN:  -
 *  @param in_index: 	IN:  -
 *  @return None.
 **/
void HS_HTTPLibClientCertSelect(UInt16 refnum, HS_HTTPLibSSL in_ssl, Int32 in_index)
		SYS_TRAP(kHTTPLibTrapClientCertSelect);

/**
 *  @brief This function sets the callback functions concerning NetLib. But for now, there is
 *	   no call to callback functions from Peer layer.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param libH:	IN:
 *  @param netLibRefnum: 	IN:
 *  @param in_open:	IN:
 *  @param in_close:	IN:
 *  @param in_online:	IN:
 *  @param in_offline:	IN:
 *  @return None.
 **/
void HS_HTTPLibSetNetLibProc(UInt16 refnum, HS_HTTPLibHandle libH, UInt16 netLibRefnum, HS_HTTPLibNetLibOpenProc in_open, HS_HTTPLibNetLibCloseProc in_close, HS_HTTPLibNetLibOnlineProc in_online, HS_HTTPLibNetLibOfflineProc in_offline)
		SYS_TRAP(kHTTPLibTrapSetNetLibProc);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param in_handle:	IN:
 *  @retval Err error code.
 **/
Int32 HS_HTTPLibDataHandle_Length(UInt16 refnum, HS_HTTPLibDataHandle in_handle)
		SYS_TRAP(kHTTPLibTrapDataHandle_Length);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param in_handle:	IN:
 *  @retval Err error code.
 **/
void *HS_HTTPLibDataHandle_Lock(UInt16 refnum, HS_HTTPLibDataHandle in_handle)
		SYS_TRAP(kHTTPLibTrapDataHandle_Lock);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param in_handle:	IN:
 *  @retval Err error code.
 **/
void HS_HTTPLibDataHandle_Unlock(UInt16 refnum, HS_HTTPLibDataHandle in_handle)
		SYS_TRAP(kHTTPLibTrapDataHandle_Unlock);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param in_handle:	IN:
 *  @retval Err error code.
 **/
void HS_HTTPLibDataHandle_Delete(UInt16 refnum, HS_HTTPLibDataHandle in_handle)
		SYS_TRAP(kHTTPLibTrapDataHandle_Delete);

/**
 *  @brief This function returns the length of the handle (byte).
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_vhandle:	IN:  The handle.
 *  @retval Int32
 **/
Int32 HS_HTTPLibVHandle_Length(UInt16 refnum, HS_HTTPLibVHandle in_vhandle)
		SYS_TRAP(kHTTPLibTrapVHandle_Length);

/**
 *  @brief This function locks the handle of type HS_HTTPLibVHandle.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_vhandle:	IN:  The handle.
 *  @retval void *
 **/
void *HS_HTTPLibVHandle_Lock(UInt16 refnum, HS_HTTPLibVHandle in_vhandle)
		SYS_TRAP(kHTTPLibTrapVHandle_Lock);

/**
 *  @brief This function unlocks the handle of type HS_HTTPLibVHandle.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_vhandle:	IN:  The handle.
 *  @retval Nothing
 **/
void HS_HTTPLibVHandle_Unlock(UInt16 refnum, HS_HTTPLibVHandle in_vhandle)
		SYS_TRAP(kHTTPLibTrapVHandle_Unlock);

/**
 *  @brief This function deletes the handle of type HS_HTTPLibVHandle.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_vhandle:	IN:  The handle.
 *  @retval Nothing
 **/
void HS_HTTPLibVHandle_Delete(UInt16 refnum, HS_HTTPLibVHandle in_vhandle)
		SYS_TRAP(kHTTPLibTrapVHandle_Delete);

/**
 *  @brief This function returns the length of the handle (byte).
 *
 *  @param refnum: 	IN: Library reference number.
 *  @param in_str:	IN: The handle.
 *  @retval Int32
 **/
Int32 HS_HTTPLibString_Length(UInt16 refnum, HS_HTTPLibString in_str)
		SYS_TRAP(kHTTPLibTrapString_Length);

/**
 *  @brief This function creates a new handle of type HS_HTTPLibString.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param *in_str:	IN:  The pointer to the string.
 *  @param in_len: 	IN:  The length of in_str.
 *  @retval HS_HTTPLibString
 **/
HS_HTTPLibString HS_HTTPLibStringNew(UInt16 refnum, Char *in_str, Int32 in_len)
		SYS_TRAP(kHTTPLibTrapStringNew);

/**
 *  @brief This function locks the handle of type HS_HTTPLibString.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_str:	IN:  The handle.
 *  @retval void *
 **/
void *HS_HTTPLibString_Lock(UInt16 refnum, HS_HTTPLibString in_str)
		SYS_TRAP(kHTTPLibTrapString_Lock);

/**
 *  @brief This function unlocks the handle of type HS_HTTPLibString.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_str:	IN:  The handle.
 *  @retval Nothing
 **/
void HS_HTTPLibString_Unlock(UInt16 refnum, HS_HTTPLibString in_str)
		SYS_TRAP(kHTTPLibTrapString_Unlock);
/**
 *  @brief This function deletes the handle of type HS_HTTPLibString.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_str:	IN:  The handle.
 *  @retval Nothing
 **/
void HS_HTTPLibString_Delete(UInt16 refnum, HS_HTTPLibString in_str)
		SYS_TRAP(kHTTPLibTrapString_Delete);

/**
 *  @brief This function returns the length of the handle (byte).
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_str:	IN:  The handle.
 *  @retval Int32
 **/
Int32 HS_HTTPLibFixedString_Length(UInt16 refnum, HS_HTTPLibFixedString in_str)
		SYS_TRAP(kHTTPLibTrapFixedString_Length);

/**
 *  @brief This function creates a new handle of type HS_HTTPLibFixedString.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param *in_str:	IN:  The pointer to the string.
 *  @param in_len: 	IN:  The length of in_str.
 *  @retval HS_HTTPLibFixedString
 **/
HS_HTTPLibFixedString HS_HTTPLibFixedStringNew(UInt16 refnum, Char *in_str, Int32 in_len)
		SYS_TRAP(kHTTPLibTrapFixedStringNew);

/**
 *  @brief This function locks the handle of type HS_HTTPLibFixedString.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_str:	IN:  The handle.
 *  @retval void *
 **/
void *HS_HTTPLibFixedString_Lock(UInt16 refnum, HS_HTTPLibFixedString in_str)
		SYS_TRAP(kHTTPLibTrapFixedString_Lock);

/**
 *  @brief This function unlocks the handle of type HS_HTTPLibFixedString.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_str:	IN:  The handle.
 *  @retval nothing
 **/
void HS_HTTPLibFixedString_Unlock(UInt16 refnum, HS_HTTPLibFixedString in_str)
		SYS_TRAP(kHTTPLibTrapFixedString_Unlock);

/**
 *  @brief This function deletes the handle of type HS_HTTPLibFixedString.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_str:	IN:  The handle.
 *  @retval nothing
 **/
void HS_HTTPLibFixedString_Delete(UInt16 refnum, HS_HTTPLibFixedString in_str)
		SYS_TRAP(kHTTPLibTrapFixedString_Delete);

/**
 *  @brief This function returns the length of the handle (byte).
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_str:	IN:  The handle.
 *  @retval Int32
 **/
Int32 HS_HTTPLibURLString_Length(UInt16 refnum, HS_HTTPLibURLString in_str)
		SYS_TRAP(kHTTPLibTrapURLString_Length);

/**
 *  @brief This function creates a new handle of type HS_HTTPLibURLString.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param *in_str:	IN:  The pointer to the string.
 *  @param in_len: 	IN:  The length of in_str.
 *  @retval HS_HTTPLibURLString
 **/
HS_HTTPLibURLString HS_HTTPLibURLStringNew(UInt16 refnum, Char *in_str, Int32 in_len)
		SYS_TRAP(kHTTPLibTrapURLStringNew);

/**
 *  @brief This function locks the handle of type HS_HTTPLibURLString.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_str:	IN:  The handle.
 *  @retval void *
 **/
void *HS_HTTPLibURLString_Lock(UInt16 refnum, HS_HTTPLibURLString in_str)
		SYS_TRAP(kHTTPLibTrapURLString_Lock);

/**
 *  @brief This function unlocks the handle of type HS_HTTPLibURLString.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_str:	IN:  The handle.
 *  @retval nothing
 **/
void HS_HTTPLibURLString_Unlock(UInt16 refnum, HS_HTTPLibURLString in_str)
		SYS_TRAP(kHTTPLibTrapURLString_Unlock);

/**
 *  @brief This function deletes the handle of type HS_HTTPLibURLString.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param in_str:	IN:  The handle.
 *  @retval nothing
 **/
void HS_HTTPLibURLString_Delete(UInt16 refnum, HS_HTTPLibURLString in_str)
		SYS_TRAP(kHTTPLibTrapURLString_Delete);

/**
 *  @brief This returns the cipher info.
 *	   SslCipherSuiteInfo is the structure defined in SslLib.h
 *
 *	   The implementation of this API in ARM native version is mere macro.
 *
 *  @param refnum: 	IN:  Library reference number.
 *  @param *iself:	IN:  -
 *  @retval nothing
 **/
void *HS_HTTPLibCertListGetCipherInfo(UInt16 refnum, void *iself)
		SYS_TRAP(kHTTPLibTrapCertListGetCipherInfo);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param libH:	IN:
 *  @param in_msec:: 	IN:
 *  @retval Err error code.
 **/
void HS_HTTPLibSetMaxKeepAliveTimeout(UInt16 refnum, HS_HTTPLibHandle libH, Int32 in_msec)
        SYS_TRAP(kHTTPLibTrapSetMaxKeepAliveTimeout);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param stream:	IN:
 *  @param *out_total: 	IN:
 *  @param *out_sent:	IN:
 *  @retval Err error code.
 **/
void HS_HTTPLibStreamGetRequestSent(UInt16 refnum, HS_HTTPLibStream stream, Int32 *out_total, Int32 *out_sent)
        SYS_TRAP(kHTTPLibTrapStreamGetRequestSent);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param stream:	IN:
 *  @param in_type 	IN:
 *  @param in_ofs:	IN:
 *  @param *out_off:	IN:
 *  @param *out_len:	IN:
 *  @retval Err error code.
 **/
Boolean HS_HTTPLibStreamGetHeaderWithOffset(UInt16 refnum, HS_HTTPLibStream stream, Int32 in_type, Int32 in_ofs, Int32 *out_off, Int32 *out_len)
        SYS_TRAP(kHTTPLibTrapStreamGetHeaderWithOffset);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param libH:	IN:
 *  @param in_type: 	IN:
 *  @param in_target:	IN:
 *  @param *in_challenge:	IN:
 *  @param *in_realm:	IN:
 *  @param *in_user:	IN:
 *  @param in_user_len:	IN:
 *  @param *in_pass:	IN:
 *  @param in_pass_len:	IN:
 *  @param *in_url:	IN:
 *  @param in_url_len:	IN:
 *  @param *in_host:	IN:
 *  @param in_host_len:	IN:
 *  @param in_port:	IN:
 *  @param in_keep_user:	IN:
 *  @param in_keep_pass:	IN:
 *  @retval Err error code.
 **/
Boolean HS_HTTPLibSetUserPassX(UInt16 refnum, HS_HTTPLibHandle libH, Int32 in_type, Int32 in_target, HS_HTTPLibSplit *in_challenge, HS_HTTPLibSplit *in_realm, Char *in_user, Int32 in_user_len, Char *in_pass, Int32 in_pass_len, Char *in_url, Int32 in_url_len, Char *in_host, Int32 in_host_len, Int32 in_port, Boolean in_keep_user, Boolean in_keep_pass)
        SYS_TRAP(kHTTPLibTrapSetUserPassX);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param libH:	IN:
 *  @param in_target: 	IN:
 *  @param *in_url:	IN:
 *  @param in_url_len:	IN:
 *  @param *in_host:	IN:
 *  @param in_host_len:	IN:
 *  @param in_port:	IN:
 *  @param *in_realm:	IN:
 *  @param in_realm_len:	IN:
 *  @param *out_user:	IN:
 *  @param *out_pass:	IN:
 *  @param *out_keep_user:	IN:
 *  @param *out_keep_pass:	IN:
 *  @param libH:	IN:
 *  @retval Err error code.
 **/
void HS_HTTPLibFindAuthCandidate(UInt16 refnum, HS_HTTPLibHandle libH, Int32 in_target, Char *in_url, Int32 in_url_len, Char *in_host, Int32 in_host_len, Int32 in_port, Char *in_realm, Int32 in_realm_len, HS_HTTPLibString *out_user, HS_HTTPLibString *out_pass, Boolean *out_keep_user, Boolean *out_keep_pass)
        SYS_TRAP(kHTTPLibTrapFindAuthCandidate);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param libH:	IN:
 *  @param in_proc: 	IN:
 *  @param in_opaque:	IN:
 *  @retval Err error code.
 **/
void HS_HTTPLibSetWakeUpCallback(UInt16 refnum, HS_HTTPLibHandle libH, HS_HTTPLibWakeUpCallbackProc in_proc, HS_HTTPLibOpaque in_opaque)
        SYS_TRAP(kHTTPLibTrapSetWakeUpCallback);

/**
 *  @brief
 *
 *  @param refnum: 	IN:
 *  @param libH:	IN:
 *  @param *rules: 	IN:
 *  @param len:		IN:
 *  @retval Err error code.
 **/
Boolean HS_HTTPLibSetUseragentSelectionRule(UInt16 refnum, HS_HTTPLibHandle libH, HttpUASelectionRule *rules, Int32 len)
		SYS_TRAP(kHTTPLibTrapSetUseragentSelectionRule);



#endif /* HS_HTTPLIB68K_H__ */
