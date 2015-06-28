#include "imagedata.h"
#include <QString>
#include <QDir>
#ifdef QT_DEBUG
#include <QDebug>
#endif
#include <sstream>

ImageData::ImageData()
{
    latitude = NULL;
    longitude = NULL;
    file_name = NULL;
    thumbnail_path = NULL;
    full_path = NULL;
    time_stamp = NULL;
    alttitude = NULL;
}

ImageData::~ImageData()
{
    delete latitude;
    delete longitude;
    delete file_name;
    delete thumbnail_path;
    delete full_path;
    delete time_stamp;
    delete alttitude;
}

void ImageData::AddGoordinate(bool longitudeValue, const char *goordinate, const char goordinateRef)
{
    int index               = 0;
    double minutes           = 0;
    double hours             = 0;
    double seconds           = 0;
    double seconds2          = 0;
    double decimal_value     = 0;
    QString* tmp_str;
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
    tmp_str = new QString(o.str().c_str());
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
    file_name = new QString(fileInfo.fileName());
}

void ImageData::AddThumbnail(QImage image)
{

}

void ImageData::AddAlttitude(const char *value)
{

    /* Example format from 1056,3m high "10563000/10000"
     * or "4867000/1000" -> 4867.000m */
    int index = 0;  // Calculate decimal values
    int index2 = 0; // Calculate divider size
    int place_of_dot;
    char* tmp_data;
    while (value[index] != '/')
        index++;
    while (value[(index+index2)] != '\0')
    {
       index2++;
    }
    /* Create new string in decimal format */
    tmp_data = strdup(value);
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

    alttitude = new QString(tmp_data);
    delete tmp_data;
}

void ImageData::ChangeFileNameExtension(const char* new_ext)
{
    int extension_index = file_name->lastIndexOf('.');
    int cut_amount = file_name->length()-extension_index;
    file_name->insert(extension_index,new_ext);
    file_name->chop(cut_amount);
}
