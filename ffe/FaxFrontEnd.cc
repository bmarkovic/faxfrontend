/***************************************************************************
$Id: FaxFrontEnd.cc,v 1.2 2004/04/04 19:36:59 glenn Exp $

                          FaxFrontEnd.cc - Fax Front End for Hylafax
                             -------------------
    begin                : Fri Apr 18 2003
    copyright            : (C) 2003 by Glenn Burkhardt
    email                : gbburkhardt@verizon.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qlineedit.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include "../config.h"
#include "FaxFrontEnd.h"
#include "rolodex.h"

/* defaults file tags. */
static const char notify_complete[]="NotifyComplete";
static const char notify_retry[]="NotifyRetry";
static const char FFresolution[]="Resolution";
static const char page_size[]="PageSize";
static const char defaults_file[]="/.faxfrontendrc";

FaxFrontEnd::FaxFrontEnd( bool haveFile, QWidget* parent,
			  const char* name, bool modal, WFlags fl )
    : FaxFrontEndDsgn( parent, name, modal, fl )
{
    rolodex = new Rolodex(this);
    connect(rolodex, SIGNAL(loadCover(const QString&, const QString&, const QString&, const QString&)),
	    this, SLOT(loadCover(const QString&, const QString&, const QString&, const QString&)));

    resButtonGroup->setButton(0);

    /* Read defaults file */
    const char *p = getenv("HOME");
    if (!p) p = ".";
    QString dfn(p);
    dfn += defaults_file;
    
    QFile df(dfn);
    if (!df.exists()) return;
    df.open(IO_ReadOnly);
    
    QString line;
    int n;
    
    while (df.readLine(line, 64) > 0) {
	n = line.find('=');
	if (n > 0) {
	    if (line.left(n) == notify_complete) {
		notifyComplete->setChecked(line.right(line.length()-n-1)
					   .stripWhiteSpace() == "true");
	    } else if (line.left(n) == notify_retry)
		notifyRetry->setChecked(line.right(line.length()-n-1)
					.stripWhiteSpace() == "true");
	    else if (line.left(n) == FFresolution)
		resButtonGroup->setButton(line.right(line.length()-n-1).toInt());
	    else if (line.left(n) == page_size)
		pageSizeComboBox->setCurrentItem(line.right(line.length()-n-1)
						 .toInt());
	}
    }
    df.close();

    if (haveFile) {
	fileLabel->hide();
	fileLineEdit->hide();
	browseButton->hide();
    }
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FaxFrontEnd::~FaxFrontEnd()
{
    /* Write out defaults */
    const char *p = getenv("HOME");
    if (!p) p = ".";
    QString dfn(p);
    dfn += defaults_file;
    FILE *fp = fopen(dfn.latin1(), "w");
    if (!fp) {
	QMessageBox::critical(0, tr("File Open Error"),
			      tr("Can't open %1 for update: %2")
			      .arg(dfn).arg(strerror(errno)));
	return;
    }
    fprintf(fp, "%s=%s\n", notify_complete, notifyComplete->isChecked()?"true":"false");
    fprintf(fp, "%s=%s\n", notify_retry, notifyRetry->isChecked()?"true":"false");
    
    int bID;
    QButton* b = resButtonGroup->selected();
    if (b)
  	bID = resButtonGroup->id(b);
    else
  	bID = 0;
    fprintf(fp, "%s=%d\n", FFresolution, bID);
    fprintf(fp, "%s=%d\n", page_size, pageSizeComboBox->currentItem());                                                                       ;
    fclose(fp);
}

/* 
 * public slot
 */
void FaxFrontEnd::addToRolodex()
{
    if (recipientBox->text().isEmpty() || faxNumberBox->text().isEmpty()) {
	QMessageBox::critical(this, tr("Error"),
			      tr("Must enter both Fax Number\n"
				 "and Recipient"));
	return;
    }
    
    rolodex->insertItem(recipientBox->text(), companyBox->text(),
			locationBox->text(), faxNumberBox->text());
}

void FaxFrontEnd::accept()
{
    if (recipientBox->text().isEmpty() || faxNumberBox->text().isEmpty()) {
	QMessageBox::critical(this, tr("Error"),
			      tr("Must enter both Fax Number\n"
				 "and Recipient"));
	return;
    }
    
    QDialog::accept();
}
/* 
 * public slot
 */
void FaxFrontEnd::showAbout()
{
    QMessageBox::information(this, tr("About FaxFrontEnd"),
			     tr("FaxFrontEnd for Hylafax\n"
				"Version %1\n"
				"Copyright (c) Glenn Burkhardt 2004\n",
				"GNU General Public License").arg(VERSION));
}

/* User has clicked 'load cover in the rolodex dialog: do it!
 */
void FaxFrontEnd::loadCover(const QString& recipient, const QString& company,
			    const QString& location, const QString& faxnumber)
{
    recipientBox->setText(recipient);
    companyBox->setText(company);
    locationBox->setText(location);
    faxNumberBox->setText(faxnumber);
}

/* 
 * public slot
 */
void FaxFrontEnd::showRolodex()
{
    setCursor(Qt::waitCursor);
    rolodex->exec();
    setCursor(Qt::arrowCursor);
}

void FaxFrontEnd::browseFile()
{
    QString home(getenv("HOME"));
    fileLineEdit->setText(QFileDialog::getOpenFileName(home,
		  "Postscript (*.ps);;Text Files (*.txt);;All Files (*.*)"));
}
