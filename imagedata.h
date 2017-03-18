#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <QImage>

class ImageData
{
public:
    ImageData();
    ~ImageData();
    void AddGoordinate(bool longitudeValue,const char* goordinate, const char goordinateRef);
    void AddFileName(QString string);
    void AddThumbnail(QImage image);
    void AddAlttitude(const char* value);
    void ChangeFileNameExtension(const char* new_ext);
    QString* alttitude;
    QString* time_stamp;
    QString* latitude;
    QString* longitude;
    QString* file_name;
    QString* full_path;
    QString* thumbnail_path;
private:
};

#endif // IMAGEDATA_H
