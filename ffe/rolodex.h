/***************************************************************************
$Id: rolodex.h,v 1.2 2004/04/04 19:36:59 glenn Exp $

                          rolodex.h  -  Rolodex for faxfrontend
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
#ifndef ROLODEX_H
#define ROLODEX_H
#include "rolodexDsgn.h"

class Rolodex : public RolodexDsgn
{ 
    Q_OBJECT

public:
    Rolodex( QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0 );
    ~Rolodex();
   	void insert(QString&, QString&, QString&);

signals:
    void loadCover(const QString&, const QString&, const QString&, const
									 QString&);
public slots:
    void deleteItem();
    void doubleClickItem(QListViewItem*);
    void insertItem(const QString&, const QString&, const QString&, const QString&);
    void accept();
};

#endif // ROLODEX_H
