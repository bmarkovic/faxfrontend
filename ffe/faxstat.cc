/***************************************************************************
$Id: faxstat.cc,v 1.4 2004/04/04 19:36:59 glenn Exp $

                          faxstat.cc - Fax Front End for Hylafax
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
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <qmessagebox.h>
#include <qmultilineedit.h>
#include "faxstat.h"

/* 
   Poll for status with 'faxstat', and display results.
 */
FaxStat::FaxStat( )
    : FaxStatDsgn( 0, "faxstat", true )
{
    startTimer(2000);
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FaxStat::~FaxStat()
{
    // no need to delete child widgets, Qt does it all for us
}

void FaxStat::timerEvent(QTimerEvent*)
{
    char tmpname[] = "/tmp/FaxStatXXXXXX";
    char cmd[100], *p;
    FILE *fp;
    time_t now;

    mktemp(tmpname);

    // Execute 'faxstat', and dump the output to a temporary file
    sprintf(cmd, "faxstat -s -l > %s 2>&1 ", tmpname);
    system(cmd);
    fp = fopen(tmpname, "r");
    if (fp < 0) {
	QMessageBox::critical(0, tr("Error"),
			      tr("Can't open tmp file %1\n%2")
			      .arg(tmpname).arg(strerror(errno)));
	return;
    }

    MultiLineEdit->clear();
    time(&now);
    MultiLineEdit->insertLine(ctime(&now));

    while (fgets(cmd, sizeof(cmd), fp)) {
	p = &cmd[strlen(cmd) - 1];
	while (p > cmd && isspace(*p)) { *p-- = 0; }
	MultiLineEdit->insertLine(cmd);
    }

    fclose(fp);
    unlink(tmpname);
}
