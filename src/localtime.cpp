/*
 * File         localtime.cpp
 * Author       Heiko Klausing (h dot klausing at gmx dot de)
 * Created      2020-08-03
 * Note         Class definition for local time.
 */

#include "localtime.h"

#define LI_VN_MODE(li,vn,mode) (((li & 0b11) << 6) | ((vn & 0b111) << 3) | (mode & 0b111))


Localtime::Localtime(const char* servers[], const int size)
    : m_servers(servers)
    , m_server_size(size)
	, m_next_sync(0)
    , m_updated(false)
    , m_timezone_defined(TZ_NOT_DEFINED)
{}

Localtime::~Localtime()
{}

bool Localtime::updateTimer()
{

    // update required?
	if (/*timeStatus() == timeSet &&*/ m_updated && m_next_sync > now()) {
		// time update not required
		return m_updated;
	}

	m_updated = false;

	// initialize, start listening on specified port.
	m_udp.begin(m_ntp_port);

    // loop over all known NTP servers, stop after first time answer
    for(int i=0; i < m_server_size; i++) {

		// ask NTP server and stop if answer was ok
        if(getNtpTime(i)) {
            m_updated = true;
			m_next_sync = now() + 6 * 60 * 60;
			break;
        }
    }

	// inform user about missing NTP servers
    if(!m_updated) {
        Serial.println(F("ERROR: no NTP data loaded!"));
    }

	return m_updated;
}


void Localtime::setTimeZone(TimeChangeRule std_time)
{
    if(m_timezone_defined == TZ_NOT_DEFINED) {
        m_timezone_defined = TZ_ONLY_STANDARD;
		m_std_time = std_time;		// standard time
    }
}

void Localtime::setTimeZone(TimeChangeRule dst_time, TimeChangeRule std_time)
{
    if(m_timezone_defined == TZ_NOT_DEFINED) {
        m_timezone_defined = TZ_CHANGE_TIMES;
		m_dst_time = dst_time;		// summer time
		m_std_time = std_time;		// standard time
    }
}


time_t Localtime::localNow()
{
	time_t time_val = now();

    if(m_timezone_defined == TZ_CHANGE_TIMES) {
		Timezone TZ(m_dst_time, m_std_time);
		time_val = TZ.toLocal(time_val);
    } else if (m_timezone_defined == TZ_ONLY_STANDARD) {
		Timezone TZ(m_std_time);
		time_val = TZ.toLocal(time_val);
	}

    return time_val;
}


bool Localtime::status(void)
{
	return m_updated;
}


/*****************************************************************************
 * private methods
 *****************************************************************************/

bool Localtime::getNtpTime(int ntpServerNameId)
{
	Serial.print(F("NTP request..."));

    // get ride of older packets
	while (m_udp.parsePacket() > 0);

	Serial.println(F("Transmit NTP Request"));

	// get a random server from the pool
	IPAddress ntpServerIP; // NTP server's ip address
	WiFi.hostByName(m_servers[ntpServerNameId], ntpServerIP);

	// Serial.printf("ask NTP server: %s (%s)\n", m_servers[ntpServerNameId], ntpServerIP.toString().c_str());

	if(sendNtpPacket(ntpServerIP)){

		//Serial.println(F("NTP server is asked"));

		// read NTP answer with timeout handling (1.5 sec)
		uint32_t beginWait = millis() + 1500;
		while (millis() < beginWait) {

			if (m_udp.parsePacket() == sizeof(m_packet)) {

				Serial.println(F("Receive NTP Response"));

  				// read answer packet from NTP server into the packet buffer
				m_udp.read((char*)&m_packet, sizeof(m_packet));

				// get the result and convert it for unix time (1.1.1970 0:0:0)
				m_packet.txTm_s = ntohl(m_packet.txTm_s) - 2208988800UL;
				// set system time
				setTime(m_packet.txTm_s);
				return true;
			}
		}
		Serial.println(F("FATAL ERROR : No NTP Response."));

	}
	else
		Serial.println(F("ERROR: NTP send failed"));
	return false; // return 0 if unable to get the time
}



// send an NTP request to the time server at the given address
bool Localtime::sendNtpPacket(IPAddress &ntp_server_address)
{
	// clean packet
	memset(&m_packet, 0, sizeof(ntp_packet_t));

	// Initialize values needed to form NTP request
	const uint8_t leap_indicator = 0;	// no warn or alarm condition
	const uint8_t version_number = 3;	// NTP version
	const uint8_t pick_mode      = 3;	// I'm a client
	m_packet.li_vn_mode = LI_VN_MODE(leap_indicator,version_number,pick_mode);

	// Request an answer packet from NTP server
	int res1 = m_udp.beginPacket(ntp_server_address, m_ntp_port);
	int res2 = m_udp.write((char*)&m_packet, sizeof(ntp_packet_t));
	int res3 = m_udp.endPacket();

	// Serial.printf("beginPacket: %i, write: %i, endPacket: %i\n", res1, res2, res3);

	return res1 ==1 && res2 == m_packet_size && res3 == 1;
}
