#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "FreeImagePlus.h"
#include "imagedata.h"
#include "fileparser.h"
#include <QThread>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void RemoveMarkers();
private slots:
    void on_pushButton_clicked();
public slots:
    void ParserStarted(int total_files);
    void FileInfoComplete(int value);
    void CloseParser();
private:
    int total_files_found;
    int files_done;
    QThread* webFrameThread;
    void AddNewMarker(const ImageData& data);
    void ParseGPSDataFromImage(QString file);
    FileParser* parser;
    Ui::MainWindow *ui;


};

#endif // MAINWINDOW_H
