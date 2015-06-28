#include "fileparser.h"
#include <QDir>
#include <QDirIterator>
#include "imagedata.h"
#include "FreeImagePlus.h"
#include <QThread>

FileParser::FileParser()
{
    path = QString("./");
    thread = new QThread();
    connect(thread,SIGNAL(started()),this,SLOT(SearchFiles()));
    connect(thread,SIGNAL(finished()),this,SLOT(Complete()));
}

FileParser::~FileParser()
{
    while (ImageTable.size())
    {
        delete ImageTable.at(0);
        ImageTable.removeAt(0);
    }
    listOfFiles.clear();
    disconnect(thread,SIGNAL(started()),this,SLOT(SearchFiles()));
    disconnect(thread,SIGNAL(finished()),this,SLOT(Complete()));
    delete thread;
}

FileParser::FileParser(QString start_path)
{
    path = start_path;
    thread = new QThread();
    connect(thread,SIGNAL(started()),this,SLOT(SearchFiles()));
    connect(thread,SIGNAL(finished()),this,SLOT(Complete()));
}

void FileParser::FindFilesWithExtension(QString ext)
{
    QFileInfo fileInfo(path);
    extension = ext;
    if (fileInfo.exists() && fileInfo.isDir())
    {
        /* Move Object to Thread and start search image data */
        this->moveToThread(thread);
        thread->start();
    }
}

ImageData* FileParser::GetImageData(int index)
{
    return ImageTable.value(index);
}

void FileParser::SearchFiles()
{
    QDir directoryToStart(path);
    FindFilesFromDir(directoryToStart);
    emit Started(FileAmount());
    GetGPSData();
}

void FileParser::Complete()
{
    emit AllDone();
}

int FileParser::FileAmount()
{
    return listOfFiles.count();
}

QString FileParser::GetFileByIndex(int index)
{
    return listOfFiles.value(index);
}

void FileParser::FindFilesFromDir(QDir dir)
{
   QDirIterator iterator(dir.absolutePath(), QDirIterator::Subdirectories);
   int index = 0;
   QString tmpString = extension;
   while (iterator.hasNext()) {
      iterator.next();
      if (!iterator.fileInfo().isDir()) {
         QString filename = iterator.fileName();
         if (filename.endsWith(extension) || filename.endsWith(extension.toUpper()))
         {
            listOfFiles << iterator.filePath();
         }
      }
   }

}

void FileParser::GetGPSData()
{
    fipTag tag;
    fipTag tag2;
    fipImage src;
    QString file;
    const char* value;
    const char* thumbnail_tag = "thumbnail_";
    char value2 = '\n';
    int amount = this->FileAmount();
    int index = 0;
    ImageData* image;

    while (index < amount)
    {
        file = this->GetFileByIndex(index);
        src.load(file.toStdString().c_str());

        image = new ImageData();
        image->AddFileName(file);

        if(src.getMetadata(FIMD_EXIF_GPS,"GPSLatitude",tag) && src.getMetadata(FIMD_EXIF_GPS,"GPSLatitudeRef",tag2))
        {
            value2 = *(tag2.toString(FIMD_EXIF_GPS));
            value = tag.toString(FIMD_EXIF_GPS);
            image->AddGoordinate(false,value,value2);
        }
        if(src.getMetadata(FIMD_EXIF_GPS,"GPSLongitude",tag) && src.getMetadata(FIMD_EXIF_GPS,"GPSLongitudeRef",tag2))
        {
            value2 = *(tag2.toString(FIMD_EXIF_GPS));
            value = tag.toString(FIMD_EXIF_GPS);
            image->AddGoordinate(true,value,value2);
        }
        if(src.getMetadata(FIMD_EXIF_GPS,"GPSAltitude",tag))
        {
            value = tag.toString(FIMD_EXIF_GPS);
            image->AddAlttitude(value);
        }

        if(src.getMetadata(FIMD_EXIF_EXIF,"DateTimeOriginal",tag))
        {
            value = tag.toString(FIMD_EXIF_EXIF);
            image->time_stamp = new QString(value);
        }

        /* Make Thumbnail from original image and store it to execution folder using PNG Format */
        /* TODO: Investigate why icon data is not show in map
        if (src.makeThumbnail(50))
        {
            int size = strlen(thumbnail_tag);
            char* filename = new char[(size + image->file_name->length() + 1)];
            strcpy(filename,thumbnail_tag);
            strcpy((filename+size),image->file_name->toStdString().c_str());
            size = (strlen(filename) - 4);
            strcpy((filename+size),".png");
            if (FreeImage_Save(FIF_PNG, src, filename, 0))
                image->thumbnail_path = new QString(filename);
            delete[] filename;
        }*/
        ImageTable << image;
        emit FileDone(index);
        index++;
     }
    thread->quit();
}
