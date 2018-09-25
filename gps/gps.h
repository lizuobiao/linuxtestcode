#ifndef __GPS_H_
#define __GPS_H_

#include "Gateway.h"

class GPS{
    public:
        GPS(std::string dev);
        ~GPS();
		bool WaitPositioningComplete(unsigned int time/*second*/);
    protected:
//		virtual bool OnGetLocation(GatewayLocation *location);

    private:
		
		bool OpenGPS();
		void CloseGPS();
        void OnGPSReceived(std::vector<std::string> &data);
        void StringToArray(std::string &str, std::vector<std::string> &list);
        inline static double LongitudeToValue(double longitude, std::string &ori);
        inline static double LatitudeToValue(double latitude, std::string &ori);
        
        static void *GPSReceiveThread(void *param);
//        static void *DataPointThread(void *param);

    private:
        bool is_run_;
        bool is_positioning_;
        int gps_dev_fd_;
        int gps_timeout_;
		std::string uartdev;

        pthread_t datapoint_thread_;
        pthread_t gps_recv_thread_;
        pthread_cond_t positioning_cond_;
        pthread_mutex_t positioning_mutex_;
        pthread_mutex_t location_mutex_;
//		GatewayLocation location_;
    };


#endif  // __WEATHERSTATIONM1_H_
