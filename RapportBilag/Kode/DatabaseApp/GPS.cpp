#include "GPS.hpp"

GPS::GPS(void)
{
    fileDescrip_ = "/dev/ttyAMA0";
}


GPS::GPS(std::string str)
{
    fileDescrip_ = str;
}

GPS::~GPS(void)
{
}

void GPS::updateCoordinates(void)
{
    numOfComma = 0; // Keeping track of Comma length
    char bufferChar_[75]; // buffer for string
    size_t iFor = 0; // Times gone through case
    std::string fileDescrip_ ="/dev/ttyAMA0"; // fileDescripter */ TEST FÆRDIG MED BRUG AF KLASSE FILEdeCTOÅ

    /* Opening UART File to read */
    int dev = open(fileDescrip_.c_str(), O_RDWR | O_NOCTTY | O_SYNC);


    /* Error Check for if file has opened */
    if (dev < 0)
    {
        std::cout << "could not open file. ERROR: " << dev << std::endl;
    }
        std::cout << "file opened succesfully" << std::endl;

    /* Reading from UART file */
    int n = read(dev,bufferChar_,sizeof(bufferChar_));

    bufferString_ = std::string(bufferChar_);
    /* Keep reading until correct NMEA Type */
    while((bufferString_.substr(0,6) != "$GPGGA") || (n < 70))
            {
                n = read(dev,bufferChar_,sizeof(bufferChar_)); // Reading
                bufferString_ = std::string(bufferChar_);
            }

    bufferString_ = bufferString_.substr(0,72);
    std::cout << bufferString_ << std::endl;


    /* Closing device UART file */
    close (dev);

    /* Splitting data into types */
while(bufferString_.substr(0,1) != "\0")
                {
                    char nmeaCharBuffer_[30];
                    std::string caseBuffer_;
                    std::string nmeaStringBuffer_;
                    int difInPos;
                    int posOfComma2;
                    int posOfComma1 = 0;
                    std::size_t length;
                    
                    /* Handling Data Types consisting of comma */
                    if (numOfComma == 2 || numOfComma == 3 || numOfComma == 7 || numOfComma == 8)
                    {
                        posOfComma2 = bufferString_.find(",",posOfComma1+1);
                        numOfComma++;

                        difInPos = posOfComma2-posOfComma1;

                        length = bufferString_.copy(nmeaCharBuffer_,(difInPos+1),posOfComma1+1);
                        nmeaCharBuffer_[length] = '\0';


                        nmeaStringBuffer_ = nmeaCharBuffer_;

                        posOfComma1 = posOfComma1+2;
                        posOfComma2 = posOfComma2+2;
                    }
                    else
                    {
                        /*Handling default data types */
                        posOfComma2 = bufferString_.find(",",posOfComma1+1);

                        numOfComma++;
                        difInPos = posOfComma2-posOfComma1;

                        length = bufferString_.copy(nmeaCharBuffer_,difInPos-1,posOfComma1+1);
                        nmeaCharBuffer_[length] = '\0';

                        nmeaStringBuffer_ = nmeaCharBuffer_;
                    }


                        caseBuffer_ = nmeaStringBuffer_;
                        /* Switching for putting data into correct buffer */
                        switch (iFor)
                        {
                            case 0:
                                setGpsType(caseBuffer_);
                                bufferString_.replace(0,length+1,"");
                                break;
                            case 1:
                                setfixTime(caseBuffer_);
                                bufferString_.replace(0,length+1,"");
                                break;
                            case 2:
                                setLatitude(caseBuffer_);
                                bufferString_.replace(0,length+1,"");
                                break;
                            case 3:
                                setLongitude(caseBuffer_);
                                bufferString_.replace(0,length+1,"");
                                break;
                            case 4:
                                setfixQuality(caseBuffer_);
                                bufferString_.replace(0,length+1,"");
                                break;
                            case 5:
                                setnumofSats(caseBuffer_);
                                bufferString_.replace(0,length+1,"");
                                break;
                            case 6:
                                sethDilutionofPos(caseBuffer_);
                                bufferString_.replace(0,length+1,"");
                                break;
                            case 7:
                                setaltitudeMeters(caseBuffer_);
                                bufferString_.replace(0,length+1,"");
                                break;
                            case 8:
                                sethOgGeoid(caseBuffer_);
                                bufferString_.replace(0,length+1,"");
                                break;
                            case 9:
                                bufferString_.replace(0,length+1,"");
                                break;
                            case 10:
                                setcheckSum(caseBuffer_);
                                bufferString_.replace(0,length+1,"");
                                break;

                        }
                    iFor++;
                }

}
