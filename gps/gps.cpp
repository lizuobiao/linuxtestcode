#include "gps.h"

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/file.h>


GPS::GPS(std::string dev)
{
	uartdev = dev;
//	bzero(&location_, sizeof(location_));
	pthread_cond_init(&positioning_cond_, NULL);
    pthread_mutex_init(&location_mutex_, NULL);
    pthread_mutex_init(&positioning_mutex_, NULL);

	OpenGPS();
}
GPS::~GPS()
{
	pthread_cond_destroy(&positioning_cond_);
    pthread_mutex_destroy(&location_mutex_);
    pthread_mutex_destroy(&positioning_mutex_);
	
	CloseGPS();
}

bool GPS::OpenGPS() {
    struct termios opts;

    if ((gps_dev_fd_ = open(uartdev.c_str(), O_RDWR | O_NOCTTY/* | O_NDELAY*/)) == -1) {
        std::cerr << "OpenGPS failed." << std::endl;
        return false;
    }

    tcgetattr(gps_dev_fd_, &opts);
    opts.c_cc[VTIME] = 30;
    opts.c_cc[VMIN] = 0;
    tcsetattr(gps_dev_fd_, TCSANOW, &opts);
    pthread_create(&gps_recv_thread_, NULL, GPSReceiveThread, this);

    return true;
}

void GPS::CloseGPS() {
    pthread_join(gps_recv_thread_, 0);
    pthread_detach(gps_recv_thread_);
    close(gps_dev_fd_);
}

void GPSReader::OnPositioning() {

}


void GPS::OnGPSReceived(std::vector<std::string> &data) {
    std::istringstream iss;
    double temp;
    int tempint;

//	std::cout << data[0] << std::endl;
    if (data.size()) {
        if (data[0] == "$GNGGA") {

            pthread_mutex_lock(&location_mutex_);

            // convert latitude
            if (!data[2].empty()) {
                iss.clear();
                iss.str(data[2]);
                iss >> temp;

                temp = LatitudeToValue(temp, data[3]);
//                    location_.SetLatitude(temp);
            }

            // convert longitude
            if (!data[4].empty()) {
                iss.clear();
                iss.str(data[4]);
                iss >> temp;

                temp = LongitudeToValue(temp, data[5]);
//                    location_.SetLongitude(temp);
            }

            // convert altitude
            if (!data[9].empty()) {
                iss.clear();
                iss.str(data[9]);
                iss >> tempint;
//                    location_.SetAltitude(tempint);
            }


            // convert satellites
            if (data[7].empty()) {
//                    location_.SetSatellites(0);
            } else {
                iss.clear();
                iss.str(data[7]);
                iss >> tempint;
//                    location_.SetSatellites(tempint);
            }

            if (is_positioning_) {
//                    GetGateway().SetDeviceLocation(&location_);
                // temp =
            }

            pthread_mutex_unlock(&location_mutex_);
        } 
		else if (data[0] == "$GNRMC") {
            if (data[2] == "A") {
                // convert latitude
                if (!data[3].empty()) {
                    iss.clear();
                    iss.str(data[3]);
                    iss >> temp;

                    temp = LatitudeToValue(temp, data[4]);
//                        location_.SetLatitude(temp);
                }

                // convert longitude
                if (!data[5].empty()) {
                    iss.clear();
                    iss.str(data[5]);
                    iss >> temp;

                    temp = LongitudeToValue(temp, data[6]);
//                        location_.SetLongitude(temp);
                }

//                    GetGateway().SetDeviceLocation(&location_);
                is_positioning_ = true;
                pthread_cond_signal(&positioning_cond_);
            } 
			else if (data[2] == "V") {
                is_positioning_ = false;
            }
        }
    }
}

void GPS::StringToArray(std::string &str, std::vector<std::string> &list) {
    std::istringstream iss(str);
    std::string buf;
    char ch;

    while ((iss >> ch) && !iss.eof()) {
        if (ch == ',') {
            list.push_back(buf);
            buf.clear();
        } else {
            buf.push_back(ch);
        }
    }

    list.push_back(buf);
}

inline double GPS::LongitudeToValue(double longitude, std::string &ori) {
    double temp;
    int tempint;

    temp = longitude / 100;
    tempint = (int)temp;
    temp = (double)tempint + (temp - tempint) * 10 / 6;

    return (ori == "W") ? -temp : ((ori == "E") ? temp : 0);
}

inline double GPS::LatitudeToValue(double latitude, std::string &ori) {
    double temp;
    int tempint;

    temp = latitude / 100;
    tempint = (int)temp;
    temp = (double)tempint + (temp - tempint) * 10 / 6;

    return (ori == "S") ? -temp : ((ori == "N") ? temp : 0);
}


bool GPS::WaitPositioningComplete(unsigned int time){
    struct timeval tv;
    struct timespec ts;

    /* Wait for update software complete */
    if (!is_positioning_) {
        gettimeofday(&tv, NULL);
        ts.tv_sec = tv.tv_sec + time;
        ts.tv_nsec = 0;

        pthread_mutex_lock(&positioning_mutex_);

        if (pthread_cond_timedwait(&positioning_cond_, &positioning_mutex_, &ts)) {
            pthread_mutex_unlock(&positioning_mutex_);
            std::cout << "WaitPositioningComplete timeout" << std::endl;
            return false;
        }

        pthread_mutex_unlock(&positioning_mutex_);
    }

    return true;
}

void *GPS::GPSReceiveThread(void *param) {
    GPS *device = reinterpret_cast<GPS*>(param);
    std::vector<std::string> list;
    std::string line;
    char ch;
    int ret;

    std::cout << "GPSReceiveThread++++++++++" << std::endl;

    while (1) {
        if ((ret = read(device->gps_dev_fd_, &ch, 1)) != -1) {
            if (ch == 0x0a || ch == 0x0d) {
                if (!line.empty()) {
                    device->StringToArray(line, list);
                    device->OnGPSReceived(list);
                    line.clear();
                    list.clear();
                }
            } else {
                line.push_back(ch);
            }
        }
    }

    std::cout << "GPSReceiveThread----------" << std::endl;

    return 0;
}
#if 0 
void *GPS::DataPointThread(void *param) {
    GPS *device = reinterpret_cast<GPS*>(param);
    std::ostringstream image_path;
    std::ostringstream image_oss_path;
    std::ostringstream image_name;
    //bool capture;
    struct tm *gmt;
    time_t t;

    std::cout << "WeatherStationM1::SensorThread++++++++++" << std::endl;

    if (device->is_run_) {
    /*
        capture = false;

        if (device->GetIlluminance() > 0) {
            image_name << device->GetDeviceID() << "_" << time(NULL) << ".jpg";
            image_path << "image/" << image_name.str();

            if (device->CaptureImage(image_path.str(), device->image_width_, device->image_height_)) {
                capture = true;
            } else {
                device->GetGateway().PostAlarm(1, "Camera", "Capture Image Failed");
            }
        }
    */
        /* Wait GPS Positioning */
        /*
        if (!device->WaitPositioningComplete(device->gps_timeout_)) {
            device->GetGateway().PostAlarm(1, "GPS", "Positioning Failed.");
            std::cout << "WeatherStationM1::DataPointThread WaitPositioningComplete failed" << std::endl;
        }
*/
        device->PostSensorDataPoint();

        /*
        if (capture) {
            time(&t);
            gmt = gmtime(&t);

            image_oss_path << "/" << (gmt->tm_year + 1900)
                            << "/" << (gmt->tm_mon + 1)
                            << "/" << gmt->tm_mday
                            << "/" << image_name.str();

            device->PostImageDataPoint(image_path.str(), image_oss_path.str());

            remove(image_path.str().c_str());
        }
        */
    }

    device->WaitUpdateSoftwareComplete(60);

    std::cout << "WeatherStationM1::SensorThread----------" << std::endl;

    /* Device sleep */
    device->Sleep(device->sleep_time_);

    return 0;

	
}
#endif

int main(int argc,char *argv[])
{
	GPS* gps = new GPS("/dev/ttyS0");

	if (!gps->WaitPositioningComplete(10)) 
	 {
//	 	device->GetGateway().PostAlarm(1, "GPS", "Positioning Failed.");
        std::cout << "DataPointThread WaitPositioningComplete failed" << std::endl;
     }
	
	sleep(10);
	return 0;
}



