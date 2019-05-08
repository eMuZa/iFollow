#include "getGPS.hpp"
#include <iostream>

//SQL Includes
#include "mySQLGPS.hpp"



#define DBHOST "tcp://10.192.139.5:3306"
#define USER "Jesus"
#define PASSWORD "pi"
#define DATABASE "DatabaseGPS"



int main(void)
{
    GPS myGPS("/dev/ttyAMA0");

    //SQL Declerations
    mySQLGPS sqlGPS(DBHOST, USER, PASSWORD, DATABASE);

    /*Updating Coordinates */
    myGPS.updateCoordinates();

    /* Making QUERY String */
    char executeQueryChar[300];
    int n = sprintf(executeQueryChar,"INSERT INTO `GPSData`(`nmeaType`, `fixTime`, `Latitude`, `Longitude`, `fixQuality`, `numOfSats`, `horizDilofPos`, `Altitude`, `heightofGeoID`, `checkSum`) VALUES (`%s`,`%s`,`%s`,`%s`,`%s`,`%s`,`%s`,`%s`,`%s`,`%s`);",
        myGPS.getGpsType().c_str(),
        myGPS.getfixTime().c_str(),
        myGPS.getLatitude().c_str(),
        myGPS.getLongitude().c_str(),
        myGPS.getfixQuality().c_str(),
        myGPS.getnumOfSats().c_str(),
        myGPS.gethDilutionofPos().c_str(),
        myGPS.getaltitudeMeters().c_str(),
        myGPS.gethOgGeoid().c_str(),
        myGPS.getcheckSum().c_str()
        );
    
    /* Connecting to DATABASE */
    sqlGPS.mysql_connect();

    /* Sending QUERY */
    sqlGPS.mysql_sendQUERY(executeQueryChar);

    /* Closing Connecting to SQL Server */
    sqlGPS.mysql_disconnect();

    return 0;
}
