#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <QString>
#include <QObject>
#include <QDir>
#include <QThread>
#include <QObject>
#include "imagedata.h"


class FileParser : public QObject
{
    Q_OBJECT
public:
    FileParser();
    ~FileParser();
    FileParser(QString start_path);
    void FindFilesWithExtension(QString ext);
    ImageData *GetImageData(int index);
private:
    void FindFilesFromDir(QDir dir);
    void GetGPSData();
    int FileAmount();
    QString GetFileByIndex(int index);
    QString path;
    QString extension;
    QStringList listOfFiles;
    QThread* thread;
    QList <ImageData*> ImageTable;
public slots:
    void SearchFiles();
    void Complete();
signals:
    void Started(int total_files);
    void FileDone(int id);
    void AllDone();
};

#endif // FILEPARSER_H
