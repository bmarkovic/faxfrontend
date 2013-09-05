/***************************************************************************
$Id: rolodex.cc,v 1.2 2004/04/04 19:36:59 glenn Exp $

                          rolodex.cc  -  Rolodex for faxfrontend
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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include "rolodex.h"
#include "RolodexParser.h"

static const char rolodexFile[]="/.faxrolodex";

Rolodex::Rolodex( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : RolodexDsgn( parent, name, modal, fl )
{
    /* Read the rolodex info; load the listview.	*/
    const char *p = getenv("HOME");
    if (!p) p = ".";
    
    QFile xmlFile( QString(p) + rolodexFile);
    QXmlInputSource source( xmlFile );
    QXmlSimpleReader reader;
    RolodexParser handler;

    connect(&handler, SIGNAL(entry(const QString&,const QString&,const QString&,const QString&)),
	    this, SLOT(insertItem(const QString&,const QString&,const QString&,const QString&)));
    
    reader.setContentHandler( &handler );
    reader.parse( source );
    xmlFile.close();
    
    ListView1->sort();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
Rolodex::~Rolodex()
{
    // Write out contents to file
    const char *p = getenv("HOME");
    if (!p) p = ".";
    QString fn(p);
    fn += rolodexFile;
    
    FILE *fp = fopen(fn.latin1(), "w");
    if (!fp) {
	QMessageBox::critical(0, tr("File Error"),
			      tr("Can't update rolodex file %1:%2")
			      .arg(fn).arg(strerror(errno)));
	return;
    }
    
    RolodexParser::writeStart(fp);
    QListViewItem* item = ListView1->firstChild();
    while (item) {
	RolodexParser::writeEntry(fp, item->text(0), item->text(1),
				  item->text(2), item->text(3));
	item = item->nextSibling();
    }
    
    RolodexParser::writeEnd(fp);
    fclose(fp);	
}

// Called when the parser completes reading an entry.
void Rolodex::insertItem(const QString& recipient, const QString& company,
			 const QString& location, const QString& faxnumber)
{
    // check for duplicate recipient
    QString rcp(recipient.stripWhiteSpace());
    QListViewItem* item = ListView1->firstChild();
    while (item) {
	if (item->text(0) == rcp) {
	    int s = QMessageBox::warning(0, 
				 tr("Duplicate Rolodex Entry Found"),
				 tr("Overwrite duplicate Rolodex entry\n"
				    "for %1").arg(rcp),
					 QMessageBox::Yes|QMessageBox::Default,
					 QMessageBox::No);

	    if (s == QMessageBox::Yes)
		break;
	    else
		return;
	}
	item = item->nextSibling();
    }	
    
    if (item) {
	// overwrite existing entry
	item->setText(1, company.stripWhiteSpace());
	item->setText(2, location.stripWhiteSpace());
	item->setText(3, faxnumber.stripWhiteSpace());
    } else
	item = new QListViewItem(ListView1, rcp, company.stripWhiteSpace(),
				 location.stripWhiteSpace(),
				 faxnumber.stripWhiteSpace());
}

/*
 * public slot
 */
void Rolodex::deleteItem()
{
    QListViewItem* item = ListView1->currentItem();
    if (!item) {
	QMessageBox::critical(this, tr("Error"),
			      tr("Must select an entry first!"));
	return;
    }
    
    delete item;
}

/* User has double clicked item; use it to load the cover sheet
 */
void Rolodex::doubleClickItem(QListViewItem* item)
{
    emit loadCover(item->text(0), item->text(1), item->text(2), item->text(3));
    QDialog::accept();
}

void Rolodex::accept()
{
    QListViewItem* item = ListView1->currentItem();
    if (!item) {
	int s = QMessageBox::warning(this, tr("No entry selected"),
			     tr("No entry selected to load cover sheet.\n"
				"Are you sure you want to quit?"),
				     QMessageBox::Yes|QMessageBox::Default,
				     QMessageBox::No);
	if (s == QMessageBox::Yes) return;
    }
    emit loadCover(item->text(0), item->text(1), item->text(2), item->text(3));	
    
    QDialog::accept();
}
