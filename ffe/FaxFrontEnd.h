/***************************************************************************
$Id: FaxFrontEnd.h,v 1.2 2004/04/04 19:36:59 glenn Exp $

                          FaxFrontEnd.h - Front End for Hylafax
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

#ifndef FAXFRONTEND_H
#define FAXFRONTEND_H
#include "FaxFrontEndDsgn.h"

class Rolodex;

class FaxFrontEnd : public FaxFrontEndDsgn
{ 
    Q_OBJECT

public:
    FaxFrontEnd( bool haveFile, QWidget* parent = 0,
		 const char* name = 0, bool modal = true, WFlags fl = 0 );
    ~FaxFrontEnd();

public slots:
    void addToRolodex();
    void showAbout();
    void showRolodex();
    void loadCover(const QString& recipient, const QString& company,
		   const QString& location, const QString& faxnumber);
    void accept();
    void browseFile();

private:
    Rolodex *rolodex;
};

#endif // FAXFRONTEND_H
