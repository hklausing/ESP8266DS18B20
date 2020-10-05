/*
 * File         src/webpages.hpp
 * Author       Heiko Klausing (h dot klausing at gmx do de)
 * Created      2020-09-22
 * Description  Contains the pages of the websever
 */

#include "webserver.hpp"
#include "meas.h"
#include "parameter.hpp"
#include "measbuffer.hpp"
#include "timehelper.h"

/*******************************************************************************
 * Helper functions
 ******************************************************************************/

/**
 * @brief Get the Style Definitions object
 * 
 * @return String 
 */
String getStyleDefinitions()
{
    String style = F(
        "<style>"
        "h1 {background-color: rgb(75, 75, 223);color: white;height: 40px;padding-left: 10px;}"
        //        "h1 {color: Indigo; font-size:32px;}"
        "h2 {color: DarkSlateBlue; font-size:24px;}"
        ".data {color: DarkSlateGray; font-size: 20px; padding-left: 1em;}"
        "button { height: 40px; min-width: 100px; font-size: 1.1em;}"
        ".buttons {text-align: left;}"
        ".info {text-align: left; font-size: 0.7em;}"
        "</style>");
    return style;
}

/*
 * HTTP 1.1 header with length information
 * Has to be used before sending a web page
 * Type: 'text/html', 'application/json'
 */
String getHTTPTypeSizeHeader(String type, uint32_t send_size)
{
    String header;

    header = "HTTP/1.1 200 OK\r\nContent-Length: " + String(send_size) + "\r\nContent-Type: " + type + "\r\nConnection: close\r\n\r\n";

    return (header);
}

String getHtmlHeadStartSequence(const String &title, uint32_t refresh)
{
    String head = F("<!DOCTYPE html>"
                    "<html>"
                    "<head>"
                    "<meta charset=\"utf-8\">");

    head += F("<title>");
    head += title;
    head += F("</title>");

    head += getStyleDefinitions();
    if (refresh)
    {
        // add refresh if refresh is required (>0!)
        head += F("<meta http-equiv=\"refresh\" content=\"");
        head += refresh;
        head += F("\" >");
    }
    return (head);
}

String getLinkList(void)
{
    return String(F("<p>"
                    "<a href=\"/\"><button>Dashboard</button></a> "
                    "<a href=\"/graph\"><button>Graph</button></a> "
                    "<a href=\"/info\"><button>Information</button></a> "
                    "<a href=\"/measval.js\"><button>JSON temperature file</button></a> "
                    "</p>"));
}

String getInfoText(void)
{
    String info = F(
        "<p class=\"info\">"
        "Page requests = ");
    info += String(g_prj_web_server.getRequestedPages());
    info += F(", free RAM = ");
    info += String(system_get_free_heap_size());
    info += F(", max. data points = ");
    info += String(g_ringbuffer.content());
    info += F(", scanned data points = ");
    info += String(g_ringbuffer.size());
    info += F("</p>");
    return info;
}

/*******************************************************************************
 * Web Pages
 ******************************************************************************/

uint32_t sendPage_Index(WiFiClient *client)
{
    uint32_t send_size = 0;
    String answer;
    // build page content
    answer = getHtmlHeadStartSequence("Actual Temperature", 60);
    answer += F(
        "<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>"
        "<script type=\"text/javascript\">"
        "google.charts.load('current', { 'packages': ['gauge'] });"
        "google.charts.setOnLoadCallback(drawChart);"
        "function drawChart() {"
        "var data = google.visualization.arrayToDataTable(["
        "['Label', 'Value'],['Temp °C',");
    char buf[16];
    sprintf(buf, "%.1f", g_temp_meas.getValue());
    answer += buf;
    answer += F(
        "]]);"
        "var options = {"
        "min: 15, max: 40,"
        "greenFrom: 19, greenTo: 26,"
        "yellowFrom: 26, yellowTo: 29,"
        "redFrom: 29, redTo: 40,"
        "minorTicks: 5, majorTicks: [15, 20, 25, 30, 35, 40]"
        "};"
        "var chart = new google.visualization.Gauge(document.getElementById('chart_div'));"
        "chart.draw(data, options);"
        "}"
        "</script>"
        "</head>"
        "<body>"
        "<h1>Actual Temperature: ");
    answer += getLocation();
    answer += F("</h1>"
                "<div id=\"chart_div\" style=\"width: 800px; height: 400px;\"></div>");
    answer += getLinkList();
    answer += getInfoText();
    answer += F(
        "</body>"
        "</html>");
    // .. and get the size
    send_size += answer.length();
    // Send the response to the client if required
    if (client)
    {
        client->print(answer);
    }
    return send_size;
}

void page_Index(WiFiClient &wifi_client)
{
    // get page size
    uint32_t send_size = 0;
    send_size += sendPage_Index(NULL);
    // send HTTP header with size information
    wifi_client.print(getHTTPTypeSizeHeader("text/html", send_size));
    // send page
    sendPage_Index(&wifi_client);
}

uint32_t sendPage_Info(WiFiClient *client)
{
    uint32_t send_size = 0;
    // build page content
    String answer;
    answer = getHtmlHeadStartSequence("Information Page", 300 / 2);
    answer += F(
        "</head>"
        "<body>"
        "<h1>Information Page</h1>"
        "<h2>Internet</h2>");
    answer += F("<div class=\"data\">Hostname: ");
    answer += WiFi.hostname();
    answer += F("</div>");

    answer += F("<div class=\"data\">IP address: ");
    answer += WiFi.localIP().toString();
    answer += F("</div>");

    answer += F("<div class=\"data\">MAC address: ");
    answer += WiFi.macAddress();
    answer += F("</div>");

    answer += F("<div class=\"data\">Page requests: ");
    answer += g_prj_web_server.getRequestedPages();
    answer += F("</div>");

    answer += F("<h2>Measurement</h2>");

    answer += F("<div class=\"data\">Location: ");
    answer += getLocation();
    answer += F("</div>");

    answer += F("<div class=\"data\">DS18B20 serial code: ");
    answer += g_temp_meas.getSerialCode();
    answer += F("</div>");

    answer += F("<div class=\"data\">Max. measurement buffer size: ");
    answer += g_ringbuffer.content();
    answer += F("</div>");

    answer += F("<div class=\"data\">Added measurements to buffer: ");
    answer += g_ringbuffer.size();
    answer += F("</div>");

    answer += F("<div class=\"data\">Measurement interval: ");
    answer += g_timer_values.store_interval / 1000;
    answer += F(" sec</div>");

    answer += F("<div class=\"data\">Measurement range: ");
    answer += RINGBUFFER_SIZE * (g_timer_values.store_interval / 1000.0) / (60.0 * 60.0 * 24.0);
    answer += F(" days</div>");

    answer += F("<div class=\"data\">Memory measurement buffer: ");
    answer += RINGBUFFER_SIZE * sizeof(measValue_t);
    answer += F(" byte</div>");

    if (g_ringbuffer.size())
    {
        answer += F("<div class=\"data\">Actual temperature: ");
        answer += g_ringbuffer.readLast().temperature;
        answer += F(" °C</div>");

        answer += F("<div class=\"data\">Start logger time: ");
        answer += convertEpochToIso8601(g_timer_values.start_timestamp);
        answer += F("</div>");

        answer += F("<div class=\"data\">Newest measurement time: ");
        answer += convertEpochToIso8601(g_ringbuffer.readLast().timestamp);
        answer += F("</div>");

        answer += F("<div class=\"data\">Oldest measurement time: ");
        answer += convertEpochToIso8601(g_ringbuffer.readFirst().timestamp);
        answer += F("</div>");
    }

    answer += F("<div class=\"data\">Temperature correction value: ");
    answer += getTempCorrection();
    answer += F(" °K</div>");

    answer += F("<h2>System</h2>");
    answer += F("<div class=\"data\">Author: " SOFTWARE_AUTHOR "</div>");
    answer += F("<div class=\"data\">Program version: " SOFTWARE_VERSION "</div>");
    answer += F("<div class=\"data\">Release date: " RELEASE_DATE "</div>");
    answer += F("<div class=\"data\">Compile time: " __DATE__ ", " __TIME__ "</div>");

    answer += F("<div class=\"data\">Compiler: ");
    answer += __cplusplus;
    answer += F("L</div>");

    answer += F("<div class=\"data\">Free memory: ");
    answer += system_get_free_heap_size();
    answer += F("</div>");

#ifdef ARDUINO_OTA_ENABLE
    answer += F("<div class=\"data\">Arduino feature OTA: enabled</div>");
#else
    answer += F("<div class=\"data\">Arduino feature OTA: disabled</div>");
#endif

    answer += getLinkList();
    answer += F("</body>");
    answer += F("</html>");

    // .. and get the size
    send_size += answer.length();
    // Send the response to the client if required
    if (client)
    {
        client->print(answer);
    }
    return send_size;
}

void page_Info(WiFiClient &wifi_client)
{
    // get page size
    uint32_t send_size = 0;
    send_size += sendPage_Info(NULL);
    // send HTTP header with size information
    wifi_client.print(getHTTPTypeSizeHeader("text/html", send_size));
    // send page
    sendPage_Info(&wifi_client);
}

uint32_t sendPage_Graph(WiFiClient *client)
{
    uint32_t send_size = 0;
    // build page content
    String answer;
    answer = getHtmlHeadStartSequence("Temperature Graph", 300 / 2);
    answer += F(
        "<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>"
        "<script type=\"text/javascript\">"
        "google.load('visualization', '1', { packages: ['controls', 'charteditor'] });"
        "google.setOnLoadCallback(drawChart);"
        "function drawChart() {"
        "var data = google.visualization.arrayToDataTable([");
    if (client)
    {
        client->print(answer);
    }
    send_size += answer.length();
    answer.clear();

    // working variables for loop
    char buf[64];
    struct tm ts;

    // get list of temperature/time values
    answer += "['Date/Time','Temperature °C']";
    for (size_t i = 0; i < g_ringbuffer.size(); i++)
    {
        // time/temperature value
        time_t epoch = g_ringbuffer.readFirst(i).timestamp;
        // gmtime is used to convert to localtime, because the eoch value is localtime
        ts = *gmtime(&epoch);
        sprintf(buf, ",[new Date(%04d,%d,%d,%d,%d,%d),%.1f]"
                // set time, month starts at 0 (google graph requirement)
                ,
                ts.tm_year + 1900, ts.tm_mon, ts.tm_mday, ts.tm_hour, ts.tm_min, ts.tm_sec
                // set temperature
                ,
                g_ringbuffer.readFirst(i).temperature);
        answer += buf;

        // send/clear buffer if buffersize limit is reached
        if (answer.length() > HTTP_BLOCK_SIZE)
        {
            if (client)
            {
                client->print(answer);
            }
            send_size += answer.length();
            answer.clear();
        }
    }

    // get rid of the rest of data list
    if (client)
    {
        client->print(answer);
    }
    send_size += answer.length();
    answer.clear();
    // set the rest of the html page
    answer += F("]);"
                "var dash = new google.visualization.Dashboard(document.getElementById('dashboard'));"
                "var chart = new google.visualization.ChartWrapper({"
                "chartType: 'ComboChart',"
                "containerId: 'chart_div',"
                "options: {"
                "title: 'Temperature Diagram',"
                "width: '100%',"
                "height: 720,"
                "chartArea: { left: 100, top: 40, width: '80%', height: '65%', right: 20, bottom: 80 },"
                "legend: {"
                "position: 'none',"
                "alignment: 'center',"
                "textStyle: {"
                "fontSize: 12"
                "}"
                "},"
                "backgroundColor: '#FEFDDE',"
                "explorer: {"
                "actions: ['dragToZoom', 'rightClickToReset'],"
                "axis: 'horizontal',"
                "keepInBounds: true"
                "},"
                "hAxis: {"
                "title: 'Time'"
                "},");

    if (g_ringbuffer.size() < 200)
        answer += "pointSize: 3,";
    answer += F("vAxis: {"
                "title: 'Temp [°C]'"
                "},"
                "series: {"
                "0: {"
                "curveType: 'function',"
                "color: '#0080FF'"
                "}"
                "},"
                "}"
                "});"
                "var control = new google.visualization.ControlWrapper({"
                "controlType: 'ChartRangeFilter',"
                "containerId: 'control_div',"
                "options: {"
                "filterColumnIndex: 0,"
                "ui: {"
                "chartOptions: {"
                "height: 50,"
                "chartArea: {"
                "width: '100%',"
                "left: 20,"
                "right: 20"
                "}"
                "}"
                "}"
                "},"
                "state: {"
                "range: {"
                "start: new Date(");
    time_t epoch = g_ringbuffer.readFirst().timestamp;
    ts = *gmtime(&epoch);
    sprintf(buf, "%4d,%d,%d,%d,%d,%d", ts.tm_year + 1900, ts.tm_mon, ts.tm_mday, ts.tm_hour, ts.tm_min, ts.tm_sec);
    answer += buf;

    // get rid of the rest of data list
    if (client)
    {
        client->print(answer);
    }
    send_size += answer.length();
    answer.clear();

    answer += F(")"
                "}"
                "}"
                "});"
                "dash.bind([control], [chart]);"
                "dash.draw(data);"
                "}"
                "</script>"
                "</head>"
                "<body>"
                "<h1>Temperature: ");
    answer += getLocation();
    answer += F("</h1>"
                "<div id=\"dashboard_div\">"
                "<div id=\"chart_div\"> </div>"
                "<div id=\"control_div\"> </div>"
                "</div>");
    answer += getLinkList();
    answer += getInfoText();
    answer += F(
        "</body>"
        "</html>");

    // .. and get the size
    send_size += answer.length();
    // Send the response to the client if required
    if (client)
    {
        client->print(answer);
    }
    //DEBUG_PRINTLN(answer);
    return send_size;
}

void page_Graph(WiFiClient &wifi_client)
{
    // get page size
    uint32_t send_size = 0;
    send_size += sendPage_Graph(NULL);
    //DEBUG_PRINTF1("MeasAll size: %u\n", send_size);
    // send HTTP header with size information
    wifi_client.print(getHTTPTypeSizeHeader("text/html", send_size));
    // send page
    sendPage_Graph(&wifi_client);
}

uint32_t sendPage_MeasValue(WiFiClient *client)
{
    uint32_t send_size = 0;
    // build page content
    String answer;
    answer = F("[");
    answer += F("[\"Date/Time\",\"Temperature °C\"]");

    for (size_t i = 0; i < g_ringbuffer.size(); i++)
    {
        answer += ",\r\n[\"" + convertEpochToIso8601(g_ringbuffer.readFirst(i).timestamp) + "\"," + String(g_ringbuffer.readFirst(i).temperature) + "]";

        // check if max buffer size is reached, than send a block
        if (answer.length() > HTTP_BLOCK_SIZE)
        {
            send_size += answer.length();
            if (client)
            {
                client->print(answer);
            }
            //DEBUG_PRINT(answer);
            answer = "";
        }
    }

    answer += F("]");
    // .. and get the size
    send_size += answer.length();
    // Send the response to the client if required
    if (client)
    {
        client->print(answer);
    }
    //DEBUG_PRINTLN(answer);
    return send_size;
}

void page_MeasValue(WiFiClient &wifi_client)
{
    // get page size
    uint32_t send_size = 0;
    send_size += sendPage_MeasValue(NULL);
    // send HTTP header with size information
    wifi_client.print(getHTTPTypeSizeHeader("application/json", send_size));
    // send page
    sendPage_MeasValue(&wifi_client);
}

uint32_t sendPage_Unknown(WiFiClient *client)
{
    uint32_t send_size = 0;
    // build page content
    String answer;
    answer = F("<html>"
               "<head>"
               "<title>404 Not Found</title>"
               "</head>"
               "<body>"
               "<h1>Not Found</h1>"
               "<p>The requested URL was not found on this server.</p>"
               "</body>"
               "</html>");
    // .. and get the size
    send_size += answer.length();
    // Send the response to the client if required
    if (client)
    {
        client->print(answer);
    }
    return send_size;
}

void page_Unknown(WiFiClient &wifi_client)
{
    // get page size
    uint32_t send_size = 0;
    send_size += sendPage_Unknown(NULL);
    // send HTTP header with size information
    wifi_client.print(getHTTPTypeSizeHeader("text/html", send_size));
    // send page
    sendPage_Unknown(&wifi_client);
}

uint32_t sendPage_Restart(WiFiClient *client)
{
    uint32_t send_size = 0;
    // build page content
    String answer;
    answer = F("<html>"
               "<head>"
               "<title>Temperature Logger Reset</title>"
               "</head>"
               "<body>"
               "<p>Temperature Logger Reset</p>"
               "</body>"
               "</html>");
    // .. and get the size
    send_size += answer.length();
    // Send the response to the client if required
    if (client)
    {
        client->print(answer);
    }
    return send_size;
}

void page_Restart(WiFiClient &wifi_client)
{
    // get page size
    uint32_t send_size = 0;
    send_size += sendPage_Restart(NULL);
    // send HTTP header with size information
    wifi_client.print(getHTTPTypeSizeHeader("text/html", send_size));
    // send page
    sendPage_Unknown(&wifi_client);
    delay(250);
    ESP.reset();
}
