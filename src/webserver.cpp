/*
 * File         src/webserver.hpp
 * Author       Heiko Klausing (h dot klausing at gmx do de)
 * Created      2020-09-21
 * Description  Handling of the web server part of this project:
 *              - user requerst for pages
 *              - page delivery
 *              - unknown page handling
 */

#include <ESP8266HTTPClient.h>

#include "signal.hpp"
#include "webserver.hpp"
#include "wifiserver.hpp"

static const String REQUEST_GET_START{"GET "};



PrjWebServer::PrjWebServer(WiFiServer *wifi_server)
    : m_wifi_server{wifi_server}
    , m_page_request_counter{0}
{
}

PrjWebServer::~PrjWebServer()
{
}

void PrjWebServer::processClient(void)
{
    // Check if a client has connected
    WiFiClient wifi_client = m_wifi_server->available();
    if (!wifi_client)
    {
        return;
    }
    else
    {

        // get name of requested page
        switch (getPageRequest(wifi_client))
        {
        case Request_t::REQUEST_INDEX:
            webPageActivityLed.ledOn();
            page_Index(wifi_client);
            break;
        case Request_t::REQUEST_INFO:
            webPageActivityLed.ledOn();
            page_Info(wifi_client);
            break;
        case Request_t::REQUEST_GRAPH:
            webPageActivityLed.ledOn();
            page_Graph(wifi_client);
            break;
        case Request_t::REQUEST_MEASVAL_JS:
            webPageActivityLed.ledOn();
            page_MeasValue(wifi_client);
            break;
        case Request_t::REQUEST_UNKNOWN:
            webPageActivityLed.ledOn();
            page_Unknown(wifi_client);
            break;
        case Request_t::REQUEST_RESTART:
            webPageActivityLed.ledOn();
            page_Restart(wifi_client);
            break;
        case Request_t::NO_REQUEST:
            webPageActivityLed.ledOn();
            // no request found
            break;
        default:
            webPageActivityLed.ledOn();
            // help
            Serial.println(F("ERROR: unknown page request answer!"));
        };

        // and stop the client
        wifi_client.stop();
        webPageActivityLed.ledOff();
    }
}

/*
 * This function checks if a page request is received from a external
 * client. If a request is found the page will be returned
 */
Request_t PrjWebServer::getPageRequest(WiFiClient &wifi_client)
{
    // Check if a client has connected
    if (!wifi_client)
    {
        // no request found return immediatly
        return Request_t::NO_REQUEST;
    }

    // Read the first line of the request
    String request_input = wifi_client.readStringUntil('\r');
    wifi_client.flush();

    // stop client, if request is empty
    if (request_input == "")
    {
        wifi_client.stop();
        return Request_t::NO_REQUEST;
    }

    Serial.print("page requested");

    /* get path; end of path is either space or ?
       Syntax is e.g. GET /?show=1234 HTTP/1.1
       String sPath="",sParam="", sCmd="";
       String sGetstart="GET ";
       int iStart, iEndSpace, iEndQuest;
    */
    m_request_path.clear();
    int param_startid;
    int param_endif;
    int param_end_query;
    param_startid = request_input.indexOf(REQUEST_GET_START);
    if (param_startid >= 0)
    {
        Serial.print(" with parameter");
        param_startid += REQUEST_GET_START.length();
        param_endif = request_input.indexOf(" ", param_startid);
        param_end_query = request_input.indexOf("?", param_startid);

        // are there parameters?
        if (param_endif > 0)
        {
            if (param_end_query > 0)
            {
                // there are parameters
                m_request_path = request_input.substring(param_startid, param_end_query);
                m_request_parameter = request_input.substring(param_end_query + 1, param_endif);

                if (!m_request_parameter.isEmpty())
                {

                    //if(sParam.lastIndexOf)

                    // check for:
                    // - range=[6|12|24|48]
                    //   = sParam.indexOf("range=",0);
                    // - calibration=<float>
                    //   = sParam.indexOf("calibartion=",0);
                    // - hostname=<name>
                    //   = sParam.indexOf("hostname=",0);
                    //
                    // if(sParam.length) {
                    //   int equalSign = sParam.indexOf("=", 0);
                }
            }
            else
            {
                // NO parameters
                m_request_path = request_input.substring(param_startid, param_endif);
            }
        }
    }

    m_page_request_counter++;

    // search the requrest page in name in supported list
    for (size_t i = 0; i < req_pages_size; i++)
    {
        if (m_request_path == req_pages[i].req_page)
        {
            Serial.printf(" - request for %s\n", req_pages[i].req_page);
            return req_pages[i].req_id;
        }
    }

    return Request_t::REQUEST_UNKNOWN;
}

int PrjWebServer::getRequestedPages(void)
{
    return m_page_request_counter;
}

void PrjWebServer::incrementPageRequestCounter(void)
{
    m_page_request_counter++;
}


PrjWebServer g_prj_web_server(&g_wifi_server);
