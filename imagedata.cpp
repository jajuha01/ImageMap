#include "imagedata.h"
#include <QString>
#include <QDir>
#ifdef QT_DEBUG
#include <QDebug>
#endif
#include <sstream>

ImageData::ImageData()
{
}

ImageData::~ImageData()
{
}

void ImageData::AddGoordinate(bool longitudeValue, const char *goordinate, const char goordinateRef)
{
    int index               = 0;
    double minutes           = 0;
    double hours             = 0;
    double seconds           = 0;
    double seconds2          = 0;
    double decimal_value     = 0;
    QString tmp_str;
    std::ostringstream o;

    /* Format is FreeImage Library specific change only in case library changes
       Remove unwanted : separators and create string in decimal format */
    /*"Goordinate xxx:xx:xx,xx" GoordinateRef 'N'/'S' or 'E'/'W'*/
    while (goordinate[index] != ':')
    {

       hours += (goordinate[index]-0x30);
       if (goordinate[index+1] != ':')
       {
           hours *= 10;
       }
       index++;
    }
    index++;
    while (goordinate[index] != ':')
    {

       minutes += (goordinate[index]-0x30);
       if (goordinate[index+1] != ':')
       {
           minutes *= 10;
       }
       index++;
    }
    index++;
    /* Check seconds */
    while (goordinate[index] != ',' && goordinate[index] != '\0')
    {
        seconds2 += (goordinate[index]-0x30);
        if (goordinate[index+1] != ',' && goordinate[index+1] != '\0')
        {
            seconds2 *= 10;
        }
        index++;
    }
    /* Search end and start from there */
    while (goordinate[index] != '\0')
        index++;
    index--;
    /* Check second decimals first*/
    while (goordinate[index] != ',' && goordinate[index] != ':')
    {
       seconds += (goordinate[index]-0x30);
       seconds /= 10;
       index--;
    }
    seconds += seconds2;
    decimal_value = seconds/3600;
    decimal_value += minutes/60;
    decimal_value += hours;

    /* Check do we need negative value based of latitude longitude reference value */
    if (longitudeValue)
    {
        if (goordinateRef == 'W')
        {
            decimal_value *=-1;
        }
    }
    else
    {
        if (goordinateRef == 'S')
        {
            decimal_value *=-1;
        }
    }
    o.precision(12);
    o << decimal_value;
    tmp_str = QString(o.str().c_str());
    if (longitudeValue)
    {
        longitude = tmp_str;
    }
    else
    {
        latitude = tmp_str;
    }
#ifdef QT_DEBUG
    qDebug() << "Original data:" << goordinate << goordinateRef;
    qDebug() << "Conver data:" << *tmp_str;
#endif
}

void ImageData::AddFileName(QString string)
{
    QFileInfo fileInfo(string);
    file_name = QString(fileInfo.fileName());
}

void ImageData::AddThumbnail(QImage image)
{

}

void ImageData::AddAlttitude(const char *value)
{

    /* Example format from 1056,3m high "10563000/10000" or "10000/10563000"
     * "4867000/1000" or "1000/4867000" -> 4867.000m */
    int index = 0;  // Calculate decimal values
    int index2 = 0; // Calculate divider size
    int place_of_dot;
    char tmp_data[256] = {0};

    while (value[index] != '/')
        index++;
    while (value[(index+index2)] != '\0')
    {
       index2++;
    }
#if 0 // Todo how to detect and handle reverse formats
    /* Check is the format in "10000/10563000" format */
    if (index > 2 && *value == '1')
    {
        i++;
        while (value[i] == '0')
        {
            i++;
        }
        if (value[i] == '/')
        {
            reverseFormat = true;
        }
    }
    /* Check format and made new string in default format ("10563000/10000") */
    if (reverseFormat)
    {
        /* create string in reverse format */
        strcpy(tmp_data,value);
        i = 1;
        // copy real values
        while (i < index2)
        {
            tmp_data[i-1] = value[index+i];
            i++;
        }
        tmp_data[i-1] = '/';
        // copy decimal place values
        i = 0;
        while (value[i] != '/')
        {
            tmp_data[index2+i] = value[i];
            i++;
        }
        // ReCalculate index values
        index=0;
        index2=0;
        while (tmp_data[index] != '/')
            index++;
        while (tmp_data[(index+index2)] != '\0')
        {
           index2++;
        }
        value = tmp_data;

    }
    else
#endif
    {
        strcpy(tmp_data,value);
    }
    /* Create new string in decimal format */
    if (index > (index2-2))
    {
        place_of_dot = (index-(index2-2));
        tmp_data[index+1] = 'm';
        tmp_data[index+2] = '\0';
        tmp_data[place_of_dot] = '.';
        strncpy(tmp_data+(place_of_dot+1),value+place_of_dot,index-place_of_dot);
    }
    else if (index == (index2-2))
    {
        tmp_data[0] = '0';
        tmp_data[1] = '.';
        tmp_data[index+2] = 'm';
        tmp_data[index+3] = '\0';
        strncpy(tmp_data+2,value,index);
    }
    else
    {
        tmp_data[index] = 'm';
        tmp_data[index+1] = '\0';
    }

    alttitude = QString(tmp_data);
}

void ImageData::ChangeFileNameExtension(const char* new_ext)
{
    int extension_index = file_name.lastIndexOf('.');
    int cut_amount = file_name.length()-extension_index;
    file_name.insert(extension_index,new_ext);
    file_name.chop(cut_amount);
}
