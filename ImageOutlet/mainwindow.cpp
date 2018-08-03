/*
# ImageOutlet
# Copyright (C) 2018  Lusberg
#
# ImageOutlet is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# ImageOutlet is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtWidgets>
#include "mainwindow.h"
#include "scribblearea.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scribbleArea = new ScribbleArea;
    setCentralWidget(scribbleArea);
    createActions();
    createMenus();
    setWindowTitle(tr("ImageOutlet"));
    resize(500, 500);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event){
    if(maybeSave()){
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::open(){
    if(maybeSave()){
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
        if(!fileName.isEmpty()){
            scribbleArea->openImage(fileName);
        }
    }
}

void MainWindow::save(){
    QAction *action = qobject_cast<QAction *>(sender());
    QByteArray fileFormat = action->data().toByteArray();
    saveFile(fileFormat);
}

void MainWindow::penColor(){
    QColor newColor = QColorDialog::getColor(scribbleArea->penColor());
    if(newColor.isValid()){
        scribbleArea->setPenColor(newColor);
    }
}

void MainWindow::penWidth(){
    bool ok;
    int newWidth = QInputDialog::getInt(this, tr("ImageOutlet"), tr("Select pen Width: "), scribbleArea->penWidth(), 1, 50, 1, &ok);
    if(ok){
        scribbleArea->setPenWidth(newWidth);
    }
}

void MainWindow::about(){
    QMessageBox::about(this, tr("About ImageOutlet"), tr("<p><b>ImageOutlet</b>, a PhotoShop clone!</p> "
                                                         "Copyright 2018 Lusberg<br> "
                                                         "ImageOutlet is distributed under the terms of the GNU LGPL v3"));
}

void MainWindow::createActions(){
    openAction = new QAction(tr("&Open"), this);
    openAction->setShortcuts(QKeySequence::Open);
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
    foreach (QByteArray format, QImageWriter::supportedImageFormats()){
        QString text = tr("%1...").arg(QString(format).toUpper());
        QAction *action = new QAction(text, this);
        action->setData(format);
        connect(action, SIGNAL(triggered()), this, SLOT(save()));
        saveAsActions.append(action);
    }
    printAction = new QAction(tr("&Print..."), this);
    connect(printAction, SIGNAL(triggered()), scribbleArea, SLOT(print()));

    exitAction = new QAction(tr("&Exit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    penColorAction = new QAction(tr("&Pen Color..."), this);
    connect(penColorAction, SIGNAL(triggered()), this, SLOT(penColor()));

    penWidthAction = new QAction(tr("&Pen Width..."), this);
    connect(penWidthAction, SIGNAL(triggered()), this, SLOT(penWidth()));

    clearScreenAction = new QAction(tr("&Clear Screen..."), this);
    clearScreenAction->setShortcut(tr("Ctrl+L"));
    connect(clearScreenAction, SIGNAL(triggered()), scribbleArea, SLOT(clearImage()));

    aboutAction = new QAction(tr("&About..."), this);
    connect(aboutAction, SIGNAL(triggered()), SLOT(about()));

    aboutQtAction = new QAction(tr("&About Qt..."), this);
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus(){
    saveAsMenu = new QMenu(tr("&Save As"), this);
    foreach(QAction *action, saveAsActions)
            saveAsMenu->addAction(action);

    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAction);
    fileMenu->addMenu(saveAsMenu);
    fileMenu->addAction(printAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    optionMenu = new QMenu(tr("&Options"), this);
    optionMenu->addAction(penColorAction);
    optionMenu->addAction(penWidthAction);
    optionMenu->addSeparator();
    optionMenu->addAction(clearScreenAction);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(optionMenu);
    menuBar()->addMenu(helpMenu);
}

bool MainWindow::maybeSave() {
    if(scribbleArea->isModified()){
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("ImageOutlet"),
                tr("The image has been modified. \n"
                   "Do you want to save changes?"),
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if(ret == QMessageBox::Save){
            return saveFile("png");
        } else if (ret == QMessageBox::Cancel){
            return false;
        }
    }
    return true;
}

bool MainWindow::saveFile(const QByteArray &fileFormat){
    QString initialPath = QDir::currentPath() = "/untitled." + fileFormat;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    initialPath,
                                                    tr("%1 Files (*. %2);; All Files(*)")
                                                       .arg(QString::fromLatin1(fileFormat.toUpper()))
                                                       .arg(QString::fromLatin1(fileFormat)));
    if(fileName.isEmpty()){
        return false;
    } else {
        return scribbleArea->saveImage(fileName, fileFormat.constData());
    }
}
