/*
 * File         src/webserver.hpp
 * Author       Heiko Klausing (h dot klausing at gmx do de)
 * Created      2020-09-21
 * Description  Handling of the web server part of this project.
 */

#pragma once

#include <WiFiServer.h>

/*
 * declare here the web pages; 
 * declared outside of the class PrjWebServer, in case of easier handling.
 */
uint32_t sendPage_Index(WiFiClient *client);
void page_Index(WiFiClient &wifi_client);

uint32_t sendPage_Info(WiFiClient *client);
void page_Info(WiFiClient &wifi_client);

uint32_t sendPage_Graph(WiFiClient *client);
void page_Graph(WiFiClient &wifi_client);

uint32_t sendPage_MeasValue(WiFiClient *client);
void page_MeasValue(WiFiClient &wifi_client);

uint32_t sendPage_Unknown(WiFiClient *client);
void page_Unknown(WiFiClient &wifi_client);

uint32_t sendPage_Restart(WiFiClient *client);
void page_Restart(WiFiClient &wifi_client);

// Request values
enum class Request_t
{
    NO_REQUEST=0,         // no request found
    REQUEST_INDEX,      // request for page index ("/")
    REQUEST_INFO,       // handle page info
    REQUEST_GRAPH,      // handle page graph ("/graph")
    REQUEST_MEASVAL_JS, // get a json list with all measurement values ("/measval.js")
    REQUEST_RESTART,    // restart temperature logger, clears the measurement queue ("/restart")
    REQUEST_UNKNOWN     // request for unknown page
};

class PrjWebServer
{
private:
    WiFiServer *m_wifi_server;

public:
    PrjWebServer(WiFiServer *wifi_server);
    ~PrjWebServer();

    void processClient(void);

    /**
     * @brief Get the Page Request object
     * 
     * @param wifi_client 
     * @return Request_t 
     */
    Request_t getPageRequest(WiFiClient &wifi_client);

    /**
     * @brief Increment page requoired counter
     * 
     */
    void incrementPageRequestCounter(void);

    /**
     * @brief Get the Page Request Counter object
     * 
     * @return int 
     */
    int getRequestedPages(void);

private:
    String m_request_path = ""; // input request from client
    String m_request_parameter = "";
    uint32_t m_page_request_counter = 0;

    // Web function pointer for page handling
    typedef void (*pageHandler_t)(WiFiClient &);

    typedef struct
    {
        Request_t req_id;
        char req_page[32];
        pageHandler_t pageHandler;
    } req_pages_t;

    const req_pages_t req_pages[6] = {
        {Request_t::REQUEST_INDEX, "/", &page_Index},
        {Request_t::REQUEST_INFO, "/info", &page_Info},
        {Request_t::REQUEST_GRAPH, "/graph", &page_Graph},
        {Request_t::REQUEST_MEASVAL_JS, "/measval.js", &page_MeasValue},
        {Request_t::REQUEST_RESTART, "/restart", &page_Restart},
        {Request_t::REQUEST_UNKNOWN, "", &page_Unknown},
    };
    const size_t req_pages_size = sizeof(req_pages) / sizeof(req_pages[0]);
};

extern PrjWebServer g_prj_web_server;
