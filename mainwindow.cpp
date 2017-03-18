#include <QWebFrame>
#include <QImage>
#include <QFile>
#include <QString>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imagedata.h"
#include "fileparser.h"
#ifdef QT_DEBUG
#include "QDebug"
#endif
#include <QObject>
#include <QMessageBox>
#include <QThread>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->webView, SIGNAL(loadProgress(int)), ui->progressBar, SLOT(setValue(int)));
    connect(ui->webView, SIGNAL(loadStarted()), ui->progressBar, SLOT(show()));
    connect(ui->webView, SIGNAL(loadFinished(bool)), ui->progressBar, SLOT(hide()));

    /* Load Super simple webpage with Java Scirpt */
    QFile file("index.html");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString html = QTextStream(&file).readAll();
        ui->webView->setHtml(html);
    }
    parser = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::RemoveMarkers()
{
    QStringList code;
    code << "{";
    code << "deleteMarkers();";
    code << "}";

    ui->webView->page()->mainFrame()->evaluateJavaScript(code.join("\n"));
}

void MainWindow::AddNewMarker(const ImageData& data)
{
    QStringList code;
    QString OneLine = "";

    if (data.longitude && data.latitude)
    {
        code << "{";
        OneLine = "var description = '" + *(data.file_name);
        if (data.alttitude)
            OneLine += " Height: " + *(data.alttitude);
        if (data.time_stamp)
            OneLine += " Timestamp:" + *(data.time_stamp);
        OneLine += "';";
        code << OneLine;
        OneLine = "var myLatlng = new google.maps.LatLng(" + *(data.latitude) + ", " + *(data.longitude) + ");";
        code << OneLine;
        code << "addMarker(myLatlng, description);";
        code << "}";

        ui->webView->page()->mainFrame()->evaluateJavaScript(code.join("\n"));
    }
#ifdef QT_DEBUG
    else
    {
        qDebug() << *(data.file_name) << " Don't have location info!!";
    }
#endif
}

void MainWindow::on_pushButton_clicked()
{
    if (!parser)
    {
        QString path = ui->lineEdit->text();

        /* Remove possible previous markers */
        RemoveMarkers();
        parser = new FileParser(path);
        connect(parser,SIGNAL(FileDone(int)),this,SLOT(FileInfoComplete(int)));
        connect(parser,SIGNAL(AllDone()),this,SLOT(CloseParser()));
        connect(parser,SIGNAL(Started(int)),this,SLOT(ParserStarted(int)));


        parser->FindFilesWithExtension(".jpg");
    }
    else
    {
    QMessageBox::information(
        this,
        tr("Image on Map"),
        tr("Please Wait that previous operation stops") );
    }
}

void MainWindow::ParserStarted(int total_files)
{
    total_files_found = total_files;
    files_done = 0;
    ui->progressBar->setValue(0);
    ui->progressBar->setRange(0,total_files_found);
    ui->progressBar->setFormat("%v/%m");
    ui->progressBar->setVisible(true);
}

void MainWindow::FileInfoComplete(int value)
{
    const ImageData& image = parser->GetImageData(value);
    AddNewMarker(image);
    /* Update progress */
    files_done +=1;
    ui->progressBar->setValue(files_done);
}

void MainWindow::CloseParser()
{
    disconnect(parser,SIGNAL(FileDone(int)),this,SLOT(FileInfoComplete(int)));
    disconnect(parser,SIGNAL(AllDone()),this,SLOT(CloseParser()));
    disconnect(parser,SIGNAL(Started(int)),this,SLOT(ParserStarted(int)));
    delete parser;
    parser = NULL;

}
